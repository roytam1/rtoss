/*-------------------------------------------
  calendar.c
    Calendar
    Two_toNe 2002
---------------------------------------------*/

#include "tclock.h"

INT_PTR CALLBACK DlgProcCalender(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// propsheet.c
void SetMyDialgPos(HWND hwnd);

/*------------------------------------------------
   open "Calender" dialog
--------------------------------------------------*/
void DialogCalender(HWND hwnd)
{
	INITCOMMONCONTROLSEX icc;

	UNREFERENCED_PARAMETER(hwnd);
	if(g_hDlgCalender && IsWindow(g_hDlgCalender))
		;
	else
	{
		OSVERSIONINFO os;
		os.dwOSVersionInfoSize = sizeof(os);
		if (GetVersionEx(&os) && os.dwMajorVersion >= 6)	// isVista or later
		{
			if (!FindVistaCalenderWindow())
				PostMessage(g_hwndClock, CLOCKM_VISTACALENDAR, 1, 0);
		}
		else
		{
			icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icc.dwICC = ICC_DATE_CLASSES;
			InitCommonControlsEx(&icc);
			g_hDlgCalender = CreateDialog(GetLangModule(), MAKEINTRESOURCE(IDD_CALENDAR),
				NULL, DlgProcCalender);
		}
	}
	SetForegroundWindow98(g_hDlgCalender);
}

/*------------------------------------------------
   dialog procedure of "Calender" dialog
--------------------------------------------------*/
INT_PTR CALLBACK DlgProcCalender(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch(message)
	{
		case WM_INITDIALOG:
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)g_hIconTClock);
			SetMyDialgPos(hDlg);
			return TRUE;
		case WM_CLOSE:
			g_hDlgCalender = NULL;
			DestroyWindow(hDlg);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDCANCEL:
					g_hDlgCalender = NULL;
					DestroyWindow(hDlg);
					return TRUE;
			}
			break;
	}
	return FALSE;
}



// Vistaのカレンダーが表示されていればそのハンドルを返す
HWND FindVistaCalenderWindow(void)
{
	return FindWindowEx(FindWindow("ClockFlyoutWindow", NULL), NULL, "DirectUIHWND", "");
}

