/*-------------------------------------------
  pagetaskbar.c
  "taskbar" page of properties
  Kazubon 1997-1999
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnStartBtnHide(HWND hDlg);
static void OnTaskSwitchFlat(HWND hDlg);
static void OnTaskBarBorder(HWND hDlg);
static BOOL IsIE4(void);
static BOOL IsTrayIconsToolbar(void);

extern BOOL g_bApplyTaskbar; // propsheet.c

__inline void SendPSChanged(HWND hDlg)
{
	g_bApplyTaskbar = TRUE;
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
}

/*------------------------------------------------
   dialog procedure of this page
--------------------------------------------------*/
BOOL CALLBACK PageTaskbarProc(HWND hDlg, UINT message,
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
			// checked "Hide Start Button", "Start menu from Clock"
			case IDC_STARTBTNHIDE:
			case IDC_STARTMENUCLOCK:
				OnStartBtnHide(hDlg);
				break;
			case IDC_TASKSWITCHFLAT:
			case IDC_TASKSWITCH_SEPARAT:
				OnTaskSwitchFlat(hDlg);
				break;
			case IDC_TBBORDER:
			case IDC_TBBORDEREX:
				OnTaskBarBorder(hDlg);
				break;
			// checked other boxes
			case IDC_STARTBTNFLAT:
			case IDC_FLATTRAY:
			case IDC_FILLTRAY:
			case IDC_RBHIDE:
				SendPSChanged(hDlg);
				break;
			// "Transparancy"
			case IDC_BARTRANS:
			case IDC_MENUTRANS:
				if(code == EN_CHANGE)
					SendPSChanged(hDlg);
				break;
			case IDC_FITCLOCK: //390
				g_bApplyClock = TRUE;
				SendPSChanged(hDlg);
				break;
			case IDC_TASKSWITCHICON:
				SendPSChanged(hDlg);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 8); break;
			}
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
   initialize
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	//BOOL bTrayIconsToolbar;
	DWORD dw;

	CheckDlgButton(hDlg, IDC_STARTBTNHIDE,
		GetMyRegLong(NULL, "StartButtonHide", FALSE));
	CheckDlgButton(hDlg, IDC_STARTMENUCLOCK,
		GetMyRegLong(NULL, "StartMenuClock", FALSE));
	OnStartBtnHide(hDlg);

	CheckDlgButton(hDlg, IDC_STARTBTNFLAT,
		GetMyRegLong(NULL, "StartButtonFlat", FALSE));

	CheckDlgButton(hDlg, IDC_TASKSWITCHFLAT,
		GetMyRegLong(NULL, "TaskSwitchFlat", FALSE));
	CheckDlgButton(hDlg, IDC_TASKSWITCH_SEPARAT,
		GetMyRegLong(NULL, "TaskSwitchSeparators", FALSE));
	CheckDlgButton(hDlg, IDC_TASKSWITCHICON,
		GetMyRegLong(NULL, "TaskSwitchIconsOnly", FALSE));
	CheckDlgButton(hDlg, IDC_RBHIDE,
		GetMyRegLong(NULL, "RebarGripperHide", FALSE));
	CheckDlgButton(hDlg, IDC_TBBORDER,
		GetMyRegLong(NULL, "TaskBarBorder", FALSE));
	CheckDlgButton(hDlg, IDC_TBBORDEREX,
		GetMyRegLong(NULL, "TaskBarBorderEx", FALSE));
	OnTaskBarBorder(hDlg);

	if(IsIE4())
	{
		OnTaskSwitchFlat(hDlg);
	}
	else
	{
		EnableDlgItem(hDlg, IDC_TASKSWITCHFLAT, FALSE);
		EnableDlgItem(hDlg, IDC_TASKSWITCH_SEPARAT, FALSE);
	}

	CheckDlgButton(hDlg, IDC_FLATTRAY,
		GetMyRegLong(NULL, "FlatTray", TRUE));

	CheckDlgButton(hDlg, IDC_FITCLOCK,                //390
		GetMyRegLong(NULL, "FitClock", FALSE));       //390

	//bTrayIconsToolbar = IsTrayIconsToolbar();

	CheckDlgButton(hDlg, IDC_FILLTRAY,
		GetMyRegLong(NULL, "FillTray", FALSE));
	//EnableDlgItem(hDlg, IDC_FILLTRAY, bTrayIconsToolbar);

	dw = GetVersion();

	if(LOBYTE(LOWORD(dw)) >= 5 || ((dw & 0x80000000) && LOBYTE(LOWORD(dw)) == 4 && HIBYTE(LOWORD(dw)) >= 90)) ; // WinXP,2000,Me
	else
	{
		EnableDlgItem(hDlg, IDC_FILLTRAY, FALSE);
	}

	if(!(dw & 0x80000000) && LOBYTE(LOWORD(dw)) >= 5) ; // Win2000
	else
	{
		int i;
		for(i = IDC_CAPBARTRANS; i <= IDC_SPINBARTRANS; i++)
			EnableDlgItem(hDlg, i, FALSE);
		if(GetDlgItem(hDlg, IDC_CAPMENUTRANS))
		{
			for(i = IDC_CAPMENUTRANS; i <= IDC_SPINMENUTRANS; i++)
				EnableDlgItem(hDlg, i, FALSE);
		}
	}

	dw = GetMyRegLong(NULL, "AlphaTaskbar", 0);
	if(dw > 100) dw = 100;
	SendDlgItemMessage(hDlg, IDC_SPINBARTRANS, UDM_SETRANGE, 0,
		MAKELONG(100, 0));
	SendDlgItemMessage(hDlg, IDC_SPINBARTRANS, UDM_SETPOS, 0,
		(int)(short)dw);

	if(GetDlgItem(hDlg, IDC_CAPMENUTRANS))
	{
		dw = GetMyRegLong(NULL, "AlphaStartMenu", 0);
		if(dw > 100) dw = 100;
		SendDlgItemMessage(hDlg, IDC_SPINMENUTRANS, UDM_SETRANGE, 0,
			MAKELONG(100, 0));
		SendDlgItemMessage(hDlg, IDC_SPINMENUTRANS, UDM_SETPOS, 0,
			(int)(short)dw);
	}
}

/*------------------------------------------------
  apply - save settings
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	SetMyRegLong(NULL, "StartButtonHide",
		IsDlgButtonChecked(hDlg, IDC_STARTBTNHIDE));
	SetMyRegLong(NULL, "StartMenuClock",
		IsDlgButtonChecked(hDlg, IDC_STARTMENUCLOCK));

	SetMyRegLong(NULL, "StartButtonFlat",
		IsDlgButtonChecked(hDlg, IDC_STARTBTNFLAT));

	SetMyRegLong(NULL, "TaskSwitchFlat",
		IsDlgButtonChecked(hDlg, IDC_TASKSWITCHFLAT));
	SetMyRegLong(NULL, "TaskSwitchSeparators",
		IsDlgButtonChecked(hDlg, IDC_TASKSWITCH_SEPARAT));
	SetMyRegLong(NULL, "TaskSwitchIconsOnly",
		IsDlgButtonChecked(hDlg, IDC_TASKSWITCHICON));
	SetMyRegLong(NULL, "RebarGripperHide",
		IsDlgButtonChecked(hDlg, IDC_RBHIDE));
	SetMyRegLong(NULL, "TaskBarBorder",
		IsDlgButtonChecked(hDlg, IDC_TBBORDER));
	SetMyRegLong(NULL, "TaskBarBorderEx",
		IsDlgButtonChecked(hDlg, IDC_TBBORDEREX));

	SetMyRegLong(NULL, "FlatTray",
		IsDlgButtonChecked(hDlg, IDC_FLATTRAY));

	SetMyRegLong(NULL, "FillTray",
		IsDlgButtonChecked(hDlg, IDC_FILLTRAY));

	SetMyRegLong(NULL, "FitClock",                         //390
		IsDlgButtonChecked(hDlg, IDC_FITCLOCK));           //390

	SetMyRegLong(NULL, "AlphaTaskbar",
		SendDlgItemMessage(hDlg, IDC_SPINBARTRANS, UDM_GETPOS, 0, 0));

	SetMyRegLong(NULL, "AlphaStartMenu",
		SendDlgItemMessage(hDlg, IDC_SPINMENUTRANS, UDM_GETPOS, 0, 0));
}

/*------------------------------------------------
  checked "Hide start button"
--------------------------------------------------*/
void OnStartBtnHide(HWND hDlg)
{
	BOOL b;

	b = IsDlgButtonChecked(hDlg, IDC_STARTBTNHIDE);
	EnableDlgItem(hDlg, IDC_STARTMENUCLOCK, b);
	SendPSChanged(hDlg);

	if(b) b = IsDlgButtonChecked(hDlg, IDC_STARTMENUCLOCK);
}

/*------------------------------------------------
  checked "Flat task switch"
--------------------------------------------------*/
void OnTaskSwitchFlat(HWND hDlg)
{
	BOOL b;

	b = IsDlgButtonChecked(hDlg, IDC_TASKSWITCHFLAT);
	EnableDlgItem(hDlg, IDC_TASKSWITCH_SEPARAT, b);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
  checked "Taskbar Border"
--------------------------------------------------*/
void OnTaskBarBorder(HWND hDlg)
{
	BOOL b;

	b = IsDlgButtonChecked(hDlg, IDC_TBBORDER);
	EnableDlgItem(hDlg, IDC_TBBORDEREX, b);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
  IE 4 or later ?
--------------------------------------------------*/
BOOL IsIE4(void)
{
	HWND hwnd;
	char classname[80];
	DWORD dw;

	dw = GetRegLong(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
		"ClassicShell", 0);
	if(dw) return TRUE;

	hwnd = FindWindow("Shell_TrayWnd", NULL);
	if(hwnd == NULL) return FALSE;
	hwnd = GetWindow(hwnd, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "ReBarWindow32") == 0)
			return TRUE;
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	return FALSE;
}

/*------------------------------------------------
  Is the tray icons "ToolbarWindow32" ?
--------------------------------------------------*/
BOOL IsTrayIconsToolbar(void)
{
	HWND hwnd;
	char classname[80];

	hwnd = FindWindow("Shell_TrayWnd", NULL);
	if(hwnd == NULL) return FALSE;
	hwnd = GetWindow(hwnd, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "TrayNotifyWnd") == 0)
			break;
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	if(hwnd == NULL) return FALSE;

	hwnd = GetWindow(hwnd, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "ToolbarWindow32") == 0)
			break;
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	if(hwnd == NULL) return FALSE;
	return TRUE;
}
