# Microsoft Developer Studio Generated NMAKE File, Based on kilib-axp64.dsp
!IF "$(CFG)" == ""
CFG=kilib - Win32 Release
!MESSAGE No configuration specified. Defaulting to kilib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "kilib - Win32 Release" && "$(CFG)" != "kilib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kilib-axp64.mak" CFG="kilib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kilib - Win32 Release" (based on "Win32 (ALPHA64) Application")
!MESSAGE "kilib - Win32 Debug" (based on "Win32 (ALPHA64) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "kilib - Win32 Release"

OUTDIR=.\Axp64Rel
INTDIR=.\Axp64Rel
# Begin Custom Macros
OutDir=.\Axp64Rel
# End Custom Macros

ALL : "$(OUTDIR)\GreenPad.exe" "$(OUTDIR)\kilib-axp64.bsc"


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
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\winutil.sbr"
	-@erase "$(OUTDIR)\GreenPad.exe"
	-@erase "$(OUTDIR)\kilib-axp64.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
CPP=cl.exe
CPP_PROJ=/nologo /ML /Gt0 /Ap64 /DWIN64 /D_WIN64 /W3 /GX /O2 /I ".\kilib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_ASMTHUNK" /D "NO_MLANG" /D "STRICT" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\kilib-axp64.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\gp_rsrc.res" /d "NDEBUG" /d "NO_JP_RES" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kilib-axp64.bsc" 
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

"$(OUTDIR)\kilib-axp64.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\GreenPad.pdb" /machine:ALPHA64 /out:"$(OUTDIR)\GreenPad.exe" 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\cmdarg.obj" \
	"$(INTDIR)\ConfigManager.obj" \
	"$(INTDIR)\ctrl.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\find.obj" \
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
	"$(INTDIR)\winutil.obj" \
	"$(INTDIR)\gp_rsrc.res"

"$(OUTDIR)\GreenPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "kilib - Win32 Debug"

OUTDIR=.\Axp64Debug
INTDIR=.\Axp64Debug
# Begin Custom Macros
OutDir=.\Axp64Debug
# End Custom Macros

ALL : "$(OUTDIR)\GreenPad.exe" "$(OUTDIR)\kilib-axp64.bsc"


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
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\winutil.sbr"
	-@erase "$(OUTDIR)\GreenPad.exe"
	-@erase "$(OUTDIR)\GreenPad.ilk"
	-@erase "$(OUTDIR)\GreenPad.pdb"
	-@erase "$(OUTDIR)\kilib-axp64.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

F90=fl32.exe
MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
CPP=cl.exe
CPP_PROJ=/nologo /ML /Gt0 /Ap64 /DWIN64 /D_WIN64 /W3 /Gm /GX /ZI /Od /I ".\kilib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "NO_MLANG" /D TARGET_VER=310 /D "STRICT" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\kilib-axp64.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\gp_rsrc.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kilib-axp64.bsc" 
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

"$(OUTDIR)\kilib-axp64.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib imm32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\GreenPad.pdb" /debug /machine:ALPHA64 /out:"$(OUTDIR)\GreenPad.exe" 
LINK32_OBJS= \
	"$(INTDIR)\app.obj" \
	"$(INTDIR)\cmdarg.obj" \
	"$(INTDIR)\ConfigManager.obj" \
	"$(INTDIR)\ctrl.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\find.obj" \
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
	"$(INTDIR)\winutil.obj" \
	"$(INTDIR)\gp_rsrc.res"

"$(OUTDIR)\GreenPad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("kilib-axp64.dep")
!INCLUDE "kilib-axp64.dep"
!ELSE 
!MESSAGE Warning: cannot find "kilib-axp64.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "kilib - Win32 Release" || "$(CFG)" == "kilib - Win32 Debug"
SOURCE=.\kilib\app.cpp

"$(INTDIR)\app.obj"	"$(INTDIR)\app.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\cmdarg.cpp

"$(INTDIR)\cmdarg.obj"	"$(INTDIR)\cmdarg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ConfigManager.cpp

"$(INTDIR)\ConfigManager.obj"	"$(INTDIR)\ConfigManager.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\kilib\ctrl.cpp

"$(INTDIR)\ctrl.obj"	"$(INTDIR)\ctrl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\file.cpp

"$(INTDIR)\file.obj"	"$(INTDIR)\file.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\find.cpp

"$(INTDIR)\find.obj"	"$(INTDIR)\find.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rsrc\gp_rsrc.rc

!IF  "$(CFG)" == "kilib - Win32 Release"


"$(INTDIR)\gp_rsrc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0xc04 /fo"$(INTDIR)\gp_rsrc.res" /i "rsrc" /i ".\rsrc" /d "NDEBUG" /d "NO_JP_RES" $(SOURCE)


!ELSEIF  "$(CFG)" == "kilib - Win32 Debug"


"$(INTDIR)\gp_rsrc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0xc04 /fo"$(INTDIR)\gp_rsrc.res" /i "rsrc" /i ".\rsrc" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\GpMain.cpp

"$(INTDIR)\GpMain.obj"	"$(INTDIR)\GpMain.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\editwing\ip_ctrl1.cpp

"$(INTDIR)\ip_ctrl1.obj"	"$(INTDIR)\ip_ctrl1.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\editwing\ip_cursor.cpp

"$(INTDIR)\ip_cursor.obj"	"$(INTDIR)\ip_cursor.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\editwing\ip_draw.cpp

"$(INTDIR)\ip_draw.obj"	"$(INTDIR)\ip_draw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\editwing\ip_parse.cpp

"$(INTDIR)\ip_parse.obj"	"$(INTDIR)\ip_parse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\editwing\ip_scroll.cpp

"$(INTDIR)\ip_scroll.obj"	"$(INTDIR)\ip_scroll.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\editwing\ip_text.cpp

"$(INTDIR)\ip_text.obj"	"$(INTDIR)\ip_text.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\editwing\ip_wrap.cpp

"$(INTDIR)\ip_wrap.obj"	"$(INTDIR)\ip_wrap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\log.cpp

"$(INTDIR)\log.obj"	"$(INTDIR)\log.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\memory.cpp

"$(INTDIR)\memory.obj"	"$(INTDIR)\memory.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\OpenSaveDlg.cpp

"$(INTDIR)\OpenSaveDlg.obj"	"$(INTDIR)\OpenSaveDlg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\kilib\path.cpp

"$(INTDIR)\path.obj"	"$(INTDIR)\path.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\registry.cpp

"$(INTDIR)\registry.obj"	"$(INTDIR)\registry.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\RSearch.cpp

"$(INTDIR)\RSearch.obj"	"$(INTDIR)\RSearch.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Search.cpp

"$(INTDIR)\Search.obj"	"$(INTDIR)\Search.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\kilib\stdafx.cpp

"$(INTDIR)\stdafx.obj"	"$(INTDIR)\stdafx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\string.cpp

"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\textfile.cpp

"$(INTDIR)\textfile.obj"	"$(INTDIR)\textfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\thread.cpp

"$(INTDIR)\thread.obj"	"$(INTDIR)\thread.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\window.cpp

"$(INTDIR)\window.obj"	"$(INTDIR)\window.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\kilib\winutil.cpp

"$(INTDIR)\winutil.obj"	"$(INTDIR)\winutil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

