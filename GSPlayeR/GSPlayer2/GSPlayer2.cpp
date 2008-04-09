#include "GSPlayer2.h"
#include "PlayerApp.h"

CPlayerApp* pApp = NULL;

const TCHAR s_szDefSupportedExt[][5] = {
	_T("rmp"), _T("mp3"), _T("mp2"),
	_T("mpa"), _T("ogg"), _T("wav"),
	NULL
};

const TCHAR s_szDefSupportedExt2[][5] = {
	_T("rmp"), _T("mp3"), _T("mp2"),
	_T("mpa"), _T("ogg"), _T("wav"),
	_T("m3u"),_T("m3u8"), _T("pls"),
	NULL
};

#if defined(_UNICODE) && !defined(_WIN32_WCE)
int WINAPI wWinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
#else
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
#endif
{
	pApp = GetPlayerAppClass();
	int nRet = pApp->Run(hInstance, lpCmdLine);
	delete pApp;
	return nRet;
}

HINSTANCE GetInst()
{
	if (!pApp)
		return NULL;

	return pApp->GetInst();
}

void SendCmdLine(HWND hwndTo, LPTSTR pszCmdLine)
{
	if (!_tcslen(pszCmdLine))
		return;

	LPTSTR psz = pszCmdLine;
#ifdef _WIN32_WCE
	DWORD dwSize = _tcslen(pszCmdLine) + 2;
	LPTSTR pszBuff = new TCHAR[dwSize];
	LPTSTR pszFiles = pszBuff;
	memset(pszBuff, 0, sizeof(TCHAR) * dwSize);
#else
	HANDLE hMem = LocalAlloc(LMEM_ZEROINIT | LMEM_FIXED, 
			sizeof(DROPFILES) + sizeof(TCHAR) * (_tcslen(pszCmdLine) + 2));
	DROPFILES* p = (DROPFILES*)LocalLock(hMem);
	p->pFiles = sizeof(DROPFILES);
	p->fNC = FALSE;
	p->pt.x = 10;
	p->pt.y = 10;
#if defined(UNICODE) || defined(_UNICODE)
	p->fWide = TRUE;
#else
	p->fWide = FALSE;
#endif
	LPTSTR pszFiles = (LPTSTR)((LPBYTE)p + sizeof(DROPFILES));
#endif
	while (TRUE) {
		if (*psz == _T('\"')) {
			psz++;
			while (*psz != NULL && *psz != _T('\"')) {
				*pszFiles++ = *psz++;
			}
			if (*psz == NULL || *psz++ == NULL)
				break;

			if (*psz == _T('\"'))
				psz++;
			if (*psz == _T(' '))
				psz++;
		}
		else {
			while (*psz != NULL && *psz != _T(' ')) {
				*pszFiles++ = *psz++;
			}
			if (*psz == NULL || *psz++ == NULL)
				break;
			if (*psz == _T(' '))
				psz++;
		}
		*pszFiles++ = NULL;
	}

#ifdef _WIN32_WCE
	COPYDATASTRUCT cds;
	cds.dwData = WM_DROPFILES;
	cds.lpData = pszBuff;
	cds.cbData = sizeof(TCHAR) * dwSize;
	SendMessage(hwndTo, WM_COPYDATA, NULL, (LPARAM)&cds);
	delete [] pszBuff;
#else
	LocalUnlock(hMem);
	PostMessage(hwndTo, WM_DROPFILES, (WPARAM)hMem, 0);
#endif
}

BOOL DefDlgPaintProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32_WCE_PPC
	if (GetSystemMetrics(SM_CXSCREEN) != 240 || GetSystemMetrics(SM_CYSCREEN) != 320)
		return FALSE;

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hDlg, &ps);
	POINT pt[2] = {{0, 24},{GetSystemMetrics(SM_CXSCREEN),24}};
	Polyline(hDC, pt, sizeof(pt)/sizeof(POINT));
	EndPaint(hDlg, &ps);
	return TRUE;
#else
	return FALSE;
#endif
}

BOOL DefDlgCtlColorStaticProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32_WCE_PPC
	HDC hDC = (HDC)wParam;
	if (GetDlgCtrlID((HWND)lParam) == IDC_TITLE) {
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHT));
		return (long)GetStockObject(WHITE_BRUSH);
	}
#endif
	return FALSE;
}

void ShellInitDialog(HWND hwndDlg)
{
#ifdef _WIN32_WCE_PPC
	SHINITDLGINFO shidi;
	shidi.dwMask = SHIDIM_FLAGS;
	shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLGFULLSCREEN;
	shidi.hDlg = hwndDlg;
	SHInitDialog(&shidi);
#endif
}

int CALLBACK PropSheetCallback(HWND hwndDlg, UINT message, LPARAM lParam)
{
#ifdef _WIN32_WCE_PPC
	switch(message) {
		case PSCB_INITIALIZED:
		{
			HWND hwndChild = GetWindow(hwndDlg, GW_CHILD);
			while (hwndChild) {
				TCHAR szTemp[32];
				GetClassName(hwndChild, szTemp, 32);
				if (_tcscmp(szTemp, _T("SysTabControl32"))==0)
					break;
				hwndChild = GetWindow(hwndChild, GW_HWNDNEXT);
			}
			if (hwndChild) {
				DWORD dwStyle = GetWindowLong(hwndChild, GWL_STYLE) | TCS_BOTTOM;
				::SetWindowLong(hwndChild, GWL_STYLE, dwStyle);
			}
			break;
		}
		case PSCB_GETVERSION:
			return COMCTL32_VERSION;
	}
#else
	switch(message) {
		case PSCB_INITIALIZED:
			SetWindowLong(hwndDlg, GWL_EXSTYLE, GetWindowLong(hwndDlg, GWL_EXSTYLE) & ~WS_EX_CONTEXTHELP);
			break;
	}
#endif
	return 0;
}

BOOL GetKeyString(FILE* fp, LPCTSTR pszSection, LPCTSTR pszKey, LPTSTR pszValue)
{
	// GetPrivateProfileString‚Ý‚½‚¢‚È‚à‚Ì
	BOOL fFound = FALSE;
	char szName[MAX_PATH];
	char szBuff[MAX_PATH];

	pszValue[0] = NULL;
	if (!fp) return FALSE;

	fseek(fp, 0, SEEK_SET);

	// section‚ð’T‚·
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, NULL, pszSection, -1, szName, MAX_PATH, NULL, NULL);
#else
	strcpy(szName, pszSection);
#endif
	while (fgets(szBuff, MAX_PATH, fp)) {
		if (szBuff[0] == '[') {
			char* p = strchr(szBuff, ']');
			if (p) *p = NULL;
			if (strcmp(szBuff + 1, szName) == 0) {
				fFound = TRUE; break;
			}
		}
	}
	if (!fFound)
		return FALSE;

	// key‚ð’T‚·
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, NULL, pszKey, -1, szName, MAX_PATH, NULL, NULL);
#else
	strcpy(szName, pszKey);
#endif
	while (fgets(szBuff, MAX_PATH, fp)) {
		if (szBuff[0] == '[')
			break;
		if (szBuff[0] == ';')
			continue;
		char* p = strchr(szBuff, '=');
		if (!p)
			continue;

		*p = NULL;
		if (strcmp(szBuff, szName) == 0) {
			// Œ©‚Â‚©‚Á‚½I
#ifdef _UNICODE
			MultiByteToWideChar(CP_ACP, NULL, p + 1, -1, pszValue, MAX_PATH);
#else
			strcpy(pszValue, p + 1);
#endif	
			LPTSTR psz = pszValue + _tcslen(pszValue) - 1;
			if (*psz == _T('\n')) *psz = NULL;
			while (pszValue[_tcslen(pszValue) - 1] == ' ')
				pszValue[_tcslen(pszValue) - 1] = NULL;
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef _WIN32_WCE_PPC
BOOL IsLandscape()
{
	return GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN);
}
#endif

BOOL IsDefSupportedExt(LPTSTR pszExt, BOOL fPlayList)
{
	int i = 0;
	if (fPlayList) {
		while (*s_szDefSupportedExt2[i] != NULL) {
			if (_tcsicmp(s_szDefSupportedExt2[i], pszExt) == 0)
				return TRUE;
			i++;
		}
	}
	else {
		while (*s_szDefSupportedExt[i] != NULL) {
			if (_tcsicmp(s_szDefSupportedExt2[i], pszExt) == 0)
				return TRUE;
			i++;
		}
	}
	return FALSE;
}