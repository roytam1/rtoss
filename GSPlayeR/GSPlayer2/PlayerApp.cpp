#include "GSPlayer2.h"
#include "PlayerApp.h"
#include "MainWnd.h"

CPlayerApp::CPlayerApp()
{
	m_hInst = NULL;
	m_pszCmdLine = NULL;
	m_pWnd = NULL;
	m_hAccel = NULL;
}

CPlayerApp::~CPlayerApp()
{
	if (m_pWnd) {
		delete m_pWnd;
		m_pWnd = NULL;
	}
}

CMainWnd* CPlayerApp::GetMainWndClass()
{
	return new CMainWnd();
}

HINSTANCE CPlayerApp::GetInst()
{
	return m_hInst;
}

CMainWnd* CPlayerApp::GetMainWnd()
{
	return m_pWnd;
}

int CPlayerApp::Run(HINSTANCE hInst, LPTSTR pszCmdLine)
{
	m_hInst = hInst;
	m_pszCmdLine = pszCmdLine;

#ifdef REGISTER_WAKE_EVENT
	if (_tcscmp(pszCmdLine, APP_RUN_AFTER_WAKEUP) == 0) {
		HWND hwndPrev = FindWindow(MAINWND_CLASS_NAME, NULL);
		if (hwndPrev) {
			PostMessage(hwndPrev, WM_WAKEUP, 0, 0);
			return FALSE;
		}
	}
#endif

	// 重複起動チェック
	HANDLE hMutex = CreateMutex(NULL, NULL, MUTEX_NAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND hwndPrev = FindWindow(MAINWND_CLASS_NAME, NULL);
		if (hwndPrev) {
			if (!(GetWindowLong(hwndPrev, GWL_STYLE) & WS_VISIBLE))
				SendMessage(hwndPrev, WM_COMMAND, IDM_APP_SHOWHIDE, 0);
			SetForegroundWindow(hwndPrev);
			SendCmdLine(hwndPrev, pszCmdLine);
		}
		return FALSE;
	}

	CMainWnd::CheckSystem();

	// コモンコントロールの初期化
	InitCommonControls();

	// メインウインドウの作成
	m_pWnd = GetMainWndClass();
	if (!m_pWnd || !m_pWnd->Create(pszCmdLine))
		return -1;

	// メッセージループ
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (m_pWnd->CanTransAccel(&msg)) {
			if (TranslateAccelerator(m_pWnd->GetHandle(), m_pWnd->GetAccelHandle(), &msg))
				continue;
		}
		if (m_pWnd->IsDialogMessage(&msg))
			continue;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
