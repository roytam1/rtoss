/*-------------------------------------------------------------
  main.c
  WinMain, window procedure, and functions for initializing
  KAZUBON 1997-2001
---------------------------------------------------------------*/

#include "tclock.h"
#include <winver.h>

#ifndef RTL_NUMBER_OF
#define RTL_NUMBER_OF(x) (sizeof(x)/sizeof(x[0]))
#endif

// Globals
HINSTANCE g_hInst;           // instance handle
HINSTANCE g_hInstResource;   // instance handle of language module
HWND      g_hwndMain = 0;    // main window
HWND      g_hwndClock;       // clock window
HWND      g_hwndPropDlg;     // property sheet window
HWND      g_hDlgTimer;       // timer dialog
HWND      g_hDlgCalender;    // calender dialog
HICON     g_hIconTClock, g_hIconPlay, g_hIconStop, g_hIconDel;
                             // icons to use frequently
char      g_mydir[MAX_PATH]; // path to tclock.exe
char      g_langdllname[MAX_PATH];  // language dll name
BOOL      g_bIniSetting = FALSE;
char      g_inifile[MAX_PATH];

#if defined(TCLOCK2CH_TTBASE)
#pragma message("use TCLOCK2CH_TTBASE")
#define TTB_LABEL(s) s
BOOL g_isTTBase = TRUE;
#define DO_WS_AGGRESSIVE()
#else
#pragma message("not use TCLOCK2CH_TTBASE")
#define TTB_LABEL(s) 0
BOOL g_isTTBase = FALSE;
// スワップアウトさせる /WS:AGGRESSIVE
#define DO_WS_AGGRESSIVE() \
          SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
#endif


#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL  0x020A
#endif
#ifndef WM_MENURBUTTONUP
#define WM_MENURBUTTONUP 0x0122
#endif

#define VERSIONM 0x20003
#define VERSIONL 0x0000



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

char szClassName[] = "TClockMainClass"; // window class name
char szWindowText[] = "TClock";         // caption of the window

static BOOL bMenuOpened = FALSE;
static BOOL bDestroy = FALSE;

void CheckCommandLine(HWND hwnd);
static void OnTimerMain(HWND hwnd);
static void InitError(int n);
static BOOL CheckTCDLL(void);
static BOOL CheckDLL(char *fname);
static void CheckRegistry(void);
static void InitDesktopIconText(void);
static void EndDesktopIconText(void);
void SetDesktopIconTextBk(BOOL bNoTrans);
static BOOL IsUserAdmin(void);
static BOOL IsWow64(void);
static BOOL ImmDisableIME(DWORD idThread);
static BOOL AddMessageFilters(void);

static UINT s_uTaskbarRestart = 0;
static BOOL bStartTimer = FALSE;
static int nCountFindingClock = -1;


// XButton Messages
#define WM_XBUTTONDOWN 0x020B
#define WM_XBUTTONUP   0x020C

// menu.c
extern HMENU g_hMenu;

// alarm.c
extern BOOL bPlayingNonstop;


/*-------------------------------------------------------
    mouse function list
---------------------------------------------------------*/
static MOUSE_FUNC_INFO mouse_func_list[] = {
	{	MOUSEFUNC_NONE,			IDS_NONE,			TTB_LABEL("None")	},
	{	MOUSEFUNC_DATETIME,		IDS_PROPDATE,		TTB_LABEL("Time/Date Properties")	},
	{	MOUSEFUNC_EXITWIN,		IDS_EXITWIN,		TTB_LABEL("Exit Windows")	},
	{	MOUSEFUNC_RUNAPP,		IDS_RUN,			TTB_LABEL("Execute Program With Path")	},
	{	MOUSEFUNC_MINALL,		IDS_ALLMIN,			TTB_LABEL("Minimize All Windows")	},
	{	MOUSEFUNC_SCREENSAVER,	IDS_SCREENSAVER,	TTB_LABEL("Test Screensaver")	},
	{	MOUSEFUNC_KYU,			IDS_KYU,			TTB_LABEL("Kyu!")	},
	{	MOUSEFUNC_SYNCTIME,		IDS_SYNCTIME,		TTB_LABEL("Synchronize Clock with Time Server")	},
	{	MOUSEFUNC_TIMER,		IDS_TIMER,			TTB_LABEL("Timer Properties")	},
	{	MOUSEFUNC_CLIPBOARD,	IDS_MOUSECOPY,		TTB_LABEL("Copy To Clipboard")	},
	{	MOUSEFUNC_OPENFILE,		IDS_OPENFILE,		TTB_LABEL("Open File")	},
	{	MOUSEFUNC_DELRECDOCS,	IDS_DELRECDOCS,		TTB_LABEL("Clear Recent Document")	},
	{	MOUSEFUNC_PROPERTY,		IDS_PROPERTY,		TTB_LABEL("TClock Properties")	},
	{	MOUSEFUNC_CALENDAR,		IDS_CALENDAR,		TTB_LABEL("Calender")	},
	{	MOUSEFUNC_STARTMENU,	IDS_STARTMNU,		TTB_LABEL("Start Menu")	},
	{	MOUSEFUNC_TASKSW,		IDS_TASKSW,			TTB_LABEL("Task Switch")	},
	{	MOUSEFUNC_SHOWDESK,		IDS_SHOWDESK,		TTB_LABEL("Show Desktop")	},
	{	MOUSEFUNC_LOCKPC,		IDS_LOCKPC,			TTB_LABEL("Lock PC(XP or later)")	},
	{	MOUSEFUNC_MENU,			IDS_MCMENU,			TTB_LABEL("Default Menu")	},
	{	MOUSEFUNC_CHANGECONF,	IDS_CHANGECONF,		TTB_LABEL("Change Set of Properties")	},
	{	MOUSEFUNC_VOLMUTE,		IDS_VOLMUTE,		TTB_LABEL("Mute Audio Output")	},
	{	MOUSEFUNC_VOLSET,		IDS_VOLSET,			TTB_LABEL("Set Audio Output Volume")	},
	{	MOUSEFUNC_VOLUD,		IDS_VOLUD,			TTB_LABEL("Up/Down Audio Output Volume")	},
	{	MOUSEFUNC_MONOFF,		IDS_MONOFF,			TTB_LABEL("Moniter Off")	},
	{	MOUSEFUNC_CDOPEN,		IDS_CDOPEN,			TTB_LABEL("CD Drive Open")	},
	{	MOUSEFUNC_CDCLOSE,		IDS_CDCLOSE,		TTB_LABEL("CD Drive Close")	},
	{	MOUSEFUNC_NETINIT,		IDS_NETINIT,		TTB_LABEL("Network Initialization")	},
	{	MOUSEFUNC_DELUS,		IDS_DELUS,			TTB_LABEL("Delete User Strings")	},
	{	MOUSEFUNC_FILELIST,		IDS_FILELIST,		TTB_LABEL("File List Menu")	},
};

MOUSE_FUNC_INFO *GetMouseFuncList(void)
{
	return mouse_func_list;
}
int GetMouseFuncCount(void)
{
	return sizeof(mouse_func_list) / sizeof(MOUSE_FUNC_INFO);
}

/*-------------------------------------------------------
  dynamic 'tcdll.dll' loader for TTBase
---------------------------------------------------------*/
#if defined(TCLOCK2CH_TTBASE)
static void (WINAPI *pHookStart)(HWND hwnd);
static void (WINAPI *pHookEnd)(void);
static HINSTANCE g_hInstHookDLL = 0;
void WINAPI HookStart(HWND hwnd)
{
	HINSTANCE hinst;
	char fname[MAX_PATH];
	strcpy(fname, g_mydir); add_title(fname, "tcdll.dll");
	hinst = LoadLibrary(fname);
	if (!hinst) return;
	*(FARPROC *)&pHookStart = GetProcAddress(hinst, "HookStart");
	*(FARPROC *)&pHookEnd   = GetProcAddress(hinst, "HookEnd");
	if (!pHookStart || !pHookEnd)
	{
		FreeLibrary(hinst);
		return;
	}
	g_hInstHookDLL = hinst;
	pHookStart(hwnd);
}
void WINAPI HookEnd(void)
{
	if (g_hInstHookDLL)
	{
		pHookEnd();
		FreeLibrary(g_hInstHookDLL);
		g_hInstHookDLL = 0;
	}
}
#endif



/*-------------------------------------------
   main routine
---------------------------------------------*/
static UINT WINAPI TclockExeMain(void)
{
	MSG msg;
	WNDCLASS wndclass;
	HWND hwnd;

	// check wow64
	if (IsWow64()) { 
		MessageBox(NULL, "64bit環境ではx64バイナリを使用する必要があります。",
			NULL, MB_ICONERROR);
		return 1; 
	}

	// for Vista 
	if (IsUserAdmin()) { AddMessageFilters(); }

	// not to execute the program twice
	hwnd = FindWindow(szClassName, szWindowText);
	if(hwnd != NULL)
	{
		CheckCommandLine(hwnd);
		return 1;
	}

	// get the path where .exe is positioned
	GetModuleFileName(g_hInst, g_mydir, MAX_PATH);
	del_title(g_mydir);

	CheckRegistry();

	if(!CheckTCDLL()) { return 1; }

	g_hInstResource = LoadLanguageDLL(g_langdllname);
	if(g_hInstResource == NULL) { return 1; }

	InitCommonControls();

	// Message of the taskbar recreating
	// Special thanks to Mr.Inuya
	s_uTaskbarRestart = RegisterWindowMessage("TaskbarCreated");

	g_hIconTClock = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
	g_hIconPlay = LoadImage(g_hInst, MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON,
		16, 16, LR_DEFAULTCOLOR);
	g_hIconStop = LoadImage(g_hInst, MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON,
		16, 16, LR_DEFAULTCOLOR);
	g_hIconDel = LoadImage(g_hInst, MAKEINTRESOURCE(IDI_DEL), IMAGE_ICON,
		16, 16, LR_DEFAULTCOLOR);

	g_hwndPropDlg = g_hDlgTimer = NULL;

	// register a window class
	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = g_hInst;
	wndclass.hIcon         = g_hIconTClock;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = szClassName;
	RegisterClass(&wndclass);

	// create a hidden window
	DO_WS_AGGRESSIVE();
	hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, szClassName, szWindowText,
		0, CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		NULL, NULL, g_hInst, NULL);
	//ShowWindow(hwnd, SW_SHOW);
	//UpdateWindow(hwnd);

	if(OleInitialize(NULL) != S_OK){
		MessageBox(NULL,"OLEの初期化に失敗しました。", NULL, MB_ICONERROR);
	}

	g_hwndMain = hwnd;

	CheckCommandLine(hwnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(g_hwndPropDlg && IsWindow(g_hwndPropDlg)
			&& IsDialogMessage(g_hwndPropDlg, &msg))
			;
		else if(g_hDlgTimer && IsWindow(g_hDlgTimer)
			&& IsDialogMessage(g_hDlgTimer, &msg))
			;
		else if(g_hDlgCalender && IsWindow(g_hDlgCalender)
			&& IsDialogMessage(g_hDlgCalender, &msg))
			;
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if(g_hMenu) DestroyMenu(g_hMenu);

	if(g_hInstResource) FreeLibrary(g_hInstResource);

	UnregisterClass(szClassName, g_hInst);	/* for TTBASE */

	return (UINT)msg.wParam;
}

/*-------------------------------------------
   Command Line Option
   /prop : Show TClock Properties
   /exit : Exit TClock
---------------------------------------------*/
void CheckCommandLine(HWND hwnd)
{
#if defined(TCLOCK2CH_TTBASE)
	UNREFERENCED_PARAMETER(hwnd);
#else
	char *p;
	p = GetCommandLine();
	while(*p)
	{
		if(*p == '/')
		{
			p++;
			if(_strnicmp(p, "prop", 4) == 0)
			{
				PostMessage(hwnd, WM_COMMAND, IDC_SHOWPROP, 0);
				p += 4;
			}
			else if(_strnicmp(p, "exit", 4) == 0)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
				p += 4;
			}
			else if(_strnicmp(p, "nowait", 4) == 0)
			{
				KillTimer(hwnd, IDTIMER_START);
				SetTimer(hwnd, IDTIMER_START, 100, NULL);
				p += 6;
			}
			else if(_strnicmp(p, "idle", 4) == 0)
			{
				HANDLE op = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());
				SetPriorityClass(op, IDLE_PRIORITY_CLASS);
				Sleep(10);
				p += 4;
			}
		}
		p++;
	}
#endif
}

/*-------------------------------------------
   the window procedure
---------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd,	UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			int nDelay;
			bDestroy = FALSE;
			InitAlarm();  // initialize alarms
			InitFormat(); // initialize a Date/Time format
			nDelay = GetMyRegLong("", "DelayStart", 0);
			if(nDelay > 0)
			{
				SetTimer(hwnd, IDTIMER_START, nDelay * 1000, NULL);
				bStartTimer = TRUE;
			}
			else SendMessage(hwnd, WM_TIMER, IDTIMER_START, 0);
			InitSyncTime(hwnd);
			InitWatchWallpaper();
			InitMouseFunction(hwnd);
			InitDesktopIconText();
			SetDesktopIconTextBk(FALSE);
			SetTimer(hwnd, IDTIMER_MAIN, 1000, NULL);
			SetTimer(hwnd, IDTIMER_CREATE, 5000, NULL);
			DO_WS_AGGRESSIVE();
			return 0;
		}
		case WM_TIMER:
			if(wParam == IDTIMER_START)
			{
				if(bStartTimer) KillTimer(hwnd, wParam);
				bStartTimer = FALSE;
				HookStart(hwnd); // install a hook
				nCountFindingClock = 0;
				DO_WS_AGGRESSIVE();
			}
			else if(wParam == IDTIMER_MAIN)
				OnTimerMain(hwnd);
			else if(wParam == IDTIMER_MOUSE)
				OnTimerMouse(hwnd);
			else if(wParam == IDTIMER_DEKSTOPICON)
			{
				KillTimer(hwnd, wParam);
				SetDesktopIconTextBk(FALSE);
			}
			else if(wParam == IDTIMER_MONOFF)
			{
				KillTimer(hwnd, wParam);
				SendMessage(GetDesktopWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, 2);
			}
			else if(wParam == IDTIMER_CREATE)
			{
				SetTimer(hwnd, IDTIMER_CREATE, 60000, NULL);
				DO_WS_AGGRESSIVE();
			}
			return 0;

		case WM_DESTROY:
			if (bDestroy == TRUE) return 0;	//２重終了しないように
			bDestroy = TRUE;
			EndMouseFunction(hwnd);
			EndSyncTime(hwnd);
			EndAlarm();
			EndTimer();
			EndWatchWallpaper();
			EndDesktopIconText();
			// MyHelp(hwnd, -1);
			KillTimer(hwnd, IDTIMER_MAIN);
			KillTimer(hwnd, IDTIMER_CREATE);
			if(bStartTimer)
			{
				KillTimer(hwnd, IDTIMER_START);
				bStartTimer = FALSE;
			}
			HookEnd();  // uninstall a hook
			PostQuitMessage(0);
			g_hwndMain = NULL;
			return 0;
		case WM_ENDSESSION:
			if(wParam)
			{
				EndSyncTime(hwnd);
				EndAlarm();
				EndTimer();
				EndDesktopIconText();
				if(bStartTimer)
				{
					KillTimer(hwnd, IDTIMER_START);
					bStartTimer = FALSE;
				}
				HookEnd();  // uninstall a hook
			}
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			return 0;
		}

		// Messages sent/posted from tclock.dll
		case WM_USER:
			nCountFindingClock = -1;
			g_hwndClock = (HWND)lParam;
			return 0;
		case (WM_USER+1):   // error
			nCountFindingClock = -1;
			InitError((int)lParam);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		case (WM_USER+2):   // exit
			if(g_hwndPropDlg && IsWindow(g_hwndPropDlg))
				PostMessage(g_hwndPropDlg, WM_CLOSE, 0, 0);
			if(g_hDlgTimer && IsWindow(g_hDlgTimer))
				PostMessage(g_hDlgTimer, WM_CLOSE, 0, 0);
			g_hwndPropDlg = NULL;
			g_hDlgTimer = NULL;
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		case MM_MCINOTIFY:
			OnMCINotify(hwnd);
			return 0;
		case MM_WOM_DONE: // stop playing wave
		case (WM_USER+3):
			StopFile();
			return 0;
		case WM_WININICHANGE:
		{
			char *p;
			BOOL b;

			SetTimer(hwnd, IDTIMER_DEKSTOPICON, 5000, NULL);

			p = (char*)lParam;
			b = GetMyRegLong("", "DeskcalTonikaku", FALSE);

			// Update Destop Calendar automatically
			if(p && (strcmp(p, "Desktop") == 0 ||
				 strcmp(p, "RefreshDesktop") == 0))
				PostMessage(hwnd, WM_USER+10, 1,0);
			else if(b && (p == 0 || *p == 0))
				PostMessage(hwnd, WM_USER+10, 1,0);
			return 0;
		}
		case WM_SYSCOLORCHANGE:
			PostMessage(hwnd, WM_USER+10, 1,0);
			SetTimer(hwnd, IDTIMER_DEKSTOPICON, 5000, NULL);
			return 0;
		case (WM_USER+10):
		{
			if(wParam && GetMyRegLong("", "DeskcalOnlyDate", FALSE)) ;
			else ExecDeskcal(hwnd);

			return 0;
		}

		// return from power saving
		case WM_POWERBROADCAST:
		{
			if(wParam == 7) //PBT_APMRESUMESUSPEND
			{
				if(GetMyRegLong("", "DeskcalResumeSuspend", FALSE))
					PostMessage(hwnd, WM_USER+10, 0,0);
			}
			break;
		}

		// context menu
		case WM_COMMAND:
			OnTClockCommand(hwnd, LOWORD(wParam), HIWORD(wParam)); // menu.c
			DO_WS_AGGRESSIVE();
			return 0;
		// messages transfered from the dll
		case WM_CONTEXTMENU:
			// menu.c
			OnContextMenu(hwnd, (HWND)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			DO_WS_AGGRESSIVE();
			return 0;
		case WM_DROPFILES:
			OnDropFiles(hwnd, (HDROP)wParam); // mouse.c
			DO_WS_AGGRESSIVE();
			return 0;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
			if(FindVistaCalenderWindow())
			{
				DWORD_PTR dw = 0;
				SendMessageTimeout(g_hwndClock, CLOCKM_VISTACALENDAR, 1, 0, SMTO_BLOCK | SMTO_ABORTIFHUNG, 5000, &dw);
			}
			if(!bPlayingNonstop)
				PostMessage(hwnd, WM_USER+3, 0, 0);
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		case WM_MOUSEWHEEL:
			OnMouseMsg(hwnd, message, wParam, lParam); // mouse.c
			return 0;
		case WM_ENTERMENULOOP:
			if(FindVistaCalenderWindow())
			{
				DWORD_PTR dw = 0;
				SendMessageTimeout(g_hwndClock, CLOCKM_VISTACALENDAR, 1, 0, SMTO_BLOCK | SMTO_ABORTIFHUNG, 5000, &dw);
			}
			bMenuOpened = TRUE;
			break;
		case WM_EXITMENULOOP:
			bMenuOpened = FALSE;
			break;
		case WM_HOTKEY:
			OnHotkey(hwnd, (int)wParam);
			break;
		case WM_MEASUREITEM:
			OnMeasureItem(hwnd, wParam, lParam); // filelist.c
			break;
		case WM_DRAWITEM:
			OnDrawItem(hwnd, wParam, lParam); // filelist.c
			break;
		case WM_INITMENUPOPUP:
			OnInitMenuPopup(hwnd, wParam, lParam); // filelist.c
			break;
		case WM_MENURBUTTONUP:
			OnMenuRButtonUp(hwnd, wParam, lParam); //filelist.c
			break;
	}

	if(message == s_uTaskbarRestart) // When Explorer is hung up,
	{								 // and the taskbar is recreated.
		HookEnd();
		SetTimer(hwnd, IDTIMER_START, 1000, NULL);
		bStartTimer = TRUE;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*-------------------------------------------------------
  show a message when TClock failed to customize the clock
---------------------------------------------------------*/
void InitError(int n)
{
	char s[160];

	wsprintf(s, "%s: %d", MyString(IDS_NOTFOUNDCLOCK), n);
	MyMessageBox(NULL, s, NULL, MB_OK, MB_ICONEXCLAMATION);
}

/*-------------------------------------------------------
   Timer
   synchronize, alarm, timer, execute Desktop Calendar...
---------------------------------------------------------*/
static int hourLast = -1, minuteLast = -1;
static int daySaved = -1;

void OnTimerMain(HWND hwnd)
{
	SYSTEMTIME st;
	BOOL b;

	GetLocalTime(&st);
	b = TRUE;
	if(hourLast == (int)st.wHour &&
		minuteLast == (int)st.wMinute) b = FALSE;
	hourLast = st.wHour;
	minuteLast = st.wMinute;
	if(daySaved >= 0 && st.wDay != daySaved)
	{
		if(ExecDeskcal(hwnd)) daySaved = st.wDay;
	}
	else daySaved = st.wDay;

	OnTimerSNTP(hwnd, &st); // sntp.c
	OnTimerAlarm(hwnd, &st); // alarm.c
	OnTimerTimer(hwnd, &st); // timer.c
	if(b) OnTimerWatchWallpaper(hwnd, &st); // deskcal.c
	if(b) SetDesktopIconTextBk(FALSE);

	// the clock window exists ?
	if(0 <= nCountFindingClock && nCountFindingClock < 20)
		nCountFindingClock++;
	else if(nCountFindingClock == 20)
	{
		nCountFindingClock++;
		/*
		InitError(4);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		*/
	}
}

/*-------------------------------------------
  load a language dll
---------------------------------------------*/
HINSTANCE LoadLanguageDLL(char *langdllname)
{
	HINSTANCE hInst = NULL;
	char fname[MAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE hfind = INVALID_HANDLE_VALUE;

	GetMyRegStr("", "LangDLL", fname, MAX_PATH, "");
	if(fname[0])
	{
		hfind = FindFirstFile(fname, &fd);
		if(hfind != INVALID_HANDLE_VALUE) FindClose(hfind);
	}
	if(hfind == INVALID_HANDLE_VALUE)
	{
		GetModuleFileName(g_hInst, fname, MAX_PATH);
		del_title(fname);
		add_title(fname, "lang*.dll");
		hfind = FindFirstFile(fname, &fd);
		if(hfind != INVALID_HANDLE_VALUE)
		{
			FindClose(hfind);
			del_title(fname); add_title(fname, fd.cFileName);
		}
	}

	if(hfind != INVALID_HANDLE_VALUE)
	{
		if(!CheckDLL(fname)) return NULL;
		hInst = LoadLibrary(fname);
	}

	if(hInst == NULL)
		MyMessageBox(NULL, "Can't load a language module.",
			NULL, MB_OK, MB_ICONEXCLAMATION);
	else strcpy(langdllname, fname);
	return hInst;
}

HINSTANCE GetLangModule(void)
{
	return g_hInstResource;
}

/*-------------------------------------------
  Check version of dll
---------------------------------------------*/
BOOL CheckTCDLL(void)
{
	char fname[MAX_PATH];

	strcpy(fname, g_mydir); add_title(fname, "tcdll.dll");
	return CheckDLL(fname);
}

/*-------------------------------------------
  Check version of dll
---------------------------------------------*/
BOOL CheckDLL(char *fname)
{
	DWORD size;
	char *pBlock;
	VS_FIXEDFILEINFO *pffi;
	BOOL br = FALSE;

	size = GetFileVersionInfoSize(fname, 0);
	if(size > 0)
	{
		pBlock = malloc(size);
		if(GetFileVersionInfo(fname, 0, size, pBlock))
		{
			UINT tmp;
			if(VerQueryValue(pBlock, "\\", &pffi, &tmp))
			{
				if(pffi->dwFileVersionMS == VERSIONM &&
					HIWORD(pffi->dwFileVersionLS) == VERSIONL)
				{
					br = TRUE;
				}
			}
		}
		free(pBlock);
	}
	if(!br)
	{
		char msg[MAX_PATH+30];

		strcpy(msg, "Invalid file version: ");
		get_title(msg + strlen(msg), fname);
		MyMessageBox(NULL, msg,
			NULL, MB_OK, MB_ICONEXCLAMATION);
	}
	return br;
}

/*-------------------------------------------
  Show "TClock Help"
---------------------------------------------*/
void MyHelp(HWND hwnd, int id)
{
	char helpurl[1024];
	char s[20];

	GetMyRegStr("", "HelpURL", helpurl, 1024, "");
	if(helpurl[0] == 0)
	{
		WIN32_FIND_DATA fd;
		HANDLE hfind;
		strcpy(helpurl, g_mydir);
		add_title(helpurl, "help\\index.html");
		hfind = FindFirstFile(helpurl, &fd);
		if(hfind != INVALID_HANDLE_VALUE)
			FindClose(hfind);
		else
			strcpy(helpurl, MyString(IDS_HELPURL));
	}
	if(id != 0)
	{
		if(strlen(helpurl) > 0 && helpurl[ strlen(helpurl) - 1 ] != '/')
			del_title(helpurl);
		wsprintf(s, "%d.html", id);
		add_title(helpurl, s);
	}

	ShellExecute(hwnd, NULL, helpurl, NULL, "", SW_SHOW);
}

void My2chHelp(HWND hwnd, int id)
{
	char helpurl[1024];
	char s[20];

	GetMyRegStr("", "2chHelpURL", helpurl, 1024, "");
	if(helpurl[0] == 0)
	{
		WIN32_FIND_DATA fd;
		HANDLE hfind;
		strcpy(helpurl, g_mydir);
		add_title(helpurl, "help2ch\\index.html");
		hfind = FindFirstFile(helpurl, &fd);
		if(hfind != INVALID_HANDLE_VALUE)
			FindClose(hfind);
		else
			strcpy(helpurl, MyString(IDS_HELP2CH));
	}
	if(id != 0)
	{
		if(strlen(helpurl) > 0 && helpurl[ strlen(helpurl) - 1 ] != '/')
			del_title(helpurl);
		wsprintf(s, "%d.html", id);
		add_title(helpurl, s);
	}

	ShellExecute(hwnd, NULL, helpurl, NULL, "", SW_SHOW);
}

/*------------------------------------------------
   initialize the registy
--------------------------------------------------*/
void CheckRegistry(void)
{
	WIN32_FIND_DATA fd;
	HANDLE hfind;
	char fname[MAX_PATH];
	char s[80];

	//SetMyRegStr(NULL, "ExePath", g_mydir);
	//DelMyRegKey("DLL");

	strcpy(fname, g_mydir);
	add_title(fname, "tclock.ini");
	hfind = FindFirstFile(fname, &fd);
	if(hfind != INVALID_HANDLE_VALUE)
	{
		FindClose(hfind);
		g_bIniSetting = TRUE;
		strcpy(g_inifile, fname);

		if(!GetMyRegLong(NULL, "RegToIni", FALSE))
		{
			MoveRegToIni(NULL);
			SetMyRegLong(NULL, "RegToIni", TRUE);
		}
	}

	SetMyRegStr(NULL, "ExePath", g_mydir);

	GetMyRegStr(NULL, "Font", s, 80, "");
	if(s[0] == 0)
	{
		HFONT hfont;
		LOGFONT lf;
		hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if(hfont)
		{
			GetObject(hfont, sizeof(lf),(LPVOID)&lf);
			SetMyRegStr(NULL, "Font", lf.lfFaceName);
		}
	}
}

static COLORREF oldDesktopTextColor = 0xFFFFFFFF;
static BOOL bChangeDesktopText = FALSE;

static COLORREF oldDesktopTextBkColor = 0xFFFFFFFF;

void InitDesktopIconText(void)
{
	HWND hwnd;

	hwnd = FindWindow("Progman", "Program Manager");
	hwnd = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	hwnd = FindWindowEx(hwnd, NULL, "SysListView32", NULL);
	if(!hwnd) return;

	if(oldDesktopTextColor == 0xFFFFFFFF)
		oldDesktopTextColor = ListView_GetTextColor(hwnd);
	bChangeDesktopText = FALSE;
	oldDesktopTextBkColor = ListView_GetTextBkColor(hwnd);

}

void EndDesktopIconText(void)
{
	HWND hwnd;

	hwnd = FindWindow("Progman", "Program Manager");
	hwnd = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	hwnd = FindWindowEx(hwnd, NULL, "SysListView32", NULL);
	if(!hwnd) return;

	if(oldDesktopTextColor != 0xFFFFFFFF && bChangeDesktopText == TRUE)
		ListView_SetTextColor(hwnd, oldDesktopTextColor);
	oldDesktopTextColor = 0xFFFFFFFF;
	ListView_SetTextBkColor(hwnd, oldDesktopTextBkColor);
	oldDesktopTextBkColor = GetSysColor(COLOR_DESKTOP);

	InvalidateRect(hwnd, NULL, TRUE);
}

/*--------------------------------------------------
  set transparent background of desktop icons text
----------------------------------------------------*/
void SetDesktopIconTextBk(BOOL bNoTrans)
{
	BOOL b,bt;
	HWND hwnd;
	COLORREF col, colText = 0;

	bt = GetMyRegLong(NULL, "UseDesktopTextColor", FALSE);
	b = GetMyRegLong(NULL, "DesktopTextBk", FALSE);
	if(b == FALSE && bt == FALSE && bNoTrans == FALSE) return;

	hwnd = FindWindow("Progman", "Program Manager");
	hwnd = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
	hwnd = FindWindowEx(hwnd, NULL, "SysListView32", NULL);
	if(!hwnd) return;

	if(bt == TRUE || bNoTrans == TRUE)
	{
		if(bt == TRUE)
		{
			colText = GetMyRegLong(NULL, "DesktopTextColor", RGB(255,255,255));
			if (colText & 0x80000000) {
				colText = GetSysColor(colText & 0x00ffffff);
			}
			col = ListView_GetTextColor(hwnd);
			if (col == colText) {
				bt = FALSE;
			} else {
				bChangeDesktopText = TRUE;
			}
		} else {
			if(oldDesktopTextColor != 0xFFFFFFFF) {
				if (bChangeDesktopText == TRUE) {
					colText = oldDesktopTextColor;
					bt = TRUE;
					bChangeDesktopText = FALSE;
				}
			}
		}
	}

	col = b? CLR_NONE : GetSysColor(COLOR_DESKTOP);
	if(ListView_GetTextBkColor(hwnd) == col) b = FALSE; else b = TRUE;

	if(b == FALSE && bt == FALSE) return;
	if(bt == TRUE) ListView_SetTextColor(hwnd, colText);
	if(b == TRUE)  ListView_SetTextBkColor(hwnd, col);
	InvalidateRect(hwnd, NULL, TRUE);
}


// IsUserAnAdmin shell32.dll@680
// http://msdn2.microsoft.com/en-us/library/aa376389.aspx
static BOOL IsUserAdmin(void)
{
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	BOOL b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
																		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
																		&AdministratorsGroup);
	if (b)
	{
		if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
		{
			b = FALSE;
		}
		FreeSid(AdministratorsGroup);
	}
	return b;
}


static BOOL IsWow64(void)
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL (WINAPI* LPFN_ISWOW64PROCESS)(HANDLE hProcess, PBOOL Wow64Process);
	LPFN_ISWOW64PROCESS IsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if(IsWow64Process)
	{
		if(!IsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			bIsWow64 = FALSE;
		}
	}
	return bIsWow64;
}


static BOOL ImmDisableIME(DWORD idThread)
{
	typedef BOOL (WINAPI* FImmDisableIME)(DWORD);

	BOOL ret = FALSE;
	HMODULE hDll = LoadLibrary("Imm32.dll");
	FImmDisableIME fImmDisableIME = (FImmDisableIME)GetProcAddress(hDll, "ImmDisableIME");
	if (fImmDisableIME) {
		ret = fImmDisableIME(idThread);
	}
	FreeLibrary(hDll);
	return ret;
}


#define MSGFLT_ADD 1
#define MSGFLT_REMOVE 2
static BOOL AddMessageFilters(void)
{
	typedef BOOL (WINAPI *pfnChangeWindowMessageFilter)(UINT, DWORD);
	int i;
	UINT messages[] = {
		WM_CLOSE,
		WM_DESTROY,
		WM_COMMAND,
		WM_CONTEXTMENU,
		WM_EXITMENULOOP,
		WM_DROPFILES,
		WM_MOUSEWHEEL,
		WM_LBUTTONDOWN,
		WM_RBUTTONDOWN,
		WM_MBUTTONDOWN,
		WM_XBUTTONDOWN,
		WM_LBUTTONUP,
		WM_RBUTTONUP,
		WM_MBUTTONUP,
		WM_XBUTTONUP,
		WM_USER,
		WM_USER+1,
		WM_USER+2,
	};

	HMODULE hUser32 = GetModuleHandle("user32.dll");
	pfnChangeWindowMessageFilter ChangeWindowMessageFilter = (pfnChangeWindowMessageFilter)
		GetProcAddress(hUser32, "ChangeWindowMessageFilter");
	if (!ChangeWindowMessageFilter)
		return FALSE;

	for (i = 0; i < RTL_NUMBER_OF(messages); i++) {
		ChangeWindowMessageFilter(messages[i], MSGFLT_ADD);
	}
	return TRUE;
}



#if !defined(TCLOCK2CH_TTBASE)
/*-------------------------------------------
  entry point of program
  not use "WinMain" for compacting the file size
---------------------------------------------*/
#ifndef _DEBUG
#pragma comment(linker, "/subsystem:windows")
#pragma message("entry WinMainCRTStartup")
void __cdecl WinMainCRTStartup(void)
{
#else
#pragma message("entry WinMain")
int WINAPI WinMain(HINSTANCE hinst,HINSTANCE hinstPrev,LPSTR lpszCmdLine, int nShow)
{
	UNREFERENCED_PARAMETER(hinst);
	UNREFERENCED_PARAMETER(hinstPrev);
	UNREFERENCED_PARAMETER(lpszCmdLine);
	UNREFERENCED_PARAMETER(nShow);
#endif

	g_hInst = GetModuleHandle(NULL);

	SetProcessShutdownParameters(0x1FF, 0); // 最後の方でシャットダウンするアプリケーション
	ImmDisableIME((DWORD)-1);

	ExitProcess(TclockExeMain());
}

#else /* defined(TCLOCK2CH_TTBASE) */

#ifdef _WIN64
# error TCLOCK2CH_TTBASE is not supported on x64
# pragma comment(linker, "/base:0x0000070066660000")
#else
# pragma comment(linker,         "/base:0x66660000")
#endif // _WIN64

#ifndef _DEBUG
# pragma comment(linker, "/entry:\"DllMain\"")
#endif

# pragma message("entry DllMain")
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)

{
	UNREFERENCED_PARAMETER(fdwReason);
	UNREFERENCED_PARAMETER(lpvReserved);
	g_hInst = hinstDLL;
	return TRUE;
}
static DWORD WINAPI TTBaseThreadFunc(LPVOID param)
{
	UNREFERENCED_PARAMETER(param);
	return TclockExeMain();
}

#include "ttbase/ttbasesdk.h"
#define	PLUGIN_NAME	"TClock2ch for TTBase"
#define	PLUGIN_TYPE	ptAlwaysLoad

static HINSTANCE hInstResource = 0;
static HANDLE hTTBaseThread = 0;

static int TTBPImplementGetCommandInfoStart(void)
{
	char temp[MAX_PATH];
	hInstResource = LoadLanguageDLL(temp);
	return GetMouseFuncCount() - 1;
}
static void TTBPImplementGetCommandInfo(int id, PLUGIN_COMMAND_INFO *pi)
{
	static char buf[80];
	MOUSE_FUNC_INFO *pmfl;

	pmfl = GetMouseFuncList() + id + 1;
	pi->Name = pmfl->ttbase_name;
	pi->Caption = buf;
	pi->CommandID = id;
	pi->Attr = 0;
	pi->ResID = -1;
	pi->DispMenu = dmHotKeyMenu;
	pi->TimerInterval = 0;
	pi->TimerCounter = 0;

	buf[0] = 0;
	if(hInstResource) LoadString(hInstResource, pmfl->idstring, buf, sizeof(buf));
}
static void TTBPImplementGetCommandInfoEnd(void)
{
	if (hInstResource)
	{
		FreeLibrary(hInstResource);
		hInstResource = 0;
	}
}

// --------------------------------------------------------
//    プラグインがロードされたときに呼ばれる
// --------------------------------------------------------
static BOOL TTBPImplementInit(void)
{
	DWORD tid;
	if (hTTBaseThread) return FALSE;
	hTTBaseThread = CreateThread(NULL, 0, TTBaseThreadFunc, 0, 0, &tid);
	return hTTBaseThread ? TRUE : FALSE;
}

// --------------------------------------------------------
//    プラグインがアンロードされたときに呼ばれる
// --------------------------------------------------------
static void TTBPImplementUnload(void)
{
	if (g_hwndMain && IsWindow(g_hwndMain))
		PostMessage(g_hwndMain, WM_COMMAND, IDC_EXIT, 0);
	if (hTTBaseThread)
	{
		WaitForSingleObject(hTTBaseThread, INFINITE);
		CloseHandle(hTTBaseThread);
		hTTBaseThread = 0;
	}
}

// --------------------------------------------------------
//    コマンド実行時に呼ばれる
// --------------------------------------------------------
static BOOL TTBPImplementExecute(int CmdId, HWND hWnd)
{
	UNREFERENCED_PARAMETER(hWnd);
	if (g_hwndMain && IsWindow(g_hwndMain))
		PostMessage(g_hwndMain, WM_COMMAND, MAKELPARAM(IDC_HOTKEY0, GetMouseFuncList()[CmdId + 1].mousefunc), 0);
	return TRUE;
}

// --------------------------------------------------------
//    グローバルフックメッセージがやってくる
// --------------------------------------------------------
static void TTBPImplementHook(UINT Msg, DWORD wParam, DWORD lParam)
{
	UNREFERENCED_PARAMETER(Msg);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
}

#include "ttbase/ttbasesdk.c"

#endif
