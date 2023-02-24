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

// ClipSpyView.cpp : implementation of the CClipSpyView class
//

#include "stdafx.h"
#include "ClipSpy.h"

#include "ClipSpyDoc.h"
#include "CntrItem.h"
#include "ClipSpyView.h"
#include "mainfrm.h"                    // for CMainFrame declaration
#include <afxpriv.h>                    // for WM_SETMESSAGESTRING

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClipSpyView

IMPLEMENT_DYNCREATE(CClipSpyView, CRichEditView)

BEGIN_MESSAGE_MAP(CClipSpyView, CRichEditView)
	//{{AFX_MSG_MAP(CClipSpyView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClipSpyView construction/destruction

CClipSpyView::CClipSpyView()
{
	// TODO: add construction code here

}

CClipSpyView::~CClipSpyView()
{
}

BOOL CClipSpyView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

void CClipSpyView::OnInitialUpdate()
{
HFONT hf = (HFONT) GetStockObject ( ANSI_FIXED_FONT );

	CRichEditView::OnInitialUpdate();

    // Set the edit control's font to a fixed-width font.
    m_font.Attach ( hf );
    SetFont ( &m_font );

    // Set the control to be read-only.
    GetRichEditCtrl().SetReadOnly();
}

void CClipSpyView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.
   CRichEditView::OnDestroy();
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
}


/////////////////////////////////////////////////////////////////////////////
// CClipSpyView diagnostics

#ifdef _DEBUG
void CClipSpyView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CClipSpyView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CClipSpyDoc* CClipSpyView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClipSpyDoc)));
	return (CClipSpyDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CClipSpyView message handlers


// OnUpdate(): Where all the action happens.  This displays a block of data
// in the rich edit control.  The format used is:
//
//     01234567 01 23 45 67  89 ab cd ef  ........
//     starting ^----bytes of data-----^  ASCII
//     address                            representation
//
// Note that this format is always the same - I don't try to make the data
// fit exactly in the window like, say, the VC hex editor.
//
// Parameters:
//  pSender: Not used.
//  lHint: If pHint != NULL, this is the index of the block to display.
//  pHint: If NULL, the control should be emptied.  If not NULL, then lHint
//         holds the index of the block to display.
//
// lHint and pHint are set by CLeftView::OnItemchanged(), the only code that
// calls UpdateAllViews().

void CClipSpyView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint) 
{
CRichEditCtrl& edit = GetRichEditCtrl();
CProgressCtrl  prog;

    // First empty the contents of the control.
    SetRedraw ( FALSE );
    edit.SetSel ( 0, -1 );
    edit.ReplaceSel ( _T("") );
    SetRedraw();
    Invalidate();

    // Bail if we don't have to display anything.
    if ( NULL == pHint )
        return;

const LPBYTE pbyData = GetDocument()->GetDataBlock ( (UINT) lHint );

    // If the data pointer is NULL, the block is empty so we don't have to do
    // anything more.
    if ( NULL == pbyData )
        return;

CWaitCursor w;                          // this might take a while...
UINT        uOffset, uDataSize;
UINT        uLineOffset;
UINT        uBytesInThisLine;
CString     sLine (' ', 64);            // preallocate enough space for a line
CString     sWork;
double      dProgressStep, dNextUpdateOffset;

    // Get the block size, and calculate stuff for the progress bar.
    uDataSize = GetDocument()->GetDataBlockSize ( (UINT) lHint );
    dProgressStep = uDataSize / 10.0;
    dNextUpdateOffset = dProgressStep;

    // Create a progress bar in the main frame status bar.
    CreateProgressCtrlInStatusBar ( &prog );

    SetRedraw ( FALSE );

    for ( uOffset = 0; uOffset < uDataSize; uOffset += 8 )
        {
        // Update progress bar if necessary.
        if ( uOffset >= dNextUpdateOffset )
            {
            dNextUpdateOffset += dProgressStep;
            prog.StepIt();
            }

        // How many bytes will be in the next line?  Max of 8.
        uBytesInThisLine = uDataSize - uOffset;

        if ( uBytesInThisLine > 8 )
            {
            uBytesInThisLine = 8;
            }

        // First part of the line - the starting offset.
        sLine.Format ( _T("%08X  "), uOffset );

        // Now loop through the data and add on the hex value of each byte.
        for ( uLineOffset = 0; uLineOffset < uBytesInThisLine; uLineOffset++ )
            {
            sWork.Format ( _T("%02X "), pbyData[uOffset + uLineOffset] );
            sLine += sWork;

            if ( 3 == uLineOffset || 7 == uLineOffset )
                sLine += ' ';
            }

        // If there were less than 8 bytes in this line, pad the line with
        // spaces so the ASCII representation will be in the right column.
        if ( uBytesInThisLine < 8 )
            {
            sLine += CString(' ', 1 + 3 * (8 - uBytesInThisLine) );

            if ( uBytesInThisLine < 4 )
                sLine += ' ';
            }

        // Add on the ASCII representation
        for ( uLineOffset = 0; uLineOffset < uBytesInThisLine; uLineOffset++ )
            {
            // If the next byte isn't printable, show a period instead.

            if ( _istprint ( pbyData[uOffset + uLineOffset] ))
                sLine += (TCHAR) pbyData[uOffset + uLineOffset];
            else
                sLine += '.';
            }

        sLine += '\n';

        edit.ReplaceSel ( sLine );
        }

    edit.SetSel ( 0, 0 );
    SetRedraw ( TRUE );
    Invalidate();

    // Restore the status bar's idle message.
    AfxGetMainWnd()->PostMessage ( WM_SETMESSAGESTRING,
                                   AFX_IDS_IDLEMESSAGE, 0 );
}


/////////////////////////////////////////////////////////////////////////////
// CClipSpyView other functions

// Create a progress bar in the main frame's status bar.  Code taken from
// "The MFC Answer Book" and tweaked a bit.
void CClipSpyView::CreateProgressCtrlInStatusBar ( CProgressCtrl* pProg )
{
CStatusBar* pStatusBar;
LPCTSTR     szMessage = _T("Reading data");
CRect       rc;

    pStatusBar = &((CMainFrame*) AfxGetMainWnd())->m_wndStatusBar;

    ASSERT_VALID(pProg);
    ASSERT_VALID(pStatusBar);

    pStatusBar->GetItemRect ( 0, &rc );

CClientDC dc ( pStatusBar );
CFont* pFont = pStatusBar->GetFont();
CFont* pOldFont = dc.SelectObject ( pFont );
CSize  sizeText = dc.GetTextExtent ( szMessage );

    dc.SelectObject ( pOldFont );

    rc.left += sizeText.cx + 10;

    pStatusBar->SetPaneText ( 0, szMessage );
    pStatusBar->RedrawWindow();

    pProg->Create ( WS_CHILD | WS_VISIBLE, rc, pStatusBar, 1 );
    pProg->SetRange ( 0, 90 );
    pProg->SetPos(0);
    pProg->SetStep(10);
}
