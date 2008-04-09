// PpcKeyCtrlDlg.h
#if !defined(__PPCKEYCTRLDLG_H_INCLUDED)
#define __PPCKEYCTRLDLG_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#define ID_TIMER_KEYCTRLCLOSE		1
#define INT_TIMER_KEYCTRLCLOSE		1000
#define KEYCTRLDLG_CLOSE_INT		10000

class CPpcKeyCtrlDlg
{
public:
	CPpcKeyCtrlDlg();
	virtual ~CPpcKeyCtrlDlg();
	void ShowKeyCtrlDlg(HWND hwndParent, RECT* prc, int nColorBack, int nColorText);

protected:
	virtual void OnInitDialog(HWND hDlg);
	virtual void OnClose(HWND hDlg);
	virtual void OnKeyDown(HWND hDlg, UINT uKeyCode);
	virtual long OnCtlColorStatic(HDC hDC);
	virtual void OnPaint(HWND hDlg);
	virtual void OnTimer(HWND hDlg, UINT nId);
	static BOOL CALLBACK PpcKeyCtrlDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	HWND		m_hwndParent;
	RECT		m_rcDlg;
	int			m_nColorBack;
	int			m_nColorText;
	HBRUSH		m_hBrushBack;
	DWORD		m_dwStart;
};

#endif //__PPCKEYCTRLDLG_H_INCLUDED