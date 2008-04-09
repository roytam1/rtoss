#ifndef __WIN32MAINWND_H__
#define __WIN32MAINWND_H__

#include "MainWnd.h"

// “§–¾‰»
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED	0x80000
#endif

#ifndef LWA_ALPHA
#define LWA_ALPHA		2
#endif

class CWin32MainWnd : public CMainWnd
{
public:
	CWin32MainWnd();
	virtual ~CWin32MainWnd();

protected:
	virtual COptions* GetOptionsClass();
	virtual void OnCreate(HWND hWnd);
	virtual void OnToolOption();
};

#endif // __WIN32MAINWND