/*-------------------------------------------
  soundselect.c
  select a sound file with "Open" dialog
  KAZUBON 1997-2001
---------------------------------------------*/

#include "tclock.h"

UINT_PTR CALLBACK HookProcAlarm(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void OnInitDialog(HWND hDlg);
static void OnFileNameChanged(HWND hDlg);
static void OnTestSound(HWND hDlg);
static void GetMMFileExts(char* dst);
static BOOL bPlaying = FALSE;

// OPENFILENAME struct for Win Me/2000
typedef struct _tagOFNA {
   DWORD        lStructSize;
   HWND         hwndOwner;
   HINSTANCE    hInstance;
   LPCSTR       lpstrFilter;
   LPSTR        lpstrCustomFilter;
   DWORD        nMaxCustFilter;
   DWORD        nFilterIndex;
   LPSTR        lpstrFile;
   DWORD        nMaxFile;
   LPSTR        lpstrFileTitle;
   DWORD        nMaxFileTitle;
   LPCSTR       lpstrInitialDir;
   LPCSTR       lpstrTitle;
   DWORD        Flags;
   WORD         nFileOffset;
   WORD         nFileExtension;
   LPCSTR       lpstrDefExt;
   LPARAM       lCustData;
   LPOFNHOOKPROC lpfnHook;
   LPCSTR       lpTemplateName;
   void *       pvReserved;
   DWORD        dwReserved;
   DWORD        FlagsEx;
} _OPENFILENAMEA, *_LPOPENFILENAMEA;

/*------------------------------------------------
   open dialog to browse sound files
--------------------------------------------------*/
BOOL BrowseSoundFile(HWND hDlg, const char *deffile, char *fname, WORD id)
{
	_OPENFILENAMEA ofn;
	OSVERSIONINFO osver;
	char filter[1024], mmfileexts[1024];
	char ftitle[MAX_PATH], initdir[MAX_PATH];

	memset(&ofn, '\0', sizeof(_OPENFILENAMEA));

	filter[0] = filter[1] = 0;
	if (id != IDC_SANSHOAUTOEXEC)
	{
		str0cat(filter, MyString(IDS_MMFILE));
		GetMMFileExts(mmfileexts);
		str0cat(filter, mmfileexts);
	}
	else
	{
		str0cat(filter, MyString(IDS_PROGRAMFILE));
		str0cat(filter, "*.exe");
	}
	str0cat(filter, MyString(IDS_ALLFILE));
	str0cat(filter, "*.*");

	if(deffile[0] == 0 || IsMMFile(deffile))
		ofn.nFilterIndex = 1;
	else ofn.nFilterIndex = 2;

	strcpy(initdir, g_mydir);
	if(deffile[0])
	{
		WIN32_FIND_DATA fd;
		HANDLE hfind;
		hfind = FindFirstFile(deffile, &fd);
		if(hfind != INVALID_HANDLE_VALUE)
		{
			FindClose(hfind);
			strcpy(initdir, deffile);
			del_title(initdir);
		}
	}

	fname[0] = 0;

	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx(&osver);
	if((osver.dwPlatformId == VER_PLATFORM_WIN32_NT &&  // Win 2000
			osver.dwMajorVersion == 5)
	 ||(osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS &&  // Win Me
			osver.dwMinorVersion == 90))
	{
		ofn.lStructSize = sizeof(_OPENFILENAMEA);
	}
	else ofn.lStructSize = sizeof(OPENFILENAME);

	ofn.hwndOwner = hDlg;
	ofn.hInstance = g_hInstResource;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile= fname;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = ftitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = initdir;
	ofn.lpfnHook = HookProcAlarm;
	ofn.lpTemplateName = MAKEINTRESOURCE(IDD_TESTSOUND);
	ofn.Flags = OFN_HIDEREADONLY|OFN_EXPLORER|OFN_FILEMUSTEXIST|
		OFN_ENABLEHOOK| OFN_ENABLETEMPLATE;

	return GetOpenFileName((LPOPENFILENAME)&ofn);
}

/*------------------------------------------------
　MCIファイルかどうか
--------------------------------------------------*/
BOOL IsMMFile(const char* fname)
{
	char s[1024], *sp;

	if(lstrcmpi(fname, "cdaudio") == 0) return TRUE;

	GetProfileString("mci extensions", NULL, "",
		s, 1024);

	sp = s;
	while(*sp)
	{
		if(ext_cmp(fname, sp) == 0) return TRUE;
		while(*sp) sp++; sp++;
	}
	return FALSE;
}

/*------------------------------------------------
　コモンダイアログのフックプロシージャ
　参考：Visual C++ 4.xのサンプルCOMDLG32
--------------------------------------------------*/
UINT_PTR CALLBACK HookProcAlarm(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			OnInitDialog(hDlg);
			break;
		case WM_DESTROY:
			if(bPlaying) StopFile(); bPlaying = FALSE;
			break;
		case WM_NOTIFY:
			switch(((LPOFNOTIFY)lParam)->hdr.code)
			{
				case CDN_SELCHANGE:
				case CDN_FOLDERCHANGE:
					OnFileNameChanged(hDlg);
				break;
			}
			return FALSE;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDC_TESTSOUND)
				OnTestSound(hDlg);
			return FALSE;
		case MM_MCINOTIFY:
		case MM_WOM_DONE:
			StopFile(); bPlaying = FALSE;
			SendDlgItemMessage(hDlg, IDC_TESTSOUND, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)g_hIconPlay);
			InvalidateRect(GetDlgItem(hDlg, IDC_TESTSOUND), NULL, FALSE);
			return FALSE;
		default:
			return FALSE;
	}
	return TRUE;
}

void OnInitDialog(HWND hDlg)
{
	HWND hwndStatic;
	RECT rc1, rc2;
	POINT pt;
	int dx;

	SendDlgItemMessage(hDlg, IDC_TESTSOUND, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconPlay);
	EnableDlgItem(hDlg, IDC_TESTSOUND, FALSE);

	bPlaying = FALSE;

	// find "File Name:" Label
	hwndStatic = GetDlgItem(GetParent(hDlg), 0x442);
	if(hwndStatic == NULL) return;
	GetWindowRect(hwndStatic, &rc1);
	// move "Test:" Label
	GetWindowRect(GetDlgItem(hDlg, IDC_LABTESTSOUND), &rc2);
	dx = rc1.left - rc2.left;
	pt.x = rc2.left + dx; pt.y = rc2.top;
	ScreenToClient(hDlg, &pt);
	SetWindowPos(GetDlgItem(hDlg, IDC_LABTESTSOUND), NULL, pt.x, pt.y, 0, 0,
		SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	// move play button
	GetWindowRect(GetDlgItem(hDlg, IDC_TESTSOUND), &rc2);
	pt.x = rc2.left + dx; pt.y = rc2.top;
	ScreenToClient(hDlg, &pt);
	SetWindowPos(GetDlgItem(hDlg, IDC_TESTSOUND), NULL, pt.x, pt.y, 0, 0,
		SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}

void OnFileNameChanged(HWND hDlg)
{
	char fname[MAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE hfind = INVALID_HANDLE_VALUE;
	BOOL b = FALSE;

	if (CommDlg_OpenSave_GetFilePath(GetParent(hDlg),
		fname, sizeof(fname)) <= sizeof(fname))
	{
		hfind = FindFirstFile(fname, &fd);
		if(hfind != INVALID_HANDLE_VALUE)
		{
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				b = TRUE;
			FindClose(hfind);
		}
	}
	EnableDlgItem(hDlg, IDC_TESTSOUND, b);
}

void OnTestSound(HWND hDlg)
{
	char fname[MAX_PATH];

	if(CommDlg_OpenSave_GetFilePath(GetParent(hDlg),
			fname, sizeof(fname)) <= sizeof(fname))
	{
		if((HICON)SendDlgItemMessage(hDlg, IDC_TESTSOUND,
			BM_GETIMAGE, IMAGE_ICON, 0) == g_hIconPlay)
		{
			if(PlayFile(hDlg, fname, 0))
			{
				SendDlgItemMessage(hDlg, IDC_TESTSOUND,
					BM_SETIMAGE, IMAGE_ICON, (LPARAM)g_hIconStop);
				InvalidateRect(GetDlgItem(hDlg, IDC_TESTSOUND), NULL, FALSE);
				bPlaying = TRUE;
			}
		}
		else
		{
			StopFile(); bPlaying = FALSE;
		}
	}
}

/*------------------------------------------------
　WIN.INIからMCIファイルの拡張子を取得
--------------------------------------------------*/
void GetMMFileExts(char* dst)
{
	char s[1024], *sp, *dp;

	GetProfileString("mci extensions", NULL, "",
		s, 1024);

	sp = s; dp = dst;
	while(*sp)
	{
		if(dp != dst) *dp++ = ';';
		*dp++ = '*'; *dp++ = '.';
		while(*sp) *dp++ = *sp++;
		sp++;
	}
	*dp = 0;
}

