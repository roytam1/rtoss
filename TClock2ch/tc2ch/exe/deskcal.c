/*-------------------------------------------
  deskcal.c
    Update Desktop Calendar automatically
    KAZUBON 1997-1999
---------------------------------------------*/

#include "tclock.h"

extern PSTR CreateFullPathName(HINSTANCE hmod, PSTR fname);

BOOL ScreenSaverExist(void);
static HANDLE hProcessDeskcal = NULL;

/*------------------------------------------------
  Execute Deskcal.exe
--------------------------------------------------*/
BOOL ExecDeskcal(HWND hwnd)
{
	char s[1024];
	char fname[MAX_PATH], option[MAX_PATH];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	PSTR path;

	if(GetMyRegLong("", "Deskcal", FALSE) == FALSE) return TRUE;

	if(hProcessDeskcal)
	{
		DWORD dwExitCode;
		if(GetExitCodeProcess(hProcessDeskcal, &dwExitCode) &&
			dwExitCode == STILL_ACTIVE)
		{
			return TRUE;
		}
	}

	if(FindWindow("TDeskcalForm", NULL)) return TRUE;

	if(ScreenSaverExist()) return FALSE;

	GetRegStr(HKEY_CURRENT_USER, "Software\\Shinonon\\Deskcal",
		"ExeFileName", s, 1024, "");
	if(s[0] == 0)
	{
		GetMyRegStr("", "DeskcalCommand", s, 1024, "");
		if(s[0] == 0) return TRUE;
	}

#if 1
	if (s[0] != '\"') {
		GetFileAndOption(s, fname, option);
		path = CreateFullPathName(g_hInst, fname);
		if (path == NULL) {
			s[0] = '\"';
			strcpy(s + 1, fname);
			strcat(s, "\" ");
			strcat(s, option);
		} else {
			s[0] = '\"';
			strcpy(s + 1, path);
			strcat(s, "\" ");
			strcat(s, option);
			free(path);
		}
	} else {
		path = CreateFullPathName(g_hInst, s);
		if (path != NULL) {
			strcpy(s, path);
			free(path);
		}
	}
#else
	if(s[0] != '\"')
	{
		GetFileAndOption(s, fname, option);
		s[0] = '\"'; strcpy(s + 1, fname);
		strcat(s, "\" "); strcat(s, option);
	}
#endif
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	hProcessDeskcal = NULL;
	if(!CreateProcess(NULL, s, NULL, NULL, FALSE, 0,
		NULL, NULL, &si, &pi))
	{
		return TRUE;
	}
	hProcessDeskcal = pi.hProcess;

	InitWatchWallpaper();

	return TRUE;
}

/*------------------------------------------------
  If screensaver is running
--------------------------------------------------*/
BOOL ScreenSaverExist(void)
{
	HWND hwnd;
	RECT rc;
	char s[80];

	hwnd = GetForegroundWindow();
	if(!hwnd) return FALSE;

	GetWindowRect(hwnd, &rc);
	if(rc.left <= 0 && rc.top <= 0 &&
		rc.right >= GetSystemMetrics(SM_CXSCREEN) &&
		rc.bottom >= GetSystemMetrics(SM_CYSCREEN))
	{
		GetClassName(hwnd, s, 80);
		if(strcmp(s, "Progman") != 0 &&
			strcmp(s, "DeskSaysNoPeekingItsASurprise") != 0)
			return TRUE;
	}
	return FALSE;
}

/*----------------------------------------
   Watch the wallpaper file
----------------------------------------*/

static char* wallpapername = NULL;
static int tilewallpaper, wallpaperstyle;
static WIN32_FIND_DATA fd_wallpaper;
static DWORD colorbackground;

void InitWatchWallpaper(void)
{
	char s[30];
	char section[] = "Control Panel\\desktop";
	HANDLE hfind;

	EndWatchWallpaper();

	if(GetMyRegLong(NULL, "WatchWallpaper", FALSE) == FALSE)
		return;

	wallpapername = (char*)malloc(MAX_PATH);
	GetRegStr(HKEY_CURRENT_USER, section, "Wallpaper",
		wallpapername, MAX_PATH, "");

	GetRegStr(HKEY_CURRENT_USER, section, "TileWallpaper", s, 30, "0");
	tilewallpaper = atoi(s);
	GetRegStr(HKEY_CURRENT_USER, section, "WallpaperStyle", s, 30, "0");
	wallpaperstyle = atoi(s);

	colorbackground = GetSysColor(COLOR_BACKGROUND);

	if(wallpapername[0])
	{
		hfind = FindFirstFile(wallpapername, &fd_wallpaper);
		if(hfind != INVALID_HANDLE_VALUE)
			FindClose(hfind);
	}
}

void EndWatchWallpaper(void)
{
	if(wallpapername) free(wallpapername);
	wallpapername = NULL;
}

void OnTimerWatchWallpaper(HWND hwnd, SYSTEMTIME* pt)
{
	char section[] = "Control Panel\\desktop";
	char s[30], fname[MAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE hfind;
	BOOL b;

	if(!wallpapername) return;

	if(GetMyRegLong(NULL, "WatchWallpaper", FALSE) == FALSE)
		return;

	b = FALSE;
	GetRegStr(HKEY_CURRENT_USER, section, "Wallpaper", fname, MAX_PATH, "");
	if(strcmp(fname, wallpapername) != 0) b = TRUE;
	GetRegStr(HKEY_CURRENT_USER, section, "TileWallpaper", s, 30, "0");
	if(tilewallpaper != atoi(s)) b = TRUE;
	GetRegStr(HKEY_CURRENT_USER, section, "WallpaperStyle", s, 30, "0");
	if(wallpaperstyle != atoi(s)) b = TRUE;

	if(colorbackground != GetSysColor(COLOR_BACKGROUND))
		b = TRUE;

	if(!b && fname[0])
	{
		hfind = FindFirstFile(wallpapername, &fd);
		if(hfind != INVALID_HANDLE_VALUE)
		{
			FindClose(hfind);
			if(fd.nFileSizeLow != fd_wallpaper.nFileSizeLow ||
				*(DWORDLONG*)&(fd.ftCreationTime) !=
					*(DWORDLONG*)&(fd_wallpaper.ftCreationTime) ||
				*(DWORDLONG*)&(fd.ftLastWriteTime) !=
					*(DWORDLONG*)&(fd_wallpaper.ftLastWriteTime) )
			{
				b = TRUE;
			}
		}
	}

	if(b)
	{
		if(FindWindow("TDeskcalForm", NULL)) return;
		ExecDeskcal(hwnd);
	}
}

