#ifndef __PPCPLAYERAPP_H__
#define __PPCPLAYERAPP_H__

class CPpcPlayerApp : public CPlayerApp
{
public:
	CPpcPlayerApp();
	virtual ~CPpcPlayerApp();

protected:
	CMainWnd* GetMainWndClass();
};
#endif // __PPCPLAYERAPP_H__
