//////////////////////////////////////////////////////////////////////////
//
// ClipSpy, Copyright 1999  by Michael Dunn <mdunn at inreach dot com>
//
// This is a utility to view the raw contents of the clipboard, or the 
// data in a drag-and-drop operation.  There are still some quirks and
// bugs.  Comments/suggestions are always welcome!
//
// Revision history:
//  Dec 20, 1999: Version 1.0, first release.
//
// You can get the latest updates for ClipSpy at:
//  http://www.codeproject.com/clipboard/clipspy.asp
//
//////////////////////////////////////////////////////////////////////////

// ClipSpy.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ClipSpy.h"

#include "MainFrm.h"
#include "ClipSpyDoc.h"
#include "LeftView.h"

#include "winnls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClipSpyApp

BEGIN_MESSAGE_MAP(CClipSpyApp, CWinApp)
	//{{AFX_MSG_MAP(CClipSpyApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClipSpyApp construction

CClipSpyApp::CClipSpyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClipSpyApp object

CClipSpyApp theApp;

// This array holds descriptive strings for the built-in clipboard types.
CClipFormatData g_aClipData[] = {
    { CF_TEXT,            _T("CF_TEXT") },
    { CF_BITMAP,          _T("CF_BITMAP") },
    { CF_METAFILEPICT,    _T("CF_METAFILEPICT") },
    { CF_SYLK,            _T("CF_SYLK") },
    { CF_DIF,             _T("CF_DIF") },
    { CF_TIFF,            _T("CF_TIFF") },
    { CF_OEMTEXT,         _T("CF_OEMTEXT") },
    { CF_DIB,             _T("CF_DIB") },
    { CF_PALETTE,         _T("CF_PALETTE") },
    { CF_PENDATA,         _T("CF_PENDATA") },
    { CF_RIFF,            _T("CF_RIFF") },
    { CF_WAVE,            _T("CF_WAVE") },
    { CF_UNICODETEXT,     _T("CF_UNICODETEXT") },
    { CF_ENHMETAFILE,     _T("CF_ENHMETAFILE") },
    { CF_HDROP,           _T("CF_HDROP") },
    { CF_LOCALE,          _T("CF_LOCALE") },
    { CF_DIBV5,           _T("CF_DIBV5") },
    { CF_OWNERDISPLAY,    _T("CF_OWNERDISPLAY") },
    { CF_DSPTEXT,         _T("CF_DSPTEXT") },
    { CF_DSPBITMAP,       _T("CF_DSPBITMAP") },
    { CF_DSPMETAFILEPICT, _T("CF_DSPMETAFILEPICT") },
    { CF_DSPENHMETAFILE,  _T("CF_DSPENHMETAFILE") },
    { 0, _T("") } 
    };


/////////////////////////////////////////////////////////////////////////////
// CClipSpyApp initialization

BOOL CClipSpyApp::InitInstance()
{
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Mike's Classy Software"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CClipSpyDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLeftView));
	pDocTemplate->SetContainerInfo(IDR_CNTR_INPLACE);
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClipSpyApp message handlers

// App command to run the dialog
void CClipSpyApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
