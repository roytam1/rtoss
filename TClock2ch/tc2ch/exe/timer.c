/*-------------------------------------------
  timer.c
           Kazubon 1998-1999
---------------------------------------------*/

#include "tclock.h"

// structure for timer setting
typedef struct _tagTimerStruct
{
	char name[80];
	int id;
	int minute;
	char fname[1024];
	BOOL bRepeat;
	BOOL bBlink;
} TIMERSTRUCT;
typedef TIMERSTRUCT* PTIMERSTRUCT;

// structure for timer executing
typedef struct _tagTimerStruct2
{
	char name[80];
	int id;
	DWORD seconds;
	DWORD tickonstart;
} TIMERSTRUCT2;
typedef TIMERSTRUCT2* PTIMERSTRUCT2;

BOOL CALLBACK DlgProcTimer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void OnInit(HWND hDlg);
static void OnDestroy(HWND hDlg);
static void OnOK(HWND hDlg);
static void OnTimerName(HWND hDlg);
static void OnSanshoAlarm(HWND hDlg, WORD id);
static void OnDel(HWND hDlg);
static void OnTest(HWND hDlg, WORD id);
static void Ring(HWND hwnd, int id);

static int nTimerCount = 0;
static PTIMERSTRUCT2 pTimersWorking = NULL;

// propsheet.c
void SetMyDialgPos(HWND hwnd);

/*------------------------------------------------
   open "Timer" dialog
--------------------------------------------------*/
void DialogTimer(HWND hwnd)
{
	HWND hwndTray;

	hwndTray = FindWindow("Shell_TrayWnd", NULL);
	if(g_hDlgTimer && IsWindow(g_hDlgTimer))
		;
	else
		g_hDlgTimer = CreateDialog(GetLangModule(), MAKEINTRESOURCE(IDD_TIMER),
			NULL, (DLGPROC)DlgProcTimer);
	SetForegroundWindow98(g_hDlgTimer);
}

/*------------------------------------------------
   dialog procedure of "Timer" dialog
--------------------------------------------------*/
BOOL CALLBACK DlgProcTimer(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)g_hIconTClock);
			OnInit(hDlg);
			SetMyDialgPos(hDlg);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
				case IDC_TIMERNAME:
					if(code == CBN_EDITCHANGE)
						OnTimerName(hDlg);
					else if(code == CBN_SELCHANGE)
						PostMessage(hDlg, WM_COMMAND,
							MAKELONG(id, CBN_EDITCHANGE), 0);
					break;
				case IDC_TIMERDEL:
					OnDel(hDlg);
					break;
				case IDC_TIMERMINUTE:
					break;
				case IDC_TIMERFILE:
					break;
				case IDC_TIMERSANSHO:
					OnSanshoAlarm(hDlg, id); break;
				case IDC_TIMERTEST:
					OnTest(hDlg, id); break;
				case IDC_TIMERHELP:
					My2chHelp(GetParent(hDlg), 13); break;
				case IDOK:
					OnOK(hDlg);
				case IDCANCEL:
					DestroyWindow(hDlg);
			}
			return TRUE;
		}
		case WM_DESTROY:
			// MyHelp(hDlg, -1);
			StopFile();
			OnDestroy(hDlg);
			g_hDlgTimer = NULL;
			break;
		case MM_MCINOTIFY:
		case MM_WOM_DONE:
			StopFile();
			SendDlgItemMessage(hDlg, IDC_TIMERTEST, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)g_hIconPlay);
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
   initialize "Timer" dialog
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	HFONT hfont;
	char subkey[20];
	int i, count;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
	{
		SendDlgItemMessage(hDlg, IDC_TIMERNAME,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_TIMERFILE,
			WM_SETFONT, (WPARAM)hfont, 0);
	}

	SendDlgItemMessage(hDlg, IDC_TIMERSPIN, UDM_SETRANGE, 0,
		MAKELONG(1440, 0));

	count = GetMyRegLong("", "TimerNum", 0);
	for(i = 0; i < count; i++)
	{
		PTIMERSTRUCT pts;
		int index;

		pts = (PTIMERSTRUCT)malloc(sizeof(TIMERSTRUCT));
		wsprintf(subkey, "Timer%d", i + 1);
		GetMyRegStr(subkey, "Name", pts->name, 80, "");
		pts->id = GetMyRegLong(subkey, "ID", 0);
		pts->minute = GetMyRegLong(subkey, "Minute", 10);
		GetMyRegStr(subkey, "File", pts->fname, 1024, "");
		pts->bRepeat = GetMyRegLong(subkey, "Repeat", FALSE);
		pts->bBlink = GetMyRegLong(subkey, "Blink", FALSE);
		index = CBAddString(hDlg, IDC_TIMERNAME, (LPARAM)pts->name);
		CBSetItemData(hDlg, IDC_TIMERNAME, index, (LPARAM)pts);
	}
	if(count > 0)
		CBSetCurSel(hDlg, IDC_TIMERNAME, 0);
	else
		SendDlgItemMessage(hDlg, IDC_TIMERSPIN, UDM_SETPOS, 0, 10);
	OnTimerName(hDlg);

	SendDlgItemMessage(hDlg, IDC_TIMERTEST, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconPlay);

	SendDlgItemMessage(hDlg, IDC_TIMERDEL, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconDel);
}

/*------------------------------------------------
  free memories associated with combo box.
--------------------------------------------------*/
void OnDestroy(HWND hDlg)
{
	int i, count;

	count = CBGetCount(hDlg, IDC_TIMERNAME);
	for(i = 0; i < count; i++)
	{
		PTIMERSTRUCT pts;
		pts = (PTIMERSTRUCT)CBGetItemData(hDlg, IDC_TIMERNAME, i);
		free(pts);
	}
}

/*------------------------------------------------
   called when "OK" button is clicked,
   start timer
--------------------------------------------------*/
void OnOK(HWND hDlg)
{
	char subkey[20], s[1024], name[80];
	int i, j, id, count, minutes;
	PTIMERSTRUCT2 temp;

	GetDlgItemText(hDlg, IDC_TIMERNAME, s, 80);
	strcpy(name, s);

	// save settings
	j = 1; id = -1;
	count = CBGetCount(hDlg, IDC_TIMERNAME);
	for(i = 0; i < count; i++)
	{
		PTIMERSTRUCT pts;
		pts = (PTIMERSTRUCT)CBGetItemData(hDlg, IDC_TIMERNAME, i);
		if(strcmp(pts->name, name) != 0)
		{
			wsprintf(subkey, "Timer%d", j + 1);
			SetMyRegStr(subkey, "Name", pts->name);
			SetMyRegLong(subkey, "ID", pts->id);
			SetMyRegLong(subkey, "Minute", pts->minute);
			SetMyRegStr(subkey, "File", pts->fname);
			SetMyRegLong(subkey, "Repeat", pts->bRepeat);
			SetMyRegLong(subkey, "Blink", pts->bBlink);
			j++;
		}
		else id = pts->id;
	}
	SetMyRegLong("", "TimerNum", j);

	if(id < 0)
	{
		id = 0;
		for(i = 0; i < count; i++)
		{
			PTIMERSTRUCT pts;
			pts = (PTIMERSTRUCT)CBGetItemData(hDlg, IDC_TIMERNAME, i);
			if(pts->id >= id) id = pts->id + 1;
		}
	}
	strcpy(subkey, "Timer1");
	SetMyRegStr(subkey, "Name", name);
	SetMyRegLong(subkey, "ID", id);
	minutes = SendDlgItemMessage(hDlg, IDC_TIMERSPIN, UDM_GETPOS, 0, 0);
	SetMyRegLong(subkey, "Minute", minutes);
	GetDlgItemText(hDlg, IDC_TIMERFILE, s, MAX_PATH);
	SetMyRegStr(subkey, "File", s);
	SetMyRegLong(subkey, "Repeat",
		IsDlgButtonChecked(hDlg, IDC_TIMERREPEAT));
	SetMyRegLong(subkey, "Blink",
		IsDlgButtonChecked(hDlg, IDC_TIMERBLINK));

	// start timer
	temp = pTimersWorking;
	pTimersWorking = (PTIMERSTRUCT2)malloc(
		sizeof(TIMERSTRUCT2)*(nTimerCount + 1));
	for(i = 0; i < nTimerCount; i++) pTimersWorking[i] = temp[i];
	if(temp) free(temp);
	strcpy(pTimersWorking[i].name, name);
	pTimersWorking[i].id = id;
	pTimersWorking[i].seconds = minutes * 60;
	pTimersWorking[i].tickonstart = GetTickCount();

	nTimerCount++;
}

/*------------------------------------------------
   called when "Name" edit box is changed
--------------------------------------------------*/
void OnTimerName(HWND hDlg)
{
	char s[1024];
	int i, count;

	GetDlgItemText(hDlg, IDC_TIMERNAME, s, 80);
	count = CBGetCount(hDlg, IDC_TIMERNAME);
	for(i = 0; i < count; i++)
	{
		PTIMERSTRUCT pts;
		pts = (PTIMERSTRUCT)CBGetItemData(hDlg, IDC_TIMERNAME, i);
		if(strcmp(s, pts->name) == 0)
		{
			SetDlgItemInt(hDlg, IDC_TIMERMINUTE, pts->minute, FALSE);
			SetDlgItemText(hDlg, IDC_TIMERFILE, pts->fname);
			CheckDlgButton(hDlg, IDC_TIMERREPEAT, pts->bRepeat);
			CheckDlgButton(hDlg, IDC_TIMERBLINK, pts->bBlink);
			break;
		}
	}

	EnableDlgItem(hDlg, IDC_TIMERDEL, i < count);
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
   delete one setting of timer
--------------------------------------------------*/
void OnDel(HWND hDlg)
{
	char s[1024], subkey[20];
	int i, count;

	GetDlgItemText(hDlg, IDC_TIMERNAME, s, 80);
	count = CBGetCount(hDlg, IDC_TIMERNAME);
	for(i = 0; i < count; i++)
	{
		PTIMERSTRUCT pts;
		pts = (PTIMERSTRUCT)CBGetItemData(hDlg, IDC_TIMERNAME, i);
		if(strcmp(s, pts->name) == 0) { free(pts); break; }
	}
	if(i >= count) return;

	CBDeleteString(hDlg, IDC_TIMERNAME, i);
	if(count > 1)
		CBSetCurSel(hDlg, IDC_TIMERNAME, (i>0)?(i-1):i);
	else
		SetDlgItemText(hDlg, IDC_TIMERNAME, "");
	OnTimerName(hDlg);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);

	wsprintf(subkey, "Timer%d", count);
	DelMyRegKey(subkey);

	for(i = 0; i < count - 1; i++)
	{
		PTIMERSTRUCT pts;
		pts = (PTIMERSTRUCT)CBGetItemData(hDlg, IDC_TIMERNAME, i);
		wsprintf(subkey, "Timer%d", i + 1);
		SetMyRegStr(subkey, "Name", pts->name);
		SetMyRegLong(subkey, "ID", pts->id);
		SetMyRegLong(subkey, "Minute", pts->minute);
		SetMyRegStr(subkey, "File", pts->fname);
		SetMyRegLong(subkey, "Repeat", pts->bRepeat);
		SetMyRegLong(subkey, "Blink", pts->bBlink);
	}
	SetMyRegLong("", "TimerNum", count - 1);
}

/*------------------------------------------------
   play or stop sound in dialog
--------------------------------------------------*/
void OnTest(HWND hDlg, WORD id)
{
	char fname[MAX_PATH];

	GetDlgItemText(hDlg, id - 3, fname, MAX_PATH);
	if(fname[0] == 0) return;

	if((HICON)SendDlgItemMessage(hDlg, id, BM_GETIMAGE, IMAGE_ICON, 0)
		== g_hIconPlay)
	{
		if(PlayFile(hDlg, fname, 0))
		{
			SendDlgItemMessage(hDlg, id, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)g_hIconStop);
			InvalidateRect(GetDlgItem(hDlg, id), NULL, FALSE);
		}
	}
	else StopFile();
}

/*------------------------------------------------
    called when main window receives WM_TIMER
--------------------------------------------------*/
void OnTimerTimer(HWND hwnd, SYSTEMTIME* st)
{
	int i;
	DWORD tick;

	if(nTimerCount == 0) return;
	tick = GetTickCount();
	for(i = 0; i < nTimerCount; i++)
	{
		DWORD seconds;

		seconds = (tick - pTimersWorking[i].tickonstart) / 1000;
		if(seconds > pTimersWorking[i].seconds)
		{
			Ring(hwnd, pTimersWorking[i].id);
			StopTimer(hwnd, i);
			i--;
		}
	}
}

/*------------------------------------------------
    sound or open file of timer.
--------------------------------------------------*/
void Ring(HWND hwnd, int id)
{
	char subkey[20], fname[1024];
	int i, count;

	count = GetMyRegLong("", "TimerNum", 0);
	for(i = 0; i < count; i++)
	{
		wsprintf(subkey, "Timer%d", i + 1);
		if(id == GetMyRegLong(subkey, "ID", 0))
		{
			GetMyRegStr(subkey, "File", fname, 1024, "");
			PlayFile(hwnd, fname,
				GetMyRegLong(subkey, "Repeat", FALSE)?(-1):0);
			if(GetMyRegLong(subkey, "Blink", FALSE))
				PostMessage(g_hwndClock, CLOCKM_BLINK, FALSE, 0);
			break;
		}
	}
}

/*------------------------------------------------
    clear all timer
--------------------------------------------------*/
void EndTimer(void)
{
	if(pTimersWorking) free(pTimersWorking);
	nTimerCount = 0;
}

/*----------------------------------------------------------------
    get timer name and remained time for popup menu.
------------------------------------------------------------------*/
int GetTimerInfo(char *dst, int num)
{
	DWORD tick;
	int seconds;

	if(num < nTimerCount)
	{
		tick = GetTickCount();
		seconds = (tick - pTimersWorking[num].tickonstart) / 1000;
		seconds = pTimersWorking[num].seconds - seconds;
		wsprintf(dst, "  %s %02d:%02d",
			pTimersWorking[num].name, seconds / 60, seconds % 60);
		return strlen(dst);
	}
	return 0;
}

/*---------------------------------------------------------------
    free memory to clear a timer
-----------------------------------------------------------------*/
void StopTimer(HWND hwnd, int tostop)
{
	PTIMERSTRUCT2 temp;
	int i, j;

	if(tostop >= nTimerCount) return;

	temp = pTimersWorking;
	if(nTimerCount > 1)
	{
		pTimersWorking = (PTIMERSTRUCT2)malloc(
			sizeof(TIMERSTRUCT2)*(nTimerCount - 1));
		for(i = 0, j = 0; i < nTimerCount; i++)
		{
			if(tostop != i) pTimersWorking[j++] = temp[i];
		}
	}
	else pTimersWorking = NULL;
	if(temp) free(temp);
	nTimerCount--;
}

