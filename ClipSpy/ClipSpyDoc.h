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

// ClipSpyDoc.h : interface of the CClipSpyDoc class
//

#if !defined(AFX_CLIPSPYDOC_H__26B8ACCA_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
#define AFX_CLIPSPYDOC_H__26B8ACCA_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CClipSpyDoc : public CRichEditDoc
{
protected: // create from serialization only
	CClipSpyDoc();
	DECLARE_DYNCREATE(CClipSpyDoc)

// Attributes
public:

// Operations
public:
    void Clear();
    void Init ( const UINT uNumBlocks );
    void AddDataBlock ( HGLOBAL hgData, const UINT uSize );
    void AddEmptyBlock();
    const LPBYTE GetDataBlock ( const UINT uBlockIndex ) const;
    UINT GetDataBlockSize ( const UINT uBlockIndex ) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClipSpyDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

// Implementation
public:
	virtual ~CClipSpyDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    BYTE**     m_ppData;                // Array of BYTE*'s that hold the data
    CUIntArray m_auDataSizes;           // Array of data block sizes
    UINT       m_uNumBlocks;            // # of data blocks (empty and nonempty)
    UINT       m_uNextBlockIndex;       // Next available index in the arrays

// Generated message map functions
protected:
	//{{AFX_MSG(CClipSpyDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIPSPYDOC_H__26B8ACCA_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
