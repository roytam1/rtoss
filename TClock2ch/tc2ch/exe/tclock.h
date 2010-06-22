/*-------------------------------------------
  tclock.h
      KAZUBON  1997-1999
---------------------------------------------*/

#define _WIN32_IE 0x0500
#define WINVER    0x0400

#define _CRT_SECURE_NO_DEPRECATE

#pragma warning(push, 0)
# include <windows.h>
# include <windowsx.h>
# include <tchar.h>
# include <mmsystem.h>
# include <shlobj.h>
#pragma warning(pop)

#include "resource.h"
#include "../language/resource.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "wsock32.lib")

#ifdef _WIN64
# if defined(_DEBUG)
#  pragma comment(lib, "../../x64/debug/tcdll.lib")
# else
#  pragma comment(lib, "../../x64/release/tcdll.lib")
# endif // _DEBUG
#else
# if defined(_DEBUG)
#  pragma comment(lib, "../../debug/tcdll.lib")
# else
#  pragma comment(lib, "../../release/tcdll.lib")
# endif // _DEBUG
#endif  // _WIN64


// replacement of standard library's functions
#define malloc(s) HeapAlloc(GetProcessHeap(), 0, s)
#define free(p)   HeapFree(GetProcessHeap(), 0, p);

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma intrinsic(memcpy, memset, strlen, strcpy, strcat)
#else
# undef memcpy
# define memcpy(d,s,l) r_memcpy(d,s,l)
# undef memset
# define memset(d,c,l) r_memset(d,c,l)
# undef strlen
# define strlen(s) lstrlen(s)
# undef strcpy
# define strcpy(d, s) lstrcpy(d, s)
# undef strcat
# define strcat(d, s) lstrcat(d, s)
void r_memcpy(void *d, const void *s, size_t l);
void r_memset(void *d, int c, size_t l);
#endif

#undef atoi
#define atoi(p) r_atoi(p)
#undef _strnicmp
#define _strnicmp(d,s,n) r_strnicmp(d,s,n)
#undef _stricmp
#define _stricmp(d,s) r_stricmp(d,s)
int r_atoi(const char *p);
int r_strnicmp(const char* d, const char* s, size_t n);
int r_stricmp(const char* d, const char* s);

// IDs for timer
#define IDTIMER_START       2
#define IDTIMER_MAIN        3
#define IDTIMER_MOUSE       4
#define IDTIMER_DEKSTOPICON 5
#define IDTIMER_MONOFF      6
#define IDTIMER_CREATE      7

// messages to send the clock
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

// for mouse.c and pagemouce.c
#define MOUSEFUNC_NONE       -1
#define MOUSEFUNC_DATETIME    0
#define MOUSEFUNC_EXITWIN     1
#define MOUSEFUNC_RUNAPP      2
#define MOUSEFUNC_MINALL      3
#define MOUSEFUNC_SYNCTIME    4
#define MOUSEFUNC_TIMER       5
#define MOUSEFUNC_CLIPBOARD   6
#define MOUSEFUNC_SCREENSAVER 7
#define MOUSEFUNC_KYU         8
#define MOUSEFUNC_DELRECDOCS  9
#define MOUSEFUNC_PROPERTY    10
#define MOUSEFUNC_CALENDAR    11
#define MOUSEFUNC_STARTMENU   12
#define MOUSEFUNC_TASKSW      13
#define MOUSEFUNC_SHOWDESK    14
#define MOUSEFUNC_LOCKPC      15
#define MOUSEFUNC_MENU        16
#define MOUSEFUNC_CHANGECONF  17
#define MOUSEFUNC_VOLMUTE     18
#define MOUSEFUNC_VOLSET      19
#define MOUSEFUNC_VOLUD       20
#define MOUSEFUNC_MONOFF      21
#define MOUSEFUNC_CDOPEN      22
#define MOUSEFUNC_CDCLOSE     23
#define MOUSEFUNC_NETINIT     24
#define MOUSEFUNC_DELUS       25
#define MOUSEFUNC_FILELIST    26
#define MOUSEFUNC_OPENFILE    100

typedef struct{
	BYTE	key;
	DWORD	flag;
} KEYEVENT,*LPKEYEVENT;

typedef struct{
	signed short mousefunc;
	signed short idstring;
	char *ttbase_name;
} MOUSE_FUNC_INFO;

// exemain.c
extern HINSTANCE g_hInst;           // instance handle
extern HINSTANCE g_hInstResource;   // instance handle of language module
extern HWND      g_hwndMain;        // main window
extern HWND      g_hwndClock;       // clock window
extern HWND      g_hwndPropDlg;     // property sheet window
extern HWND      g_hDlgTimer;       // timer dialog
extern HWND      g_hDlgCalender;    // calender dialog
extern HICON     g_hIconTClock, g_hIconPlay, g_hIconStop, g_hIconDel;
                                    // icons to use frequently
extern char      g_mydir[];         // path to tclock.exe
extern char      g_langdllname[];   // language dll name
extern BOOL      g_bIniSetting;
extern char      g_inifile[];
MOUSE_FUNC_INFO *GetMouseFuncList(void);
int GetMouseFuncCount(void);
HINSTANCE LoadLanguageDLL(char *fname); // load language dll
HINSTANCE GetLangModule(void);      // instance handle of language dll
void MyHelp(HWND hDlg, int id);     // show help
void My2chHelp(HWND hDlg, int id);  // show 2ch help

// propsheet.c
extern BOOL g_bApplyClock;
extern BOOL g_bApplyTaskbar;
extern BOOL g_bApplyLangDLL;
void MyPropertyDialog(void);
BOOL SelectMyFile(HWND hDlg, const char *filter, DWORD nFilterIndex,
	const char *deffile, char *retfile);

// alarm.c
void InitAlarm(void);
void OnTimerAlarm(HWND hwnd, SYSTEMTIME* st);
void EndAlarm(void);
void PlayFilePre(HWND hwnd, char *fname);
BOOL PlayFile(HWND hwnd, char *fname, DWORD dwLoops);
void StopFile(void);
void OnMCINotify(HWND hwnd);
void GetFileAndOption(const char* command, char* fname, char* opt);
BOOL ExecFile(HWND hwnd, char* command);

// alarmday.c
int SetAlarmDay(HWND hDlg, int n);

// autoexecday.c
int SetAutoExecDay(HWND hDlg, int n);

// autoexechour.c
int SetAutoExecHour(HWND hDlg, int n);

// autoexecmin.c
void SetAutoExecMin(HWND hDlg, int nOld1, int nOld2, int* nNew1, int* nNew2);

// soundselect.c
BOOL BrowseSoundFile(HWND hDlg, const char *deffile, char *fname, WORD id);
BOOL IsMMFile(const char* fname);

// pageformat.c
void InitFormat(void);
void CreateFormat(char* s, int* checks);

// menu.c
void OnContextMenu(HWND hwnd, HWND hwndClicked, int xPos, int yPos);
void OnTClockCommand(HWND hwnd, WORD wID, WORD wCode);

// mouse.c
void InitMouseFunction(HWND hwnd);
void EndMouseFunction(HWND hwnd);
void ResetHotkey(HWND hwnd);
void OnHotkey(HWND hwnd, int id);
void ExecuteMouseFunction(HWND hwnd, LONG fnc, int btn, int clk);
void OnDropFiles(HWND hwnd, HDROP hdrop);
void OnMouseMsg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnTimerMouse(HWND hwnd);
void PushKeybd(LPKEYEVENT lpkey);

// deskcal.c
BOOL ExecDeskcal(HWND hwnd);
BOOL ExecFile(HWND hwnd, char* command);
void OnDeskcalTimer(HWND hwnd);
void InitWatchWallpaper(void);
void EndWatchWallpaper(void);
void OnTimerWatchWallpaper(HWND hwnd, SYSTEMTIME* pt);

// calendar.c
HWND FindVistaCalenderWindow(void);
void DialogCalender(HWND hwnd);

// timer.c
#define MAX_TIMER  20

void DialogTimer(HWND hwnd);
void OnTimerTimer(HWND hwnd, SYSTEMTIME* st);
void EndTimer(void);
int GetTimerInfo(char *dst, int num);
void StopTimer(HWND hwnd, int n);

// sntp.c
BOOL InitSyncTime(HWND hwnd);
void InitSyncTimeSetting(void);
void EndSyncTime(HWND hwnd);
void OnTimerSNTP(HWND hwnd, SYSTEMTIME* st);
void StartSyncTime(HWND hwndParent, char* pServer, int nto);

// mixer.c
BOOL InitMixer(void);
void ReleaseMixer(void);
BOOL GetMasterVolume(int *Val);
BOOL SetMasterVolume(int Val);
BOOL UpDownMasterVolume(int dif);
BOOL GetMasterMute(BOOL *Val);
BOOL SetMasterMute(BOOL Val);
BOOL ReverseMasterMute(void);

// utl.c
int atox(const char *p);
void add_title(char *path, char* titile);
void del_title(char *path);
void get_title(char* dst, const char *path);
int ext_cmp(const char *fname, const char *ext);
void parse(char *dst, char *src, int n);
void parsechar(char *dst, char *src, char ch, int n);
void str0cat(char* dst, const char* src);
char* MyString(UINT id);
int MyMessageBox(HWND hwnd, char* msg, char* title, UINT uType, UINT uBeep);
int GetLocaleInfoWA(int ilang, LCTYPE LCType, char* dst, int n);
DWORDLONG M32x32to64(DWORD a, DWORD b);
void SetForegroundWindow98(HWND hwnd);
void WriteDebug(const char* s);
int GetMyRegStr(char* section, char* entry, char* val, int cbData,
	char* defval);
LONG GetMyRegLong(char* section, char* entry, LONG defval);
int GetMyRegStrEx(char* section, char* entry, char* val, int cbData,
	char* defval, int confno);
LONG GetMyRegLongEx(char* section, char* entry, LONG defval, int confno);
LONG GetRegLong(HKEY rootkey, char*subkey, char* entry, LONG defval);
int GetRegStr(HKEY rootkey, char*subkey, char* entry,
	char* val, int cbData, char* defval);
BOOL SetMyRegStr(char* subkey, char* entry, char* val);
BOOL SetMyRegStrEx(char* subkey, char* entry, char* val, int confno);
BOOL SetRegStr(HKEY rootkey, char* subkey, char* entry, char* val);
BOOL SetMyRegLong(char* subkey, char* entry, DWORD val);
BOOL SetMyRegLongEx(char* subkey, char* entry, DWORD val, int confno);
BOOL DelMyReg(char* subkey, char* entry);
BOOL DelMyRegKey(char* subkey);
int DelRegAll(void);
void MoveRegToIni(char *section);
void AdjustConboBoxDropDown(HWND hComboBox, int nDropDownCount);

// tcdll.dllのAPI
void WINAPI HookStart(HWND hwnd);
void WINAPI HookEnd(void);

// Macros

#define EnableDlgItem(hDlg,id,b) EnableWindow(GetDlgItem((hDlg),(id)),(b))
#define ShowDlgItem(hDlg,id,b) ShowWindow(GetDlgItem((hDlg),(id)),(b)?SW_SHOW:SW_HIDE)
#define AdjustDlgConboBoxDropDown(hDlg,id,b) AdjustConboBoxDropDown(GetDlgItem((hDlg),(id)),(b))

#define CBAddString(hDlg,id,lParam) SendDlgItemMessage((hDlg),(id),CB_ADDSTRING,0,(lParam))
#define CBDeleteString(hDlg,id, i) SendDlgItemMessage((hDlg),(id),CB_DELETESTRING,(i),0)
#define CBFindString(hDlg,id,s) SendDlgItemMessage((hDlg),(id),CB_FINDSTRING,0,(LPARAM)(s))
#define CBFindStringExact(hDlg,id,s) SendDlgItemMessage((hDlg),(id),CB_FINDSTRINGEXACT,0,(LPARAM)(s))
#define CBGetCount(hDlg,id) SendDlgItemMessage((hDlg),(id),CB_GETCOUNT,0,0)
#define CBGetCurSel(hDlg,id) SendDlgItemMessage((hDlg),(id),CB_GETCURSEL,0,0)
#define CBGetItemData(hDlg,id,i) SendDlgItemMessage((hDlg),(id),CB_GETITEMDATA,(i),0)
#define CBGetLBText(hDlg,id,i,s) SendDlgItemMessage((hDlg),(id),CB_GETLBTEXT,(i),(LPARAM)(s))
#define CBInsertString(hDlg,id,i,s) SendDlgItemMessage((hDlg),(id),CB_INSERTSTRING,(i),(LPARAM)(s))
#define CBResetContent(hDlg,id) SendDlgItemMessage((hDlg),(id),CB_RESETCONTENT,0,0)
#define CBSetCurSel(hDlg,id,i) SendDlgItemMessage((hDlg),(id),CB_SETCURSEL,(i),0)
#define CBSetItemData(hDlg,id,i,lParam) SendDlgItemMessage((hDlg),(id),CB_SETITEMDATA,(i),(lParam))

// アラーム用構造体
typedef struct _tagAlarmStruct
{
	char name[40];
	BOOL bAlarm;
	int hour;
	int minute;
	char fname[1024];
	BOOL bHour12;
	BOOL bRepeat;
	BOOL bBlink;
	int days;
} ALARMSTRUCT;
typedef ALARMSTRUCT* PALARMSTRUCT;

// 自動実行用構造体
typedef struct _tagAutoExecStruct
{
	char name[40];
	BOOL bAutoExec;
	int hour;
	int minute1;
	int minute2;
	char fname[1024];
	int days;
	BOOL bOnBoot;
} AUTOEXECSTRUCT;
typedef AUTOEXECSTRUCT* PAUTOEXECSTRUCT;

// page.c
typedef struct {
	WORD id;
	char *colname;
	COLORREF defcol;
} COMBOCOLOR;

void InitComboColor(HWND hDlg, int numcol, COMBOCOLOR *pCombocolor, int maxcol, BOOL ex_flg);
void OnMeasureItemColorCombo(LPARAM lParam);
void OnDrawItemColorCombo(LPARAM lParam, int maxcol);
void OnChooseColor(HWND hDlg, WORD id, int maxcol);

// filelist.c
void OnMeasureItem(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnDrawItem(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnInitMenuPopup(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnMenuRButtonUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
void showUserMenu(HWND hwnd, HWND hwndClicked, int xPos, int yPos, int btn, int clk);
