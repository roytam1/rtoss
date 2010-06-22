/*-------------------------------------------
  pagesntp.c
      "Syncronization" setting page
      KAZUBON 1998-1999
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnDestroy(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnNTPServer(HWND hDlg);
static void OnDelServer(HWND hDlg);
static void OnSyncTimer(HWND hDlg);
static void OnSyncADay(HWND hDlg);
static void OnSanshoAlarm(HWND hDlg, WORD id);
static void OnSyncNow(HWND hDlg);
static void CreateTip(HWND hwnd);
static void LoadLogFile();

HICON hIconSync = NULL;
static HWND hwndTip;
static BOOL bNoRasAPI = FALSE;

// sntp.c
extern HWND hwndSNTP;
extern HWND hwndSNTPLog;

#define SendPSChanged(hDlg) SendMessage(GetParent(hDlg),PSM_CHANGED,(WPARAM)(hDlg),0)

/*------------------------------------------------
  Dialog procedure
--------------------------------------------------*/
BOOL CALLBACK PageSNTPProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			OnInit(hDlg);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
			case IDC_NTPSERVER:
				if(code == CBN_EDITCHANGE)
					OnNTPServer(hDlg);
				if(code == CBN_EDITCHANGE || code == CBN_SELCHANGE)
					SendPSChanged(hDlg);
				break;
			case IDC_DELSERVER:
				OnDelServer(hDlg);
				SendPSChanged(hDlg);
				break;
			case IDC_SYNCTIMER:
				OnSyncTimer(hDlg);
				SendPSChanged(hDlg);
				break;
			case IDC_SYNCADAY:
				OnSyncADay(hDlg);
				SendPSChanged(hDlg);
				break;
			case IDC_SYNCMIN:
			case IDC_TIMEOUT:
			case IDC_SYNCSOUND:
				if(code == EN_CHANGE)
					SendPSChanged(hDlg);
				break;
			case IDC_SYNCNODIAL:
			case IDC_SNTPLOG:
				SendPSChanged(hDlg);
				break;
			case IDC_SYNCNOW:
				OnSyncNow(hDlg);
				break;
			case IDC_SYNCSOUNDBROWSE:
				OnSanshoAlarm(hDlg, id);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 10); break;
			}
			return TRUE;
		case WM_DESTROY:
			OnDestroy(hDlg);
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
  Initialize the page
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	char section[] = "SNTP";
	char s[1024], entry[20], server[81];
	UDACCEL uda[2];
	RECT rc;
	int n, count, i;
	HFONT hfont;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
		SendDlgItemMessage(hDlg, IDC_SYNCSOUND,
			WM_SETFONT, (WPARAM)hfont, 0);

	GetMyRegStr(section, "Server", server, 80, "");

	count = GetMyRegLong(section, "ServerNum", 0);
	for(i = 0; i < count; i++)
	{
		wsprintf(entry, "Server%d", i + 1);
		GetMyRegStr(section, entry, s, 80, "");
		if(s[0]) CBAddString(hDlg, IDC_NTPSERVER, (LPARAM)s);
	}
	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_NTPSERVER, 7);
	if(server[0])
	{
		i = CBFindStringExact(hDlg, IDC_NTPSERVER, server);
		if(i == LB_ERR)
		{
			CBInsertString(hDlg, IDC_NTPSERVER, 0, (LPARAM)server);
			i = 0;
		}
		CBSetCurSel(hDlg, IDC_NTPSERVER, i);
	}

	SendDlgItemMessage(hDlg, IDC_DELSERVER, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconDel);

	if(!hIconSync)
		hIconSync = LoadImage(g_hInst, MAKEINTRESOURCE(IDI_SNTP), IMAGE_ICON,
				16, 16, LR_DEFAULTCOLOR);
	SendDlgItemMessage(hDlg, IDC_SYNCNOW, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)hIconSync);

	OnNTPServer(hDlg);

	CreateTip(hDlg);

	CheckDlgButton(hDlg, IDC_SYNCTIMER,
		GetMyRegLong(section, "Timer", FALSE));
	CheckDlgButton(hDlg, IDC_SYNCADAY,
		GetMyRegLong(section, "ADay", FALSE));

	SendDlgItemMessage(hDlg, IDC_SYNCMINSPIN, UDM_SETRANGE, 0,
		MAKELONG(1440, 1));
	n = GetMyRegLong(section, "Minutes", 60);
	if(n < 1 || 1440 < n) n = 60;
	SendDlgItemMessage(hDlg, IDC_SYNCMINSPIN, UDM_SETPOS, 0, n);

	bNoRasAPI = GetMyRegLong("SNTP", "NoRASAPI", FALSE);

	CheckDlgButton(hDlg, IDC_SYNCNODIAL,
		GetMyRegLong(section, "NoDial", !bNoRasAPI));

	SendDlgItemMessage(hDlg, IDC_TIMEOUTSPIN, UDM_SETRANGE, 0,
		MAKELONG(30000, 1));
	n = GetMyRegLong(section, "Timeout", 1000);
	if(n < 1 || 30000 < n) n = 1000;
	SendDlgItemMessage(hDlg, IDC_TIMEOUTSPIN, UDM_SETPOS, 0, n);
	uda[0].nSec = 1; uda[0].nInc = 10;
	uda[1].nSec = 2; uda[1].nInc = 100;
	SendDlgItemMessage(hDlg, IDC_TIMEOUTSPIN, UDM_SETACCEL, 2,
		(LPARAM)uda);

	OnSyncADay(hDlg);
	OnSyncTimer(hDlg);

	CheckDlgButton(hDlg, IDC_SNTPLOG,
		GetMyRegLong(section, "SaveLog", TRUE));
	GetMyRegStr(section, "Sound", s, 1024, "");
	SetDlgItemText(hDlg, IDC_SYNCSOUND, s);

	// embed "Result" listbox in the dialog
	GetWindowRect(GetDlgItem(hDlg, IDC_SNTPLISTHERE), &rc);
	rc.right -= rc.left; rc.bottom -= rc.top;
	ScreenToClient(hDlg, (POINT*)&rc);
	SetParent(hwndSNTPLog, hDlg);
	SetWindowPos(hwndSNTPLog, GetDlgItem(hDlg, IDC_SYNCSOUNDBROWSE),
		rc.left, rc.top,
		rc.right, rc.bottom, SWP_SHOWWINDOW);
	SendMessage(hwndSNTPLog, WM_SETFONT,
		(WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);

	PostMessage(hDlg, WM_NEXTDLGCTL,
		(WPARAM)GetDlgItem(hDlg, IDC_NTPSERVER), TRUE);

	// load sntp log
	LoadLogFile();
}

void OnDestroy(HWND hDlg)
{
	UNREFERENCED_PARAMETER(hDlg);
	SetParent(hwndSNTPLog, hwndSNTP);
}

/*------------------------------------------------
  Save the setting
--------------------------------------------------*/
static void OnApply(HWND hDlg)
{
	char section[] = "SNTP";
	char s[1024], entry[20];
	char server[81];
	int n, i, count, index;

	GetDlgItemText(hDlg, IDC_NTPSERVER, server, 80);
	SetMyRegStr(section, "Server", server);

	if(server[0])
	{
		index = CBFindStringExact(hDlg, IDC_NTPSERVER, server);
		if(index != LB_ERR)
			CBDeleteString(hDlg, IDC_NTPSERVER, index);
		CBInsertString(hDlg, IDC_NTPSERVER, 0, server);
		CBSetCurSel(hDlg, IDC_NTPSERVER, 0);
	}
	count = CBGetCount(hDlg, IDC_NTPSERVER);
	for(i = 0; i < count; i++)
	{
		CBGetLBText(hDlg, IDC_NTPSERVER, i, s);
		wsprintf(entry, "Server%d", i+1);
		SetMyRegStr(section, entry, s);
	}
	SetMyRegLong(section, "ServerNum", count);

	OnNTPServer(hDlg);

	SetMyRegLong(section, "Timer", IsDlgButtonChecked(hDlg, IDC_SYNCTIMER));
	SetMyRegLong(section, "ADay", IsDlgButtonChecked(hDlg, IDC_SYNCADAY));
	n = SendDlgItemMessage(hDlg, IDC_SYNCMINSPIN, UDM_GETPOS, 0, 0);
	if(n & 0xffff0000) n = 60;
	SetMyRegLong(section, "Minutes", n);
	SetMyRegLong(section, "NoDial", IsDlgButtonChecked(hDlg, IDC_SYNCNODIAL));
	n = SendDlgItemMessage(hDlg, IDC_TIMEOUTSPIN, UDM_GETPOS, 0, 0);
	if(n & 0xffff0000) n = 1000;
	SetMyRegLong(section, "Timeout", n);
	SetMyRegLong(section, "SaveLog", IsDlgButtonChecked(hDlg, IDC_SNTPLOG));

	GetDlgItemText(hDlg, IDC_SYNCSOUND, s, 1024);
	SetMyRegStr(section, "Sound", s);

	InitSyncTimeSetting();
}

/*------------------------------------------------
    When server name changed
--------------------------------------------------*/
void OnNTPServer(HWND hDlg)
{
	EnableDlgItem(hDlg, IDC_SYNCNOW,
		GetWindowTextLength(GetDlgItem(hDlg,IDC_NTPSERVER))>0);
	EnableDlgItem(hDlg, IDC_DELSERVER,
		CBGetCount(hDlg, IDC_NTPSERVER)>0);
}

/*------------------------------------------------
    Delete Server Name
--------------------------------------------------*/
void OnDelServer(HWND hDlg)
{
	char server[81];
	int index, count;

	GetDlgItemText(hDlg, IDC_NTPSERVER, server, 80);
	count = CBGetCount(hDlg, IDC_NTPSERVER);
	index = CBFindStringExact(hDlg, IDC_NTPSERVER, server);
	if(index != LB_ERR)
	{
		CBDeleteString(hDlg, IDC_NTPSERVER, index);
		if(count > 1)
		{
			if(index == count - 1) index--;
			CBSetCurSel(hDlg, IDC_NTPSERVER, index);
		}
		else SetDlgItemText(hDlg, IDC_NTPSERVER, "");
		PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
		OnNTPServer(hDlg);
	}
}

/*------------------------------------------------
  "Adjust Regularly"
--------------------------------------------------*/
void OnSyncTimer(HWND hDlg)
{
	BOOL b;
	int i;

	b = IsDlgButtonChecked(hDlg, IDC_SYNCTIMER);
	for(i = IDC_SYNCADAY; i <= IDC_LABSYNCMIN2; i++)
		EnableDlgItem(hDlg, i, b);
	EnableDlgItem(hDlg, IDC_SYNCNODIAL, b && (!bNoRasAPI) );

	if(b) OnSyncADay(hDlg);
}

/*------------------------------------------------
  "Once A Day"
--------------------------------------------------*/
void OnSyncADay(HWND hDlg)
{
	BOOL b;
	int i;

	b = IsDlgButtonChecked(hDlg, IDC_SYNCADAY);
	for(i = IDC_SYNCMIN; i <= IDC_LABSYNCMIN2; i++)
		EnableDlgItem(hDlg, i, !b);
}

/*------------------------------------------------
  browse sound file
--------------------------------------------------*/
void OnSanshoAlarm(HWND hDlg, WORD id)
{
	char deffile[MAX_PATH], fname[MAX_PATH];

	GetDlgItemText(hDlg, id - 1, deffile, MAX_PATH);

	if(!BrowseSoundFile(hDlg, deffile, fname, id)) // soundselect.c
		return;

	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
}

/*------------------------------------------------
  "Adjust Now"
--------------------------------------------------*/
void OnSyncNow(HWND hDlg)
{
	char server[81];
	int nto;
	int index;

	GetDlgItemText(hDlg, IDC_NTPSERVER, server, 80);
	if(server[0] == 0) return;
	nto = SendDlgItemMessage(hDlg, IDC_TIMEOUTSPIN, UDM_GETPOS, 0, 0);
	StartSyncTime(g_hwndMain, server, nto);

	index = CBFindStringExact(hDlg, IDC_NTPSERVER, server);
	if(index != LB_ERR)
		CBDeleteString(hDlg, IDC_NTPSERVER, index);
	CBInsertString(hDlg, IDC_NTPSERVER, 0, server);
	CBSetCurSel(hDlg, IDC_NTPSERVER, 0);
	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_NTPSERVER, 7);
}

/*------------------------------------------------
    create tooltip window
--------------------------------------------------*/
void CreateTip(HWND hDlg)
{
	TOOLINFO ti;

	hwndTip = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR)NULL,
		TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hDlg, NULL, g_hInst, NULL);
	if(!hwndTip) return;

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND|TTF_SUBCLASS;
	ti.hwnd = hDlg;
	ti.hinst = GetLangModule();
	ti.uId = (UINT_PTR)GetDlgItem(hDlg, IDC_SYNCNOW);
	ti.lpszText = (char*)IDS_SYNCNOW;

	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO)&ti);
}

// load sntp log file
void LoadLogFile()
{
	HANDLE hFile;
	char fname[MAX_PATH];
	char* logValue = NULL;
	char* p = NULL;
	DWORD dwSize = 0;
	DWORD i = 0;

	SendMessage(hwndSNTPLog, LB_RESETCONTENT, 0, 0);

	strcpy(fname, g_mydir);
	add_title(fname, "SNTP.txt");
	hFile = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) goto CLEANUP;

	dwSize = GetFileSize(hFile, NULL);
	if (!dwSize) goto CLEANUP;

	logValue = malloc(dwSize+1);
	if (!logValue) goto CLEANUP;

	if (!ReadFile(hFile, logValue, dwSize, &dwSize, NULL)) goto CLEANUP;
	logValue[dwSize] = '\0';

	p = logValue;
	for (i=0; i<dwSize; ++i)
	{
		if (logValue[i] == '\r')
		{
			if (logValue[++i] == '\n')
			{
				logValue[i-1] = logValue[i] = '\0';
				SendMessage(hwndSNTPLog, LB_ADDSTRING, 0, (LPARAM)p);
				p = logValue + i+1;
			}
		}
	}

	SendMessage(hwndSNTPLog, LB_SETCURSEL, SendMessage(hwndSNTPLog, LB_GETCOUNT, 0, 0)-1, 0);

CLEANUP:
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	if (logValue) free(logValue);
}

