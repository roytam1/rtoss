#include "resource.h"
#include "windows.h"
#include "wcehelper.h"

// class CWinceHelper
CWinceHelper::CWinceHelper() : 
m_hAygShell(NULL), m_pSHInitDialog(NULL)
{
	m_hAygShell = LoadLibrary(_T("aygshell.dll"));
	if (m_hAygShell) {
		(FARPROC&)m_pSHInitDialog = GetProcAddress(m_hAygShell, _T("SHInitDialog"));
		(FARPROC&)m_pSHCreateMenuBar = GetProcAddress(m_hAygShell, _T("SHCreateMenuBar"));		
	}
	m_hCoreDll = LoadLibrary(_T("coredll.dll"));
	if (m_hCoreDll) {
		(FARPROC&)m_pSipGetInfo = GetProcAddress(m_hCoreDll, _T("SipGetInfo"));
	}
}

CWinceHelper::~CWinceHelper()
{
	if (m_hAygShell)
		FreeLibrary(m_hAygShell);
	if (m_hCoreDll)
		FreeLibrary(m_hCoreDll);
}

BOOL CWinceHelper::DefDlgPaintProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if (IsPocketPC()) {
		if (GetSystemMetrics(SM_CXSCREEN) != 240 || GetSystemMetrics(SM_CYSCREEN) != 320)
			return FALSE;

		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hDlg, &ps);
		POINT pt[2] = {{0, 24},{GetSystemMetrics(SM_CXSCREEN),24}};
		Polyline(hDC, pt, sizeof(pt)/sizeof(POINT));
		EndPaint(hDlg, &ps);
		return TRUE;
	}
	return FALSE;
}

BOOL CWinceHelper::DefDlgCtlColorStaticProc(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if (IsPocketPC()) {
		HDC hDC = (HDC)wParam;
		if (GetDlgCtrlID((HWND)lParam) == IDC_TITLE)
		{
			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHT));
			return (long)GetStockObject(WHITE_BRUSH);
		}
		else
			return FALSE;
	}
	return FALSE;
}

void CWinceHelper::SHInitDialog(HWND hwndDlg)
{
	if (m_hAygShell && m_pSHInitDialog) {
		SHINITDLGINFO shidi;
		shidi.dwMask = SHIDIM_FLAGS;
		shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLGFULLSCREEN;
		shidi.hDlg = hwndDlg;
		m_pSHInitDialog(&shidi);
	}
}

HWND CWinceHelper::SHCreateMenuBar(HWND hwndParent, int nMenuID, int nBmpID, int cBmpImages)
{
	if (m_pSHCreateMenuBar) {
		SHMENUBARINFO mbi;
		memset(&mbi, 0, sizeof(SHMENUBARINFO));
		mbi.cbSize = sizeof(SHMENUBARINFO);
		mbi.hwndParent = hwndParent;
		mbi.nToolBarId = nMenuID;
		mbi.hInstRes = g_hInst;
		mbi.nBmpId = nBmpID;
		mbi.cBmpImages = cBmpImages;
		//mbi.dwFlags = SHCMBF_EMPTYBAR;

		if (m_pSHCreateMenuBar(&mbi)) 
			return mbi.hwndMB;
	}
	return NULL;
}

BOOL CWinceHelper::IsSipPanelVisible()
{
	if (m_pSipGetInfo) {
		SIPINFO si;
		memset(&si, 0, sizeof(SIPINFO));
		si.cbSize = sizeof(SIPINFO);
		m_pSipGetInfo(&si);
		if ((si.fdwFlags & SIPF_ON))
			return TRUE;
	}
	return FALSE;
}