# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
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
!MESSAGE NMAKE /f "GreenPad_mips.mak" CFG="GreenPad - Win32 Debug"
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
# PROP Target_Last_Scanned "GreenPad - Win32 Debug"
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

ALL : "$(OUTDIR)\GreenPad_mips.exe" "$(OUTDIR)\GreenPad_mips.bsc"

CLEAN : 
	-@erase ".\MIPSRel\GreenPad_mips.exe"
	-@erase ".\MIPSRel\ip_text.obj"
	-@erase ".\MIPSRel\GpMain.obj"
	-@erase ".\MIPSRel\find.obj"
	-@erase ".\MIPSRel\ip_parse.obj"
	-@erase ".\MIPSRel\ip_ctrl1.obj"
	-@erase ".\MIPSRel\ip_draw.obj"
	-@erase ".\MIPSRel\ip_wrap.obj"
	-@erase ".\MIPSRel\ip_cursor.obj"
	-@erase ".\MIPSRel\ctrl.obj"
	-@erase ".\MIPSRel\log.obj"
	-@erase ".\MIPSRel\OpenSaveDlg.obj"
	-@erase ".\MIPSRel\app.obj"
	-@erase ".\MIPSRel\Search.obj"
	-@erase ".\MIPSRel\memory.obj"
	-@erase ".\MIPSRel\string.obj"
	-@erase ".\MIPSRel\file.obj"
	-@erase ".\MIPSRel\ip_scroll.obj"
	-@erase ".\MIPSRel\cmdarg.obj"
	-@erase ".\MIPSRel\path.obj"
	-@erase ".\MIPSRel\ConfigManager.obj"
	-@erase ".\MIPSRel\window.obj"
	-@erase ".\MIPSRel\textfile.obj"
	-@erase ".\MIPSRel\stdafx.obj"
	-@erase ".\MIPSRel\winutil.obj"
	-@erase ".\MIPSRel\RSearch.obj"
	-@erase ".\MIPSRel\thread.obj"
	-@erase ".\MIPSRel\registry.obj"
	-@erase ".\MIPSRel\gp_rsrc.res"
	-@erase ".\MIPSRel\GreenPad_mips.bsc"
	-@erase ".\MIPSRel\find.sbr"
	-@erase ".\MIPSRel\ip_parse.sbr"
	-@erase ".\MIPSRel\ip_ctrl1.sbr"
	-@erase ".\MIPSRel\ip_draw.sbr"
	-@erase ".\MIPSRel\ip_wrap.sbr"
	-@erase ".\MIPSRel\ip_cursor.sbr"
	-@erase ".\MIPSRel\ctrl.sbr"
	-@erase ".\MIPSRel\log.sbr"
	-@erase ".\MIPSRel\OpenSaveDlg.sbr"
	-@erase ".\MIPSRel\app.sbr"
	-@erase ".\MIPSRel\Search.sbr"
	-@erase ".\MIPSRel\memory.sbr"
	-@erase ".\MIPSRel\string.sbr"
	-@erase ".\MIPSRel\file.sbr"
	-@erase ".\MIPSRel\ip_scroll.sbr"
	-@erase ".\MIPSRel\cmdarg.sbr"
	-@erase ".\MIPSRel\path.sbr"
	-@erase ".\MIPSRel\ConfigManager.sbr"
	-@erase ".\MIPSRel\window.sbr"
	-@erase ".\MIPSRel\textfile.sbr"
	-@erase ".\MIPSRel\stdafx.sbr"
	-@erase ".\MIPSRel\winutil.sbr"
	-@erase ".\MIPSRel\RSearch.sbr"
	-@erase ".\MIPSRel\thread.sbr"
	-@erase ".\MIPSRel\registry.sbr"
	-@erase ".\MIPSRel\ip_text.sbr"
	-@erase ".\MIPSRel\GpMain.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "NDEBUG" /mips
# ADD MTL /nologo /D "NDEBUG" /mips
MTL_PROJ=/nologo /D "NDEBUG" /mips 
# ADD BASE CPP /nologo /Gt0 /QMOb2000 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /O2 /I ".." /I "..\kilib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /Fr /YX"stdafx.h" /c
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /O2 /I ".." /I "..\kilib" /D\
 "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D\
 TARGET_VER=310 /Fr"$(INTDIR)/" /Fp"$(INTDIR)/GreenPad_mips.pch" /YX"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
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
 /pdb:"$(OUTDIR)/GreenPad_mips.pdb" /machine:MIPS\
 /out:"$(OUTDIR)/GreenPad_mips.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ip_text.obj" \
	"$(INTDIR)/GpMain.obj" \
	"$(INTDIR)/find.obj" \
	"$(INTDIR)/ip_parse.obj" \
	"$(INTDIR)/ip_ctrl1.obj" \
	"$(INTDIR)/ip_draw.obj" \
	"$(INTDIR)/ip_wrap.obj" \
	"$(INTDIR)/ip_cursor.obj" \
	"$(INTDIR)/ctrl.obj" \
	"$(INTDIR)/log.obj" \
	"$(INTDIR)/OpenSaveDlg.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/Search.obj" \
	"$(INTDIR)/memory.obj" \
	"$(INTDIR)/string.obj" \
	"$(INTDIR)/file.obj" \
	"$(INTDIR)/ip_scroll.obj" \
	"$(INTDIR)/cmdarg.obj" \
	"$(INTDIR)/path.obj" \
	"$(INTDIR)/ConfigManager.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/textfile.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/winutil.obj" \
	"$(INTDIR)/RSearch.obj" \
	"$(INTDIR)/thread.obj" \
	"$(INTDIR)/registry.obj" \
	"$(INTDIR)/gp_rsrc.res"

"$(OUTDIR)\GreenPad_mips.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GreenPad_mips.bsc" 
BSC32_SBRS= \
	"$(INTDIR)/find.sbr" \
	"$(INTDIR)/ip_parse.sbr" \
	"$(INTDIR)/ip_ctrl1.sbr" \
	"$(INTDIR)/ip_draw.sbr" \
	"$(INTDIR)/ip_wrap.sbr" \
	"$(INTDIR)/ip_cursor.sbr" \
	"$(INTDIR)/ctrl.sbr" \
	"$(INTDIR)/log.sbr" \
	"$(INTDIR)/OpenSaveDlg.sbr" \
	"$(INTDIR)/app.sbr" \
	"$(INTDIR)/Search.sbr" \
	"$(INTDIR)/memory.sbr" \
	"$(INTDIR)/string.sbr" \
	"$(INTDIR)/file.sbr" \
	"$(INTDIR)/ip_scroll.sbr" \
	"$(INTDIR)/cmdarg.sbr" \
	"$(INTDIR)/path.sbr" \
	"$(INTDIR)/ConfigManager.sbr" \
	"$(INTDIR)/window.sbr" \
	"$(INTDIR)/textfile.sbr" \
	"$(INTDIR)/stdafx.sbr" \
	"$(INTDIR)/winutil.sbr" \
	"$(INTDIR)/RSearch.sbr" \
	"$(INTDIR)/thread.sbr" \
	"$(INTDIR)/registry.sbr" \
	"$(INTDIR)/ip_text.sbr" \
	"$(INTDIR)/GpMain.sbr"

"$(OUTDIR)\GreenPad_mips.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
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

ALL : "$(OUTDIR)\GreenPad_mips.exe"

CLEAN : 
	-@erase ".\MIPSDbg\vc40.pdb"
	-@erase ".\MIPSDbg\GreenPad_mips.exe"
	-@erase ".\MIPSDbg\ip_parse.obj"
	-@erase ".\MIPSDbg\find.obj"
	-@erase ".\MIPSDbg\window.obj"
	-@erase ".\MIPSDbg\ip_ctrl1.obj"
	-@erase ".\MIPSDbg\thread.obj"
	-@erase ".\MIPSDbg\OpenSaveDlg.obj"
	-@erase ".\MIPSDbg\app.obj"
	-@erase ".\MIPSDbg\winutil.obj"
	-@erase ".\MIPSDbg\RSearch.obj"
	-@erase ".\MIPSDbg\ip_scroll.obj"
	-@erase ".\MIPSDbg\file.obj"
	-@erase ".\MIPSDbg\GpMain.obj"
	-@erase ".\MIPSDbg\ip_text.obj"
	-@erase ".\MIPSDbg\ip_cursor.obj"
	-@erase ".\MIPSDbg\path.obj"
	-@erase ".\MIPSDbg\stdafx.obj"
	-@erase ".\MIPSDbg\textfile.obj"
	-@erase ".\MIPSDbg\ConfigManager.obj"
	-@erase ".\MIPSDbg\ip_draw.obj"
	-@erase ".\MIPSDbg\ip_wrap.obj"
	-@erase ".\MIPSDbg\Search.obj"
	-@erase ".\MIPSDbg\ctrl.obj"
	-@erase ".\MIPSDbg\log.obj"
	-@erase ".\MIPSDbg\registry.obj"
	-@erase ".\MIPSDbg\memory.obj"
	-@erase ".\MIPSDbg\string.obj"
	-@erase ".\MIPSDbg\cmdarg.obj"
	-@erase ".\MIPSDbg\gp_rsrc.res"
	-@erase ".\MIPSDbg\GreenPad_mips.ilk"
	-@erase ".\MIPSDbg\GreenPad_mips.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "_DEBUG" /mips
# ADD MTL /nologo /D "_DEBUG" /mips
MTL_PROJ=/nologo /D "_DEBUG" /mips 
# ADD BASE CPP /nologo /ML /Gt0 /QMOb2000 /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I ".." /I "..\kilib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /YX"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /Gt0 /QMOb2000 /W3 /GX /Zi /Od /I ".." /I "..\kilib" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D\
 TARGET_VER=310 /Fp"$(INTDIR)/GreenPad_mips.pch" /YX"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MIPSDbg/
CPP_SBRS=
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/gp_rsrc.res" /d "_DEBUG" 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:MIPS
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:MIPS
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib comctl32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/GreenPad_mips.pdb" /debug /machine:MIPS\
 /out:"$(OUTDIR)/GreenPad_mips.exe" 
LINK32_OBJS= \
	"$(INTDIR)/ip_parse.obj" \
	"$(INTDIR)/find.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/ip_ctrl1.obj" \
	"$(INTDIR)/thread.obj" \
	"$(INTDIR)/OpenSaveDlg.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/winutil.obj" \
	"$(INTDIR)/RSearch.obj" \
	"$(INTDIR)/ip_scroll.obj" \
	"$(INTDIR)/file.obj" \
	"$(INTDIR)/GpMain.obj" \
	"$(INTDIR)/ip_text.obj" \
	"$(INTDIR)/ip_cursor.obj" \
	"$(INTDIR)/path.obj" \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/textfile.obj" \
	"$(INTDIR)/ConfigManager.obj" \
	"$(INTDIR)/ip_draw.obj" \
	"$(INTDIR)/ip_wrap.obj" \
	"$(INTDIR)/Search.obj" \
	"$(INTDIR)/ctrl.obj" \
	"$(INTDIR)/log.obj" \
	"$(INTDIR)/registry.obj" \
	"$(INTDIR)/memory.obj" \
	"$(INTDIR)/string.obj" \
	"$(INTDIR)/cmdarg.obj" \
	"$(INTDIR)/gp_rsrc.res"

"$(OUTDIR)\GreenPad_mips.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GreenPad_mips.bsc" 
BSC32_SBRS=

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
DEP_CPP_IP_PA=\
	".\kilib\stdafx.h"\
	".\editwing\ip_doc.h"\
	".\kilib\yvals.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewCommon.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_parse.obj" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_parse.sbr" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ip_parse.obj" : $(SOURCE) $(DEP_CPP_IP_PA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GpMain.cpp
DEP_CPP_GPMAI=\
	".\kilib\stdafx.h"\
	".\GpMain.h"\
	".\kilib\yvals.h"\
	".\kilib\kilib.h"\
	".\editwing\editwing.h"\
	".\OpenSaveDlg.h"\
	".\ConfigManager.h"\
	".\Search.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\kilib\string.h"\
	".\kilib\memory.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


"$(INTDIR)\GpMain.obj" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"

"$(INTDIR)\GpMain.sbr" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\GpMain.obj" : $(SOURCE) $(DEP_CPP_GPMAI) "$(INTDIR)"


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
	".\kilib\stdafx.h"\
	".\kilib\find.h"\
	".\kilib\yvals.h"\
	".\kilib\path.h"\
	

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
DEP_CPP_OPENS=\
	".\kilib\stdafx.h"\
	".\kilib\kilib.h"\
	".\OpenSaveDlg.h"\
	".\kilib\yvals.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\kilib\string.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


"$(INTDIR)\OpenSaveDlg.obj" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"

"$(INTDIR)\OpenSaveDlg.sbr" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\OpenSaveDlg.obj" : $(SOURCE) $(DEP_CPP_OPENS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\app.cpp
DEP_CPP_APP_C=\
	".\kilib\stdafx.h"\
	".\kilib\app.h"\
	".\kilib\log.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\yvals.h"\
	".\kilib\ktlaptr.h"\
	

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
DEP_CPP_IP_TE=\
	".\kilib\stdafx.h"\
	".\editwing\ip_doc.h"\
	".\kilib\yvals.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewCommon.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_text.obj" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_text.sbr" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ip_text.obj" : $(SOURCE) $(DEP_CPP_IP_TE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\registry.cpp
DEP_CPP_REGIS=\
	".\kilib\stdafx.h"\
	".\kilib\registry.h"\
	".\kilib\yvals.h"\
	".\kilib\path.h"\
	

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
	".\kilib\stdafx.h"\
	".\kilib\path.h"\
	".\kilib\yvals.h"\
	

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
	".\kilib\stdafx.h"\
	".\kilib\app.h"\
	".\kilib\ctrl.h"\
	".\kilib\yvals.h"\
	".\kilib\log.h"\
	".\kilib\window.h"\
	".\kilib\ktlaptr.h"\
	

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
DEP_CPP_IP_CT=\
	".\kilib\stdafx.h"\
	".\editwing\ewCtrl1.h"\
	".\kilib\yvals.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\editwing\ewCommon.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_ctrl1.obj" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_ctrl1.sbr" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ip_ctrl1.obj" : $(SOURCE) $(DEP_CPP_IP_CT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_wrap.cpp
DEP_CPP_IP_WR=\
	".\kilib\stdafx.h"\
	".\editwing\ip_view.h"\
	".\kilib\yvals.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_wrap.obj" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_wrap.sbr" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ip_wrap.obj" : $(SOURCE) $(DEP_CPP_IP_WR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RSearch.cpp
DEP_CPP_RSEAR=\
	".\kilib\stdafx.h"\
	".\RSearch.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\yvals.h"\
	".\NSearch.h"\
	".\Search.h"\
	".\editwing\editwing.h"\
	".\kilib\window.h"\
	".\kilib\memory.h"\
	".\kilib\string.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


"$(INTDIR)\RSearch.obj" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"

"$(INTDIR)\RSearch.sbr" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\RSearch.obj" : $(SOURCE) $(DEP_CPP_RSEAR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\editwing\ip_cursor.cpp
DEP_CPP_IP_CU=\
	".\kilib\stdafx.h"\
	".\editwing\ip_view.h"\
	".\kilib\yvals.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_cursor.obj" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_cursor.sbr" : $(SOURCE) $(DEP_CPP_IP_CU) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


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
	".\kilib\yvals.h"\
	

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
	".\kilib\stdafx.h"\
	".\kilib\app.h"\
	".\kilib\textfile.h"\
	".\kilib\ktlarray.h"\
	".\kilib\path.h"\
	".\kilib\yvals.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\file.h"\
	

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
	".\kilib\stdafx.h"\
	".\kilib\app.h"\
	".\kilib\yvals.h"\
	".\kilib\log.h"\
	

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
	".\kilib\stdafx.h"\
	".\kilib\app.h"\
	".\kilib\window.h"\
	".\kilib\yvals.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	

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
	".\kilib\yvals.h"\
	

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
DEP_CPP_IP_SC=\
	".\kilib\stdafx.h"\
	".\editwing\ip_view.h"\
	".\kilib\yvals.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_scroll.obj" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_scroll.sbr" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ip_scroll.obj" : $(SOURCE) $(DEP_CPP_IP_SC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\log.cpp
DEP_CPP_LOG_C=\
	".\kilib\stdafx.h"\
	".\kilib\log.h"\
	".\kilib\app.h"\
	".\kilib\yvals.h"\
	

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
DEP_CPP_CONFI=\
	".\kilib\stdafx.h"\
	".\ConfigManager.h"\
	".\RSearch.h"\
	".\kilib\yvals.h"\
	".\editwing\editwing.h"\
	".\OpenSaveDlg.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	".\kilib\string.h"\
	".\NSearch.h"\
	".\Search.h"\
	".\kilib\memory.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


"$(INTDIR)\ConfigManager.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"

"$(INTDIR)\ConfigManager.sbr" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ConfigManager.obj" : $(SOURCE) $(DEP_CPP_CONFI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\kilib\winutil.cpp
DEP_CPP_WINUT=\
	".\kilib\stdafx.h"\
	".\kilib\app.h"\
	".\kilib\winutil.h"\
	".\kilib\yvals.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	

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
	".\kilib\stdafx.h"\
	".\kilib\cmdarg.h"\
	".\kilib\yvals.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	

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
	".\kilib\stdafx.h"\
	".\kilib\file.h"\
	".\kilib\yvals.h"\
	

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
DEP_CPP_IP_DR=\
	".\kilib\stdafx.h"\
	".\editwing\ip_view.h"\
	".\kilib\yvals.h"\
	".\editwing\ewView.h"\
	".\editwing\ip_doc.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewDoc.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\window.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ip_draw.obj" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\ip_draw.sbr" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


"$(INTDIR)\ip_draw.obj" : $(SOURCE) $(DEP_CPP_IP_DR) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Search.cpp
DEP_CPP_SEARC=\
	".\kilib\stdafx.h"\
	".\Search.h"\
	".\NSearch.h"\
	".\RSearch.h"\
	".\kilib\yvals.h"\
	".\editwing\editwing.h"\
	".\kilib\window.h"\
	".\kilib\memory.h"\
	".\kilib\ktlaptr.h"\
	".\kilib\string.h"\
	".\editwing\ewCommon.h"\
	".\editwing\ewCtrl1.h"\
	".\kilib\kilib.h"\
	".\kilib\app.h"\
	".\kilib\thread.h"\
	".\kilib\cmdarg.h"\
	".\kilib\path.h"\
	".\kilib\file.h"\
	".\kilib\ctrl.h"\
	".\kilib\textfile.h"\
	".\kilib\winutil.h"\
	".\kilib\find.h"\
	".\kilib\registry.h"\
	".\kilib\log.h"\
	".\kilib\ktlarray.h"\
	".\kilib\ktlgap.h"\
	".\editwing\ewDoc.h"\
	".\editwing\ewView.h"\
	

!IF  "$(CFG)" == "GreenPad - Win32 Release"


"$(INTDIR)\Search.obj" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"

"$(INTDIR)\Search.sbr" : $(SOURCE) $(DEP_CPP_SEARC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "GreenPad - Win32 Debug"


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
