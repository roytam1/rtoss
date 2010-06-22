# -------------------------------------------
# langja.mak                 Kazubon 2001
#-------------------------------------------

!IFNDEF SRCDIR
SRCDIR=.
!ENDIF

DLLFILE=..\out\langja.dll
LANGID=0x411
RCFILE=$(SRCDIR)\langja.rc
RESFILE=langja.res
DEFFILE=$(SRCDIR)\langja.def

OBJS=langmain.obj
LIBS=kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib

!IFDEF _NMAKE_VER
CC=icl
LINK=xilink
RC=rc
COPT=/c /W3 /Os /nologo /DNDEBUG /Fo
LOPT=/SUBSYSTEM:WINDOWS /OPT:NOWIN98 /DLL /entry:DllMain 

$(DLLFILE): $(OBJS) $(RESFILE)
    $(LINK) $(LOPT) $(OBJS) $(RESFILE) $(LIBS) /DEF:$(DEFFILE) /OUT:$@ /NODEFAULTLIB

!ELSE
CC=bcc32
LINK=ilink32
RC=brcc32
COPT=-c -w -O2 -DWIN32 -tWD -o
LOPT=/c /C /Gn /Tpd /x /b:1000000h

$(DLLFILE): $(OBJS) $(SRCDIR)\bcclang.pat $(RESFILE)
	$(LINK) $(LOPT) $(OBJS) $(SRCDIR)\bcclang.pat,$@,,$(LIBS),$(DEFFILE),$(RESFILE)

$(SRCDIR)\bcclang.pat: $(SRCDIR)\bcclang.nas
	nasmw -f obj -o $@ $(SRCDIR)\bcclang.nas

!ENDIF

langmain.obj: $(SRCDIR)\main.c
    $(CC) $(COPT)$@ $(SRCDIR)\main.c
$(RESFILE): $(RCFILE) $(SRCDIR)\resource.h
    $(RC) /l $(LANGID) /fo $@ $(RCFILE)
