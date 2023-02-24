# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=ClipSpy - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to ClipSpy - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ClipSpy - Win32 Release" && "$(CFG)" !=\
 "ClipSpy - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "ClipSpy.mak" CFG="ClipSpy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ClipSpy - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ClipSpy - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "ClipSpy - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe
F90=fl32.exe

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\ClipSpy.exe"

CLEAN : 
	-@erase "$(INTDIR)\ClipSpy.obj"
	-@erase "$(INTDIR)\ClipSpy.res"
	-@erase "$(INTDIR)\ClipSpyDoc.obj"
	-@erase "$(INTDIR)\ClipSpyView.obj"
	-@erase "$(INTDIR)\CntrItem.obj"
	-@erase "$(INTDIR)\LeftView.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\ClipSpy.exe"
	-@erase "$(OUTDIR)\ClipSpy.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "Release/"
# ADD F90 /I "Release/"
F90_PROJ=/I "Release/" /Fo"Release/" 
F90_OBJS=.\Release/
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/ClipSpy.pch" /YX"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/ClipSpy.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ClipSpy.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/ClipSpy.pdb" /debug /machine:I386 /out:"$(OUTDIR)/ClipSpy.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ClipSpy.obj" \
	"$(INTDIR)\ClipSpy.res" \
	"$(INTDIR)\ClipSpyDoc.obj" \
	"$(INTDIR)\ClipSpyView.obj" \
	"$(INTDIR)\CntrItem.obj" \
	"$(INTDIR)\LeftView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\ClipSpy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\ClipSpy.exe" "$(OUTDIR)\ClipSpy.bsc"

CLEAN : 
	-@erase "$(INTDIR)\ClipSpy.obj"
	-@erase "$(INTDIR)\ClipSpy.res"
	-@erase "$(INTDIR)\ClipSpy.sbr"
	-@erase "$(INTDIR)\ClipSpyDoc.obj"
	-@erase "$(INTDIR)\ClipSpyDoc.sbr"
	-@erase "$(INTDIR)\ClipSpyView.obj"
	-@erase "$(INTDIR)\ClipSpyView.sbr"
	-@erase "$(INTDIR)\CntrItem.obj"
	-@erase "$(INTDIR)\CntrItem.sbr"
	-@erase "$(INTDIR)\LeftView.obj"
	-@erase "$(INTDIR)\LeftView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\ClipSpy.bsc"
	-@erase "$(OUTDIR)\ClipSpy.exe"
	-@erase "$(OUTDIR)\ClipSpy.ilk"
	-@erase "$(OUTDIR)\ClipSpy.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE F90 /I "Debug/"
# ADD F90 /FR /I "Debug/"
F90_PROJ=/FR"Debug/" /I "Debug/" /Fo"Debug/" 
F90_OBJS=.\Debug/
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/ClipSpy.pch"\
 /YX"stdafx.h" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/ClipSpy.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/ClipSpy.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ClipSpy.sbr" \
	"$(INTDIR)\ClipSpyDoc.sbr" \
	"$(INTDIR)\ClipSpyView.sbr" \
	"$(INTDIR)\CntrItem.sbr" \
	"$(INTDIR)\LeftView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\ClipSpy.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/ClipSpy.pdb" /debug /machine:I386 /out:"$(OUTDIR)/ClipSpy.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ClipSpy.obj" \
	"$(INTDIR)\ClipSpy.res" \
	"$(INTDIR)\ClipSpyDoc.obj" \
	"$(INTDIR)\ClipSpyView.obj" \
	"$(INTDIR)\CntrItem.obj" \
	"$(INTDIR)\LeftView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\ClipSpy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "ClipSpy - Win32 Release"
# Name "ClipSpy - Win32 Debug"

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\StdAfx.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipSpy.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipSpy.rc

"$(INTDIR)\ClipSpy.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipSpyDoc.cpp
DEP_CPP_CLIPS=\
	".\ClipSpy.h"\
	".\ClipSpyDoc.h"\
	".\CntrItem.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClipSpy - Win32 Release"


"$(INTDIR)\ClipSpyDoc.obj" : $(SOURCE) $(DEP_CPP_CLIPS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"


"$(INTDIR)\ClipSpyDoc.obj" : $(SOURCE) $(DEP_CPP_CLIPS) "$(INTDIR)"

"$(INTDIR)\ClipSpyDoc.sbr" : $(SOURCE) $(DEP_CPP_CLIPS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipSpyDoc.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipSpyView.cpp
DEP_CPP_CLIPSP=\
	".\ClipSpy.h"\
	".\ClipSpyDoc.h"\
	".\ClipSpyView.h"\
	".\CntrItem.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClipSpy - Win32 Release"


"$(INTDIR)\ClipSpyView.obj" : $(SOURCE) $(DEP_CPP_CLIPSP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"


"$(INTDIR)\ClipSpyView.obj" : $(SOURCE) $(DEP_CPP_CLIPSP) "$(INTDIR)"

"$(INTDIR)\ClipSpyView.sbr" : $(SOURCE) $(DEP_CPP_CLIPSP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipSpyView.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CntrItem.cpp
DEP_CPP_CNTRI=\
	".\ClipSpy.h"\
	".\ClipSpyDoc.h"\
	".\ClipSpyView.h"\
	".\CntrItem.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClipSpy - Win32 Release"


"$(INTDIR)\CntrItem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"


"$(INTDIR)\CntrItem.obj" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"

"$(INTDIR)\CntrItem.sbr" : $(SOURCE) $(DEP_CPP_CNTRI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CntrItem.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LeftView.cpp
DEP_CPP_LEFTV=\
	".\ClipSpy.h"\
	".\ClipSpyDoc.h"\
	".\LeftView.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClipSpy - Win32 Release"


"$(INTDIR)\LeftView.obj" : $(SOURCE) $(DEP_CPP_LEFTV) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"


"$(INTDIR)\LeftView.obj" : $(SOURCE) $(DEP_CPP_LEFTV) "$(INTDIR)"

"$(INTDIR)\LeftView.sbr" : $(SOURCE) $(DEP_CPP_LEFTV) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\LeftView.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\ClipSpy.h"\
	".\ClipSpyDoc.h"\
	".\ClipSpyView.h"\
	".\LeftView.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClipSpy - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "ClipSpy - Win32 Release"

!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClipSpy - Win32 Release"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"

"$(INTDIR)\StdAfx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipSpy.cpp
DEP_CPP_CLIPSPY=\
	".\ClipSpy.h"\
	".\ClipSpyDoc.h"\
	".\LeftView.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "ClipSpy - Win32 Release"


"$(INTDIR)\ClipSpy.obj" : $(SOURCE) $(DEP_CPP_CLIPSPY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ClipSpy - Win32 Debug"


"$(INTDIR)\ClipSpy.obj" : $(SOURCE) $(DEP_CPP_CLIPSPY) "$(INTDIR)"

"$(INTDIR)\ClipSpy.sbr" : $(SOURCE) $(DEP_CPP_CLIPSPY) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
