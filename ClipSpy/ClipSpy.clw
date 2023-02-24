; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ClipSpy.h"
LastPage=0

ClassCount=6
Class1=CClipSpyApp
Class2=CClipSpyDoc
Class3=CClipSpyView
Class4=CMainFrame

ResourceCount=4
Resource1=IDR_CNTR_INPLACE
Resource2=IDR_MAINFRAME
Class5=CLeftView
Class6=CAboutDlg
Resource4=IDD_ABOUTBOX

[CLS:CClipSpyApp]
Type=0
HeaderFile=ClipSpy.h
ImplementationFile=ClipSpy.cpp
Filter=N

[CLS:CClipSpyDoc]
Type=0
HeaderFile=ClipSpyDoc.h
ImplementationFile=ClipSpyDoc.cpp
Filter=N
BaseClass=CRichEditDoc
VirtualFilter=DC

[CLS:CClipSpyView]
Type=0
HeaderFile=ClipSpyView.h
ImplementationFile=ClipSpyView.cpp
Filter=C
BaseClass=CRichEditView
VirtualFilter=VWC
LastObject=CClipSpyView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=W
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=IDC_SAVE_DATA



[CLS:CLeftView]
Type=0
HeaderFile=LeftView.h
ImplementationFile=LeftView.cpp
Filter=T
BaseClass=CListView
VirtualFilter=VWC
LastObject=CLeftView

[CLS:CAboutDlg]
Type=0
HeaderFile=ClipSpy.cpp
ImplementationFile=ClipSpy.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=IDC_READ_CLIPBOARD
Command2=IDC_CLEAR_CLIPBOARD
Command3=IDC_SAVE_DATA
Command4=ID_APP_EXIT
Command5=ID_APP_ABOUT
CommandCount=5

[MNU:IDR_CNTR_INPLACE]
Type=1
Class=CClipSpyView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_APP_EXIT
CommandCount=5

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=IDC_SAVE_DATA
Command2=ID_CANCEL_EDIT_CNTR
CommandCount=2

[ACL:IDR_CNTR_INPLACE]
Type=1
Class=CClipSpyView
Command1=ID_CANCEL_EDIT_CNTR
CommandCount=1

