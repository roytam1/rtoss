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

// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "ClipSpy.h"

#include "ClipSpyDoc.h"
#define INITGUID
#include "LeftView.h"

// We need this declaration for CComPtr, which uses __uuidof()
struct __declspec(uuid("{4657278B-411B-11d2-839A-00C04FD918D0}")) IDropTargetHelper;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CListView)

BEGIN_MESSAGE_MAP(CLeftView, CListView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_WM_CHANGECBCHAIN()
	ON_WM_DRAWCLIPBOARD()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_COMMAND(IDC_READ_CLIPBOARD, OnReadClipboard)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

CLeftView::CLeftView() : m_hNextClipboardViewer(NULL),
                         m_bCallingSetClipboardViewer(FALSE),
                         m_bUseDnDHelper(false)
{
    if ( SUCCEEDED( m_spDropHelper.CoCreateInstance (
                                       CLSID_DragDropHelper,
                                       NULL, CLSCTX_INPROC_SERVER )))
        {
        m_bUseDnDHelper = true;
        }
}
                            
CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~LVS_TYPESTYLEMASK;
    cs.style |= LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CClipSpyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

// OnInitialUpdate(): Do some initialization.
void CLeftView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

    // Register the list control as a drop target.
    m_drop.Register ( this );

CListCtrl& list = GetListCtrl();

    // Init the list control.
    list.SetExtendedStyle ( LVS_EX_FULLROWSELECT );

    list.InsertColumn ( 0, _T("Format"), LVCFMT_LEFT, 0, 0 );
    list.InsertColumn ( 1, _T("Data size"), LVCFMT_LEFT, 0, 1 );

    list.SetColumnWidth ( 0, LVSCW_AUTOSIZE_USEHEADER );
    list.SetColumnWidth ( 1, LVSCW_AUTOSIZE_USEHEADER );

    // Register this window as a clipboard viewer.  This makes us get the
    // WM_DRAWCLIPBOARD and WM_CHANGECBCHAIN messages.  The 
    // m_bCallingSetClipboardViewer is necessary because SetClipboardViewer()
    // sends us a WM_DRAWCLIPBOARD but we must not process it.  See
    // OnDrawClipboard() for more details.
    m_bCallingSetClipboardViewer = TRUE;
    m_hNextClipboardViewer = SetClipboardViewer();
    m_bCallingSetClipboardViewer = FALSE;

    // Read the stuff on the clipboard, if there is any.
    OnReadClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CListView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CClipSpyDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClipSpyDoc)));
	return (CClipSpyDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CLeftView command handlers

// OnReadClipboard(): Handler for the File|Read Clipboard command.  Attaches
// a COleDataObject to the clipboard and reads the contents of the clipboard.
void CLeftView::OnReadClipboard() 
{
COleDataObject xDataObj;

    if ( !xDataObj.AttachClipboard() )
        return;

    ReadDataAndFillList ( &xDataObj );
}


/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

// Handler for WM_CHANGECBCHAIN.
void CLeftView::OnChangeCbChain(HWND hWndRemove, HWND hWndAfter) 
{
    // If the next window in the chain is being removed, reset our
    // "next window" handle.
    if ( m_hNextClipboardViewer == hWndRemove )
        {
        m_hNextClipboardViewer = hWndAfter;
        }

    // If there is a next clipboard viewer, pass the message on to it.
    if ( NULL != m_hNextClipboardViewer )
        {
        ::SendMessage ( m_hNextClipboardViewer, WM_CHANGECBCHAIN, 
                        (WPARAM) hWndRemove, (LPARAM) hWndAfter );
        }

    CListView::OnChangeCbChain(hWndRemove, hWndAfter);
}

// Handler for WM_DRAWCLIPBOARD.  We get this message when the contents of
// the clipboard changes.
void CLeftView::OnDrawClipboard() 
{
    // Note the m_bCallingSetClipboardViewer flag.  This flag is set before
    // the call to SetClipboardViewer().  We get a WM_DRAWCLIPBOARD message
    // before SetClipboardViewer() returns, but we must not process it.
    // So we don't do anything if that flag is set.

    if ( !m_bCallingSetClipboardViewer )
        {
        // If there is a next clipboard viewer, pass the message on to it.
        if ( NULL != m_hNextClipboardViewer )
            ::SendMessage ( m_hNextClipboardViewer, WM_DRAWCLIPBOARD, 0, 0 );

        // Read the contents of the clipboard.
        OnReadClipboard();

        CListView::OnDrawClipboard();
        }
}

// Handler for NM_CUSTOMDRAW.
void CLeftView::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
NMLVCUSTOMDRAW* pNMLV = (NMLVCUSTOMDRAW*) pNMHDR;

    *pResult = 0;

    if ( CDDS_PREPAINT == pNMLV->nmcd.dwDrawStage )
        {
        *pResult = CDRF_NOTIFYITEMDRAW;
        }
    else if ( CDDS_ITEMPREPAINT == pNMLV->nmcd.dwDrawStage )
        {
        // If the item has the "cut" flag set, draw the item with grey text.
        if ( LVIS_CUT & 
               GetListCtrl().GetItemState ( pNMLV->nmcd.dwItemSpec, LVIS_CUT ))
            {
            pNMLV->clrText = GetSysColor ( COLOR_GRAYTEXT );
            }
        }
}

// Handler for LVN_ITEMCHANGED.  If an item has become unselected, then clear
// out the rich edit view.  If an item is now selected, fill the rich edit
// view with the associated data.
void CLeftView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
NMLISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;

    // If the change wasn't in the state of an item, then we don't have to
    // do anything.
    if ( 0 == ( LVIF_STATE & pNMLV->uChanged ))
        return;

    if ( pNMLV->uNewState & LVIS_SELECTED )
        {
        // The item is now selected, so tell the rich edit view to show the
        // associated data.
        GetDocument()->UpdateAllViews ( this, pNMLV->iItem, (CObject*) 1 );
        }
    else
        {
        // The item is unselected, so empty out the rich edit view.  The
        // 3rd param (NULL) will tell the rich edit view to empty itself.
        GetDocument()->UpdateAllViews ( this, 0, NULL );
        }

    *pResult = 0;                       // Return value is ignored.
}

// Handler for WM_DESTROY.  Tell Windows that we're closing so the clipboard
// viewer chain gets updated properly.
void CLeftView::OnDestroy() 
{
    if ( NULL != m_hNextClipboardViewer )
        {
        ChangeClipboardChain ( m_hNextClipboardViewer );
        }

    CListView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CLeftView drag & drop functions

DROPEFFECT CLeftView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
DROPEFFECT dropeff = DROPEFFECT_COPY;

    if ( m_bUseDnDHelper )
        {
        IDataObject* pdo = pDataObject->GetIDataObject ( FALSE );
        m_spDropHelper->DragEnter ( GetSafeHwnd(), pdo, &point, dropeff );
        }

    return dropeff;
}

DROPEFFECT CLeftView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
DROPEFFECT dropeff = DROPEFFECT_COPY;

    if ( m_bUseDnDHelper )
        {
        m_spDropHelper->DragOver ( &point, dropeff );
        }

    return dropeff;
}

BOOL CLeftView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
    if ( m_bUseDnDHelper )
        {
        IDataObject* pdo = pDataObject->GetIDataObject ( FALSE );
        m_spDropHelper->Drop ( pdo, &point, dropEffect );
        }

    return ReadDataAndFillList ( pDataObject );
}

void CLeftView::OnDragLeave()
{
    if ( m_bUseDnDHelper )
        {
        m_spDropHelper->DragLeave();
        }
}


/////////////////////////////////////////////////////////////////////////////
// CLeftView other functions

// ReadDataAndFillList(): Reads the data from the passed-in COleDataObject
// and fills in the list control with the available formats.
BOOL CLeftView::ReadDataAndFillList ( COleDataObject* pDO )
{
CListCtrl&       list = GetListCtrl();
CClipSpyDoc*     pDoc = GetDocument();
FORMATETC        etc;
UINT             uNumFormats = 0;
CClipFormatData* pClip;
TCHAR            szFormat[256];
HGLOBAL          hgData;
UINT             uDataSize;
CString          sSize;
int              nItem = 0;

    ASSERT ( AfxIsValidAddress ( pDO, sizeof(COleDataObject) ));

    // Empty the list control and the doc.

    list.DeleteAllItems();
    pDoc->Clear();
    pDoc->UpdateAllViews ( this, 0, NULL ); // NULL tells the rich edit view to empty itself

    // Determine how many formats are available on the clipboard.

    pDO->BeginEnumFormats();

    while ( pDO->GetNextFormat ( &etc ))
        {
        if ( pDO->IsDataAvailable ( etc.cfFormat ))
            {
            uNumFormats++;
            }
        }

    // Now get all the data and pass it to the doc for storage.

    pDoc->Init ( uNumFormats );
    pDO->BeginEnumFormats();

    while ( pDO->GetNextFormat ( &etc ))
        {
        bool bReadData = false;

        if ( !pDO->IsDataAvailable ( etc.cfFormat ))
            continue;

        // See if this is a built-in clipboard format.  If so, we already have
        // a description string for it - we just have to find it in the 
        // g_aClipData array.
        for ( pClip = &g_aClipData[0]; 0 != pClip->uFormat; pClip++ )
            {
            if ( etc.cfFormat == pClip->uFormat )
                {
                lstrcpy ( szFormat, pClip->szFormatName );
                break;
                }
            }

        // If we didn't find the format in g_aClipData, then it's a custom
        // format, and we need to get the name from Windows.
        if ( 0 == pClip->uFormat )
            {
            GetClipboardFormatName ( etc.cfFormat, szFormat, 256 );
            }

        // Get an HGLOBAL of the data.
        hgData = pDO->GetGlobalData ( etc.cfFormat );

        if ( NULL != hgData )
            {
            uDataSize = GlobalSize ( hgData );

            sSize.Format ( _T("%u (0x%X)"), uDataSize, uDataSize );
            pDoc->AddDataBlock ( hgData, uDataSize );

            // Free the memory that GetGlobalData() allocated for us.
            GlobalFree ( hgData );

            bReadData = true;
            }
        else
            {
            // The data isn't in global memory, so try getting an IStream 
            // interface to it.
            STGMEDIUM stg;

            if ( pDO->GetData ( etc.cfFormat, &stg ) )
                {
                switch ( stg.tymed )
                    {
                    case TYMED_HGLOBAL:
                        {
                        uDataSize = GlobalSize ( stg.hGlobal );

                        sSize.Format ( _T("%u (0x%X)"), uDataSize, uDataSize );
                        pDoc->AddDataBlock ( stg.hGlobal, uDataSize );
            
                        bReadData = true;
                        }
                    break;
                    
                    case TYMED_ISTREAM:
                        {
                        LARGE_INTEGER li;
                        ULARGE_INTEGER uli;

                        li.HighPart = li.LowPart = 0;

                        if ( SUCCEEDED( stg.pstm->Seek ( li, STREAM_SEEK_END, &uli )))
                            {
                            HGLOBAL hg = GlobalAlloc ( GMEM_MOVEABLE | GMEM_SHARE,
                                                       uli.LowPart );
                            void* pv = GlobalLock ( hg );

                            stg.pstm->Seek ( li, STREAM_SEEK_SET, NULL );
                            
                            if ( SUCCEEDED( stg.pstm->Read ( pv, uli.LowPart, (PULONG) &uDataSize )))
                                {
                                GlobalUnlock ( hg );

                                sSize.Format ( _T("%u (0x%X)"), uDataSize, uDataSize );
                                pDoc->AddDataBlock ( hg, uDataSize );
                            
                                // Free the memory we just allocated.
                                GlobalFree ( hg );
            
                                bReadData = true;
                                }
                            else
                                {
                                GlobalUnlock ( hg );
                                }
                            }
                        }
                    break;  // case TYMED_ISTREAM
                    }

                ReleaseStgMedium ( &stg );
                }
            }

        list.InsertItem ( nItem, szFormat );
        
        if ( bReadData )
            {
            list.SetItemText ( nItem, 1, sSize );
            }
        else
            {
            // If we couldn't get the data for this format, set the "cut" flag
            // of the list control item, so that the NM_CUSTOMDRAW handler will
            // draw the text in grey.
            
            pDoc->AddEmptyBlock();
            list.SetItemText ( nItem, 1, _T("<Data unavailable>") );
            list.SetItemState ( nItem, LVIS_CUT, LVIS_CUT );
            }

        nItem++;
        }   // end while

    list.SetColumnWidth ( 0, LVSCW_AUTOSIZE_USEHEADER );
    list.SetColumnWidth ( 1, LVSCW_AUTOSIZE_USEHEADER );

    return TRUE;
}
