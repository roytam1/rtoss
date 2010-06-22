/*-----------------------------------------------------
  tooltip.c
  customize the tray clock
  KAZUBON 1997-2001
-------------------------------------------------------*/
#define COBJMACROS
#include "tcdll.h"
#include <exdisp.h>

#include "cwebpage.h"

extern HANDLE hmod;
extern HWND hwndClock;
extern int gConfigNo;
extern BOOL bWin2000;
extern BOOL bWinNT;
extern BOOL bVista;
extern void GetDisplayTime(SYSTEMTIME* pt, int* beat100);

#define GRADIENT_FILL_RECT_H    0x00000000
#define GRADIENT_FILL_RECT_V    0x00000001

#ifndef TTS_NOANIMATE
#define TTS_NOANIMATE 0x10
#endif
#ifndef TTS_NOFADE
#define TTS_NOFADE 0x20
#endif
#ifndef TTS_BALLOON
#define TTS_BALLOON 0x40
#endif
#ifndef TTM_SETTITLE
#define TTM_SETTITLE 0x420
#endif
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x80000
#endif
#ifndef WS_EX_COMPOSITED
#define WS_EX_COMPOSITED 0x02000000L
#endif

/* ツールチップの許容量 */
#define LEN_TOOLTIP   10240
/* HTMLツールチップの表示までの時間 */
#define TOOLTIPHTMLDISPTIME 3
/* HTMLツールチップの消去までの時間 */
#define TOOLTIPHTMLERASETIME iTooltipDispTime
enum
{
	TOOLTIPTYPE_NORMAL = 0,
	TOOLTIPTYPE_BALLOON = 1,
	TOOLTIPTYPE_IECOMPO = 2
};

static DWORD dwTooltipType = 0;
static DWORD dwTooltipTypeCur = 0;
static int alphaTooltip = 255;
static int iTooltipDispTime = 5;
static int nTooltipIcon = 0;
static HWND hwndTooltip = NULL;
static int iTooltipSelected = 0;
static BOOL bTooltipCustomDrawDisable = TRUE;
static BOOL bTooltipUpdateEnable[3] = {FALSE, FALSE, FALSE};
static BOOL bTooltipEnableDoubleBuffering = FALSE;
static BOOL bTooltipUpdated = FALSE;
static BOOL bTooltipShow = FALSE;
static BOOL bTooltipBalloon = FALSE;
static BOOL bTooltipHasTitle = FALSE;
/*static volatile LONG lLockTooltipTextUpdate = 0;*/

static HFONT hFonTooltip = NULL;
static int iTooltipDispIntervalCount = 0;
static int iTooltipDispInterval = 1;
static int iTooltipSizeX;
static int iTooltipSizeY;
static DWORD dwTooltipFonCol;
static DWORD dwTooltipBakCol;
static COLORREF colTooltipText, colTooltipBack;

static char formatTooltip[LEN_TOOLTIP];
static WCHAR formatTooltipW[LEN_TOOLTIP];
static char titleTooltip[300] = { 0 };
static HRESULT hrOleInitialize = E_NOTIMPL;

static DWORD dwMousePosPrev = 0;
static int nTooltipHtmlDispCount = 0;
static HWND hwndHTMLParent = NULL;
static ATOM atomHTMLParent = 0;
static BOOL bEmbedBrowserObject = FALSE;

static void TooltipApplySetting(void)
{
	COLORREF tcolf, tcolb;
	tcolf = dwTooltipFonCol; tcolb = dwTooltipBakCol;
	if(tcolf & 0x80000000) tcolf = GetSysColor(tcolf & 0x00ffffff);
	if(tcolb & 0x80000000) tcolb = GetSysColor(tcolb & 0x00ffffff);
	colTooltipText = tcolf;
	colTooltipBack = tcolb;
	switch(dwTooltipTypeCur)
	{
	case TOOLTIPTYPE_NORMAL:
	case TOOLTIPTYPE_BALLOON:
		if(hwndTooltip)
		{
			LONG_PTR exstyle;
			exstyle = GetWindowLongPtr(hwndTooltip, GWL_EXSTYLE) & ~WS_EX_COMPOSITED;
			SetWindowLongPtr(hwndTooltip, GWL_EXSTYLE, exstyle | (bTooltipEnableDoubleBuffering ? WS_EX_COMPOSITED : 0));
			if (hFonTooltip) SendMessage(hwndTooltip, WM_SETFONT, (WPARAM)hFonTooltip, TRUE);
			SendMessage(hwndTooltip, TTM_SETTIPBKCOLOR, tcolb, 0);
			SendMessage(hwndTooltip, TTM_SETTIPTEXTCOLOR, tcolf, 0);
			SendMessage(hwndTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELONG(iTooltipDispTime*1000,0));
			if(bWin2000) SetLayeredWindow(hwndTooltip,(INT)(25500-255*alphaTooltip)/100, colTooltipBack);
		}
		break;
	case TOOLTIPTYPE_IECOMPO:
		if(hwndHTMLParent)
		{
			LONG_PTR exstyle;
			exstyle = GetWindowLongPtr(hwndHTMLParent, GWL_EXSTYLE) & ~WS_EX_COMPOSITED;
			SetWindowLongPtr(hwndHTMLParent, GWL_EXSTYLE, exstyle | (bTooltipEnableDoubleBuffering ? WS_EX_COMPOSITED : 0));
			if(bWin2000) SetLayeredWindow(hwndHTMLParent,(INT)(25500-255*alphaTooltip)/100, colTooltipBack);
		}
		break;
	}
}

static void TooltipUpdateText(void);
static void TooltipUpdateTitle(void);

static const CHAR cszWindowClassName[] = "__tclock html tooltip parent__";
static LRESULT CALLBACK MyWindowWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_ACTIVATE:
			if ( wParam == WA_INACTIVE && bEmbedBrowserObject )
			{
				ShowWindow(hwndHTMLParent, SW_HIDE);
				UnEmbedBrowserObject(hwndHTMLParent);
				bEmbedBrowserObject = FALSE;
				bTooltipShow = FALSE;
			}
			break;
		default:
			return DefWindowProcA(hwnd, message, wParam, lParam);
	}
	return DefWindowProcA(hwnd, message, wParam, lParam);
}
static ATOM MyWindowRegisterClass(void)
{
	WNDCLASS wndcls;
	wndcls.style         = 0;
	wndcls.lpfnWndProc   = MyWindowWndProc;
	wndcls.cbClsExtra    = 0;
	wndcls.cbWndExtra    = 0;
	wndcls.hInstance     = GetModuleHandleA(NULL);
#pragma warning(push)
#pragma warning(disable: 4305)	// '型キャスト' : 'LPSTR' から 'WORD' へ切り詰めます。
	wndcls.hIcon         = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wndcls.hCursor       = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
#pragma warning(pop)
	wndcls.hbrBackground = NULL;
	wndcls.lpszMenuName  = NULL;
	wndcls.lpszClassName = cszWindowClassName;
	return RegisterClass(&wndcls);
}
static void MyWindowUnregisterClass(void)
{
	UnregisterClass(cszWindowClassName, GetModuleHandleA(NULL));
}

/*------------------------------------------------
　ツールチップウィンドウの作成
--------------------------------------------------*/
void TooltipInit(HWND hwnd)
{
	TOOLINFO ti;
	dwTooltipTypeCur = dwTooltipType;
	switch(dwTooltipTypeCur)
	{
	case TOOLTIPTYPE_NORMAL:
	case TOOLTIPTYPE_BALLOON:
		hwndTooltip = CreateWindowEx((bTooltipEnableDoubleBuffering ? WS_EX_COMPOSITED : 0) | WS_EX_LAYERED, TOOLTIPS_CLASS, (LPSTR)NULL,
			TTS_ALWAYSTIP | TTS_NOPREFIX | ((dwTooltipTypeCur == TOOLTIPTYPE_BALLOON) ? TTS_BALLOON : 0),
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, hmod, NULL);
		if(!hwndTooltip) return;

		SetWindowPos(hwndTooltip, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

		bTooltipBalloon = (GetWindowLongPtr(hwndTooltip, GWL_STYLE) & TTS_BALLOON) != 0;

		ti.cbSize = sizeof(TOOLINFO);
		ti.hwnd = hwnd;
		ti.uId = 1;
		ti.uFlags = 0;
		ti.hinst = NULL;
		ti.lpszText = LPSTR_TEXTCALLBACK;
		ti.rect.left = 0;
		ti.rect.top = 0;
		ti.rect.right = 480;
		ti.rect.bottom = 480;

		TooltipApplySetting();

		TooltipUpdateText();
		TooltipUpdateTitle();

		SendMessage(hwndTooltip, TTM_SETMAXTIPWIDTH, 0, (LPARAM) LEN_TOOLTIP);
		SendMessage(hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO)&ti);
		SendMessage(hwndTooltip, TTM_ACTIVATE, TRUE, 0);
		break;
	case TOOLTIPTYPE_IECOMPO:
		hrOleInitialize = OleInitialize(NULL);
		atomHTMLParent = MyWindowRegisterClass();
		if (!atomHTMLParent) break;
		hwndHTMLParent = CreateWindowExA((bTooltipEnableDoubleBuffering ? WS_EX_COMPOSITED : 0) | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
											cszWindowClassName, "", WS_POPUP/* | WS_BORDER*/, 0,0,0,0, NULL, NULL, GetModuleHandleA(NULL), 0);
		if (!hwndHTMLParent) break;
		TooltipApplySetting();
		/* SetWindowPos(hwndHTMLParent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE); */
		break;
	}
}

void TooltipDeleteRes(void)
{
	if(hFonTooltip) DeleteObject(hFonTooltip); hFonTooltip = NULL;	//635@p5
}

void TooltipEnd(HWND hwnd)
{
	switch(dwTooltipTypeCur)
	{
	case TOOLTIPTYPE_NORMAL:
	case TOOLTIPTYPE_BALLOON:
		if(hwndTooltip)
		{
			TOOLINFO ti;
			ti.cbSize = sizeof(TOOLINFO);
			ti.hwnd = hwnd;
			ti.uId = 1;
			SendMessage(hwndTooltip, TTM_ACTIVATE, FALSE, 0);
			SendMessage(hwndTooltip, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO)&ti);
			DestroyWindow(hwndTooltip);
			hwndTooltip = NULL;
		}
		break;
	case TOOLTIPTYPE_IECOMPO:
		if (hwndHTMLParent && IsWindow(hwndHTMLParent))
		{
			ShowWindow(hwndHTMLParent, SW_HIDE);
			if (bEmbedBrowserObject)
			{
				UnEmbedBrowserObject(hwndHTMLParent);
			}
			DestroyWindow(hwndHTMLParent);
			hwndHTMLParent = NULL;
		}
		if (atomHTMLParent)
		{
			MyWindowUnregisterClass();
			atomHTMLParent = 0;
		}
		bEmbedBrowserObject = FALSE;
		bTooltipShow = FALSE;
		if (SUCCEEDED(hrOleInitialize))
		{
			hrOleInitialize = E_NOTIMPL;
			OleUninitialize();
		}
		break;
	}
}

void TooltipOnRefresh(HWND hwnd)
{
	if (dwTooltipTypeCur == dwTooltipType)
		TooltipApplySetting();
	else
	{
		TooltipEnd(hwnd);
		TooltipInit(hwnd);
	}
}

//	2002/11/26 - H.N. Neri-uma added.
//	2002/11/27 - 635 modified.
//-------------------------------------------------
//	GetTooltipText
//		Tooltipのテキストを、レジストリからではなく、テキストファイルから読み込みます。
//	引数：	[char * pszText]	読み込んだ文字列を格納する変数へのポインタ
//	戻り値：0		エラー。テキストファイルが存在しなかった。ファイルは存在したが、空だった。など
//			0以外	成功
//-------------------------------------------------
static BOOL GetTooltipText(PSTR pszText)
{
	HANDLE	hFile;
	char	szFilePath[MAX_PATH];	//	テキストファイルのパス
	DWORD	dwReadSize;
	DWORD	dwFileSize;
	size_t	len;
	BOOL	bAbsPath;

//	{
//		GetCurrentDirectory(MAX_PATH, pszText);
//		return TRUE;
//	}

	if(!pszText)
	{
		//	値を格納するアドレスが存在しないので、FALSEを返して終了
		return FALSE;
	}

	//	テキストファイルのパス作成
	//GetModuleFileName(hmod, szFilePath, sizeof(szFilePath));
	//del_title(szFilePath);
	//add_title(szFilePath, TIPTEXTFILENAME);

	// \\NAME\C\path\path\filename.txt
	// C:\path\path\filename.txt
	// 以上の絶対パス以外を相対パスと判断して
	// その前にTClockのパスを基準ディレクトリとして付加
	bAbsPath = FALSE;
	len = strlen(pszText);
	if (len >= 2) {
		if ((*pszText == '\\') && (*(pszText + 1) == '\\')) {
			//UNC name
			bAbsPath = TRUE;
		} else if (*(pszText + 1) == ':') {
			bAbsPath = TRUE;
		}
	}
	if (bAbsPath == FALSE) {
		// TClockの位置を基準パスとして指定文字列を相対パスとして追加
		GetModuleFileName(hmod, szFilePath, sizeof(szFilePath));
		del_title(szFilePath);
		if (len + strlen(szFilePath) >= MAX_PATH) {
			return FALSE;
		}
		add_title(szFilePath, pszText);
		*pszText = '\0';
	} else {
		//635@p5 ファイル名を取得してツールチップをクリア。
		strcpy(szFilePath, pszText);
		*pszText = '\0';
	}

	//	ファイルを読み込む
	hFile = CreateFile(szFilePath,
					   GENERIC_READ,
					   FILE_SHARE_READ,
					   NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		//	ファイルが存在しないので、FALSEを返して終了
		return FALSE;
	}

	dwFileSize = GetFileSize(hFile, NULL);

	if ( dwFileSize > (DWORD)LEN_TOOLTIP)
		dwFileSize = (DWORD)(LEN_TOOLTIP-1);

	//	ファイル読み込み
	ReadFile(hFile, pszText, dwFileSize, &dwReadSize, NULL);
	//	ファイルをクローズ
	CloseHandle(hFile);

	if(strcmp(pszText, "") == 0)
	{
		//	文字列取得に失敗。レジストリ取得へ走らせるためにFALSEを返して終了
		return FALSE;
	}

	//	文字列の末尾に、強制的にNULL Stringを付加（^^;
	pszText[dwFileSize] = '\0';

	//	成功したのでTRUEを返す
	return TRUE;
}


/*------------------------------------------------
  draw tooltip
--------------------------------------------------*/
static void TooltipUpdate(HDC hdc, LPRECT lprcDraw, LPRECT lprect, UINT uDrawFlags)
{
	HGDIOBJ hOldFont;
	HBRUSH hBrushTooltipBack = NULL, hBrushTooltipText = NULL;
	RECT rc, rcall;
	LPSTR pszText;
	int maxwidth, width, height, len;
	LONG top;

	if (lprcDraw)
	{
		if (bTooltipBalloon)
		{
			TooltipUpdate(hdc, 0, &rc, uDrawFlags);
			rcall = *lprcDraw;
			if (bTooltipHasTitle)
				rcall.top += 22;
			rcall.bottom = rcall.top + (rc.bottom - rc.top);
		}
		else
		{
			TooltipUpdate(hdc, 0, &rc, uDrawFlags);
			rcall = *lprcDraw;
			rcall.top = rcall.bottom - (rc.bottom - rc.top);
			if (bTooltipHasTitle)
			{
				rcall.left += 10;
				rcall.top -= 8;
			}
		}
		hBrushTooltipText = CreateSolidBrush(colTooltipText);
		hBrushTooltipBack = CreateSolidBrush(colTooltipBack);
	}
	else
	{
		rcall.top = rcall.left = 0;
		rcall.right = rcall.bottom = 16384;
	}
	hOldFont = SelectObject(hdc, hFonTooltip);
	top = rcall.top;
	maxwidth = 0;
	/*InterlockedIncrement(&lLockTooltipTextUpdate);*/
	pszText = formatTooltip;
	while (*pszText)
	{
		len = 0;
		if (*pszText == 0x0d/*'\r'*/) pszText++;
		if (*pszText == 0x0a/*'\n'*/) pszText++;
		while (pszText[len] && pszText[len] != '\r' && pszText[len] != '\n') len++;
		rc = rcall;
		rc.top = top;
		if (len)
		{
			height = DrawText(hdc, pszText, len, &rc, uDrawFlags | DT_CALCRECT);
			width = rc.right - rc.left;
			if (maxwidth < width) maxwidth = width;
		}
		else
		{
			height = DrawText(hdc, " ", 1, &rc, uDrawFlags | DT_CALCRECT);
			width = 0;
		}
		if (lprcDraw)
		{
			int iOldMode;
			COLORREF colOldTooltipText, colOldTooltipBack;
			HDC hdcTemp;
			HBITMAP hbmpTemp;
			HGDIOBJ hbmpOld, hfonOld, hbrOld;
			hdcTemp= CreateCompatibleDC(hdc);
			if (hdcTemp)
			{
				hbmpTemp = CreateCompatibleBitmap(hdc, rcall.right - rcall.left, height);
				if (hbmpTemp)
				{
					hbmpOld = SelectObject(hdcTemp, hbmpTemp);
					hbrOld = SelectObject(hdcTemp, hBrushTooltipText);
					hfonOld = SelectObject(hdcTemp, hFonTooltip);
					colOldTooltipText = SetTextColor(hdcTemp, colTooltipText);
					colOldTooltipBack = SetBkColor(hdcTemp, colTooltipBack);
					iOldMode = SetBkMode(hdcTemp, TRANSPARENT);
					rc.top = 0;
					rc.bottom = height;
					rc.left = 0;
					rc.right = rcall.right - rcall.left;
					FillRect(hdcTemp, &rc, hBrushTooltipBack);
					if (len)
					{
						rc.top = 0;
						rc.bottom = height;
						rc.left = 0;
						rc.right = rcall.right - rcall.left;
						DrawText(hdcTemp, pszText, len, &rc, uDrawFlags);
					}
					BitBlt(hdc, rcall.left, top, rcall.right - rcall.left, height, hdcTemp, 0, 0, SRCCOPY);
					SetBkMode(hdcTemp, iOldMode);
					SetBkColor(hdcTemp, colOldTooltipBack);
					SetTextColor(hdcTemp, colOldTooltipText);
					SelectObject(hdcTemp, hfonOld);
					SelectObject(hdcTemp, hbrOld);
					SelectObject(hdcTemp, hbmpOld);
					DeleteObject(hbmpTemp);
				}
				DeleteDC(hdcTemp);
			}
		}
		top += height;
		pszText += len;
	}
	/*InterlockedDecrement(&lLockTooltipTextUpdate);*/
	SelectObject(hdc, hOldFont);
	if (lprcDraw)
	{
		if (top < rcall.bottom)
		{
			rc.top = top;
			rc.bottom = rcall.bottom;
			rc.left = rcall.left;
			rc.right = rcall.right;
			FillRect(hdc, &rc, hBrushTooltipBack);
		}
		DeleteObject(hBrushTooltipBack);
		DeleteObject(hBrushTooltipText);
	}
	if (lprect)
	{
		lprect->top = rcall.top;
		lprect->left = rcall.left;
		lprect->bottom = top;
		lprect->right = rcall.left + maxwidth;
	}
}

/*------------------------------------------------
　ツールチップのタイトル取得
--------------------------------------------------*/
static void TooltipUpdateTitle(void)
{
	if (!hwndTooltip) return;
	if (bTooltipHasTitle || *titleTooltip)
	{
		bTooltipHasTitle = (BOOL)SendMessage(hwndTooltip, TTM_SETTITLE,nTooltipIcon,(LPARAM)(LPCTSTR)(char*)titleTooltip);
		if (!*titleTooltip) bTooltipHasTitle = FALSE;
	}
}

/*------------------------------------------------
　ツールチップの表示内容取得
--------------------------------------------------*/
static void TooltipUpdateText(void)
{
	const static char * const aszRegEntry[3] =
	{
		"Tooltip",
		"Tooltip2",
		"Tooltip3",
	};
	SYSTEMTIME t;
	int beat100;
	char fmt[LEN_TOOLTIP], s[LEN_TOOLTIP], tiptitle[300], tipt[300], ini_key[10];
	DWORD dw;
	RECT rcClock;
	int clLen, mPos;
	BOOL bTooltip2, bTooltip3, bTooltipTate;

	bTooltipUpdated = TRUE;
	bTooltip2 = GetMyRegLongEx("Tooltip", "Tip2Use", FALSE, gConfigNo);
	if (bTooltip2)
	{
		bTooltip3    = GetMyRegLongEx("Tooltip", "Tip3Use", FALSE, gConfigNo);
		bTooltipTate = GetMyRegLongEx("Tooltip", "TipTateFlg", FALSE, gConfigNo);

		GetWindowRect(hwndClock, &rcClock);
		dw = GetMessagePos();
		if (bTooltipTate)
		{
			clLen = rcClock.bottom - rcClock.top;
			mPos  = GET_Y_LPARAM(dw) - rcClock.top;
		}
		else
		{
			clLen = rcClock.right - rcClock.left;
			mPos  = GET_X_LPARAM(dw) - rcClock.left;
		}
		if (bTooltip3)
		{
			if ( mPos > (2*clLen/3) )
				iTooltipSelected = 2;
			if ( mPos <= (2*clLen/3) && mPos >= (clLen/3) )
				iTooltipSelected = 1;
			if ( mPos < (clLen/3) )
				iTooltipSelected = 0;
		}
		else
		{
			if ( mPos > (clLen/2) )
				iTooltipSelected = 1;
			if ( mPos <= (clLen/2) )
				iTooltipSelected = 0;
		}
	}
	else
	{
		iTooltipSelected = 0;
	}
	strcpy(ini_key, aszRegEntry[iTooltipSelected]);

	GetMyRegStrEx("Tooltip", ini_key, fmt, sizeof(fmt), "", gConfigNo);
	if(fmt[0] == 'f' && fmt[1] == 'i' && fmt[2] == 'l' && fmt[3] == 'e' && fmt[4] == ':')
	{
		memmove( fmt, fmt + 5, (size_t)((strchr(fmt,'\0')-1)-fmt));
		if(!GetTooltipText(fmt)){
			strcpy(fmt, "ファイル取得失敗");
		}
	}
	if(fmt[0] == 0)
	{
		strcpy(fmt, "TClock <%LDATE%>");
	}

	GetDisplayTime(&t, &beat100);
	MakeFormat(s, &t, beat100, fmt);
	strcpy(formatTooltip, s);

	//635@p5
	GetMyRegStrEx("", "TipTitle", tiptitle, 300, "", gConfigNo);
	if(tiptitle[0] != 0)
	{
		MakeFormat(tipt, &t, beat100, tiptitle);
		strcpy(titleTooltip, tipt);
	}
	else
	{
		titleTooltip[0] = 0;
	}
}

/*------------------------------------------------
　ツールチップの表示更新
--------------------------------------------------*/
void TooltipOnTimer(HWND hwnd)
{
	if (dwTooltipTypeCur == TOOLTIPTYPE_IECOMPO)
	{
		DWORD dwMousePos;
		POINT pt;
		RECT rc;
		GetCursorPos(&pt);
		dwMousePos = (pt.y << 16) | pt.x;
		GetWindowRect(hwndClock, &rc);
		if (bTooltipShow)
		{
			RECT rc2;
			GetWindowRect(hwndHTMLParent, &rc2);
			if (bEmbedBrowserObject)
			{
				if (++nTooltipHtmlDispCount >= TOOLTIPHTMLERASETIME * 5 && GetForegroundWindow() != hwndHTMLParent ) ////!PtInRect(&rc, pt) && !PtInRect(&rc2, pt)
				{
					ShowWindow(hwndHTMLParent, SW_HIDE);
					UnEmbedBrowserObject(hwndHTMLParent);
					bEmbedBrowserObject = FALSE;
					bTooltipShow = FALSE;
					nTooltipHtmlDispCount = 0;
				}
			}
			else
				nTooltipHtmlDispCount = 0;
		}
		else
		{
			if (!bEmbedBrowserObject && dwMousePosPrev == dwMousePos && PtInRect(&rc, pt))
			{
				if (++nTooltipHtmlDispCount >= TOOLTIPHTMLDISPTIME)
				{
					int x,y,cx,cy;
					cx = iTooltipSizeX;
					cy = iTooltipSizeY;
					x = pt.x;
					y = (pt.y > cy) ? (pt.y - cy) : pt.y;
					GetWindowRect(GetDesktopWindow(), &rc);
					if (rc.right <= x + cx) x = rc.right - cx;
					SetWindowPos(hwndHTMLParent, HWND_TOPMOST, x, y, cx, cy, 0);
					bEmbedBrowserObject = EmbedBrowserObject(hwndHTMLParent) == 0;
					if (bEmbedBrowserObject)
					{
						TooltipUpdateText();
						if(formatTooltip[0] == 'h' && formatTooltip[1] == 't' && formatTooltip[2] == 't' && formatTooltip[3] == 'p')
							DisplayHTMLPage(hwndHTMLParent, formatTooltip);
						else
							DisplayHTMLStr(hwndHTMLParent, formatTooltip);
						ShowWindow(hwndHTMLParent, SW_SHOW);
						SetActiveWindow(hwndHTMLParent);
						bTooltipShow = TRUE;
					}
					nTooltipHtmlDispCount = 0;
				}
			}
			else
				nTooltipHtmlDispCount = 0;
		}
		dwMousePosPrev = dwMousePos;
	}
	if (!bTooltipShow) return;
	if (++iTooltipDispIntervalCount < iTooltipDispInterval * 5) return;
	iTooltipDispIntervalCount = 0;
	if (!bTooltipUpdateEnable[iTooltipSelected]) return;

	switch(dwTooltipTypeCur)
	{
	case TOOLTIPTYPE_NORMAL:
	case TOOLTIPTYPE_BALLOON:
		if (hwndTooltip)
		{
			TOOLINFO ti;
			TooltipUpdateText();
			TooltipUpdateTitle();

			ti.cbSize = sizeof(TOOLINFO);
			ti.hwnd = hwnd;
			ti.uId = 1;
			SendMessage(hwndTooltip, TTM_GETTOOLINFO, 0, (LPARAM)(LPTOOLINFO)&ti);
			ti.uFlags = 0;
			ti.hinst = NULL;
			ti.lpszText = LPSTR_TEXTCALLBACK;
			SendMessage(hwndTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO)&ti);
		}
		break;
	}
}

void TooltipReadData(void)
{
	char fontname[80];
	int fontsize;
	LONG weight, italic;

	GetMyRegStrEx(NULL, "TipFont", fontname, 80, "", gConfigNo);
	if (fontname[0] == 0) 
	{
		HFONT hfont;
		hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if(hfont)
		{
			LOGFONT lf;
			GetObject(hfont, sizeof(lf),(LPVOID)&lf);
			strcpy(fontname, lf.lfFaceName);
		}
	}

	fontsize = GetMyRegLongEx(NULL, "TipFontSize", 9, gConfigNo);
	weight = GetMyRegLongEx(NULL, "TipBold", 0, gConfigNo);
	if(weight) weight = FW_BOLD;
	else weight = 0;
	italic = GetMyRegLongEx(NULL, "TipItalic", 0, gConfigNo);

	if(hFonTooltip) DeleteObject(hFonTooltip);
	hFonTooltip = CreateMyFont(fontname, fontsize, weight, italic);
	dwTooltipType = GetMyRegLong(NULL, "BalloonFlg", 0);
	alphaTooltip = (int)(short)GetMyRegLongEx(NULL, "AlphaTip", 0, gConfigNo);
	iTooltipDispTime = (int)(short)GetMyRegLong(NULL, "TipDispTime", 5);
	bTooltipCustomDrawDisable = GetMyRegLong("Tooltip", "TipDisableCustomDraw", TRUE);
	bTooltipUpdateEnable[0] = GetMyRegLong("Tooltip", "Tip1Update", 0);
	bTooltipUpdateEnable[1] = GetMyRegLong("Tooltip", "Tip2Update", 0);
	bTooltipUpdateEnable[2] = GetMyRegLong("Tooltip", "Tip3Update", 0);

	dwTooltipFonCol = GetMyRegLongEx(NULL, "TipFonColor",
		0x80000000 | COLOR_INFOTEXT, gConfigNo);
	dwTooltipBakCol = GetMyRegLongEx(NULL, "TipBakColor",
		0x80000000 | COLOR_INFOBK, gConfigNo);

	bTooltipEnableDoubleBuffering = GetMyRegLong("Tooltip", "TipEnableDoubleBuffering", FALSE);

	nTooltipIcon = GetMyRegLongEx("", "TipIcon", 0, gConfigNo);

	iTooltipDispInterval = (int)(short)GetMyRegLong("Tooltip", "TipDispInterval", 1);
	if (iTooltipDispInterval > 30) iTooltipDispInterval = 30;
	if (iTooltipDispInterval < 1) iTooltipDispInterval = 1;

	iTooltipSizeX = GetMyRegLong("Tooltip", "HTMLTipDispSizeX", 200);
	iTooltipSizeY = GetMyRegLong("Tooltip", "HTMLTipDispSizeY", 200);
}

BOOL TooltipOnNotify(LRESULT *plRes, LPARAM lParam)
{
	if (!hwndTooltip || hwndTooltip != ((LPNMHDR)lParam)->hwndFrom) return FALSE;
	switch (((LPNMHDR)lParam)->code)
	{
		case NM_CUSTOMDRAW:
			if (bTooltipCustomDrawDisable) break;
			if (((LPNMCUSTOMDRAW)lParam)->dwDrawStage == CDDS_PREPAINT)
			{
				if (((LPNMCUSTOMDRAW)lParam)->rc.top)
				{
					RECT rc;
					TooltipUpdate(((LPNMCUSTOMDRAW)lParam)->hdc, &((LPNMCUSTOMDRAW)lParam)->rc, &rc, ((LPNMTTCUSTOMDRAW)lParam)->uDrawFlags);
					ExcludeClipRect(((LPNMCUSTOMDRAW)lParam)->hdc, rc.left, rc.top, rc.right, rc.bottom);
				}
				else
				{
					ExcludeClipRect(((LPNMCUSTOMDRAW)lParam)->hdc, ((LPNMCUSTOMDRAW)lParam)->rc.left, ((LPNMCUSTOMDRAW)lParam)->rc.top, ((LPNMCUSTOMDRAW)lParam)->rc.right, ((LPNMCUSTOMDRAW)lParam)->rc.bottom);
				}
				*plRes = CDRF_NOTIFYPOSTPAINT;
				return TRUE;
			}
			if (((LPNMCUSTOMDRAW)lParam)->dwDrawStage == CDDS_POSTPAINT)
			{
				HRGN hrgn = CreateRectRgn(((LPNMCUSTOMDRAW)lParam)->rc.left, ((LPNMCUSTOMDRAW)lParam)->rc.top, ((LPNMCUSTOMDRAW)lParam)->rc.right, ((LPNMCUSTOMDRAW)lParam)->rc.bottom);
				if (hrgn)
				{
					ExtSelectClipRgn(((LPNMCUSTOMDRAW)lParam)->hdc, hrgn, RGN_OR);
					DeleteObject(hrgn);
				}
				*plRes = 0;
				return TRUE;
			}
			break;
		case TTN_NEEDTEXTW:
			if (!bTooltipShow || !bTooltipUpdated)
			{
				TooltipUpdateText();
				TooltipUpdateTitle();
			}
			MultiByteToWideChar(CP_ACP, 0, formatTooltip, -1,
				formatTooltipW, sizeof(formatTooltipW)/sizeof(WCHAR));
			((LPTOOLTIPTEXTW)lParam)->lpszText = formatTooltipW;
			*plRes = 0;
			//if (!formatTooltipW[0] && bVista) 
			//	PostMessage(hwndClock, CLOCKM_VISTATOOLTIP, 1, 0);
			return TRUE;
		case TTN_NEEDTEXT:
			if (!bTooltipShow || !bTooltipUpdated)
			{
				TooltipUpdateText();
				TooltipUpdateTitle();
			}
			((LPTOOLTIPTEXT)lParam)->lpszText = formatTooltip;
			*plRes = 0;
			//if (!formatTooltip[0] && bVista) 
			//	PostMessage(hwndClock, CLOCKM_VISTATOOLTIP, 1, 0);
			return TRUE;
		case TTN_SHOW:
			if (!bTooltipShow)
			{
				TooltipUpdateText();
				TooltipUpdateTitle();
				SetWindowPos(hwndTooltip, HWND_TOPMOST, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
				bTooltipShow = TRUE;
			}
			break;
		case TTN_POP:
			bTooltipShow = FALSE;
			bTooltipUpdated = FALSE;
			break;
	}
	return FALSE;
}

DWORD TooltipFindFormat(void)
{
	DWORD dwInfoTooltip = 0;
	char tip[LEN_TOOLTIP];

	//	Tooltipのテキストをレジストリから読み込む
	GetMyRegStrEx("Tooltip", "Tooltip", tip, sizeof(tip), "", gConfigNo);
	//ファイル指定であった場合
	if(tip[0] == 'f' && tip[1] == 'i' && tip[2] == 'l' && tip[3] == 'e' && tip[4] == ':')
	{
		//Pathを切り出す
		memmove( tip, tip + 5, (size_t)((strchr(tip,'\0')-1)-tip));
		if(!GetTooltipText(tip)){
			strcpy(tip, "ファイル取得失敗");
		}
	}
	dwInfoTooltip |= FindFormat(tip);
	//	Tooltipのテキストをレジストリから読み込む
	GetMyRegStrEx("Tooltip", "Tooltip2", tip, sizeof(tip), "", gConfigNo);
	//ファイル指定であった場合
	if(tip[0] == 'f' && tip[1] == 'i' && tip[2] == 'l' && tip[3] == 'e' && tip[4] == ':')
	{
		//Pathを切り出す
		memmove( tip, tip + 5, (size_t)((strchr(tip,'\0')-1)-tip));
		if(!GetTooltipText(tip)){
			strcpy(tip, "ファイル取得失敗");
		}
	}
	dwInfoTooltip |= FindFormat(tip);
	//	Tooltipのテキストをレジストリから読み込む
	GetMyRegStrEx("Tooltip", "Tooltip3", tip, sizeof(tip), "", gConfigNo);
	//ファイル指定であった場合
	if(tip[0] == 'f' && tip[1] == 'i' && tip[2] == 'l' && tip[3] == 'e' && tip[4] == ':')
	{
		//Pathを切り出す
		memmove( tip, tip + 5, (size_t)((strchr(tip,'\0')-1)-tip));
		if(!GetTooltipText(tip)){
			strcpy(tip, "ファイル取得失敗");
		}
	}
	dwInfoTooltip |= FindFormat(tip);
	GetMyRegStrEx("", "TipTitle", tip, sizeof(tip), "", gConfigNo);
	dwInfoTooltip |= FindFormat(tip);
	return dwInfoTooltip;
}

void TooltipOnMouseEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MSG msg;
	switch(dwTooltipTypeCur)
	{
	case TOOLTIPTYPE_NORMAL:
	case TOOLTIPTYPE_BALLOON:
		msg.hwnd = hwnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = GetMessageTime();
		msg.pt.x = GET_X_LPARAM(GetMessagePos());
		msg.pt.y = GET_Y_LPARAM(GetMessagePos());
		if(hwndTooltip)
			SendMessage(hwndTooltip, TTM_RELAYEVENT, 0, (LPARAM)&msg);
		break;
	}
}
