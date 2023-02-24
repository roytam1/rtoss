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

// CntrItem.cpp : implementation of the CClipSpyCntrItem class
//

#include "stdafx.h"
#include "ClipSpy.h"

#include "ClipSpyDoc.h"
#include "ClipSpyView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClipSpyCntrItem implementation

IMPLEMENT_SERIAL(CClipSpyCntrItem, CRichEditCntrItem, 0)

CClipSpyCntrItem::CClipSpyCntrItem(REOBJECT* preo, CClipSpyDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
	
}

CClipSpyCntrItem::~CClipSpyCntrItem()
{
	// TODO: add cleanup code here
	
}

/////////////////////////////////////////////////////////////////////////////
// CClipSpyCntrItem diagnostics

#ifdef _DEBUG
void CClipSpyCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CClipSpyCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
