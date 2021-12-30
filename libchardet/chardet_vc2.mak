# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "chardet_vc2.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

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

ALL : $(OUTDIR)/chardet.dll $(OUTDIR)/chardet_vc2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /I "src" /I "src\tables" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /I "src" /I "src\tables" /D "NDEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"chardet_vc2.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chardet_vc2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/nsMBCSSM.sbr \
	$(INTDIR)/nsCharSetProber.sbr \
	$(INTDIR)/nsMBCSGroupProber.sbr \
	$(INTDIR)/nsEUCJPProber.sbr \
	$(INTDIR)/nsLatin1Prober.sbr \
	$(INTDIR)/impl.sbr \
	$(INTDIR)/JpCntx.sbr \
	$(INTDIR)/LangCyrillicModel.sbr \
	$(INTDIR)/LangHungarianModel.sbr \
	$(INTDIR)/LangGreekModel.sbr \
	$(INTDIR)/nsHebrewProber.sbr \
	$(INTDIR)/nsEscSM.sbr \
	$(INTDIR)/nsBig5Prober.sbr \
	$(INTDIR)/LangBulgarianModel.sbr \
	$(INTDIR)/nsGB2312Prober.sbr \
	$(INTDIR)/nsUniversalDetector.sbr \
	$(INTDIR)/nsSBCharSetProber.sbr \
	$(INTDIR)/nsEUCTWProber.sbr \
	$(INTDIR)/nsSJISProber.sbr \
	$(INTDIR)/nsSBCSGroupProber.sbr \
	$(INTDIR)/LangThaiModel.sbr \
	$(INTDIR)/CharDistribution.sbr \
	$(INTDIR)/LangHebrewModel.sbr \
	$(INTDIR)/nsEUCKRProber.sbr \
	$(INTDIR)/nsEscCharsetProber.sbr \
	$(INTDIR)/nsUTF8Prober.sbr \
	$(INTDIR)/LangSpanishModel.sbr \
	$(INTDIR)/LangEsperantoModel.sbr \
	$(INTDIR)/LangDanishModel.sbr \
	$(INTDIR)/LangTurkishModel.sbr \
	$(INTDIR)/LangGermanModel.sbr \
	$(INTDIR)/LangVietnameseModel.sbr \
	$(INTDIR)/LangArabicModel.sbr \
	$(INTDIR)/LangFrenchModel.sbr

$(OUTDIR)/chardet_vc2.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /PDB:"WinRel/chardet.pdb" /MACHINE:I386 /OUT:"WinRel/chardet.dll"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /DLL /INCREMENTAL:no /PDB:"WinRel/chardet.pdb" /MACHINE:I386\
 /OUT:"WinRel/chardet.dll" /IMPLIB:$(OUTDIR)/"chardet_vc2.lib" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/nsMBCSSM.obj \
	$(INTDIR)/nsCharSetProber.obj \
	$(INTDIR)/nsMBCSGroupProber.obj \
	$(INTDIR)/nsEUCJPProber.obj \
	$(INTDIR)/nsLatin1Prober.obj \
	$(INTDIR)/impl.obj \
	$(INTDIR)/JpCntx.obj \
	$(INTDIR)/LangCyrillicModel.obj \
	$(INTDIR)/LangHungarianModel.obj \
	$(INTDIR)/LangGreekModel.obj \
	$(INTDIR)/nsHebrewProber.obj \
	$(INTDIR)/nsEscSM.obj \
	$(INTDIR)/nsBig5Prober.obj \
	$(INTDIR)/LangBulgarianModel.obj \
	$(INTDIR)/nsGB2312Prober.obj \
	$(INTDIR)/nsUniversalDetector.obj \
	$(INTDIR)/nsSBCharSetProber.obj \
	$(INTDIR)/nsEUCTWProber.obj \
	$(INTDIR)/nsSJISProber.obj \
	$(INTDIR)/nsSBCSGroupProber.obj \
	$(INTDIR)/LangThaiModel.obj \
	$(INTDIR)/CharDistribution.obj \
	$(INTDIR)/LangHebrewModel.obj \
	$(INTDIR)/nsEUCKRProber.obj \
	$(INTDIR)/nsEscCharsetProber.obj \
	$(INTDIR)/nsUTF8Prober.obj \
	$(INTDIR)/LangSpanishModel.obj \
	$(INTDIR)/LangEsperantoModel.obj \
	$(INTDIR)/LangDanishModel.obj \
	$(INTDIR)/LangTurkishModel.obj \
	$(INTDIR)/LangGermanModel.obj \
	$(INTDIR)/LangVietnameseModel.obj \
	$(INTDIR)/LangArabicModel.obj \
	$(INTDIR)/LangFrenchModel.obj

$(OUTDIR)/chardet.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Debug"

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

ALL : $(OUTDIR)/chardet.dll $(OUTDIR)/chardet_vc2.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /I "src" /I "src\tables" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /FR /c
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /I "src" /I "src\tables" /D "_DEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"chardet_vc2.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"chardet_vc2.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"chardet_vc2.bsc" 
BSC32_SBRS= \
	$(INTDIR)/nsMBCSSM.sbr \
	$(INTDIR)/nsCharSetProber.sbr \
	$(INTDIR)/nsMBCSGroupProber.sbr \
	$(INTDIR)/nsEUCJPProber.sbr \
	$(INTDIR)/nsLatin1Prober.sbr \
	$(INTDIR)/impl.sbr \
	$(INTDIR)/JpCntx.sbr \
	$(INTDIR)/LangCyrillicModel.sbr \
	$(INTDIR)/LangHungarianModel.sbr \
	$(INTDIR)/LangGreekModel.sbr \
	$(INTDIR)/nsHebrewProber.sbr \
	$(INTDIR)/nsEscSM.sbr \
	$(INTDIR)/nsBig5Prober.sbr \
	$(INTDIR)/LangBulgarianModel.sbr \
	$(INTDIR)/nsGB2312Prober.sbr \
	$(INTDIR)/nsUniversalDetector.sbr \
	$(INTDIR)/nsSBCharSetProber.sbr \
	$(INTDIR)/nsEUCTWProber.sbr \
	$(INTDIR)/nsSJISProber.sbr \
	$(INTDIR)/nsSBCSGroupProber.sbr \
	$(INTDIR)/LangThaiModel.sbr \
	$(INTDIR)/CharDistribution.sbr \
	$(INTDIR)/LangHebrewModel.sbr \
	$(INTDIR)/nsEUCKRProber.sbr \
	$(INTDIR)/nsEscCharsetProber.sbr \
	$(INTDIR)/nsUTF8Prober.sbr \
	$(INTDIR)/LangSpanishModel.sbr \
	$(INTDIR)/LangEsperantoModel.sbr \
	$(INTDIR)/LangDanishModel.sbr \
	$(INTDIR)/LangTurkishModel.sbr \
	$(INTDIR)/LangGermanModel.sbr \
	$(INTDIR)/LangVietnameseModel.sbr \
	$(INTDIR)/LangArabicModel.sbr \
	$(INTDIR)/LangFrenchModel.sbr

$(OUTDIR)/chardet_vc2.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows /DLL /PDB:"WinDebug/chardet.pdb" /DEBUG /MACHINE:I386 /OUT:"WinDebug/chardet.dll"
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO\
 /SUBSYSTEM:windows /DLL /INCREMENTAL:yes /PDB:"WinDebug/chardet.pdb" /DEBUG\
 /MACHINE:I386 /OUT:"WinDebug/chardet.dll" /IMPLIB:$(OUTDIR)/"chardet_vc2.lib" 
DEF_FILE=
LINK32_OBJS= \
	$(INTDIR)/nsMBCSSM.obj \
	$(INTDIR)/nsCharSetProber.obj \
	$(INTDIR)/nsMBCSGroupProber.obj \
	$(INTDIR)/nsEUCJPProber.obj \
	$(INTDIR)/nsLatin1Prober.obj \
	$(INTDIR)/impl.obj \
	$(INTDIR)/JpCntx.obj \
	$(INTDIR)/LangCyrillicModel.obj \
	$(INTDIR)/LangHungarianModel.obj \
	$(INTDIR)/LangGreekModel.obj \
	$(INTDIR)/nsHebrewProber.obj \
	$(INTDIR)/nsEscSM.obj \
	$(INTDIR)/nsBig5Prober.obj \
	$(INTDIR)/LangBulgarianModel.obj \
	$(INTDIR)/nsGB2312Prober.obj \
	$(INTDIR)/nsUniversalDetector.obj \
	$(INTDIR)/nsSBCharSetProber.obj \
	$(INTDIR)/nsEUCTWProber.obj \
	$(INTDIR)/nsSJISProber.obj \
	$(INTDIR)/nsSBCSGroupProber.obj \
	$(INTDIR)/LangThaiModel.obj \
	$(INTDIR)/CharDistribution.obj \
	$(INTDIR)/LangHebrewModel.obj \
	$(INTDIR)/nsEUCKRProber.obj \
	$(INTDIR)/nsEscCharsetProber.obj \
	$(INTDIR)/nsUTF8Prober.obj \
	$(INTDIR)/LangSpanishModel.obj \
	$(INTDIR)/LangEsperantoModel.obj \
	$(INTDIR)/LangDanishModel.obj \
	$(INTDIR)/LangTurkishModel.obj \
	$(INTDIR)/LangGermanModel.obj \
	$(INTDIR)/LangVietnameseModel.obj \
	$(INTDIR)/LangArabicModel.obj \
	$(INTDIR)/LangFrenchModel.obj

$(OUTDIR)/chardet.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
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

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\src\nsMBCSSM.cpp

$(INTDIR)/nsMBCSSM.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsCharSetProber.cpp

$(INTDIR)/nsCharSetProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsMBCSGroupProber.cpp

$(INTDIR)/nsMBCSGroupProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCJPProber.cpp

$(INTDIR)/nsEUCJPProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsLatin1Prober.cpp

$(INTDIR)/nsLatin1Prober.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\entry\impl.cpp

$(INTDIR)/impl.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\JpCntx.cpp

$(INTDIR)/JpCntx.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangCyrillicModel.cpp

$(INTDIR)/LangCyrillicModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangHungarianModel.cpp

$(INTDIR)/LangHungarianModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangGreekModel.cpp

$(INTDIR)/LangGreekModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsHebrewProber.cpp

$(INTDIR)/nsHebrewProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEscSM.cpp

$(INTDIR)/nsEscSM.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsBig5Prober.cpp

$(INTDIR)/nsBig5Prober.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangBulgarianModel.cpp

$(INTDIR)/LangBulgarianModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsGB2312Prober.cpp

$(INTDIR)/nsGB2312Prober.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsUniversalDetector.cpp

$(INTDIR)/nsUniversalDetector.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSBCharSetProber.cpp

$(INTDIR)/nsSBCharSetProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCTWProber.cpp

$(INTDIR)/nsEUCTWProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSJISProber.cpp

$(INTDIR)/nsSJISProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsSBCSGroupProber.cpp

$(INTDIR)/nsSBCSGroupProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangThaiModel.cpp

$(INTDIR)/LangThaiModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\CharDistribution.cpp

$(INTDIR)/CharDistribution.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangHebrewModel.cpp

$(INTDIR)/LangHebrewModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEUCKRProber.cpp

$(INTDIR)/nsEUCKRProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsEscCharsetProber.cpp

$(INTDIR)/nsEscCharsetProber.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\nsUTF8Prober.cpp

$(INTDIR)/nsUTF8Prober.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangSpanishModel.cpp

$(INTDIR)/LangSpanishModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangEsperantoModel.cpp

$(INTDIR)/LangEsperantoModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangDanishModel.cpp

$(INTDIR)/LangDanishModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangTurkishModel.cpp

$(INTDIR)/LangTurkishModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangGermanModel.cpp

$(INTDIR)/LangGermanModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangVietnameseModel.cpp

$(INTDIR)/LangVietnameseModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangArabicModel.cpp

$(INTDIR)/LangArabicModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\src\LangFrenchModel.cpp

$(INTDIR)/LangFrenchModel.obj :  $(SOURCE)  $(INTDIR)
   $(CPP) $(CPP_PROJ)  $(SOURCE) 

# End Source File
# End Group
# End Project
################################################################################
