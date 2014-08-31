#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "HexEditWnd.h"
#include "File.h"
#include "CPathString.h"
#include "OFN.h"
#include "macros.H"

extern HexEditWnd HEdit;

LRESULT  __stdcall HEditMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT  __stdcall HEditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT  __stdcall TBHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall GotoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall OptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall ReplaceDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall SelBlockDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall SearchDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD    __stdcall HEditWindowThread();

#define SB_STATUS		0
#define SB_ORIGIN_SIZE	1
#define SB_NEW_SIZE		2
#define SB_POSITION		3

const int SBbs[] = {
	150,
	80,
	80,
	80
};

const TBBUTTON   TBbs[]      = {
	{  6, TB_WIN2TOP,        TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  20,TB_OPEN,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  2, TB_SAVE,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  1, TB_GOTO,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  19,TB_READONLY,       TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  16,TB_MULTI,          TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  29,TB_OFFSET,         TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  24,TB_SIZE,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, NULL,              TBSTATE_ENABLED, (BYTE)TBSTYLE_SEP,    0, 0},
	{  8, TB_SELBLOCK,       TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 15, TB_SELALL,         TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 27, TB_REPLACE,        TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  7, TB_SEARCH,         TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 12, TB_SEARCHUP,       TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 13, TB_SEARCHDOWN,     TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, NULL,              TBSTATE_ENABLED, (BYTE)TBSTYLE_SEP,    0, 0},
	{ 23, TB_INSERT,         TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 14, TB_UNDO,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 21, TB_REDO,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 30, TB_DELETE,         TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  9, TB_CUT,            TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 10, TB_COPY,      	 TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
//	{ 31, TB_COPY_TEXT,      TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 11, TB_PASTE,          TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, NULL,              TBSTATE_ENABLED, (BYTE)TBSTYLE_SEP,    0, 0},
	{  26, TB_OPTION,        TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  3, TB_ABOUT,          TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, TB_CLOSE,          TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0}
};

WNDPROC            pOrgTBWndProc;
BOOL			   bRightClickMenu;
int				   iFileCDMode;
UINT			   cf16Edit;

void DebugPrint(char *szFormat, ...) {
	char msg[1024];
	va_list args;

	va_start(args, szFormat);
	wvsprintf(msg, szFormat, args);
	va_end(args);
	OutputDebugString(msg);
}

void mymemcpy(void *dest, void *src, int count) {
	int i;
	if (dest < src) {
		for (i = 0; i < count; i++) {
			((BYTE*)dest)[i] = ((BYTE*)src)[i]; 
		}
	} else {
		for (i = 0; i < count; i++) {
			((BYTE*)dest)[count - 1 - i] = ((BYTE*)src)[count - 1 - i];
		}
	}
}

HexEditWnd::HexEditWnd() {
	LOGFONT lf;
	DWORD	fontHeight;
	DWORD	fontQuality;
	char	fontName[256];

	hInst = GetModuleHandle(NULL);

	iyHETop           = 0;
	iyHEBottom        = 0;
	bDispMultiByte    = FALSE;

	timerId			  = 0;
	hMainWnd          = 0;
	bHEOnTop          = 0;
	bFileOffset		  = TRUE;
	bResizingAllowed  = FALSE;
	bMinToTray        = FALSE;
	bSaveWinPos       = FALSE;
	bInsert			  = FALSE;
	uMaxLines         = DEF_MAX_LINES;
	operList		  = NULL;
	current			  = NULL;

	InitEdition();
	ZERO(search);

	GetModuleFileName( hInst, cInitialDir, sizeof(cInitialDir) );
	CPathString::PathToDir( cInitialDir );
	CPathString::ForceEndBackslash( cInitialDir);
	wsprintf(cIniPath, "%s"INI_NAME, cInitialDir);

	fontHeight = GetPrivateProfileInt(INI_SECTION, "fh", 0, cIniPath);
	fontQuality = GetPrivateProfileInt(INI_SECTION, "fq", 0, cIniPath);
	GetPrivateProfileString(INI_SECTION, "fn", "", fontName, sizeof(fontName), cIniPath);

	ZERO(lf);
	lf.lfHeight     = fontHeight ? -fontHeight : DEF_FONT_HEIGHT;
	//lf.lfWidth      = DEF_FONT_WIDTH;
	lf.lfWeight     = FW_LIGHT;
	if(fontName[0])
		lstrcpy(lf.lfFaceName, fontName);
	else
		lstrcpy(lf.lfFaceName, "Courier New");

	if(fontQuality)
		lf.lfQuality     = fontQuality;

	hFont   = CreateFontIndirect(&lf);

	lf.lfUnderline  = TRUE;
	hFontU  = CreateFontIndirect(&lf);

	cf16Edit = RegisterClipboardFormat(CF_16Edit);

	hmTray = CreatePopupMenu();
	AppendMenu(hmTray, MF_STRING, IDT_RESTORE, "&Restore");
	AppendMenu(hmTray, MF_STRING, IDT_EXIT, "E&xit");
}

HexEditWnd::~HexEditWnd() {
	DeleteObject(hFont);
	DeleteObject(hFontU);

	DestroyMenu(hmTray);
}

void HexEditWnd::InitEdition() {
	ZERO(stat);
	ZERO(search);

	stat.bCaretPosValid = TRUE;
	stat.posCaret.bHiword = TRUE;
	stat.iLastLine = -1;

	delete operList;
	operList		  = new EditOperList();
	current			  = operList;
	savepoint		  = operList;
	bSavePointValid	  = TRUE;
	diData.dwSize     = 0;

	return;
}

void HexEditWnd::QuitEdition() {
	free(diData.pDataBuff);
	if (search.bInited) {
		if (search.pData)
			free(search.pData);
		if (search.pDlgStr)
			free(search.pDlgStr);
	}
	return;
}

DWORD __stdcall HEditWindowThread() {
	WNDCLASS               wc;
	MSG                    msg;
	UINT                   icx, icy, ix, iy;
	HWND                   hWnd, hTB;
	RECT                   rct;
	HACCEL                 hAccel;
	HE_WIN_POS             wp;

	InitCommonControls();

	ZERO(wc);
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance      = HEdit.GetInstance();
	wc.hIcon          = LoadIcon(HEdit.GetInstance(), (PSTR)IDI_16Edit);
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName  = HEDIT_WND_CLASS;
	wc.lpfnWndProc    = HEditWndProc;
	RegisterClass(&wc);

	wc.lpszClassName  = HEDIT_CLASS;
	wc.lpfnWndProc    = HEditWndProc;
	RegisterClass(&wc);

	wp.icx = icx = HEDIT_WND_WIDTH;
	wp.icy = icy = HEDIT_WND_HEIGHT;
	wp.ix = ix = (GetSystemMetrics(SM_CXFULLSCREEN) - icx) / 2;
	wp.iy = iy = (GetSystemMetrics(SM_CYFULLSCREEN) - icy) / 2;

	hWnd = CreateWindow(
		   HEDIT_WND_CLASS,
		   HEDIT_WND_TITLE,
		   WS_OVERLAPPEDWINDOW,
		   ix,
		   iy,
		   icx,
		   icy,
		   0,
		   NULL,
		   HEdit.GetInstance(),
		   NULL);

	HEdit.hMainWnd = hWnd;

	hTB = CreateToolbarEx(
		 hWnd,
		 WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		 ID_TB,
		 50, // number of buttons in the bitmap
		 NULL,
		 (DWORD)LoadBitmap(HEdit.GetInstance(), (PSTR)IDB_TOOLBAR),
		 (LPTBBUTTON)&TBbs,
		 ARRAY_ITEMS(TBbs),
		 16,
		 16,
		 16,
		 16,
		 sizeof(TBBUTTON));
	HEdit.SetTBHandle(hTB);
	pOrgTBWndProc = (WNDPROC)SetWindowLong(hTB, GWL_WNDPROC, (DWORD)TBHookProc);

	GetClientRect(hTB, &rct);
	HEdit.iyHETop = rct.bottom - rct.top + 2;

	hAccel = LoadAccelerators(HEdit.GetInstance(), (PSTR)IDR_ACCEL);

	bRightClickMenu = FALSE;
	iFileCDMode = 0;
	GetPrivateProfileStruct(INI_SECTION, INI_SHELL, &bRightClickMenu, sizeof(BOOL), HEdit.cIniPath);
	GetPrivateProfileStruct(INI_SECTION, INI_FILECD, &iFileCDMode, sizeof(int), HEdit.cIniPath);

	GetPrivateProfileStruct(INI_SECTION, INI_WINPOSKEY, &wp, sizeof(wp), HEdit.cIniPath);
	MoveWindow( hWnd, wp.ix, wp.iy, wp.icx, wp.icy, TRUE);
	ShowWindow( hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(hWnd, hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

BOOL HexEditWnd::DoEditFile(char* szFilePath, BOOL bForceReadOnly) {
	BOOL          bRet;

	fInput.Destroy();

	if (bForceReadOnly)
		bRet = fInput.GetFileHandle(szFilePath, F_OPENEXISTING_R);
	else
		bRet = fInput.GetFileHandleWithMaxAccess(szFilePath);
	if (!bRet)
		return FALSE;

	if (!fInput.MapFile())
		return FALSE;

	diOrgData.bReadOnly  = fInput.IsFileReadOnly();
	diOrgData.dwSize     = fInput.GetFSize();
	diOrgData.pDataBuff  = (BYTE*)fInput.GetMapPtr();
	dwOldSize = diOrgData.dwSize;

	diData = diOrgData;
	fInput.SetMapPtrSize(NULL, 0);
	fInput.Destroy();

	if (file_type((char *)diData.pDataBuff)) {
		bFileOffset = FALSE;
	}

	SetHEWndCaption();
	return TRUE;
}

DWORD HexEditWnd::GetFileOffset(DWORD dwVirtualAddress) {
	DWORD	dwOffset;

	if (file_type((char *)diData.pDataBuff)) {
		dwOffset = get_fo((char *)diData.pDataBuff, dwVirtualAddress);
	} else {
		dwOffset = dwVirtualAddress;
	}
	return dwOffset;
}

DWORD HexEditWnd::GetVirtualAddress(DWORD dwFileOffset) {
	DWORD	dwOffset;

	if (file_type((char *)diData.pDataBuff)) {
		dwOffset = get_va((char *)diData.pDataBuff, dwFileOffset);
	} else {
		dwOffset = dwFileOffset;
	}
	return dwOffset;
}

DWORD HexEditWnd::GetOffset(DWORD dwFileOffset) {
	DWORD	dwOffset;

	if (bFileOffset) {
		dwOffset = dwFileOffset;
	} else {
		dwOffset = GetVirtualAddress(dwFileOffset);
	}
	return dwOffset;
}

BOOL HexEditWnd::PaintText(HWND hWnd) {
	HDC           hDC, hdcBuff;
	DWORD         dwOffset;
	UINT          u, u2, icySel;
	char          cBuff[40];
	BYTE          byCur, byNext;
	RECT          rct;
	HBITMAP       hBmp;
	HBRUSH        hbrColor;
	HGDIOBJ       hobjOld;
	PAINTSTRUCT   ps;
	BOOL          bSkipOneByte = FALSE;

	hDC      = BeginPaint(hWnd, &ps);

	// create virtual bmp
	GetClientRect(hWnd, &rct);
	hdcBuff  = CreateCompatibleDC(hDC);
	hBmp     = CreateCompatibleBitmap(hDC,
			rct.right - rct.left,
			rct.bottom - rct.top - iyHETop);
	hobjOld = SelectObject(hdcBuff, hBmp);

	// fill bmp
	hbrColor = CreateSolidBrush( (COLORREF)0x00FFFFFF );
	FillRect(hdcBuff, &rct, hbrColor);
	DeleteObject(hbrColor);

	if (!diData.dwSize)
		goto Exit; // ERR

	// paint to bmp
	dwOffset = stat.dwCurOffset;

	for (u = 0; u < uMaxLines; u++) {
		// end of buffer ?
		if (dwOffset >= diData.dwSize)
			break;

		// paint offset
		SetTextColor(hdcBuff, RGB_BLACK);
		wsprintf(cBuff, H8":", GetOffset(dwOffset));
		SelectObject(hdcBuff, hFont);
		TextOut(hdcBuff, LEFT_OFFSET, u * uFontHeight + TOP_OFFSET, cBuff, 9);

		// paint digit pairs
		for (u2 = 0; u2 < 16; u2++) {
			// end of buffer ?
			if (dwOffset >= diData.dwSize)
				break; // upper, same structured decision handles painting end

			//
			// change at this position?
			//
			byCur = *(BYTE*)((DWORD)diData.pDataBuff + dwOffset);

			// next byte may be multibyte
			if (dwOffset <= diData.dwSize - 1) {
				byNext = *(BYTE*)((DWORD)diData.pDataBuff + dwOffset + 1);
			} else {
				byNext = 0;
			}

			switch (GetDataStatus(dwOffset)) {
				case 0:
					SetTextColor(hdcBuff, RGB_BLACK);
					break;
				case 1:
					SetTextColor(hdcBuff, RGB_RED);
					break;
				case 2:
					SetTextColor(hdcBuff, RGB_BLUE);
					break;
				case 3:
					SetTextColor(hdcBuff, RGB_GREEN);
					break;
			}

			//
			// paint digit pair
			//
			if (stat.bCaretVisible &&
				stat.posCaret.dwOffset == dwOffset &&
				stat.posCaret.bTextSection)
				SelectObject(hdcBuff, hFontU);
			else
				SelectObject(hdcBuff, hFont);

			wsprintf(cBuff, H2, byCur);
			TextOut(hdcBuff,
					PAIRS_X + DIGIT_PAIR_WIDTH * u2, u * uFontHeight + TOP_OFFSET,
					cBuff, 2);

			//
			// paint character
			//
			if (stat.bCaretVisible &&
				stat.posCaret.dwOffset == dwOffset &&
				!stat.posCaret.bTextSection)
				SelectObject(hdcBuff, hFontU);
			else
				SelectObject(hdcBuff, hFont);

			if (!bSkipOneByte) {
				if (IsDBCSLeadByte(byCur) && bDispMultiByte) {
					if (dwOffset == stat.dwCurOffset && !IsDBCSFirstByte(dwOffset)) {
						lstrcpy(cBuff, " ");
					} else {
						if (byNext != 0) {
							bSkipOneByte = TRUE;
						}
						cBuff[0] = byCur;
						cBuff[1] = byNext;
						cBuff[2] = 0;
					}
				} else if (byCur < 0x20 || (byCur & 0x80)) {
					lstrcpy(cBuff, ".");
				} else {
					wsprintf(cBuff, "%c", byCur);
				}

				TextOut(hdcBuff,
						CHARS_X + u2 * uFontWidth, u * uFontHeight + TOP_OFFSET,
						cBuff, strlen(cBuff));
			} else {
				bSkipOneByte = FALSE;
			}

			//
			// draw sel
			//
			if (stat.bSel &&
				dwOffset >= stat.dwOffSelStart &&
				dwOffset <= stat.dwOffSelEnd) {
				if (dwOffset == stat.dwOffSelEnd ||
					dwOffset % 16 == 15)
					icySel = uFontWidth * 2 + 2;
				else
					icySel = DIGIT_PAIR_WIDTH;

				// sel pair
				BitBlt(hdcBuff,
					   PAIRS_X + u2 * DIGIT_PAIR_WIDTH - 2,
					   u * uFontHeight + TOP_OFFSET,
					   icySel,
					   uFontHeight,
					   hdcBuff,
					   PAIRS_X + u2 * DIGIT_PAIR_WIDTH - 2,
					   u * uFontHeight + TOP_OFFSET,
					   NOTSRCCOPY);

				// sel char
				BitBlt(hdcBuff,
					   CHARS_X + u2 * uFontWidth - 2,
					   u * uFontHeight + TOP_OFFSET,
					   uFontWidth,
					   uFontHeight,
					   hdcBuff,
					   CHARS_X + u2 * uFontWidth - 2,
					   u * uFontHeight + TOP_OFFSET,
					   NOTSRCCOPY);
			}

			// adjust vars
			++dwOffset;
		}
	}

	//
	// SB stuff
	//

Exit:
	// paint SB text
	SetTextColor(hdcBuff, RGB_BLACK);
	SelectObject(hdcBuff, hFont);
	SetStatusText();
	MoveToEx(hdcBuff, rct.left + 4, rct.bottom - SB_HEIGHT - 30, NULL);
	LineTo(hdcBuff, CHARS_X + 16 * uFontWidth + 8, rct.bottom - SB_HEIGHT - 30);
	LineTo(hdcBuff, CHARS_X + 16 * uFontWidth + 8, rct.top);
	LineTo(hdcBuff, rct.left + 4, rct.top);
	LineTo(hdcBuff, rct.left + 4, rct.bottom - SB_HEIGHT - 30);
	TextOut(hdcBuff, rct.left + 4, rct.bottom - SB_HEIGHT - 28, cSBText, lstrlen(cSBText) );

	// copy buffer content to client area
	BitBlt(hDC,
		   rct.left,
		   rct.top + iyHETop,
		   rct.right - rct.left,
		   rct.bottom - rct.top - iyHETop,
		   hdcBuff,
		   0, 0, SRCCOPY);

	// cleanup
	SelectObject(hdcBuff, hobjOld);
	DeleteDC(hdcBuff);
	DeleteObject(hBmp);

	EndPaint(hWnd, &ps);

	return TRUE; // OK
}

void HexEditWnd::HEHandleWM_SETFOCUS(HWND hWnd) {
	CreateCaret(hWnd, NULL, uFontWidth, uFontHeight);
	SetCaretSet(TRUE);
	if (stat.bCaretPosValid)
		SetCaret(&stat.posCaret);

	return;
}

void HexEditWnd::HEHandleWM_KILLFOCUS(HWND hWnd) {
	if (!stat.bCaretVisible)
		HideCaret(hWnd);
	DestroyCaret();
	SetCaretSet(FALSE);
	stat.bCaretVisible = FALSE;

	return;
}

//
// save new caret pos and repaints
//
// returns:
// FALSE - mainly if the caret was hidden because it's not in the current visible range
//
BOOL HexEditWnd::SetCaret(PHE_POS ppos)
{
	BOOL     bRet = FALSE;
	DWORD    dwOffDelta;
	UINT     uxPair, uyLine, ux;

	if (IsOutOfRange(ppos))
		return FALSE; // ERR

	SetCaretPosData(ppos);

	if (stat.bSel)
		return FALSE; // ERR

	// new pos in current range?
	if (!IsOffsetVisible(ppos->dwOffset)) {
		if (stat.bCaretVisible)
			HideCaret(hMainWnd);
		stat.bCaretVisible = FALSE;
		return FALSE; // ERR
	}

	dwOffDelta = ppos->dwOffset - stat.dwCurOffset;
	uyLine = dwOffDelta / 16;
	uxPair = dwOffDelta % 16;

	// caret in the text section ?
	if (ppos->bTextSection) {
		bRet = SetCaretPos(
						  CHARS_X + uFontWidth * uxPair,
						  uyLine * uFontHeight + iyHETop + TOP_OFFSET);
	} else {
		ux = PAIRS_X + uxPair * DIGIT_PAIR_WIDTH;
		if (!ppos->bHiword)
			ux += uFontWidth;
		bRet = SetCaretPos(
						  ux,
						  uyLine * uFontHeight + iyHETop + TOP_OFFSET);
	}
	if (bRet) {
		if (!stat.bCaretVisible)
			ShowCaret(hMainWnd);
		stat.bCaretVisible   = TRUE;
		stat.bCaretPosValid  = TRUE;

		RepaintClientArea();

		return TRUE; // OK
	} else
		return FALSE; // ERR
}

//
// overloaded
//
BOOL HexEditWnd::SetCaret(DWORD dwOffset)
{
	HE_POS  posNew;

	if (IsOutOfRange(dwOffset))
		return FALSE; // ERR

	posNew.bHiword      = TRUE;
	posNew.bTextSection = FALSE;
	posNew.dwOffset     = dwOffset;

	return SetCaret(&posNew);
}

BOOL HexEditWnd::SetCaret() {
	return SetCaret(&stat.posCaret);
}

//
// checks whether an Offset is currently visible in the GUI
//
BOOL HexEditWnd::IsOffsetVisible(DWORD dwOffset)
{
	DWORD dwBytes2C;

	// out of mem range ?
	if (dwOffset >= diData.dwSize)
		return FALSE; // ERR

	// out of visible range ?
	dwBytes2C = 16 * uMaxLines;
	if (dwBytes2C + stat.dwCurOffset > diData.dwSize)
		dwBytes2C = diData.dwSize - stat.dwCurOffset;

	if (dwOffset >= stat.dwCurOffset &&
		dwOffset <  stat.dwCurOffset + dwBytes2C)
		return TRUE; // OK
	else
		return FALSE; // ERR
}

BOOL HexEditWnd::HESetFont(HFONT hf) {
	TEXTMETRIC  tm;
	HDC         hDC;
	BOOL        bRet = FALSE;

	hDC = GetDC(hMainWnd);
	if (!hDC)
		return FALSE; // FAILURE
	SelectObject(hDC, hf);
	if (!GetTextMetrics(hDC, &tm))
		goto Exit; // FAILURE

	uFontHeight = tm.tmHeight;
	uFontWidth  = tm.tmAveCharWidth;

	SendMessage(hMainWnd, WM_SETFONT, (WPARAM)HEdit.hFont, 0);

	Exit:
	ReleaseDC(hMainWnd, hDC);

	return bRet; // EXIT
}

void HexEditWnd::HEHandleWM_SIZE(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	RECT              rct;
	UINT              uWidth, uHeight;

	if ( wParam != SIZE_MINIMIZED &&
		 wParam != SIZE_MAXIMIZED)
		GetWindowRect( hWnd, &rctLastPos );

	if (bMinToTray && wParam == SIZE_MINIMIZED) {
		HEditToTray();
		return;
	}

	// wnd size info -> vars
	uWidth = LOWORD(lParam);
	uHeight = HIWORD(lParam); 

	if (uFontHeight) { // avoid division through 0
		// calc max lines
		uMaxLines = (uHeight - iyHETop - SB_HEIGHT) / uFontHeight;

		// bottom of HE
		GetClientRect(hWnd, &rct);

		HEdit.iyHEBottom = rct.bottom - SB_HEIGHT;

		// get HE rect
		rctHE.top     = iyHETop;
		rctHE.bottom  = iyHEBottom;
		rctHE.left    = 0;
		rctHE.right   = CHARS_X + 16 * uFontWidth;
	}

	// resize TB
	SendMessage(hTB, TB_AUTOSIZE, 0, 0);
	return;	// RET
}

BOOL HexEditWnd::HEHandleWM_TIMER(HWND hWnd, WPARAM wTimerId) {
	POINT pos;
	if (wTimerId == SELECT_TIMER) {
		if (IsKeyDown(VK_LBUTTON)) {
			GetCursorPos(&pos);
			ScreenToClient(hWnd, &pos);
			MouseMoveSelect(&pos);
		} else {
			KillTimer(hWnd, SELECT_TIMER);
			timerId = 0;
		}
		return 0;
	}
	return TRUE;
}

BOOL HexEditWnd::HEHandleLButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT   pClick;
	HE_POS  pos;
	static  DWORD   dwOldOff;
	static  BOOL	lastvalid = FALSE;

	pClick.x = LOWORD(lParam);
	pClick.y = HIWORD(lParam);

	switch (uMsg) {
	case WM_LBUTTONDOWN:
		if (!PointToPos(&pClick, &pos)) {
			stat.bLastLBDownPos = FALSE;
			return FALSE; // ERR
		}

		stat.bMouseSelecting = TRUE;
		if (!stat.bSel || lastvalid == FALSE) {
			lastvalid = TRUE;
			dwOldOff = stat.posCaret.dwOffset;
		}
		stat.posLastLButtonDown = pos;
		stat.bLastLBDownPos = TRUE;

		// handle shift key actions ?
		if ( TESTBIT(wParam, MK_SHIFT) && stat.bCaretPosValid )
			SetSelection( dwOldOff, pos.dwOffset );
		else {
			lastvalid = FALSE;
			if (stat.bSel) {
				KillSelection();
			}
		}
		SetCaret(&pos);
		break;

	case WM_LBUTTONUP:
		KillTimer(hMainWnd, SELECT_TIMER);
		timerId = 0;
		stat.bMouseSelecting = FALSE;
		break;
	}

	return TRUE;
}

BOOL HexEditWnd::PointToPos(IN POINT *pp, OUT PHE_POS ppos) {
	UINT  uLine, uPair, uxCurPair;

	memset(ppos, 0, sizeof(HE_POS));

	// in digit pair field ?
	if ((DWORD)pp->x >= PAIRS_X &&
		(DWORD)pp->x <  PAIRS_X + 16 * DIGIT_PAIR_WIDTH &&
		(DWORD)pp->y >= iyHETop &&
		(DWORD)pp->y <  iyHETop + uMaxLines * uFontHeight) {
		uLine =  ((DWORD)pp->y - iyHETop) / uFontHeight;
		uPair =  pp->x - PAIRS_X;
		uPair /= DIGIT_PAIR_WIDTH;

		// x in space between digit pairs ?
		uxCurPair = PAIRS_X + uPair * DIGIT_PAIR_WIDTH;	// -> x pos of cur pair
		if ((UINT)pp->x > uxCurPair + 2*uFontWidth) {
			// last pair of the line ?
			if (uPair == 0xF)
				return FALSE; // ERR

			// autosel next pair
			if (pp->x - uxCurPair - 2*uFontWidth > uFontWidth / 2) {
				++uPair;
				uxCurPair += DIGIT_PAIR_WIDTH;
			}
		}

		// out of range?
		if (IsOutOfRange(stat.dwCurOffset + uLine * 16 + uPair))
			return FALSE; // ERR		

		// x -> LOWORD ?
		ppos->bHiword = ((UINT)pp->x > uxCurPair + uFontWidth) ? FALSE: TRUE;

		// save offset
		ppos->dwOffset = stat.dwCurOffset + uLine * 16 + uPair;

		return TRUE; // OK
	}
	// in text field ?
	else if ((DWORD)pp->x >= CHARS_X &&
			 (DWORD)pp->x <  CHARS_X + uFontWidth * 16 &&
			 (DWORD)pp->y >= iyHETop &&
			 (DWORD)pp->y <  iyHETop + uMaxLines * uFontHeight) {
		uLine =  ((DWORD)pp->y - iyHETop) / uFontHeight;
		uPair =  (UINT)pp->x - CHARS_X;
		uPair /= uFontWidth;

		// out of range?
		if (IsOutOfRange(stat.dwCurOffset + uLine * 16 + uPair))
			return FALSE; // ERR

		// build output
		ppos->bTextSection = TRUE;
		ppos->bHiword      = TRUE;
		ppos->dwOffset     = stat.dwCurOffset + uLine * 16 + uPair;

		return TRUE; // OK
	} else
		return FALSE; // ERR
}

BOOL HexEditWnd::HEHandleWM_KEYDOWN(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HE_POS  posNew;
	int     iNewLine;
	DWORD   dwOff;

	if (wParam == VK_ESCAPE) {
		if (stat.bSel) {
			KillSelection();
			RepaintClientAreaNow();
		} else {
			SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		return TRUE;
	}

	// caret already set
	if (!stat.bCaretPosValid)
		return FALSE; // ERR

	// handle special keys

	// tab key
	switch (wParam) {
	case VK_TAB:
		// switch between text/data fields
		if (stat.bCaret) {
			// kill sel
			if (stat.bSel)
				KillSelection();

			// reset caret
			stat.posCaret.bTextSection ^= TRUE;
			stat.posCaret.bHiword       = TRUE;
			SetCaret();

			// make visible if is not
			if (!IsOffsetVisible(stat.posCaret.dwOffset))
				SetTopLine();

			return TRUE; // OK
		} else
			return FALSE; // ERR
	}

	// shift key
	if ( IsKeyDown(VK_SHIFT) ) {
		posNew = stat.posCaret;
		switch (wParam) {
		case VK_LEFT:	 // left
			posNew.dwOffset--;
			break;

		case VK_RIGHT:	 // right
			posNew.dwOffset++;
			break;

		case VK_NEXT:	 // page down
			posNew.dwOffset += uMaxLines * 16;
			break;

		case VK_PRIOR:	 // page up
			posNew.dwOffset -= uMaxLines * 16;
			break;

		case VK_DOWN:	 // down
			posNew.dwOffset += 16;
			break;

		case VK_UP:		 // up
			posNew.dwOffset -= 16;
			break;

		default:
			return FALSE; // ERR
		}

		// validate
		if (ValidatePos(&posNew))
			Beep();

		if (stat.bSel) {
			if (stat.dwOffSelStart == stat.posCaret.dwOffset)
				dwOff = stat.dwOffSelEnd;
			else
				dwOff = stat.dwOffSelStart;

			SetSelection( dwOff, posNew.dwOffset);
		} else
			SetSelection( stat.posCaret.dwOffset, posNew.dwOffset );

		SetCaret(&posNew);
		MakeCaretVisible();

		return TRUE; // OK
	}

	//
	// move caret in HE area / move current line
	//
	posNew    = stat.posCaret;
	iNewLine  = GetCurrentLine();
	switch (wParam) {
	case VK_NEXT:	 // page down
		posNew.dwOffset += uMaxLines * 16;
		iNewLine += uMaxLines;
		break;

	case VK_PRIOR:	 // page up
		posNew.dwOffset -= uMaxLines * 16;
		iNewLine -= uMaxLines;
		break;

	case VK_DOWN:	 // down
		posNew.dwOffset += 16;
		++iNewLine;
		break;

	case VK_UP:		 // up
		posNew.dwOffset -= 16;
		--iNewLine;
		break;

	case VK_RIGHT:	 // rigth
		if (stat.posCaret.bTextSection) {
			++posNew.dwOffset;
			posNew.bHiword = TRUE;
		} else {
			posNew.bHiword ^= 1;    
			if (!stat.posCaret.bHiword)
				++posNew.dwOffset;
		}
		break;

	case VK_LEFT:	 // left
		if (stat.posCaret.bTextSection) {
			--posNew.dwOffset;
			posNew.bHiword = TRUE;
		} else {
			posNew.bHiword ^= 1;
			if (stat.posCaret.bHiword)
				--posNew.dwOffset;
		}
		break;

	case VK_BACK:
		if (stat.posCaret.bTextSection)
			--posNew.dwOffset;
		break;

	default:
		return FALSE; // ERR
	}

	// changes -> GUI
	if (stat.bSel &&
		wParam != VK_RIGHT && 
		wParam != VK_LEFT) {
		// validate
		if (ValidateLine(&iNewLine))
			Beep();

		// set
		SetCurrentLine(iNewLine);
	} else {
		// validate
		if (ValidatePos(&posNew))
			Beep();

		// set
		KillSelection();
		SetCaret(&posNew);
		MakeCaretVisible();
	}

	return TRUE; // OK
}

//
// if caret isn't in the visible area, the top line is reset
//
// returns:
// whether it was needed to reset the top line
//
BOOL HexEditWnd::MakeCaretVisible() {
	DWORD dwLastVisibleOff;

	if (IsOffsetVisible(stat.posCaret.dwOffset))
		return FALSE; // ERR

	dwLastVisibleOff = __min(uMaxLines * 16 + stat.dwCurOffset,
							 diData.dwSize);
	if (stat.posCaret.dwOffset < stat.dwCurOffset) // caret above ?
		SetTopLine((int)(stat.posCaret.dwOffset / 16));
	else // caret below
		SetTopLine((int)(stat.posCaret.dwOffset / 16 - uMaxLines + 1));

	return TRUE; // OK
}

//
// corrects the information in a given HE_POS structure if it's out of range
//
// returns:
// whether sth was fixed
//
BOOL HexEditWnd::ValidatePos(PHE_POS ppos) {
	if ((int)ppos->dwOffset < 0) {
		ppos->dwOffset = 0;
		ppos->bHiword  = TRUE;
		return TRUE; // OK
	} else if (ppos->dwOffset >= diData.dwSize) {
		ppos->dwOffset = diData.dwSize - 1;
		ppos->bHiword  = FALSE;
		return TRUE; // OK
	} else
		return FALSE; // ERR
}

//
// returns:
// last caret status
//
BOOL HexEditWnd::SetCaretSet(BOOL bSet) {
	BOOL bRet;

	bRet = stat.bCaret;
	stat.bCaret = bSet;

	return bRet;
}

void HexEditWnd::SetupVScrollbar() {
	DWORD dwTotalLines = GetTotalLineNum();

	SetScrollRange(hMainWnd, SB_VERT, 0, (int)(dwTotalLines - 1), TRUE);
	return;
}

UINT HexEditWnd::GetTotalLineNum() {
	DWORD dwTotalLines;

	dwTotalLines = diData.dwSize / 16;
	if (diData.dwSize % 16)
		++dwTotalLines;

	return dwTotalLines;
}

BOOL HexEditWnd::HEHandleWM_VSCROLL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int          nPos;
	SCROLLINFO   scrollinfo = { sizeof(SCROLLINFO)};

	nPos = GetScrollPos(hWnd, SB_VERT);

	switch (LOWORD(wParam)) {
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		// get the tracking position
		scrollinfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(hWnd, SB_VERT, &scrollinfo);
		nPos = scrollinfo.nTrackPos;
		break;

	case SB_TOP:
		nPos = 0;
		break;

	case SB_BOTTOM:
		nPos = GetTotalLineNum() - 1;
		break;

	case SB_LINEDOWN:
		++nPos;
		break;

	case SB_LINEUP:
		--nPos;
		break;

	case SB_PAGEDOWN:
		nPos += uMaxLines;      
		break;

	case SB_PAGEUP:
		nPos -= uMaxLines;
		break;      
	}

	return SetTopLine((int)nPos);
}

//
// sets the line to the top of the visible data range
//
BOOL HexEditWnd::SetTopLine(int iNewLine) {
	int nNewPos;

	nNewPos = iNewLine;

	// validation
	if (nNewPos < 0)
		nNewPos = 0;
	nNewPos = __min(nNewPos, (int)GetTotalLineNum() - 1);

	// avoid repainting ?
	if (nNewPos == stat.iLastLine)
		return TRUE; // OK

	// set new line
	SetScrollPos(hMainWnd, SB_VERT, nNewPos, TRUE);
	stat.iLastLine = nNewPos;

	// set new offset
	stat.dwCurOffset = nNewPos * 16;

	// reset caret
	if (stat.bCaretPosValid)
		SetCaret();

	// repaint
	RepaintClientArea();

	return TRUE; // OK
}

BOOL HexEditWnd::SetTopLine(DWORD dwOffset) {
	return SetTopLine( (int)dwOffset / 16);
}

BOOL HexEditWnd::SetTopLine() {
	return SetTopLine((DWORD)stat.posCaret.dwOffset);
}

void HexEditWnd::RepaintClientArea() {
	InvalidateRect(hMainWnd, NULL, FALSE);
	return;
}

void HexEditWnd::RepaintClientAreaNow() {
	RepaintClientArea();
	UpdateWindow(hMainWnd);

	return;
}

void HexEditWnd::HEHandleWM_MOUSEWHEEL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int         nPos;
	short int   iDelta;

	iDelta = (int)HIWORD(wParam);
	nPos = GetScrollPos(hWnd, SB_VERT);
	if (iDelta > 0) { // move up	
		nPos -= WHEEL_MOVE_STEPS;
	} else { // move down
		nPos += WHEEL_MOVE_STEPS;
	}

	SetTopLine((int)nPos);

	return;
}

void HexEditWnd::HEHandleWM_SHOWWINDOW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HEdit.HESetFont(HEdit.hFont);
	SetupVScrollbar();
	SetTopLine(stat.dwCurOffset);
	ConfigureTB();
	SetHEWndCaption();

	return;
}

void HexEditWnd::Beep() {
	MessageBeep(MB_ICONEXCLAMATION);
	return;
}

BOOL HexEditWnd::HEHandleWM_COMMAND(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
	case TB_GOTO:
		if (DialogBoxParam( GetInstance(), (PSTR)IDD_GOTO, hWnd, GotoDlgProc, 0) &&
			stat.bCaretPosValid) {
			SetTopLine();
			SetCaret(&stat.posCaret);
		}
		return TRUE;

	case TB_OPTION:
		DialogBoxParam( GetInstance(), (PSTR)IDD_OPTION, hWnd, OptionDlgProc, 0);
		return TRUE;

	case TB_SELBLOCK:
		DialogBoxParam( GetInstance(), (PSTR)IDD_SELBLOCK, hWnd, SelBlockDlgProc, 0);
		return TRUE;

	case TB_SELALL:
		SelectAll();
		return TRUE;

	case TB_REPLACE:
		DialogBoxParam( GetInstance(), (PSTR)IDD_REPLACE, hWnd, ReplaceDlgProc, 0);
		return TRUE;

	case TB_SEARCH:
		DialogBoxParam( GetInstance(), (PSTR)IDD_SEARCH, hWnd, SearchDlgProc, 0);
		return TRUE;

	case TB_SEARCHDOWN:
	case TB_SEARCHUP:
		PerformSearchAgain(&search, LOWORD(wParam) == TB_SEARCHDOWN ? TRUE : FALSE);
		return TRUE;

	case TB_WIN2TOP:
		SetHEWnd2Top( bHEOnTop ^ 1 );
		ConfigureTB();
		return TRUE;

	case TB_SAVE:
		SaveChanges();
		SetHEWndCaption();
		ConfigureTB();
		RepaintClientAreaNow();
		return TRUE;

	case TB_OFFSET:
		bFileOffset = !bFileOffset;
		ConfigureTB();
		RepaintClientAreaNow();
		return TRUE;

	case TB_INSERT:
		bInsert = !bInsert;
		ConfigureTB();
		return TRUE;

	case TB_UNDO:
		UndoChanges();
		SetHEWndCaption();
		return TRUE;

	case TB_REDO:
		RedoChanges();
		SetHEWndCaption();
		return TRUE;

	case TB_DELETE:
		DeleteSelectedBlock();
		SetHEWndCaption();
		return TRUE;

	case TB_CUT:
		CutSelectedBlock();
		SetHEWndCaption();
		return TRUE;

	case TB_COPY:
		CopySelectedBlock();
		return TRUE;

	case TB_COPY_TEXT:
		CopySelectedBlockAsText();
		return TRUE;

	case TB_PASTE:
		PasteBlockFromCB();
		SetHEWndCaption();
		return TRUE;

	case TB_ABOUT:
		ShowAbout();
		return TRUE;

	case TB_CLOSE:
		HEditQuit();
		return TRUE;

	case IDT_RESTORE:
		HEditReturnFromTray();
		return TRUE;

	case IDT_EXIT:
		HEditKillTrayIcon();
		HEditQuit();
		return TRUE;

	case TB_MULTI:
		bDispMultiByte = !bDispMultiByte;
		ConfigureTB();
		RepaintClientAreaNow();
		return TRUE;

	case TB_READONLY:
		if (!diOrgData.bReadOnly) {
			bReadOnly = !bReadOnly;
			ConfigureTB();
			SetHEWndCaption();
		}
		return TRUE;
	case TB_SIZE:
		bResizingAllowed = !bResizingAllowed;
		ConfigureTB();
		return TRUE;
	case TB_OPEN:
		OFN          ofn;
		char         *szCmdl;
		int          iRet;

		if (CanSave()) {
			iRet = MessageBox(hMainWnd, "File changed, save or not", "16Edit", MB_YESNOCANCEL);
			if (iRet == IDYES) {
				SaveChanges();
			} else if (iRet == IDCANCEL) {
				return TRUE;
			}
		}

		if (ofn.GetOpenFilePath()) {
			szCmdl = ofn.cPathOpen;

			QuitEdition();
			if (!HEdit.DoSpecifySettings(szCmdl)) {
				MessageBox(hMainWnd, "File access error!", "16Edit", MB_ICONERROR);
				return TRUE;
			}
			SetCaret();
			SetHEWndCaption();
			SetupVScrollbar();
			SetTopLine();
			ConfigureTB();
			RepaintClientAreaNow();
		}
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL HexEditWnd::HEHandleWM_NOTIFY(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (((LPNMHDR)lParam)->code == TTN_NEEDTEXT) {
		switch (((LPNMHDR) lParam)->idFrom) {
		case TB_SAVE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "save changes  ( CTRL+S )";
			return TRUE;

		case TB_UNDO:
			((LPTOOLTIPTEXT)lParam)->lpszText = "undo last changes  ( CTRL+Z )";
			return TRUE;

		case TB_REDO:
			((LPTOOLTIPTEXT)lParam)->lpszText = "undo last changes  ( CTRL+Y )";
			return TRUE;

		case TB_GOTO:
			((LPTOOLTIPTEXT)lParam)->lpszText = "goto offset  ( CTRL+G )";
			return TRUE;

		case TB_SELBLOCK:
			((LPTOOLTIPTEXT)lParam)->lpszText = "select block  ( CTRL+B )";
			return TRUE;

		case TB_SELALL:
			if ( IsAllSelected() )
				((LPTOOLTIPTEXT)lParam)->lpszText = "deselect all  ( CTRL+A )";
			else
				((LPTOOLTIPTEXT)lParam)->lpszText = "select all  ( CTRL+A )";
			return TRUE;

		case TB_REPLACE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "replace  ( CTRL+R )";
			return TRUE;

		case TB_SEARCH:
			((LPTOOLTIPTEXT)lParam)->lpszText = "search  ( CTRL+F )";
			return TRUE;

		case TB_SEARCHDOWN:
			((LPTOOLTIPTEXT)lParam)->lpszText = "search down again  ( F3 )";
			return TRUE;

		case TB_SEARCHUP:
			((LPTOOLTIPTEXT)lParam)->lpszText = "search up again  ( CTRL+F3 )";
			return TRUE;

		case TB_DELETE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "delete selected block( DELETE )";
			return TRUE;

		case TB_CUT:
			((LPTOOLTIPTEXT)lParam)->lpszText = "cut into clipboard  ( CTRL+X )";
			return TRUE;

		case TB_COPY:
			((LPTOOLTIPTEXT)lParam)->lpszText = "copy to clipboard  ( CTRL+C )";
			return TRUE;

		case TB_PASTE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "paste from clipboard  ( CTRL+V )";
			return TRUE;

		case TB_WIN2TOP:
			((LPTOOLTIPTEXT)lParam)->lpszText =
			bHEOnTop ? "set window state to non-top" : "set window state to top  ( CTRL+T )";
			return TRUE;

		case TB_ABOUT:
			((LPTOOLTIPTEXT)lParam)->lpszText = "about  ( F12 )";
			return TRUE;

		case TB_MULTI:
			((LPTOOLTIPTEXT)lParam)->lpszText = "ansi or native language  ( CTRL+D )";
			return TRUE;

		case TB_READONLY:
			((LPTOOLTIPTEXT)lParam)->lpszText = "readonly or readwrite ( CTRL+W )";
			return TRUE;

		case TB_INSERT:
			((LPTOOLTIPTEXT)lParam)->lpszText = "insert or overwrite ( CTRL+I )";
			return TRUE;

		case TB_SIZE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "size change allowed or not ( CTRL+L )";
			return TRUE;

		case TB_OFFSET:
			((LPTOOLTIPTEXT)lParam)->lpszText = "file offset or virtual address ( CTRL+E )";
			return TRUE;

		case TB_OPTION:
			((LPTOOLTIPTEXT)lParam)->lpszText = "16edit options ( CTRL+O )";
			return TRUE;

		case TB_CLOSE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "close this window  ( ESC )";
			return TRUE;
		}
	}

	return FALSE;
}

void HexEditWnd::ErrMsg(HWND hWnd, char* szText) {
	MessageBox(hWnd, szText, "ERROR", MB_ICONERROR);
	return;
}

void HexEditWnd::ErrMsg(char* szText) {
	MessageBox(hMainWnd, szText, "ERROR", MB_ICONERROR);
	return;
}

void HexEditWnd::ErrMsg(HWND hWnd, char* szText, char* szCaption) {
	MessageBox(hWnd, szText, szCaption, MB_ICONERROR);
	return;
}

BOOL HexEditWnd::IsOutOfRange(DWORD dwOffset) {
	return(dwOffset >= diData.dwSize) ? TRUE : FALSE;
}

BOOL HexEditWnd::IsOutOfRange(PHE_POS ppos) {
	return(ppos->dwOffset >= diData.dwSize) ? TRUE : FALSE;
}

void HexEditWnd::SetCaretPosData(PHE_POS ppos) {
	memcpy( &stat.posCaret, ppos, sizeof(HE_POS));
	stat.bCaretPosValid = TRUE;

	return;
}

void HexEditWnd::SetCaretPosData(DWORD dwOffset) {
	HE_POS  posNew;

	posNew.bHiword       = TRUE;
	posNew.bTextSection  = FALSE;
	posNew.dwOffset      = dwOffset;
	SetCaretPosData(&posNew);

	return;
}

//
// returns:
// 0 - if the message was processed
//
LRESULT HexEditWnd::HEHandleWM_CHAR(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	BYTE       byNew, byMask, byOld;
	BOOL	   hiWord;

	// caret set ?
	if (!stat.bCaretVisible ||
		!stat.bCaretPosValid)
		return 1; // ERR

	// skip key combinations (CTRL + Z, ...)
	if (wParam < 0x20)
		return 1;

	if (IsReadOnly()) {
		Beep();
		return 1;
	}

	hiWord = stat.posCaret.bHiword;

	// process HE change
	byOld = *(BYTE*)((DWORD)diData.pDataBuff + stat.posCaret.dwOffset);
	if (stat.posCaret.bTextSection) {
		byNew = (BYTE)wParam;
	} else {
		// build new byte
		if (wParam >= '0' && wParam <= '9')
			byMask = wParam - 0x30;
		else if (wParam >= 'a' && wParam <= 'f')
			byMask = wParam - 87;
		else if (wParam >= 'A' && wParam <= 'F')
			byMask = wParam - 55;
		else
			return 1; // ERR

		if (hiWord)
			byNew = (byOld & 0x0F) + byMask * 16;
		else
			byNew = (byOld & 0xF0) + byMask;
	}

	if (byOld != byNew) {
		HE_OPER *op;

		op = current->getOper();
		if (current->getNext() == NULL &&
			op != NULL &&
			op->type == op_modify &&
			(!bSavePointValid || current != savepoint) &&
			op->dwOffset == stat.posCaret.dwOffset) {
			op = current->getOper();
			op->newData[0] 	= byNew;
		} else {
			op = new HE_OPER(op_modify, stat.posCaret.dwOffset, 1, 1);
			op->newData[0] 	= byNew;
			op->oldData[0] 	= byOld;
			AddOper(op);
		}

		ApplyOper(op);
	}

	// reset caret
	if (stat.posCaret.bTextSection) {
		++stat.posCaret.dwOffset;
	} else {
		if (!hiWord) {
			++stat.posCaret.dwOffset;
			stat.posCaret.bHiword = TRUE;
		} else {
			stat.posCaret.bHiword = FALSE;
		}
	}

	SetHEWndCaption();
	ConfigureTB();
	ValidatePos(&stat.posCaret);
	SetCaret();

	return 0; // OK
}

#define STATUS (bEnabled ? TBSTATE_ENABLED:TBSTATE_INDETERMINATE)
void HexEditWnd::ConfigureTBCCP() {
	BOOL bEnabled;

	// cut/delete button
	bEnabled = CanCut();
	SendMessage(hTB, TB_SETSTATE, TB_CUT, STATUS);
	SendMessage(hTB, TB_SETSTATE, TB_DELETE, STATUS);

	// copy button
	bEnabled = CanCopy();
	SendMessage(hTB, TB_SETSTATE, TB_COPY, STATUS);
	SendMessage(hTB, TB_SETSTATE, TB_COPY_TEXT, STATUS);

	// paste button
	bEnabled = CanPaste();
	SendMessage(hTB, TB_SETSTATE, TB_PASTE, STATUS);
	return;
}

void HexEditWnd::ConfigureTB() {
	BOOL bEnabled;

	//Cut copy paste
	ConfigureTBCCP();

	// save button
	bEnabled = CanSave();
	SendMessage(hTB, TB_SETSTATE, TB_SAVE, STATUS);

	// insert button
	bEnabled = bInsert;
	SendMessage(hTB, TB_CHANGEBITMAP, TB_INSERT, bEnabled ? 22 : 23);

	// offset type button
	if (!file_type((char *)diData.pDataBuff)) {
		SendMessage(hTB, TB_CHANGEBITMAP, TB_OFFSET, 29);
		SendMessage(hTB, TB_SETSTATE, TB_OFFSET, FALSE);
	} else {
		bEnabled = bFileOffset;
		SendMessage(hTB, TB_CHANGEBITMAP, TB_OFFSET, bEnabled ? 29 : 28);
	}

	// undo button
	bEnabled = CanUndo();
	SendMessage(hTB, TB_SETSTATE, TB_UNDO, STATUS);

	// redo button
	bEnabled = CanRedo();
	SendMessage(hTB, TB_SETSTATE, TB_REDO, STATUS);

	// top button
	SendMessage(hTB, TB_CHANGEBITMAP, TB_WIN2TOP, bHEOnTop ? 5 : 6);

	// search buttons
	bEnabled = search.bInited;
	SendMessage(hTB, TB_SETSTATE, TB_SEARCHDOWN, STATUS);
	SendMessage(hTB, TB_SETSTATE, TB_SEARCHUP, STATUS);

	// cut button
	bEnabled = CanCut();
	SendMessage(hTB, TB_SETSTATE, TB_CUT, STATUS);

	// copy button
	bEnabled = CanCopy();
	SendMessage(hTB, TB_SETSTATE, TB_COPY, STATUS);

	// paste button
	bEnabled = CanPaste();
	SendMessage(hTB, TB_SETSTATE, TB_PASTE, STATUS);

	// Display mulitibyte button
	bEnabled = bDispMultiByte;
	SendMessage(hTB, TB_CHANGEBITMAP, TB_MULTI, bEnabled ? 16 : 17);

	// Display Readonly button
	bEnabled = IsReadOnly();
	SendMessage(hTB, TB_CHANGEBITMAP, TB_READONLY, bEnabled ? 18 : 19);
	SendMessage(hTB, TB_SETSTATE, TB_READONLY, 
				diOrgData.bReadOnly ? TBSTATE_INDETERMINATE : TBSTATE_ENABLED );

	bEnabled = IsResizingAllowed();
	SendMessage(hTB, TB_CHANGEBITMAP, TB_SIZE, bEnabled ? 25 : 24);

	return;
}

BOOL HexEditWnd::CanCut() {
	return stat.bSel && !IsReadOnly() && IsResizingAllowed();
}

BOOL HexEditWnd::CanCopy() {
	return stat.bSel;
}

BOOL HexEditWnd::CanPaste() {
	BOOL	bRet;
	PHE_CLIPBOARD_DATA   pcbd;

	if (IsReadOnly() || !IsClipboardFormatOK()) {
		return FALSE;
	}

	if (!IsResizingAllowed()) {
		OpenClipboard(hMainWnd);
		pcbd = GetClipboardData();
		if (!pcbd) {
			CloseClipboard();
			bRet = FALSE;
		}
		CloseClipboard();
		if (stat.bSel) {
			if (stat.dwOffSelEnd - stat.dwOffSelStart + 1 != pcbd->dwDataSize) {
				bRet = FALSE;
			} else {
				bRet = TRUE;
			}
		} else {
			if (bInsert) {
				bRet = FALSE;
			} else {
				bRet = TRUE;
			}
		}
	}

	if (!pcbd) {
		free(pcbd);
	}

	return bRet;
}

BOOL HexEditWnd::SaveChanges() {
	BOOL       bRet = FALSE;

	if (!CanSave()) {
		return FALSE;
	}

	SetStatusInfo("Saving...");
	RepaintClientAreaNow();

	if (fInput.OpenFileForSave()) {
		fInput.SetMapPtrSize(diData.pDataBuff, diData.dwSize);
		fInput.FlushFileMap();
		fInput.SetMapPtrSize(NULL, 0);
		fInput.Destroy();
		savepoint = current;
		bSavePointValid = TRUE;
		SetStatusInfo("Save OK!");

		return TRUE;
	} else {
		MessageBox(hMainWnd, "Unable open file for save", "16Edit", MB_OK | MB_ICONWARNING);
		SetStatusInfo("Save Failed!");
		return FALSE;
	}
}

void HexEditWnd::SetHEWndCaption() {
	char cCaption[400];

	if (!diOrgData.dwSize)
		SetWindowText(hMainWnd, HEDIT_WND_TITLE);

	lstrcpy(cCaption, HEDIT_WND_TITLE);
	lstrcat(cCaption, " - ");
	lstrcat(cCaption, fInput.GetFilePath());
	lstrcat(cCaption, " ");
	if (IsReadOnly())
		lstrcat(cCaption, "[READONLY]");
	else
		lstrcat(cCaption, "[READWRITE]");

	if (CanSave()) {
		lstrcat(cCaption, " *");
	}

	SetWindowText(hMainWnd, cCaption);
	return;
}

BOOL HexEditWnd::IsReadOnly() {
	return diOrgData.bReadOnly || bReadOnly;
}

BOOL HexEditWnd::SetSelection(DWORD dwOffStart, DWORD dwOffEnd) {
	if (IsOutOfRange(dwOffStart) || IsOutOfRange(dwOffEnd))
		return FALSE;

	stat.dwOffSelStart   = __min(dwOffStart, dwOffEnd);
	stat.dwOffSelEnd     = __max(dwOffStart, dwOffEnd);
	stat.bSel            = TRUE;

	if (stat.bCaretVisible) {
		HideCaret(hMainWnd);
		stat.bCaretVisible = FALSE;
	}

	if (dwOffEnd > dwOffStart) {
		stat.posCaret.bHiword       = FALSE;
		stat.posCaret.dwOffset      = dwOffEnd;
	} else {
		stat.posCaret.bHiword   = TRUE;
		stat.posCaret.dwOffset  = dwOffEnd;
	}

	if (!stat.bCaretPosValid) {
		stat.posCaret.bTextSection = FALSE;
		stat.bCaretPosValid        = TRUE;
	}

	ConfigureTBCCP();
	RepaintClientArea();

	return TRUE;
}

//
// returns:
// whether there was a selection before
//
BOOL HexEditWnd::KillSelection() {
	int iRet;

	if (stat.bSel) {
		stat.bSel = FALSE;
		ConfigureTBCCP();
		iRet = TRUE;
	} else {
		iRet = FALSE;
	}

	return iRet;
}

//
// args:
// ppClient - x and y position in client coordinates
//
// returns:
// whether the the input changed the selection
//
BOOL HexEditWnd::Point2Selection(LPPOINT ppClient)
{
	HE_POS  pos, *pposLast;

	if (!stat.bLastLBDownPos)
		return FALSE;

	if (!PointToPos(ppClient, &pos))
		return FALSE;

	// mouse moved since last button down ?
	pposLast = &stat.posLastLButtonDown;
	if (pos.bTextSection == pposLast->bTextSection) {
		if ((pos.dwOffset != pposLast->dwOffset) ||
			((pos.dwOffset == pposLast->dwOffset) && pos.bHiword != pposLast->bHiword)) {
			SetSelection(pposLast->dwOffset, pos.dwOffset);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL HexEditWnd::MouseMoveSelect(LPPOINT pos) {
	POINT  poi;
	BOOL   bRet;

	poi = *pos;

	// cursor not in text/hex region ?
	if ((DWORD)poi.x > PAIRS_X
		&& (DWORD)poi.x < CHARS_X + 16*uFontWidth
		&& stat.bMouseSelecting)
		if ((DWORD)poi.y > iyHETop + uMaxLines*uFontHeight) { // under ?
			// scroll down
			stat.posCaret.dwOffset += 2*16;
			ValidatePos( &stat.posCaret );
			MakeCaretVisible();

			if (stat.posCaret.bTextSection)
				poi.x = CHARS_X + 15*uFontWidth;
			else
				poi.x = PAIRS_X + 15*DIGIT_PAIR_WIDTH;
			poi.y = iyHETop + uFontHeight * (uMaxLines-1);
			if (timerId == 0) {
				SetTimer(hMainWnd, SELECT_TIMER, 50, NULL);
			}
		} else if ((DWORD)poi.y < iyHETop) { // over ?
			// scroll up
			stat.posCaret.dwOffset -= 2*16;
			ValidatePos( &stat.posCaret );
			MakeCaretVisible();

			if (stat.posCaret.bTextSection)
				poi.x = CHARS_X;
			else
				poi.x = PAIRS_X;
			poi.y = iyHETop;
			if (timerId == 0) {
				SetTimer(hMainWnd, SELECT_TIMER, 50, NULL);
			}
		} else {
			KillTimer(hMainWnd, SELECT_TIMER);
			timerId = 0;
		}

	bRet = Point2Selection(&poi);

	return bRet; // RET
}

//
// Args:
// hWnd - can be the window handle of the main window or of the TB
//
BOOL HexEditWnd::HEHandleWM_MOUSEMOVE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	POINT  poi;

	//
	// handle selection
	//
	if ( !TESTBIT(wParam, MK_LBUTTON) )
		return FALSE; // ERR

	poi.x = LOWORD(lParam);
	poi.y = HIWORD(lParam);
	
	return MouseMoveSelect(&poi);
}

UINT HexEditWnd::GetCurrentLine() {
	return(UINT)(stat.dwCurOffset / 16);
}

//
// same as "SetTopLine" but fails if "iLine" is invalid
//
BOOL HexEditWnd::SetCurrentLine(UINT iLine)
{
	if ((int)iLine < 0)
		return FALSE; // ERR
	if (iLine > GetTotalLineNum())
		return FALSE; // ERR

	SetTopLine((int)iLine);

	return TRUE;
}

//
// returns:
// whether sth was fixed
//
BOOL HexEditWnd::ValidateLine(int *piLine)
{
	int iLastLine = GetTotalLineNum();

	if (*piLine < 0) {
		*piLine = 0;
		return TRUE; // OK
	} else if (*piLine > iLastLine) {
		*piLine = iLastLine;
		return TRUE; // OK
	} else
		return FALSE; // ERR
}

//
// returns:
// whether the last state
//
BOOL HexEditWnd::SetHEWnd2Top(BOOL bTop)
{
	RECT  rect;
	BOOL  bBefore;

	// set wnd state
	GetWindowRect(hMainWnd, &rect);
	SetWindowPos(
				hMainWnd,
				bTop ? HWND_TOPMOST : HWND_NOTOPMOST,
				rect.left,
				rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				SWP_SHOWWINDOW);

	// handle vars
	bBefore  = bHEOnTop;
	bHEOnTop = bTop;

	return bBefore; 
}

BOOL HexEditWnd::Search(PHE_SEARCHOPTIONS pso, DWORD *pOffset) {
	DWORD   dwCurOff;
	BYTE*   pby;
	BOOL    bFound;

	if (!pso->bInited)
		return FALSE;

	dwCurOff = pso->dwStartOff;

	// correct a too big off (when up search)
	if (!pso->bDown &&
		diData.dwSize - dwCurOff < pso->dwcStr)
		dwCurOff = diData.dwSize - pso->dwcStr;

	pby      = (BYTE*)((DWORD)diData.pDataBuff + dwCurOff);
	bFound   = FALSE;

	if (pso->bASCIIStr || pso->bWideCharStr) {
		//
		// non-case sensitive ASCII/UNICODE string search
		//
		if (!pso->bCaseSensitive) {
			if ( pso->bASCIIStr ) {
				// ascii
				if (pso->bDown)	// ...down
					while (dwCurOff + pso->dwcStr <= diData.dwSize) {
						if (!strnicmp((PCSTR)pso->pData, (PCSTR)pby, pso->dwcStr)) {
							bFound = TRUE;
							break;
						}
						++dwCurOff;
						++pby;
					}
				else // ...up
					while (dwCurOff != (DWORD)-1) {
						if (!strnicmp((PCSTR)pso->pData, (PCSTR)pby, pso->dwcStr)) {
							bFound = TRUE;
							break;
						}
						--dwCurOff;
						--pby;
					}
			} else {
				// unicode
				if (pso->bDown)	// ...down
					while (dwCurOff + pso->dwcStr <= diData.dwSize) {
						if ( !wcsnicmp((LPWSTR)pso->pData, (LPWSTR)pby, pso->dwcStr/2) ) {
							bFound = TRUE;
							break;
						}
						++dwCurOff;
						++pby;
					}
				else // ...up
					while (dwCurOff != (DWORD)-1) {
						if ( !wcsnicmp((LPWSTR)pso->pData, (LPWSTR)pby, pso->dwcStr/2) ) {
							bFound = TRUE;
							break;
						}
						--dwCurOff;
						--pby;
					}
			}
		}
		//
		// case sensitive ASCII/UNICODE string search
		//
		else {
			if (pso->bDown)	// ..down
				while (dwCurOff + pso->dwcStr <= diData.dwSize) {
					if (!memcmp(pso->pData, pby, pso->dwcStr)) {
						bFound = TRUE;
						break;
					}
					++dwCurOff;
					++pby;
				}
			else // ...up
				while (dwCurOff != -1) {
					if (!memcmp(pso->pData, pby, pso->dwcStr)) {
						bFound = TRUE;
						break;
					}
					--dwCurOff;
					--pby;
				}
		}
	} else {
		//
		// byte search
		//
		if (pso->bDown)	// ...down
			while (dwCurOff + pso->dwcStr <= diData.dwSize) {
				if (!memcmp(pso->pData, pby, pso->dwcStr)) {
					bFound = TRUE;
					break;
				}
				++dwCurOff;
				++pby;
			}
		else
			while (dwCurOff != -1) {
				if (dwCurOff == 0x10)
					__asm NOP
					if (!memcmp(pso->pData, pby, pso->dwcStr)) {
						bFound = TRUE;
						break;
					}
				--dwCurOff;
				--pby;
			}

	}

	if (bFound) {
		if (pOffset != NULL) {
			*pOffset = dwCurOff;
		}
	}
	return bFound;
}

//
// searchs the stuff in the HE_SEARCHOPTIONS structure and setups the GUI
//
// returns:
// whether sth was found
//
//#pragma optimize("", off)
BOOL HexEditWnd::PerformStrSearch(PHE_SEARCHOPTIONS pso) {
	DWORD   dwCurOff;
	BOOL    bFound;

	SetStatusInfo("Searching...");
	//KillSelection();
	RepaintClientAreaNow();
	bFound = Search(pso, &dwCurOff);

	if (bFound) {
		SetSelection(dwCurOff, dwCurOff + pso->dwcStr - 1);
		if (!IsOffsetVisible(dwCurOff))
			SetTopLine(dwCurOff);
		SetStatusInfo("Found!");
	} else {
		SetStatusInfo(pso->bDown ? "Buffer end reached." : "Buffer start reached.");
	}

	MakeCaretVisible();
	SetCaret();
	RepaintClientArea();
	return bFound;
}

BOOL HexEditWnd::PerformSearchAgain(PHE_SEARCHOPTIONS pso, BOOL bDown) {
	if (!pso->bInited)
		return FALSE; // ERR

	if (!stat.bCaretPosValid) {
		ErrMsg("Caret to set!");
		return FALSE; // ERR
	}

	if (bDown) {
		if (stat.bSel)
			pso->dwStartOff = stat.dwOffSelEnd + 1;
		else
			pso->dwStartOff	= stat.posCaret.dwOffset;
	} else {
		if (stat.bSel)
			pso->dwStartOff = stat.dwOffSelStart - 1;
		else
			pso->dwStartOff	= stat.posCaret.dwOffset;
	}
	pso->bDown = bDown;

	return PerformStrSearch(pso);
}

BOOL HexEditWnd::PerformStrReplace(PHE_SEARCHOPTIONS pso) {
	DWORD   dwCurOff;
	BOOL    bFound;

	if (IsReadOnly() || (pso->dwcStr != pso->dwcReplaceStr && !IsResizingAllowed())) {
		SetStatusInfo("Readonly or not sizable!");
		RepaintClientArea();
		return FALSE;
	}

	/*
	 * may be already selected
	 */
	if (pso->bDown) {
		if (stat.bSel)
			pso->dwStartOff = stat.dwOffSelStart;
	} else {
		if (stat.bSel)
			pso->dwStartOff = stat.dwOffSelEnd;
	}

	bFound = Search(pso, &dwCurOff);
	if (bFound) {
		HE_OPER *op = new HE_OPER(op_paste, dwCurOff, pso->dwcStr, pso->dwcReplaceStr);
		memcpy(op->oldData, (BYTE*)diData.pDataBuff + dwCurOff, pso->dwcStr);
		if (pso->dwcReplaceStr > 0) {
			memcpy(op->newData, pso->pReplaceData, pso->dwcReplaceStr);
		}
		AddOper(op);
		ApplyOper(op);
		if (!IsOffsetVisible(dwCurOff))
			SetTopLine(dwCurOff);
		SetStatusInfo("Repalced!");
	} else {
		SetStatusInfo("Not found!");
	}

	ConfigureTB();
	SetupVScrollbar();
	RepaintClientArea();
	return bFound;
}

BOOL HexEditWnd::PerformStrReplaceAll(PHE_SEARCHOPTIONS pso) {
	DWORD   dwCurOff;
	BOOL    bFound;
	int		count;

	if (IsReadOnly() || (pso->dwcStr != pso->dwcReplaceStr && !IsResizingAllowed())) {
		SetStatusInfo("Readonly or not sizable!");
		return FALSE;
	}

	pso->dwStartOff = 0;
	pso->bDown = TRUE;

	count = 0;
	while (TRUE) {
		bFound = Search(pso, &dwCurOff);
		if (!bFound) {
			break;
		}
		count++;
		HE_OPER *op = new HE_OPER(op_paste, dwCurOff, pso->dwcStr, pso->dwcReplaceStr);
		memcpy(op->oldData, (BYTE*)diData.pDataBuff + dwCurOff, pso->dwcStr);
		if (pso->dwcReplaceStr > 0) {
			memcpy(op->newData, pso->pReplaceData, pso->dwcReplaceStr);
		}
		AddOper(op);
		ApplyOper(op);
		if (!IsOffsetVisible(dwCurOff))
			SetTopLine(dwCurOff);
		pso->dwStartOff = dwCurOff + pso->dwcStr;
	}

	if (count > 0) {
		SetStatusInfo("%d occurance replaced!", count);
	} else {
		SetStatusInfo("Search string not found!");
	}
	ConfigureTB();
	SetupVScrollbar();
	RepaintClientArea();
	return bFound;
}

BOOL HexEditWnd::CopySelectedBlockAsText() {
	HANDLE               hMem;
	void                 *pMem;
	int 				 dwc;

	if (!CanCopy()) return FALSE;
	return TRUE;

	dwc = stat.dwOffSelEnd - stat.dwOffSelStart + 1;
	if (!OpenClipboard(NULL)) {
		ErrMsg("Couldn't open clipboard!");
		return FALSE;
	}

	hMem = GlobalAlloc(GHND | GMEM_SHARE, dwc + 1);
	if (!hMem) {
		CloseClipboard();
		ErrMsg(STR_NO_MEM);
		return FALSE;
	}

	pMem = GlobalLock(hMem);
	memcpy(pMem, (void*)((DWORD)diData.pDataBuff + stat.dwOffSelStart), dwc);
	((char *)pMem)[dwc] = 0;
	GlobalUnlock(hMem);

	if (!SetClipboardData(CF_TEXT, hMem)) {
		GlobalFree(hMem);
		CloseClipboard();
		ErrMsg("Couldn't copy data to clipboard!");
		return FALSE;
	}

	CloseClipboard();
	ConfigureTB();
	return TRUE;
}

BOOL HexEditWnd::CopySelectedBlock()
{
	HANDLE               hMem;
	void                 *pMem;
	PHE_CLIPBOARD_DATA   pcbd;
	DWORD                dwc;

	if (!CanCopy()) return FALSE;

	dwc = stat.dwOffSelEnd - stat.dwOffSelStart + 1;
	if (!OpenClipboard(NULL)) {
		ErrMsg("Couldn't open clipboard!");
		return FALSE;
	}

	hMem = GlobalAlloc(GHND | GMEM_SHARE, dwc + 1);
	if (!hMem) {
		CloseClipboard();
		ErrMsg(STR_NO_MEM);
		return FALSE;
	}
	pMem = GlobalLock(hMem);
	memcpy(pMem, (void*)((DWORD)diData.pDataBuff + stat.dwOffSelStart), dwc);
	((char *)pMem)[dwc] = 0;
	GlobalUnlock(hMem);

    EmptyClipboard();
	if (!SetClipboardData(CF_TEXT, hMem)) {
		GlobalFree(hMem);
		CloseClipboard();
		ErrMsg("Couldn't copy data to clipboard!");
		return FALSE;
	}

	hMem  = GlobalAlloc(GHND | GMEM_SHARE, 4 + dwc);
	if (!hMem) {
		CloseClipboard();
		ErrMsg(STR_NO_MEM);
		return FALSE;
	}
	pMem = GlobalLock(hMem);
	pcbd = (PHE_CLIPBOARD_DATA)pMem;
	memcpy(
		  &pcbd->byDataStart,
		  (void*)((DWORD)diData.pDataBuff + stat.dwOffSelStart),
		  dwc);
	pcbd->dwDataSize = dwc;
	GlobalUnlock(hMem);

	if (!SetClipboardData(cf16Edit, hMem)) {
		GlobalFree(hMem);
		CloseClipboard();
		ErrMsg("Couldn't copy data to clipboard!");
		return FALSE;
	}

	CloseClipboard();
	ConfigureTB();
	return TRUE;
}

BOOL HexEditWnd::DeleteSelectedBlock() {
	if (!CanCut()) return FALSE;

	DWORD sellen = stat.dwOffSelEnd - stat.dwOffSelStart + 1;

	HE_OPER *op = new HE_OPER(op_cut, stat.dwOffSelStart, sellen, 0);
	memcpy(op->oldData, (BYTE *)diData.pDataBuff + stat.dwOffSelStart, sellen);
	AddOper(op);
	ApplyOper(op);

	KillSelection();
	ConfigureTB();
	SetupVScrollbar();
	RepaintClientArea();

	return TRUE;
}

BOOL HexEditWnd::CutSelectedBlock() {
	if (!CanCut()) return FALSE;

	if (!CopySelectedBlock())
		return FALSE;

	DeleteSelectedBlock();
	return TRUE;
}

BOOL HexEditWnd::PasteBlockFromCB() {
	PHE_CLIPBOARD_DATA   pcbd;

	if (!CanPaste()) return FALSE;

	if (!stat.bCaretPosValid) {
		ErrMsg("Please select one byte first!");
		return FALSE;
	}

	if (!OpenClipboard(hMainWnd)) {
		ErrMsg("Couldn't open clipboard!");
		return FALSE;
	}
	pcbd = GetClipboardData();
	if (!pcbd) {
		CloseClipboard();
		ErrMsg("Couldn't receive clipboard data!");
		return FALSE;
	}
	CloseClipboard();


	DWORD oldlen, dwOffset;
	if (stat.bSel) {
		oldlen = stat.dwOffSelEnd - stat.dwOffSelStart + 1;
		dwOffset = stat.dwOffSelStart;
		KillSelection();
	} else {
		if (bInsert) {
			oldlen = 0;
		} else {
			oldlen = pcbd->dwDataSize;
		}
		dwOffset = stat.posCaret.dwOffset;
	}

	if (oldlen == pcbd->dwDataSize) {
		if (!memcmp((BYTE*)diData.pDataBuff + dwOffset, &pcbd->byDataStart, oldlen)) {
			if (pcbd) {
				free(pcbd);
			}
			return TRUE;
		}
	}

	HE_OPER *op = new HE_OPER(op_paste, dwOffset, oldlen, pcbd->dwDataSize);
	if (oldlen > 0) {
		memcpy(op->oldData, (BYTE*)diData.pDataBuff + dwOffset, oldlen);
	}

	memcpy(op->newData, &pcbd->byDataStart, pcbd->dwDataSize);
	AddOper(op);
	ApplyOper(op);

	//
	// repaint
	//
	ConfigureTB();
	SetupVScrollbar();
	if (!IsOffsetVisible( stat.posCaret.dwOffset ) )
		SetTopLine();
	RepaintClientArea();

	if (pcbd) {
		free(pcbd);
	}

	return TRUE; // OK
}

BOOL HexEditWnd::UndoChanges() {
	if (!CanUndo()) {
		return FALSE; 
	}

	HE_OPER *op = current->getOper();
	UndoOper(op);
	current = current->getPrev();

	ValidatePos( &stat.posCaret );
	SetCaret();
	ConfigureTB();
	SetupVScrollbar();
	if (!IsOffsetVisible( stat.posCaret.dwOffset ) )
		SetTopLine();
	RepaintClientArea();

	return TRUE;
}

BOOL HexEditWnd::RedoChanges() {
	if (!CanRedo()) {
		return FALSE; 
	}

	current = current->getNext();
	HE_OPER *op = current->getOper();
	ApplyOper(op);

	ValidatePos( &stat.posCaret );
	SetCaret();
	ConfigureTB();
	SetupVScrollbar();
	if (!IsOffsetVisible( stat.posCaret.dwOffset ) )
		SetTopLine();
	RepaintClientArea();

	return TRUE;
}

void HexEditWnd::AddOper(HE_OPER *op) {
	if (current == NULL) {
		current = operList->addOper(op);
	} else {
		if (savepoint != NULL && current->before(savepoint)) {
			savepoint = operList;
			bSavePointValid = FALSE;
		}
		current->releaseAfter();
		current = current->addOper(op);
	}
}

BOOL HexEditWnd::CanSave() {
	if (!bSavePointValid || current != savepoint) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL HexEditWnd::CanRedo() {
	if (IsReadOnly())
		return FALSE;
	if (current->getNext() != NULL) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL HexEditWnd::CanUndo() {
	if (IsReadOnly())
		return FALSE;
	if (current == operList) {
		return FALSE;
	}

	return TRUE;
}

void HexEditWnd::ApplyOper(HE_OPER *op) {
	HE_POS posNew;

	memcpy(&posNew, &stat.posCaret, sizeof(HE_POS));
	posNew.dwOffset = op->dwOffset;
	posNew.bHiword      = TRUE;
	switch (op->type) {
		case op_modify:
			((BYTE *)(diData.pDataBuff))[op->dwOffset] = op->newData[0];
			break;
		case op_cut:
		   	if (diData.dwSize > op->dwOffset + op->dwOldLen)
				MEMCPY(
				  (BYTE*)diData.pDataBuff + op->dwOffset,
				  (BYTE*)diData.pDataBuff + op->dwOffset + op->dwOldLen,
				  diData.dwSize - op->dwOffset - op->dwOldLen);
			diData.dwSize -= op->dwOldLen;
			diData.pDataBuff = (BYTE*)realloc(diData.pDataBuff, diData.dwSize);
			KillSelection();
			break;
		case op_paste:
			if (op->dwNewLen > op->dwOldLen) {
				diData.pDataBuff = (BYTE*)realloc(diData.pDataBuff, diData.dwSize + op->dwNewLen - op->dwOldLen);
				MEMCPY(
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwNewLen, 
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwOldLen, 
					diData.dwSize - op->dwOffset - op->dwOldLen
					);
				diData.dwSize += op->dwNewLen - op->dwOldLen;
			} else if (op->dwOldLen > op->dwNewLen) {
				MEMCPY(
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwNewLen,
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwOldLen, 
					diData.dwSize - op->dwOffset - op->dwOldLen
					);
				diData.dwSize -= op->dwOldLen - op->dwNewLen;
				diData.pDataBuff = (BYTE*)realloc(diData.pDataBuff, diData.dwSize);
			}
			memcpy((BYTE*)diData.pDataBuff + op->dwOffset, op->newData, op->dwNewLen);
			break;
	}
	SetCaret(&posNew);
}

void HexEditWnd::UndoOper(HE_OPER *op) {
	HE_POS posNew;

	memcpy(&posNew, &stat.posCaret, sizeof(HE_POS));
	posNew.dwOffset = op->dwOffset;
	posNew.bHiword      = TRUE;
	switch (op->type) {
		case op_modify:
			((BYTE *)(diData.pDataBuff))[op->dwOffset] = op->oldData[0];
			break;
		case op_cut:
			diData.pDataBuff = (BYTE*)realloc(diData.pDataBuff, diData.dwSize + op->dwOldLen);
			if (diData.dwSize - 1 > op->dwOffset) {
				MEMCPY(
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwOldLen,
					(BYTE*)diData.pDataBuff + op->dwOffset,
					diData.dwSize - op->dwOffset
					  );
			}
			memcpy((BYTE*)diData.pDataBuff + op->dwOffset, op->oldData, op->dwOldLen);
			diData.dwSize += op->dwOldLen;

			SetSelection(op->dwOffset, op->dwOffset + op->dwOldLen - 1);
			break;
		case op_paste:
			if (op->dwNewLen > op->dwOldLen) {
				MEMCPY(
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwOldLen, 
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwNewLen, 
					diData.dwSize - op->dwOffset - op->dwNewLen
					);
				diData.dwSize -= op->dwNewLen - op->dwOldLen;
				diData.pDataBuff = (BYTE*)realloc(diData.pDataBuff, diData.dwSize);
			} else if (op->dwOldLen > op->dwNewLen) {
				diData.pDataBuff = (BYTE*)realloc(diData.pDataBuff, diData.dwSize + op->dwOldLen - op->dwNewLen);
				MEMCPY(
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwOldLen,
					(BYTE*)diData.pDataBuff + op->dwOffset + op->dwNewLen, 
					diData.dwSize - op->dwOffset - op->dwNewLen
					);
				diData.dwSize += op->dwOldLen - op->dwNewLen;
			}

			if (op->dwOldLen > 0) {
				memcpy((BYTE*)diData.pDataBuff + op->dwOffset, op->oldData, op->dwOldLen);
			}
			break;
	}
	SetCaret(&posNew);
}

void HexEditWnd::SelectAll() {
	if (IsAllSelected()) {
		KillSelection();
		SetCaret();
	} else {
		SetSelection(0, diData.dwSize - 1);
	}

	return;
}

BOOL HexEditWnd::IsAllSelected() {
	return(stat.bSel &&
		   stat.dwOffSelStart == 0 &&
		   stat.dwOffSelEnd == diData.dwSize - 1) ? TRUE : FALSE;
}

BOOL HexEditWnd::IsResizingAllowed() {
	return bResizingAllowed;
}

BOOL HexEditWnd::DoSpecifySettings(char *filename, int start, int len) {
	InitEdition();
	if (!DoEditFile(filename, FALSE)) {
		return FALSE;
	}

	bResizingAllowed = FALSE;
	bMinToTray       = FALSE;
	bSaveWinPos      = TRUE;
	bReadOnly = diOrgData.bReadOnly;
	if (start > 0) {
		SetSelection(start, start + len - 1);
		stat.dwCurOffset = start;
		stat.posCaret.dwOffset = start;
	}

	return TRUE;
}

void HexEditWnd::HEHandleWM_CLOSE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HE_WIN_POS   wp;
	int          iRet;

	if (CanSave()) {
		iRet = MessageBox(hMainWnd, "File changed, save or not", "16Edit", MB_YESNOCANCEL);
		if (iRet == IDYES) {
			SaveChanges();
		} else if (iRet == IDCANCEL) {
			return;
		}
	}

	if (bSaveWinPos) {
		wp.ix    = rctLastPos.left;
		wp.iy    = rctLastPos.top;
		wp.icx   = rctLastPos.right - rctLastPos.left;
		wp.icy   = rctLastPos.bottom - rctLastPos.top;
		WritePrivateProfileStruct(INI_SECTION, INI_WINPOSKEY, &wp, sizeof(wp), cIniPath);
	}

	PostQuitMessage(0);
	return;
}

void HexEditWnd::HEHandleWM_TRAYMENU(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	POINT p;

	switch (lParam) {
	case WM_RBUTTONDOWN:
		GetCursorPos(&p);
		TrackPopupMenu(hmTray, TPM_RIGHTALIGN, p.x, p.y, 0, hWnd, NULL);
		break;

	case WM_LBUTTONDBLCLK:
		HEditReturnFromTray();
		break;
	}
	return;
}

void HexEditWnd::HEditQuit() {
	SendMessage(hMainWnd, WM_CLOSE, 0, 0);
	return;
}

void HexEditWnd::HEHandleWM_MOVE(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	if (!IsIconic(hWnd) &&
		!IsZoomed(hWnd) )
		GetWindowRect( hWnd, &rctLastPos );
	return;
}

BOOL HexEditWnd::IsKeyDown(int iVKey) {
	return( HIWORD( GetAsyncKeyState(iVKey) ) != 0);
}

void HexEditWnd::SetTBHandle(HWND hTB) {
	this->hTB = hTB;
	return;
}

HINSTANCE HexEditWnd::GetInstance() {
	return hInst;
}

void HexEditWnd::SetStatusInfo(char *szFormat, ...) {
	va_list args;

	va_start(args, szFormat);
	wvsprintf(cSBInfo, szFormat, args);
	va_end(args);

	return;
}

void HexEditWnd::SetStatusText(char *szFormat, ...) {
	va_list args;

	va_start(args, szFormat);
	wvsprintf(cSBText, szFormat, args);
	va_end(args);

	if (strlen(cSBInfo) > 0) {
		strcat(cSBText, " | ");
		strcat(cSBText, cSBInfo);
	}
	return;
}

void HexEditWnd::SetStatusText() {
	char	msg[256];
	DWORD	dwOffset;

	if (stat.bSel)
		dwOffset = stat.dwOffSelStart;
	else if (stat.bCaretPosValid)
		dwOffset = stat.posCaret.dwOffset;
	else
		dwOffset = 0;

	wsprintf(msg, "OS:0x%04X | NS:0x%04X | FO:0x%04X", dwOldSize,diData.dwSize,dwOffset);
	if (file_type((char *)diData.pDataBuff)) {
		wsprintf(msg, "%s | VA:0x%08X", msg, GetVirtualAddress(dwOffset));
	}

	SetStatusText(msg);
	return;
}

void HexEditWnd::ShowAbout() {
	MSGBOXPARAMS args;

	ZERO(args);
	args.cbSize         = sizeof( args );
	args.hwndOwner      = hMainWnd;
	args.dwStyle        = MB_USERICON;
	args.hInstance      = GetInstance();
	args.lpszIcon       = (PSTR)IDI_16Edit;
	args.lpszCaption    = "About";
	args.lpszText       = "16Edit 1.04 - HexEditor by slangmgh\n"\
						  "Created from yoda's 16Edit module\n\n"\
						  "Changelog 1.04:\n"
						  "1. Copy text available in clipboard\n"
						  "2. Goto offset save in profile\n"
						  "3. Search/Replace text enable paste\n"
						  "4. Search/Replace text save in profile\n"
						  "5. Some small bugfix\n\n"
						  "Feature:\n"\
						  "1. Small/Green/Robust/Freeware\n"\
						  "2. Unlimited undo/redo\n"\
						  "3. Ascii/Ansi display switch\n"\
						  "4. Add/Modify/Add&Modify color indicator\n"\
						  "5. Size lock\n"\
						  "6. Shell integration\n"\
						  "7. Paste insert/overwrite mode\n"\
						  "8. Readonly/Readwrite mode\n"\
						  "9. File offset/Virtual address mode\n"\
						  "10. Ascii/Unicode/Binary search/replace\n"\
						  "11. You requesting...\n\n"\
						  "slangmgh@netease.com";
	MessageBoxIndirect( &args );

	return;
}

BOOL HexEditWnd::IsDBCSFirstByte(DWORD dwOffset) {
	BYTE byCur;
	int i;

	byCur = *((BYTE*)diData.pDataBuff + dwOffset);
	if (!IsDBCSLeadByte(byCur)) {
		return FALSE;
	}

	for (i = dwOffset - 1; i >= 0; i--) {
		byCur = *((BYTE*)diData.pDataBuff + i);
		if (!IsDBCSLeadByte(byCur)) {
			if ((dwOffset - i)%2) {
				return TRUE;
			} else {
				return FALSE;
			}
		}
	}

	//
	// every byte before are dbcs lead byte
	//
	if (dwOffset%2) {
		return FALSE;
	} else {
		return TRUE;
	}
}

//
// Get the data status at the given offset
//
// return
// 0 : NO change
// 1 : modify
// 2 : add
// 3 : add & modify
int	HexEditWnd::GetDataStatus(DWORD dwOffset) {
	int status = 0;
	EditOperList *oplist;
	HE_OPER *op;

	if (current == savepoint) {
		return 0;
	}

	if (savepoint->before(current)) {
		oplist = current;
		while (oplist != savepoint) {
			op = oplist->getOper();

			switch (op->type) {
				case op_modify:
					if (dwOffset == op->dwOffset) {
						status |= 1;
					}
					break;
				case op_paste:
					if (dwOffset >= op->dwOffset) {
						if (op->dwNewLen > op->dwOldLen) {
							if (dwOffset >= op->dwOffset + op->dwNewLen) {
								dwOffset -= op->dwNewLen - op->dwOldLen;
							} else if (dwOffset < op->dwOffset + op->dwOldLen) {
								status |= 1;
							} else {
								status |= 2;
								return status;
							}
						} else {
							if (dwOffset >= op->dwOffset + op->dwNewLen) {
								dwOffset += op->dwOldLen - op->dwNewLen;
							} else {
								status |= 1;
							}
						}
					}
					break;
				case op_cut:
					if (dwOffset >= op->dwOffset) {
						if (status & 2 && dwOffset < op->dwOffset + op->dwOldLen) {
							status = 1;
						}
						dwOffset += op->dwOldLen;
					}
					break;
			}
			oplist = oplist->getPrev();
		}
	} else {
		oplist = current->getNext();
		while (oplist != NULL) {
			op = oplist->getOper();

			switch (op->type) {
				case op_modify:
					if (dwOffset == op->dwOffset) {
						status |= 1;
					}
					break;
				case op_paste:
					if (dwOffset >= op->dwOffset) {
						if (op->dwOldLen > op->dwNewLen) {
							if (dwOffset >= op->dwOffset + op->dwOldLen) {
								dwOffset -= op->dwOldLen - op->dwNewLen;
							} else if (dwOffset < op->dwOffset + op->dwNewLen) {
								status |= 1;
							} else {
								status |= 2;
								return status;
							}
						} else {
							if (dwOffset >= op->dwOffset + op->dwOldLen) {
								dwOffset += op->dwNewLen - op->dwOldLen;
							} else {
								status |= 1;
							}
						}
					}
					break;
				case op_cut:
					if (dwOffset >= op->dwOffset && dwOffset < op->dwOffset + op->dwOldLen) {
						status |= 2;
						return status;
					} else if (dwOffset >= op->dwOffset + op->dwOldLen) {
						dwOffset -= op->dwOldLen;
					}
					break;
			}
			if (oplist == savepoint) {
				break;
			} else {
				oplist = oplist->getNext();
			}
		}
	}

	return status;
}

BOOL HexEditWnd::HEditToTray() {
	char *pch;

	ZERO(nidTray);
	nidTray.cbSize            = sizeof(nidTray);

	lstrcpy(nidTray.szTip, HEDIT_TRAY_TIP);
	if (pch = CPathString::ExtractFileName( fInput.GetFilePath())) {
		if (sizeof(nidTray.szTip) - sizeof(HEDIT_TRAY_TIP) - 3 >= lstrlen(pch) ) {
			lstrcat(nidTray.szTip, " - ");
			lstrcat(nidTray.szTip, pch);
			lstrlen(nidTray.szTip);
		}
	}

	nidTray.hWnd              = hMainWnd;
	nidTray.uID               = ID_TRAYICON;
	nidTray.uFlags            = NIF_TIP | NIF_ICON | NIF_MESSAGE;
	nidTray.hIcon             = (HICON)GetClassLong(hMainWnd, GCL_HICON);
	nidTray.uCallbackMessage  = WM_TRAYMENU;
	if (!Shell_NotifyIcon(NIM_ADD, &nidTray))
		return FALSE;

	ShowWindow(hMainWnd, SW_HIDE);
	return TRUE;
}

BOOL HexEditWnd::HEditKillTrayIcon() {
	return Shell_NotifyIcon(NIM_DELETE, &nidTray);
}

BOOL HexEditWnd::HEditReturnFromTray() {
	if (!HEditKillTrayIcon())
		return FALSE;

	ShowWindow(hMainWnd, SW_RESTORE);
	SetForegroundWindow(hMainWnd);

	return TRUE;
}

BOOL HexEditWnd::IsClipboardFormatOK() {
	return IsClipboardFormatAvailable(cf16Edit) || IsClipboardFormatAvailable(CF_TEXT);
}

PHE_CLIPBOARD_DATA HexEditWnd::GetClipboardData() {
	PHE_CLIPBOARD_DATA	pcbd;

	OpenClipboard(hMainWnd);
	if (IsClipboardFormatAvailable(cf16Edit)) {
		PHE_CLIPBOARD_DATA	pcbdold;
		int		len;

		pcbdold = (PHE_CLIPBOARD_DATA)::GetClipboardData(cf16Edit);
		len = pcbdold->dwDataSize;
		pcbd = (PHE_CLIPBOARD_DATA)malloc(len + 4);
		pcbd->dwDataSize = len;
		memcpy(&pcbd->byDataStart, &pcbdold->byDataStart, len);
	} else if (IsClipboardFormatAvailable(CF_TEXT)) {
		char	*pData;
		int		len;

		pData = (char *)::GetClipboardData(CF_TEXT);
		if(pData) {
			len = strlen(pData);
			pcbd = (PHE_CLIPBOARD_DATA)malloc(len + 4);
			pcbd->dwDataSize = len;
			memcpy(&pcbd->byDataStart, pData, len);
		}
	}
	CloseClipboard();
	return pcbd;
}
