/*-------------------------------------------
  startbtn.c
    customize start button
    Kazubon 1997-1999
---------------------------------------------*/

#include "tcdll.h"

#ifndef CCM_FIRST
#define CCM_FIRST               0x2000
#endif
#ifndef CCM_SETCOLORSCHEME
#define CCM_SETCOLORSCHEME      (CCM_FIRST + 2)
#endif

#define USE_FORMAT_FOR_STARTBTN	1	// add by 505

#define USE_ANALOG_CLOCK	1	// add by 505

extern PSTR CreateFullPathName(HINSTANCE hmod, PSTR fname);

#ifdef USE_ANALOG_CLOCK	// add by 505
enum {
	ANALOG_CLOCK_POS_LEFT,
	ANALOG_CLOCK_POS_RIGHT,
	ANALOG_CLOCK_POS_MIDDLE
};
extern BOOL bAnalogClockAtStartBtn;
extern SIZE sizeAClock;
extern int nAnalogClockPos;
extern BOOL DrawAnalogClock(HDC hDC, SYSTEMTIME* pt, int xclock, int yclock, int wclock, int hclock);
#endif //USE_ANALOG_CLOCK	// add by 505

extern HANDLE hmod;
extern BOOL bVisualStyle;

/*------------------------------------------------
  globals
--------------------------------------------------*/
LRESULT CALLBACK WndProcStart(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcTask(HWND, UINT, WPARAM, LPARAM);
HWND hwndStart = NULL;
BOOL bStartMenuClock = FALSE;
BOOL bUseAlphaBlend = FALSE;
HBITMAP hbmpStartButton = NULL;

static WNDPROC oldWndProcStart = NULL, oldWndProcTask = NULL;
static HWND hwndTask = NULL, hwndTray = NULL;
static HBITMAP hbmpstart = NULL, hbmpstartold = NULL;
static int wStart = -1, hStart = -1;
static BOOL bCustStartButton = FALSE;
static BOOL bHideStartButton = FALSE;
static BOOL bStartButtonFlat = FALSE;
static BOOL bNormalStartButton = FALSE;
static BOOL bCursorOnStartButton = FALSE;
static BOOL bHiddenGrippers = FALSE;
static BOOL bIsRebar = FALSE;
static LONG_PTR oldClassStyleStartButton = 0;
static COLORREF CoolStartColorHigh, CoolStartColorLow, CoolStartColorFace,
			CoolStartColorHigh2, CoolStartColorLow2;

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
static char StartCaption[1024];
#else //USE_FORMAT_FOR_STARTBTN	// add by 505
static char StartCaption[80];
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505
static BOOL bStartTextDisable = FALSE;
static char StartFontName[80];

static void OnPaint(HWND hwnd, HDC hdc);
static void OnPaintXP(HWND hwnd, HDC hdc);
static void OnPaint9x(HWND hwnd, HDC hdc);
static void SetStartButtonBmp(void);
static void SetStartButtonBmpXP(void);
static void SetStartButtonBmp9x(void);
static void SetTaskWinPos(void);
static void SetRebarGrippers(void);

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505

BOOL bStartBtnDispOther = FALSE;
BOOL bStartBtnDispSecond = FALSE;
static int nStartBtnDispBeat = FALSE;
static DWORD dwStartBtnInfoFormat = 0;
BOOL bUseFormatForStartBtn = FALSE;

static char szStartBtnText[86] = "";
static HFONT hStartBtnFont = NULL;
static int nStartBtnTextSx = 0;

extern int nDispBeat;
extern void GetDisplayTime(SYSTEMTIME* pt, int* beat100);
static void GetFormatString(PSTR s, PSTR format)
{
	SYSTEMTIME t;
	int beat100 = 0;

	if (*format == '\0') {
		*s = '\0';
		return;
	}

	GetDisplayTime(&t, nStartBtnDispBeat?(&beat100):NULL);
	MakeFormat(s, &t, beat100, format);
}
#endif // USE_FORMAT_FOR_STARTBTN // add by 505

/*--------------------------------------------------
   initialize
----------------------------------------------------*/

BOOL GetVisualStyleInfo(wchar_t *dllname, wchar_t *sizename, wchar_t *colorname)
{
	int result;
	unsigned long length;
	HKEY hKey;
	wchar_t dllnametempW[MAX_PATH];

	//レジストリキーのオープン
	result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\ThemeManager", 0, KEY_READ, &hKey);
	if(result != ERROR_SUCCESS)
		return FALSE;

	//VisualStyle DLL名ゲット
	length = _MAX_PATH;
	result = RegQueryValueExW(hKey,L"DllName", NULL, NULL, (BYTE*)dllname, &length);
	if(result != ERROR_SUCCESS)
		return FALSE;

	//%SystemRoot%を置き換える
	if(wcsstr(dllname,L"%SystemRoot%"))
	{
		wcscpy(dllnametempW, dllname);
		GetEnvironmentVariableW(L"SystemRoot", dllname, MAX_PATH);
		wcscat(dllname, dllnametempW + wcslen(L"%SystemRoot%"));
	}

	//サイズ名ゲット
	length = 100;
	result = RegQueryValueExW(hKey,L"SizeName", NULL, NULL, (BYTE*)sizename, &length);
	if(result != ERROR_SUCCESS)
		return FALSE;

	//カラー名ゲット
	length = 100;
	result = RegQueryValueExW(hKey,L"ColorName", NULL, NULL, (BYTE*)colorname, &length);
	if(result != ERROR_SUCCESS)
		return FALSE;

	//レジストリキーのクローズ
	RegCloseKey(hKey);

	return TRUE;
}

int ReadResource(HMODULE hModule,wchar_t *name,wchar_t *type,wchar_t **buffer)
{
	HGLOBAL hGlobal;
	HRSRC hRsrc;
	void *ptr;
	int size;

	hRsrc = FindResourceW(hModule,name,type);
	hGlobal = LoadResource(hModule,hRsrc);
	ptr = LockResource(hGlobal);
	size = SizeofResource(hModule,hRsrc);

	*buffer = (wchar_t*)GlobalAlloc(GMEM_FIXED,size);
	CopyMemory(*buffer,ptr,size);

	return size;
}

#define GET_STARTBUTTONBITMAP 0
#define GET_STARTFONTNAME     1

BOOL LoadStartButtonBitmap(int gettype)
{
	HMODULE hStyle;
	int IniFileLength,i;
	int CurrentColor,CurrentSize,nColor,nSize;
	wchar_t SizeNameW[100],ColorNameW[100],DllNameW[MAX_PATH];
	wchar_t *ColorNames,*SizeNames,*FileResNames,*IniFileW;
	wchar_t ImageFileNameW[MAX_PATH];
	wchar_t *ptr,tmpname[MAX_PATH],tmppath[MAX_PATH];
	wchar_t StartFontNameW[MAX_PATH];
	HANDLE hTmp;
	DWORD written;

	//レジストリからVisualStyleの情報をゲット
	if(GetVisualStyleInfo(DllNameW, SizeNameW, ColorNameW) == FALSE)
		return FALSE;

	//DLLをロード
	hStyle = LoadLibraryW(DllNameW);

	ReadResource(hStyle,L"#1",L"COLORNAMES",&ColorNames);
	ReadResource(hStyle,L"#1",L"SIZENAMES",&SizeNames);
	ReadResource(hStyle,L"#1",L"FILERESNAMES",&FileResNames);

	//色数をカウント
	for(nColor = 0,CurrentColor = 0; *ColorNames; nColor++)
	{
		if(wcsstr(ColorNames,ColorNameW))
			CurrentColor = nColor;

		ColorNames += wcslen(ColorNames) + 1;
	}

	//サイズ数をカウント
	for(nSize = 0,CurrentSize = 0; *SizeNames; nSize++)
	{
		if(wcsstr(SizeNames,SizeNameW))
			CurrentSize = nSize;

		SizeNames += wcslen(SizeNames) + 1;
	}

	//INIファイル名決定
	for(i = 0;i < nSize * CurrentColor + CurrentSize;i++)
		FileResNames += wcslen(FileResNames) + 1;

	//INIファイル読み込み
	IniFileLength = ReadResource(hStyle,FileResNames,L"TEXTFILE",&IniFileW);

	//テンポラリファイルにINIファイル書き出し
	GetTempPathW(MAX_PATH,tmppath);
	GetTempFileNameW(tmppath,L"vsc",0,tmpname);

	hTmp = CreateFileW(tmpname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hTmp,IniFileW,IniFileLength,&written,0);
	CloseHandle(hTmp);

	switch(gettype)
	{
	case GET_STARTBUTTONBITMAP:
		//スタートボタンのImageFile名読み出し
		GetPrivateProfileStringW(L"Start::Button",L"ImageFile",L"",ImageFileNameW,_MAX_PATH,tmpname);
		//ImageFile名を整形。"\"と"."は"_"に置き換え
		for(ptr = wcspbrk(ImageFileNameW,L"\\.");ptr;ptr = wcspbrk(ImageFileNameW,L"\\."))
			*ptr = L'_';
		hbmpStartButton = LoadBitmapW(hStyle,ImageFileNameW);
		break;

	case GET_STARTFONTNAME:
		//スタートボタンのFont名読み出し
		GetPrivateProfileStringW(L"Start::Button",L"Font",L"",StartFontNameW,_MAX_PATH,tmpname);
		wcstombs(StartFontName, StartFontNameW, sizeof(StartFontName));
//		MessageBox(NULL, StartFontName, "start font", MB_OK);
		break;
	}

	//テンポラリファイル削除
	DeleteFileW(tmpname);

	//不要なメモリを解放
	GlobalFree(ColorNames);
	GlobalFree(SizeNames);
	GlobalFree(FileResNames);
	GlobalFree(IniFileW);

	FreeLibrary(hStyle);

	return TRUE;
}

void SetStartButton(HWND hwndClock)
{
	HWND hwnd;
	char classname[80];

	EndStartButton();

	bStartMenuClock = FALSE;
	if(GetMyRegLong(NULL, "StartButtonHide", FALSE))
		bStartMenuClock = GetMyRegLong(NULL, "StartMenuClock", FALSE);

	// "button"と"MSTaskSwWClass"のウィンドウハンドルを得る
	hwndStart = hwndTask = NULL;
	hwndTray = GetParent(hwndClock); // TrayNotifyWnd
	if(hwndTray == NULL)
		return;
	hwnd = GetParent(hwndTray);      // Shell_TrayWnd
	if(hwnd == NULL)
		return;
	//hwndStart = FindWindowEx(FindWindow("#32769", NULL), NULL, "Button", NULL);

	hwnd = GetWindow(hwnd, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "Button") == 0)
			hwndStart = hwnd;
		else if(lstrcmpi(classname, "MSTaskSwWClass") == 0)
		{
			hwndTask = hwnd;
			bIsRebar = FALSE;
		}
		else if(lstrcmpi(classname, "ReBarWindow32") == 0)
		{
			hwndTask = hwnd;
			bIsRebar = TRUE;
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
	if(hwndStart == NULL || hwndTask == NULL)
	{
		hwndStart = hwndTask = NULL; return;
	}

	oldClassStyleStartButton = GetClassLongPtr(hwndStart, GCL_STYLE);

	bCustStartButton = GetMyRegLong(NULL, "StartButton", FALSE);
	bHideStartButton = GetMyRegLong(NULL, "StartButtonHide", FALSE);
	bHiddenGrippers  = GetMyRegLong(NULL, "RebarGripperHide", FALSE);
	bStartButtonFlat = (bWinXP && bVisualStyle) ? TRUE : GetMyRegLong(NULL, "StartButtonFlat", FALSE);

	if(bIsRebar&&bHiddenGrippers)
		SetRebarGrippers();

	if(!bCustStartButton && !bHideStartButton && !bStartButtonFlat) return;

	if(bCustStartButton || bStartButtonFlat)
	{
		if (bStartButtonFlat)
		{
			CoolStartColorHigh = GetSysColor(COLOR_3DHILIGHT);
			CoolStartColorHigh2 = GetSysColor(COLOR_3DFACE);
			CoolStartColorLow = GetSysColor(COLOR_3DSHADOW);
			CoolStartColorLow2 = GetSysColor(COLOR_3DFACE);
			CoolStartColorFace = GetSysColor(COLOR_3DFACE);
		}
		else
		{
			bNormalStartButton = TRUE;
			CoolStartColorHigh = GetSysColor(COLOR_3DHILIGHT);
			CoolStartColorHigh2 = GetSysColor(COLOR_3DLIGHT);
			CoolStartColorLow = GetSysColor(COLOR_3DDKSHADOW);
			CoolStartColorLow2 = GetSysColor(COLOR_3DSHADOW);
			CoolStartColorFace = GetSysColor(COLOR_3DFACE);
		}

		SetClassLongPtr(hwndStart, GCL_STYLE,
			oldClassStyleStartButton & ~(CS_HREDRAW|CS_VREDRAW));
	}

	// サブクラス化
	oldWndProcStart = (WNDPROC)GetWindowLongPtr(hwndStart, GWLP_WNDPROC);
	SubclassWindow(hwndStart, WndProcStart);
	oldWndProcTask = (WNDPROC)GetWindowLongPtr(hwndTask, GWLP_WNDPROC);
	SubclassWindow(hwndTask, WndProcTask);

	if(bHideStartButton) // ボタンを隠す
	{
		RECT rc; POINT pt;
		ShowWindow(hwndStart, SW_HIDE);
		wStart = 0; hStart = 0;
		GetWindowRect(hwndTray, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(GetParent(hwndTray), &pt);
		SetWindowPos(hwndStart, NULL, pt.x, pt.y,
			rc.right - rc.left, rc.bottom - rc.top,
			SWP_NOZORDER|SWP_NOACTIVATE);
	}
	else if(bCustStartButton)
	{
		// ボタン用ビットマップの設定
		SetStartButtonBmp();
	}
	// MSTaskSwWClassの位置・サイズの設定
	if(bCustStartButton || bHideStartButton)
		SetTaskWinPos();
}

/*--------------------------------------------------
    reset start button
----------------------------------------------------*/
void EndStartButton(void)
{
	if(bHiddenGrippers && hwndTask && IsWindow(hwndTask))
	{
		COLORSCHEME colScheme2;

		colScheme2.dwSize = sizeof(colScheme2);
		colScheme2.clrBtnHighlight = GetSysColor(COLOR_3DHILIGHT);
		colScheme2.clrBtnShadow = GetSysColor(COLOR_3DSHADOW);

		SendMessage(hwndTask, RB_SETCOLORSCHEME, 0, (LPARAM) (LPCOLORSCHEME)&colScheme2);
		InvalidateRect(hwndTask, NULL, TRUE);
	}

	if(hwndStart && IsWindow(hwndStart))
	{
		if(hbmpstartold != NULL)
		{
			SendMessage(hwndStart, BM_SETIMAGE,
				0, (LPARAM)hbmpstartold);
			hbmpstartold = NULL;
		}
		SetClassLongPtr(hwndStart, GCL_STYLE, oldClassStyleStartButton);

#if ENABLE_CHECK_SUBCLASS_NESTING
		if(oldWndProcStart && (WNDPROC)WndProcStart == (WNDPROC)GetWindowLongPtr(hwndStart, GWLP_WNDPROC))
#else
		if(oldWndProcStart)
#endif
			SubclassWindow(hwndStart, oldWndProcStart);
		bStartTextDisable = FALSE;
		SetWindowPos(hwndStart, NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
		ShowWindow(hwndStart, SW_SHOW);
	}
	oldWndProcStart = NULL; hwndStart = NULL;

	if(hbmpstart) DeleteObject(hbmpstart); hbmpstart = NULL;
	if(hbmpStartButton) DeleteObject(hbmpStartButton); hbmpStartButton = NULL;

	if(hwndTask && IsWindow(hwndTask))
	{
#if ENABLE_CHECK_SUBCLASS_NESTING
		if(oldWndProcTask && (WNDPROC)WndProcTask == (WNDPROC)GetWindowLongPtr(hwndTask, GWLP_WNDPROC))
#else
		if(oldWndProcTask)
#endif
			SubclassWindow(hwndTask, oldWndProcTask);
	}
	oldWndProcTask = NULL; hwndTask = NULL;

	bCustStartButton = bHideStartButton = bStartButtonFlat = FALSE;
	bNormalStartButton = FALSE; bHiddenGrippers = FALSE;


#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
	if (hStartBtnFont != NULL) {
		DeleteObject(hStartBtnFont);
		hStartBtnFont = NULL;
	}
#endif // USE_FORMAT_FOR_STARTBTN // add by 505

}

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
void RefreshStartBtnXp(void)
{
	if (bUseFormatForStartBtn == FALSE) {
		return;
	}
	if (bWinXP && bVisualStyle) {
		if (!hbmpstart) {
			// キャプションの取得
			GetFormatString(StartCaption, szStartBtnText);
			CallWindowProc(oldWndProcStart, hwndStart, WM_SETTEXT, 0, (LPARAM)StartCaption);
//			SetWindowPos(hwndStart, NULL, 0, 0, 0, 0,
//				SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
		}
	}
}
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505

/*------------------------------------------------
   subclass procedure of start button
--------------------------------------------------*/
LRESULT CALLBACK WndProcStart(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
//	if(!bCustStartButton && bWinXP && bVisualStyle)
//		return CallWindowProc(oldWndProcStart, hwnd, message, wParam, lParam);

	switch(message)
	{
		case WM_SYSCOLORCHANGE:  // システムの設定変更
		case WM_WININICHANGE:
			if(bCustStartButton && !bHideStartButton)
				PostMessage(hwnd, WM_USER+10, 0, 0L);
			return 0;
		case (WM_USER + 10):     // 再初期化
			SetStartButtonBmp();
			return 0;
		case WM_WINDOWPOSCHANGING:  // サイズを変更させない
		{
			LPWINDOWPOS pwp;
			if(!(bCustStartButton || bHideStartButton)) break;
			pwp = (LPWINDOWPOS)lParam;
			if(!(pwp->flags & SWP_NOSIZE))
			{
				if(wStart > 0) pwp->cx = wStart;
				if(hStart > 0) pwp->cy = hStart;
			}
			if(bHideStartButton)
			{
				RECT rc; POINT pt;
				GetWindowRect(hwndTray, &rc);
				pt.x = rc.left; pt.y = rc.top;
				ScreenToClient(GetParent(hwndTray), &pt);
				pwp->x = pt.x; pwp->y = pt.y;
				pwp->cx = rc.right - rc.left;
				pwp->cy = rc.bottom - rc.top;
			}
			break;
		}
		case WM_DESTROY:
			if(hbmpstartold)
				SendMessage(hwndStart, BM_SETIMAGE, 0, (LPARAM)hbmpstartold); hbmpstartold = NULL;
			if(hbmpstart)
				DeleteObject(hbmpstart); hbmpstart = NULL;
			if(hbmpStartButton)
				DeleteObject(hbmpStartButton); hbmpStartButton = NULL;
			break;

		// -------- for "flat start button" -----------
		case WM_SETTEXT:
			if(bStartTextDisable == TRUE)
				return 0;
		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
			RefreshStartBtnXp();
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505
			if(!bCustStartButton && !bStartButtonFlat) break;
			if(bWinXP && bVisualStyle && !hbmpstart) break;
			hdc = BeginPaint(hwnd, &ps);
			OnPaint(hwnd, hdc);
			EndPaint(hwnd, &ps);
			return 0;
		}
		case BM_SETSTATE:
		{
			LRESULT r;
			HDC hdc;
			if(!bCustStartButton && !bStartButtonFlat) break;
			if(bWinXP && bVisualStyle && !hbmpstart) break;
			r = CallWindowProc(oldWndProcStart, hwnd, message, wParam, lParam);
			hdc = GetDC(hwnd);
			OnPaint(hwnd, hdc);
			ReleaseDC(hwnd, hdc);
			return 0;
		}
		case WM_SETFOCUS:
			if (bCustStartButton || !bStartButtonFlat) break;
			return 0;
		case WM_MOUSELEAVE:
		case WM_MOUSEMOVE:
			if(!bCustStartButton) break;
			CheckCursorOnStartButton();
			break;
	}
	return CallWindowProc(oldWndProcStart, hwnd, message, wParam, lParam);
}

/*--------------------------------------------------
   subclass procedure of
   "MSTaskSwWClass"/"ReBarWindow32" class window
----------------------------------------------------*/
LRESULT CALLBACK WndProcTask(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_WINDOWPOSCHANGING: // 位置・サイズの制限
		{
			LPWINDOWPOS pwp;
			RECT rcBar, rcTray;

			if(!(bCustStartButton || bHideStartButton)) break;

			pwp = (LPWINDOWPOS)lParam;
//			if((pwp->flags & SWP_NOMOVE) ||
//				wStart < 0 || hStart < 0) break;

			GetClientRect(GetParent(hwndStart), &rcBar); // タスクバー
			GetWindowRect(hwndTray, &rcTray); // TrayNotifyWnd

			// タスクバーが横置きのとき
			if(rcBar.right > rcBar.bottom)
			{
				pwp->x = 2 + wStart; // 右位置
				pwp->cx = rcTray.left - 2 - wStart - 2; // 横幅
				if(wStart > 0)
				{
					pwp->x += 2; pwp->cx -= 2;
				}
			}
			else // 縦置きのとき
			{
				if(rcTray.top < pwp->y)
				{
					pwp->cy = rcBar.bottom - 2 - hStart - 2; // 高さ
				}
				else
				{
					pwp->cy = rcTray.top - 2 - hStart - 2; // 高さ
				}
				pwp->y = 2 + hStart; // 上位置
				if(hStart > 0)
				{
					pwp->y += 1; pwp->cy -= 2;
				}
			}
			break;
		}
	}
	return CallWindowProc(oldWndProcTask, hwnd, message, wParam, lParam);
}

/*--------------------------------------------------
　スタートボタンのビットマップとサイズの設定
----------------------------------------------------*/
void SetStartButtonBmp(void)
{
	if (bWinXP && bVisualStyle)
		SetStartButtonBmpXP();
	else
		SetStartButtonBmp9x();
}

void SetStartButtonBmpXP(void)
{
	char filepath[MAX_PATH];
	BITMAP bmp;
	BITMAPINFO bi;
	RECT rc;
	HFILE hf;
	PSTR path;

	if(hbmpstart) DeleteObject(hbmpstart);
	hbmpstart = NULL;
	if(hbmpStartButton) DeleteObject(hbmpStartButton);
	hbmpStartButton = NULL;

	// ファイルからアイコン用ビットマップの読み込み
	if(!GetMyRegStr(NULL, "StartButtonIcon",filepath,MAX_PATH, ""))
	{
		HDC hdc;
		HFONT hfont;
		LOGFONT logfont;
		NONCLIENTMETRICS ncm;
		SIZE sz;
		char *p;
		int point;

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
		char caption[80];

		// キャプションの取得
		bUseFormatForStartBtn = GetMyRegLong("", "StartButtonIsFormat", FALSE);

		if (bUseFormatForStartBtn == FALSE) {
			GetMyRegStr(NULL, "StartButtonCaption", szStartBtnText, 80, "");
			dwStartBtnInfoFormat = 0;
			bStartBtnDispSecond = FALSE;
			nStartBtnDispBeat = 0;
			bStartBtnDispOther = FALSE;
			strcpy(StartCaption, szStartBtnText);
		} else {
			GetMyRegStr(NULL, "StartButtonCaption", caption, 80, "");
			strcpy(szStartBtnText, "<%");
			strcat(szStartBtnText, caption);
			strcat(szStartBtnText, "%>");
			dwStartBtnInfoFormat = FindFormat(szStartBtnText);
			bStartBtnDispSecond = (dwStartBtnInfoFormat&FORMAT_SECOND)? TRUE:FALSE;
			nStartBtnDispBeat = dwStartBtnInfoFormat & (FORMAT_BEAT1 | FORMAT_BEAT2);
			bStartBtnDispOther = (dwStartBtnInfoFormat& ~(FORMAT_BEAT1 | FORMAT_BEAT2 | FORMAT_SECOND))? TRUE:FALSE;

			GetFormatString(StartCaption, szStartBtnText);
		}
#else //USE_FORMAT_FOR_STARTBTN	// add by 505
		GetMyRegStr(NULL, "StartButtonCaption", StartCaption, 80, "");
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505

		bStartTextDisable = FALSE;
		SendMessage(hwndStart, WM_SETTEXT, 0, (LPARAM)StartCaption);
		SetWindowPos(hwndStart, NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
		bStartTextDisable = TRUE;

		GetClientRect(hwndStart,&rc);
		hStart = rc.bottom - rc.top;

		StartFontName[0] = 0;
		if(LoadStartButtonBitmap(GET_STARTFONTNAME) == TRUE)
		{
			//visual style
			p = strchr(StartFontName, ',');
			*p = 0;
			point = atoi(p+1);
			if(point == 0) point = 9;

			hdc = GetDC(hwndStart);
			memset(&logfont, 0, sizeof(LOGFONT));
			logfont.lfHeight = -MulDiv(point, GetDeviceCaps(hdc, LOGPIXELSY), 72);
			logfont.lfWeight = 400;
//			logfont.lfCharSet = DEFAULT_CHARSET;		//→2バイト文字の長さがおかしくなる
			logfont.lfCharSet = SHIFTJIS_CHARSET;
//			strcpy(logfont.lfFaceName, "Franklin Gothic Medium");
			strcpy(logfont.lfFaceName, StartFontName);

			hfont = CreateFontIndirect(&logfont);
			SelectObject(hdc, hfont);
			//キャプションの幅を得る
			GetTextExtentPoint32(hdc, StartCaption, strlen(StartCaption), &sz);
			wStart = sz.cx + 64;			//→64は根拠の無い数値
#if 0
#ifdef USE_ANALOG_CLOCK	// add by 505
			if (bAnalogClockAtStartBtn != FALSE) {
				if (nAnalogClockPos != ANALOG_CLOCK_POS_MIDDLE) {
					wStart += sizeAClock.cx + 1;
				}
			}
#endif //USE_ANALOG_CLOCK	// add by 505
#endif
			ReleaseDC(hwndStart, hdc);
			DeleteObject(hfont);
		}
		else
		{
			//classic style
			hdc = GetDC(hwndStart);
			ncm.cbSize = sizeof(ncm);
			SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
			ncm.lfCaptionFont.lfWeight = FW_BOLD;
			hfont =  CreateFontIndirect(&(ncm.lfCaptionFont));
			SelectObject(hdc, hfont);
			//キャプションの幅を得る
			GetTextExtentPoint32(hdc, StartCaption, strlen(StartCaption), &sz);
			wStart = sz.cx + 30;
#if 0
#ifdef USE_ANALOG_CLOCK	// add by 505
			if (bAnalogClockAtStartBtn != FALSE) {
				if (nAnalogClockPos != ANALOG_CLOCK_POS_MIDDLE) {
					wStart += sizeAClock.cx + 1;
				}
			}
#endif //USE_ANALOG_CLOCK	// add by 505
#endif
			ReleaseDC(hwndStart, hdc);
			DeleteObject(hfont);
		}

		return;
	}

	path = CreateFullPathName(hmod, filepath);
	if (path == NULL) {
		hbmpstart = (HBITMAP)LoadImage(0,filepath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if (!hbmpstart) {
			return;
		}
		//32bitビットマップならアルファブレンドを有効化
		hf = _lopen(filepath, OF_READ);
	} else {
		hbmpstart = (HBITMAP)LoadImage(0,path,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if (!hbmpstart) {
			free(path);
			return;
		}
		//32bitビットマップならアルファブレンドを有効化
		hf = _lopen(path, OF_READ);
		free(path);
	}

	_llseek(hf, sizeof(BITMAPFILEHEADER), 0);
	_lread(hf, &bi, sizeof(bi));
	_lclose(hf);

	bUseAlphaBlend = (bi.bmiHeader.biBitCount == 32);

	//ボタンの幅はビットマップにあわせる
	// →できれば上に貼るビットマップや文字数に合わせて変化させたいが、
	//   自力でビットマップを伸び縮みさせるのは面倒
	//ボタンの高さは現状を維持
	GetClientRect(hwndStart,&rc);
	GetObject(hbmpstart,sizeof(BITMAP),&bmp);
	wStart = bmp.bmWidth;
	hStart = rc.bottom - rc.top;

#if 0
#ifdef USE_ANALOG_CLOCK	// add by 505
	if (bAnalogClockAtStartBtn != FALSE) {
		if (nAnalogClockPos != ANALOG_CLOCK_POS_MIDDLE) {
			wStart += sizeAClock.cx + 1;
		}
		if (hStart < sizeAClock.cy) {
			hStart = sizeAClock.cy;
		}
	}
#endif //USE_ANALOG_CLOCK	// add by 505
#endif

	LoadStartButtonBitmap(GET_STARTBUTTONBITMAP);
}

static void DrawStartBtnText9x(HDC hdcMem, PSTR caption, int whbmp, int hhbmp, int sx)
{
	TEXTMETRIC tm;
	int x, y;
	RECT rc;
	HBRUSH hbr;
	HFONT hOldFont;

	// ビットマップにキャプションを書く
	if(*caption == '\0') {
		return;
	}

	hOldFont = SelectObject(hdcMem, hStartBtnFont);
	GetTextMetrics(hdcMem, &tm);

	x = sx;
	if (hhbmp >= tm.tmHeight) {
		y = (hhbmp - tm.tmHeight) / 2;
	} else {
		y = 0;
	}

	// 背景色で塗りつぶし
	SetRect(&rc, x, y, whbmp, hhbmp);
	hbr = CreateSolidBrush(CoolStartColorFace);
	FillRect(hdcMem, &rc, hbr);
	DeleteObject(hbr);


	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, GetSysColor(COLOR_BTNTEXT));
	TextOut(hdcMem, x, y, caption, strlen(caption));

	SelectObject(hdcMem, hOldFont);
}

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
static void SetButtonSize9x(HWND hwndStart, int whbmp, int hhbmp)
{
	UNREFERENCED_PARAMETER(hwndStart);
	// ボタンのサイズの設定  上限：160x80
	wStart = whbmp + 8;
	if(wStart > 160) wStart = 160;
	hStart = GetSystemMetrics(SM_CYCAPTION) + 3;
	if(hhbmp + 6 > hStart) hStart = hhbmp + 6;
	if(hStart > 80) hStart = 80;

#ifdef USE_ANALOG_CLOCK	// add by 505
	if (bAnalogClockAtStartBtn != FALSE) {
		if (nAnalogClockPos != ANALOG_CLOCK_POS_MIDDLE) {
			wStart += sizeAClock.cx + 1;
		}
		if (hStart < sizeAClock.cy + 4) {
			hStart = sizeAClock.cy + 4;
		}
	}
#endif //USE_ANALOG_CLOCK	// add by 505
}
#endif //USE_FORMAT_FOR_STARTBTN	// add by 505

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505

void SetStartButtonBmp9x(void)
{
	char s[1024], caption[1024];
	HBITMAP hbmpicon, hbmpold;
	HICON hicon;
	HDC hdc, hdcMem;
	BITMAP bmp;
	int whbmp, hhbmp, cxicon, cyicon;

	if(hwndStart == NULL) return;

	hbmpicon = NULL; hicon = NULL;
	cxicon = GetSystemMetrics(SM_CXSMICON);
	cyicon = GetSystemMetrics(SM_CYSMICON);

	// ファイルからアイコン用ビットマップの読み込み
	if(GetMyRegStr(NULL, "StartButtonIcon", s, 1024, "") > 0)
	{
		char fname[MAX_PATH], head[2];
		HFILE hf;
		PSTR path;
		path = CreateFullPathName(hmod, s);
		if (path == NULL) {
			parse(fname, s, 0);
		} else {
			parse(fname, path, 0);
			free(path);
		}
		hf = _lopen(fname, OF_READ);
		if(hf != HFILE_ERROR)
		{
			_lread(hf, head, 2);
			_lclose(hf);
			if(head[0] == 'B' && head[1] == 'M') //ビットマップの場合
				hbmpicon = ReadBitmap(hwndStart, fname, TRUE);
			else if(head[0] == 'M' && head[1] == 'Z') //実行ファイルの場合
			{
				char numstr[10], *p; int n;
				HICON hiconl;
				parse(numstr, s, 1);
				n = 0; p = numstr;
				while(*p)
				{
					if(*p < '0' || '9' < *p) break;
					n = n * 10 + *p++ - '0';
				}
				if(ExtractIconEx(fname, n, &hiconl, &hicon, 1) < 2)
					hicon = NULL;
				else DestroyIcon(hiconl);
			}
			else // アイコンの場合
			{
				hicon = (HICON)LoadImage(hmod, fname,
					IMAGE_ICON, cxicon, cyicon,
					LR_DEFAULTCOLOR|LR_LOADFROMFILE);
			}
		}
	}

	if(hbmpicon)
	{
		GetObject(hbmpicon, sizeof(BITMAP), (LPVOID)&bmp);
		cxicon = bmp.bmWidth;
		cyicon = bmp.bmHeight;
	}

	// キャプションの取得
	bUseFormatForStartBtn = GetMyRegLong("", "StartButtonIsFormat", FALSE);

	if (bUseFormatForStartBtn == FALSE) {
		GetMyRegStr(NULL, "StartButtonCaption", szStartBtnText, 80, "");
		dwStartBtnInfoFormat = 0;
		bStartBtnDispSecond = FALSE;
		nStartBtnDispBeat = 0;
		bStartBtnDispOther = FALSE;
		strcpy(caption, szStartBtnText);
	} else {
		GetMyRegStr(NULL, "StartButtonCaption", caption, 80, "");
		strcpy(szStartBtnText, "<%");
		strcat(szStartBtnText, caption);
		strcat(szStartBtnText, "%>");
		dwStartBtnInfoFormat = FindFormat(szStartBtnText);
		bStartBtnDispSecond = (dwStartBtnInfoFormat&FORMAT_SECOND)? TRUE:FALSE;
		nStartBtnDispBeat = dwStartBtnInfoFormat & (FORMAT_BEAT1 | FORMAT_BEAT2);
		bStartBtnDispOther = (dwStartBtnInfoFormat& ~(FORMAT_BEAT1 | FORMAT_BEAT2 | FORMAT_SECOND))? TRUE:FALSE;
		GetFormatString(caption, szStartBtnText);
	}

	hdc = GetDC(hwndStart);


	// ボタン用のフォント = タイトルバーのフォント + BOLD
	whbmp = cxicon; hhbmp = cyicon;
	{
		NONCLIENTMETRICS ncm;
		SIZE sz;

		ncm.cbSize = sizeof(ncm);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
		ncm.lfCaptionFont.lfWeight = FW_BOLD;
		hStartBtnFont =  CreateFontIndirect(&(ncm.lfCaptionFont));
		SelectObject(hdc, hStartBtnFont);

		if (caption[0]) {
			//キャプションの幅を得る
			GetTextExtentPoint32(hdc, caption, strlen(caption), &sz);
			whbmp = sz.cx;
			if(hbmpicon || hicon) whbmp += cxicon + 2;
			hhbmp = sz.cy;
			if((hbmpicon || hicon) && cyicon > sz.cy)
				hhbmp = cyicon;
			//if(hhbmp < 16) hhbmp = 16;
		}
	}

	// ビットマップの作成
	hdcMem = CreateCompatibleDC(hdc);
	hbmpstart = CreateCompatibleBitmap(hdc, whbmp, hhbmp);
	SelectObject(hdcMem, hbmpstart);

	{ // 背景色で塗りつぶし
		RECT rc; HBRUSH hbr;
		SetRect(&rc, 0, 0, whbmp, hhbmp);
		hbr = CreateSolidBrush(CoolStartColorFace);
		FillRect(hdcMem, &rc, hbr);
		DeleteObject(hbr);
	}

	// ビットマップにアイコンの絵を描画
	if(hbmpicon)
	{
		HDC hdcicon;
		hdcicon = CreateCompatibleDC(hdc);
		SelectObject(hdcicon, hbmpicon);
		BitBlt(hdcMem, 0, (hhbmp - cyicon)/2,
			cxicon, cyicon, hdcicon, 0, 0, SRCCOPY);
		DeleteDC(hdcicon);
		DeleteObject(hbmpicon);
	}
	if(hicon)
	{
		DrawIconEx(hdcMem, 0, (hhbmp - cyicon)/2,
			hicon, cxicon, cyicon, 0, NULL, DI_NORMAL);
		DestroyIcon(hicon);
	}

	{
		int sx;
		sx = 0; if(hbmpicon || hicon) sx = cxicon + 2;
		nStartBtnTextSx = sx;
		DrawStartBtnText9x(hdcMem, caption, whbmp, hhbmp, sx);
	}

	DeleteDC(hdcMem);
	ReleaseDC(hwndStart, hdc);

	// ボタンにビットマップを設定
	hbmpold = (HBITMAP)SendMessage(hwndStart,
		BM_SETIMAGE, 0, (LPARAM)hbmpstart);

	// 以前のビットマップを保存 / 破棄
	if(hbmpstartold == NULL) hbmpstartold = hbmpold;
	else DeleteObject(hbmpold);

	// ボタンのサイズの設定  上限：160x80
	SetButtonSize9x(hwndStart, whbmp, hhbmp);

	SetWindowPos(hwndStart, NULL, 0, 0,
		wStart, hStart,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}

#else // USE_FORMAT_FOR_STARTBTN	// add by 505

void SetStartButtonBmp9x(void)
{
	char s[1024], caption[80];
	HBITMAP hbmpicon, hbmpold;
	HICON hicon;
	HDC hdc, hdcMem;
	HFONT hfont;
	BITMAP bmp;
	int whbmp, hhbmp, cxicon, cyicon;

	if(hwndStart == NULL) return;

	hbmpicon = NULL; hicon = NULL;
	cxicon = GetSystemMetrics(SM_CXSMICON);
	cyicon = GetSystemMetrics(SM_CYSMICON);

	// ファイルからアイコン用ビットマップの読み込み
	if(GetMyRegStr(NULL, "StartButtonIcon", s, 1024, "") > 0)
	{
		char fname[MAX_PATH], head[2];
		HFILE hf;
		PSTR path;
		path = CreateFullPathName(hmod, s);
		if (path == NULL) {
			parse(fname, s, 0);
		} else {
			parse(fname, path, 0);
			free(path);
		}
		hf = _lopen(fname, OF_READ);
		if(hf != HFILE_ERROR)
		{
			_lread(hf, head, 2);
			_lclose(hf);
			if(head[0] == 'B' && head[1] == 'M') //ビットマップの場合
				hbmpicon = ReadBitmap(hwndStart, fname, TRUE);
			else if(head[0] == 'M' && head[1] == 'Z') //実行ファイルの場合
			{
				char numstr[10], *p; int n;
				HICON hiconl;
				parse(numstr, s, 1);
				n = 0; p = numstr;
				while(*p)
				{
					if(*p < '0' || '9' < *p) break;
					n = n * 10 + *p++ - '0';
				}
				if(ExtractIconEx(fname, n, &hiconl, &hicon, 1) < 2)
					hicon = NULL;
				else DestroyIcon(hiconl);
			}
			else // アイコンの場合
			{
				hicon = (HICON)LoadImage(hmod, fname,
					IMAGE_ICON, cxicon, cyicon,
					LR_DEFAULTCOLOR|LR_LOADFROMFILE);
			}
		}
	}

	if(hbmpicon)
	{
		GetObject(hbmpicon, sizeof(BITMAP), (LPVOID)&bmp);
		cxicon = bmp.bmWidth;
		cyicon = bmp.bmHeight;
	}

	// キャプションの取得
	GetMyRegStr(NULL, "StartButtonCaption", caption, 80, "");

	hdc = GetDC(hwndStart);


	// ボタン用のフォント = タイトルバーのフォント + BOLD
	hfont = NULL;
	whbmp = cxicon; hhbmp = cyicon;
	if(caption[0])
	{
		NONCLIENTMETRICS ncm;
		SIZE sz;
		ncm.cbSize = sizeof(ncm);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
		ncm.lfCaptionFont.lfWeight = FW_BOLD;
		hfont =  CreateFontIndirect(&(ncm.lfCaptionFont));
		SelectObject(hdc, hfont);

		//キャプションの幅を得る
		GetTextExtentPoint32(hdc, caption, strlen(caption), &sz);
		whbmp = sz.cx;
		if(hbmpicon || hicon) whbmp += cxicon + 2;
		hhbmp = sz.cy;
		if((hbmpicon || hicon) && cyicon > sz.cy)
			hhbmp = cyicon;
		//if(hhbmp < 16) hhbmp = 16;
	}

	// ビットマップの作成
	hdcMem = CreateCompatibleDC(hdc);
	hbmpstart = CreateCompatibleBitmap(hdc, whbmp, hhbmp);
	SelectObject(hdcMem, hbmpstart);

	{ // 背景色で塗りつぶし
		RECT rc; HBRUSH hbr;
		SetRect(&rc, 0, 0, whbmp, hhbmp);
		hbr = CreateSolidBrush(CoolStartColorFace);
		FillRect(hdcMem, &rc, hbr);
		DeleteObject(hbr);
	}

	// ビットマップにアイコンの絵を描画
	if(hbmpicon)
	{
		HDC hdcicon;
		hdcicon = CreateCompatibleDC(hdc);
		SelectObject(hdcicon, hbmpicon);
		BitBlt(hdcMem, 0, (hhbmp - cyicon)/2,
			cxicon, cyicon, hdcicon, 0, 0, SRCCOPY);
		DeleteDC(hdcicon);
		DeleteObject(hbmpicon);
	}
	if(hicon)
	{
		DrawIconEx(hdcMem, 0, (hhbmp - cyicon)/2,
			hicon, cxicon, cyicon, 0, NULL, DI_NORMAL);
		DestroyIcon(hicon);
	}

	// ビットマップにキャプションを書く
	if(caption[0])
	{
		TEXTMETRIC tm;
		int x, y;

		GetTextMetrics(hdc, &tm);
		SelectObject(hdcMem, hfont);
		x = 0; if(hbmpicon || hicon) x = cxicon + 2;
		y = (hhbmp - tm.tmHeight) / 2;

		SetBkMode(hdcMem, TRANSPARENT);
		SetTextColor(hdcMem, GetSysColor(COLOR_BTNTEXT));
		TextOut(hdcMem, x, y, caption, strlen(caption));
	}

	DeleteDC(hdcMem);
	ReleaseDC(hwndStart, hdc);

	if(hfont) DeleteObject(hfont);

	// ボタンにビットマップを設定
	hbmpold = (HBITMAP)SendMessage(hwndStart,
		BM_SETIMAGE, 0, (LPARAM)hbmpstart);

	// 以前のビットマップを保存 / 破棄
	if(hbmpstartold == NULL) hbmpstartold = hbmpold;
	else DeleteObject(hbmpold);

	// ボタンのサイズの設定  上限：160x80
	wStart = whbmp + 8;
	if(wStart > 160) wStart = 160;
	hStart = GetSystemMetrics(SM_CYCAPTION) + 3;
	if(hhbmp + 6 > hStart) hStart = hhbmp + 6;
	if(hStart > 80) hStart = 80;

#ifdef USE_ANALOG_CLOCK	// add by 505
	if (bAnalogClockAtStartBtn != FALSE) {
		if (nAnalogClockPos != ANALOG_CLOCK_POS_MIDDLE) {
			wStart += sizeAClock.cx + 1;
			nStartBtnTextSx += sizeAClock.cx + 1;
		}
		if (hStart < sizeAClock.cy + 4) {
			hStart = sizeAClock.cy + 4;
		}
	}
#endif //USE_ANALOG_CLOCK	// add by 505

	SetWindowPos(hwndStart, NULL, 0, 0,
		wStart, hStart,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}
#endif	// USE_FORMAT_FOR_STARTBTN	// add by 505

/*--------------------------------------------------
　MSTaskSwWClassの位置・サイズの設定
----------------------------------------------------*/
void SetTaskWinPos(void)
{
	RECT rcBar, rcTask, rcTray;
	POINT pt;
	int x, y, w, h;

	GetClientRect(GetParent(hwndStart), &rcBar);  // Shell_TrayWnd
	GetWindowRect(hwndTray, &rcTray); // TrayNotifyWnd
	GetWindowRect(hwndTask, &rcTask);             // MSTaskSwWClass

	// MSTaskSwWClassの右上位置
	pt.x = rcTask.left; pt.y = rcTask.top;
	ScreenToClient(GetParent(hwndStart), &pt);

	x = pt.x; y = pt.y;
	w = rcTask.right - rcTask.left;
	h = rcTask.bottom - rcTask.top;

	// タスクバーが横置きのとき
	if(rcBar.right > rcBar.bottom)
	{
		x = 2 + wStart;
		w = rcTray.left - 2 - wStart - 2;
		if(wStart > 0)
		{
			x += 2; w -= 2;
		}
	}
	else // 縦置きのとき
	{
		y = 2 + hStart;
		h = rcTray.top - 2 - hStart - 2;
		if(hStart > 0)
		{
			y += 1; h -= 2;
		}
	}
	SetWindowPos(hwndTask, NULL, x, y, w, h,
		SWP_NOZORDER|SWP_NOACTIVATE);
}

/*--------------------------------------------------
   draw "flat start button"
----------------------------------------------------*/
void OnPaint(HWND hwnd, HDC hdc)
{
	if (bWinXP && bVisualStyle)
		OnPaintXP(hwnd, hdc);
	else
		OnPaint9x(hwnd, hdc);
}


#ifdef USE_ANALOG_CLOCK	// add by 505
void OnPaintXP(HWND hwnd, HDC hdc)
{
	HDC hdcStartButton,hdcImage,hdcTemp;
	HBITMAP hbmpTemp;
	BITMAP bmp;
	RECT rc;
	int wStartButton,hStartButton,wImage,hImage,topStartButton;
	BOOL bPushed;
	BLENDFUNCTION bf;
	int x, y;
	SYSTEMTIME t;
	int w, h;

	bPushed = (SendMessage(hwnd, BM_GETSTATE, 0, 0) & BST_PUSHED)?1:0;
	GetClientRect(hwnd, &rc);

	//StartボタンのBitmapを設定
	hdcStartButton = CreateCompatibleDC(hdc);
	SaveDC(hdcStartButton);
	SelectObject(hdcStartButton, hbmpStartButton);
	GetObject(hbmpStartButton, sizeof(BITMAP), (LPVOID)&bmp);
	wStartButton = bmp.bmWidth;
	hStartButton = bmp.bmHeight/3;

	//イメージファイル用DC
	hdcImage = CreateCompatibleDC(hdc);
	SaveDC(hdcImage);
	SelectObject(hdcImage, hbmpstart);
	GetObject(hbmpstart, sizeof(BITMAP), (LPVOID)&bmp);
	wImage = bmp.bmWidth;
	hImage = bmp.bmHeight;

	//下地用
	hdcTemp = CreateCompatibleDC(hdc);
	SaveDC(hdcTemp);
	hbmpTemp = CreateCompatibleBitmap(hdc,wImage,rc.bottom);
	SelectObject(hdcTemp, hbmpTemp);
	MyDrawThemeParentBackground(hwnd,hdcTemp,0);

	if(bPushed)
		topStartButton = hStartButton * 2;
	else
	if(bCursorOnStartButton)
		topStartButton = hStartButton * 1;
	else
		topStartButton = hStartButton * 0;

	MyTransparentBlt(hdcTemp,0,0,wImage,rc.bottom,
					hdcStartButton,0,topStartButton,wStartButton,hStartButton,0x00ff00ff);

	if (hdcImage != NULL) {
		if(bUseAlphaBlend){
			ZeroMemory(&bf,sizeof(bf));

			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.SourceConstantAlpha = 0xff;

			MyAlphaBlend(hdcTemp,0, (rc.bottom - hImage)/2,wImage,hImage,
							hdcImage,0,0,wImage,hImage,bf);
		}else{
			MyTransparentBlt(hdcTemp,0,(rc.bottom - hImage)/2,wImage,hImage,
							hdcImage,0,0,wImage,hImage,0x00ff00ff);
		}
	}

	if (bAnalogClockAtStartBtn != FALSE) {
		x = 0;
		y = (hImage - sizeAClock.cy) / 2;
		if (y < 0) {
			y = 0;
		}
		y += (rc.bottom - hImage)/2;
		w = wImage;
		h = sizeAClock.cy;
		GetLocalTime(&t);
		DrawAnalogClock(hdcTemp, &t, x, y, w, h);
	}

	BitBlt(hdc, 0, 0,wImage,rc.bottom, hdcTemp, 0, 0, SRCCOPY);

	RestoreDC(hdcTemp,-1);
	RestoreDC(hdcStartButton,-1);
	DeleteDC(hdcTemp);
	if (hbmpstart != NULL) {
		RestoreDC(hdcImage,-1);
		DeleteDC(hdcImage);
	}
	DeleteDC(hdcStartButton);
	DeleteObject(hbmpTemp);
}
#else //USE_ANALOG_CLOCK	// add by 505
void OnPaintXP(HWND hwnd, HDC hdc)
{
	HDC hdcStartButton,hdcImage,hdcTemp;
	HBITMAP hbmpTemp;
	BITMAP bmp;
	RECT rc;
	int wStartButton,hStartButton,wImage,hImage,topStartButton;
	BOOL bPushed;
	BLENDFUNCTION bf;
	int x, y;

	bPushed = (SendMessage(hwnd, BM_GETSTATE, 0, 0) & BST_PUSHED)?1:0;
	GetClientRect(hwnd, &rc);

	//StartボタンのBitmapを設定
	hdcStartButton = CreateCompatibleDC(hdc);
	SaveDC(hdcStartButton);
	SelectObject(hdcStartButton, hbmpStartButton);
	GetObject(hbmpStartButton, sizeof(BITMAP), (LPVOID)&bmp);
	wStartButton = bmp.bmWidth;
	hStartButton = bmp.bmHeight/3;

	//イメージファイル用DC
	hdcImage = CreateCompatibleDC(hdc);
	SaveDC(hdcImage);
	SelectObject(hdcImage, hbmpstart);
	GetObject(hbmpstart, sizeof(BITMAP), (LPVOID)&bmp);
	wImage = bmp.bmWidth;
	hImage = bmp.bmHeight;

	//下地用
	hdcTemp = CreateCompatibleDC(hdc);
	SaveDC(hdcTemp);
	hbmpTemp = CreateCompatibleBitmap(hdc,wImage,rc.bottom);
	SelectObject(hdcTemp, hbmpTemp);
	MyDrawThemeParentBackground(hwnd,hdcTemp,0);

	if(bPushed)
		topStartButton = hStartButton * 2;
	else
	if(bCursorOnStartButton)
		topStartButton = hStartButton * 1;
	else
		topStartButton = hStartButton * 0;

	MyTransparentBlt(hdcTemp,0,0,wImage,rc.bottom,
					hdcStartButton,0,topStartButton,wStartButton,hStartButton,0x00ff00ff);

	if(bUseAlphaBlend){
		ZeroMemory(&bf,sizeof(bf));

		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = 0xff;

		MyAlphaBlend(hdcTemp,0,(rc.bottom - hImage)/2,wImage,hImage,
						hdcImage,0,0,wImage,hImage,bf);
	}else{
		MyTransparentBlt(hdcTemp,0,(rc.bottom - hImage)/2,wImage,hImage,
						hdcImage,0,0,wImage,hImage,0x00ff00ff);
	}

	BitBlt(hdc, 0, 0,wImage,rc.bottom, hdcTemp, 0, 0, SRCCOPY);

	RestoreDC(hdcTemp,-1);
	RestoreDC(hdcImage,-1);
	RestoreDC(hdcStartButton,-1);
	DeleteDC(hdcTemp);
	DeleteDC(hdcImage);
	DeleteDC(hdcStartButton);
	DeleteObject(hbmpTemp);
}
#endif //USE_ANALOG_CLOCK	// add by 505

void OnPaint9x(HWND hwnd, HDC hdc)
{
	HDC hdcMem1, hdcMem2;
	HBITMAP hbmp, hbmpTemp;
	HBRUSH hbr;
	BITMAP bmp;
	RECT rc;
	int x, y, w, h;
	BOOL bPushed;

	bPushed = (SendMessage(hwnd, BM_GETSTATE, 0, 0) & BST_PUSHED)?1:0;

	hdcMem1 = CreateCompatibleDC(hdc);

#ifdef USE_FORMAT_FOR_STARTBTN	// add by 505
	// キャプションの取得
	if (hbmpstart != NULL) {
		char caption[1024];
		hbmp = hbmpstart;
		SelectObject(hdcMem1, hbmp);
		GetObject(hbmp, sizeof(BITMAP), (LPVOID)&bmp);
		w = bmp.bmWidth; h = bmp.bmHeight;
		if (bUseFormatForStartBtn == FALSE) {
			strcpy(caption, szStartBtnText);
		} else {
			GetFormatString(caption, szStartBtnText);
		}
		DrawStartBtnText9x(hdcMem1, caption, w, h, nStartBtnTextSx);
	} else {
		hbmp = (HBITMAP)SendMessage(hwnd, BM_GETIMAGE, IMAGE_BITMAP, 0);
		SelectObject(hdcMem1, hbmp);
		GetObject(hbmp, sizeof(BITMAP), (LPVOID)&bmp);
		w = bmp.bmWidth; h = bmp.bmHeight;
	}
#else	// USE_FORMAT_FOR_STARTBTN // add by 505
	if (hbmpstart != NULL) {
		hbmp = hbmpstart;
	} else {
		hbmp = (HBITMAP)SendMessage(hwnd, BM_GETIMAGE, IMAGE_BITMAP, 0);
	}
	SelectObject(hdcMem1, hbmp);
	GetObject(hbmp, sizeof(BITMAP), (LPVOID)&bmp);
	w = bmp.bmWidth; h = bmp.bmHeight;
#endif	// USE_FORMAT_FOR_STARTBTN // add by 505

	hdcMem2 = CreateCompatibleDC(hdc);
	GetClientRect(hwnd, &rc);
	hbmpTemp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);

	SelectObject(hdcMem2, hbmpTemp);

	hbr = CreateSolidBrush(CoolStartColorFace);
	FillRect(hdcMem2, &rc, hbr);
	DeleteObject(hbr);

#ifdef USE_ANALOG_CLOCK	// add by 505
	if (bCustStartButton && (bAnalogClockAtStartBtn != FALSE)) {
		SYSTEMTIME t;
		int x2, y2, w2;
		if (nAnalogClockPos == ANALOG_CLOCK_POS_LEFT) {
			x = (rc.right - w - sizeAClock.cx - 1)  / 2 + (!bCustStartButton ? 2:0) + (int)bPushed;
			y = (rc.bottom - h) / 2 + (int)bPushed;
			x2 = x;
			x += sizeAClock.cx + 1;
			w2 = w + sizeAClock.cx;
			y2 = (rc.bottom - sizeAClock.cy) / 2 + (int)bPushed;
		} else if (nAnalogClockPos == ANALOG_CLOCK_POS_RIGHT) {
			x = (rc.right - w - sizeAClock.cx - 1)  / 2 + (!bCustStartButton ? 2:0) + (int)bPushed;
			y = (rc.bottom - h) / 2 + (int)bPushed;
			x2 = x + 1;
			w2 = w + sizeAClock.cx + 1;
			y2 = (rc.bottom - sizeAClock.cy) / 2 + (int)bPushed;
		} else {
			x = (rc.right - w)  / 2 + (!bCustStartButton ? 2:0) + (int)bPushed;
			y = (rc.bottom - h) / 2 + (int)bPushed;
			x2 = x;
			w2 = w;
			y2 = (rc.bottom - sizeAClock.cy) / 2 + (int)bPushed;;
		}
		BitBlt(hdcMem2, x, y, w, h, hdcMem1, 0, 0, SRCCOPY);
		GetLocalTime(&t);
		DrawAnalogClock(hdcMem2, &t, x2, y2, w2, h);
	} else {
		x = (rc.right - w)/2 + (!bCustStartButton ? 2:0) + (int)bPushed;
		y = (rc.bottom - h)/2 + (int)bPushed;
		BitBlt(hdcMem2, x, y, w, h, hdcMem1, 0, 0, SRCCOPY);
	}
#else //USE_ANALOG_CLOCK	// add by 505
	x = (rc.right - w)/2 + (!bCustStartButton ? 2:0) + (int)bPushed;
	y = (rc.bottom - h)/2 + (int)bPushed;
	BitBlt(hdcMem2, x, y, w, h, hdcMem1, 0, 0, SRCCOPY);
#endif //USE_ANALOG_CLOCK	// add by 505

	if(bPushed || bCursorOnStartButton || bNormalStartButton) // draw frame
	{
		HPEN hpen, hpenold;
		int color;

		color = (bPushed?CoolStartColorLow:CoolStartColorHigh);
		hpen = CreatePen(PS_SOLID, 1, color);
		hpenold = SelectObject(hdcMem2, hpen);
		MoveToEx(hdcMem2, 0, 0, NULL);
		LineTo(hdcMem2, rc.right, 0);
		MoveToEx(hdcMem2, 0, 0, NULL);
		LineTo(hdcMem2, 0, rc.bottom);
		SelectObject(hdcMem2, hpenold);
		DeleteObject(hpen);

		color = (bPushed?CoolStartColorHigh:CoolStartColorLow);
		hpen = CreatePen(PS_SOLID, 1, color);
		hpenold = SelectObject(hdcMem2, hpen);
		MoveToEx(hdcMem2, rc.right-1, 0, NULL);
		LineTo(hdcMem2, rc.right-1, rc.bottom);
		MoveToEx(hdcMem2, 0, rc.bottom-1, NULL);
		LineTo(hdcMem2, rc.right, rc.bottom-1);
		SelectObject(hdcMem2, hpenold);
		DeleteObject(hpen);

		color = (bPushed?CoolStartColorLow2:CoolStartColorHigh2);
		hpen = CreatePen(PS_SOLID, 1, color);
		hpenold = SelectObject(hdcMem2, hpen);
		MoveToEx(hdcMem2, 1, 1, NULL);
		LineTo(hdcMem2, rc.right - 1, 1);
		MoveToEx(hdcMem2, 1, 1, NULL);
		LineTo(hdcMem2, 1, rc.bottom - 1);
		SelectObject(hdcMem2, hpenold);
		DeleteObject(hpen);

		color = (bPushed?CoolStartColorHigh2:CoolStartColorLow2);
		hpen = CreatePen(PS_SOLID, 1, color);
		hpenold = SelectObject(hdcMem2, hpen);
		MoveToEx(hdcMem2, rc.right-2, 1, NULL);
		LineTo(hdcMem2, rc.right-2, rc.bottom-1);
		MoveToEx(hdcMem2, 1, rc.bottom-2, NULL);
		LineTo(hdcMem2, rc.right-1, rc.bottom-2);
		SelectObject(hdcMem2, hpenold);
		DeleteObject(hpen);
	}

	BitBlt(hdc, 0, 0,
		rc.right, rc.bottom, hdcMem2, 0, 0, SRCCOPY);

	DeleteDC(hdcMem1);
	DeleteDC(hdcMem2);
	DeleteObject(hbmpTemp);
}

/*--------------------------------------------------
   called when clock window receive WM_TIMER.
   check cursor position, and draw "flat start button"
----------------------------------------------------*/
void CheckCursorOnStartButton(void)
{
	POINT pt;
	RECT rc;

	if(hwndStart == NULL) return;
	if(!bStartButtonFlat) return;

	GetCursorPos(&pt);
	GetWindowRect(hwndStart, &rc);
	if(PtInRect(&rc, pt))
	{
		if(!bCursorOnStartButton)
		{
			bCursorOnStartButton = TRUE;
			InvalidateRect(hwndStart, NULL, FALSE);
		}
	}
	else
	{
		if(bCursorOnStartButton)
		{
			bCursorOnStartButton = FALSE;
			InvalidateRect(hwndStart, NULL, FALSE);
		}
	}
}

void SetRebarGrippers(void)
{
	COLORSCHEME colScheme;

	if(!bHiddenGrippers) return;

	colScheme.dwSize = sizeof(colScheme);
	colScheme.clrBtnHighlight = GetSysColor(COLOR_3DFACE);
	colScheme.clrBtnShadow = GetSysColor(COLOR_3DFACE);

	SendMessage(hwndTask, RB_SETCOLORSCHEME, 0, (LPARAM) (LPCOLORSCHEME)&colScheme);
	InvalidateRect(hwndTask, NULL, TRUE);
}
