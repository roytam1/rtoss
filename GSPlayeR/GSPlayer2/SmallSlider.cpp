#include "resource.h"
#include "gsplayer2.h"
#include "smallslider.h"

typedef struct _tagSliderInfo
{
	int nPos;
	int nMin;
	int nMax;
	int nTick;
	BOOL fPushed;
	BOOL fFocus;
}SLIDERINFO;

HBRUSH g_hBrBackgrnd = NULL;
HBITMAP g_hBmpTrack = NULL;
HBITMAP g_hBmpBackgrnd = NULL;
BOOL g_fPushed = FALSE;

void InitializeSmallSliderControl()
{
	// Load Bitmap for "Track"
 	g_hBmpTrack = LoadBitmap(GetInst(), (LPCTSTR)IDB_TRACK);
#ifdef _WIN32_WCE_PPC
 	g_hBrBackgrnd = (HBRUSH)GetStockObject(WHITE_BRUSH);
#else
	g_hBrBackgrnd = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
#endif

	// Register Class
	WNDCLASS	wc;
    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) SmallSliderControl;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
    wc.hInstance		= GetInst();
    wc.hIcon			= NULL;
    wc.hCursor			= 0;
    wc.hbrBackground	= g_hBrBackgrnd;
    wc.lpszMenuName		= 0;
    wc.lpszClassName	= SSCONTROL;
	RegisterClass(&wc);
}

void UninitializeSmallSliderControl()
{
	if (g_hBmpTrack)
		DeleteObject(g_hBmpTrack);
	if (g_hBrBackgrnd)
		DeleteObject(g_hBrBackgrnd);
}

void CreateBackground(HWND hWnd)
{
	if (g_hBmpBackgrnd)
		DeleteObject(g_hBmpBackgrnd);

	HDC hDC = GetDC(hWnd);
	SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);

	RECT rc;
	GetClientRect(hWnd, &rc);
	int nCenter = RECT_WIDTH(&rc) / 2;

	HDC hMemDC = CreateCompatibleDC(hDC);
	g_hBmpBackgrnd = CreateCompatibleBitmap(hDC, RECT_WIDTH(&rc), RECT_HEIGHT(&rc));
	SelectObject(hMemDC, g_hBmpBackgrnd);

	FillRect(hMemDC, &rc, g_hBrBackgrnd);

	// draw tick
	if (pInfo->nMax != pInfo->nMin)
	{
		int nTick = int(((double)(RECT_HEIGHT(&rc) - SSWIDTH) / 
					(pInfo->nMax - pInfo->nMin)) * (pInfo->nTick - pInfo->nMin)) + 5;
		POINT pt[2];
		pt[0].x = nCenter - 7; pt[0].y = nTick;
		pt[1].x = nCenter + 8; pt[1].y = nTick;
		Polyline(hMemDC, pt, sizeof(pt) / sizeof(POINT));
	}

	// draw background
	rc.left = nCenter - 4; rc.right = nCenter + 5;
#ifdef _WIN32_WCE_PPC
	FillRect(hMemDC, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
#else
	FillRect(hMemDC, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
#endif

	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hDC);
}

void UpdatePosition(HWND hWnd, SLIDERINFO* pInfo, int nNew)
{
	if (pInfo->nPos != nNew)
	{
		// invalidate old pos
		RECT rc;
		int nOldPos, nNewPos;
		GetClientRect(hWnd, &rc);

		int nCenter = RECT_WIDTH(&rc) / 2;
		
		nOldPos = (pInfo->nMax != pInfo->nMin) ? 
			int(((double)(RECT_HEIGHT(&rc) - SSWIDTH) / (pInfo->nMax - pInfo->nMin)) * (pInfo->nPos - pInfo->nMin)) : 0;
		pInfo->nPos = nNew;
		nNewPos = (pInfo->nMax != pInfo->nMin) ?
			int(((double)(RECT_HEIGHT(&rc) - SSWIDTH) / (pInfo->nMax - pInfo->nMin)) * (pInfo->nPos - pInfo->nMin)) : 0;

		rc.left = nCenter - 5;
		rc.top = min(nOldPos, nNewPos);
		rc.right = rc.left + SSWIDTH;
		rc.bottom = max(nOldPos, nNewPos) + SSWIDTH;

		InvalidateRect(hWnd, &rc, TRUE);
		UpdateWindow(hWnd);
		SendMessage(GetParent(hWnd), WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, pInfo->nPos), (LPARAM)hWnd);
	}
}

LRESULT CALLBACK SmallSliderControl(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)malloc(sizeof(SLIDERINFO));
			pInfo->nPos = pInfo->nMin = pInfo->nMax = pInfo->nTick = 0;
			pInfo->fPushed = FALSE;
			SetWindowLong(hWnd, GWL_USERDATA, (DWORD)pInfo);
			CreateBackground(hWnd);
			return 0;
		}
		case WM_DESTROY:
		{
			DeleteObject(g_hBmpBackgrnd);
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			free(pInfo);
			return 0;
		}
		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			InvalidateRect(hWnd, 0, TRUE);
			UpdateWindow(hWnd);
			return 0;
		case WM_PAINT:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);

			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);

			RECT rc;
			GetClientRect(hWnd, &rc);
			int nCenter = RECT_WIDTH(&rc) / 2;

			HDC hBmpDC = CreateCompatibleDC(hDC);
			SelectObject(hBmpDC, g_hBmpBackgrnd);
			BitBlt(hDC, ps.rcPaint.left, ps.rcPaint.top, RECT_WIDTH(&ps.rcPaint), RECT_HEIGHT(&ps.rcPaint),
				hBmpDC, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

			// draw focus line
			if (GetFocus() == hWnd)
			{
				rc.left = nCenter - 4; rc.right = nCenter + 5;
				POINT pt[5];
				pt[0].x = rc.left; pt[0].y = rc.top;
				pt[1].x = rc.left; pt[1].y = rc.bottom - 1;
				pt[2].x = rc.right - 1; pt[2].y = rc.bottom - 1;
				pt[3].x = rc.right - 1; pt[3].y = rc.top;
				pt[4].x = rc.left; pt[4].y = rc.top;
				Polyline(hDC, pt, sizeof(pt) / sizeof(POINT));
			}
						
			// draw trackpoint
			int x = nCenter - 5;
			int y = (pInfo->nMax != pInfo->nMin) ?
				int(((double)(RECT_HEIGHT(&rc) - SSWIDTH) / (pInfo->nMax - pInfo->nMin)) * (pInfo->nPos - pInfo->nMin)) : 0;

			SelectObject(hBmpDC, g_hBmpTrack);
			BitBlt(hDC, x, y, SSWIDTH, SSWIDTH, hBmpDC, 0, 0, SRCCOPY);
			
			DeleteDC(hBmpDC);
			EndPaint(hWnd, &ps);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			SetCapture(hWnd);
			pInfo->fPushed = TRUE;

			SetFocus(hWnd);

			RECT rc;
			GetClientRect(hWnd, &rc);

			short nPoint = HIWORD(lParam);
			int nNewPos = (RECT_HEIGHT(&rc) - SSWIDTH) ?
				int((double(pInfo->nMax - pInfo->nMin) / (RECT_HEIGHT(&rc) - SSWIDTH)) * nPoint + pInfo->nMin) : 0;
			nNewPos = min(pInfo->nMax, max(pInfo->nMin, nNewPos));
			UpdatePosition(hWnd, pInfo, nNewPos);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			ReleaseCapture();
			pInfo->fPushed = FALSE;
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			if (pInfo->fPushed)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);

				short nPoint = HIWORD(lParam);
				int nNewPos = (RECT_HEIGHT(&rc) - SSWIDTH) ?
					int((double(pInfo->nMax - pInfo->nMin) / (RECT_HEIGHT(&rc) - SSWIDTH)) * nPoint + pInfo->nMin): 0;
				nNewPos = min(pInfo->nMax, max(pInfo->nMin, nNewPos));
				UpdatePosition(hWnd, pInfo, nNewPos);
			}
			return 0;
		}
		// scrollbar messages
		case TBM_GETPOS:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			return pInfo->nPos;
		}
		case TBM_GETRANGEMAX:
		case TBM_GETRANGEMIN:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			return (uMsg == TBM_GETRANGEMIN) ? pInfo->nMin : pInfo->nMax;
		}
		case TBM_SETPOS:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			pInfo->nPos = min(pInfo->nMax, max(pInfo->nMin, (long)lParam));
			if (wParam)
			{
				InvalidateRect(hWnd, 0, TRUE);
				UpdateWindow(hWnd);
			}
			return 0;
		}
		case TBM_SETRANGE:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			pInfo->nMin = LOWORD(lParam);
			pInfo->nMax = HIWORD(lParam);
			pInfo->nPos = min(pInfo->nMax, max(pInfo->nMin, pInfo->nPos));
			CreateBackground(hWnd);
			InvalidateRect(hWnd, 0, TRUE);
			UpdateWindow(hWnd);
			return 0;
		}
		case WM_KEYDOWN:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			int nNewPos;
			switch (wParam)
			{
				case VK_RIGHT:
				case VK_UP: nNewPos = pInfo->nPos - 1; break;
				case VK_LEFT:
				case VK_DOWN: nNewPos = pInfo->nPos + 1; break;
				case VK_PRIOR: nNewPos = pInfo->nPos - (pInfo->nMax - pInfo->nMin) / 5; break;
				case VK_NEXT: nNewPos = pInfo->nPos + (pInfo->nMax - pInfo->nMin) / 5; break;
				case VK_HOME: nNewPos = pInfo->nMin; break;
				case VK_END: nNewPos = pInfo->nMax; break;
				default: return 0;
			}

			RECT rc;
			GetClientRect(hWnd, &rc);

			nNewPos = min(pInfo->nMax, max(pInfo->nMin, nNewPos));
			
			UpdatePosition(hWnd, pInfo, nNewPos);
			return 0;
		}
		case TBM_SETTIC:
		{
			SLIDERINFO* pInfo = (SLIDERINFO*)GetWindowLong(hWnd, GWL_USERDATA);
			pInfo->nTick = (long)lParam;
			CreateBackground(hWnd);
			InvalidateRect(hWnd, 0, TRUE);
			UpdateWindow(hWnd);
			return 0;
		}
		case WM_GETDLGCODE:
			return DLGC_WANTARROWS;
		default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

