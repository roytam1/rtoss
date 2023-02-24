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

// CntrItem.h : interface of the CClipSpyCntrItem class
//

#if !defined(AFX_CNTRITEM_H__26B8ACD0_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
#define AFX_CNTRITEM_H__26B8ACD0_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClipSpyDoc;
class CClipSpyView;

class CClipSpyCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CClipSpyCntrItem)

// Constructors
public:
	CClipSpyCntrItem(REOBJECT* preo = NULL, CClipSpyDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CClipSpyDoc* GetDocument()
		{ return (CClipSpyDoc*)CRichEditCntrItem::GetDocument(); }
	CClipSpyView* GetActiveView()
		{ return (CClipSpyView*)CRichEditCntrItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClipSpyCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CClipSpyCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__26B8ACD0_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
