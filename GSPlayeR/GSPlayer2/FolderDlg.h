//FolderDlg.h
#if !defined(__FOLDERDLG_H_INCLUDED)
#define __FOLDERDLG_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////
//INCLUDE
#include "resource.h"

///////////////////////////////////////////////////////////////
//
class CFolderDlg
{
public:
	CFolderDlg();
	virtual ~CFolderDlg();
	virtual int ShowFolderDlg(HWND hwndParent, LPTSTR szPath, int nSize);
	virtual void SetSubFolder(BOOL bSubFolder) {m_bSubFolder = bSubFolder;}
	virtual BOOL GetSubFolder() {return m_bSubFolder;}

protected:
	LPTSTR m_pszPath;
	int m_nSize;
	HIMAGELIST m_hImageList;

	BOOL m_bRefresh;
	BOOL m_bSubFolder;
	LPTSTR m_lpszName;

	virtual void OnOK(HWND);
	virtual void OnCancel(HWND);
	virtual void OnInitDialog(HWND);
	virtual void CreateNewFolder(HWND);
	virtual void OnRenameFolder(HWND);
	virtual void SetCurrentFolder(HWND, LPCTSTR);

	virtual void GetTree (HWND, HTREEITEM, LPTSTR);
	virtual int CountChildren (LPTSTR, LPTSTR);
	virtual int EnumChildren (HWND, HTREEITEM, LPTSTR);
	virtual void OnItemExpanded(NMTREEVIEW*);
	virtual void OnItemExpanding(NMTREEVIEW*);

	static LRESULT CALLBACK FolderDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif //__FOLDERDLG_H_INCLUDED