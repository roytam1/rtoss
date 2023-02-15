# Microsoft Visual C++ generated build script - Do not modify

PROJ = CHARDET_VC1
DEBUG = 0
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
ORIGIN = MSVCNT
ORIGIN_VER = 1.00
PROJPATH = C:\CHARDET\ 
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = 
FIRSTCPP = CHARDETOR.CPP
RC = rc
CFLAGS_D_WDLL32 = /nologo /W3 /Zi /YX /D "_X86_" /D "_DEBUG" /FR /MT /Fd"CHARDET_VC1.PDB" /Fp"CHARDET_VC1.PCH"
CFLAGS_R_WDLL32 = /nologo /Gy /W3 /YX /O2 /Ob1 /D "_X86_" /D "NDEBUG" /D "DLL_EXPORTS" /I "src\tables" /I "src" /ML /Fp"CHARDET_VC1.PCH"
LFLAGS_D_WDLL32 = /NOLOGO /DEBUG /DEBUGTYPE:cv /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_R_WDLL32 = /NOLOGO /SUBSYSTEM:windows user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib olecli32.lib olesvr32.lib shell32.lib
LFLAGS_D_LIB32 = /NOLOGO
LFLAGS_R_LIB32 = /NOLOGO
LIBS_D_WDLL32 = 
LIBS_R_WDLL32 = 
RCFLAGS32 = 
D_RCDEFINES32 = -d_DEBUG
R_RCDEFINES32 = -dNDEBUG
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL32)
LFLAGS = $(LFLAGS_D_WDLL32)
LIBS = $(LIBS_D_WDLL32)
LFLAGS_LIB=$(LFLAGS_D_LIB32)
MAPFILE_OPTION = 
RCDEFINES = $(D_RCDEFINES32)
!else
CFLAGS = $(CFLAGS_R_WDLL32)
LFLAGS = $(LFLAGS_R_WDLL32)
LIBS = $(LIBS_R_WDLL32)
MAPFILE_OPTION = 
LFLAGS_LIB=$(LFLAGS_R_LIB32)
RCDEFINES = $(R_RCDEFINES32)
!endif
SBRS = CHARDISTRIBUTION.SBR \
		JPCNTX.SBR \
		LANGBULGARIANMODEL.SBR \
		LANGCYRILLICMODEL.SBR \
		LANGGREEKMODEL.SBR \
		LANGHEBREWMODEL.SBR \
		LANGHUNGARIANMODEL.SBR \
		LANGTHAIMODEL.SBR \
		NSBIG5PROBER.SBR \
		NSCHARSETPROBER.SBR \
		NSESCCHARSETPROBER.SBR \
		NSESCSM.SBR \
		NSEUCJPPROBER.SBR \
		NSEUCKRPROBER.SBR \
		NSEUCTWPROBER.SBR \
		NSGB2312PROBER.SBR \
		NSHEBREWPROBER.SBR \
		NSLATIN1PROBER.SBR \
		NSMBCSGROUPPROBER.SBR \
		NSMBCSSM.SBR \
		NSSBCHARSETPROBER.SBR \
		NSSBCSGROUPPROBER.SBR \
		NSSJISPROBER.SBR \
		NSUNIVERSALDETECTOR.SBR \
		NSUTF8PROBER.SBR \
		IMPL.SBR \
		LANGARABICMODEL.SBR \
		LANGDANISHMODEL.SBR \
		LANGESPERANTOMODEL.SBR \
		LANGFRENCHMODEL.SBR \
		LANGGERMANMODEL.SBR \
		LANGSPANISHMODEL.SBR \
		LANGTURKISHMODEL.SBR \
		LANGVIETNAMESEMODEL.SBR


CHARDISTRIBUTION_DEP =  \
	c:\chardet\src\chardistribution.h \
	c:\chardet\src\nscore.h \
	C:\CHARDET\src\tables\jisfreq.tab \
	C:\CHARDET\src\tables\big5freq.tab \
	C:\CHARDET\src\tables\euckrfreq.tab \
	C:\CHARDET\src\tables\euctwfreq.tab \
	C:\CHARDET\src\tables\gb2312freq.tab


JPCNTX_DEP =  \
	c:\chardet\src\nscore.h \
	c:\chardet\src\jpcntx.h


LANGBULGARIANMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGCYRILLICMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGGREEKMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGHEBREWMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGHUNGARIANMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGTHAIMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


NSBIG5PROBER_DEP =  \
	c:\chardet\src\nsbig5prober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\chardistribution.h


NSCHARSETPROBER_DEP =  \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\prmem.h


NSESCCHARSETPROBER_DEP =  \
	c:\chardet\src\nsesccharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\nsuniversaldetector.h


NSESCSM_DEP =  \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\nscore.h


NSEUCJPPROBER_DEP =  \
	c:\chardet\src\nseucjpprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\jpcntx.h \
	c:\chardet\src\chardistribution.h


NSEUCKRPROBER_DEP =  \
	c:\chardet\src\nseuckrprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\chardistribution.h


NSEUCTWPROBER_DEP =  \
	c:\chardet\src\nseuctwprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\chardistribution.h


NSGB2312PROBER_DEP =  \
	c:\chardet\src\nsgb2312prober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\chardistribution.h


NSHEBREWPROBER_DEP =  \
	c:\chardet\src\nshebrewprober.h \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


NSLATIN1PROBER_DEP =  \
	c:\chardet\src\nslatin1prober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\prmem.h


NSMBCSGROUPPROBER_DEP =  \
	c:\chardet\src\nsmbcsgroupprober.h \
	c:\chardet\src\nssjisprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\jpcntx.h \
	c:\chardet\src\chardistribution.h \
	c:\chardet\src\nsutf8prober.h \
	c:\chardet\src\nseucjpprober.h \
	c:\chardet\src\nsgb2312prober.h \
	c:\chardet\src\nseuckrprober.h \
	c:\chardet\src\nsbig5prober.h \
	c:\chardet\src\nseuctwprober.h \
	c:\chardet\src\nsuniversaldetector.h


NSMBCSSM_DEP =  \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\nscore.h


NSSBCHARSETPROBER_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


NSSBCSGROUPPROBER_DEP =  \
	c:\chardet\src\prmem.h \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nssbcsgroupprober.h \
	c:\chardet\src\nshebrewprober.h


NSSJISPROBER_DEP =  \
	c:\chardet\src\nssjisprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\jpcntx.h \
	c:\chardet\src\chardistribution.h


NSUNIVERSALDETECTOR_DEP =  \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nsuniversaldetector.h \
	c:\chardet\src\nsmbcsgroupprober.h \
	c:\chardet\src\nssjisprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h \
	c:\chardet\src\jpcntx.h \
	c:\chardet\src\chardistribution.h \
	c:\chardet\src\nsutf8prober.h \
	c:\chardet\src\nseucjpprober.h \
	c:\chardet\src\nsgb2312prober.h \
	c:\chardet\src\nseuckrprober.h \
	c:\chardet\src\nsbig5prober.h \
	c:\chardet\src\nseuctwprober.h \
	c:\chardet\src\nssbcsgroupprober.h \
	c:\chardet\src\nsesccharsetprober.h \
	c:\chardet\src\nslatin1prober.h


NSUTF8PROBER_DEP =  \
	c:\chardet\src\nsutf8prober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h \
	c:\chardet\src\nscodingstatemachine.h \
	c:\chardet\src\nspkgint.h


IMPL_DEP =  \
	C:\CHARDET\src\chardet.h \
	C:\CHARDET\src\nscore.h \
	C:\CHARDET\src\nsuniversaldetector.h


LANGARABICMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGDANISHMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGESPERANTOMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGFRENCHMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGGERMANMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGSPANISHMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGTURKISHMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


LANGVIETNAMESEMODEL_DEP =  \
	c:\chardet\src\nssbcharsetprober.h \
	c:\chardet\src\nscharsetprober.h \
	c:\chardet\src\nscore.h


all:	$(PROJ).DLL

CHARDISTRIBUTION.OBJ:	SRC\CHARDISTRIBUTION.CPP $(CHARDISTRIBUTION_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\CHARDISTRIBUTION.CPP

JPCNTX.OBJ:	SRC\JPCNTX.CPP $(JPCNTX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\JPCNTX.CPP

LANGBULGARIANMODEL.OBJ:	SRC\LANGBULGARIANMODEL.CPP $(LANGBULGARIANMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGBULGARIANMODEL.CPP

LANGCYRILLICMODEL.OBJ:	SRC\LANGCYRILLICMODEL.CPP $(LANGCYRILLICMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGCYRILLICMODEL.CPP

LANGGREEKMODEL.OBJ:	SRC\LANGGREEKMODEL.CPP $(LANGGREEKMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGGREEKMODEL.CPP

LANGHEBREWMODEL.OBJ:	SRC\LANGHEBREWMODEL.CPP $(LANGHEBREWMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGHEBREWMODEL.CPP

LANGHUNGARIANMODEL.OBJ:	SRC\LANGHUNGARIANMODEL.CPP $(LANGHUNGARIANMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGHUNGARIANMODEL.CPP

LANGTHAIMODEL.OBJ:	SRC\LANGTHAIMODEL.CPP $(LANGTHAIMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGTHAIMODEL.CPP

NSBIG5PROBER.OBJ:	SRC\NSBIG5PROBER.CPP $(NSBIG5PROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSBIG5PROBER.CPP

NSCHARSETPROBER.OBJ:	SRC\NSCHARSETPROBER.CPP $(NSCHARSETPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSCHARSETPROBER.CPP

NSESCCHARSETPROBER.OBJ:	SRC\NSESCCHARSETPROBER.CPP $(NSESCCHARSETPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSESCCHARSETPROBER.CPP

NSESCSM.OBJ:	SRC\NSESCSM.CPP $(NSESCSM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSESCSM.CPP

NSEUCJPPROBER.OBJ:	SRC\NSEUCJPPROBER.CPP $(NSEUCJPPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSEUCJPPROBER.CPP

NSEUCKRPROBER.OBJ:	SRC\NSEUCKRPROBER.CPP $(NSEUCKRPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSEUCKRPROBER.CPP

NSEUCTWPROBER.OBJ:	SRC\NSEUCTWPROBER.CPP $(NSEUCTWPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSEUCTWPROBER.CPP

NSGB2312PROBER.OBJ:	SRC\NSGB2312PROBER.CPP $(NSGB2312PROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSGB2312PROBER.CPP

NSHEBREWPROBER.OBJ:	SRC\NSHEBREWPROBER.CPP $(NSHEBREWPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSHEBREWPROBER.CPP

NSLATIN1PROBER.OBJ:	SRC\NSLATIN1PROBER.CPP $(NSLATIN1PROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSLATIN1PROBER.CPP

NSMBCSGROUPPROBER.OBJ:	SRC\NSMBCSGROUPPROBER.CPP $(NSMBCSGROUPPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSMBCSGROUPPROBER.CPP

NSMBCSSM.OBJ:	SRC\NSMBCSSM.CPP $(NSMBCSSM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSMBCSSM.CPP

NSSBCHARSETPROBER.OBJ:	SRC\NSSBCHARSETPROBER.CPP $(NSSBCHARSETPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSSBCHARSETPROBER.CPP

NSSBCSGROUPPROBER.OBJ:	SRC\NSSBCSGROUPPROBER.CPP $(NSSBCSGROUPPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSSBCSGROUPPROBER.CPP

NSSJISPROBER.OBJ:	SRC\NSSJISPROBER.CPP $(NSSJISPROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSSJISPROBER.CPP

NSUNIVERSALDETECTOR.OBJ:	SRC\NSUNIVERSALDETECTOR.CPP $(NSUNIVERSALDETECTOR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSUNIVERSALDETECTOR.CPP

NSUTF8PROBER.OBJ:	SRC\NSUTF8PROBER.CPP $(NSUTF8PROBER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\NSUTF8PROBER.CPP

IMPL.OBJ:	SRC\ENTRY\IMPL.CPP $(IMPL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\ENTRY\IMPL.CPP

LANGARABICMODEL.OBJ:	SRC\LANGARABICMODEL.CPP $(LANGARABICMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGARABICMODEL.CPP

LANGDANISHMODEL.OBJ:	SRC\LANGDANISHMODEL.CPP $(LANGDANISHMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGDANISHMODEL.CPP

LANGESPERANTOMODEL.OBJ:	SRC\LANGESPERANTOMODEL.CPP $(LANGESPERANTOMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGESPERANTOMODEL.CPP

LANGFRENCHMODEL.OBJ:	SRC\LANGFRENCHMODEL.CPP $(LANGFRENCHMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGFRENCHMODEL.CPP

LANGGERMANMODEL.OBJ:	SRC\LANGGERMANMODEL.CPP $(LANGGERMANMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGGERMANMODEL.CPP

LANGSPANISHMODEL.OBJ:	SRC\LANGSPANISHMODEL.CPP $(LANGSPANISHMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGSPANISHMODEL.CPP

LANGTURKISHMODEL.OBJ:	SRC\LANGTURKISHMODEL.CPP $(LANGTURKISHMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGTURKISHMODEL.CPP

LANGVIETNAMESEMODEL.OBJ:	SRC\LANGVIETNAMESEMODEL.CPP $(LANGVIETNAMESEMODEL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SRC\LANGVIETNAMESEMODEL.CPP


$(PROJ).DLL:	CHARDISTRIBUTION.OBJ JPCNTX.OBJ LANGBULGARIANMODEL.OBJ LANGCYRILLICMODEL.OBJ \
	LANGGREEKMODEL.OBJ LANGHEBREWMODEL.OBJ LANGHUNGARIANMODEL.OBJ LANGTHAIMODEL.OBJ NSBIG5PROBER.OBJ \
	NSCHARSETPROBER.OBJ NSESCCHARSETPROBER.OBJ NSESCSM.OBJ NSEUCJPPROBER.OBJ NSEUCKRPROBER.OBJ \
	NSEUCTWPROBER.OBJ NSGB2312PROBER.OBJ NSHEBREWPROBER.OBJ NSLATIN1PROBER.OBJ NSMBCSGROUPPROBER.OBJ \
	NSMBCSSM.OBJ NSSBCHARSETPROBER.OBJ NSSBCSGROUPPROBER.OBJ NSSJISPROBER.OBJ NSUNIVERSALDETECTOR.OBJ \
	NSUTF8PROBER.OBJ IMPL.OBJ LANGARABICMODEL.OBJ LANGDANISHMODEL.OBJ LANGESPERANTOMODEL.OBJ \
	LANGFRENCHMODEL.OBJ LANGGERMANMODEL.OBJ LANGSPANISHMODEL.OBJ LANGTURKISHMODEL.OBJ \
	LANGVIETNAMESEMODEL.OBJ $(OBJS_EXT) $(LIBS_EXT)
	echo >NUL @<<$(PROJ).CRF
CHARDISTRIBUTION.OBJ 
JPCNTX.OBJ 
LANGBULGARIANMODEL.OBJ 
LANGCYRILLICMODEL.OBJ 
LANGGREEKMODEL.OBJ 
LANGHEBREWMODEL.OBJ 
LANGHUNGARIANMODEL.OBJ 
LANGTHAIMODEL.OBJ 
NSBIG5PROBER.OBJ 
NSCHARSETPROBER.OBJ 
NSESCCHARSETPROBER.OBJ 
NSESCSM.OBJ 
NSEUCJPPROBER.OBJ 
NSEUCKRPROBER.OBJ 
NSEUCTWPROBER.OBJ 
NSGB2312PROBER.OBJ 
NSHEBREWPROBER.OBJ 
NSLATIN1PROBER.OBJ 
NSMBCSGROUPPROBER.OBJ 
NSMBCSSM.OBJ 
NSSBCHARSETPROBER.OBJ 
NSSBCSGROUPPROBER.OBJ 
NSSJISPROBER.OBJ 
NSUNIVERSALDETECTOR.OBJ 
NSUTF8PROBER.OBJ 
IMPL.OBJ 
LANGARABICMODEL.OBJ 
LANGDANISHMODEL.OBJ 
LANGESPERANTOMODEL.OBJ 
LANGFRENCHMODEL.OBJ 
LANGGERMANMODEL.OBJ 
LANGSPANISHMODEL.OBJ 
LANGTURKISHMODEL.OBJ 
LANGVIETNAMESEMODEL.OBJ 
$(OBJS_EXT)
-DLL -OUT:$(PROJ).DLL
$(MAPFILE_OPTION)

$(LIBS)
$(LIBS_EXT)
$(DEFFILE_OPTION) -implib:$(PROJ).lib
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
