/*-----------------------------------------------------
  main.c
   API, hook procedure
   KAZUBON 1997-2001
-------------------------------------------------------*/

#include "tcdll.h"

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
void InitClock(HWND hwnd);

/*------------------------------------------------
  shared data among processes
--------------------------------------------------*/
extern HHOOK hhook;
extern HWND hwndTClockMain;
extern HWND hwndTaskbar;
extern HWND hwndHook;
extern BOOL bHookEnable;

/*------------------------------------------------
  globals
--------------------------------------------------*/
extern HANDLE hmod;
extern WNDPROC oldWndProc;
extern BOOL bWin2000;

extern int nAlphaStartMenu;

/*------------------------------------------------
  entry point of this DLL
--------------------------------------------------*/
//BOOL WINAPI _DllMainCRTStartup(HANDLE hModule, DWORD dwFunction, LPVOID lpNot)
int WINAPI DllMain(HANDLE hModule, DWORD dwFunction, LPVOID lpNot)
{
	UNREFERENCED_PARAMETER(lpNot);
	UNREFERENCED_PARAMETER(dwFunction);

	hmod = hModule;
	DisableThreadLibraryCalls(hModule);
	return TRUE;
}

/*------------------------------------------------
   API: install my hook
--------------------------------------------------*/
void WINAPI HookStart(HWND hwnd)
{
	HWND hwndTray;
	DWORD ThreadID;

	hwndTClockMain = hwnd;

	// find the taskbar
	hwndTaskbar = FindWindow("Shell_TrayWnd", "");
	if(!hwndTaskbar)
	{
		SendMessage(hwnd, WM_USER+1, 0, 1);
		return;
	}

	// get thread ID of taskbar (explorer)
	// Specal thanks to T.Iwata.
	ThreadID = GetWindowThreadProcessId(hwndTaskbar, NULL);
	if(!ThreadID)
	{
		SendMessage(hwnd, WM_USER+1, 0, 2);
		return;
	}

	// install an hook to thread of taskbar
	bHookEnable = TRUE;
	hhook = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc, hmod, ThreadID);
	if(!hhook)
	{
		SendMessage(hwnd, WM_USER+1, 0, 3);
		return;
	}

	// find the clock window
	hwndTray = FindWindowEx(hwndTaskbar, NULL, "TrayNotifyWnd", "");
	hwndHook = FindWindowEx(hwndTray, NULL, "TrayClockWClass", NULL);
	if (hwndHook) SendMessage(hwndHook, WM_NULL, 0, 0);

	// refresh the taskbar
	SendMessage(hwndTaskbar, WM_SIZE, SIZE_RESTORED, 0);
}

/*------------------------------------------------
  API: uninstall my hook
--------------------------------------------------*/
void WINAPI HookEnd(void)
{
	bHookEnable = FALSE;

	// force the clock to end cunstomizing
	if(hwndHook && IsWindow(hwndHook))
		SendMessage(hwndHook, WM_COMMAND, 102, 0);
	// uninstall my hook
	if(hhook != NULL)
		UnhookWindowsHookEx(hhook); hhook = NULL;

	// refresh the clock
	if(hwndHook && IsWindow(hwndHook))
		PostMessage(hwndHook, WM_TIMER, 0, 0);
	hwndHook = NULL;

	// refresh the taskbar
	if(hwndTaskbar)
	{
//		SendMessage(hwndTaskbar, WM_SIZE, SIZE_RESTORED, 0);
		PostMessage(hwndTaskbar, WM_SIZE, SIZE_RESTORED, 0);
		InvalidateRect(hwndTaskbar, NULL, TRUE);
	}
}

/*------------------------------------------------
  hook procedure
--------------------------------------------------*/
LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPCWPSTRUCT pcwps = (LPCWPSTRUCT)lParam;
	char classname[80];

	if(nCode == HC_ACTION && pcwps && pcwps->hwnd)
	{
		if(bHookEnable == TRUE &&
			oldWndProc == NULL &&
			pcwps->hwnd == hwndHook)
		{
			// initialize  cf. wndproc.c
			InitClock(pcwps->hwnd);
		}

		if(bWin2000 && nAlphaStartMenu < 255)
		{
			if(GetClassName(pcwps->hwnd, classname, 80) > 0 &&
				(lstrcmpi(classname, "BaseBar") == 0 ||
				 lstrcmpi(classname, "DV2ControlHost") == 0))
			{
				SetLayeredStartMenu(pcwps->hwnd);
			}
		}
	}
	return CallNextHookEx(hhook, nCode, wParam, lParam);
}

