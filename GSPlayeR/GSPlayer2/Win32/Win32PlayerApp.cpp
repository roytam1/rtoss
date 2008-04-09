#include "GSPlayer2.h"
#include "Win32PlayerApp.h"
#include "Win32MainWnd.h"

CPlayerApp* GetPlayerAppClass()
{
	return new CWin32PlayerApp();
}

CWin32PlayerApp::CWin32PlayerApp()
{
}

CWin32PlayerApp::~CWin32PlayerApp()
{
}

CMainWnd* CWin32PlayerApp::GetMainWndClass()
{
	return new CWin32MainWnd();
}