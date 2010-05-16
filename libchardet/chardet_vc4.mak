# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=chardet_vc4 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to chardet_vc4 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "chardet_vc4 - Win32 Release" && "$(CFG)" !=\
 "chardet_vc4 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "chardet_vc4.mak" CFG="chardet_vc4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chardet_vc4 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "chardet_vc4 - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
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
# PROP Target_Last_Scanned "chardet_vc4 - Win32 Debug"
CPP=cl.exe
MTL=mktyplib.exe
RSC=rc.exe

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "chardet_"
# PROP BASE Intermediate_Dir "chardet_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\chardet.dll"

CLEAN : 
	-@erase "$(INTDIR)\CharDistribution.obj"
	-@erase "$(INTDIR)\impl.obj"
	-@erase "$(INTDIR)\JpCntx.obj"
	-@erase "$(INTDIR)\LangBulgarianModel.obj"
	-@erase "$(INTDIR)\LangCyrillicModel.obj"
	-@erase "$(INTDIR)\LangGreekModel.obj"
	-@erase "$(INTDIR)\LangHebrewModel.obj"
	-@erase "$(INTDIR)\LangHungarianModel.obj"
	-@erase "$(INTDIR)\LangThaiModel.obj"
	-@erase "$(INTDIR)\nsBig5Prober.obj"
	-@erase "$(INTDIR)\nsCharSetProber.obj"
	-@erase "$(INTDIR)\nsEscCharsetProber.obj"
	-@erase "$(INTDIR)\nsEscSM.obj"
	-@erase "$(INTDIR)\nsEUCJPProber.obj"
	-@erase "$(INTDIR)\nsEUCKRProber.obj"
	-@erase "$(INTDIR)\nsEUCTWProber.obj"
	-@erase "$(INTDIR)\nsGB2312Prober.obj"
	-@erase "$(INTDIR)\nsHebrewProber.obj"
	-@erase "$(INTDIR)\nsLatin1Prober.obj"
	-@erase "$(INTDIR)\nsMBCSGroupProber.obj"
	-@erase "$(INTDIR)\nsMBCSSM.obj"
	-@erase "$(INTDIR)\nsSBCharSetProber.obj"
	-@erase "$(INTDIR)\nsSBCSGroupProber.obj"
	-@erase "$(INTDIR)\nsSJISProber.obj"
	-@erase "$(INTDIR)\nsUniversalDetector.obj"
	-@erase "$(INTDIR)\nsUTF8Prober.obj"
	-@erase "$(OUTDIR)\chardet.dll"
	-@erase "$(OUTDIR)\chardet.exp"
	-@erase "$(OUTDIR)\chardet.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "src" /I "src\tables" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /YX /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "src" /I "src\tables" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /Fp"$(INTDIR)/chardet_vc4.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chardet_vc4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/chardet.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/chardet.pdb" /machine:I386 /out:"$(OUTDIR)/chardet.dll"\
 /implib:"$(OUTDIR)/chardet.lib" 
LINK32_OBJS= \
	"$(INTDIR)\CharDistribution.obj" \
	"$(INTDIR)\impl.obj" \
	"$(INTDIR)\JpCntx.obj" \
	"$(INTDIR)\LangBulgarianModel.obj" \
	"$(INTDIR)\LangCyrillicModel.obj" \
	"$(INTDIR)\LangGreekModel.obj" \
	"$(INTDIR)\LangHebrewModel.obj" \
	"$(INTDIR)\LangHungarianModel.obj" \
	"$(INTDIR)\LangThaiModel.obj" \
	"$(INTDIR)\nsBig5Prober.obj" \
	"$(INTDIR)\nsCharSetProber.obj" \
	"$(INTDIR)\nsEscCharsetProber.obj" \
	"$(INTDIR)\nsEscSM.obj" \
	"$(INTDIR)\nsEUCJPProber.obj" \
	"$(INTDIR)\nsEUCKRProber.obj" \
	"$(INTDIR)\nsEUCTWProber.obj" \
	"$(INTDIR)\nsGB2312Prober.obj" \
	"$(INTDIR)\nsHebrewProber.obj" \
	"$(INTDIR)\nsLatin1Prober.obj" \
	"$(INTDIR)\nsMBCSGroupProber.obj" \
	"$(INTDIR)\nsMBCSSM.obj" \
	"$(INTDIR)\nsSBCharSetProber.obj" \
	"$(INTDIR)\nsSBCSGroupProber.obj" \
	"$(INTDIR)\nsSJISProber.obj" \
	"$(INTDIR)\nsUniversalDetector.obj" \
	"$(INTDIR)\nsUTF8Prober.obj"

"$(OUTDIR)\chardet.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\chardet.dll"

CLEAN : 
	-@erase "$(INTDIR)\CharDistribution.obj"
	-@erase "$(INTDIR)\impl.obj"
	-@erase "$(INTDIR)\JpCntx.obj"
	-@erase "$(INTDIR)\LangBulgarianModel.obj"
	-@erase "$(INTDIR)\LangCyrillicModel.obj"
	-@erase "$(INTDIR)\LangGreekModel.obj"
	-@erase "$(INTDIR)\LangHebrewModel.obj"
	-@erase "$(INTDIR)\LangHungarianModel.obj"
	-@erase "$(INTDIR)\LangThaiModel.obj"
	-@erase "$(INTDIR)\nsBig5Prober.obj"
	-@erase "$(INTDIR)\nsCharSetProber.obj"
	-@erase "$(INTDIR)\nsEscCharsetProber.obj"
	-@erase "$(INTDIR)\nsEscSM.obj"
	-@erase "$(INTDIR)\nsEUCJPProber.obj"
	-@erase "$(INTDIR)\nsEUCKRProber.obj"
	-@erase "$(INTDIR)\nsEUCTWProber.obj"
	-@erase "$(INTDIR)\nsGB2312Prober.obj"
	-@erase "$(INTDIR)\nsHebrewProber.obj"
	-@erase "$(INTDIR)\nsLatin1Prober.obj"
	-@erase "$(INTDIR)\nsMBCSGroupProber.obj"
	-@erase "$(INTDIR)\nsMBCSSM.obj"
	-@erase "$(INTDIR)\nsSBCharSetProber.obj"
	-@erase "$(INTDIR)\nsSBCSGroupProber.obj"
	-@erase "$(INTDIR)\nsSJISProber.obj"
	-@erase "$(INTDIR)\nsUniversalDetector.obj"
	-@erase "$(INTDIR)\nsUTF8Prober.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\chardet.dll"
	-@erase "$(OUTDIR)\chardet.exp"
	-@erase "$(OUTDIR)\chardet.ilk"
	-@erase "$(OUTDIR)\chardet.lib"
	-@erase "$(OUTDIR)\chardet.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /I "src\tables" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /YX /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "src" /I "src\tables" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /Fp"$(INTDIR)/chardet_vc4.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chardet_vc4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/chardet.dll"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/chardet.pdb" /debug /machine:I386 /out:"$(OUTDIR)/chardet.dll"\
 /implib:"$(OUTDIR)/chardet.lib" 
LINK32_OBJS= \
	"$(INTDIR)\CharDistribution.obj" \
	"$(INTDIR)\impl.obj" \
	"$(INTDIR)\JpCntx.obj" \
	"$(INTDIR)\LangBulgarianModel.obj" \
	"$(INTDIR)\LangCyrillicModel.obj" \
	"$(INTDIR)\LangGreekModel.obj" \
	"$(INTDIR)\LangHebrewModel.obj" \
	"$(INTDIR)\LangHungarianModel.obj" \
	"$(INTDIR)\LangThaiModel.obj" \
	"$(INTDIR)\nsBig5Prober.obj" \
	"$(INTDIR)\nsCharSetProber.obj" \
	"$(INTDIR)\nsEscCharsetProber.obj" \
	"$(INTDIR)\nsEscSM.obj" \
	"$(INTDIR)\nsEUCJPProber.obj" \
	"$(INTDIR)\nsEUCKRProber.obj" \
	"$(INTDIR)\nsEUCTWProber.obj" \
	"$(INTDIR)\nsGB2312Prober.obj" \
	"$(INTDIR)\nsHebrewProber.obj" \
	"$(INTDIR)\nsLatin1Prober.obj" \
	"$(INTDIR)\nsMBCSGroupProber.obj" \
	"$(INTDIR)\nsMBCSSM.obj" \
	"$(INTDIR)\nsSBCharSetProber.obj" \
	"$(INTDIR)\nsSBCSGroupProber.obj" \
	"$(INTDIR)\nsSJISProber.obj" \
	"$(INTDIR)\nsUniversalDetector.obj" \
	"$(INTDIR)\nsUTF8Prober.obj"

"$(OUTDIR)\chardet.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Target

# Name "chardet_vc4 - Win32 Release"
# Name "chardet_vc4 - Win32 Debug"

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\src\chardet.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\CharDistribution.cpp
DEP_CPP_CHARD=\
	".\src\CharDistribution.h"\
	".\src\nscore.h"\
	".\src\tables\Big5Freq.tab"\
	".\src\tables\EUCKRFreq.tab"\
	".\src\tables\EUCTWFreq.tab"\
	".\src\tables\GB2312Freq.tab"\
	".\src\tables\JISFreq.tab"\
	

"$(INTDIR)\CharDistribution.obj" : $(SOURCE) $(DEP_CPP_CHARD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\CharDistribution.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\JpCntx.cpp
DEP_CPP_JPCNT=\
	".\src\JpCntx.h"\
	".\src\nscore.h"\
	

"$(INTDIR)\JpCntx.obj" : $(SOURCE) $(DEP_CPP_JPCNT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\JpCntx.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangBulgarianModel.cpp
DEP_CPP_LANGB=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\LangBulgarianModel.obj" : $(SOURCE) $(DEP_CPP_LANGB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangCyrillicModel.cpp
DEP_CPP_LANGC=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\LangCyrillicModel.obj" : $(SOURCE) $(DEP_CPP_LANGC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangGreekModel.cpp
DEP_CPP_LANGG=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\LangGreekModel.obj" : $(SOURCE) $(DEP_CPP_LANGG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangHebrewModel.cpp
DEP_CPP_LANGH=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\LangHebrewModel.obj" : $(SOURCE) $(DEP_CPP_LANGH) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangHungarianModel.cpp
DEP_CPP_LANGHU=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\LangHungarianModel.obj" : $(SOURCE) $(DEP_CPP_LANGHU) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangThaiModel.cpp
DEP_CPP_LANGT=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\LangThaiModel.obj" : $(SOURCE) $(DEP_CPP_LANGT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsBig5Prober.cpp
DEP_CPP_NSBIG=\
	".\src\CharDistribution.h"\
	".\src\nsBig5Prober.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsBig5Prober.obj" : $(SOURCE) $(DEP_CPP_NSBIG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsBig5Prober.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsCharSetProber.cpp
DEP_CPP_NSCHA=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\prmem.h"\
	

"$(INTDIR)\nsCharSetProber.obj" : $(SOURCE) $(DEP_CPP_NSCHA) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsCharSetProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsCodingStateMachine.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nscore.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEscCharsetProber.cpp
DEP_CPP_NSESC=\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsEscCharsetProber.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsEscCharsetProber.obj" : $(SOURCE) $(DEP_CPP_NSESC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEscCharsetProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEscSM.cpp
DEP_CPP_NSESCS=\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsEscSM.obj" : $(SOURCE) $(DEP_CPP_NSESCS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCJPProber.cpp
DEP_CPP_NSEUC=\
	".\src\CharDistribution.h"\
	".\src\JpCntx.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsEUCJPProber.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsEUCJPProber.obj" : $(SOURCE) $(DEP_CPP_NSEUC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCJPProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCKRProber.cpp
DEP_CPP_NSEUCK=\
	".\src\CharDistribution.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsEUCKRProber.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsEUCKRProber.obj" : $(SOURCE) $(DEP_CPP_NSEUCK) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCKRProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCTWProber.cpp
DEP_CPP_NSEUCT=\
	".\src\CharDistribution.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsEUCTWProber.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsEUCTWProber.obj" : $(SOURCE) $(DEP_CPP_NSEUCT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCTWProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsGB2312Prober.cpp
DEP_CPP_NSGB2=\
	".\src\CharDistribution.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsGB2312Prober.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsGB2312Prober.obj" : $(SOURCE) $(DEP_CPP_NSGB2) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsGB2312Prober.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsHebrewProber.cpp
DEP_CPP_NSHEB=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsHebrewProber.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\nsHebrewProber.obj" : $(SOURCE) $(DEP_CPP_NSHEB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsHebrewProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsLatin1Prober.cpp
DEP_CPP_NSLAT=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsLatin1Prober.h"\
	".\src\prmem.h"\
	

"$(INTDIR)\nsLatin1Prober.obj" : $(SOURCE) $(DEP_CPP_NSLAT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsLatin1Prober.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsMBCSGroupProber.cpp
DEP_CPP_NSMBC=\
	".\src\CharDistribution.h"\
	".\src\JpCntx.h"\
	".\src\nsBig5Prober.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsEUCJPProber.h"\
	".\src\nsEUCKRProber.h"\
	".\src\nsEUCTWProber.h"\
	".\src\nsGB2312Prober.h"\
	".\src\nsMBCSGroupProber.h"\
	".\src\nsPkgInt.h"\
	".\src\nsSJISProber.h"\
	".\src\nsUTF8Prober.h"\
	".\src\prmem.h"\
	

"$(INTDIR)\nsMBCSGroupProber.obj" : $(SOURCE) $(DEP_CPP_NSMBC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsMBCSGroupProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsMBCSSM.cpp
DEP_CPP_NSMBCS=\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsPkgInt.h"\
	

"$(INTDIR)\nsMBCSSM.obj" : $(SOURCE) $(DEP_CPP_NSMBCS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsPkgInt.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSBCharSetProber.cpp
DEP_CPP_NSSBC=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsSBCharSetProber.h"\
	

"$(INTDIR)\nsSBCharSetProber.obj" : $(SOURCE) $(DEP_CPP_NSSBC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSBCharSetProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSBCSGroupProber.cpp
DEP_CPP_NSSBCS=\
	".\src\nsCharSetProber.h"\
	".\src\nscore.h"\
	".\src\nsHebrewProber.h"\
	".\src\nsSBCharSetProber.h"\
	".\src\nsSBCSGroupProber.h"\
	".\src\prmem.h"\
	

"$(INTDIR)\nsSBCSGroupProber.obj" : $(SOURCE) $(DEP_CPP_NSSBCS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSBCSGroupProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSJISProber.cpp
DEP_CPP_NSSJI=\
	".\src\CharDistribution.h"\
	".\src\JpCntx.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsPkgInt.h"\
	".\src\nsSJISProber.h"\
	

"$(INTDIR)\nsSJISProber.obj" : $(SOURCE) $(DEP_CPP_NSSJI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSJISProber.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsUniversalDetector.cpp
DEP_CPP_NSUNI=\
	".\src\CharDistribution.h"\
	".\src\JpCntx.h"\
	".\src\nsBig5Prober.h"\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsEscCharsetProber.h"\
	".\src\nsEUCJPProber.h"\
	".\src\nsEUCKRProber.h"\
	".\src\nsEUCTWProber.h"\
	".\src\nsGB2312Prober.h"\
	".\src\nsLatin1Prober.h"\
	".\src\nsMBCSGroupProber.h"\
	".\src\nsPkgInt.h"\
	".\src\nsSBCSGroupProber.h"\
	".\src\nsSJISProber.h"\
	".\src\nsUniversalDetector.h"\
	".\src\nsUTF8Prober.h"\
	

"$(INTDIR)\nsUniversalDetector.obj" : $(SOURCE) $(DEP_CPP_NSUNI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsUniversalDetector.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsUTF8Prober.cpp
DEP_CPP_NSUTF=\
	".\src\nsCharSetProber.h"\
	".\src\nsCodingStateMachine.h"\
	".\src\nscore.h"\
	".\src\nsPkgInt.h"\
	".\src\nsUTF8Prober.h"\
	

"$(INTDIR)\nsUTF8Prober.obj" : $(SOURCE) $(DEP_CPP_NSUTF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsUTF8Prober.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\prmem.h

!IF  "$(CFG)" == "chardet_vc4 - Win32 Release"

!ELSEIF  "$(CFG)" == "chardet_vc4 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\entry\impl.cpp
NODEP_CPP_IMPL_=\
	".\src\entry\chardet.h"\
	".\src\entry\nscore.h"\
	".\src\entry\nsUniversalDetector.h"\
	

"$(INTDIR)\impl.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
