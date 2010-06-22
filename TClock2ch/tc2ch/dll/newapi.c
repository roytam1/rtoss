/*-------------------------------------------
  newapi.c
  GradientFill and Layerd Window
  Kazubon 1999
---------------------------------------------*/

#include "tcdll.h"

#if (defined(_MSC_VER) && (_MSC_VER < 1200)) || (defined(__BORLANDC__) && (__BORLANDC__ < 0x550))

typedef USHORT COLOR16;

typedef struct _TRIVERTEX
{
    LONG    x;
    LONG    y;
    COLOR16 Red;
    COLOR16 Green;
    COLOR16 Blue;
    COLOR16 Alpha;
}TRIVERTEX,*PTRIVERTEX,*LPTRIVERTEX;

typedef struct _GRADIENT_RECT
{
    ULONG UpperLeft;
    ULONG LowerRight;
}GRADIENT_RECT,*PGRADIENT_RECT,*LPGRADIENT_RECT;

typedef struct _BLENDFUNCTION
{
    BYTE   BlendOp;
    BYTE   BlendFlags;
    BYTE   SourceConstantAlpha;
    BYTE   AlphaFormat;
} BLENDFUNCTION,*PBLENDFUNCTION;

#define AC_SRC_OVER                 0x00

#define GRADIENT_FILL_RECT_H    0x00000000
#define GRADIENT_FILL_RECT_V    0x00000001
#define GRADIENT_FILL_TRIANGLE  0x00000002
#define GRADIENT_FILL_OP_FLAG   0x000000ff

#endif

#if !defined(WS_EX_LAYERED)
#define WS_EX_LAYERED 0x80000
#endif

#if !defined(LWA_COLORKEY)
#define LWA_COLORKEY  1
#endif

#if !defined(LWA_ALPHA)
#define LWA_ALPHA     2
#endif

HMODULE hmodMSIMG32 = NULL;
BOOL (WINAPI *pGradientFill)(HDC,PTRIVERTEX,ULONG,PVOID,ULONG,ULONG) = NULL;
BOOL (WINAPI *pAlphaBlend)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION) = NULL;
BOOL (WINAPI *pTransparentBlt)(HDC,int,int,int,int,HDC,int,int,int,int,UINT) = NULL;

HMODULE hmodUSER32 = NULL;
BOOL (WINAPI *pSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD) = NULL;

HMODULE hmodUXTHEME = NULL;
HRESULT (WINAPI *pDrawThemeParentBackground)(HWND,HDC,RECT*) = NULL;

static BOOL bInitGradientFill = FALSE;
static BOOL bInitAlphaBlend = FALSE;
static BOOL pInitTransparentBlt = FALSE;
static BOOL bInitLayeredWindow = FALSE;
static BOOL bInitDrawTheme = FALSE;

static void EndLayeredStartMenu(HWND hwndClock);
static void RefreshRebar(HWND hwndBar);

int nAlphaStartMenu = 255;

void InitGradientFill(void)
{
	if(bInitGradientFill) return;

	hmodMSIMG32 = LoadLibrary("msimg32.dll");
	if(hmodMSIMG32 != NULL)
	{
		(FARPROC)pGradientFill = GetProcAddress(hmodMSIMG32, "GradientFill");
		if(pGradientFill == NULL)
		{
			FreeLibrary(hmodMSIMG32); hmodMSIMG32 = NULL;
		}
	}
	bInitGradientFill = TRUE;
}

void InitAlphaBlend(void)
{
	if(bInitAlphaBlend) return;

	hmodMSIMG32 = LoadLibrary("msimg32.dll");
	if(hmodMSIMG32 != NULL)
	{
		(FARPROC)pAlphaBlend = GetProcAddress(hmodMSIMG32, "AlphaBlend");
		if(pAlphaBlend == NULL)
		{
			FreeLibrary(hmodMSIMG32); hmodMSIMG32 = NULL;
		}
	}
	bInitAlphaBlend = TRUE;
}

void InitTransparentBlt(void)
{
	if(pInitTransparentBlt) return;

	hmodMSIMG32 = LoadLibrary("msimg32.dll");
	if(hmodMSIMG32 != NULL)
	{
		(FARPROC)pTransparentBlt = GetProcAddress(hmodMSIMG32, "TransparentBlt");
		if(pTransparentBlt == NULL)
		{
			FreeLibrary(hmodMSIMG32); hmodMSIMG32 = NULL;
		}
	}
	pInitTransparentBlt = TRUE;
}

void InitLayeredWindow(void)
{
	if(bInitLayeredWindow) return;

	hmodUSER32 = LoadLibrary("user32.dll");
	if(hmodUSER32 != NULL)
	{
		(FARPROC)pSetLayeredWindowAttributes =
			GetProcAddress(hmodUSER32, "SetLayeredWindowAttributes");
		if(pSetLayeredWindowAttributes == NULL)
		{
			FreeLibrary(hmodUSER32); hmodUSER32 = NULL;
		}
	}
	bInitLayeredWindow = TRUE;
}

void InitDrawTheme(void)
{
	if(bInitDrawTheme) return;

	hmodUXTHEME = LoadLibrary("uxtheme.dll");
	if(hmodUXTHEME != NULL)
	{
		(FARPROC)pDrawThemeParentBackground =
			GetProcAddress(hmodUXTHEME, "DrawThemeParentBackground");
		if(pDrawThemeParentBackground == NULL)
		{
			FreeLibrary(hmodUXTHEME); hmodUXTHEME = NULL;
		}
	}
	bInitDrawTheme = TRUE;
}

void EndNewAPI(HWND hwndClock)
{
	if(hmodUXTHEME != NULL) FreeLibrary(hmodUXTHEME);
	hmodUXTHEME = NULL; pDrawThemeParentBackground = NULL;

	if(hmodMSIMG32 != NULL) FreeLibrary(hmodMSIMG32);
	hmodMSIMG32 = NULL; pGradientFill = NULL; pAlphaBlend = NULL; pTransparentBlt = NULL;

	if(pSetLayeredWindowAttributes)
	{
		HWND hwnd;
		LONG exstyle;

		hwnd = GetParent(GetParent(hwndClock));
		exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
		if(exstyle & WS_EX_LAYERED)
		{
			exstyle &= ~WS_EX_LAYERED;
			SetWindowLong(hwnd, GWL_EXSTYLE, exstyle);
			RefreshRebar(hwnd);
		}

		EndLayeredStartMenu(hwndClock);
	}

	if(hmodUSER32 != NULL) FreeLibrary(hmodUSER32);
	hmodUSER32 = NULL;
	pSetLayeredWindowAttributes = NULL;
}

void GradientFillClock(HDC hdc, RECT* prc, COLORREF col1, COLORREF col2, DWORD grad)
{
	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;

	if(!pGradientFill) InitGradientFill();
	if(!pGradientFill) return;

	vert[0].x      = prc->left;
	vert[0].y      = prc->top;
	vert[0].Red    = (COLOR16)GetRValue(col1) * 256;
	vert[0].Green  = (COLOR16)GetGValue(col1) * 256;
	vert[0].Blue   = (COLOR16)GetBValue(col1) * 256;
	vert[0].Alpha  = 0x0000;
	vert[1].x      = prc->right;
	vert[1].y      = prc->bottom;
	vert[1].Red    = (COLOR16)GetRValue(col2) * 256;
	vert[1].Green  = (COLOR16)GetGValue(col2) * 256;
	vert[1].Blue   = (COLOR16)GetBValue(col2) * 256;
	vert[1].Alpha  = 0x0000;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	pGradientFill(hdc, vert, 2, &gRect, 1, grad);
}

void SetLayeredTaskbar(HWND hwndClock)
{
	LONG exstyle;
	HWND hwnd;
	int alpha;

	alpha = GetMyRegLong(NULL, "AlphaTaskbar", 0);
	alpha = 255 - (alpha * 255 / 100);
	if(alpha != 0 && alpha < 8) alpha = 8; else if(alpha > 255) alpha = 255;

	if(!pSetLayeredWindowAttributes && alpha < 255) InitLayeredWindow();
	if(!pSetLayeredWindowAttributes) return;

	hwnd = GetParent(GetParent(hwndClock));

	exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	if(alpha < 255) exstyle |= WS_EX_LAYERED;
	else exstyle &= ~WS_EX_LAYERED;
	SetWindowLong(hwnd, GWL_EXSTYLE, exstyle);

	RefreshRebar(hwnd);

	if(alpha == 0)
		pSetLayeredWindowAttributes(hwnd, GetSysColor(COLOR_3DFACE), 0, LWA_COLORKEY);
	else if(alpha < 255)
		pSetLayeredWindowAttributes(hwnd, 0, (BYTE)alpha, LWA_ALPHA);
}

void InitLayeredStartMenu(HWND hwndClock)
{
	EndLayeredStartMenu(hwndClock);

	nAlphaStartMenu = GetMyRegLong(NULL, "AlphaStartMenu", 0);
	nAlphaStartMenu = 255 - (nAlphaStartMenu * 255 / 100);
	if(nAlphaStartMenu != 0 && nAlphaStartMenu < 8) nAlphaStartMenu = 8;
	else if(nAlphaStartMenu > 255) nAlphaStartMenu = 255;
}

void EndLayeredStartMenu(HWND hwndClock)
{
	HWND hwnd;
	LONG exstyle;
	char classname[80];

	nAlphaStartMenu = 255;

	hwnd = GetDesktopWindow();
	hwnd = GetWindow(hwnd, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if((lstrcmpi(classname, "BaseBar") == 0 ||
			lstrcmpi(classname, "DV2ControlHost") == 0) &&
			GetWindowThreadProcessId(hwnd, NULL) ==
				GetWindowThreadProcessId(hwndClock, NULL))
		{
			exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
			if(exstyle & WS_EX_LAYERED)
			{
				exstyle &= ~WS_EX_LAYERED;
				SetWindowLong(hwnd, GWL_EXSTYLE, exstyle);
			}
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
}

void SetLayeredWindow(HWND hwnd, INT alphaTip, COLORREF colBack)
{
	LONG exstyle;

	if(!pSetLayeredWindowAttributes) InitLayeredWindow();
	if(!pSetLayeredWindowAttributes) return;

	exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	if(!(exstyle & WS_EX_LAYERED))
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, exstyle|WS_EX_LAYERED);
	}
	if(alphaTip == 0)
		pSetLayeredWindowAttributes(hwnd, colBack, 0, LWA_COLORKEY);
	else
		pSetLayeredWindowAttributes(hwnd, 0, (BYTE)alphaTip, LWA_ALPHA);
}

void SetLayeredStartMenu(HWND hwnd)
{
	SetLayeredWindow(hwnd, nAlphaStartMenu, GetSysColor(COLOR_3DFACE));
}

HRESULT MyAlphaBlend(
  HDC hdcDest,                 // handle to destination DC
  int nXOriginDest,            // x-coord of upper-left corner
  int nYOriginDest,            // y-coord of upper-left corner
  int nWidthDest,              // destination width
  int nHeightDest,             // destination height
  HDC hdcSrc,                  // handle to source DC
  int nXOriginSrc,             // x-coord of upper-left corner
  int nYOriginSrc,             // y-coord of upper-left corner
  int nWidthSrc,               // source width
  int nHeightSrc,              // source height
  BLENDFUNCTION blendFunction  // alpha-blending function
)
{
	if(!pAlphaBlend) InitAlphaBlend();
	if(!pAlphaBlend) return S_FALSE;

	return pAlphaBlend(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                       hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, blendFunction);
}

HRESULT MyTransparentBlt(
  HDC hdcDest,        // handle to destination DC
  int nXOriginDest,   // x-coord of destination upper-left corner
  int nYOriginDest,   // y-coord of destination upper-left corner
  int nWidthDest,     // width of destination rectangle
  int nHeightDest,    // height of destination rectangle
  HDC hdcSrc,         // handle to source DC
  int nXOriginSrc,    // x-coord of source upper-left corner
  int nYOriginSrc,    // y-coord of source upper-left corner
  int nWidthSrc,      // width of source rectangle
  int nHeightSrc,     // height of source rectangle
  UINT crTransparent  // color to make transparent
)
{
	if(!pTransparentBlt) InitTransparentBlt();
	if(!pTransparentBlt) return S_FALSE;

	return pTransparentBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
                       hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, crTransparent);
}

HRESULT MyDrawThemeParentBackground(HWND hwnd,HDC hdc, RECT* prc)
{
	if(!pDrawThemeParentBackground) InitDrawTheme();
	if(!pDrawThemeParentBackground) return S_FALSE;

	return pDrawThemeParentBackground(hwnd, hdc, prc);
}

/*--------------------------------------------------
    redraw ReBarWindow32 forcely
----------------------------------------------------*/
void RefreshRebar(HWND hwndBar)
{
	HWND hwnd;
	char classname[80];

	hwnd = GetWindow(hwndBar, GW_CHILD);
	while(hwnd)
	{
		GetClassName(hwnd, classname, 80);
		if(lstrcmpi(classname, "ReBarWindow32") == 0)
		{
			InvalidateRect(hwnd, NULL, TRUE);
			hwnd = GetWindow(hwnd, GW_CHILD);
			while(hwnd)
			{
				InvalidateRect(hwnd, NULL, TRUE);
				hwnd = GetWindow(hwnd, GW_HWNDNEXT);
			}
			break;
		}
		hwnd = GetWindow(hwnd, GW_HWNDNEXT);
	}
}

