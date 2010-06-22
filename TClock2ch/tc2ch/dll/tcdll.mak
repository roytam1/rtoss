# -------------------------------------------
# tcdll.mak              Kazubon 1998-2001
#-------------------------------------------

!IFNDEF SRCDIR
SRCDIR=.
!ENDIF

DLLFILE=..\out\tcdll.dll
LANGID=0x411
RCFILE=$(SRCDIR)\tcdll.rc
RESFILE=tcdll.res
LIBFILE=tcdll.lib
DEFFILE=$(SRCDIR)\tcdll.def

OBJS=dllmain.obj tclock.obj format.obj desktop.obj startbtn.obj \
 startmenu.obj taskswitch.obj traynotify.obj \
 font.obj bmp.obj dllutl.obj newapi.obj sysres.obj cpu.obj mbm.obj \
 net.obj hdd.obj mixerdll.obj tooltip.obj cwebpage.obj permon.obj
# permon.obj 
LIBS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib winmm.lib ole32.lib oleaut32.lib urlmon.lib

!IFDEF _NMAKE_VER
CC=icl
LINK=xilink
RC=rc
COPT=/c /W3 /O3 /DWIN32 /nologo /Qipo /Fo
LOPT=/SUBSYSTEM:WINDOWS /OPT:NOWIN98 /DLL

$(DLLFILE): $(OBJS) $(RESFILE)
    $(LINK) $(LOPT) $(OBJS) $(RESFILE) $(LIBS) /DEF:$(DEFFILE) /IMPLIB:$(LIBFILE) /OUT:$@

!ELSE
CC=bcc32
LINK=ilink32
RC=brcc32
COPT=-c -w -O2 -DWIN32 -tWD -tWM -o
LOPT=/c /C /Gn /Tpd /x

$(DLLFILE): $(OBJS) $(SRCDIR)\bccdll.pat $(RESFILE)
	IMPLIB $(LIBFILE) $(DEFFILE)
	$(LINK) $(LOPT) $(OBJS) $(SRCDIR)\bccdll.pat c0d32x.obj,$@,,$(LIBS) cw32mt.lib,$(DEFFILE),$(RESFILE)

$(SRCDIR)\bccdll.pat: $(SRCDIR)\bccdll.nas
	nasmw -f obj -o $@ $(SRCDIR)\bccdll.nas
!ENDIF

dllmain.obj: $(SRCDIR)\dllmain.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\dllmain.c
dllutl.obj: $(SRCDIR)\utl.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\utl.c
tclock.obj: $(SRCDIR)\tclock.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\tclock.c
format.obj: $(SRCDIR)\format.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\format.c
desktop.obj: $(SRCDIR)\desktop.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\desktop.c
startbtn.obj: $(SRCDIR)\startbtn.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\startbtn.c
startmenu.obj: $(SRCDIR)\startmenu.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\startmenu.c
taskswitch.obj: $(SRCDIR)\taskswitch.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\taskswitch.c
traynotify.obj: $(SRCDIR)\traynotify.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\traynotify.c
font.obj: $(SRCDIR)\font.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\font.c
bmp.obj: $(SRCDIR)\bmp.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\bmp.c
newapi.obj: $(SRCDIR)\newapi.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\newapi.c
sysres.obj: $(SRCDIR)\sysres.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\sysres.c
cpu.obj: $(SRCDIR)\cpu.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\cpu.c
mbm.obj: $(SRCDIR)\mbm.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\mbm.c
permon.obj: $(SRCDIR)\permon.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\permon.c
net.obj: $(SRCDIR)\net.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\net.c
hdd.obj: $(SRCDIR)\hdd.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\hdd.c
mixerdll.obj: $(SRCDIR)\mixer.c $(SRCDIR)\tcdll.h
    $(CC) $(COPT)$@ $(SRCDIR)\mixer.c
tooltip.obj: $(SRCDIR)\tooltip.c $(SRCDIR)\tcdll.h $(SRCDIR)\cwebpage.h
    $(CC) $(COPT)$@ $(SRCDIR)\tooltip.c
cwebpage.obj: $(SRCDIR)\cwebpage.c
    $(CC) $(COPT)$@ $(SRCDIR)\cwebpage.c

$(RESFILE): $(RCFILE)
    $(RC) /l $(LANGID) /fo $(RESFILE) $(RCFILE)

