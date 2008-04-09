#ifndef __WIN32OPTIONS_H__
#define __WIN32OPTIONS_H__

#include "options.h"

class CWin32MainWnd;
class CWin32Options : public COptions
{
protected:
	friend	CWin32MainWnd;
	int		m_nWndAlpha;

public:
	CWin32Options();
	virtual ~CWin32Options();
	virtual void Save(HANDLE hMap);
	virtual void Load(HANDLE hMap);

protected:
	virtual int GetPropPages(PROPSHEETPAGE** ppPage);

	static BOOL CALLBACK WndAlphaPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void WndAlphaPageOnInitDialog(HWND hwndDlg);
	void WndAlphaPageOnOK(HWND hwndDlg);
};

#endif // __WIN32OPTIONS_H__
