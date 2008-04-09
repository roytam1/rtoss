#ifndef __WIN32PLAYERAPP_H__
#define __WIN32PLAYERAPP_H__

#include "PlayerApp.h"

class CWin32PlayerApp : public CPlayerApp
{
public:
	CWin32PlayerApp();
	virtual ~CWin32PlayerApp();

protected:
	virtual CMainWnd* GetMainWndClass();
};

#endif // __WIN32PLAYERAPP_H__