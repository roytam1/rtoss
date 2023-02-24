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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ClipSpy.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "ClipSpyView.h"
#include "ClipSpyDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDC_CLEAR_CLIPBOARD, OnClearClipboard)
	ON_COMMAND(IDC_SAVE_DATA, OnSaveData)
	ON_UPDATE_COMMAND_UI(IDC_SAVE_DATA, OnUpdateSaveData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CClipSpyView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

    cs.style &= ~FWS_ADDTOTITLE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

// OnClearClipboard(): Handler for the File|Clear Clipboard commmand.  Empties
// the clipboard.
void CMainFrame::OnClearClipboard() 
{
    if ( OpenClipboard() )
        {
        EmptyClipboard();
        CloseClipboard();
        }
}

// OnSaveData(): Handler for the File|Save Selected Data command.  Saves the
// data being viewed in the right pane to a file.  The idea for this feature comes
// from Jim Barry's clipboard viewer.  See his program at:
// http://www.geocities.com/SiliconValley/2060/clipspy.html
// I just added it to this ClipSpy 'cause it's a neat feature, and something that
// might come in handy.
void CMainFrame::OnSaveData() 
{
CListView* pListView = (CListView*) m_wndSplitter.GetPane ( 0, 0 );
int nSelItem;

    ASSERT_KINDOF(CListView, pListView);

    nSelItem = pListView->GetListCtrl().GetNextItem ( -1, LVNI_SELECTED );

    ASSERT ( -1 != nSelItem );          // shouldn't get here if no item is selected

UINT uSelItem = (UINT) nSelItem;
UINT uBlockSize;

    uBlockSize = ((CClipSpyDoc*) GetActiveDocument())->GetDataBlockSize ( uSelItem );

    if ( 0 == uBlockSize )
        {
        MessageBox ( _T("No data to save!") );
        return;
        }

const LPBYTE pbyData = ((CClipSpyDoc*) GetActiveDocument())->GetDataBlock ( uSelItem );

    ASSERT ( NULL != pbyData );

CFileDialog dlg ( FALSE, NULL, NULL, 
                  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
                  _T("All files ( *.* )|*.*|"), this );

    if ( IDOK == dlg.DoModal() )
        {
        CFile file;
        CFileException e;

        if ( !file.Open ( dlg.GetPathName(),
                          CFile::modeWrite | CFile::shareDenyWrite |
                            CFile::modeCreate,
                          &e ))
            {
            e.ReportError();
            }
        else
            {
            try
                {
                file.Write ( pbyData, uBlockSize );
                }
            catch ( CFileException* ex )
                {
                ex->ReportError();
                }
            }
        }
}

void CMainFrame::OnUpdateSaveData(CCmdUI* pCmdUI) 
{
CListView* pListView = (CListView*) m_wndSplitter.GetPane ( 0, 0 );
bool bEnable = false;

    ASSERT_KINDOF(CListView, pListView);

CListCtrl& list = pListView->GetListCtrl();
POSITION pos = list.GetFirstSelectedItemPosition();

    if ( NULL != pos )
        {
        int nSelItem = list.GetNextSelectedItem(pos);

        bEnable = ( 0 != ((CClipSpyDoc*) GetActiveDocument())->GetDataBlockSize ( nSelItem ) );
        }

    pCmdUI->Enable ( bEnable );
}
