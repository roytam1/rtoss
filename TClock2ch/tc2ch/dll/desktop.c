/*-------------------------------------------
  desktop.c
    Customize Desktop Icon View
    Two_toNe 2002
---------------------------------------------*/
#include "tcdll.h"

#ifndef LVS_SMALLICON
#define LVS_SMALLICON           0x0002
#endif

static HWND hwndDesktop = NULL;
static LONG oldstyle = 0;

static VOID CALLBACK ScreenFlush(HWND hWnd, UINT uMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(dwTime);
	KillTimer(NULL, nIDEvent);
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, NULL, NULL);
}

#if 1	// modify by 505
//#define  LVM_SETEXTENDEDLISTVIEWSTYLE    0x1036
//#define  LVM_GETEXTENDEDLISTVIEWSTYLE    0x1037
void SetDesktopIcons(void)
{
	BOOL b;
	HWND hwnd;
	LONG s;

	b = GetMyRegLong(NULL, "DesktopIconList", FALSE);
	if (!b) {
		EndDesktopIcons();
		return;
	}

	if (!hwndDesktop) {
		hwnd = FindWindow("Progman", "Program Manager");
		hwnd = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
		hwnd = FindWindowEx(hwnd, NULL, "SysListView32", NULL);
		if (!hwnd) return;
		hwndDesktop = hwnd;
	}
#if 0
	//LVS_ICON, LVS_LIST, LVS_REPORT, LVS_SMALLICON
//	s = SendMessage(hwndDesktop, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
//	s = SendMessage(hwndDesktop, LVM_SETEXTENDEDLISTVIEWSTYLE, mask, s | mask);
	s = GetWindowLong(hwndDesktop, GWL_STYLE);
	if ((s & LVS_REPORT) == 0) {
		SetWindowLong(hwndDesktop, GWL_STYLE, s | LVS_REPORT);
		oldstyle = s;
	}
#else
	s = GetWindowLong(hwndDesktop, GWL_STYLE);
	if (b == 1)
	{
		if ((s & LVS_REPORT) != 0) {
			s ^= LVS_REPORT;
		}
		if ((s & LVS_SMALLICON) == 0) {
			//s ^= LVS_NOSCROLL;
			//s ^= LVS_NOCOLUMNHEADER;
			SetWindowLong(hwndDesktop, GWL_STYLE, s | LVS_SMALLICON);
			oldstyle = s;
		}
	}
	else if (b == 2)
	{
		// è⁄ç◊ï\é¶
		if ((s & LVS_SMALLICON) != 0) {
			s ^= LVS_SMALLICON;
		}
		if ((s & LVS_REPORT) == 0) {
			oldstyle = s;
			s |= LVS_REPORT;
			//s ^= LVS_NOSCROLL;
			//s ^= LVS_NOCOLUMNHEADER;
			//s ^= LVS_SORTASCENDING;
			//s ^= LVS_SORTDESCENDING;
			//s ^= LVS_NOSORTHEADER;
			//s ^= LVS_OWNERDRAWFIXED;
			SetWindowLong(hwndDesktop, GWL_STYLE, s);
			SetTimer(NULL, 0, 10, ScreenFlush);
		}
	}	
#endif
}
#else	// modify by 505
/*--------------------------------------------------
   initialize
----------------------------------------------------*/
void SetDesktopIcons(void)
{
	BOOL b;
	HWND hwnd;

	b = GetMyRegLong(NULL, "DesktopIconList", FALSE);
	if(!b)
	{
		EndDesktopIcons();
		return;
	}

	if (!hwndDesktop)
	{
		hwnd = FindWindow("Progman", "Program Manager");
		hwnd = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
		hwnd = FindWindowEx(hwnd, NULL, "SysListView32", NULL);
		if(!hwnd) return;
		hwndDesktop = hwnd;
		oldstyle = GetWindowLong(hwndDesktop, GWL_STYLE);
		SetWindowLong(hwndDesktop, GWL_STYLE, oldstyle|LVS_SMALLICON);
	}

	if (!(GetWindowLong(hwndDesktop, GWL_STYLE)&LVS_SMALLICON))
	{
		oldstyle = GetWindowLong(hwndDesktop, GWL_STYLE);
		SetWindowLong(hwndDesktop, GWL_STYLE, oldstyle|LVS_SMALLICON);
	}
}
#endif	// modify by 505

/*--------------------------------------------------
    reset desktop icons
----------------------------------------------------*/
void EndDesktopIcons(void)
{
	if(hwndDesktop)
	{
		SetWindowLong(hwndDesktop, GWL_STYLE, oldstyle);
	}
	hwndDesktop = NULL;
}
