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

// ClipSpy.h : main header file for the CLIPSPY application
//

#if !defined(AFX_CLIPSPY_H__26B8ACC4_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
#define AFX_CLIPSPY_H__26B8ACC4_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CClipSpyApp:
// See ClipSpy.cpp for the implementation of this class
//

class CClipSpyApp : public CWinApp
{
public:
	CClipSpyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClipSpyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CClipSpyApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIPSPY_H__26B8ACC4_B529_11D3_8D3B_E82FC0F8C918__INCLUDED_)
