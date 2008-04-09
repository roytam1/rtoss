#include "GSPlayer2.h"
#include "Win32MainWnd.h"
#include "Win32Options.h"

CWin32MainWnd::CWin32MainWnd()
{
}

CWin32MainWnd::~CWin32MainWnd()
{
}

COptions* CWin32MainWnd::GetOptionsClass()
{
	return new CWin32Options();
}

void CWin32MainWnd::OnCreate(HWND hWnd)
{
	CMainWnd::OnCreate(hWnd);

	// “§–¾‰»‚·‚é
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	BOOL (WINAPI *pSetLayeredWindowAttributes)(HWND, COLORREF, BYTE bAlpha, DWORD);
	pSetLayeredWindowAttributes = NULL;
	HINSTANCE hInstDll = (HINSTANCE)LoadLibrary(_T("user32.dll"));
	if (hInstDll) {
		(FARPROC&)pSetLayeredWindowAttributes = GetProcAddress(hInstDll, "SetLayeredWindowAttributes");
		if (pSetLayeredWindowAttributes)
			pSetLayeredWindowAttributes(m_hWnd, 0, ((CWin32Options*)m_pOptions)->m_nWndAlpha, LWA_ALPHA);
		FreeLibrary(hInstDll);
	}
}

void CWin32MainWnd::OnToolOption()
{
	CMainWnd::OnToolOption();

	// “§–¾‰»‚·‚é
	BOOL (WINAPI *pSetLayeredWindowAttributes)(HWND, COLORREF, BYTE bAlpha, DWORD);
	pSetLayeredWindowAttributes = NULL;
	HINSTANCE hInstDll = (HINSTANCE)LoadLibrary(_T("user32.dll"));
	if (hInstDll) {
		(FARPROC&)pSetLayeredWindowAttributes = GetProcAddress(hInstDll, "SetLayeredWindowAttributes");
		if (pSetLayeredWindowAttributes)
			pSetLayeredWindowAttributes(m_hWnd, 0, ((CWin32Options*)m_pOptions)->m_nWndAlpha, LWA_ALPHA);
		FreeLibrary(hInstDll);
	}
}