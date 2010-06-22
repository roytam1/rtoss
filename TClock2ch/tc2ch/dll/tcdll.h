/*-------------------------------------------
Å@TCDLL.H
Å@KAZUBON 1997-2001
---------------------------------------------*/

#define _WIN32_IE 0x0500
#define WINVER    0x0400

#define _CRT_SECURE_NO_DEPRECATE

#pragma warning(push, 0)
# include <windows.h>
# include <windowsx.h>
# include <tchar.h>
# include <winreg.h>
# include <shellapi.h>
# include <winnls.h>
# include <commctrl.h>
# include <shlwapi.h>
# include <shlobj.h>
#pragma warning(pop)

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

#if defined(_DEBUG)
# include <crtdbg.h>
#else
# define _ASSERTE(expr) ((void)0)
#endif

#ifdef _WIN64
# pragma comment(linker, "/base:0x0000070066060000")
#else
# pragma comment(linker,         "/base:0x66060000")
#endif // _WIN64

//#if defined(NDEBUG)
//# pragma comment(linker, "/entry:\"DllMain\"")
//# pragma comment(linker, "/nodefaultlib:libcmt.lib")
//#endif

#define ENABLE_CHECK_SUBCLASS_NESTING 1

#define CLOCKM_REFRESHCLOCK   (WM_USER+1)
#define CLOCKM_REFRESHTASKBAR (WM_USER+2)
#define CLOCKM_BLINK          (WM_USER+3)
#define CLOCKM_COPY           (WM_USER+4)
#define CLOCKM_REFRESHDESKTOP (WM_USER+5)
#define CLOCKM_VOLCHANGE      (WM_USER+6)
#define CLOCKM_NETINIT        (WM_USER+7)
#define CLOCKM_DELUSRSTR      (WM_USER+8)

#define CLOCKM_VISTACALENDAR  (WM_USER+102)
#define CLOCKM_VISTATOOLTIP   (WM_USER+103)

#define AC_SRC_ALPHA                0x01

typedef struct{
  char u_str[100];
} USRSTR;

// tclock.c
void DrawClock(HWND hwnd, HDC hdc);
void FillClock(HWND hwnd, HDC hdc, RECT *prc, int nblink);
extern BOOL bNoClock;
extern BOOL bWinXP;
extern BOOL bWin2000;

// utl.c
extern BOOL g_bIniSetting;
extern char g_inifile[];
int _strncmp(const char* d, const char* s, size_t n);

HFONT CreateMyFont(char* fontname, int fontsize,
	LONG weight, LONG italic);
int ext_cmp(char *fname, char *ext);
void add_title(char *path, char *title);
void del_title(char *path);
void parse(char *dst, char *src, int n);
char* MyString(UINT id);
int GetMyRegStr(char* section, char* entry, char* val, int cbData,
	char* defval);
int GetMyRegStrEx(char* section, char* entry, char* val, int cbData,
	char* defval, int confno);
LONG GetMyRegLong(char* section, char* entry, LONG defval);
LONG GetMyRegLongEx(char* section, char* entry, LONG defval, int confno);
LONG GetRegLong(HKEY rootkey, char*subkey, char* entry, LONG defval);
int GetRegStr(HKEY rootkey, char*subkey, char* entry,
	char* val, int cbData, char* defval);
BOOL SetMyRegStr(char* subkey, char* entry, char* val);
BOOL SetMyRegLong(char* subkey, char* entry, DWORD val);
BOOL SetMyRegStrEx(char* subkey, char* entry, char* val, int confno);
BOOL SetMyRegLongEx(char* subkey, char* entry, DWORD val, int confno);
void WriteDebug(LPSTR s);

// FORMAT.C
void InitFormat(SYSTEMTIME* lt);
void MakeFormat(char* s, SYSTEMTIME* pt, int beat100, char* fmt);
#define FORMAT_SECOND    0x0001
#define FORMAT_SYSINFO   0x0002
#define FORMAT_BEAT1     0x0004
#define FORMAT_BEAT2     0x0008
#define FORMAT_BATTERY   0x0010
#define FORMAT_MEMORY    0x0020
#define FORMAT_MOTHERBRD 0x0040
#define FORMAT_PERMON    0x0080
#define FORMAT_NET       0x0100
#define FORMAT_HDD       0x0200
#define FORMAT_CPU       0x0400
#define FORMAT_VOL       0x0800
DWORD FindFormat(char* fmt);

// DESKTOP.C
void SetDesktopIcons(void);
void EndDesktopIcons(void);

// STARTBTN.C
void SetStartButton(HWND hwnd);
void EndStartButton(void);
//void SetStartButtonBmp(void);   // warning C4211: nonstandard extension used : redefined extern to static
void CheckCursorOnStartButton(void);

// STARTMENU.C
void SetStartMenu(HWND hwnd);
void EndStartMenu(void);
void InitStartMenuIE4(void);
void ClearStartMenuIE4(void);

// taskswitch.c
void InitTaskSwitch(HWND hwndClock);
void EndTaskSwitch(void);
void InitTaskBarBorder(HWND hwndClock);
void EndTaskBarBorder(void);

// traynotifywnd.c
void InitTrayNotify(HWND hwnd);
void EndTrayNotify(void);

// BMP.C
HBITMAP ReadBitmap(HWND hwnd, char* fname, BOOL b);

// newapi.c
void EndNewAPI(HWND hwnd);
void SetLayeredWindow(HWND hwnd, INT alphaTip, COLORREF colBack);
void SetLayeredTaskbar(HWND hwndClock);
void InitLayeredStartMenu(HWND hwndClock);
void SetLayeredStartMenu(HWND hwnd);
void SetLayeredClock(HWND hwndClock, COLORREF col);
void GradientFillClock(HDC hdc, RECT* prc, COLORREF col1, COLORREF col2, DWORD grad);
HRESULT MyAlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
  HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, BLENDFUNCTION blendFunction);
HRESULT MyTransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
  HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, UINT crTransparent);
HRESULT MyDrawThemeParentBackground(HWND hwnd,HDC hdc, RECT* prc);

// sysres.c
int GetFreeSystemResources(WORD wSysRes);
void EndSysres(void);

void InitBatteryLife(void);
int GetBatteryLifePercent(void);
void FreeBatteryLife(void);

void InitCpuClock(void);
void UpdateCpuClock(void);
void FreeCpuClock(void);

// cpu.c
void CpuMoni_start(void);
int CpuMoni_get(void);
void CpuMoni_end(void);

// mbm.c
int MBMSharedDataUpdate(void);

// permon.c
void PerMoni_start(void);
int PerMoni_get(void);
void PerMoni_end(void);

// net.c
void Net_start(void);
void Net_restart(void);
void Net_getRecvSend(double*,double*);
void Net_get(void);
void Net_end(void);

// hdd.c
void Hdd_get(void);

// mixer.c
BOOL InitMixer(void);
void ReleaseMixer(void);
BOOL GetMasterVolume(int *Val);
BOOL GetMasterMute(BOOL *Val);

/* tooltip.c */
void TooltipInit(HWND hwnd);
void TooltipReadData(void);
void TooltipDeleteRes(void);
void TooltipEnd(HWND hwnd);
DWORD TooltipFindFormat(void);
void TooltipOnTimer(HWND hwnd);
void TooltipOnRefresh(HWND hwnd);
BOOL TooltipOnNotify(LRESULT *plRes, LPARAM lParam);
void TooltipOnMouseEvent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
