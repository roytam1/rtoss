//FolderDlg.h
#if !defined(__PPCFOLDERDLG_H_INCLUDED)
#define __PPCFOLDERDLG_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "FolderDlg.h"

class CPpcFolderDlg : public CFolderDlg
{
public:
	CPpcFolderDlg();
	virtual ~CPpcFolderDlg();

protected:
	virtual void OnInitDialog(HWND);
	virtual void OnOK(HWND);
	virtual void OnCancel(HWND);

	HWND	m_hwndMB;
};

#endif //__PPCFOLDERDLG_H_INCLUDED