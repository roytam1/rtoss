# Microsoft Developer Studio Generated NMAKE File, Based on 16Edit-axp64.dsp
!IF "$(CFG)" == ""
CFG=16Edit - Win32 Release
!MESSAGE No configuration specified. Defaulting to 16Edit - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "16Edit - Win32 Release" && "$(CFG)" != "16Edit - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "16Edit-axp64.mak" CFG="16Edit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "16Edit - Win32 Release" (based on "Win32 (ALPHA) Application")
!MESSAGE "16Edit - Win32 Debug" (based on "Win32 (ALPHA) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "16Edit - Win32 Release"

OUTDIR=.\Axp64Rel
INTDIR=.\Axp64Rel
# Begin Custom Macros
OutDir=.\Axp64Rel
# End Custom Macros

ALL : ".\bin\16Edit_axp64.exe" "$(OUTDIR)\16Edit-axp64.bsc"


CLEAN :
	-@erase "$(INTDIR)\16EditLoader.obj"
	-@erase "$(INTDIR)\16EditLoader.sbr"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\Common.sbr"
	-@erase "$(INTDIR)\CPathString.obj"
	-@erase "$(INTDIR)\CPathString.sbr"
	-@erase "$(INTDIR)\DialogProc.obj"
	-@erase "$(INTDIR)\DialogProc.sbr"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\File.sbr"
	-@erase "$(INTDIR)\HexEditWnd.obj"
	-@erase "$(INTDIR)\HexEditWnd.sbr"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\list.sbr"
	-@erase "$(INTDIR)\OFN.obj"
	-@erase "$(INTDIR)\OFN.sbr"
	-@erase "$(INTDIR)\rsrc.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WideChar.obj"
	-@erase "$(INTDIR)\WideChar.sbr"
	-@erase "$(OUTDIR)\16Edit-axp64.bsc"
	-@erase ".\bin\16Edit_axp64.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
CPP=cl.exe
CPP_PROJ=/nologo /MD /Gt0 /Ap64 /D_WIN64 /DWIN64 /W3 /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /O1 /c 

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
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\rsrc.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\16Edit-axp64.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\16EditLoader.sbr" \
	"$(INTDIR)\Common.sbr" \
	"$(INTDIR)\CPathString.sbr" \
	"$(INTDIR)\DialogProc.sbr" \
	"$(INTDIR)\File.sbr" \
	"$(INTDIR)\HexEditWnd.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\OFN.sbr" \
	"$(INTDIR)\WideChar.sbr"

"$(OUTDIR)\16Edit-axp64.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\16Edit_axp64.pdb" /machine:ALPHA64 /out:"bin\16Edit_axp64.exe" 
LINK32_OBJS= \
	"$(INTDIR)\16EditLoader.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\CPathString.obj" \
	"$(INTDIR)\DialogProc.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\HexEditWnd.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\OFN.obj" \
	"$(INTDIR)\WideChar.obj" \
	"$(INTDIR)\rsrc.res"

".\bin\16Edit_axp64.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "16Edit - Win32 Debug"

OUTDIR=.\Axp64Debug
INTDIR=.\Axp64Debug
# Begin Custom Macros
OutDir=.\Axp64Debug
# End Custom Macros

ALL : ".\bin\16Edit_axp64.exe" "$(OUTDIR)\16Edit-axp64.bsc"


CLEAN :
	-@erase "$(INTDIR)\16EditLoader.obj"
	-@erase "$(INTDIR)\16EditLoader.sbr"
	-@erase "$(INTDIR)\Common.obj"
	-@erase "$(INTDIR)\Common.sbr"
	-@erase "$(INTDIR)\CPathString.obj"
	-@erase "$(INTDIR)\CPathString.sbr"
	-@erase "$(INTDIR)\DialogProc.obj"
	-@erase "$(INTDIR)\DialogProc.sbr"
	-@erase "$(INTDIR)\File.obj"
	-@erase "$(INTDIR)\File.sbr"
	-@erase "$(INTDIR)\HexEditWnd.obj"
	-@erase "$(INTDIR)\HexEditWnd.sbr"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\list.sbr"
	-@erase "$(INTDIR)\OFN.obj"
	-@erase "$(INTDIR)\OFN.sbr"
	-@erase "$(INTDIR)\rsrc.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WideChar.obj"
	-@erase "$(INTDIR)\WideChar.sbr"
	-@erase "$(OUTDIR)\16Edit-axp64.bsc"
	-@erase "$(OUTDIR)\16Edit_axp64.pdb"
	-@erase ".\bin\16Edit_axp64.exe"
	-@erase ".\bin\16Edit_axp64.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
CPP=cl.exe
CPP_PROJ=/nologo /MLd /Gt0 /Ap64 /D_WIN64 /DWIN64 /W3 /GX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\16Edit-axp64.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /ZI /GZ /c 

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
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\rsrc.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\16Edit-axp64.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\16EditLoader.sbr" \
	"$(INTDIR)\Common.sbr" \
	"$(INTDIR)\CPathString.sbr" \
	"$(INTDIR)\DialogProc.sbr" \
	"$(INTDIR)\File.sbr" \
	"$(INTDIR)\HexEditWnd.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\OFN.sbr" \
	"$(INTDIR)\WideChar.sbr"

"$(OUTDIR)\16Edit-axp64.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib COMCTL32.LIB /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\16Edit_axp64.pdb" /debug /machine:ALPHA64 /out:"bin\16Edit_axp64.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\16EditLoader.obj" \
	"$(INTDIR)\Common.obj" \
	"$(INTDIR)\CPathString.obj" \
	"$(INTDIR)\DialogProc.obj" \
	"$(INTDIR)\File.obj" \
	"$(INTDIR)\HexEditWnd.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\OFN.obj" \
	"$(INTDIR)\WideChar.obj" \
	"$(INTDIR)\rsrc.res"

".\bin\16Edit_axp64.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("16Edit-axp64.dep")
!INCLUDE "16Edit-axp64.dep"
!ELSE 
!MESSAGE Warning: cannot find "16Edit-axp64.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "16Edit - Win32 Release" || "$(CFG)" == "16Edit - Win32 Debug"
SOURCE=.\16EditLoader.cpp

"$(INTDIR)\16EditLoader.obj"	"$(INTDIR)\16EditLoader.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Common.cpp

"$(INTDIR)\Common.obj"	"$(INTDIR)\Common.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CPathString.cpp

"$(INTDIR)\CPathString.obj"	"$(INTDIR)\CPathString.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DialogProc.cpp

"$(INTDIR)\DialogProc.obj"	"$(INTDIR)\DialogProc.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\File.cpp

"$(INTDIR)\File.obj"	"$(INTDIR)\File.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HexEditWnd.cpp

"$(INTDIR)\HexEditWnd.obj"	"$(INTDIR)\HexEditWnd.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\list.cpp

"$(INTDIR)\list.obj"	"$(INTDIR)\list.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OFN.cpp

"$(INTDIR)\OFN.obj"	"$(INTDIR)\OFN.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rsrc.rc

"$(INTDIR)\rsrc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\WideChar.cpp

"$(INTDIR)\WideChar.obj"	"$(INTDIR)\WideChar.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

