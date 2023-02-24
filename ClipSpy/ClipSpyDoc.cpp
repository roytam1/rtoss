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

// ClipSpyDoc.cpp : implementation of the CClipSpyDoc class
//

#include "stdafx.h"
#include "ClipSpy.h"

#include "ClipSpyDoc.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClipSpyDoc

IMPLEMENT_DYNCREATE(CClipSpyDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CClipSpyDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CClipSpyDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClipSpyDoc construction/destruction

CClipSpyDoc::CClipSpyDoc() : m_ppData(NULL), m_uNumBlocks(0),
                             m_uNextBlockIndex(0)
{
    m_auDataSizes.SetSize ( 0, 10 );    // grow by 10 elts
}

CClipSpyDoc::~CClipSpyDoc()
{
}

BOOL CClipSpyDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

CRichEditCntrItem* CClipSpyDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return new CClipSpyCntrItem(preo, (CClipSpyDoc*) this);
}



/////////////////////////////////////////////////////////////////////////////
// CClipSpyDoc serialization

void CClipSpyDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CRichEditDoc enables serialization
	//  of the container document's COleClientItem objects.
	// TODO: set CRichEditDoc::m_bRTF = FALSE if you are serializing as text
	CRichEditDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CClipSpyDoc diagnostics

#ifdef _DEBUG
void CClipSpyDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CClipSpyDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CClipSpyDoc overrides

// Override of SaveModified(): This just returns TRUE to indicate to MFC that
// it's OK to close the document.  Not doing this would result in a message
// box when closing the app (because CRichEditDoc would prompt you to save
// the modified document).
BOOL CClipSpyDoc::SaveModified() 
{
    return TRUE;    // OK to close
}

// Override of DeleteContents(): Clear our private data, then let the base
// class handle the rest.
void CClipSpyDoc::DeleteContents() 
{
    Clear();	
	CRichEditDoc::DeleteContents();
}


/////////////////////////////////////////////////////////////////////////////
// CClipSpyDoc public interface

// Clear(): Frees any clipboard data that has been stored in the doc.
void CClipSpyDoc::Clear()
{
    if ( NULL != m_ppData )
        {
        for ( UINT u = 0; u < m_uNumBlocks; u++ )
            {
            if ( NULL != m_ppData[u] )
                {
                delete [] m_ppData[u];
                }
            }

        delete m_ppData;
        m_ppData = NULL;
        }
        
    m_auDataSizes.RemoveAll();
    m_uNumBlocks = 0;
    m_uNextBlockIndex = 0;
}

// Init(): The client calls this function to tell the doc how many blocks of
// data it will be storing.
void CClipSpyDoc::Init ( const UINT uNumBlocks )
{
    // Clear out existing data if necessary.
    if ( NULL != m_ppData )
        {
        Clear();
        }

    m_uNumBlocks = uNumBlocks;
    m_uNextBlockIndex = 0;

    m_ppData = new LPBYTE [ uNumBlocks ];
}

// AddDataBlock(): Adds a chunk of data to the document.  hgData is an HGLOBAL
// of the data, and uSize is the size in bytes.
void CClipSpyDoc::AddDataBlock ( HGLOBAL hgData, const UINT uSize )
{
LPVOID pvData;

    ASSERT ( NULL != m_ppData );        // Called Init() yet?
    ASSERT ( m_uNextBlockIndex < m_uNumBlocks );    // Too many blocks added?

    // Get a real pointer to the data.
    pvData = GlobalLock ( hgData );

    if ( NULL != pvData )
        {
        ASSERT ( AfxIsValidAddress ( pvData, uSize, FALSE ));

        // Allocate memory to hold a copy of the data.
        m_ppData [ m_uNextBlockIndex ] = new BYTE [ uSize ];

        // Copy the data to the newly-allocated memory.
        CopyMemory ( m_ppData [ m_uNextBlockIndex ], pvData, uSize );

        // Update other stuff...
        m_auDataSizes.Add ( uSize );
        m_uNextBlockIndex++;

        // Unlock the HGLOBAL.
        GlobalUnlock ( hgData );
        }
    else
        {
        // Couldn't access the data, so just add an empty block instead.
        AddEmptyBlock();
        }
}

// AddEmptyBlock(): Adds a zero-length block to the document.  The client calls
// this if a clipboard format is listed as being on the clipboard, but
// can't access the data for some reason.
void CClipSpyDoc::AddEmptyBlock()
{
    ASSERT ( NULL != m_ppData );        // Called Init() yet?
    ASSERT ( m_uNextBlockIndex < m_uNumBlocks );    // Too many blocks added?

    m_ppData [ m_uNextBlockIndex++ ] = NULL;
    m_auDataSizes.Add ( 0 );
}

// GetDataBlock(): Retrieves a pointer to a given block of data.  Can return
// NULL if the doc is empty, or the given index has an empty data block.
const LPBYTE CClipSpyDoc::GetDataBlock ( const UINT uBlockIndex ) const
{
    if ( 0 == m_uNumBlocks )
        return NULL;

    ASSERT ( uBlockIndex < m_uNumBlocks );  // Index out of range?

    return m_ppData [ uBlockIndex ];
}

// GetDataBlockSize(): Returns the length of the data block at the given index.
UINT CClipSpyDoc::GetDataBlockSize ( const UINT uBlockIndex ) const
{
    ASSERT ( uBlockIndex < m_uNumBlocks );  // Index out of range?

    return m_auDataSizes [ uBlockIndex ];
}
