/*-------------------------------------------
  startmenu.c
    customize start menu
    Kazubon 1997-1999
---------------------------------------------*/

#include "tcdll.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL  0x020A
#endif

extern HANDLE hmod;
extern PSTR CreateFullPathName(HINSTANCE hmod, PSTR fname);

static void OnDrawItem(HWND hwnd, DRAWITEMSTRUCT* pdis);
static BOOL IsStartMenu(HMENU hmenu);
void DeleteStartMenuRes(void);

/*------------------------------------------------
　Globals
--------------------------------------------------*/
static BOOL bStartMenu = FALSE;
static HWND hwndBar = NULL;           // タスクバーのウィンドウハンドル
static WNDPROC oldWndProcBar = NULL;  // ウィンドウプロシージャを保存
static HDC hdcMemMenu = NULL;         // メニュー描画用メモリDC
static HBITMAP hbmpMenu = NULL;       // メニュー描画用ビットマップ
static HDC hdcMemMenuLeft = NULL;     // 「Windows95」部分用メモリDC
static HBITMAP hbmpMenuLeft = NULL;   // 「Windows95」部分用ビットマップ
static int hStartMenu = 0;            // メニューの高さを保存
static COLORREF colMenuLeft;          // 色
static BOOL bTile = FALSE;     // 並べる

extern HWND hwndTClockMain;

/*------------------------------------------------
　タスクバーのサブクラスプロシージャ
--------------------------------------------------*/
LRESULT CALLBACK WndProcBar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		// メニューのオーナードロー
		case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* pdis;

			if(!bStartMenu) break;
			pdis = (DRAWITEMSTRUCT*)lParam;
			// スタートメニューでなかったらそのまま
			if(wParam || !IsStartMenu((HMENU)pdis->hwndItem)) break;
			OnDrawItem(hwnd, pdis);
			return 1;
		}
		// システム色変更
		case WM_SYSCOLORCHANGE:
			if(hdcMemMenu) DeleteDC(hdcMemMenu); hdcMemMenu = NULL;
			if(hbmpMenu) DeleteObject(hbmpMenu); hbmpMenu = NULL;
			break;
		// v1.7  タスクバーの表示が乱れるのを防ぐ
		case WM_EXITSIZEMOVE:
			PostMessage(hwnd, WM_SIZE, SIZE_RESTORED, 0);
			break;

		case WM_DESTROY:
			DeleteStartMenuRes();
			break;
		case WM_MOUSEWHEEL:
			PostMessage(hwndTClockMain, message, wParam, lParam);
			break;
	}
	return CallWindowProc(oldWndProcBar, hwnd, message, wParam, lParam);
}

/*------------------------------------------------
　メニューのオーナードロー
--------------------------------------------------*/
void OnDrawItem(HWND hwnd, DRAWITEMSTRUCT* pdis)
{
	HDC hdc;
	RECT rcBox, rcItem;
	HBRUSH hbr;
	BITMAP bmp;

	hdc = pdis->hDC;
	CopyRect(&rcItem, &(pdis->rcItem));
	GetClipBox(hdc, &rcBox); // メニュー全体の大きさ

	// 描画用メモリDCとビットマップの作成
	if(hdcMemMenu == NULL ||
		(hStartMenu != rcBox.bottom && rcBox.left == 0))
	{
		if(hdcMemMenu) DeleteDC(hdcMemMenu);
		if(hbmpMenu) DeleteObject(hbmpMenu);

		hdcMemMenu = CreateCompatibleDC(hdc);
		hbmpMenu = CreateCompatibleBitmap(hdc, rcBox.right, rcBox.bottom);
		SelectObject(hdcMemMenu, hbmpMenu);
		hbr = CreateSolidBrush(GetSysColor(COLOR_MENU));
		FillRect(hdcMemMenu, &rcBox, hbr);
		hStartMenu = rcBox.bottom;
	}

	SelectObject(hdcMemMenu, (HFONT)GetCurrentObject(hdc, OBJ_FONT));

	// 背景色、文字色の設定
	if(pdis->itemState & ODS_FOCUS)
	{
		SetTextColor(hdcMemMenu, GetSysColor(COLOR_HIGHLIGHTTEXT));
		SetBkColor(hdcMemMenu, GetSysColor(COLOR_HIGHLIGHT));
	}
	else
	{
		SetTextColor(hdcMemMenu, GetSysColor(COLOR_MENUTEXT));
		SetBkColor(hdcMemMenu, GetSysColor(COLOR_MENU));
	}

	// メモリDCにデフォルトの描画をさせる
	pdis->hDC = hdcMemMenu;
	CallWindowProc(oldWndProcBar, hwnd, WM_DRAWITEM, 0, (LPARAM)pdis);

	// 「Windows95」の幅が、pdis->rcItem.leftに入ってくる
	rcItem.right = pdis->rcItem.left;

	if(rcItem.right > 0)
	{
		COLORREF col;

		if(!bTile)
		{
			//「色」で塗りつぶし
			col = colMenuLeft;
			if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
			hbr = CreateSolidBrush(col);
			FillRect(hdcMemMenu, &rcItem, hbr);
			DeleteObject(hbr);
		}

		if(hbmpMenuLeft)
			GetObject(hbmpMenuLeft, sizeof(BITMAP), &bmp);

		if(hbmpMenuLeft)
		// && rcItem.bottom > rcBox.bottom - bmp.bmHeight)
		{
			int i, j;
			// ビットマップ描画
			for(i = 0; ; i++)
			{
				int y, ysrc, h, x, w;
				for(j = 0; ; j++)
				{
					y = rcBox.bottom - ((i + 1) * bmp.bmHeight);
					ysrc = 0;
					h = bmp.bmHeight;
					if(y < 0)
					{
						y = 0;
						ysrc = ((i + 1) * bmp.bmHeight) - rcBox.bottom;
						h -= ysrc;
					}
					x = j * bmp.bmWidth; w = bmp.bmWidth;
					if(x + w > rcItem.right)
					{
						w -= ((j + 1) * bmp.bmWidth) - rcItem.right;
					}
					if(w > 0 && h > 0)
						BitBlt(hdcMemMenu, x, y, w, h,
							hdcMemMenuLeft, 0, ysrc, SRCCOPY);
					if(!bTile || w < bmp.bmWidth) break;
				}
				if(!bTile || y == 0) break;
			}
		}
	}

	// 本当のDCにまとめて描画
	BitBlt(hdc, 0, rcItem.top,
		pdis->rcItem.right, rcItem.bottom - rcItem.top,
		hdcMemMenu, 0, rcItem.top, SRCCOPY);
	pdis->hDC = hdc;
}

/*--------------------------------------------------
　スタートメニュー改造の初期化
----------------------------------------------------*/
void SetStartMenu(HWND hwndClock)
{
	HWND hwndTray;

	char fname[1024];

	EndStartMenu();

	// タスクバーのサブクラス化（v1.7より必ずサブクラス化）
	hwndTray = GetParent(hwndClock); // TrayNotifyWnd
	if(hwndTray == NULL)
		return;
	hwndBar = GetParent(hwndTray);   // Shell_TrayWnd
	if(hwndBar == NULL)
		return;
	oldWndProcBar = (WNDPROC)GetWindowLong(hwndBar, GWL_WNDPROC);
	SetWindowLong(hwndBar, GWL_WNDPROC, (LONG)WndProcBar);

	bStartMenu = GetMyRegLong(NULL, "StartMenu", FALSE);

	if(!bStartMenu) return;

	colMenuLeft = GetMyRegLong(NULL, "StartMenuCol",
		RGB(128, 128, 128));

	bTile = GetMyRegLong(NULL, "StartMenuTile", FALSE);

	GetMyRegStr(NULL, "StartMenuBmp", fname, 1024, "");

	if(fname[0]) // 「Windows95」部分用のメモリDCとビットマップ作成
	{
		PSTR path;
		path = CreateFullPathName(hmod, fname);
		if (path == NULL) {
			hbmpMenuLeft = ReadBitmap(hwndBar, fname, FALSE);
		} else {
			hbmpMenuLeft = ReadBitmap(hwndBar, path, FALSE);
			free(path);
		}
		if(hbmpMenuLeft)
		{
			HDC hdc;
			hdc = GetDC(hwndBar);
			hdcMemMenuLeft = CreateCompatibleDC(hdc);
			SelectObject(hdcMemMenuLeft, hbmpMenuLeft);
			ReleaseDC(hwndBar, hdc);
		}
	}
}

/*--------------------------------------------------
　元に戻す
----------------------------------------------------*/
void DeleteStartMenuRes(void)
{
	if(hdcMemMenu) DeleteDC(hdcMemMenu); hdcMemMenu = NULL;
	if(hbmpMenu) DeleteObject(hbmpMenu); hbmpMenu = NULL;
	if(hdcMemMenuLeft) DeleteDC(hdcMemMenuLeft); hdcMemMenuLeft = NULL;
	if(hbmpMenuLeft) DeleteObject(hbmpMenuLeft); hbmpMenuLeft = NULL;
}
void EndStartMenu(void)
{
	if(hwndBar && IsWindow(hwndBar))
	{
#if ENABLE_CHECK_SUBCLASS_NESTING
		if (oldWndProcBar && (WNDPROC)WndProcBar == (WNDPROC)GetWindowLong(hwndBar, GWL_WNDPROC))
#else
		if (oldWndProcBar)
#endif
			SetWindowLong(hwndBar, GWL_WNDPROC, (LONG)oldWndProcBar);
	}
	hwndBar = NULL; oldWndProcBar = NULL;

	DeleteStartMenuRes();
}

/*--------------------------------------------------
　スタートメニューかどうか判別
----------------------------------------------------*/
BOOL IsStartMenu(HMENU hmenu)
{
	int i, count, id;

	count = GetMenuItemCount(hmenu);
	for(i = 0; i < count; i++)
	{
		id = GetMenuItemID(hmenu, i);
		// 「ヘルプ」があればスタートメニュー
		if(id == 503) return TRUE;
	}
	return FALSE;
}

// ------------------------------------------------------
// 以下、IE4用

LRESULT CALLBACK WndProcStartMenu(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam);
HWND hwndStartMenu = NULL; // スタートメニューのウィンドウハンドル
WNDPROC oldWndProcStartMenu = NULL;
void OnPaintStartmenuIE4(HWND hwnd, HDC hdc, BOOL bPrint);

/*--------------------------------------------------
　スタートメニューウィンドウのサブクラス化
----------------------------------------------------*/
void InitStartMenuIE4(void)
{
	HWND hwnd, hwndChild;
	HWND hwndFound;
	char classname[80];
	RECT rc1, rc2;

	ClearStartMenuIE4();
	if(!bStartMenu) return;
	// スタートメニューを探す
	hwnd = GetDesktopWindow();
	hwnd = GetWindow(hwnd, GW_CHILD);
	hwndFound = NULL;
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "BaseBar") == 0)
		{
			if(GetWindowThreadProcessId(hwnd, NULL) ==
				GetCurrentThreadId())
			{
				hwndChild = GetWindow(hwnd, GW_CHILD);
				GetClientRect(hwnd, &rc1);
				GetClientRect(hwndChild, &rc2);
				if(rc1.right - rc2.right == 21 || rc2.right == 0)
				{
					if(hwndFound == NULL
						|| (int)hwndFound > (int)hwnd)
						hwndFound = hwnd;
				}
			}
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	hwnd = hwndFound;
	if(hwnd == NULL) return;
	// サブクラス化
	oldWndProcStartMenu = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
	SetWindowLong(hwnd, GWL_WNDPROC, (LONG)WndProcStartMenu);
	hwndStartMenu = hwnd;
}

/*--------------------------------------------------
　スタートメニューウィンドウを元に戻す
----------------------------------------------------*/
void ClearStartMenuIE4(void)
{
	if(hwndStartMenu && IsWindow(hwndStartMenu))
	{
#if ENABLE_CHECK_SUBCLASS_NESTING
		if (oldWndProcStartMenu && (WNDPROC)WndProcStartMenu == (WNDPROC)GetWindowLong(hwndStartMenu, GWL_WNDPROC))
#else
		if (oldWndProcStartMenu)
#endif
			SetWindowLong(hwndStartMenu, GWL_WNDPROC, (LONG)oldWndProcStartMenu);
	}
	hwndStartMenu = NULL; oldWndProcStartMenu = NULL;
}

/*------------------------------------------------
　スタートメニューのサブクラスプロシージャ
--------------------------------------------------*/
LRESULT CALLBACK WndProcStartMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		// メニューの描画
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			if(!bStartMenu) break;
			hdc = BeginPaint(hwnd, &ps);
			OnPaintStartmenuIE4(hwnd, hdc, FALSE);
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_PRINT:
		{
			LRESULT r;
			if(!bStartMenu) break;
			r = CallWindowProc(oldWndProcStartMenu, hwnd, message, wParam, lParam);
			OnPaintStartmenuIE4(hwnd, (HDC)wParam, TRUE);
			return r;
		}
		// システム色変更
		case WM_SYSCOLORCHANGE:
			break;
	}
	return CallWindowProc(oldWndProcStartMenu, hwnd, message, wParam, lParam);
}

/*------------------------------------------------
　スタートメニューの描画
--------------------------------------------------*/
void OnPaintStartmenuIE4(HWND hwnd, HDC hdc, BOOL bPrint)
{
	RECT rc, rcWin, rcChild;
	POINT pt;
	COLORREF col;
	HBRUSH hbr;
	BITMAP bmp;
	int hClient, wClient;

	GetWindowRect(GetWindow(hwnd, GW_CHILD), &rcChild);
	GetWindowRect(hwnd, &rcWin);
	GetClientRect(hwnd, &rc);
	pt.x = 0; pt.y = 0;
	ClientToScreen(hwnd, &pt);
	if(pt.x == rcChild.left) return;

	rc.right = 21;
	wClient = rc.right; hClient = rc.bottom;
	if(bPrint)
	{
		int dx, dy;
		dx = pt.x - rcWin.left; dy = pt.y - rcWin.top;
		rc.left += dx; rc.right += dx;
		rc.top += dy; rc.bottom += dy;
	}
	//「色」で塗りつぶし
	col = colMenuLeft;
	if(col & 0x80000000) col = GetSysColor(col & 0x00ffffff);
	hbr = CreateSolidBrush(col);
	FillRect(hdc, &rc, hbr);
	DeleteObject(hbr);
	if(hbmpMenuLeft)
	{
		int i, j;
		GetObject(hbmpMenuLeft, sizeof(BITMAP), &bmp);
		// ビットマップ描画
		for(i = 0; ; i++)
		{
			int y, ysrc, h, x, w;
			for(j = 0; ; j++)
			{
				y = hClient - ((i + 1) * bmp.bmHeight);
				ysrc = 0;
				h = bmp.bmHeight;
				if(y < 0)
				{
					y = 0;
					ysrc = ((i + 1) * bmp.bmHeight) - hClient;
					h -= ysrc;
				}
				x = j * bmp.bmWidth; w = bmp.bmWidth;
				if(x + w > wClient)
				{
					w -= ((j + 1) * bmp.bmWidth) - wClient;
				}
				if(w > 0 && h > 0)
					BitBlt(hdc, rc.left + x, rc.top + y, w, h,
						hdcMemMenuLeft, 0, ysrc, SRCCOPY);
				if(!bTile || w < bmp.bmWidth) break;
			}
			if(!bTile || y == 0) break;
		}
	}
}

