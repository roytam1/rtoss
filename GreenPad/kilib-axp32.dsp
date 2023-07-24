# Microsoft Developer Studio Project File - Name="kilib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (ALPHA) Application" 0x0601

CFG=kilib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kilib-axp32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kilib-axp32.mak" CFG="kilib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kilib - Win32 Release" (based on "Win32 (ALPHA) Application")
!MESSAGE "kilib - Win32 Debug" (based on "Win32 (ALPHA) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kilib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\AxpRel"
# PROP BASE Intermediate_Dir ".\AxpRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\AxpRel"
# PROP Intermediate_Dir ".\AxpRel"
# PROP Ignore_Export_Lib 0
F90=fl32.exe
# ADD BASE F90 /I "AxpRel/"
# ADD F90 /I "AxpRel/"
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /Gt0 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /Gt0 /W3 /GX /O2 /I ".\kilib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D "STRICT" /Fr /YX"stdafx.h" /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "NO_JP_RES"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:ALPHA
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib /nologo /subsystem:windows /machine:ALPHA /out:".\AxpRel/GreenPad.exe"

!ELSEIF  "$(CFG)" == "kilib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\AxpDebug"
# PROP BASE Intermediate_Dir ".\AxpDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\AxpDebug"
# PROP Intermediate_Dir ".\AxpDebug"
# PROP Ignore_Export_Lib 0
F90=fl32.exe
# ADD BASE F90 /I "AxpDebug/"
# ADD F90 /FR /I "AxpDebug/"
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /ML /Gt0 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /ML /Gt0 /W3 /Gm /GX /ZI /Od /I ".\kilib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D "STRICT" /Fr /YX"stdafx.h" /FD /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:ALPHA
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib /nologo /subsystem:windows /debug /machine:ALPHA /out:".\AxpDebug/GreenPad.exe"

!ENDIF 

# Begin Target

# Name "kilib - Win32 Release"
# Name "kilib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\kilib\app.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\cmdarg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigManager.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\ctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\file.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\find.cpp
# End Source File
# Begin Source File

SOURCE=.\rsrc\gp_rsrc.rc
# ADD BASE RSC /l 0xc04 /i "rsrc"
# ADD RSC /l 0xc04 /i "rsrc" /i ".\rsrc"
# End Source File
# Begin Source File

SOURCE=.\GpMain.cpp
# End Source File
# Begin Source File

SOURCE=.\editwing\ip_ctrl1.cpp
# End Source File
# Begin Source File

SOURCE=.\editwing\ip_cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\editwing\ip_draw.cpp
# End Source File
# Begin Source File

SOURCE=.\editwing\ip_parse.cpp
# End Source File
# Begin Source File

SOURCE=.\editwing\ip_scroll.cpp
# End Source File
# Begin Source File

SOURCE=.\editwing\ip_text.cpp
# End Source File
# Begin Source File

SOURCE=.\editwing\ip_wrap.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\log.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\memory.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenSaveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\path.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\RSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\Search.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\string.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\thread.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\window.cpp
# End Source File
# Begin Source File

SOURCE=.\kilib\winutil.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\kilib\app.h
# End Source File
# Begin Source File

SOURCE=.\kilib\cmdarg.h
# End Source File
# Begin Source File

SOURCE=.\ConfigManager.h
# End Source File
# Begin Source File

SOURCE=.\kilib\ctrl.h
# End Source File
# Begin Source File

SOURCE=.\kilib\file.h
# End Source File
# Begin Source File

SOURCE=.\kilib\find.h
# End Source File
# Begin Source File

SOURCE=.\GpMain.h
# End Source File
# Begin Source File

SOURCE=.\kilib\log.h
# End Source File
# Begin Source File

SOURCE=.\kilib\memory.h
# End Source File
# Begin Source File

SOURCE=.\NSearch.h
# End Source File
# Begin Source File

SOURCE=.\OpenSaveDlg.h
# End Source File
# Begin Source File

SOURCE=.\kilib\path.h
# End Source File
# Begin Source File

SOURCE=.\kilib\registry.h
# End Source File
# Begin Source File

SOURCE=.\RSearch.h
# End Source File
# Begin Source File

SOURCE=.\Search.h
# End Source File
# Begin Source File

SOURCE=.\kilib\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\kilib\textfile.h
# End Source File
# Begin Source File

SOURCE=.\kilib\thread.h
# End Source File
# Begin Source File

SOURCE=.\kilib\window.h
# End Source File
# Begin Source File

SOURCE=.\kilib\winutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
