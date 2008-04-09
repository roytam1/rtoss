#ifndef __PLAYERAPP_H__
#define __PLAYERAPP_H__

#define MUTEX_NAME	_T("GSPLAYER")
#include "MainWnd.h"

class CPlayerApp
{
public:
	CPlayerApp();
	virtual ~CPlayerApp();
	virtual int Run(HINSTANCE hInst, LPTSTR pszCmdLine);

	HINSTANCE GetInst();
	CMainWnd* GetMainWnd();

protected:
	virtual CMainWnd* GetMainWndClass();

protected:
	HINSTANCE	m_hInst;
	LPTSTR		m_pszCmdLine;
	HACCEL		m_hAccel;

	CMainWnd*	m_pWnd;
};
#endif // __PLAYERAPP_H__