//////////////////////////////////////////////////////////////////////////
//
// ClipSpy, Copyright 1999-2001  by Michael Dunn <mdunn at inreach dot com>
//
// This is a utility to view the raw contents of the clipboard, or the 
// data in a drag-and-drop operation.  There are still some quirks and
// bugs.  Comments/suggestions are always welcome!
//
// Revision history:
//  Dec 20, 1999: Version 1.0, first release.
//
//  Sep 27, 2001: Version 1.2, added ability to save any block of data;
//      added IDropTargetHelper support to CLeftView.
//
// You can get the latest updates for ClipSpy at:
//  http://www.codeproject.com/clipboard/clipspy.asp
//
//////////////////////////////////////////////////////////////////////////

// LeftView.h : interface of the CLeftView class
//

#if !defined(AFX_LEFTVIEW_H__26B8ACCE_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
#define AFX_LEFTVIEW_H__26B8ACCE_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <shlobj.h>     // for IDropTargetHelper
#include <shlguid.h>

class CClipSpyDoc;

class CLeftView : public CListView
{
protected: // create from serialization only
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CClipSpyDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    HWND m_hNextClipboardViewer;
    BOOL m_bCallingSetClipboardViewer;
    COleDropTarget m_drop;              // makes the list a drop target

    BOOL ReadDataAndFillList ( COleDataObject* );

    CComPtr<IDropTargetHelper> m_spDropHelper;
    bool m_bUseDnDHelper;

// Generated message map functions
protected:
	//{{AFX_MSG(CLeftView)
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDrawClipboard();
	afx_msg void OnDestroy();
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReadClipboard();
	//}}AFX_MSG
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CClipSpyDoc* CLeftView::GetDocument()
   { return (CClipSpyDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__26B8ACCE_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
