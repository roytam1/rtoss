# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=GreenPad - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to GreenPad - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GreenPad - Win32 Release" && "$(CFG)" !=\
 "GreenPad - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "GreenPad.mak" CFG="GreenPad - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GreenPad - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GreenPad - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "GreenPad - Win32 Release"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe
F90=fl32.exe

!IF  "$(CFG)" == "GreenPad - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : ".\WinRel\GreenPad.exe" ".\WinRel\GreenPad.bsc"

CLEAN : 
	-@erase ".\WinRel\app.obj"
	-@erase ".\WinRel\app.sbr"
	-@erase ".\WinRel\cmdarg.obj"
	-@erase ".\WinRel\cmdarg.sbr"
	-@erase ".\WinRel\ConfigManager.obj"
	-@erase ".\WinRel\ConfigManager.sbr"
	-@erase ".\WinRel\ctrl.obj"
	-@erase ".\WinRel\ctrl.sbr"
	-@erase ".\WinRel\file.obj"
	-@erase ".\WinRel\file.sbr"
	-@erase ".\WinRel\find.obj"
	-@erase ".\WinRel\find.sbr"
	-@erase ".\WinRel\gp_rsrc.res"
	-@erase ".\WinRel\GpMain.obj"
	-@erase ".\WinRel\GpMain.sbr"
	-@erase ".\WinRel\GreenPad.bsc"
	-@erase ".\WinRel\GreenPad.exe"
	-@erase ".\WinRel\ip_ctrl1.obj"
	-@erase ".\WinRel\ip_ctrl1.sbr"
	-@erase ".\WinRel\ip_cursor.obj"
	-@erase ".\WinRel\ip_cursor.sbr"
	-@erase ".\WinRel\ip_draw.obj"
	-@erase ".\WinRel\ip_draw.sbr"
	-@erase ".\WinRel\ip_parse.obj"
	-@erase ".\WinRel\ip_parse.sbr"
	-@erase ".\WinRel\ip_scroll.obj"
	-@erase ".\WinRel\ip_scroll.sbr"
	-@erase ".\WinRel\ip_text.obj"
	-@erase ".\WinRel\ip_text.sbr"
	-@erase ".\WinRel\ip_wrap.obj"
	-@erase ".\WinRel\ip_wrap.sbr"
	-@erase ".\WinRel\log.obj"
	-@erase ".\WinRel\log.sbr"
	-@erase ".\WinRel\memory.obj"
	-@erase ".\WinRel\memory.sbr"
	-@erase ".\WinRel\OpenSaveDlg.obj"
	-@erase ".\WinRel\OpenSaveDlg.sbr"
	-@erase ".\WinRel\path.obj"
	-@erase ".\WinRel\path.sbr"
	-@erase ".\WinRel\registry.obj"
	-@erase ".\WinRel\registry.sbr"
	-@erase ".\WinRel\RSearch.obj"
	-@erase ".\WinRel\RSearch.sbr"
	-@erase ".\WinRel\Search.obj"
	-@erase ".\WinRel\Search.sbr"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\stdafx.sbr"
	-@erase ".\WinRel\string.obj"
	-@erase ".\WinRel\string.sbr"
	-@erase ".\WinRel\textfile.obj"
	-@erase ".\WinRel\textfile.sbr"
	-@erase ".\WinRel\thread.obj"
	-@erase ".\WinRel\thread.sbr"
	-@erase ".\WinRel\window.obj"
	-@erase ".\WinRel\window.sbr"
	-@erase ".\WinRel\winutil.obj"
	-@erase ".\WinRel\winutil.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "WinRel/"
# ADD F90 /I "WinRel/"
F90_PROJ=/I "WinRel/" /Fo"WinRel/" 
F90_OBJS=.\WinRel/
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".." /I "..\kilib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D "STRICT" /Fr /YX"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".." /I "..\kilib" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D\
 "STRICT" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/GreenPad.pch" /YX"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=.\WinRel/
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GreenPad.bsc" 
BSC32_SBRS= \
	".\WinRel\app.sbr" \
	".\WinRel\cmdarg.sbr" \
	".\WinRel\ConfigManager.sbr" \
	".\WinRel\ctrl.sbr" \
	".\WinRel\file.sbr" \
	".\WinRel\find.sbr" \
	".\WinRel\GpMain.sbr" \
	".\WinRel\ip_ctrl1.sbr" \
	".\WinRel\ip_cursor.sbr" \
	".\WinRel\ip_draw.sbr" \
	".\WinRel\ip_parse.sbr" \
	".\WinRel\ip_scroll.sbr" \
	".\WinRel\ip_text.sbr" \
	".\WinRel\ip_wrap.sbr" \
	".\WinRel\log.sbr" \
	".\WinRel\memory.sbr" \
	".\WinRel\OpenSaveDlg.sbr" \
	".\WinRel\path.sbr" \
	".\WinRel\registry.sbr" \
	".\WinRel\RSearch.sbr" \
	".\WinRel\Search.sbr" \
	".\WinRel\stdafx.sbr" \
	".\WinRel\string.sbr" \
	".\WinRel\textfile.sbr" \
	".\WinRel\thread.sbr" \
	".\WinRel\window.sbr" \
	".\WinRel\winutil.sbr"

".\WinRel\GreenPad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/GreenPad.pdb" /machine:I386 /out:"$(OUTDIR)/GreenPad.exe" 
LINK32_OBJS= \
	".\WinRel\app.obj" \
	".\WinRel\cmdarg.obj" \
	".\WinRel\ConfigManager.obj" \
	".\WinRel\ctrl.obj" \
	".\WinRel\file.obj" \
	".\WinRel\find.obj" \
	".\WinRel\gp_rsrc.res" \
	".\WinRel\GpMain.obj" \
	".\WinRel\ip_ctrl1.obj" \
	".\WinRel\ip_cursor.obj" \
	".\WinRel\ip_draw.obj" \
	".\WinRel\ip_parse.obj" \
	".\WinRel\ip_scroll.obj" \
	".\WinRel\ip_text.obj" \
	".\WinRel\ip_wrap.obj" \
	".\WinRel\log.obj" \
	".\WinRel\memory.obj" \
	".\WinRel\OpenSaveDlg.obj" \
	".\WinRel\path.obj" \
	".\WinRel\registry.obj" \
	".\WinRel\RSearch.obj" \
	".\WinRel\Search.obj" \
	".\WinRel\stdafx.obj" \
	".\WinRel\string.obj" \
	".\WinRel\textfile.obj" \
	".\WinRel\thread.obj" \
	".\WinRel\window.obj" \
	".\WinRel\winutil.obj"

".\WinRel\GreenPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : ".\WinDebug\GreenPad.exe" ".\WinDebug\GreenPad.bsc"

CLEAN : 
	-@erase ".\WinDebug\app.obj"
	-@erase ".\WinDebug\app.sbr"
	-@erase ".\WinDebug\cmdarg.obj"
	-@erase ".\WinDebug\cmdarg.sbr"
	-@erase ".\WinDebug\ConfigManager.obj"
	-@erase ".\WinDebug\ConfigManager.sbr"
	-@erase ".\WinDebug\ctrl.obj"
	-@erase ".\WinDebug\ctrl.sbr"
	-@erase ".\WinDebug\file.obj"
	-@erase ".\WinDebug\file.sbr"
	-@erase ".\WinDebug\find.obj"
	-@erase ".\WinDebug\find.sbr"
	-@erase ".\WinDebug\gp_rsrc.res"
	-@erase ".\WinDebug\GpMain.obj"
	-@erase ".\WinDebug\GpMain.sbr"
	-@erase ".\WinDebug\GreenPad.bsc"
	-@erase ".\WinDebug\GreenPad.exe"
	-@erase ".\WinDebug\GreenPad.ilk"
	-@erase ".\WinDebug\GreenPad.pdb"
	-@erase ".\WinDebug\ip_ctrl1.obj"
	-@erase ".\WinDebug\ip_ctrl1.sbr"
	-@erase ".\WinDebug\ip_cursor.obj"
	-@erase ".\WinDebug\ip_cursor.sbr"
	-@erase ".\WinDebug\ip_draw.obj"
	-@erase ".\WinDebug\ip_draw.sbr"
	-@erase ".\WinDebug\ip_parse.obj"
	-@erase ".\WinDebug\ip_parse.sbr"
	-@erase ".\WinDebug\ip_scroll.obj"
	-@erase ".\WinDebug\ip_scroll.sbr"
	-@erase ".\WinDebug\ip_text.obj"
	-@erase ".\WinDebug\ip_text.sbr"
	-@erase ".\WinDebug\ip_wrap.obj"
	-@erase ".\WinDebug\ip_wrap.sbr"
	-@erase ".\WinDebug\log.obj"
	-@erase ".\WinDebug\log.sbr"
	-@erase ".\WinDebug\memory.obj"
	-@erase ".\WinDebug\memory.sbr"
	-@erase ".\WinDebug\OpenSaveDlg.obj"
	-@erase ".\WinDebug\OpenSaveDlg.sbr"
	-@erase ".\WinDebug\path.obj"
	-@erase ".\WinDebug\path.sbr"
	-@erase ".\WinDebug\registry.obj"
	-@erase ".\WinDebug\registry.sbr"
	-@erase ".\WinDebug\RSearch.obj"
	-@erase ".\WinDebug\RSearch.sbr"
	-@erase ".\WinDebug\Search.obj"
	-@erase ".\WinDebug\Search.sbr"
	-@erase ".\WinDebug\stdafx.obj"
	-@erase ".\WinDebug\stdafx.sbr"
	-@erase ".\WinDebug\string.obj"
	-@erase ".\WinDebug\string.sbr"
	-@erase ".\WinDebug\textfile.obj"
	-@erase ".\WinDebug\textfile.sbr"
	-@erase ".\WinDebug\thread.obj"
	-@erase ".\WinDebug\thread.sbr"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\window.obj"
	-@erase ".\WinDebug\window.sbr"
	-@erase ".\WinDebug\winutil.obj"
	-@erase ".\WinDebug\winutil.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "WinDebug/"
# ADD F90 /FR /I "WinDebug/"
F90_PROJ=/FR"WinDebug/" /I "WinDebug/" /Fo"WinDebug/" 
F90_OBJS=.\WinDebug/
# ADD BASE CPP /nologo /ML /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MT /W3 /Gm /GX /Zi /Od /I ".." /I "..\kilib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D "STRICT" /Fr /YX"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /Gm /GX /Zi /Od /I ".." /I "..\kilib" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D\
 TARGET_VER=310 /D "STRICT" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/GreenPad.pch"\
 /YX"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=.\WinDebug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GreenPad.bsc" 
BSC32_SBRS= \
	".\WinDebug\app.sbr" \
	".\WinDebug\cmdarg.sbr" \
	".\WinDebug\ConfigManager.sbr" \
	".\WinDebug\ctrl.sbr" \
	".\WinDebug\file.sbr" \
	".\WinDebug\find.sbr" \
	".\WinDebug\GpMain.sbr" \
	".\WinDebug\ip_ctrl1.sbr" \
	".\WinDebug\ip_cursor.sbr" \
	".\WinDebug\ip_draw.sbr" \
	".\WinDebug\ip_parse.sbr" \
	".\WinDebug\ip_scroll.sbr" \
	".\WinDebug\ip_text.sbr" \
	".\WinDebug\ip_wrap.sbr" \
	".\WinDebug\log.sbr" \
	".\WinDebug\memory.sbr" \
	".\WinDebug\OpenSaveDlg.sbr" \
	".\WinDebug\path.sbr" \
	".\WinDebug\registry.sbr" \
	".\WinDebug\RSearch.sbr" \
	".\WinDebug\Search.sbr" \
	".\WinDebug\stdafx.sbr" \
	".\WinDebug\string.sbr" \
	".\WinDebug\textfile.sbr" \
	".\WinDebug\thread.sbr" \
	".\WinDebug\window.sbr" \
	".\WinDebug\winutil.sbr"

".\WinDebug\GreenPad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/GreenPad.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/GreenPad.exe" 
LINK32_OBJS= \
	".\WinDebug\app.obj" \
	".\WinDebug\cmdarg.obj" \
	".\WinDebug\ConfigManager.obj" \
	".\WinDebug\ctrl.obj" \
	".\WinDebug\file.obj" \
	".\WinDebug\find.obj" \
	".\WinDebug\gp_rsrc.res" \
	".\WinDebug\GpMain.obj" \
	".\WinDebug\ip_ctrl1.obj" \
	".\WinDebug\ip_cursor.obj" \
	".\WinDebug\ip_draw.obj" \
	".\WinDebug\ip_parse.obj" \
	".\WinDebug\ip_scroll.obj" \
	".\WinDebug\ip_text.obj" \
	".\WinDebug\ip_wrap.obj" \
	".\WinDebug\log.obj" \
	".\WinDebug\memory.obj" \
	".\WinDebug\OpenSaveDlg.obj" \
	".\WinDebug\path.obj" \
	".\WinDebug\registry.obj" \
	".\WinDebug\RSearch.obj" \
	".\WinDebug\Search.obj" \
	".\WinDebug\stdafx.obj" \
	".\WinDebug\string.obj" \
	".\WinDebug\textfile.obj" \
	".\WinDebug\thread.obj" \
	".\WinDebug\window.obj" \
	".\WinDebug\winutil.obj"

".\WinDebug\GreenPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.for{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

.f90{$(F90_OBJS)}.obj:
   $(F90) $(F90_PROJ) $<  

################################################################################
# Begin Target

# Name "GreenPad - Win32 Release"
# Name "GreenPad - Win32 Debug"

!IF  "$(CFG)" == "GreenPad - Win32 Release"

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\editwing\ip_parse.cpp
DEP_CPP_IP_PA=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ip_doc.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_IP_PA=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ip_parse.obj" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ip_parse.sbr" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ip_parse.obj" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ip_parse.sbr" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GpMain.cpp
DEP_CPP_GPMAI=\
	".\ConfigManager.h"\
	".\editwing\editwing.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\GpMain.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	".\OpenSaveDlg.h"\
	".\Search.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


".\WinRel\GpMain.obj" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"

".\WinRel\GpMain.sbr" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


".\WinDebug\GpMain.obj" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"

".\WinDebug\GpMain.sbr" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\memory.cpp
DEP_CPP_MEMOR=\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\memory.sbr" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\memory.sbr" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\find.cpp
DEP_CPP_FIND_=\
	".\kilib\find.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_FIND_=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\find.sbr" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\find.sbr" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OpenSaveDlg.cpp
DEP_CPP_OPENS=\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	".\OpenSaveDlg.h"\
	
NODEP_CPP_OPENS=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


".\WinRel\OpenSaveDlg.obj" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"

".\WinRel\OpenSaveDlg.sbr" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


".\WinDebug\OpenSaveDlg.obj" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"

".\WinDebug\OpenSaveDlg.sbr" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\app.cpp
DEP_CPP_APP_C=\
	".\kilib\app.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_APP_C=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\app.sbr" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\app.sbr" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_text.cpp
DEP_CPP_IP_TE=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ip_doc.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_IP_TE=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ip_text.obj" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ip_text.sbr" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ip_text.obj" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ip_text.sbr" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\registry.cpp
DEP_CPP_REGIS=\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_REGIS=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\registry.sbr" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\registry.sbr" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\path.cpp
DEP_CPP_PATH_=\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_PATH_=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\path.sbr" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\path.sbr" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\ctrl.cpp
DEP_CPP_CTRL_=\
	".\kilib\app.h"\
	".\kilib\ctrl.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\log.h"\
	".\kilib\stdafx.h"\
	".\kilib\window.h"\
	".\kilib\yvals.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ctrl.obj" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ctrl.sbr" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ctrl.obj" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ctrl.sbr" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_ctrl1.cpp
DEP_CPP_IP_CT=\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_IP_CT=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ip_ctrl1.obj" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ip_ctrl1.sbr" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ip_ctrl1.obj" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ip_ctrl1.sbr" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_wrap.cpp
DEP_CPP_IP_WR=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ip_view.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_IP_WR=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ip_wrap.obj" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ip_wrap.sbr" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ip_wrap.obj" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ip_wrap.sbr" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RSearch.cpp
DEP_CPP_RSEAR=\
	".\editwing\editwing.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	".\NSearch.h"\
	".\RSearch.h"\
	".\Search.h"\
	
NODEP_CPP_RSEAR=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


".\WinRel\RSearch.obj" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"

".\WinRel\RSearch.sbr" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


".\WinDebug\RSearch.obj" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"

".\WinDebug\RSearch.sbr" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_cursor.cpp
DEP_CPP_IP_CU=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ip_view.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_IP_CU=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ip_cursor.obj" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ip_cursor.sbr" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ip_cursor.obj" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ip_cursor.sbr" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\thread.cpp
DEP_CPP_THREA=\
	".\kilib\stdafx.h"\
	".\kilib\thread.h"\
	".\kilib\yvals.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\thread.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\thread.sbr" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\thread.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\thread.sbr" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\textfile.cpp
DEP_CPP_TEXTF=\
	".\kilib\app.h"\
	".\kilib\file.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_TEXTF=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\textfile.obj" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\textfile.sbr" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\textfile.obj" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\textfile.sbr" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\string.cpp
DEP_CPP_STRIN=\
	".\kilib\app.h"\
	".\kilib\log.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_STRIN=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\string.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\string.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\string.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\string.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\window.cpp
DEP_CPP_WINDO=\
	".\kilib\app.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\log.h"\
	".\kilib\stdafx.h"\
	".\kilib\window.h"\
	".\kilib\yvals.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\window.sbr" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\window.sbr" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\stdafx.cpp
DEP_CPP_STDAF=\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_scroll.cpp
DEP_CPP_IP_SC=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ip_view.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_IP_SC=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ip_scroll.obj" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ip_scroll.sbr" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ip_scroll.obj" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ip_scroll.sbr" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\log.cpp
DEP_CPP_LOG_C=\
	".\kilib\app.h"\
	".\kilib\log.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_LOG_C=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\log.sbr" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\log.sbr" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ConfigManager.cpp
DEP_CPP_CONFI=\
	".\ConfigManager.h"\
	".\editwing\editwing.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	".\NSearch.h"\
	".\OpenSaveDlg.h"\
	".\RSearch.h"\
	".\Search.h"\
	
NODEP_CPP_CONFI=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


".\WinRel\ConfigManager.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"

".\WinRel\ConfigManager.sbr" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


".\WinDebug\ConfigManager.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"

".\WinDebug\ConfigManager.sbr" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\winutil.cpp
DEP_CPP_WINUT=\
	".\kilib\app.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\log.h"\
	".\kilib\stdafx.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\winutil.obj" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\winutil.sbr" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\winutil.obj" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\winutil.sbr" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\cmdarg.cpp
DEP_CPP_CMDAR=\
	".\kilib\cmdarg.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_CMDAR=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\cmdarg.obj" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\cmdarg.sbr" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\cmdarg.obj" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\cmdarg.sbr" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\file.cpp
DEP_CPP_FILE_=\
	".\kilib\file.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\file.sbr" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\file.sbr" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_draw.cpp
DEP_CPP_IP_DR=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ip_view.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	
NODEP_CPP_IP_DR=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinRel\ip_draw.obj" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

".\WinRel\ip_draw.sbr" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\WinDebug\ip_draw.obj" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

".\WinDebug\ip_draw.sbr" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Search.cpp
DEP_CPP_SEARC=\
	".\editwing\editwing.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\find.h"\
	".\kilib\kilib.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	".\NSearch.h"\
	".\RSearch.h"\
	".\Search.h"\
	
NODEP_CPP_SEARC=\
	".\kilib\kstring.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


".\WinRel\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"

".\WinRel\Search.sbr" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


".\WinDebug\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"

".\WinDebug\Search.sbr" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\rsrc\gp_rsrc.rc

!IF  "$(CFG)" == "GreenPad - Win32 Release"


".\WinRel\gp_rsrc.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /i "rsrc" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


".\WinDebug\gp_rsrc.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /i "rsrc" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
