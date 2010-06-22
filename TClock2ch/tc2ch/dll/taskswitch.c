/*-------------------------------------------
  taskswitch.c
    Customize Task Switcher
    Kazubon 1999
---------------------------------------------*/
#include "tcdll.h"
#include <commctrl.h>

#ifndef TCS_FLATBUTTONS
#define TCS_FLATBUTTONS         0x0008
#endif
#ifndef TCS_HOTTRACK
#define TCS_HOTTRACK            0x0040
#endif

#ifndef TCS_EX_FLATSEPARATORS
#define TCS_EX_FLATSEPARATORS	0x0001
#endif

#ifndef TCM_SETEXTENDEDSTYLE
#define TCM_SETEXTENDEDSTYLE	(4916)
#define TabCtrl_SetExtendedStyle(hwnd, dw)	\
SendMessage((hwnd), TCM_SETEXTENDEDSTYLE, 0, dw)
#endif

#ifndef TCM_GETEXTENDEDSTYLE
#define TCM_GETEXTENDEDSTYLE	(4917)
#define TabCtrl_GetExtendedStyle(hwnd)		\
SendMessage((hwnd), TCM_GETEXTENDEDSTYLE, 0, 0)
#endif

#ifndef TCM_SETITEMSIZE
#define TCM_SETITEMSIZE         (TCM_FIRST + 41)
#endif

#ifndef TB_SETBUTTONSIZE
#define TB_SETBUTTONSIZE        (WM_USER + 31)
#endif

#define TB_SETEXTENDEDSTYLE     (WM_USER + 84)  // For TBSTYLE_EX_*
#define TB_GETEXTENDEDSTYLE     (WM_USER + 85)  // For TBSTYLE_EX_*
#define BTNS_SHOWTEXT   0x0040              // ignored unless TBSTYLE_EX_MIXEDBUTTONS is set
#define TBSTYLE_EX_MIXEDBUTTONS             0x00000008


extern HANDLE hmod;

LRESULT CALLBACK WndProcTab(HWND, UINT, WPARAM, LPARAM);
void EndTaskSwitch(void);
void EndTaskBarBorder(void);
static WNDPROC oldWndProcTab = NULL;
static HWND hwndTab = NULL;
static HWND hwndBar = NULL;
static HWND hwndTabTaskSwitch = NULL;
static LONG_PTR oldstyle;
static LONG_PTR oldBarStyle;
static DWORD oldExstyle;
static DWORD oldBarBtnSize = 0;
static DWORD oldTBStyle;
static BOOL bTaskSwitchFlat = FALSE;
static BOOL bTaskSwitchIcons = FALSE;
static BOOL bTBBorder = FALSE;
static WORD OldTaskWidth;

/*--------------------------------------------------
   initialize
----------------------------------------------------*/
void InitTaskSwitch(HWND hwndClock)
{
	HANDLE hwnd, hwndTaskSwitch;
	char classname[80];

	EndTaskSwitch();

	bTaskSwitchFlat = GetMyRegLong(NULL, "TaskSwitchFlat", FALSE);
	bTaskSwitchIcons = GetMyRegLong(NULL, "TaskSwitchIconsOnly", FALSE);

	if(!bTaskSwitchFlat && !bTaskSwitchIcons)
		return;

	// get window handle of MSTaskSwWClass
	hwndTaskSwitch = NULL;
	hwnd = GetParent(hwndClock);  // TrayNotifyWnd
	hwnd = GetParent(hwnd);       // Shell_TrayWnd
	if(hwnd == NULL) return;
	hwnd = GetWindow(hwnd, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "ReBarWindow32") == 0)
		{
			hwnd = GetWindow(hwnd, GW_CHILD);
			while(hwnd)
			{
				GetClassName(hwnd, classname, 80);
				if(lstrcmpi(classname, "MSTaskSwWClass") == 0)
				{
					hwndTaskSwitch = hwnd;
					break;
				}
				if(lstrcmpi(classname, "ToolbarWindow32") == 0)
				{
					hwndTaskSwitch = hwnd;
					break;
				}
				hwnd = GetWindow(hwnd, GW_HWNDNEXT);
			}
		}
		else if(lstrcmpi(classname, "MSTaskSwWClass") == 0)
		{
			hwndTaskSwitch = hwnd;
			break;
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	if(hwndTaskSwitch == NULL)
		return;
	hwndTab = GetWindow(hwndTaskSwitch, GW_CHILD);
	if(hwndTab == NULL)
		return;
	oldstyle = GetWindowLongPtr(hwndTab, GWL_STYLE);
	if (bTaskSwitchFlat)
	{
		if (bWinXP)
		{
			// WinXPはタブではなくツールバー
			SetWindowLongPtr(hwndTab, GWL_STYLE, oldstyle|TBSTYLE_FLAT);
		}
		else
		{
			SetWindowLongPtr(hwndTab, GWL_STYLE, oldstyle|TCS_FLATBUTTONS|TCS_HOTTRACK);
//			SetWindowLongPtr(hwndTab, GWL_STYLE, (oldstyle&(~TCS_BUTTONS))|TCS_BOTTOM|TCS_HOTTRACK);
		}
	}
	oldExstyle = TabCtrl_GetExtendedStyle(hwndTab);
	if(GetMyRegLong(NULL, "TaskSwitchSeparators", FALSE))
	{
		TabCtrl_SetExtendedStyle(hwndTab, oldExstyle|TCS_EX_FLATSEPARATORS);
	}
	if(bTaskSwitchIcons)
	{
		if(!bWinXP)
		{
			OldTaskWidth = LOWORD(SendMessage(hwndTab, TCM_SETITEMSIZE, 0, MAKELPARAM(23, 23)));
		}else
		{
			oldTBStyle = (DWORD)SendMessage(hwndTab, TB_GETEXTENDEDSTYLE, 0, 0);
			SendMessage(hwndTab, TB_SETEXTENDEDSTYLE, 0, oldTBStyle|TBSTYLE_EX_MIXEDBUTTONS);
		}
	}

	oldWndProcTab = (WNDPROC)GetWindowLongPtr(hwndTab, GWLP_WNDPROC);
	SubclassWindow(hwndTab, WndProcTab);
}

/*--------------------------------------------------
    undo
----------------------------------------------------*/
void EndTaskSwitch(void)
{
	if(hwndTab && IsWindow(hwndTab))
	{
#if ENABLE_CHECK_SUBCLASS_NESTING
		if(oldWndProcTab && (WNDPROC)WndProcTab == (WNDPROC)GetWindowLongPtr(hwndTab, GWLP_WNDPROC))
#else
		if(oldWndProcTab)
#endif
			SubclassWindow(hwndTab, oldWndProcTab);
		oldWndProcTab = NULL;
		SetWindowLongPtr(hwndTab, GWL_STYLE, oldstyle);
		TabCtrl_SetExtendedStyle(hwndTab, oldExstyle);
		if(bTaskSwitchIcons)
		{
			if(!bWinXP)
				SendMessage(hwndTab, TCM_SETITEMSIZE, 0, MAKELPARAM(OldTaskWidth, 23));
			else
				SendMessage(hwndTab, TB_SETEXTENDEDSTYLE, 0, oldTBStyle);
		}
	}
	hwndTab = NULL;
}

/*------------------------------------------------
   subclass procedure of SysTabControl32
--------------------------------------------------*/
LRESULT CALLBACK WndProcTab(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case TCM_SETITEMSIZE:
			if(bTaskSwitchIcons)
			{
				lParam = MAKELPARAM(23, HIWORD(lParam));
			}else
			{
				if(LOWORD(lParam)-8 >= 22)
					lParam = MAKELPARAM(LOWORD(lParam)-8, HIWORD(lParam));
			}
			break;
		case TCM_INSERTITEM:
			PostMessage(GetParent(hwnd), WM_SIZE, SIZE_RESTORED, 0);
			break;
		case TCM_DELETEITEM:
			PostMessage(GetParent(hwnd), WM_SIZE, SIZE_RESTORED, 0);
			break;
		case TB_SETBUTTONSIZE:
			if(bTaskSwitchIcons)
			{
				lParam = MAKELPARAM(23, HIWORD(lParam));
			}else
			{
				if(LOWORD(lParam)-8 >= 22)
					lParam = MAKELPARAM(LOWORD(lParam)-8, HIWORD(lParam));
			}
			break;
		case TB_INSERTBUTTON:
			PostMessage(GetParent(hwnd), WM_SIZE, SIZE_RESTORED, 0);
			break;
		case TB_DELETEBUTTON:
			PostMessage(GetParent(hwnd), WM_SIZE, SIZE_RESTORED, 0);
			break;
	}
	return CallWindowProc(oldWndProcTab, hwnd, message, wParam, lParam);
}

/*----------------------------------------------------
   Taskbar を薄く。
------------------------------------------------------*/
void InitTaskBarBorder(HWND hwndClock)
{
	HANDLE hwnd, hwndTaskSwitch;
	char classname[80];
	//HWND hwnd;
	STYLESTRUCT ss;

	EndTaskBarBorder();

	if(!GetMyRegLong(NULL, "TaskBarBorder", FALSE))
		return;
	bTBBorder = GetMyRegLong(NULL, "TaskBarBorderEx", FALSE);

	hwndTaskSwitch = NULL;
	hwnd = GetParent(hwndClock);  // TrayNotifyWnd
	hwndBar = GetParent(hwnd);       // Shell_TrayWnd
	if(hwndBar == NULL) return;
	hwnd = GetWindow(hwndBar, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "ReBarWindow32") == 0)
		{
			hwnd = GetWindow(hwnd, GW_CHILD);
			while(hwnd)
			{
				GetClassName(hwnd, classname, 80);
				if(lstrcmpi(classname, "MSTaskSwWClass") == 0)
				{
					hwndTaskSwitch = hwnd;
					break;
				}
				hwnd = GetWindow(hwnd, GW_HWNDNEXT);
			}
		}
		else if(lstrcmpi(classname, "MSTaskSwWClass") == 0)
		{
			hwndTaskSwitch = hwnd;
			break;
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	if(hwndTaskSwitch == NULL)
		return;
	hwndTabTaskSwitch = GetWindow(hwndTaskSwitch, GW_CHILD);
	if(hwndTabTaskSwitch == NULL)
		return;
	oldBarStyle = GetWindowLongPtr(hwndBar, GWL_STYLE);
	SetWindowLongPtr(hwndBar, GWL_STYLE, oldBarStyle&(~WS_THICKFRAME)&(~WS_BORDER));
	if(bTBBorder)
	{
		if(oldBarBtnSize == 0)
		{
			if (bWinXP)
				oldBarBtnSize = (DWORD)SendMessage(hwndTabTaskSwitch, TB_GETPADDING, 0, 0);
			else
				oldBarBtnSize = MAKELONG(3,8);
		}
		if (bWinXP)
			SendMessage(hwndTabTaskSwitch, TB_SETPADDING, 0, MAKELPARAM(LOWORD(oldBarBtnSize),0));
		else
			SendMessage(hwndTabTaskSwitch, TCM_SETPADDING, 0, MAKELPARAM(LOWORD(oldBarBtnSize),0));
	}
	ss.styleOld = (DWORD)oldBarStyle;
	ss.styleNew = (DWORD)oldBarStyle&(~WS_THICKFRAME)&(~WS_BORDER);
	SendMessage(hwndBar, WM_STYLECHANGED, (WPARAM)(GWL_STYLE|GWL_EXSTYLE), (LPARAM)&ss);
	SendMessage(hwndBar, WM_SYSCOLORCHANGE, 0, 0);
	SendMessage(hwndBar, WM_SIZE, SIZE_RESTORED, 0);
	SetWindowPos(hwndBar, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

void EndTaskBarBorder(void)
{
	if(hwndTabTaskSwitch && IsWindow(hwndTabTaskSwitch) && oldBarBtnSize && bTBBorder)
	{
		if (bWinXP)
			SendMessage(hwndTabTaskSwitch, TB_SETPADDING, 0, MAKELPARAM(LOWORD(oldBarBtnSize),HIWORD(oldBarBtnSize)));
		else
			SendMessage(hwndTabTaskSwitch, TCM_SETPADDING, 0, MAKELPARAM(LOWORD(oldBarBtnSize),HIWORD(oldBarBtnSize)));
	}
	if(hwndBar && IsWindow(hwndBar))
	{
		STYLESTRUCT ss;
		SetWindowLongPtr(hwndBar, GWL_STYLE, oldBarStyle);
		ss.styleOld = (DWORD)oldBarStyle&(~WS_THICKFRAME)&(~WS_BORDER);
		ss.styleNew = (DWORD)oldBarStyle;
		SendMessage(hwndBar, WM_STYLECHANGED, (WPARAM)(GWL_STYLE|GWL_EXSTYLE), (LPARAM)&ss);
		SendMessage(hwndBar, WM_SYSCOLORCHANGE, 0, 0);
		SendMessage(hwndBar, WM_SIZE, SIZE_RESTORED, 0);
		SetWindowPos(hwndBar, NULL, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	}
	hwndBar = NULL; hwndTabTaskSwitch = NULL;
}
