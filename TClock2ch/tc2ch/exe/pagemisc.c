/*-------------------------------------------
  pagemisc.c
　　「その他」プロパティページ
　　KAZUBON 1997-1998
---------------------------------------------*/
#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnDeskcal(HWND hDlg);
static void OnCheckNet(HWND hDlg);
static void OnSanshoDeskcal(HWND hDlg, WORD id);

static void OnStartup(HWND hDlg);
BOOL CreateLink(LPCSTR fname, LPCSTR dstpath, LPCSTR name);

#define SendPSChanged(hDlg) SendMessage(GetParent(hDlg),PSM_CHANGED,(WPARAM)(hDlg),0)

/*------------------------------------------------
　「その他」ページ用ダイアログプロシージャ
--------------------------------------------------*/
BOOL CALLBACK PageMiscProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
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
			// 「時計を改造しない」
			case IDC_NOCLOCK:
			case IDC_TASKWNDCTRL:
				{
				g_bApplyClock = TRUE;
				SendPSChanged(hDlg);
				break;
				}
			// 「デスクトップカレンダーの自動更新」
			case IDC_DESKCAL:
				OnDeskcal(hDlg);
				break;
			// 「定期的にネットワークの確認」
			case IDC_CHECKNETWORK:
				OnCheckNet(hDlg);
				break;
			case IDC_CHECKNETINTERVAL:
				if(code == EN_CHANGE)
					SendPSChanged(hDlg);
				break;
			// チェックボックス
			case IDC_MCIWAVE:
			case IDC_ONLYDATECHANGED:
			case IDC_RESUMESUSPEND:
			case IDC_TONIKAKU:
			case IDC_WATCHWALL:
				SendPSChanged(hDlg);
				break;
			// 何秒後に開始
			case IDC_DELAYSTART:
			case IDC_DELAYNET:
				if(code == EN_CHANGE)
					SendPSChanged(hDlg);
				break;
			// 「...」 デスクトップカレンダーの参照
			case IDC_SANSHODESKCAL:
				OnSanshoDeskcal(hDlg, id);
				break;
			// 「スタートアップ」にショートカットをつくる
			case IDC_STARTUP:
				OnStartup(hDlg);
				break;
			case IDC_DELREG:
				{
					int r;
					r = DelRegAll();
					if (r == 0)
						MyMessageBox(hDlg,MyString(IDS_DELREGNGINI),MyString(IDS_DELREGT),MB_OK,MB_ICONEXCLAMATION);
					else if (r == 1)
						MyMessageBox(hDlg,MyString(IDS_DELREGOK),MyString(IDS_DELREGT),MB_OK,MB_ICONINFORMATION);
					else
						MyMessageBox(hDlg,MyString(IDS_DELREGNG),MyString(IDS_DELREGT),MB_OK,MB_ICONEXCLAMATION);
				}
				break;
			case IDC_CONFMAX:
			case IDC_CFMAXSPIN:
				SendPSChanged(hDlg);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 11); break;
			}
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
　ページの初期化
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	char s[1024];
	HFONT hfont;
	DWORD dw;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
		SendDlgItemMessage(hDlg, IDC_COMDESKCAL,
			WM_SETFONT, (WPARAM)hfont, 0);

	CheckDlgButton(hDlg, IDC_NOCLOCK,
		GetMyRegLong("", "NoClock", FALSE));

	CheckDlgButton(hDlg, IDC_MCIWAVE,
		GetMyRegLong("", "MCIWave", FALSE));

	SendDlgItemMessage(hDlg, IDC_SPINDELAYSTART, UDM_SETRANGE, 0,
		MAKELONG(600, 0));
	SendDlgItemMessage(hDlg, IDC_SPINDELAYSTART, UDM_SETPOS, 0,
		(int)(short)GetMyRegLong("", "DelayStart", 0));
	SendDlgItemMessage(hDlg, IDC_SPINDELAYNET, UDM_SETRANGE, 0,
		MAKELONG(600, 0));
	SendDlgItemMessage(hDlg, IDC_SPINDELAYNET, UDM_SETPOS, 0,
		(int)(short)GetMyRegLong("", "DelayNet", 0));
	SendDlgItemMessage(hDlg, IDC_SPINCHECKNETINTERVAL, UDM_SETRANGE, 0,
		MAKELONG(0, 600));
	SendDlgItemMessage(hDlg, IDC_SPINCHECKNETINTERVAL, UDM_SETPOS, 0,
		(int)(short)GetMyRegLong("", "NetRestartInterval", 0));

	CheckDlgButton(hDlg, IDC_DESKCAL,
		GetMyRegLong("", "Deskcal", FALSE));
	CheckDlgButton(hDlg, IDC_CHECKNETWORK,
		GetMyRegLong("", "DoNetRestart", FALSE));

	GetMyRegStr("", "DeskcalCommand", s, 1024, "");
	if(s[0] == '\0') {
		GetRegStr(HKEY_CURRENT_USER, "Software\\Shinonon\\Deskcal",
			"ExeFileName", s, 1024, "");
	}
	SetDlgItemText(hDlg, IDC_COMDESKCAL, s);

	CheckDlgButton(hDlg, IDC_ONLYDATECHANGED,
		GetMyRegLong("", "DeskcalOnlyDate", FALSE));
	CheckDlgButton(hDlg, IDC_RESUMESUSPEND,
		GetMyRegLong("", "DeskcalResumeSuspend", FALSE));
	CheckDlgButton(hDlg, IDC_TONIKAKU,
		GetMyRegLong("", "DeskcalTonikaku", FALSE));
	CheckDlgButton(hDlg, IDC_WATCHWALL,
		GetMyRegLong("", "WatchWallpaper", FALSE));

	CheckDlgButton(hDlg, IDC_TASKWNDCTRL,
		GetMyRegLong("", "WatchTaskbarWindow", FALSE));

	dw = GetMyRegLong(NULL, "ConfigMax", 1);
	if(dw > 30) dw = 30;
	if(dw < 1  ) dw = 1;
	SendDlgItemMessage(hDlg,IDC_CFMAXSPIN,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)30, (short)1));
	SendDlgItemMessage(hDlg, IDC_CFMAXSPIN, UDM_SETPOS, 0,
		(int)(short)dw);

	OnDeskcal(hDlg);
	OnCheckNet(hDlg);
}

/*------------------------------------------------
　更新
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	char s[1024];
	DWORD dw;

	SetMyRegLong("", "NoClock", IsDlgButtonChecked(hDlg, IDC_NOCLOCK));

	SetMyRegLong("", "MCIWave", IsDlgButtonChecked(hDlg, IDC_MCIWAVE));

	SetMyRegLong("", "DelayStart",
		SendDlgItemMessage(hDlg, IDC_SPINDELAYSTART, UDM_GETPOS, 0, 0));
	SetMyRegLong("", "DelayNet",
		SendDlgItemMessage(hDlg, IDC_SPINDELAYNET, UDM_GETPOS, 0, 0));

	SetMyRegLong("", "DoNetRestart", IsDlgButtonChecked(hDlg, IDC_CHECKNETWORK));
	SetMyRegLong("", "NetRestartInterval",
		SendDlgItemMessage(hDlg, IDC_SPINCHECKNETINTERVAL, UDM_GETPOS, 0, 0));

	SetMyRegLong("", "Deskcal", IsDlgButtonChecked(hDlg, IDC_DESKCAL));

	GetDlgItemText(hDlg, IDC_COMDESKCAL, s, 1024);
	SetMyRegStr("", "DeskcalCommand", s);

	SetMyRegLong("", "DeskcalOnlyDate",
		IsDlgButtonChecked(hDlg, IDC_ONLYDATECHANGED));

	SetMyRegLong("", "DeskcalResumeSuspend",
		IsDlgButtonChecked(hDlg, IDC_RESUMESUSPEND));

	SetMyRegLong("", "DeskcalTonikaku",
		IsDlgButtonChecked(hDlg, IDC_TONIKAKU));
	SetMyRegLong("", "WatchWallpaper",
		IsDlgButtonChecked(hDlg, IDC_WATCHWALL));

	SetMyRegLong("", "WatchTaskbarWindow", IsDlgButtonChecked(hDlg, IDC_TASKWNDCTRL));

	dw = GetDlgItemInt(hDlg, IDC_CONFMAX, NULL, FALSE);
	if(dw > 30) dw = 30;
	if(dw < 1  ) dw = 1;
	SetDlgItemInt(hDlg, IDC_CONFMAX, dw, FALSE);

	SetMyRegLong("", "ConfigMax", dw);

	InitWatchWallpaper(); // deskcal.c
}

/*------------------------------------------------
　「デスクトップカレンダー」の自動更新
--------------------------------------------------*/
void OnDeskcal(HWND hDlg)
{
	BOOL b;
	int i;

	b = IsDlgButtonChecked(hDlg, IDC_DESKCAL);
	for(i = IDC_LABDESKCAL; i <= IDC_WATCHWALL; i++)
		EnableDlgItem(hDlg, i, b);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
　定期的にネットワーク確認
--------------------------------------------------*/
void OnCheckNet(HWND hDlg)
{
	BOOL b;
	int i;

	b = IsDlgButtonChecked(hDlg, IDC_CHECKNETWORK);
	for(i = IDC_CHECKNETINTERVAL; i <= IDC_NETCHECKSTATIC; i++)
		EnableDlgItem(hDlg, i, b);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
　「...」　デスクトップカレンダーの参照
--------------------------------------------------*/
void OnSanshoDeskcal(HWND hDlg, WORD id)
{
	char filter[] = "Deskcal.exe\0Deskcal.exe\0";
	char deffile[MAX_PATH], fname[MAX_PATH];
	//HFILE hf = HFILE_ERROR;

	GetDlgItemText(hDlg, id - 1, deffile, MAX_PATH);
	if(strlen(deffile) > 10 &&
		strcmp(deffile + (strlen(deffile) - 10), " -OnlyDraw") == 0)
		deffile[strlen(deffile) - 10] = 0;

	if(!SelectMyFile(hDlg, filter, 0, deffile, fname)) // propsheet.c
		return;

	strcat(fname, " -OnlyDraw");
	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
　「スタートアップ」にショートカットをつくる
--------------------------------------------------*/
void OnStartup(HWND hDlg)
{
	LPITEMIDLIST pidl;
	char dstpath[MAX_PATH], myexe[MAX_PATH];

	if(SHGetSpecialFolderLocation(hDlg, CSIDL_STARTUP, &pidl) == NOERROR &&
		SHGetPathFromIDList(pidl, dstpath) == TRUE)
		;
	else return;

	if(MyMessageBox(hDlg, MyString(IDS_STARTUPLINK),
		"TClock", MB_YESNO, MB_ICONQUESTION) != IDYES) return;

	GetModuleFileName(GetModuleHandle(NULL), myexe, MAX_PATH);
	CreateLink(myexe, dstpath, "TClock");
}

/*------------------------------------------------
　ショートカットの作成
--------------------------------------------------*/
BOOL CreateLink(LPCSTR fname, LPCSTR dstpath, LPCSTR name)
{
	HRESULT hres;
	IShellLink* psl;

	CoInitialize(NULL);

	hres = CoCreateInstance(&CLSID_ShellLink, NULL,
		CLSCTX_INPROC_SERVER, &IID_IShellLink, &psl);
	if(SUCCEEDED(hres))
	{
		IPersistFile* ppf;
		char path[MAX_PATH];

/*		path[0] = '\"';
		strcpy(path+1, fname);
		strcat(path, "\"");*/
		psl->lpVtbl->SetPath(psl, fname);
		psl->lpVtbl->SetDescription(psl, name);
		strcpy(path, fname);
		del_title(path);
		psl->lpVtbl->SetWorkingDirectory(psl, path);

		hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile,
			&ppf);

		if(SUCCEEDED(hres))
		{
			WORD wsz[MAX_PATH];
			char lnkfile[MAX_PATH];
			strcpy(lnkfile, dstpath);
			add_title(lnkfile, (char*)name);
			strcat(lnkfile, ".lnk");

			MultiByteToWideChar(CP_ACP, 0, lnkfile, -1,
				wsz, MAX_PATH);

			hres = ppf->lpVtbl->Save(ppf, wsz, TRUE);
			ppf->lpVtbl->Release(ppf);
		}
		psl->lpVtbl->Release(psl);
	}
	CoUninitialize();

	if(SUCCEEDED(hres)) return TRUE;
	else return FALSE;
}
