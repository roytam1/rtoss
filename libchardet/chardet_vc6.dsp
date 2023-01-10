# Microsoft Developer Studio Project File - Name="chardet_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=chardet_vc6 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chardet_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chardet_vc6.mak" CFG="chardet_vc6 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chardet_vc6 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "chardet_vc6 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chardet_vc6 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\chardet_"
# PROP BASE Intermediate_Dir ".\chardet_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp4 /MT /W3 /Ox /Ot /Oa /Og /Oi /Ob2 /Gf /Gy /I ".\src" /I ".\src\tables" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /D "SUPERTINY" /YX /FD /c
# SUBTRACT CPP /Os
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /machine:I386 /nodefaultlib /out:".\Release\chardet.dll" /filealign:512 /OPT:REF /OPT:ICF,20 /LARGEADDRESSAWARE
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "chardet_vc6 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\src" /I ".\src\tables" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "DLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\chardet.dll"
# SUBTRACT LINK32 /profile /nodefaultlib

!ENDIF 

# Begin Target

# Name "chardet_vc6 - Win32 Release"
# Name "chardet_vc6 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\src\chardet.h
# End Source File
# Begin Source File

SOURCE=.\src\CharDistribution.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CharDistribution.h
# End Source File
# Begin Source File

SOURCE=.\src\entry\impl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\JpCntx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\JpCntx.h
# End Source File
# Begin Source File

SOURCE=.\src\LangArabicModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangBulgarianModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangCyrillicModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangDanishModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangEsperantoModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangFrenchModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangGermanModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangGreekModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangHebrewModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangHungarianModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangSpanishModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangThaiModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangTurkishModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LangVietnameseModel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsBig5Prober.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsBig5Prober.h
# End Source File
# Begin Source File

SOURCE=.\src\nsCharSetProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsCharSetProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsCodingStateMachine.h
# End Source File
# Begin Source File

SOURCE=.\src\nscore.h
# End Source File
# Begin Source File

SOURCE=.\src\nsEscCharsetProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsEscCharsetProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsEscSM.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsEUCJPProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsEUCJPProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsEUCKRProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsEUCKRProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsEUCTWProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsEUCTWProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsGB2312Prober.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsGB2312Prober.h
# End Source File
# Begin Source File

SOURCE=.\src\nsHebrewProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsHebrewProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsLatin1Prober.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsLatin1Prober.h
# End Source File
# Begin Source File

SOURCE=.\src\nsMBCSGroupProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsMBCSGroupProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsMBCSSM.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsPkgInt.h
# End Source File
# Begin Source File

SOURCE=.\src\nsSBCharSetProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsSBCharSetProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsSBCSGroupProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsSBCSGroupProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsSJISProber.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsSJISProber.h
# End Source File
# Begin Source File

SOURCE=.\src\nsUniversalDetector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsUniversalDetector.h
# End Source File
# Begin Source File

SOURCE=.\src\nsUTF8Prober.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nsUTF8Prober.h
# End Source File
# Begin Source File

SOURCE=.\src\prmem.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\rc\chardet.rc
# End Source File
# End Group
# End Target
# End Project
