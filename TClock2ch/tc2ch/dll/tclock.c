/*-----------------------------------------------------
  tclock.c
  customize the tray clock
  KAZUBON 1997-2001
-------------------------------------------------------*/
#include "tcdll.h"
#include <math.h>

#define TEST_505	1

#define GRADIENT_FILL_RECT_H    0x00000000
#define GRADIENT_FILL_RECT_V    0x00000001

#define MAX_PROCESSOR               8

//グラフの記録数
#define MAXGRAPHLOG 600

extern PSTR CreateFullPathName(HINSTANCE hmod, PSTR fname);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitClock(HWND hwnd);
void EndClock(void);
BOOL IsIE4(HWND hwnd);
void CreateClockDC(HWND hwnd);
void ReadData(HWND hwnd);
void InitSysInfo(HWND hwnd);
void OnTimer(HWND hwnd);
void OnTimerUpperTaskbar(HWND hwnd);
static void DrawClockFocusRect(HWND hwnd, HDC hdc);
void DrawClockSub(HWND hwnd, HDC hdc, SYSTEMTIME* pt, int beat100);
void DrawGraph(HWND hwnd, HDC hdc, int xclock, int yclock, int wclock, int hclock);
LRESULT OnCalcRect(HWND hwnd);
void OnCopy(HWND hwnd, LPARAM lParam);
void UpdateSysRes(HWND hwnd, BOOL bsysres, BOOL bbattery, BOOL bmem, BOOL bmb, BOOL bpermon, BOOL bnet, BOOL bhdd, BOOL bcpuc, BOOL bvol);
void InitDaylightTimeTransition(void);
BOOL CheckDaylightTimeTransition(SYSTEMTIME *lt);
void FitClock(); //390
void getGraphVal();
void OnCopyData(HWND hwnd, COPYDATASTRUCT* pCopyDataStruct);

static BOOL bTooltipTimerStart = FALSE;

/*------------------------------------------------
  shared data among processes
--------------------------------------------------*/
#ifdef _MSC_VER
#pragma data_seg("MYDATA")
HHOOK hhook = 0;
HWND hwndTClockMain = NULL;
HWND hwndTaskbar = NULL;
HWND hwndHook = NULL;
BOOL bHookEnable = FALSE;
char szShareBuf[81] = { 0 };
#pragma data_seg()
#else
extern HWND hwndTClockMain;
extern HWND hwndTaskbar;
#endif

/*------------------------------------------------
  globals
--------------------------------------------------*/
HANDLE hmod = 0;
HWND hwndClock = NULL;
WNDPROC oldWndProc = NULL;
BOOL bTimer = FALSE;
BOOL bTimerTesting = FALSE;
HDC hdcClock = NULL;
static HGDIOBJ hbmpClockOld = NULL;
HBITMAP hbmpClock = NULL;
HBITMAP hbmpClockSkin = NULL;
HFONT hFon = NULL;
COLORREF colback, colback2, colfore;
BOOL fillbackcolor = FALSE;
DWORD grad;

// add by 505 =================================
static BOOL bGetingFocus = FALSE;
#define USE_ANALOG_CLOCK	1		// add by 505
#define USE_FORMAT_FOR_STARTBTN	1	// add by 505

#ifdef USE_FORMAT_FOR_STARTBTN		// add by 505
extern BOOL bStartBtnDispSecond;
extern BOOL bStartBtnDispOther;
extern BOOL bUseFormatForStartBtn;
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505

#ifdef USE_ANALOG_CLOCK	// add by 505
enum {
	ANALOG_CLOCK_NOTREAD,
	ANALOG_CLOCK_NOTUSE,
	ANALOG_CLOCK_USE
};
enum {
	ANALOG_CLOCK_POS_LEFT,
	ANALOG_CLOCK_POS_RIGHT,
	ANALOG_CLOCK_POS_MIDDLE
};
#define ACLOCK_SIZE_CX	18
#define ACLOCK_SIZE_CY	18
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
int nAnalogClockUseFlag = ANALOG_CLOCK_NOTREAD;
BOOL bAnalogClockAtStartBtn = FALSE;
SIZE sizeAClock = { 0, 0 };
int nAnalogClockPos = ANALOG_CLOCK_POS_LEFT;
static HDC hdcAnalogClock = NULL;
static HBITMAP hbmpAnalogClock = NULL;
static HBITMAP hbmpAnalogClockMask = NULL;
static HDC hdcAnalogClockMask  = NULL;
static LPBYTE lpbyAnalogClockMask = NULL;
static int nHourPenWidth = 2;
static int nMinPenWidth = 1;
static HPEN hpenHour = NULL;
static HPEN hpenMin = NULL;
static int nAnalogClockHPos = 0;
static int nAnalogClockVPos = 0;
static COLORREF colAClockHourHandColor = RGB(255, 0, 0);
static COLORREF colAClockMinHandColor = RGB(0, 0, 255);
static TCHAR szAnalogClockBmp[MAX_PATH] = "";
static SIZE  sizeAnalogBitmapSize = { 0, 0 };
static BOOL InitAnalogClockData(HWND hWnd);
#endif //USE_ANALOG_CLOCK	// add by 505
// add by 505 =================================

static BOOL bGraphTimerStart = FALSE;
char format[1024];

USRSTR usrstr[10];

BOOL bHour12, bHourZero;
SYSTEMTIME LastTime;
int beatLast = -1;
int bDispSecond = FALSE;
int nDispBeat = 0;
BOOL bNoClock = FALSE;
int nBlink = 0;
BOOL bIE4 = FALSE, bWin95 = FALSE, bWin98 = FALSE,
	bWinNT = FALSE, bWin2000 = FALSE, bWinXP = FALSE, bVista = FALSE,
	bVisualStyle = FALSE;
int dwidth = 0, dheight = 0, dvpos = 0, dlineheight = 0, dclkvpos = 0;
int iClockWidth = -1;
BOOL bPlaying = FALSE;
BOOL bDispSysInfo = FALSE, bTimerSysInfo = FALSE;
BOOL bGetSysRes = FALSE, bGetBattery = FALSE, bGetMem = FALSE,
     bGetMb = FALSE, bGetPm = FALSE, bGetNet = FALSE, bGetHdd = FALSE, bGetCpu = FALSE, bGetVol = FALSE;
int iFreeRes[3] = {0,0,0}, iCPUUsage = 0, iBatteryLife = 0, iVolume = 0;
int iCPUClock[MAX_PROCESSOR] = {0};
MEMORYSTATUSEX msMemory;
BOOL bWatchTaskWnd = FALSE;
BOOL bClockShadow = FALSE;
BOOL bClockBorder = FALSE;
BOOL bVolRedraw = FALSE;
int nShadowRange = 1;
COLORREF colShadow;
int nTextPos = 0;
BOOL bFitClock = FALSE;  //390
BOOL bRClickMenu = FALSE;
int tEdgeTop;
int tEdgeLeft;
int tEdgeBottom;
int tEdgeRight;
int gConfigNo = 1;
//Addby ひまじん
BOOL checknet=FALSE;
BOOL bGraph = FALSE;
BOOL bLogGraph = FALSE;
BOOL bGraphTate = FALSE;
BOOL bReverseGraph = FALSE;
BOOL bGraphRedraw = FALSE;
int BackNetBand = 100;
int BackNetBand2 = 100;
int GraphL=0;
int GraphT=0;
int GraphR=0;
int GraphB=0;
int GraphType=1;
double sendlog[MAXGRAPHLOG+1] = { 0 };
double recvlog[MAXGRAPHLOG+1] = { 0 };
int ColSend,ColRecv,ColSR;
int graphInterval = 1;
int graphMode = 1;
int cpuHigh;

extern HWND hwndStart;
extern HWND hwndStartMenu;
extern BOOL bWin2000; //635@p5
extern BOOL bStartMenuClock; // startbtn.c
extern BOOL bSkinTray;
extern BOOL bFillTray;

extern int codepage;

// XButton Messages
#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C

/*------------------------------------------------
  initialize the clock
--------------------------------------------------*/
void InitClock(HWND hwnd)
{
	WIN32_FIND_DATA fd;
	HANDLE hfind;
	char fname[MAX_PATH];
	BOOL b;
	DWORD dw;
	int nDelay;

	if(hwndClock != NULL) return;
	hwndClock = hwnd;

	dw = GetVersion();
	if (dw & 0x80000000)
	{
		bWin95 = TRUE;
		bWin98 = (LOBYTE(LOWORD(dw)) >= 4 && HIBYTE(LOWORD(dw)) >= 10);
	//bWinME = (LOBYTE(LOWORD(dw)) >= 4 && HIBYTE(LOWORD(dw)) >= 90);
	}
	else
	{
		bWinNT   = TRUE;
		bWin2000 = (LOBYTE(LOWORD(dw)) >= 5);
		bWinXP   = (LOBYTE(LOWORD(dw)) >= 5 && HIBYTE(LOWORD(dw)) >= 1);
		bVista   = (LOBYTE(LOWORD(dw)) >= 6);
	}	
	if (bWin2000) bWin98 = TRUE;
	if (bVista)   bWinXP = TRUE;

	dw = GetRegStr(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager",
		"DllName", fname, MAX_PATH, "");
	bVisualStyle = (dw>0)? TRUE:FALSE;

	GetModuleFileName(hmod, fname, MAX_PATH);
	del_title(fname); add_title(fname, "tclock.ini");
	hfind = FindFirstFile(fname, &fd);
	if(hfind != INVALID_HANDLE_VALUE)
	{
		g_bIniSetting = TRUE;
		strcpy(g_inifile, fname);
		FindClose(hfind);
	}

	CpuMoni_start(); // cpu.c
	PerMoni_start(); // permon.c
	Net_start();     // net.c

	PostMessage(hwndTClockMain, WM_USER, 0, (LPARAM)hwnd);

	//レジストリ読み込み
	ReadData(hwndClock);

#ifdef USE_ANALOG_CLOCK	// add by 505
	InitAnalogClockData(hwndClock);
#endif //USE_ANALOG_CLOCK	// add by 505

	InitDaylightTimeTransition();
	//サブクラス化
	oldWndProc = (WNDPROC)GetWindowLongPtr(hwndClock, GWLP_WNDPROC);
	SubclassWindow(hwndClock, WndProc);
	//ダブルクリック受け付けない
	SetClassLongPtr(hwndClock, GCL_STYLE,
	  GetClassLongPtr(hwndClock, GCL_STYLE) & ~CS_DBLCLKS);

	//デスクトップの初期化
	SetDesktopIcons();
	//スタートボタンの初期化
	SetStartButton(hwndClock);
	//スタートメニューの初期化
	SetStartMenu(hwndClock);

	//ツールチップ作成
	TooltipInit(hwndClock);

	b = GetMyRegLong(NULL, "DropFiles", FALSE);
	DragAcceptFiles(hwnd, b);

	bIE4 = IsIE4(hwndClock);
	if(bIE4)
	{
		InitStartMenuIE4();
		InitTaskSwitch(hwndClock);
	}

	InitTaskBarBorder(hwndClock);
	InitTrayNotify(hwndClock);

	if(bWin2000) SetLayeredTaskbar(hwndClock);
	if(bWin2000) InitLayeredStartMenu(hwndClock);

	//ネットワーク遅延
	nDelay = GetMyRegLong("", "DelayNet", 0);
	if(nDelay > 0)
	{
		SetTimer(hwnd, 7, nDelay * 1000, NULL);
	}

	// DrawClock()k処理から移動
	CreateClockDC(hwndClock);

	//タスクバーの更新
	PostMessage(GetParent(GetParent(hwndClock)), WM_SIZE,
		SIZE_RESTORED, 0);
	InvalidateRect(GetParent(GetParent(hwndClock)), NULL, TRUE);
}

/*------------------------------------------------
  ending process
--------------------------------------------------*/
void DeleteClockRes(void)
{
	TooltipDeleteRes();
	if(hFon) DeleteObject(hFon); hFon = NULL;
	if(hdcClock) DeleteDC(hdcClock); hdcClock = NULL;
	if(hbmpClock) DeleteObject(hbmpClock); hbmpClock = NULL;
#ifdef USE_ANALOG_CLOCK	// add by 505
	if (hpenHour) {
		DeleteObject(hpenHour); hpenHour = NULL;
	}
	if (hpenMin) {
		DeleteObject(hpenMin); hpenMin = NULL;
	}
	if (hdcAnalogClock) {
		DeleteDC(hdcAnalogClock);
		hdcAnalogClock = NULL;
	}
	if (hbmpAnalogClock) {
		DeleteObject(hbmpAnalogClock);
		hbmpAnalogClock = NULL;
	}
	if (hdcAnalogClockMask) {
		DeleteDC(hdcAnalogClockMask);
		hdcAnalogClockMask = NULL;
	}
	if (hbmpAnalogClockMask) {
		DeleteObject(hbmpAnalogClockMask);
		hbmpAnalogClockMask = NULL;
		lpbyAnalogClockMask = NULL;
	}
#endif //USE_ANALOG_CLOCK	// add by 505
}

void EndClock(void)
{
	DragAcceptFiles(hwndClock, FALSE);
	TooltipEnd(hwndClock);
	EndTrayNotify();
	EndTaskSwitch();
	EndTaskBarBorder();
	EndStartButton();
	EndStartMenu();
	EndDesktopIcons();
	ClearStartMenuIE4();
	DeleteClockRes();
	EndNewAPI(hwndClock);
	EndSysres();
	FreeBatteryLife();
	FreeCpuClock();
	CpuMoni_end(); // cpu.c
	PerMoni_end(); // permon.c
	Net_end();     // net.c

	if(hwndClock && IsWindow(hwndClock))
	{
		if(bTimer) KillTimer(hwndClock, 1); bTimer = FALSE;
		if(bTimerSysInfo) KillTimer(hwndClock, 2);
		if(bWatchTaskWnd) KillTimer(hwndClock, 3); bWatchTaskWnd = FALSE;
		if(checknet) KillTimer(hwndClock, 4); checknet=FALSE;
		if(bGraphTimerStart) KillTimer(hwndClock, 5); bGraphTimerStart=FALSE;
		if(bTooltipTimerStart) KillTimer(hwndClock, 6); bTooltipTimerStart=FALSE;
		if (hwndClock && IsWindow(hwndClock))
		{
#if ENABLE_CHECK_SUBCLASS_NESTING
			if(oldWndProc && (WNDPROC)WndProc == (WNDPROC)GetWindowLongPtr(hwndClock, GWLP_WNDPROC))
#else
			if(oldWndProc)
#endif
				SubclassWindow(hwndClock, oldWndProc);
		}
		oldWndProc = NULL;
	}
//	hwndClock = NULL;	//環境によって、2回EndClockされるかもしれないので、
						//クリアしないように。2回目にNULLに対してメッセージを
						//送ってしまい、動作がおかしくなる原因になってるっぽい
	//TClockウィンドウを終了させる
	if(IsWindow(hwndTClockMain))
		PostMessage(hwndTClockMain, WM_USER+2, 0, 0);
}

/*------------------------------------------------
  IE 4 or later is installed ?
--------------------------------------------------*/
BOOL IsIE4(HWND hwnd)
{
	char classname[80];
	DWORD dw;

	dw = GetRegLong(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
		"ClassicShell", 0);
	if(dw) return TRUE;

	hwnd = GetParent(GetParent(hwnd));
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
  subclass procedure of the clock
--------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) // ツールチップ対応
	{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			TooltipOnMouseEvent(hwnd, message, wParam, lParam);
			break;
	}

	switch(message)
	{
		case WM_DESTROY:
			DeleteClockRes();
			break;
		// 親ウィンドウから送られ、サイズを返すメッセージ
		case (WM_USER+100):
			if(bNoClock) break;
			return OnCalcRect(hwnd);
		// システムの設定を反映する
		case WM_SYSCOLORCHANGE:
			CreateClockDC(hwnd); //hdcClock作り直し
		case WM_WININICHANGE:
		case WM_TIMECHANGE:
		// 親ウィンドウから送られる
		case (WM_USER+101):
		{
			HDC hdc;
			if(bNoClock) break;
			if(!hdcClock) CreateClockDC(hwnd);
			hdc = GetDC(hwnd);
			DrawClock(hwnd, hdc);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		//case WM_MOVE:
		//	if(bFitClock) { FitClock(); }
		//	CreateClockDC(hwnd);
		//	break;
		case WM_SIZE:
			CreateClockDC(hwnd); //hdcClock作り直し
			break;
		case WM_ERASEBKGND:
			break;
		case WM_SETFOCUS:
			bGetingFocus = TRUE;
			break;
		case WM_KILLFOCUS:
			bGetingFocus = FALSE;
			break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			if(bNoClock) break;
			hdc = BeginPaint(hwnd, &ps);
			DrawClock(hwnd, hdc);
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_TIMER:
			if(wParam == 1) OnTimer(hwnd);
			else if(wParam == 2)
				UpdateSysRes(hwnd, bGetSysRes, bGetBattery, bGetMem, bGetMb, bGetPm, bGetNet, bGetHdd, bGetCpu, bGetVol);
			else if(wParam == 3)
				OnTimerUpperTaskbar(hwnd);
			else if(wParam == 4)	//Add by ひまじん
				Net_restart();	//net.c
			else if(wParam == 5)
				getGraphVal();
			else if(wParam == 6)
				TooltipOnTimer(hwnd);
			else if(wParam == 7)
			{
				KillTimer(hwnd, wParam);
				Net_restart();	//net.c
			}
			else
			{
				if(bNoClock) break;
			}
			return 0;
		// マウスダウン
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
			if(nBlink)
			{
				nBlink = 0; InvalidateRect(hwnd, NULL, TRUE);
			}
			if(message == WM_LBUTTONDOWN && bStartMenuClock &&
				hwndStart && IsWindow(hwndStart))
			{
				SetWindowPos(hwndStart, NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
				// スタートメニュー
				PostMessage(hwndStart, WM_LBUTTONDOWN, wParam, lParam);
				return 0;
			}
			if (message == WM_RBUTTONDOWN && (wParam & MK_LBUTTON || ((wParam&MK_CONTROL)&&(wParam&MK_SHIFT)) || bRClickMenu))
			{
				return 0;
			}

			PostMessage(hwndTClockMain, message, wParam, lParam);
			return 0;
		// マウスアップ
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
			if (message == WM_RBUTTONUP && (wParam & MK_LBUTTON || ((wParam&MK_CONTROL)&&(wParam&MK_SHIFT)) || bRClickMenu))
			{
				DWORD mp;
				mp = GetMessagePos();
				PostMessage(hwndTClockMain, WM_CONTEXTMENU, (WPARAM)hwnd, (LPARAM)mp);
				return 0;
			}
			PostMessage(hwndTClockMain, message, wParam, lParam);
			// ↓右クリックがWM_CONTEXTMENUに変換されないようにoldWndProcを呼ばない
			// 呼んでしまうと独自処理を割り当てたときもメニューが開いてしまう
			//if(message == WM_RBUTTONUP) break;
			return 0;
		case WM_MOUSEMOVE:
			return 0;
		case WM_CONTEXTMENU:   // 右クリックメニュー
			// ApplicationKeyを押されたときにくる
			// 右クリックの場合は↑で自前で変換しないと､独自処理を割り当てたときも
			// メニューが開いてしまう
			//if (bRClickMenu) {
				PostMessage(hwndTClockMain, message, wParam, lParam);
			//}
			return 0;
		case WM_NCHITTEST: // oldWndProcに処理させない
			return DefWindowProc(hwnd, message, wParam, lParam);
		case WM_MOUSEACTIVATE:
			return MA_ACTIVATE;
		// ファイルのドロップ
		case WM_DROPFILES:
			PostMessage(hwndTClockMain, WM_DROPFILES, wParam, lParam);
			return 0;
		case WM_NOTIFY: //ツールチップのテキスト表示
		{
			LRESULT lres;
			if (TooltipOnNotify(&lres, lParam)) return lres;
			break;
		}
		case WM_COMMAND:
			if(LOWORD(wParam) == 102) EndClock();
			return 0;
		case CLOCKM_REFRESHCLOCK: // refresh the clock
		{
			BOOL b;
			ReadData(hwnd);

#ifdef USE_ANALOG_CLOCK	// add by 505
			InitAnalogClockData(hwndClock);
#endif //USE_ANALOG_CLOCK	// add by 505

			//390CreateClockDC(hwnd);
			b = GetMyRegLong(NULL, "DropFiles", FALSE);
			DragAcceptFiles(hwnd, b);

			// DrawClock()k処理から移動
			CreateClockDC(hwndClock);

			InvalidateRect(hwnd, NULL, FALSE);
			InvalidateRect(GetParent(hwndClock), NULL, TRUE);
			//return 0; //390
			//635@p5
			TooltipOnRefresh(hwnd);
		}
		case CLOCKM_REFRESHTASKBAR: // refresh other elements than clock
			SetDesktopIcons();
			SetStartButton(hwnd);
			SetStartMenu(hwnd);
			if(bIE4)
			{
				InitStartMenuIE4();
				InitTaskSwitch(hwnd);
			}
			InitTaskBarBorder(hwnd);
			InitTrayNotify(hwnd);
			// DrawClock()k処理から移動
			CreateClockDC(hwnd);
			if(bWin2000) SetLayeredTaskbar(hwndClock);
			if(bWin2000) InitLayeredStartMenu(hwndClock);
			PostMessage(GetParent(GetParent(hwnd)), WM_SIZE,
				SIZE_RESTORED, 0);
			InvalidateRect(GetParent(GetParent(hwndClock)), NULL, TRUE);
			return 0;
		case CLOCKM_BLINK: // blink the clock
			if(wParam) { if(nBlink == 0) nBlink = 4; }
			else nBlink = 2;
			if(lParam) { nBlink = 0; }
			return 0;
		case CLOCKM_COPY: // copy format to clipboard
			OnCopy(hwnd, lParam);
			return 0;
		case CLOCKM_REFRESHDESKTOP:
			SetDesktopIcons();
			return 0;
		case CLOCKM_VOLCHANGE:
			if(bGetVol)
			{
				BOOL muteflg;
				GetMasterVolume(&iVolume);
				GetMasterMute(&muteflg);
				if (muteflg)
					iVolume = 0;
				bVolRedraw = TRUE;
				OnTimer(hwnd);
				return 0;
			}
			else
				return 0;
		case CLOCKM_NETINIT:
			Net_restart();	//net.c
			return 0;
		case CLOCKM_DELUSRSTR:
			{
				int i;
				for(i=0;i<10;i++) { lstrcpy(usrstr[i].u_str,""); }
				return 0;
			}
		case WM_COPYDATA:
			OnCopyData((HWND)wParam,(COPYDATASTRUCT*)lParam);
			return 0;
		case WM_WINDOWPOSCHANGING:  // サイズ調整
			{
				static RECT rcOld = { 1,0,0,0 };
				RECT rc;
				GetWindowRect(GetParent(hwnd), &rc);
				if ((bFitClock && EqualRect(&rcOld, &rc)))
				{
					((LPWINDOWPOS)lParam)->flags |= SWP_NOSIZE | SWP_NOMOVE;
					break;
				}
				rcOld = rc;
			}
			if(bNoClock) break;
			if(!bWinXP || !bVisualStyle)
			{
				LPWINDOWPOS pwp;
				pwp = (LPWINDOWPOS)lParam;
				if(IsWindowVisible(hwnd) && !(pwp->flags & SWP_NOSIZE))
				{
					int h;
					h = (int)HIWORD(OnCalcRect(hwnd));
					if(pwp->cy > h) pwp->cy = h;
				}
				break;
			}
			else
			{
				//WinXP Visual Style
				LPWINDOWPOS pwp;
				RECT rc,rcBar;

				pwp = (LPWINDOWPOS)lParam;
				if(IsWindowVisible(hwnd) && !(pwp->flags & SWP_NOSIZE))
				{
					int h;//,w;
					h = (int)HIWORD(OnCalcRect(hwnd));
					//w = (int)LOWORD(OnCalcRect(hwnd));
					if(pwp->cy > h) pwp->cy = h;

					GetWindowRect(GetParent(hwndClock),&rc);
					GetWindowRect(GetParent(GetParent(hwndClock)),&rcBar);

					if((rcBar.right - rcBar.left) > (rcBar.bottom - rcBar.top))
					{
						//横置き
						pwp->y = ((rc.bottom - rc.top) - pwp->cy) / 2;
						pwp->x = ((rc.right - rc.left) - pwp->cx);// - 3;
					}else{
						//縦置き
						pwp->y = ((rc.bottom - rc.top) - pwp->cy);// - 3;
						pwp->x = ((rc.right - rc.left) - pwp->cx) / 2;
					}
				}
				break;
			}
			break;
	}
	return CallWindowProc(oldWndProc, hwnd, message, wParam, lParam);
}

/*------------------------------------------------
　設定の読み込みとデータの初期化
--------------------------------------------------*/
void ReadData(HWND hwnd)
{
	char fontname[80];
	int fontsize;
	LONG weight, italic;
	SYSTEMTIME lt;
	DWORD dwInfoFormat;
	int resnetinterval;

	//設定番号取得
	gConfigNo = GetMyRegLong(NULL,"ConfigNo",1);

	colfore = GetMyRegLongEx(NULL, "ForeColor",
		0x80000000 | COLOR_BTNTEXT, gConfigNo);
	colback = GetMyRegLongEx(NULL, "BackColor",
		0x80000000 | COLOR_3DFACE, gConfigNo);
	if(GetMyRegLongEx(NULL, "UseBackColor2", FALSE, gConfigNo))
		colback2 = GetMyRegLongEx(NULL, "BackColor2", colback, gConfigNo);
	else colback2 = colback;

	{
		BOOL bWin2000;
		DWORD dw = GetVersion();
		bWin2000 = (!(dw & 0x80000000) && LOBYTE(LOWORD(dw)) == 5 && HIBYTE(LOWORD(dw)) == 0);
		fillbackcolor = GetMyRegLongEx(NULL, "UseBackColor", bWin2000, gConfigNo);
	}
	grad = GetMyRegLongEx(NULL, "GradDir", GRADIENT_FILL_RECT_H, gConfigNo);

	bClockShadow = GetMyRegLongEx(NULL, "UseClockShadow", FALSE, gConfigNo);
	bClockBorder = GetMyRegLongEx(NULL, "UseClockBorder", FALSE, gConfigNo);
	colShadow = GetMyRegLongEx(NULL, "ShadowColor", RGB(0, 0, 0), gConfigNo);
	nShadowRange = (int)(short)GetMyRegLongEx(NULL, "ClockShadowRange", 1, gConfigNo);

	bRClickMenu = GetMyRegLong(NULL, "RightClickMenu", TRUE);

	GetMyRegStrEx(NULL, "Font", fontname, 80, "", gConfigNo);

	fontsize = GetMyRegLongEx(NULL, "FontSize", 9, gConfigNo);
	weight = GetMyRegLongEx(NULL, "Bold", 0, gConfigNo);
	if(weight) weight = FW_BOLD;
	else weight = 0;
	italic = GetMyRegLongEx(NULL, "Italic", 0, gConfigNo);

	if(hFon) DeleteObject(hFon);
	hFon = CreateMyFont(fontname, fontsize, weight, italic);

	TooltipReadData();
	if (bTooltipTimerStart) KillTimer(hwndClock, 6); bTooltipTimerStart = FALSE;
	bTooltipTimerStart = SetTimer(hwndClock, 6, 200, NULL) != 0;

	nTextPos = GetMyRegLongEx(NULL, "TextPos", 0, gConfigNo);

	dheight = (int)(short)GetMyRegLongEx(NULL, "ClockHeight", 0, gConfigNo);
	dwidth = (int)(short)GetMyRegLongEx(NULL, "ClockWidth", 0, gConfigNo);
	dvpos = (int)(short)GetMyRegLongEx(NULL, "VertPos", 0, gConfigNo);
	dlineheight = (int)(short)GetMyRegLongEx(NULL, "LineHeight", 0, gConfigNo);
	dclkvpos = (int)(short)GetMyRegLongEx(NULL, "ClockVertPos", 0, gConfigNo);

	//from tclock2
	tEdgeTop = (int)(short)GetMyRegLong("Clock", "ClockSkinEdgeTop", 1);
	if(tEdgeTop < 1) tEdgeTop = 1;
	tEdgeLeft = (int)(short)GetMyRegLong("Clock", "ClockSkinEdgeLeft", 1);
	if(tEdgeLeft < 1) tEdgeLeft = 1;
	tEdgeBottom = (int)(short)GetMyRegLong("Clock", "ClockSkinEdgeBottom", 1);
	if(tEdgeBottom < 1) tEdgeBottom = 1;
	tEdgeRight = (int)(short)GetMyRegLong("Clock", "ClockSkinEdgeRight", 1);
	if(tEdgeRight < 1) tEdgeRight = 1;

	//390 Beep(4000,500);
	bFitClock = GetMyRegLong(NULL, "FitClock", FALSE);

	resnetinterval = GetMyRegLong(NULL, "NetRestartInterval", 0);
	checknet = GetMyRegLong(NULL, "DoNetRestart", FALSE);
	KillTimer(hwndClock, 4);
	if(checknet) SetTimer(hwndClock, 4, resnetinterval*1000, NULL);
	bGraph = GetMyRegLong(NULL, "BackNet", FALSE);
	bLogGraph = GetMyRegLong(NULL, "LogGraph", FALSE);
	bReverseGraph = GetMyRegLong(NULL, "ReverseGraph", FALSE);
	bGraphTate = GetMyRegLong(NULL, "GraphTate", FALSE);
	BackNetBand = GetMyRegLong(NULL, "BackNetBand", 100);
	BackNetBand2 = GetMyRegLong(NULL, "BackNetBand2", 100);
	ColSend = GetMyRegLong(NULL, "BackNetColSend", 0x000000ff);
	ColRecv = GetMyRegLong(NULL, "BackNetColRecv", 0x00ff0000);
	ColSR = GetMyRegLong(NULL, "BackNetColSR", 0x00800080);
	graphInterval = GetMyRegLong(NULL, "GraphInterval", 1);
	graphMode = GetMyRegLong(NULL, "GraphMode", 1);
	cpuHigh = GetMyRegLong(NULL, "CpuHigh", 70);
	GraphL = GetMyRegLong(NULL, "GraphLeft", 0);
	GraphT = GetMyRegLong(NULL, "GraphTop", 0);
	GraphR = GetMyRegLong(NULL, "GraphRight", 1);
	GraphB = GetMyRegLong(NULL, "GraphBottom", 1);
	GraphType = GetMyRegLong(NULL, "GraphType", 1);

	if(bGraphTimerStart) KillTimer(hwndClock, 5); bGraphTimerStart = FALSE;
	if(bGraph)
		bGraphTimerStart = SetTimer(hwndClock, 5, graphInterval*1000, NULL) != 0;

	bNoClock = GetMyRegLong(NULL, "NoClock", FALSE);

	if(!GetMyRegStrEx(NULL, "Format", format, 1024, "", gConfigNo) || !format[0])
	{
		bNoClock = TRUE;
	}

	// 時計の書式は読み込み時に<%～%>を追加する
	{
		char fmt_tmp[1024];
		strcpy(fmt_tmp,"<%");
		strcat(fmt_tmp,format);
		strcat(fmt_tmp,"%>");
		strcpy(format,fmt_tmp);
	}

	if(bWatchTaskWnd) KillTimer(hwndClock, 3);
	bWatchTaskWnd = GetMyRegLong(NULL, "WatchTaskbarWindow", FALSE);
	if(bWatchTaskWnd) SetTimer(hwndClock, 3, 200, NULL);

	dwInfoFormat = FindFormat(format);
	bDispSecond = (dwInfoFormat&FORMAT_SECOND)? TRUE:FALSE;
	nDispBeat = dwInfoFormat & (FORMAT_BEAT1 | FORMAT_BEAT2);
	if(!bTimer) SetTimer(hwndClock, 1, 1000, NULL);
	bTimer = TRUE;

	bHour12 = GetMyRegLong(NULL, "Hour12", 0);
	bHourZero = GetMyRegLong(NULL, "HourZero", 0);

	GetLocalTime(&lt);
	LastTime.wDay = lt.wDay;

	InitFormat(&lt);      // format.c

	iClockWidth = -1;

	InitSysInfo(hwnd);
}

#ifdef USE_FORMAT_FOR_STARTBTN		// add by 505
void InitSysInfo(HWND hwnd)
{
	DWORD dwInfoFormat, dwInfoTip, dwInfoStartBtn;
	DWORD dwInfoCheck;
	BOOL use;
	char temp[80];
	char caption[86];

	if(bTimerSysInfo) KillTimer(hwndClock, 2);
	bDispSysInfo = bTimerSysInfo = FALSE;
	bGetSysRes = bGetBattery = bGetMem = FALSE;
	memset(&msMemory, 0, sizeof(msMemory));

	dwInfoFormat = FindFormat(format);
	dwInfoTip = TooltipFindFormat();

	// キャプションの取得
	use = GetMyRegLong("", "StartButtonIsFormat", FALSE);
	if (use == FALSE) {
		dwInfoStartBtn = 0;
	} else {
		GetMyRegStr(NULL, "StartButtonCaption", temp, 80, "");
		strcpy(caption, "<%");
		strcat(caption, temp);
		strcat(caption, "%>");
		dwInfoStartBtn = FindFormat(caption);
	}
	dwInfoCheck = dwInfoFormat | dwInfoTip | dwInfoStartBtn;

	bGetSysRes = ((dwInfoCheck & FORMAT_SYSINFO))? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_SYSINFO) bDispSysInfo = TRUE;

	if(bWin95 || bWin2000)
	{
		bGetBattery = (dwInfoCheck & FORMAT_BATTERY)? TRUE:FALSE;
		if(dwInfoFormat & FORMAT_BATTERY) bDispSysInfo = TRUE;
	}

	bGetMem = (dwInfoCheck & FORMAT_MEMORY)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_MEMORY) bDispSysInfo = TRUE;

	bGetMb = (dwInfoCheck & FORMAT_MOTHERBRD)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_MOTHERBRD) bDispSysInfo = TRUE;

	bGetPm = (dwInfoCheck & FORMAT_PERMON)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_PERMON) bDispSysInfo = TRUE;

	bGetNet = ((dwInfoCheck & FORMAT_NET)||(bGraph&&graphMode==1))? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_NET) bDispSysInfo = TRUE;

	bGetHdd = (dwInfoCheck & FORMAT_HDD)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_HDD) bDispSysInfo = TRUE;

	bGetCpu = (dwInfoCheck & FORMAT_CPU)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_CPU) bDispSysInfo = TRUE;

	bGetVol = (dwInfoCheck & FORMAT_VOL)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_VOL) bDispSysInfo = TRUE;

	if(bGetSysRes || bGetBattery || bGetMem || bGetMb || bGetPm || bGetNet || bGetHdd || bGetCpu || bGetVol)
	{
		int interval;

		bTimerSysInfo = TRUE;
		UpdateSysRes(hwnd, bGetSysRes, bGetBattery, bGetMem, bGetMb, bGetPm, bGetNet, bGetHdd, bGetCpu, bGetVol);
		interval = (int)GetMyRegLong(NULL, "IntervalSysInfo", 5);
		if(interval <= 0 || 60 < interval) interval = 5;
		SetTimer(hwndClock, 2, interval * 1000, NULL);
	}
}
#else //USE_FORMAT_FOR_STARTBTN	// add by 505
void InitSysInfo(HWND hwnd)
{
	DWORD dwInfoFormat, dwInfoTip;

	if(bTimerSysInfo) KillTimer(hwndClock, 2);
	bDispSysInfo = bTimerSysInfo = FALSE;
	bGetSysRes = bGetBattery = bGetMem = FALSE;
	memset(&msMemory, 0, sizeof(msMemory));

	dwInfoFormat = FindFormat(format);
	dwInfoTip = TooltipFindFormat();

	bGetSysRes = (((dwInfoFormat | dwInfoTip) & FORMAT_SYSINFO))? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_SYSINFO) bDispSysInfo = TRUE;

	if(bWin95 || bWin2000)
	{
		bGetBattery = ((dwInfoFormat | dwInfoTip) & FORMAT_BATTERY)? TRUE:FALSE;
		if(dwInfoFormat & FORMAT_BATTERY) bDispSysInfo = TRUE;
	}

	bGetMem = ((dwInfoFormat | dwInfoTip) & FORMAT_MEMORY)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_MEMORY) bDispSysInfo = TRUE;

	bGetMb = ((dwInfoFormat | dwInfoTip) & FORMAT_MOTHERBRD)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_MOTHERBRD) bDispSysInfo = TRUE;

	bGetPm = ((dwInfoFormat | dwInfoTip) & FORMAT_PERMON)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_PERMON) bDispSysInfo = TRUE;

	bGetNet = (((dwInfoFormat | dwInfoTip) & FORMAT_NET)||(bGraph&&graphMode==1))? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_NET) bDispSysInfo = TRUE;

	bGetHdd = ((dwInfoFormat | dwInfoTip) & FORMAT_HDD)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_HDD) bDispSysInfo = TRUE;

	bGetCpu = ((dwInfoFormat | dwInfoTip) & FORMAT_CPU)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_CPU) bDispSysInfo = TRUE;

	bGetVol = ((dwInfoFormat | dwInfoTip) & FORMAT_VOL)? TRUE:FALSE;
	if(dwInfoFormat & FORMAT_VOL) bDispSysInfo = TRUE;

	if(bGetSysRes || bGetBattery || bGetMem || bGetMb || bGetPm || bGetNet || bGetHdd || bGetCpu || bGetVol)
	{
		int interval;

		bTimerSysInfo = TRUE;
		UpdateSysRes(hwnd, bGetSysRes, bGetBattery, bGetMem, bGetMb, bGetPm, bGetNet, bGetHdd, bGetCpu, bGetVol);
		interval = (int)GetMyRegLong(NULL, "IntervalSysInfo", 5);
		if(interval <= 0 || 60 < interval) interval = 5;
		SetTimer(hwndClock, 2, interval * 1000, NULL);
	}
}
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505

/*------------------------------------------------
　描画用メモリDCの作成
--------------------------------------------------*/
void CreateClockDC(HWND hwnd)
{
	RECT rc;
	COLORREF col;
	HDC hdc;
	char s[1024];

	//390時計サイズ限界まで
	if(bFitClock) { FitClock(); }
	else {
		if(!GetMyRegLong(NULL, "FlatTray", TRUE)){
			//描き直し？
			SetMyRegLong(NULL, "FlatTray",TRUE);
			InitTrayNotify(hwndClock);
			SetMyRegLong(NULL, "FlatTray",FALSE);
			InitTrayNotify(hwndClock);
		}
	}
	//---------

	if(hdcClock)
	{
		if (hbmpClockOld) SelectObject(hdcClock, hbmpClockOld);
		DeleteDC(hdcClock);
		hdcClock = NULL;
	}
	if(hbmpClock) DeleteObject(hbmpClock); hbmpClock = NULL;
	if(hbmpClockSkin) DeleteObject(hbmpClockSkin); hbmpClockSkin = NULL;

	if(bNoClock) return;

	if(bFillTray && (fillbackcolor || bSkinTray))
		hwnd = GetParent(hwnd);

	GetClientRect(hwnd, &rc);

	hdc = GetDC(NULL);

	hdcClock = CreateCompatibleDC(hdc);
	if(!hdcClock)
	{
		ReleaseDC(NULL, hdc);
		return;
	}
	hbmpClock = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);

	if(bSkinTray)
	{
		if(GetMyRegStr("Clock", "ClockSkin", s, 1024, "") > 0)
		{
			PSTR path;
			path = CreateFullPathName(hmod, s);
			if (path == NULL) {
				hbmpClockSkin = ReadBitmap(hwnd, s, TRUE);
			} else {
				hbmpClockSkin = ReadBitmap(hwnd, path, TRUE);
				free(path);
			}
		}
	}

	if(!hbmpClock)
	{
		DeleteDC(hdcClock); hdcClock = NULL;
		ReleaseDC(NULL, hdc);
		return;
	}
	hbmpClockOld = SelectObject(hdcClock, hbmpClock);

	SelectObject(hdcClock, hFon);
	SetBkMode(hdcClock, TRANSPARENT);
	if(nTextPos == 1)
		SetTextAlign(hdcClock, TA_LEFT|TA_TOP);
	else if(nTextPos == 2)
		SetTextAlign(hdcClock, TA_RIGHT|TA_TOP);
	else
		SetTextAlign(hdcClock, TA_CENTER|TA_TOP);
	col = colfore;
	if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
	SetTextColor(hdcClock, col);

	FillClock(hwnd, hdcClock, &rc, 0);

	ReleaseDC(NULL, hdc);
}

/*------------------------------------------------
   get date/time and beat to display
--------------------------------------------------*/
void GetDisplayTime(SYSTEMTIME* pt, int* beat100)
{
	FILETIME ft, lft;
	SYSTEMTIME lt;

	GetSystemTimeAsFileTime(&ft);

	if(beat100)
	{
		DWORDLONG dl;
		SYSTEMTIME st;
		int sec;

		dl = *(DWORDLONG*)&ft + 36000000000;
		FileTimeToSystemTime((FILETIME*)&dl, &st);

		sec = st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
		*beat100 = (sec * 1000) / 864;
	}

	FileTimeToLocalFileTime(&ft, &lft);
	FileTimeToSystemTime(&lft, &lt);
	memcpy(pt, &lt, sizeof(lt));
}

/*------------------------------------------------
　WM_TIMER の処理
--------------------------------------------------*/
void OnTimer(HWND hwnd)
{
	SYSTEMTIME t;
	int beat100 = 0;
	HDC hdc;
	BOOL bRedraw;

	GetDisplayTime(&t, nDispBeat?(&beat100):NULL);

	if((t.wMilliseconds > 200 || (bWinNT && t.wMilliseconds > 50))&&(!bVolRedraw))
	{
		KillTimer(hwnd, 1);
		bTimerTesting = TRUE;
		SetTimer(hwnd, 1, 1001 - t.wMilliseconds, NULL);
	}
	else if(bTimerTesting)
	{
		KillTimer(hwnd, 1);
		bTimerTesting = FALSE;
		SetTimer(hwnd, 1, 1000, NULL);
	}
	if (bVolRedraw)
		bVolRedraw = FALSE;
	if(CheckDaylightTimeTransition(&t))
	{
		CallWindowProc(oldWndProc, hwnd, WM_TIMER, 0, 0);
		GetDisplayTime(&t, nDispBeat?(&beat100):NULL);
	}

	bRedraw = FALSE;
	if(nBlink > 0) bRedraw = TRUE;
	else if(bDispSecond) bRedraw = TRUE;
	else if(nDispBeat == FORMAT_BEAT1 && beatLast != (beat100/100)) bRedraw = TRUE;
	else if(nDispBeat == FORMAT_BEAT2 && beatLast != beat100) bRedraw = TRUE;
	else if(bDispSysInfo) bRedraw = TRUE;
	else if(LastTime.wHour != (int)t.wHour
		|| LastTime.wMinute != (int)t.wMinute) bRedraw = TRUE;
	else if(bGraphRedraw)
	{
		bGraphRedraw = FALSE;
		bRedraw = TRUE;
	}

	if(bNoClock) bRedraw = FALSE;

	if(LastTime.wDay != t.wDay || LastTime.wMonth != t.wMonth ||
		LastTime.wYear != t.wYear)
	{
		InitFormat(&t); // format.c
		InitDaylightTimeTransition();
	}

	hdc = NULL;
	if(bRedraw) hdc = GetDC(hwnd);

	memcpy(&LastTime, &t, sizeof(t));

	if(nDispBeat == FORMAT_BEAT1) beatLast = beat100/100;
	else if(nDispBeat == FORMAT_BEAT2) beatLast = beat100;

	if(nBlink >= 3 && t.wMinute == 1) nBlink = 0;
	if(hdc)
	{
		DrawClockSub(hwnd, hdc, &t, beat100); //描画
		DrawClockFocusRect(hwnd, hdc);
		ReleaseDC(hwnd, hdc);
	}

	if(nBlink)
	{
		if(nBlink % 2) nBlink++;
		else nBlink--;
	}

	CheckCursorOnStartButton();

	SetDesktopIcons();
}

void OnTimerUpperTaskbar(HWND hwnd)
{
	RECT rectTaskbar, rectWnd, rectClient;
	HWND hActiveWnd;
  
	UNREFERENCED_PARAMETER(hwnd);

	if (!hwndTaskbar) return;
	if (!GetWindowRect(hwndTaskbar, &rectTaskbar)) return;

	hActiveWnd = GetForegroundWindow();
	if (!GetWindowRect(hActiveWnd, &rectWnd)) return;
	if (rectWnd.top == rectWnd.bottom) return;
	if (rectWnd.left == rectWnd.right) return;
	if (IsZoomed(hActiveWnd)
	 || !IsWindowVisible(hActiveWnd)
	 || (rectWnd.bottom-rectWnd.top) >= GetSystemMetrics(SM_CYFULLSCREEN)
	 || (rectWnd.right-rectWnd.left) >= GetSystemMetrics(SM_CXFULLSCREEN))
		return;
	//「デスクトップの表示」で最小化されたウィンドウを復元したときに
	//ウィンドウ位置がおかしくなる問題回避
	if (!GetClientRect(hActiveWnd, &rectClient)) return;
	if (rectClient.right == 0 && rectClient.bottom == 0) return;

	if (rectTaskbar.bottom <= (GetSystemMetrics(SM_CYSCREEN)/2))
	{
		//上タスクバー
		if (rectWnd.top < rectTaskbar.bottom)
		{
			MoveWindow(hActiveWnd, rectWnd.left, rectTaskbar.bottom,
				rectWnd.right-rectWnd.left, rectWnd.bottom-rectWnd.top, TRUE);
		}
	}
	else
	if (rectTaskbar.right <= (GetSystemMetrics(SM_CXSCREEN)/2))
	{
		//左タスクバー
		if (rectWnd.left < rectTaskbar.right)
		{
			MoveWindow(hActiveWnd, rectTaskbar.right, rectWnd.top,
				rectWnd.right-rectWnd.left, rectWnd.bottom-rectWnd.top, TRUE);
		}
	}
	else
	if (rectTaskbar.left >= (GetSystemMetrics(SM_CXSCREEN)/2))
	{
		//右タスクバー
		if (rectWnd.right > rectTaskbar.left)
		{
			MoveWindow(hActiveWnd, rectTaskbar.left-(rectWnd.right-rectWnd.left) , rectWnd.top,
				rectWnd.right-rectWnd.left, rectWnd.bottom-rectWnd.top, TRUE);
		}
	}
	else
	{
		//下タスクバー
		if (rectWnd.bottom > rectTaskbar.top)
		{
			MoveWindow(hActiveWnd, rectWnd.left, rectTaskbar.top-(rectWnd.bottom-rectWnd.top),
				rectWnd.right-rectWnd.left, rectWnd.bottom-rectWnd.top, TRUE);
		}
	}
}

/*------------------------------------------------
　時計の描画
--------------------------------------------------*/
void DrawClock(HWND hwnd, HDC hdc)
{
	SYSTEMTIME t;
	int beat100 = 0;

	GetDisplayTime(&t, nDispBeat?(&beat100):NULL);
	DrawClockSub(hwnd, hdc, &t, beat100);
	DrawClockFocusRect(hwnd, hdc);
}

#ifdef USE_ANALOG_CLOCK	// add by 505

static POINT ptMinHand[15] = {
	{  9,  1 },	//  0
	{ 10,  1 },	//  1
	{ 11,  2 },	//  2
	{ 12,  2 }, //  3
	{ 12,  3 }, //  4
	{ 13,  3 }, //  5
	{ 13,  3 }, //  6
	{ 14,  4 }, //  7
	{ 14,  4 }, //  8
	{ 15,  5 }, //  9
	{ 15,  5 }, // 10
	{ 16,  6 }, // 11
	{ 16,  6 }, // 12
	{ 17,  7 }, // 13
	{ 17,  8 }  // 14
};

static POINT ptHourHand[15] = {
	{  9,  1 },	//  0
	{ 10,  1 },	//  1
	{ 11,  2 },	//  2
	{ 12,  2 }, //  3
	{ 12,  3 }, //  4
	{ 13,  3 }, //  5
	{ 13,  3 }, //  6
	{ 14,  4 }, //  7
	{ 14,  4 }, //  8
	{ 15,  5 }, //  9
	{ 15,  5 }, // 10
	{ 16,  6 }, // 11
	{ 16,  6 }, // 12
	{ 17,  7 }, // 13
	{ 17,  8 }  // 14
};

static void GetHnadLinePos(WORD index, WORD sector, POINT tbl[], POINT pos[], int dx, int dy)
{
	int cx, cy;
	int sx, sy;
	int x, y;

	x = tbl[index].x;
	y = tbl[index].y;
	cx = (sizeAClock.cx / 2);
	cy = (sizeAClock.cy / 2);
	sx = x;
	sy = y;
	switch (sector) {
	case 0:
		sx = x;
		sy = y;
		//cy += 1;
		break;
	case 1:
		sx = (sizeAClock.cx) - y;
		sy = x;
		//cx -= 1;
		break;
	case 2:
		sx = (sizeAClock.cx) - x;
		sy = (sizeAClock.cy)- y;
		//cy -= 1;
		break;
	case 3:
		sx = y;
		sy = (sizeAClock.cy) - x;
		//cx += 1;
		break;
	}
	pos[0].x = cx + dx;
	pos[0].y = cy + dy;
	pos[1].x = sx + dx;
	pos[1].y = sy + dy;
}

//ビットマップヘッダーのチェック
static BOOL CheckBitmapHeader(LPBYTE top, DWORD dwFileSize)
{
	LPBITMAPFILEHEADER lpbmfh;
	LPBITMAPINFOHEADER lpbmih;
	LPBYTE			  lpOffBits;
	RGBQUAD			 *lprgb;

	if (dwFileSize <= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) {
		// サイズ異常
		return FALSE;
	}

	lpbmfh = (BITMAPFILEHEADER *)top;
	if (lpbmfh->bfType != 0x4D42) { // BM
		//ヘッダー異常
		return FALSE;
	}
	if (lpbmfh->bfSize <= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)) {
		// サイズ異常
		return FALSE;
	}
	lpOffBits = (LPBYTE)(top + lpbmfh->bfOffBits);
	lpbmih = (BITMAPINFOHEADER *)(top + sizeof(BITMAPFILEHEADER));
	lprgb = (RGBQUAD *)(lpbmih + sizeof(BITMAPINFOHEADER));

	if (lpbmih->biSize != sizeof(BITMAPINFOHEADER)) {
		// ビットマップインフォヘッダーサイズ異常
		return FALSE;
	}

	if (lpbmih->biHeight == 0) {
		//ビットマップの高さの異常
		return FALSE;
	}

	if (lpbmih->biWidth == 0) {
		//ビットマップの幅の異常
		return FALSE;
	}
	switch (lpbmih->biBitCount) {
		case 1:
		case 4:
		case 8:
		case 16:
		case 24:
		case 32:
			break;
		default:
			//ピクセルサイズの異常
			return FALSE;
	}
	return TRUE;
}

static HBITMAP ReadBitmapData(HDC hDC, LPBYTE top, DWORD dwFileSize, SIZE *psize)
{
	LPBITMAPFILEHEADER lpbmfh;
	LPBITMAPINFO lpbmi;
	LPBITMAPINFOHEADER lpbmih;
	LPBYTE ptr;
	HBITMAP hBitmap;

	if (top == NULL) {
		return NULL;
	}
	if (!CheckBitmapHeader(top, dwFileSize)) {
		return NULL;
	}

	lpbmfh = (LPBITMAPFILEHEADER)top;
	lpbmih = (LPBITMAPINFOHEADER)(lpbmfh + 1);
	lpbmi = (LPBITMAPINFO)lpbmih;

	psize->cx = lpbmih->biWidth;
	psize->cy = lpbmih->biHeight;

	ptr = top + lpbmfh->bfOffBits;

	hBitmap = CreateDIBitmap(
		hDC,
		lpbmih,
		CBM_INIT,
		ptr,
		lpbmi,
		DIB_RGB_COLORS
	);
	if (hBitmap == NULL) {
		return NULL;
	}
	return hBitmap;
}

static HBITMAP ReadBitmapFile(HDC hDC, LPTSTR lpszFileName, SIZE *psize)
{
	HANDLE hFile;
	HANDLE hMapFile;
	LPBYTE ptr;
	DWORD dwFileSize;
	HBITMAP hBitmap;

	if (*lpszFileName == '\0') {
		return NULL;
	}
	hFile = CreateFile(
		lpszFileName,			// lpszName
		GENERIC_READ,			// fdwAccess
		FILE_SHARE_READ,		// fdwShareMode
		NULL,					// lpsa
		OPEN_EXISTING,			// fdwCreate
		FILE_ATTRIBUTE_NORMAL,	// fdwAttrsAndFlags
		NULL					// hTemplateFile
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize == 0) {
		CloseHandle(hFile);
		return NULL;
	}

	hMapFile = CreateFileMapping(
		(HANDLE)hFile,
		NULL,
		PAGE_READONLY,
		0, 0,
		NULL
	);
	if (hMapFile == NULL) {
		CloseHandle(hFile);
		return NULL;
	}

	ptr = (LPBYTE)MapViewOfFile(
		hMapFile,
		FILE_MAP_READ,
		0, 0, 0
	);
	if (ptr == NULL) {
		CloseHandle(hMapFile);
		CloseHandle(hFile);
	}

	hBitmap = ReadBitmapData(hDC, ptr, dwFileSize, psize);

	UnmapViewOfFile(ptr);

	CloseHandle(hMapFile);

	CloseHandle(hFile);

	return hBitmap;
}

static HBITMAP Create24BitBitmap(HDC hDC, SIZE *psize, LPBYTE *ppImgBuf)
{
	HBITMAP hBitmap;
	BITMAPINFO bmi = {
		{	// bmiHeader
			sizeof(BITMAPINFOHEADER),	// DWORD	biSize;
			0,							// LONG		biWidth;
			0,							// LONG		biHeight;
			1,							// WORD		biPlanes;
			24,							// WORD		biBitCount
			BI_RGB,						// DWORD	biCompression;
			0,							// DWORD	biSizeImage;
			0,							// LONG		biXPelsPerMeter;
			0,							// LONG		biYPelsPerMeter;
			0,							// DWORD	biClrUsed;
			0							// DWORD	biClrImportant;
		}, {	// bmiColors
			{
				0x00, 0x00, 0x00
			}
		}
	};

	bmi.bmiHeader.biWidth = psize->cx;
	bmi.bmiHeader.biHeight = psize->cy;
	bmi.bmiHeader.biSizeImage = WIDTHBYTES(bmi.bmiHeader.biBitCount * psize->cx) * psize->cy;

	hBitmap = CreateDIBSection(
		hDC,
		&bmi,
		DIB_RGB_COLORS,
		ppImgBuf,
		NULL,
		0
	);
//	 GdiFlush();
	return hBitmap;
}

static VOID ModifyMaskImage(SIZE *psize, LPBYTE buf)
{
	int x, y;
	int bw;
	LPBYTE ptr;
	RGBTRIPLE *prgb;
	int cy;

	if (buf == NULL) {
		return;
	}

	bw = WIDTHBYTES(24 * psize->cx);

	ptr = buf;
	cy = psize->cy / 2;
	//mask
	for (y = 0; y < cy; y++) {
		prgb = (RGBTRIPLE *)ptr;
		for (x = 0; x < psize->cx; x++) {
			if (
				(prgb->rgbtBlue  == 0xFF) &&
				(prgb->rgbtGreen == 0x00) &&
				(prgb->rgbtRed   == 0xFF)
			) {
				prgb->rgbtBlue  = 0xFF;
				prgb->rgbtGreen = 0xFF;
				prgb->rgbtRed   = 0xFF;
			} else {
				prgb->rgbtBlue  = 0x00;
				prgb->rgbtGreen = 0x00;
				prgb->rgbtRed   = 0x00;
			}
			prgb++;
		}
		ptr += bw;
	}
	//invert
	for (y = 0; y < cy; y++) {
		prgb = (RGBTRIPLE *)ptr;
		for (x = 0; x < psize->cx; x++) {
			if (
				(prgb->rgbtBlue  == 0xFF) &&
				(prgb->rgbtGreen == 0x00) &&
				(prgb->rgbtRed   == 0xFF)
			) {
				prgb->rgbtBlue  = 0x00;
				prgb->rgbtGreen = 0x00;
				prgb->rgbtRed   = 0x00;
			}
			prgb++;
		}
		ptr += bw;
	}
}

static BOOL CreateAnalogClockDC(HWND hWnd, HDC hDC, LPTSTR fname)
{
	SIZE size;
	SIZE msize;
	PSTR path;

	UNREFERENCED_PARAMETER(hWnd);

	if (hdcAnalogClock) {
		DeleteDC(hdcAnalogClock);
		hdcAnalogClock = NULL;
	}
	if (hbmpAnalogClock) {
		DeleteObject(hbmpAnalogClock);
		hbmpAnalogClock = NULL;
	}
	if (hdcAnalogClockMask) {
		DeleteDC(hdcAnalogClockMask);
		hdcAnalogClockMask = NULL;
	}
	if (hbmpAnalogClockMask) {
		DeleteObject(hbmpAnalogClockMask);
		hbmpAnalogClockMask = NULL;
		lpbyAnalogClockMask = NULL;
	}

	if (*fname == '\0') {
		return FALSE;
	}

	path = CreateFullPathName(hmod, fname);
	if (path == NULL) {
		hbmpAnalogClock = ReadBitmapFile(hDC, fname, &size);
	} else {
		hbmpAnalogClock = ReadBitmapFile(hDC, path, &size);
		free(path);
	}
	if (hbmpAnalogClock == NULL) {
		hdcAnalogClock = NULL;
		return TRUE;
	}
	sizeAnalogBitmapSize = size;
	msize = size;
	msize.cy *= 2;
	hbmpAnalogClockMask = Create24BitBitmap(hDC, &msize, &lpbyAnalogClockMask);
	if (hbmpAnalogClockMask == NULL) {
		lpbyAnalogClockMask = NULL;
		DeleteObject(hbmpAnalogClock);
		hbmpAnalogClock = NULL;
		return FALSE;
	}

	hdcAnalogClock = CreateCompatibleDC(hDC);
	if (hdcAnalogClock == NULL) {
		DeleteObject(hbmpAnalogClockMask);
		hbmpAnalogClockMask = NULL;
		lpbyAnalogClockMask = NULL;
		DeleteObject(hbmpAnalogClock);
		hbmpAnalogClock = NULL;
		return FALSE;
	}

	hdcAnalogClockMask = CreateCompatibleDC(hDC);
	if (hdcAnalogClockMask == NULL) {
		DeleteDC(hdcAnalogClock);
		hdcAnalogClock = NULL;
		DeleteObject(hbmpAnalogClockMask);
		hbmpAnalogClockMask = NULL;
		lpbyAnalogClockMask = NULL;
		DeleteObject(hbmpAnalogClock);
		hbmpAnalogClock = NULL;
		return FALSE;
	}

	SelectObject(hdcAnalogClock, hbmpAnalogClock);
	SelectObject(hdcAnalogClockMask, hbmpAnalogClockMask);

	BitBlt(hdcAnalogClockMask, 0,       0, size.cx, size.cy, hdcAnalogClock, 0, 0, SRCCOPY);
	BitBlt(hdcAnalogClockMask, 0, size.cy, size.cx, size.cy, hdcAnalogClock, 0, 0, SRCCOPY);

	ModifyMaskImage(&msize, lpbyAnalogClockMask);

	if (hdcAnalogClock) {
		DeleteDC(hdcAnalogClock);
		hdcAnalogClock = NULL;
	}
	if (hbmpAnalogClock) {
		DeleteObject(hbmpAnalogClock);
		hbmpAnalogClock = NULL;
	}

	return TRUE;
}

static double ctbl[15] = {
	0.000000,
	0.104528,
	0.207912,
	0.309017,
	0.406737,
	0.500000,
	0.587785,
	0.669131,
	0.743145,
	0.809017,
	0.866025,
	0.913545,
	0.951057,
	0.978148,
	0.994522
};

static double stbl[15] = {
	0.000000,
	0.994522,
	0.978148,
	0.951057,
	0.913545,
	0.866025,
	0.809017,
	0.743145,
	0.669131,
	0.587785,
	0.500000,
	0.406737,
	0.309017,
	0.207912,
	0.104528
};

static VOID MakePosTable(int cx, int cy)
{
	int x, y;
	int x2, y2;
	double c, s;
	int i;
	int r;
	int sr;

	UNREFERENCED_PARAMETER(cy);

	// x2 = x * cosA - y * sinA
	// y2 = x * sinA + y * cosA
	// ラジアン = (角度)＊（π/180）
	r = cx / 2;
	sr = r;

	x = sr;
	y = 0;
	for (i = 14; i > 0; i--) {
		c = ctbl[i];
		s = stbl[i];
		x2 = (int)(x * c - y * s);
		y2 = (int)(x * s + y * c);
		ptMinHand[i].x = x2 + r;
		ptMinHand[i].y = r - y2;
	}
	ptMinHand[0].x = r;
	ptMinHand[0].y = r - sr;

	r = cx / 2;
	sr = r * 7 / 10;

	x = sr;
	y = 0;
	for (i = 14; i > 0; i--) {
		c = ctbl[i];
		s = stbl[i];
		x2 = (int)(x * c - y * s);
		y2 = (int)(x * s + y * c);
		ptHourHand[i].x = x2 + r;
		ptHourHand[i].y = r - y2;
	}
	ptHourHand[0].x = r;
	ptHourHand[0].y = r - sr;
}

static VOID SetAnalogClockSize(SIZE *s, int confNo)
{
	s->cy = s->cx = (int)(short)GetMyRegLongEx("", "AnalogClockSize", ACLOCK_SIZE_CX, confNo);
	if (s->cy <= 0) {
		s->cx = ACLOCK_SIZE_CX;
		s->cy = ACLOCK_SIZE_CY;
	}
	MakePosTable(s->cx, s->cy);
}

static BOOL InitAnalogClockData(HWND hWnd)
{
	BOOL use;
	int confNo;
	TCHAR fname[MAX_PATH];
	HDC hDC;

	confNo = GetMyRegLong("", "ConfigNo", 1);

	use = GetMyRegLongEx("", "UseAnalogClock", FALSE, confNo);
	if (use == FALSE) {
		nAnalogClockUseFlag = ANALOG_CLOCK_NOTUSE;
		bAnalogClockAtStartBtn = FALSE;
		nAnalogClockPos = ANALOG_CLOCK_POS_MIDDLE;
		sizeAClock.cx = 0;
		sizeAClock.cy = 0;
		return FALSE;
	}

	nAnalogClockHPos = (int)(short)GetMyRegLongEx("", "AnalogClockHPos", 0, confNo);
	nAnalogClockVPos = (int)(short)GetMyRegLongEx("", "AnalogClockVPos", 0, confNo);
	nAnalogClockPos = (int)(short)GetMyRegLongEx("", "AnalogClockPos", 0, confNo);

	colAClockHourHandColor = (COLORREF)GetMyRegLongEx("", "AClockHourHandColor", (LONG)RGB(255, 0, 0), confNo);
	colAClockMinHandColor = (COLORREF)GetMyRegLongEx("", "AClockMinHandColor", (LONG)RGB(0, 0, 255), confNo);

	if (hpenHour) {
		DeleteObject(hpenHour);
	}
	if (hpenMin) {
		DeleteObject(hpenMin);
	}

	if (GetMyRegLongEx("", "AnalogClockHourHandBold", FALSE, confNo)) {
		nHourPenWidth = 2;
	} else {
		nHourPenWidth = 1;
	}
	if (GetMyRegLongEx("", "AnalogClockMinHandBold", FALSE, confNo)) {
		nMinPenWidth = 2;
	} else {
		nMinPenWidth = 1;
	}

	hpenHour = CreatePen(PS_SOLID, nHourPenWidth, colAClockHourHandColor);
	hpenMin = CreatePen(PS_SOLID, nMinPenWidth, colAClockMinHandColor);

	SetAnalogClockSize(&sizeAClock, confNo);

	GetMyRegStrEx("", "AnalogClockBmp", fname, MAX_PATH, "tclock.bmp", confNo);
	lstrcpy(szAnalogClockBmp, fname);

	hDC = GetDC(hWnd);
	CreateAnalogClockDC(hWnd, hDC, szAnalogClockBmp);
	ReleaseDC(hWnd, hDC);

	nAnalogClockUseFlag = ANALOG_CLOCK_USE;
	bAnalogClockAtStartBtn = GetMyRegLongEx("", "AnalogClockAtStartBtn", FALSE, confNo);
	return TRUE;
}

static VOID DrawAnalogClockHand(HDC hDC, int dx, int dy, SYSTEMTIME* pt)
{
	HPEN hpenOld;
	WORD index;
	WORD sector;
	POINT posPoly[2];

	hpenOld = SelectObject(hDC, hpenHour);

	index = ((pt->wHour % 12) * 5) + (pt->wMinute / 12);
	sector = index / 15;
	index %= 15;
	GetHnadLinePos(index, sector, ptHourHand, posPoly, dx, dy);

	Polyline(hDC, posPoly, 2);

	SelectObject(hDC, hpenMin);

	index  = pt->wMinute % 15;
	sector = pt->wMinute / 15;
	GetHnadLinePos(index, sector, ptMinHand, posPoly, dx, dy);
	Polyline(hDC, posPoly, 2);

	SelectObject(hDC, hpenOld);
}

static VOID AnalogClockBlt(HDC hDC, int dx, int dy, SIZE *pdst, int sx, int sy, SIZE *psrc)
{
	if (
		(pdst->cx == sizeAnalogBitmapSize.cx) &&
		(pdst->cy == sizeAnalogBitmapSize.cy)
	) {
		BitBlt(
			hDC,
			dx, dy, pdst->cx, pdst->cy,
			hdcAnalogClockMask,
			sx, sy + sizeAnalogBitmapSize.cy,
			SRCAND
		);
		BitBlt(
			hDC,
			dx, dy, pdst->cx, pdst->cy,
			hdcAnalogClockMask,
			sx, sy,
			SRCINVERT
		);
	} else {
		StretchBlt(
			hDC,
			dx, dy, pdst->cx, pdst->cy,
			hdcAnalogClockMask,
			sx, sy + sizeAnalogBitmapSize.cy,
			psrc->cx, psrc->cy,
			SRCAND
		);
		StretchBlt(
			hDC,
			dx, dy,
			pdst->cx, pdst->cy,
			hdcAnalogClockMask,
			sx, sy,
			psrc->cx, psrc->cy,
			SRCINVERT
		);
	}
}

BOOL DrawAnalogClock(HDC hDC, SYSTEMTIME* pt, int xclock, int yclock, int wclock, int hclock)
{
//	RECT r;
	int sx, sy, dx, dy;
	SIZE clock_size;
	SIZE bitmapsize;

	UNREFERENCED_PARAMETER(hclock);

	if (hdcAnalogClockMask != NULL) {

		clock_size = sizeAClock;
		bitmapsize = sizeAnalogBitmapSize;

		if (nAnalogClockPos == ANALOG_CLOCK_POS_RIGHT) {
			sx = 0;
			dx = xclock + wclock - sizeAClock.cx + nAnalogClockHPos;
			if (dx >= xclock + wclock) {
				return FALSE;
			}
			if (dx < 0) {
				if (dx + sizeAClock.cx <= 0) {
					return FALSE;
				}
				sx = 0 - dx;
				clock_size.cx = clock_size.cx + dx;
				//clock_size.cx = clock_size.cx + dx;
				sx = sx * sizeAnalogBitmapSize.cx / sizeAClock.cx;
				dx = xclock;
				bitmapsize.cx = bitmapsize.cx - sx;
			}
		} else {
			if (nAnalogClockHPos < 0) {
				dx = xclock;
				if (nAnalogClockHPos + sizeAClock.cx <= 0) {
					return FALSE;
				}
				sx = 0 - nAnalogClockHPos;
				clock_size.cx = sizeAClock.cx + nAnalogClockHPos;
				sx = sx * sizeAnalogBitmapSize.cx / sizeAClock.cx;
				bitmapsize.cx = bitmapsize.cx - sx;
			} else {
				sx = 0;
				dx = xclock + nAnalogClockHPos;
			}
		}

		if (nAnalogClockVPos < 0) {
			dy = yclock;
			if (nAnalogClockVPos + sizeAClock.cy <= 0) {
				return FALSE;
			}
			sy = 0 - nAnalogClockVPos;
			clock_size.cy = sizeAClock.cy + nAnalogClockVPos;
			sy = sy * sizeAnalogBitmapSize.cy / sizeAClock.cy;
			bitmapsize.cy = bitmapsize.cy - sy;
		} else {
			sy = 0;
			dy = yclock + nAnalogClockVPos;
		}
		AnalogClockBlt(hDC, dx, dy, &clock_size, sx, sy, &bitmapsize);
	}

	if (nAnalogClockPos == ANALOG_CLOCK_POS_RIGHT) {
		dx = xclock + wclock - sizeAClock.cx + nAnalogClockHPos;
		dy = yclock + nAnalogClockVPos;
	} else {
		dx = xclock + nAnalogClockHPos;
		dy = yclock + nAnalogClockVPos;
	}
	DrawAnalogClockHand(hDC, dx, dy, pt);

	return TRUE;
}

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
extern void RefreshStartBtnXp(void);
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505

static BOOL LocalDrawAnalogClock(HDC hDC, SYSTEMTIME* pt, int xclock, int yclock, int wclock, int hclock)
{
//	RECT r;
	static WORD lastSec = 0xFFFF;
	static WORD lastMin = 0xFFFF;
	static WORD lastHour = 0xFFFF;
	BOOL bRedraw;

	if (hwndStart != NULL) {
		bRedraw = FALSE;
#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
		if (bUseFormatForStartBtn != FALSE) {
			if (bStartBtnDispOther != FALSE) {
				bRedraw = TRUE;
			} else if (bStartBtnDispSecond == FALSE) {
				if ((pt->wHour != lastHour) || (pt->wMinute != lastMin)) {
					lastHour = pt->wHour;
					lastMin  = pt->wMinute;
					bRedraw = TRUE;
				}
			} else {
				if ((pt->wHour != lastHour) || (pt->wMinute != lastMin) || (pt->wSecond != lastSec)) {
					lastSec = pt->wSecond;
					lastHour = pt->wHour;
					lastMin  = pt->wMinute;
					bRedraw = TRUE;
				}
			}
		} else {
			if (nAnalogClockUseFlag == ANALOG_CLOCK_USE) {
				if (bAnalogClockAtStartBtn != FALSE) {
					if ((pt->wHour != lastHour) || (pt->wMinute != lastMin)) {
						lastHour = pt->wHour;
						lastMin  = pt->wMinute;
						bRedraw = TRUE;
					}
				}
			}
		}
#else //USE_FORMAT_FOR_STARTBTN	// add by 505
		if ((pt->wHour != lastHour) || (pt->wMinute != lastMin)) {
			lastHour = pt->wHour;
			lastMin  = pt->wMinute;
			bRedraw = TRUE;
		}
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505
		if (bRedraw != FALSE) {
			InvalidateRect(hwndStart, NULL, FALSE);
		}
	}

	if (nAnalogClockUseFlag != ANALOG_CLOCK_USE) {
		return FALSE;
	}

	if (bAnalogClockAtStartBtn != FALSE) {
		return TRUE;
	}
	return DrawAnalogClock(hDC, pt, xclock, yclock, wclock, hclock);
}
#endif // USE_ANALOG_CLOCK	// add by 505

static void DrawClockFocusRect(HWND hwnd, HDC hdc)
{
	if (bGetingFocus != FALSE) {
		RECT rc;

		GetClientRect(hwnd, &rc);
		DrawFocusRect(hdc, &rc);
	}
}

/*------------------------------------------------
  draw the clock
--------------------------------------------------*/
void DrawClockSub(HWND hwnd, HDC hdc, SYSTEMTIME* pt, int beat100)
{
	BITMAP bmp;
	RECT rcFill,  rcClock;

	TEXTMETRIC tm;
	int hf, y, w;
	char s[1024], *p, *sp;
	SIZE sz;
	int xclock, yclock, wclock, hclock, xsrc, ysrc, wsrc, hsrc;
	int xcenter;
	HRGN hRgn = NULL, hOldRgn = NULL;
	COLORREF textcol, textshadow;

	textcol = colfore;
	if(textcol & 0x80000000) textcol = GetSysColor(textcol & 0x00ffffff);
	textshadow = colShadow;
	if(textshadow & 0x80000000) textshadow = GetSysColor(textshadow & 0x00ffffff);

	// InitClockの最後に移動
//	if(!hdcClock) CreateClockDC(hwnd);

	if(!hdcClock || !hbmpClock) return;

	GetObject(hbmpClock, sizeof(BITMAP), (LPVOID)&bmp);
	rcFill.left = rcFill.top = 0;
	rcFill.right = bmp.bmWidth; rcFill.bottom = bmp.bmHeight;

	FillClock(hwnd, hdcClock, &rcFill, nBlink);

	MakeFormat(s, pt, beat100, format);

	GetClientRect(hwndClock, &rcClock);

	if(bFillTray && (fillbackcolor || bSkinTray))
	{
		POINT ptTray, ptClock;
		ptTray.x = ptTray.y = 0;
		ClientToScreen(GetParent(hwndClock), &ptTray);
		ptClock.x = ptClock.y = 0;
		ClientToScreen(hwndClock, &ptClock);
		xclock = ptClock.x - ptTray.x;
		yclock = ptClock.y - ptTray.y;
		wclock = rcClock.right;
		hclock = rcClock.bottom; //rcClock.bottom;

#ifdef USE_ANALOG_CLOCK	// add by 505
		if (
			(nAnalogClockUseFlag == ANALOG_CLOCK_USE) &&
			(bAnalogClockAtStartBtn == FALSE)
		) {
			if (nAnalogClockPos == ANALOG_CLOCK_POS_MIDDLE) {
				hRgn = CreateRectRgn(xclock, 0,
					xclock + wclock, yclock + hclock);
			} else {
				hRgn = CreateRectRgn(xclock, 0,
					xclock + wclock + sizeAClock.cx, yclock + hclock);
			}
		} else {
			hRgn = CreateRectRgn(xclock, 0,
				xclock + wclock, yclock + hclock);
		}
		hOldRgn = SelectObject(hdcClock, hRgn);
#else // USE_ANALOG_CLOCK	// add by 505
		hRgn = CreateRectRgn(xclock, 0,
			xclock + wclock, yclock + hclock);
		hOldRgn = SelectObject(hdcClock, hRgn);
#endif // USE_ANALOG_CLOCK	// add by 505
	}
	else
	{
		xclock = 0;
		yclock = 0;
		wclock = rcClock.right;
		hclock = rcClock.bottom; //rcClock.bottom;
	}
#ifdef USE_ANALOG_CLOCK	// add by 505
	if (
		(nAnalogClockUseFlag == ANALOG_CLOCK_USE) &&
		(bAnalogClockAtStartBtn == FALSE)
	) {
		if (nAnalogClockPos == ANALOG_CLOCK_POS_MIDDLE) {
			if(bGraph&&BackNetBand>0&&(xclock+wclock)>0&&(yclock+hclock)>0)
				DrawGraph(hwnd,hdcClock,xclock,yclock,wclock,hclock);
		} else {
			if(bGraph&&BackNetBand>0&&(xclock+wclock)>0&&(yclock+hclock)>0)
				DrawGraph(hwnd,hdcClock,xclock,yclock,wclock+sizeAClock.cx,hclock);
		}
	} else {
		if(bGraph&&BackNetBand>0&&(xclock+wclock)>0&&(yclock+hclock)>0)
			DrawGraph(hwnd,hdcClock,xclock,yclock,wclock,hclock);
	}
#else // USE_ANALOG_CLOCK	// add by 505
	if(bGraph&&BackNetBand>0&&(xclock+wclock)>0&&(yclock+hclock)>0)
		DrawGraph(hwnd,hdcClock,xclock,yclock,wclock,hclock);
#endif // USE_ANALOG_CLOCK	// add by 505


	GetTextMetrics(hdcClock, &tm);
	hf = tm.tmHeight - tm.tmInternalLeading;
	p = s;
	y = hf / 4 - tm.tmInternalLeading / 2 + yclock;

#ifdef USE_ANALOG_CLOCK	// add by 505
	if (
		(nAnalogClockUseFlag == ANALOG_CLOCK_USE) &&
		(bAnalogClockAtStartBtn == FALSE)
	) {
		if (nAnalogClockPos == ANALOG_CLOCK_POS_MIDDLE) {
			if(nTextPos == 1) {
				xcenter = xclock;
			} else if(nTextPos == 2) {
				xcenter = wclock + xclock - nShadowRange;
			} else {
				xcenter = wclock / 2 + xclock;
			}
		} else if (nAnalogClockPos == ANALOG_CLOCK_POS_LEFT) {
			if(nTextPos == 1) {
				xcenter = xclock + sizeAClock.cx;
			} else if(nTextPos == 2) {
				xcenter = wclock + xclock - nShadowRange;
			} else {
				xcenter = (wclock - sizeAClock.cx) / 2 + xclock + sizeAClock.cx;
			}
		} else {
			if(nTextPos == 1) {
				xcenter = xclock;
			} else if(nTextPos == 2) {
				xcenter = wclock + xclock - nShadowRange - sizeAClock.cx;
			} else {
				xcenter = (wclock - sizeAClock.cx) / 2 + xclock;
			}
		}
	} else {
		if(nTextPos == 1) {
			xcenter = xclock;
		} else if(nTextPos == 2) {
			xcenter = wclock + xclock - nShadowRange;
		} else {
			xcenter = wclock / 2 + xclock;
		}
	}
	w = 0;
#else // USE_ANALOG_CLOCK	// add by 505
	if(nTextPos == 1)
		xcenter = xclock;
	else if(nTextPos == 2)
		xcenter = wclock + xclock - nShadowRange;
	else
		xcenter = wclock / 2 + xclock;
	w = 0;
#endif // USE_ANALOG_CLOCK	// add by 505

	while(*p)
	{
		sp = p;
		while(*p && *p != 0x0d) p++;
		if(*p == 0x0d) { *p = 0; p += 2; }
		if(*p == 0 && sp == s)
		{
			y = (hclock - tm.tmHeight) / 2  - tm.tmInternalLeading / 4 + yclock;
		}
		if(GetTextExtentPoint32(hdcClock, sp, strlen(sp), &sz) == 0)
		{
			sz.cx = (LONG)strlen(sp) * tm.tmAveCharWidth;
			sz.cy = tm.tmHeight;
		}
		if(w < sz.cx) w = sz.cx;

		if(bClockShadow)
		{
			SetTextColor(hdcClock, textshadow);
			TextOut(hdcClock, xcenter + nShadowRange, y + dvpos + nShadowRange, sp, strlen(sp));
			SetTextColor(hdcClock, textcol);
		}
		if(bClockBorder)
		{
			SetTextColor(hdcClock, textshadow);
			TextOut(hdcClock, xcenter - 1, y + dvpos + 1, sp, strlen(sp));
			TextOut(hdcClock, xcenter + 1, y + dvpos - 1, sp, strlen(sp));
			TextOut(hdcClock, xcenter + 1, y + dvpos + 1, sp, strlen(sp));
			TextOut(hdcClock, xcenter    , y + dvpos - 1, sp, strlen(sp));
			TextOut(hdcClock, xcenter + 1, y + dvpos    , sp, strlen(sp));
			TextOut(hdcClock, xcenter - 1, y + dvpos - 1, sp, strlen(sp));
			SetTextColor(hdcClock, textcol);
		}
		TextOut(hdcClock, xcenter, y + dvpos, sp, strlen(sp));

		y += hf; if(*p) y += 2 + dlineheight;
	}

#ifdef USE_ANALOG_CLOCK	// add by 505
	LocalDrawAnalogClock(hdcClock, pt, xclock, yclock, wclock, hclock);
#endif //USE_ANALOG_CLOCK	// add by 505

	xsrc = 0; ysrc = 0; wsrc = rcFill.right; hsrc = rcFill.bottom;
	if(bFillTray && (fillbackcolor||bSkinTray))
	{
#ifdef USE_ANALOG_CLOCK	// add by 505
		if (
			(nAnalogClockUseFlag == ANALOG_CLOCK_USE) &&
			(bAnalogClockAtStartBtn == FALSE)
		) {
			if (nAnalogClockPos == ANALOG_CLOCK_POS_MIDDLE) {
				xsrc = xclock;
				ysrc = yclock;
				wsrc = wclock;
				hsrc = hclock;
			} else {
				xsrc = xclock;
				ysrc = yclock;
				wsrc = wclock + sizeAClock.cx;
				hsrc = hclock;
			}
		} else {
			xsrc = xclock; ysrc = yclock;
			wsrc = wclock; hsrc = hclock;
		}
#else // USE_ANALOG_CLOCK	// add by 505
		xsrc = xclock; ysrc = yclock;
		wsrc = wclock; hsrc = hclock;
#endif //USE_ANALOG_CLOCK	// add by 505
	}
	if(nBlink == 0 || (nBlink % 2))
		BitBlt(hdc, 0, dclkvpos, wsrc, hsrc - dclkvpos, hdcClock, xsrc, ysrc, SRCCOPY);
	else
		BitBlt(hdc, 0, dclkvpos, wsrc, hsrc - dclkvpos, hdcClock, xsrc, ysrc, NOTSRCCOPY);

	if(bFillTray && (fillbackcolor||bSkinTray))
	{
		SelectObject(hdcClock, hOldRgn);
		DeleteObject(hRgn);
	}

#ifdef USE_ANALOG_CLOCK	// add by 505
	if (
		(nAnalogClockUseFlag == ANALOG_CLOCK_USE) &&
		(bAnalogClockAtStartBtn == FALSE)
	) {
		if (nAnalogClockPos != ANALOG_CLOCK_POS_MIDDLE) {
			w += sizeAClock.cx;
		}
	}
#endif //USE_ANALOG_CLOCK	// add by 505
	w += tm.tmAveCharWidth * 2;
	w += dwidth;
	if(w > iClockWidth)
	{
		iClockWidth = w;
		PostMessage(GetParent(GetParent(hwndClock)), WM_SIZE,
			SIZE_RESTORED, 0);
		InvalidateRect(GetParent(GetParent(hwndClock)), NULL, TRUE);
	}
}

/*------------------------------------------------
  paint graph
--------------------------------------------------*/
void DrawGraph(HWND hwnd, HDC hdc, int xclock, int yclock, int wclock, int hclock)
{
	int i, x, y, d;
	double one_dots = 0, one_dotr = 0;
	int graphSizeS;
	int graphSizeR;

	UNREFERENCED_PARAMETER(hwnd);

	if(GraphL>wclock||GraphT>hclock)return;
	xclock+=GraphL;
	yclock+=GraphT;
	wclock-=GraphL+GraphR;
	hclock-=GraphT+GraphB;
	if(wclock > 0 && hclock > 0)
	{
		HPEN penSR = CreatePen(PS_SOLID,1,ColSR);
		HPEN penR = CreatePen(PS_SOLID,1,ColRecv);
		HPEN penS = CreatePen(PS_SOLID,1,ColSend);
		HGDIOBJ oldPen=SelectObject(hdc,(HGDIOBJ)penSR);

		// Network
		if (graphMode == 1)
		{
			one_dotr = (double)(BackNetBand  * 1024);
			one_dots = (double)(BackNetBand2 * 1024);
		}
		// CPU
		else if (graphMode == 2)
		{
			one_dotr = (double)100;
			one_dots = (double)100;
		}

		// 横方向
		if ( bGraphTate )
		{
			one_dotr /= (double)(xclock + wclock);
			one_dots /= (double)(xclock + wclock);
			MoveToEx(hdc, (xclock + wclock) - 1, yclock, NULL);
			for(y = yclock;y < (yclock + hclock);y++)
			{
				if (bReverseGraph)
				{
					i = y - yclock;
					d = -1;
				}
				else
				{
					i = (yclock+hclock)-y-1;
					d = 1;
				}
				if(i > 0 && i < MAXGRAPHLOG)
				{
					if(bLogGraph == TRUE && graphMode == 1)
					{
						graphSizeS = (int)(log(sendlog[i]) / log(BackNetBand2 * 1024) * wclock);
						graphSizeR = (int)(log(recvlog[i]) / log(BackNetBand * 1024) * wclock);
					}
					else
					{
						graphSizeS = (int)(sendlog[i] / one_dots);
						graphSizeR = (int)(recvlog[i] / one_dotr);
					}

					if(GraphType == 1)
					{
						MoveToEx(hdc, (xclock + wclock) - 1, y, NULL);
						if(graphSizeR > graphSizeS)
						{
							if(graphSizeS > 0)
							{
								SelectObject(hdc, (HGDIOBJ)penSR);
								LineTo(hdc, max((xclock + wclock) - graphSizeS, 0), y);
							}
							if(graphSizeR > graphSizeS)
							{
								SelectObject(hdc, (HGDIOBJ)penR);
								LineTo(hdc, max((xclock + wclock) - graphSizeR, 0), y);
							}
						}
						else
						{
							if(graphSizeR > 0)
							{
								SelectObject(hdc,(HGDIOBJ)penSR);
								LineTo(hdc, max((xclock + wclock) - graphSizeR, 0), y);
							}
							if(graphSizeR < graphSizeS)
							{
								SelectObject(hdc,(HGDIOBJ)penS);
								LineTo(hdc, max((xclock + wclock) - graphSizeS, 0), y);
							}
						}
					}
					else if(GraphType==2)
					{
						if(i<MAXGRAPHLOG)
						{
							if(graphMode==1)
							{
								MoveToEx(hdc,max((xclock+wclock)-(int)(sendlog[i+1]/one_dots),0),y-d,NULL);
								SelectObject(hdc,(HGDIOBJ)penS);
								LineTo(hdc,max((xclock+wclock)-(int)(sendlog[i]/one_dots),0),y);
								MoveToEx(hdc,max((xclock+wclock)-(int)(recvlog[i+1]/one_dotr),0),y-d,NULL);
								SelectObject(hdc,(HGDIOBJ)penR);
								LineTo(hdc,max((xclock+wclock)-(int)(recvlog[i]/one_dotr),0),y);
							}
							else if(graphMode==2)
							{
								MoveToEx(hdc,max((xclock+wclock)-(int)(recvlog[i+1]/one_dotr),0),y-d,NULL);
								if(sendlog[i]==recvlog[i])
								{
									SelectObject(hdc,(HGDIOBJ)penSR);
								}
								else
								{
									SelectObject(hdc,(HGDIOBJ)penR);
								}
								LineTo(hdc,max((xclock+wclock)-(int)(recvlog[i]/one_dotr),0),y);
							}
						}
					}
				}
			}
		}

    // 縦方向
		else
		{
			one_dotr/=(double)(yclock+hclock);
			one_dots/=(double)(yclock+hclock);
			for(x = xclock;x < (xclock + wclock);x++)
			{
				if (bReverseGraph)
				{
					i = x - xclock;
					d = -1;
				}
				else
				{
					i = (xclock+wclock)-x-1;
					d = 1;
				}
				if(i > 0 && i < MAXGRAPHLOG)
				{
					if(bLogGraph == TRUE && graphMode == 1)
					{
						graphSizeS = (int)(log(sendlog[i]) / log(BackNetBand2 * 1024) * hclock);
						graphSizeR = (int)(log(recvlog[i]) / log(BackNetBand * 1024) * hclock);
					}
					else
					{
						graphSizeS = (int)(sendlog[i] / one_dots);
						graphSizeR = (int)(recvlog[i] / one_dotr);
					}

					if(GraphType==1)
					{
						MoveToEx(hdc, x, (yclock + hclock) - 1,NULL);
						if(recvlog[i]>sendlog[i])
						{
							if(graphSizeS > 0)
							{
								SelectObject(hdc,(HGDIOBJ)penSR);
								LineTo(hdc, x, max((yclock + hclock) - graphSizeS, 0));
							}
							if(graphSizeR > graphSizeS)
							{
								SelectObject(hdc,(HGDIOBJ)penR);
								LineTo(hdc, x, max((yclock + hclock) - graphSizeR, 0));
							}
						}
						else
						{
							if(graphSizeR > 0)
							{
								SelectObject(hdc, (HGDIOBJ)penSR);
								LineTo(hdc, x, max((yclock + hclock) - graphSizeR, 0));
							}
							if(graphSizeR < graphSizeS)
							{
								SelectObject(hdc,(HGDIOBJ)penS);
								LineTo(hdc, x, max((yclock + hclock) - graphSizeS, 0));
							}
						}
					}
					else if(GraphType==2)
					{
						if(i<MAXGRAPHLOG)
						{
							if(graphMode==1)
							{
								MoveToEx(hdc,x-d,max((yclock+hclock)-(int)(sendlog[i+1]/one_dots),0),NULL);
								SelectObject(hdc,(HGDIOBJ)penS);
								LineTo(hdc,x,max((yclock+hclock)-(int)(sendlog[i]/one_dots),0));
								MoveToEx(hdc,x-d,max((yclock+hclock)-(int)(recvlog[i+1]/one_dotr),0),NULL);
								SelectObject(hdc,(HGDIOBJ)penR);
								LineTo(hdc,x,max((yclock+hclock)-(int)(recvlog[i]/one_dotr),0));
							}
							else if(graphMode==2)
							{
								MoveToEx(hdc,x-d,max((yclock+hclock)-(int)(recvlog[i+1]/one_dotr),0),NULL);
								if(sendlog[i]==recvlog[i])
								{
									SelectObject(hdc,(HGDIOBJ)penSR);
								}
								else
								{
									SelectObject(hdc,(HGDIOBJ)penR);
								}
								LineTo(hdc,x,max((yclock+hclock)-(int)(recvlog[i]/one_dotr),0));
							}
						}
					}
				}
			}
		}
		SelectObject(hdc,oldPen);
		DeleteObject(penSR);
		DeleteObject(penR);
		DeleteObject(penS);
	}
}

void getGraphVal()
{
	int i;

	if (graphMode == 1)
	{
		double allrecv=0,allsend=0;
		Net_getRecvSend(&allrecv,&allsend);
		if(recvlog[0]==0)
		{
			recvlog[0]=allrecv;
			sendlog[0]=allsend;
		}
		else
		{
			recvlog[0]=(allrecv-recvlog[0])/graphInterval;
			sendlog[0]=(allsend-sendlog[0])/graphInterval;
			for(i=MAXGRAPHLOG-1;i>=0;i--)
			{
				sendlog[i+1]=sendlog[i];
				recvlog[i+1]=recvlog[i];
			}
			recvlog[0]=allrecv;
			sendlog[0]=allsend;
		}
	}
	else if (graphMode == 2)
	{
		int cpu_u;
		for(i=MAXGRAPHLOG-1;i>=0;i--)
		{
			sendlog[i+1]=sendlog[i];
			recvlog[i+1]=recvlog[i];
		}
		cpu_u = bGetSysRes ? iCPUUsage : CpuMoni_get();
		if (cpu_u >= cpuHigh)
		{
			recvlog[0]=(double)cpu_u;
			sendlog[0]=cpuHigh;
		}
		else
		{
			recvlog[0]=(double)cpu_u;
			sendlog[0]=(double)cpu_u;
		}
	}
	bGraphRedraw = TRUE;
}

/*------------------------------------------------
  paint background of clock
--------------------------------------------------*/
void FillClock(HWND hwnd, HDC hdc, RECT *prc, int nblink)
{
	HBRUSH hbr;
	COLORREF col;
	HDC hdcTemp;
	BITMAP bmp;

	UNREFERENCED_PARAMETER(nblink);

	col = colfore;
	if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
	SetTextColor(hdc, col);

	if(bSkinTray)
	{
		HGDIOBJ hOldBmp;
		hdcTemp = CreateCompatibleDC(hdc);
		hOldBmp = SelectObject(hdcTemp, hbmpClockSkin);
		GetObject(hbmpClockSkin, sizeof(BITMAP), (LPVOID)&bmp);
		//-----draw corners-----
		//Top Left
		StretchBlt(hdc, 0, 0, tEdgeLeft, tEdgeTop, hdcTemp, 0,
			0, tEdgeLeft, tEdgeTop, SRCCOPY);
		//Top Right
		StretchBlt(hdc, prc->right - tEdgeRight, 0, tEdgeRight, tEdgeTop, hdcTemp,
			bmp.bmWidth - tEdgeRight, 0, tEdgeRight, tEdgeTop, SRCCOPY);
		//Bottom Left
		StretchBlt(hdc, 0, prc->bottom - tEdgeBottom, tEdgeLeft, tEdgeBottom, hdcTemp,
			0, bmp.bmHeight - tEdgeBottom, tEdgeLeft, tEdgeBottom, SRCCOPY);
		//Bottom Right
		StretchBlt(hdc, prc->right - tEdgeRight, prc->bottom - tEdgeBottom, tEdgeRight,
			tEdgeBottom, hdcTemp, bmp.bmWidth - tEdgeRight, bmp.bmHeight - tEdgeBottom,
			tEdgeRight, tEdgeBottom, SRCCOPY);

		//-----draw edges-----
		//Top
		StretchBlt(hdc, tEdgeLeft, 0, (prc->right - tEdgeRight) - tEdgeLeft, tEdgeTop,
			hdcTemp, tEdgeLeft, 0, (bmp.bmWidth - tEdgeRight) - tEdgeLeft, tEdgeTop, SRCCOPY);
		//Left
		StretchBlt(hdc, 0, tEdgeTop, tEdgeLeft, (prc->bottom - tEdgeBottom) - tEdgeTop,
			hdcTemp, 0, tEdgeTop, tEdgeLeft, (bmp.bmHeight - tEdgeBottom) - tEdgeTop, SRCCOPY);
		//Bottom
		StretchBlt(hdc, tEdgeLeft, prc->bottom - tEdgeBottom,
			(prc->right - tEdgeRight) - tEdgeLeft, tEdgeBottom, hdcTemp, tEdgeLeft,
			bmp.bmHeight - tEdgeBottom, (bmp.bmWidth - tEdgeRight) - tEdgeLeft,
			tEdgeBottom, SRCCOPY);
		//Right
		StretchBlt(hdc, prc->right - tEdgeRight, tEdgeTop, tEdgeRight,
			(prc->bottom - tEdgeBottom) - tEdgeTop, hdcTemp, bmp.bmWidth - tEdgeRight,
			tEdgeTop, tEdgeRight, (bmp.bmHeight - tEdgeBottom) - tEdgeTop, SRCCOPY);


		StretchBlt(hdc, tEdgeLeft, tEdgeTop, (prc->right - tEdgeRight) - tEdgeLeft,
			(prc->bottom - tEdgeBottom) - tEdgeTop, hdcTemp, tEdgeLeft, tEdgeTop,
			(bmp.bmWidth - tEdgeRight) - tEdgeLeft, (bmp.bmHeight - tEdgeBottom) - tEdgeTop,
			SRCCOPY);

		SelectObject(hdcTemp, hOldBmp);
		DeleteDC(hdcTemp);
	}else if (!fillbackcolor && !bSkinTray)
	{
		HGDIOBJ hOldBmp;
		HDC hdcTemp, hdcBmp;
		RECT rcTray, rcClock;
		HBITMAP hbmp;

		GetWindowRect(GetParent(hwnd), &rcTray);
		GetWindowRect(hwnd, &rcClock);

		hdcTemp = GetDC(NULL);
		hdcBmp = CreateCompatibleDC(hdcTemp);
		hbmp = CreateCompatibleBitmap(hdcTemp, rcTray.right-rcTray.left, rcTray.bottom-rcTray.top);
		hOldBmp = SelectObject(hdcBmp, hbmp);
		SendMessage(GetParent(hwnd), WM_PRINTCLIENT, (WPARAM)hdcBmp, (LPARAM)PRF_CLIENT);

		BitBlt(hdc, 0, 0, prc->right, prc->bottom, hdcBmp, rcClock.left-rcTray.left, rcClock.top-rcTray.top, SRCCOPY);

		SelectObject(hdcTemp, hOldBmp);
		DeleteDC(hdcBmp);
		DeleteObject(hbmp);
		ReleaseDC(NULL, hdcTemp);
	}
	else if(colback == colback2 || !bWin98)
	{
		col = colback;
		if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
		hbr = CreateSolidBrush(col);
		FillRect(hdc, prc, hbr);
		DeleteObject(hbr);
	}
	else
	{
		COLORREF col2;

		col = colback;
		if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
		col2 = colback2;
		if(col2 & 0x80000000) col2 = GetSysColor(col2 & 0x00ffffff);

		GradientFillClock(hdc, prc, col, col2, grad);
	}
}

/*------------------------------------------------
　時計に必要なサイズを返す
　戻り値：上位WORD 高さ　下位WORD 幅
--------------------------------------------------*/
LRESULT OnCalcRect(HWND hwnd)
{
	SYSTEMTIME t;
	int beat100 = 0;
	LONG w, h;
	HDC hdc;
	HGDIOBJ hOldFont = NULL;
	TEXTMETRIC tm;
	char s[1024], *p, *sp;
	SIZE sz;
	int hf;

	if(!(GetWindowLongPtr(hwnd, GWL_STYLE) & WS_VISIBLE)) return 0;

	hdc = GetDC(hwnd);

	if(hFon) hOldFont = SelectObject(hdc, hFon);
	GetTextMetrics(hdc, &tm);

	GetDisplayTime(&t, nDispBeat?(&beat100):NULL);
	MakeFormat(s, &t, beat100, format);

	p = s; w = 0; h = 0;
	hf = tm.tmHeight - tm.tmInternalLeading;
	while(*p)
	{
		sp = p;
		while(*p && *p != 0x0d) p++;
		if(*p == 0x0d) { *p = 0; p += 2; }
		if(GetTextExtentPoint32(hdc, sp, strlen(sp), &sz) == 0)
			sz.cx = (LONG)strlen(sp) * tm.tmAveCharWidth;
		if(w < sz.cx) w = sz.cx;
		h += hf; if(*p) h += 2 + dlineheight;
	}

#ifdef USE_ANALOG_CLOCK	// add by 505
	if (
		(nAnalogClockUseFlag == ANALOG_CLOCK_USE) &&
		(bAnalogClockAtStartBtn == FALSE)
	) {
		if (nAnalogClockPos != ANALOG_CLOCK_POS_MIDDLE) {
			w += sizeAClock.cx;
		}
	}
#endif //USE_ANALOG_CLOCK	// add by 505

	w += tm.tmAveCharWidth * 2;
	w += dwidth;
	if(w > iClockWidth)iClockWidth = w;
	h += hf / 2 + dheight + dclkvpos;
	if(bClockShadow)
	{
		h += nShadowRange; w += nShadowRange;
	}
	if(h < 4) h = 4;

	if(hFon) SelectObject(hdc, hOldFont);
	ReleaseDC(hwnd, hdc);

	return (h << 16) + w;
}

/*------------------------------------------------
  copy date/time text to clipboard
--------------------------------------------------*/
void OnCopy(HWND hwnd, LPARAM lParam)
{
	SYSTEMTIME t;
	int beat100;
	char entry[20], fmt[1024], s[1024], *pbuf;
	HGLOBAL hg;
	BOOL br, bb, bm, bmb, bpm, bn, bhd, bcc, bvl;
	DWORD dwInfoFormat;

	GetDisplayTime(&t, &beat100);
	wsprintf(entry, "%d%dClip", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	GetMyRegStrEx("Mouse", entry, fmt, 1024, "", gConfigNo);
	if(fmt[0] == 0) strcpy(fmt, format);

	// 読み込み時に<%～%>を追加する
	{
		char fmt_tmp[1024];
		strcpy(fmt_tmp,"<%");
		strcat(fmt_tmp,fmt);
		strcat(fmt_tmp,"%>");
		strcpy(fmt,fmt_tmp);
	}
	dwInfoFormat = FindFormat(fmt);
	br = (dwInfoFormat&FORMAT_SYSINFO)? TRUE:FALSE;
	bb = (dwInfoFormat&FORMAT_BATTERY)? TRUE:FALSE;
	bm = (dwInfoFormat&FORMAT_MEMORY) ? TRUE:FALSE;
	bmb = (dwInfoFormat&FORMAT_MOTHERBRD) ? TRUE:FALSE;
	bpm = (dwInfoFormat&FORMAT_PERMON) ? TRUE:FALSE;
	bn = (dwInfoFormat&FORMAT_NET) ? TRUE:FALSE;
	bhd = (dwInfoFormat&FORMAT_HDD) ? TRUE:FALSE;
	bcc = (dwInfoFormat&FORMAT_CPU) ? TRUE:FALSE;
	bvl = (dwInfoFormat&FORMAT_VOL) ? TRUE:FALSE;
	if(br || bb || bm || bmb || bpm || bn || bhd || bcc || bvl)
		UpdateSysRes(hwnd, br, bb, bm, bmb, bpm, bn, bhd, bcc, bvl);

	MakeFormat(s, &t, beat100, fmt);

	if(!OpenClipboard(hwnd)) return;
	EmptyClipboard();
	hg = GlobalAlloc(GMEM_DDESHARE, strlen(s) + 1);
	pbuf = (char*)GlobalLock(hg);
	strcpy(pbuf, s);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
}

void UpdateSysRes(HWND hwnd, BOOL bsysres, BOOL bbattery, BOOL bmem, BOOL bmb, BOOL bpermon, BOOL bnet, BOOL bhdd, BOOL bcpuc, BOOL bvol)
{
	int i;

	UNREFERENCED_PARAMETER(hwnd);

	if((bWin95 || bWin2000) && bbattery)
	{
		iBatteryLife = GetBatteryLifePercent();
	}
	if((bWin98 || bWin2000) && bcpuc)
	{
		UpdateCpuClock();
	}
	if(bmem)
	{
		msMemory.dwLength = sizeof(msMemory);
		//GlobalMemoryStatus(&msMemory);
		GlobalMemoryStatusEx(&msMemory);
	}
	if(bmb)
	{
		MBMSharedDataUpdate();
	}
	if(bvol)
	{
		BOOL muteflg;
		GetMasterVolume(&iVolume);
		GetMasterMute(&muteflg);
		if (muteflg)
			iVolume = 0;
	}
	if(bpermon)
	{
		PerMoni_get();
	}
	if(bnet)
	{
		Net_get();
	}
	if(bhdd)
	{
		Hdd_get();
	}
	if(bsysres)
	{
		if(bWin95)
		{
			for(i = 0; i < 3; i++)
				iFreeRes[i] = GetFreeSystemResources((WORD)i);
		}
		iCPUUsage = CpuMoni_get(); // cpu.c
	}
}

int iHourTransition = -1, iMinuteTransition = -1;

/*------------------------------------------------
  initialize time-zone information
--------------------------------------------------*/
void InitDaylightTimeTransition(void)
{
	SYSTEMTIME lt, *plt = NULL;
	TIME_ZONE_INFORMATION tzi;
	DWORD dw;
	BOOL b;

	iHourTransition = iMinuteTransition = -1;

	GetLocalTime(&lt);

	b = FALSE;
	memset(&tzi, 0, sizeof(tzi));
	dw = GetTimeZoneInformation(&tzi);
	if(dw == TIME_ZONE_ID_STANDARD
		&& tzi.DaylightDate.wMonth == lt.wMonth
		&& tzi.DaylightDate.wDayOfWeek == lt.wDayOfWeek)
	{
		b = TRUE; plt = &(tzi.DaylightDate);
	}
	if(dw == TIME_ZONE_ID_DAYLIGHT
		&& tzi.StandardDate.wMonth == lt.wMonth
		&& tzi.StandardDate.wDayOfWeek == lt.wDayOfWeek)
	{
		b = TRUE; plt = &(tzi.StandardDate);
	}

	if(b && plt->wDay < 5)
	{
		if(((lt.wDay - 1) / 7 + 1) == plt->wDay)
		{
			iHourTransition = plt->wHour;
			iMinuteTransition = plt->wMinute;
		}
	}
	else if(b && plt->wDay == 5)
	{
		FILETIME ft;
		SystemTimeToFileTime(&lt, &ft);
		*(DWORDLONG*)&ft += 6048000000000i64;
		FileTimeToSystemTime(&ft, &lt);
		if(lt.wDay < 8)
		{
			iHourTransition = plt->wHour;
			iMinuteTransition = plt->wMinute;
		}
	}
}

BOOL CheckDaylightTimeTransition(SYSTEMTIME *plt)
{
	if((int)plt->wHour == iHourTransition &&
		 (int)plt->wMinute >= iMinuteTransition)
	{
		iHourTransition = iMinuteTransition = -1;
		return TRUE;
	}
	else return FALSE;
}

/*---------------------------------------
	タスクトレイ一杯に時計を広げる
---------------------------------------*/
void FitClock()
{
	static LONG lLockFitClock = -1;
	int x, y, cx, cy;
	RECT rcTray, rcClock;
	if (InterlockedIncrement(&lLockFitClock) == 0)
	{
		GetClientRect(hwndClock, &rcClock);
		GetClientRect(GetParent(hwndClock),&rcTray);

		if (((rcTray.right - rcTray.left) - (rcClock.right - rcClock.left)) > ((rcTray.bottom - rcTray.top) - (rcClock.bottom - rcClock.top)))
			{
				/* fit right-side */
				x = rcTray.left + (rcTray.right - rcTray.left) - (rcClock.right - rcClock.left);
				cx = rcClock.right - rcClock.left;
				y = rcTray.top;
				cy = rcTray.bottom - rcTray.top;
				SetWindowPos(hwndClock,0, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOREDRAW);
			}
		else
			{
				/* fit up-side */
				x = rcTray.left;
				y = rcTray.top;
				cx = rcTray.right - rcTray.left;
				cy = rcClock.bottom - rcTray.top;
				SetWindowPos(hwndClock,0, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOREDRAW);
			}
	}
	InterlockedDecrement(&lLockFitClock);
}

/*----------------------------------------------
     ユーザー定義文字列をもらう。
----------------------------------------------*/
void OnCopyData(HWND hwnd, COPYDATASTRUCT* pCopyDataStruct)
{
	UNREFERENCED_PARAMETER(hwnd);
	if (lstrlen((LPCTSTR)pCopyDataStruct->lpData) > 100 )
		return;
	switch(pCopyDataStruct->dwData)
	{
		case 0:
			lstrcpy(usrstr[0].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 1:
			lstrcpy(usrstr[1].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 2:
			lstrcpy(usrstr[2].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 3:
			lstrcpy(usrstr[3].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 4:
			lstrcpy(usrstr[4].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 5:
			lstrcpy(usrstr[5].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 6:
			lstrcpy(usrstr[6].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 7:
			lstrcpy(usrstr[7].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 8:
			lstrcpy(usrstr[8].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
		case 9:
			lstrcpy(usrstr[9].u_str,(LPCTSTR)pCopyDataStruct->lpData);
			break;
	}
}
