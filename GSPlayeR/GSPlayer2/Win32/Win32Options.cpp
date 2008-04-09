#include "GSPlayer2.h"
#include "Win32Options.h"
#define	NAME_WINDOW_ALPHA	_T("Window Alpha")

CWin32Options::CWin32Options()
{
	m_nWndAlpha = 255;
}

CWin32Options::~CWin32Options()
{
}

void CWin32Options::Save(HANDLE hMap)
{
	// ベースクラス呼び出し
	COptions::Save(hMap);

	// 書き込み
	HKEY hKey = 0;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, KEY_SETTINGS, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {
		RegSetValueEx(hKey, NAME_WINDOW_ALPHA, 0, REG_DWORD, (LPBYTE)&m_nWndAlpha, sizeof(DWORD));
		RegCloseKey(hKey);
	}
}

void CWin32Options::Load(HANDLE hMap)
{
	// ベースクラス呼び出し
	COptions::Load(hMap);

	// 読み込み
	HKEY hKey = 0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, KEY_SETTINGS, 
						0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		DWORD dwType, dwSize;
		dwSize = sizeof(DWORD);
		if (RegQueryValueEx(hKey, NAME_WINDOW_ALPHA, 0, &dwType, (LPBYTE)&m_nWndAlpha, &dwSize) != ERROR_SUCCESS)
			m_nWndAlpha = 255;
		RegCloseKey(hKey);
	}
}

int CWin32Options::GetPropPages(PROPSHEETPAGE** ppPage)
{
	int nPages = COptions::GetPropPages(ppPage);

	BOOL (WINAPI *pSetLayeredWindowAttributes)(HWND, COLORREF, BYTE bAlpha, DWORD);
	pSetLayeredWindowAttributes = NULL;
	HINSTANCE hInstDll = (HINSTANCE)LoadLibrary(_T("user32.dll"));
	if (hInstDll) {
		(FARPROC&)pSetLayeredWindowAttributes = GetProcAddress(hInstDll, "SetLayeredWindowAttributes");
		FreeLibrary(hInstDll);
		if (pSetLayeredWindowAttributes) {
			*ppPage = (PROPSHEETPAGE*)realloc(*ppPage, sizeof(PROPSHEETPAGE) * (nPages + 1));
			(*ppPage)[nPages].dwSize = sizeof(PROPSHEETPAGE);
			(*ppPage)[nPages].dwFlags = PSP_DEFAULT;
			(*ppPage)[nPages].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_WNDALPHA);
			(*ppPage)[nPages].pfnDlgProc = (DLGPROC)WndAlphaPageProc;
			(*ppPage)[nPages].hInstance = GetInst();
			(*ppPage)[nPages].pszIcon = NULL;
			(*ppPage)[nPages].lParam = (LPARAM)this;
			nPages++;
		}
	}
	return nPages;
}

BOOL CALLBACK CWin32Options::WndAlphaPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CWin32Options* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (CWin32Options*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->WndAlphaPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_NOTIFY:
	{
		NMHDR* pnmh = (NMHDR*)lParam; 
		if (pnmh->code == PSN_APPLY) {
			pOptions->WndAlphaPageOnOK(hwndDlg);
			return TRUE;
		}
		return FALSE;
	}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	default:
		return FALSE;
	}
}

void CWin32Options::WndAlphaPageOnInitDialog(HWND hwndDlg)
{
	HWND hwndSlider = GetDlgItem(hwndDlg, IDC_SLIDER_ALPHA);
	SendMessage(hwndSlider, TBM_SETRANGE, TRUE, MAKELONG(25, 100));
	SendMessage(hwndSlider, TBM_SETTIC, 0, 50);
	SendMessage(hwndSlider, TBM_SETTIC, 0, 75);
	SendMessage(hwndSlider, TBM_SETPOS, TRUE, m_nWndAlpha * 100 / 255);
}

void CWin32Options::WndAlphaPageOnOK(HWND hwndDlg)
{
	HWND hwndSlider = GetDlgItem(hwndDlg, IDC_SLIDER_ALPHA);
	m_nWndAlpha = min(255, (SendMessage(hwndSlider, TBM_GETPOS, 0, 0) * 255 / 100) + 1);
}
