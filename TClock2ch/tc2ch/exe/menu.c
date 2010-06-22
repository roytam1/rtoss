/*-------------------------------------------------------------
  menu.c
  pop-up menu on right button click
  KAZUBON 1997-2001
---------------------------------------------------------------*/

#include "tclock.h"

// main.c
extern BOOL g_isTTBase;
HMENU g_hMenu = NULL;
static HMENU hPopupMenu = NULL;
void UpdateTimerMenu(HMENU hMenu);

/*------------------------------------------------
   when the clock is right-clicked
   show pop-up menu
--------------------------------------------------*/
void OnContextMenu(HWND hwnd, HWND hwndClicked, int xPos, int yPos)
{
	char s[80];

	UNREFERENCED_PARAMETER(hwndClicked);
	if(!g_hMenu)
	{
		g_hMenu = LoadMenu(GetLangModule(), MAKEINTRESOURCE(IDR_MENU));
		hPopupMenu = GetSubMenu(g_hMenu, 0);
		if (g_isTTBase)
		{
			RemoveMenu(hPopupMenu, IDC_EXIT, MF_BYCOMMAND);
		}
		SetMenuDefaultItem(hPopupMenu, 408, FALSE);

		if(GetVersion() & 0x80000000) // == 9x
		{
			DeleteMenu(hPopupMenu, 7, MF_BYPOSITION);
			DeleteMenu(hPopupMenu, 7, MF_BYPOSITION);
		}
	}

	UpdateTimerMenu(hPopupMenu);

	GetMyRegStr("SNTP", "Server", s, 80, "");
	if (!s[0])
	{
		EnableMenuItem(hPopupMenu, IDC_SYNCTIME, MF_BYCOMMAND|MF_GRAYED);
	}

	SetForegroundWindow98(hwnd);
	TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON,
		xPos, yPos, 0, hwnd, NULL);
}

/*------------------------------------------------
    command message
--------------------------------------------------*/
void OnTClockCommand(HWND hwnd, WORD wID, WORD wCode)
{
	switch(wID)
	{
		case IDC_SHOWDIR: // Show Directory
			ShellExecute(g_hwndMain, NULL, g_mydir, NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDC_SHOWPROP: // Show property
			MyPropertyDialog();
			return;
		case IDC_SHOWHELP: // Help
			MyHelp(hwnd, 0);
			return;
		case IDC_2CHHELP: // Help
			My2chHelp(hwnd, 0);
			return;
		case IDC_EXIT: // Exit
			PostMessage(g_hwndClock, WM_COMMAND, 102, 0);
			return;
		case IDC_SHOWCALENDER: // Calender
			DialogCalender(hwnd);
			return;
		case IDC_TIMER: // Timer
			DialogTimer(hwnd);
			return;
		case IDC_SYNCTIME: // Syncronize time
			StartSyncTime(hwnd, NULL, 0);
			return;
		case IDC_DATETIME:
		case IDC_CASCADE:
		case IDC_TILEHORZ:
		case IDC_TILEVERT:
		case IDC_MINALL:
		case IDC_TASKMAN:
		case IDC_TASKBARPROP:
		{
			HWND hwndTray;
			hwndTray = FindWindow("Shell_TrayWnd", NULL);
			if(hwndTray)
			{
				// Vista / 2008‚Å‚Í[¶‰E‚É•À‚×‚é]‚Æ[ã‰º‚É•À‚×‚é]‹t“]‚µ‚Ä‚¢‚é‚Ì‚Å‘Î‰ž‚·‚é
				OSVERSIONINFO os;
				os.dwOSVersionInfoSize = sizeof(os);
				if (GetVersionEx(&os) && os.dwMajorVersion >= 6)
				{
					if (wID == IDC_TILEHORZ)
						PostMessage(hwndTray, WM_COMMAND, (WPARAM)IDC_TILEVERT, 0);
					else if (wID == IDC_TILEVERT)
						PostMessage(hwndTray, WM_COMMAND, (WPARAM)IDC_TILEHORZ, 0);
					else
						PostMessage(hwndTray, WM_COMMAND, (WPARAM)wID, 0);
				}
				else	// VistaˆÈ‘O
					PostMessage(hwndTray, WM_COMMAND, (WPARAM)wID, 0);
			}
			return;
		}
		case IDC_HOTKEY0:
			ExecuteMouseFunction(hwnd, wCode, IDS_HOTKEY - IDS_LEFTBUTTON, 0);
			return;
		case IDC_HOTKEY1:
		case IDC_HOTKEY2:
		case IDC_HOTKEY3:
		case IDC_HOTKEY4:
			ExecuteMouseFunction(hwnd, -1, IDS_HOTKEY - IDS_LEFTBUTTON, wID - IDC_HOTKEY1 + 1);
			return;
	}

	if((IDC_STOPTIMER <= wID && wID < IDC_STOPTIMER + MAX_TIMER))
	{
		// stop timer
		StopTimer(hwnd, wID - IDC_STOPTIMER);
	}
	return;
}

/*------------------------------------------------
    update menu items to stop timers
--------------------------------------------------*/
void UpdateTimerMenu(HMENU hMenu)
{
	int i, j, len;
	UINT id;
	char s[80];

	len = GetMenuItemCount(hMenu);
	for(i = 0; i < len; i++)
	{
		id = GetMenuItemID(hMenu, i);
		if(IDC_STOPTIMER <= id && id < IDC_STOPTIMER + MAX_TIMER)
		{
			DeleteMenu(hMenu, id, MF_BYCOMMAND);
			len = GetMenuItemCount(hMenu);
			i--;
		}
	}

	for(i = 0; i < len; i++)
	{
		id = GetMenuItemID(hMenu, i);
		if(id == IDC_TIMER) break;
	}
	if(i < len) i++;

	for(j = 0; ; i++, j++)
	{
		if(GetTimerInfo(s, j) == 0) break;
		InsertMenu(hMenu, i, MF_BYPOSITION|MF_STRING,
			IDC_STOPTIMER + j, s);
	}
}
