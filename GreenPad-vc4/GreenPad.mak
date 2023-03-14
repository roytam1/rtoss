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

ALL : "$(OUTDIR)\GreenPad.exe" "$(OUTDIR)\GreenPad.bsc"

CLEAN : 
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\app.sbr"
	-@erase "$(INTDIR)\cmdarg.obj"
	-@erase "$(INTDIR)\cmdarg.sbr"
	-@erase "$(INTDIR)\ConfigManager.obj"
	-@erase "$(INTDIR)\ConfigManager.sbr"
	-@erase "$(INTDIR)\ctrl.obj"
	-@erase "$(INTDIR)\ctrl.sbr"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\file.sbr"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\find.sbr"
	-@erase "$(INTDIR)\gp_rsrc.res"
	-@erase "$(INTDIR)\GpMain.obj"
	-@erase "$(INTDIR)\GpMain.sbr"
	-@erase "$(INTDIR)\ip_ctrl1.obj"
	-@erase "$(INTDIR)\ip_ctrl1.sbr"
	-@erase "$(INTDIR)\ip_cursor.obj"
	-@erase "$(INTDIR)\ip_cursor.sbr"
	-@erase "$(INTDIR)\ip_draw.obj"
	-@erase "$(INTDIR)\ip_draw.sbr"
	-@erase "$(INTDIR)\ip_parse.obj"
	-@erase "$(INTDIR)\ip_parse.sbr"
	-@erase "$(INTDIR)\ip_scroll.obj"
	-@erase "$(INTDIR)\ip_scroll.sbr"
	-@erase "$(INTDIR)\ip_text.obj"
	-@erase "$(INTDIR)\ip_text.sbr"
	-@erase "$(INTDIR)\ip_wrap.obj"
	-@erase "$(INTDIR)\ip_wrap.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\OpenSaveDlg.obj"
	-@erase "$(INTDIR)\OpenSaveDlg.sbr"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\path.sbr"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.sbr"
	-@erase "$(INTDIR)\RSearch.obj"
	-@erase "$(INTDIR)\RSearch.sbr"
	-@erase "$(INTDIR)\Search.obj"
	-@erase "$(INTDIR)\Search.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\textfile.obj"
	-@erase "$(INTDIR)\textfile.sbr"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\thread.sbr"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\winutil.sbr"
	-@erase "$(OUTDIR)\GreenPad.bsc"
	-@erase "$(OUTDIR)\GreenPad.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "WinRel/"
# ADD F90 /I "WinRel/"
F90_PROJ=/I "WinRel/" /Fo"WinRel/" 
F90_OBJS=.\WinRel/
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "kilib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D "STRICT" /Fr /YX"stdafx.h" /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "kilib" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D\
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
	"$(INTDIR)\app.sbr" \
	"$(INTDIR)\cmdarg.sbr" \
	"$(INTDIR)\ConfigManager.sbr" \
	"$(INTDIR)\ctrl.sbr" \
	"$(INTDIR)\file.sbr" \
	"$(INTDIR)\find.sbr" \
	"$(INTDIR)\GpMain.sbr" \
	"$(INTDIR)\ip_ctrl1.sbr" \
	"$(INTDIR)\ip_cursor.sbr" \
	"$(INTDIR)\ip_draw.sbr" \
	"$(INTDIR)\ip_parse.sbr" \
	"$(INTDIR)\ip_scroll.sbr" \
	"$(INTDIR)\ip_text.sbr" \
	"$(INTDIR)\ip_wrap.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\OpenSaveDlg.sbr" \
	"$(INTDIR)\path.sbr" \
	"$(INTDIR)\registry.sbr" \
	"$(INTDIR)\RSearch.sbr" \
	"$(INTDIR)\Search.sbr" \
	"$(INTDIR)\stdafx.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\textfile.sbr" \
	"$(INTDIR)\thread.sbr" \
	"$(INTDIR)\window.sbr" \
	"$(INTDIR)\winutil.sbr"

"$(OUTDIR)\GreenPad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
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
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\cmdarg.obj" \
	"$(INTDIR)\ConfigManager.obj" \
	"$(INTDIR)\ctrl.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\find.obj" \
	"$(INTDIR)\gp_rsrc.res" \
	"$(INTDIR)\GpMain.obj" \
	"$(INTDIR)\ip_ctrl1.obj" \
	"$(INTDIR)\ip_cursor.obj" \
	"$(INTDIR)\ip_draw.obj" \
	"$(INTDIR)\ip_parse.obj" \
	"$(INTDIR)\ip_scroll.obj" \
	"$(INTDIR)\ip_text.obj" \
	"$(INTDIR)\ip_wrap.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\OpenSaveDlg.obj" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\RSearch.obj" \
	"$(INTDIR)\Search.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\textfile.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\winutil.obj"

"$(OUTDIR)\GreenPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "$(OUTDIR)\GreenPad.exe" "$(OUTDIR)\GreenPad.bsc"

CLEAN : 
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\app.sbr"
	-@erase "$(INTDIR)\cmdarg.obj"
	-@erase "$(INTDIR)\cmdarg.sbr"
	-@erase "$(INTDIR)\ConfigManager.obj"
	-@erase "$(INTDIR)\ConfigManager.sbr"
	-@erase "$(INTDIR)\ctrl.obj"
	-@erase "$(INTDIR)\ctrl.sbr"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\file.sbr"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\find.sbr"
	-@erase "$(INTDIR)\gp_rsrc.res"
	-@erase "$(INTDIR)\GpMain.obj"
	-@erase "$(INTDIR)\GpMain.sbr"
	-@erase "$(INTDIR)\ip_ctrl1.obj"
	-@erase "$(INTDIR)\ip_ctrl1.sbr"
	-@erase "$(INTDIR)\ip_cursor.obj"
	-@erase "$(INTDIR)\ip_cursor.sbr"
	-@erase "$(INTDIR)\ip_draw.obj"
	-@erase "$(INTDIR)\ip_draw.sbr"
	-@erase "$(INTDIR)\ip_parse.obj"
	-@erase "$(INTDIR)\ip_parse.sbr"
	-@erase "$(INTDIR)\ip_scroll.obj"
	-@erase "$(INTDIR)\ip_scroll.sbr"
	-@erase "$(INTDIR)\ip_text.obj"
	-@erase "$(INTDIR)\ip_text.sbr"
	-@erase "$(INTDIR)\ip_wrap.obj"
	-@erase "$(INTDIR)\ip_wrap.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\OpenSaveDlg.obj"
	-@erase "$(INTDIR)\OpenSaveDlg.sbr"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\path.sbr"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.sbr"
	-@erase "$(INTDIR)\RSearch.obj"
	-@erase "$(INTDIR)\RSearch.sbr"
	-@erase "$(INTDIR)\Search.obj"
	-@erase "$(INTDIR)\Search.sbr"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\stdafx.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\textfile.obj"
	-@erase "$(INTDIR)\textfile.sbr"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\thread.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\winutil.sbr"
	-@erase "$(OUTDIR)\GreenPad.bsc"
	-@erase "$(OUTDIR)\GreenPad.exe"
	-@erase "$(OUTDIR)\GreenPad.ilk"
	-@erase "$(OUTDIR)\GreenPad.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "WinDebug/"
# ADD F90 /FR /I "WinDebug/"
F90_PROJ=/FR"WinDebug/" /I "WinDebug/" /Fo"WinDebug/" 
F90_OBJS=.\WinDebug/
# ADD BASE CPP /nologo /ML /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /ML /W3 /Gm /GX /Zi /Od /I "kilib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D "STRICT" /Fr /YX"stdafx.h" /c
CPP_PROJ=/nologo /ML /W3 /Gm /GX /Zi /Od /I "kilib" /D "_DEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D\
 "STRICT" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/GreenPad.pch" /YX"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
	"$(INTDIR)\app.sbr" \
	"$(INTDIR)\cmdarg.sbr" \
	"$(INTDIR)\ConfigManager.sbr" \
	"$(INTDIR)\ctrl.sbr" \
	"$(INTDIR)\file.sbr" \
	"$(INTDIR)\find.sbr" \
	"$(INTDIR)\GpMain.sbr" \
	"$(INTDIR)\ip_ctrl1.sbr" \
	"$(INTDIR)\ip_cursor.sbr" \
	"$(INTDIR)\ip_draw.sbr" \
	"$(INTDIR)\ip_parse.sbr" \
	"$(INTDIR)\ip_scroll.sbr" \
	"$(INTDIR)\ip_text.sbr" \
	"$(INTDIR)\ip_wrap.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\OpenSaveDlg.sbr" \
	"$(INTDIR)\path.sbr" \
	"$(INTDIR)\registry.sbr" \
	"$(INTDIR)\RSearch.sbr" \
	"$(INTDIR)\Search.sbr" \
	"$(INTDIR)\stdafx.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\textfile.sbr" \
	"$(INTDIR)\thread.sbr" \
	"$(INTDIR)\window.sbr" \
	"$(INTDIR)\winutil.sbr"

"$(OUTDIR)\GreenPad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
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
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\cmdarg.obj" \
	"$(INTDIR)\ConfigManager.obj" \
	"$(INTDIR)\ctrl.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\find.obj" \
	"$(INTDIR)\gp_rsrc.res" \
	"$(INTDIR)\GpMain.obj" \
	"$(INTDIR)\ip_ctrl1.obj" \
	"$(INTDIR)\ip_cursor.obj" \
	"$(INTDIR)\ip_draw.obj" \
	"$(INTDIR)\ip_parse.obj" \
	"$(INTDIR)\ip_scroll.obj" \
	"$(INTDIR)\ip_text.obj" \
	"$(INTDIR)\ip_wrap.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\OpenSaveDlg.obj" \
	"$(INTDIR)\path.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\RSearch.obj" \
	"$(INTDIR)\Search.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\textfile.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\winutil.obj"

"$(OUTDIR)\GreenPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_parse.obj" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_parse.sbr" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

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
	

"$(INTDIR)\GpMain.obj" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"

"$(INTDIR)\GpMain.sbr" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\memory.cpp
DEP_CPP_MEMOR=\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\memory.sbr" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\find.sbr" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

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
	

"$(INTDIR)\OpenSaveDlg.obj" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"

"$(INTDIR)\OpenSaveDlg.sbr" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"


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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\app.sbr" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_text.obj" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_text.sbr" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\registry.sbr" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\path.sbr" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ctrl.obj" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrl.sbr" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_ctrl1.obj" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_ctrl1.sbr" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_wrap.obj" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_wrap.sbr" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

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
	

"$(INTDIR)\RSearch.obj" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"

"$(INTDIR)\RSearch.sbr" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"


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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_cursor.obj" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_cursor.sbr" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\thread.cpp
DEP_CPP_THREA=\
	".\kilib\stdafx.h"\
	".\kilib\thread.h"\
	".\kilib\yvals.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\thread.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\thread.sbr" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\textfile.obj" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\textfile.sbr" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\string.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\string.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\window.sbr" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\stdafx.cpp
DEP_CPP_STDAF=\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_scroll.obj" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_scroll.sbr" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\log.sbr" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

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
	

"$(INTDIR)\ConfigManager.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"

"$(INTDIR)\ConfigManager.sbr" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\winutil.obj" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\winutil.sbr" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cmdarg.obj" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmdarg.sbr" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\file.cpp
DEP_CPP_FILE_=\
	".\kilib\file.h"\
	".\kilib\stdafx.h"\
	".\kilib\yvals.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\file.sbr" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_draw.obj" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_draw.sbr" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

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
	

"$(INTDIR)\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"

"$(INTDIR)\Search.sbr" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rsrc\gp_rsrc.rc

!IF  "$(CFG)" == "GreenPad - Win32 Release"


"$(INTDIR)\gp_rsrc.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /i "rsrc" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\gp_rsrc.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) /l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /i "rsrc" /d "_DEBUG" $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
