#include "GSPlayer2.h"
#include "PpcPlayerApp.h"
#include "PpcMainWnd.h"

CPlayerApp* GetPlayerAppClass()
{
	return new CPpcPlayerApp();
}

CPpcPlayerApp::CPpcPlayerApp()
{
	INITCOMMONCONTROLSEX iccex;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccex.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES;
    InitCommonControlsEx(&iccex);
}

CPpcPlayerApp::~CPpcPlayerApp()
{
}

CMainWnd* CPpcPlayerApp::GetMainWndClass()
{
	return new CPpcMainWnd();
}