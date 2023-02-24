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

// ClipSpyView.h : interface of the CClipSpyView class
//

#if !defined(AFX_CLIPSPYVIEW_H__26B8ACCC_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
#define AFX_CLIPSPYVIEW_H__26B8ACCC_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClipSpyCntrItem;

class CClipSpyView : public CRichEditView
{
protected: // create from serialization only
	CClipSpyView();
	DECLARE_DYNCREATE(CClipSpyView)

// Attributes
public:
	CClipSpyDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClipSpyView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CClipSpyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CFont m_font;
    void CreateProgressCtrlInStatusBar ( CProgressCtrl* );

// Generated message map functions
protected:
	//{{AFX_MSG(CClipSpyView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ClipSpyView.cpp
inline CClipSpyDoc* CClipSpyView::GetDocument()
   { return (CClipSpyDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIPSPYVIEW_H__26B8ACCC_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
