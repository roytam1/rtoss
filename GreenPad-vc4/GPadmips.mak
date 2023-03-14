# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (MIPS) Application" 0x0501

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
!MESSAGE NMAKE /f "GPadmips.mak" CFG="GreenPad - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GreenPad - Win32 Release" (based on "Win32 (MIPS) Application")
!MESSAGE "GreenPad - Win32 Debug" (based on "Win32 (MIPS) Application")
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
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "GreenPad - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MIPSRel"
# PROP BASE Intermediate_Dir "MIPSRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MIPSRel"
# PROP Intermediate_Dir "MIPSRel"
# PROP Target_Dir ""
OUTDIR=.\MIPSRel
INTDIR=.\MIPSRel

ALL : "$(OUTDIR)\GPadmips.exe" "$(OUTDIR)\GPadmips.bsc"

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
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\winutil.sbr"
	-@erase "$(OUTDIR)\GPadmips.bsc"
	-@erase "$(OUTDIR)\GPadmips.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /Gt0 /QMOb2000 /W3 /Zi /O1 /I "kilib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "STRICT" /D "NO_MLANG" /D TARGET_VER=310 /D "NO_ASMTHUNK" /FR /YX"stdafx.h" /c
CPP_PROJ=/nologo /ML /Gt0 /QMOb2000 /W3 /Zi /O1 /I "kilib" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "STRICT" /D "NO_MLANG" /D\
 TARGET_VER=310 /D "NO_ASMTHUNK" /FR"$(INTDIR)/" /Fp"$(INTDIR)/GPadmips.pch"\
 /YX"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MIPSRel/
CPP_SBRS=.\MIPSRel/
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /d "NDEBUG" 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /machine:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/GPadmips.pdb" /machine:MIPS /out:"$(OUTDIR)/GPadmips.exe" 
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

"$(OUTDIR)\GPadmips.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GPadmips.bsc" 
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

"$(OUTDIR)\GPadmips.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MIPSDbg"
# PROP BASE Intermediate_Dir "MIPSDbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MIPSDbg"
# PROP Intermediate_Dir "MIPSDbg"
# PROP Target_Dir ""
OUTDIR=.\MIPSDbg
INTDIR=.\MIPSDbg

ALL : "$(OUTDIR)\GPadmips.exe"

CLEAN : 
	-@erase "$(INTDIR)\app.obj"
	-@erase "$(INTDIR)\cmdarg.obj"
	-@erase "$(INTDIR)\ConfigManager.obj"
	-@erase "$(INTDIR)\ctrl.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\find.obj"
	-@erase "$(INTDIR)\gp_rsrc.res"
	-@erase "$(INTDIR)\GpMain.obj"
	-@erase "$(INTDIR)\ip_ctrl1.obj"
	-@erase "$(INTDIR)\ip_cursor.obj"
	-@erase "$(INTDIR)\ip_draw.obj"
	-@erase "$(INTDIR)\ip_parse.obj"
	-@erase "$(INTDIR)\ip_scroll.obj"
	-@erase "$(INTDIR)\ip_text.obj"
	-@erase "$(INTDIR)\ip_wrap.obj"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\OpenSaveDlg.obj"
	-@erase "$(INTDIR)\path.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\RSearch.obj"
	-@erase "$(INTDIR)\Search.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\textfile.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(OUTDIR)\GPadmips.exe"
	-@erase "$(OUTDIR)\GPadmips.ilk"
	-@erase "$(OUTDIR)\GPadmips.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
# ADD BASE CPP /nologo /ML /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /ML /Gt0 /QMOb2000 /W3 /Zi /O1 /I "kilib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "STRICT" /D "NO_MLANG" /D TARGET_VER=310 /D "NO_ASMTHUNK" /YX"stdafx.h" /c
CPP_PROJ=/nologo /ML /Gt0 /QMOb2000 /W3 /Zi /O1 /I "kilib" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "STRICT" /D "NO_MLANG" /D\
 TARGET_VER=310 /D "NO_ASMTHUNK" /Fp"$(INTDIR)/GPadmips.pch" /YX"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MIPSDbg/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /d "_DEBUG" 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/GPadmips.pdb" /debug /machine:MIPS\
 /out:"$(OUTDIR)/GPadmips.exe" 
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

"$(OUTDIR)\GPadmips.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GPadmips.bsc" 
BSC32_SBRS= \
	

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

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\kstring.h"\
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
	".\kilib\YVALS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_parse.obj" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_parse.sbr" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

DEP_CPP_IP_PA=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ip_doc.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	

"$(INTDIR)\ip_parse.obj" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GpMain.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\YVALS.H"\
	".\OpenSaveDlg.h"\
	".\Search.h"\
	

"$(INTDIR)\GpMain.obj" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"

"$(INTDIR)\GpMain.sbr" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	".\OpenSaveDlg.h"\
	".\Search.h"\
	

"$(INTDIR)\GpMain.obj" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\memory.cpp
DEP_CPP_MEMOR=\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\memory.sbr" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\find.cpp
DEP_CPP_FIND_=\
	".\kilib\find.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\find.sbr" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\find.obj" : $(SOURCE) $(DEP_CPP_FIND_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\OpenSaveDlg.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

DEP_CPP_OPENS=\
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
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	".\OpenSaveDlg.h"\
	

"$(INTDIR)\OpenSaveDlg.obj" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"

"$(INTDIR)\OpenSaveDlg.sbr" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

DEP_CPP_OPENS=\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	".\OpenSaveDlg.h"\
	

"$(INTDIR)\OpenSaveDlg.obj" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\app.cpp
DEP_CPP_APP_C=\
	".\kilib\app.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\app.sbr" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_text.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\kstring.h"\
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
	".\kilib\YVALS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_text.obj" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_text.sbr" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

DEP_CPP_IP_TE=\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ip_doc.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	

"$(INTDIR)\ip_text.obj" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\registry.cpp
DEP_CPP_REGIS=\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\path.h"\
	".\kilib\registry.h"\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\registry.sbr" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\registry.obj" : $(SOURCE) $(DEP_CPP_REGIS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\path.cpp
DEP_CPP_PATH_=\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\path.sbr" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\path.obj" : $(SOURCE) $(DEP_CPP_PATH_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ctrl.obj" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ctrl.sbr" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ctrl.obj" : $(SOURCE) $(DEP_CPP_CTRL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_ctrl1.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\kstring.h"\
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
	".\kilib\YVALS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_ctrl1.obj" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_ctrl1.sbr" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

DEP_CPP_IP_CT=\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\kilib\app.h"\
	".\kilib\cmdarg.h"\
	".\kilib\ctrl.h"\
	".\kilib\file.h"\
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	

"$(INTDIR)\ip_ctrl1.obj" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_wrap.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\kstring.h"\
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
	".\kilib\YVALS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_wrap.obj" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_wrap.sbr" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	

"$(INTDIR)\ip_wrap.obj" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RSearch.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\YVALS.H"\
	".\NSearch.h"\
	".\RSearch.h"\
	".\Search.h"\
	

"$(INTDIR)\RSearch.obj" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"

"$(INTDIR)\RSearch.sbr" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	".\NSearch.h"\
	".\RSearch.h"\
	".\Search.h"\
	

"$(INTDIR)\RSearch.obj" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_cursor.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\kstring.h"\
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
	".\kilib\YVALS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_cursor.obj" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_cursor.sbr" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	

"$(INTDIR)\ip_cursor.obj" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\thread.cpp
DEP_CPP_THREA=\
	".\kilib\stdafx.h"\
	".\kilib\thread.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\thread.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\thread.sbr" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\thread.obj" : $(SOURCE) $(DEP_CPP_THREA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\textfile.cpp
DEP_CPP_TEXTF=\
	".\kilib\app.h"\
	".\kilib\file.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\textfile.obj" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\textfile.sbr" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\textfile.obj" : $(SOURCE) $(DEP_CPP_TEXTF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\string.cpp
DEP_CPP_STRIN=\
	".\kilib\app.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\log.h"\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\string.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\string.sbr" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\string.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\window.sbr" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\stdafx.cpp
DEP_CPP_STDAF=\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_scroll.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\kstring.h"\
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
	".\kilib\YVALS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_scroll.obj" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_scroll.sbr" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	

"$(INTDIR)\ip_scroll.obj" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\log.cpp
DEP_CPP_LOG_C=\
	".\kilib\app.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\log.h"\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\log.sbr" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\log.obj" : $(SOURCE) $(DEP_CPP_LOG_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ConfigManager.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\YVALS.H"\
	".\NSearch.h"\
	".\OpenSaveDlg.h"\
	".\RSearch.h"\
	".\Search.h"\
	

"$(INTDIR)\ConfigManager.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"

"$(INTDIR)\ConfigManager.sbr" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	".\NSearch.h"\
	".\OpenSaveDlg.h"\
	".\RSearch.h"\
	".\Search.h"\
	

"$(INTDIR)\ConfigManager.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


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
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\winutil.obj" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\winutil.sbr" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\winutil.obj" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\cmdarg.cpp
DEP_CPP_CMDAR=\
	".\kilib\cmdarg.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cmdarg.obj" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\cmdarg.sbr" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\cmdarg.obj" : $(SOURCE) $(DEP_CPP_CMDAR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\file.cpp
DEP_CPP_FILE_=\
	".\kilib\file.h"\
	".\kilib\stdafx.h"\
	".\kilib\YVALS.H"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\file.sbr" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_draw.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\kstring.h"\
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
	".\kilib\YVALS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_draw.obj" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_draw.sbr" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	

"$(INTDIR)\ip_draw.obj" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Search.cpp

!IF  "$(CFG)" == "GreenPad - Win32 Release"

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
	".\kilib\YVALS.H"\
	".\NSearch.h"\
	".\RSearch.h"\
	".\Search.h"\
	

"$(INTDIR)\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"

"$(INTDIR)\Search.sbr" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"

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
	".\kilib\kilib.h"\
	".\kilib\kstring.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\log.h"\
	".\kilib\memory.h"\
	".\kilib\path.h"\
	".\kilib\stdafx.h"\
	".\kilib\textfile.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\winutil.h"\
	".\kilib\YVALS.H"\
	".\NSearch.h"\
	".\RSearch.h"\
	".\Search.h"\
	

"$(INTDIR)\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"


!ENDIF 

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
