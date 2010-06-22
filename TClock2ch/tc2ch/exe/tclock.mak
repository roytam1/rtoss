# ------------------------------------------
# tclock.mak            KAZUBON 1998-2001
#-------------------------------------------

!IFNDEF SRCDIR
SRCDIR=.
!ENDIF

EXEFILE=..\out\tclock.exe
DLLFILE=..\out\tclock.dll
DEFFILE=$(SRCDIR)\ttbase\ttbasesdk.def
LANGID=0x411
RCFILE=$(SRCDIR)\tclock.rc
RESFILE=tclock.res
RESOURCEH=..\language\resource.h

OBJS=propdlg.obj page.obj pagecolor.obj pageformat.obj pageanalog.obj\
 pageformat2.obj pagealarm.obj alarmday.obj autoexecday.obj autoexechour.obj\
 autoexecmin.obj  pagemouse.obj pagetooltip.obj pageskin.obj pagegraph.obj\
 pagetaskbar.obj pagedesktop.obj pagesntp.obj pageautoexec.obj pagemisc.obj\
 pageabout.obj alarm.obj menu.obj mouse.obj deskcal.obj timer.obj calendar.obj\
 sntp.obj soundselect.obj mixer.obj filelist.obj utl.obj memcpy_amd.obj iccsupp.obj
LIBS=kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib\
 shell32.lib winmm.lib comctl32.lib ole32.lib uuid.lib version.lib\
 wsock32.lib libircmt.lib

all: $(EXEFILE) $(DLLFILE)

!IFDEF _NMAKE_VER
CC=icl
LINK=xilink
RC=rc
COPT=/c /W3 /O3 /DWIN32 /nologo /Qipo /DNDEBUG /Fo
LOPT=/SUBSYSTEM:WINDOWS /OPT:NOWIN98 /NODEFAULTLIB 

$(EXEFILE): exemain.obj $(OBJS) $(RESFILE)
    $(LINK) $(LOPT) exemain.obj $(OBJS) $(RESFILE) TCDLL.lib $(LIBS) /OUT:$@

$(DLLFILE): ttbasemain.obj $(OBJS) $(RESFILE)
    $(LINK) $(LOPT) ttbasemain.obj $(OBJS) $(RESFILE) $(LIBS) /OUT:$@ /DEF:$(DEFFILE) /DLL

!ELSE
CC=bcc32
LINK=ilink32
RC=brcc32
COPT=-c -w -O2 -DWIN32 -tW -tWM -o
LOPT=/c /C /Gn /x

$(EXEFILE): exemain.obj $(OBJS) $(SRCDIR)\bccexe.pat $(RESFILE)
	$(LINK) $(LOPT) /Tpe /aa exemain.obj $(OBJS) $(SRCDIR)\bccexe.pat,$@,,$(LIBS) TCDLL.lib,,$(RESFILE)

$(DLLFILE): ttbasemain.obj $(OBJS) $(SRCDIR)\bccexe.pat $(RESFILE)
	$(LINK) $(LOPT) /Tpd ttbasemain.obj $(OBJS) $(SRCDIR)\bccexe.pat,$@,,$(LIBS),$(DEFFILE),$(RESFILE)

$(SRCDIR)\bccexe.pat: $(SRCDIR)\bccexe.nas
	nasmw -f obj -o $@ $(SRCDIR)\bccexe.nas

!ENDIF

ttbasemain.obj: $(SRCDIR)\exemain.c $(SRCDIR)\tclock.h $(RESOURCEH) $(SRCDIR)\ttbase\ttbasesdk.c
    $(CC) -DTCLOCK2CH_TTBASE $(COPT)$@ $(SRCDIR)\exemain.c
exemain.obj: $(SRCDIR)\exemain.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\exemain.c
memcpy_amd.obj: $(SRCDIR)\memcpy_amd.c $(SRCDIR)\memcpy_amd.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\memcpy_amd.c
propdlg.obj: $(SRCDIR)\propdlg.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\propdlg.c
page.obj: $(SRCDIR)\page.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\page.c
pagecolor.obj: $(SRCDIR)\pagecolor.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagecolor.c
pageformat.obj: $(SRCDIR)\pageformat.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pageformat.c
pageformat2.obj: $(SRCDIR)\pageformat2.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pageformat2.c
pagealarm.obj: $(SRCDIR)\pagealarm.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagealarm.c
pageanalog.obj: $(SRCDIR)\pageanalog.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pageanalog.c
alarmday.obj: $(SRCDIR)\alarmday.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\alarmday.c
pagemouse.obj: $(SRCDIR)\pagemouse.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagemouse.c
pagetooltip.obj: $(SRCDIR)\pagetooltip.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagetooltip.c
pageskin.obj: $(SRCDIR)\pageskin.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pageskin.c
pagegraph.obj: $(SRCDIR)\pagegraph.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagegraph.c
pagetaskbar.obj: $(SRCDIR)\pagetaskbar.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagetaskbar.c
pagedesktop.obj: $(SRCDIR)\pagedesktop.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagedesktop.c
pagesntp.obj: $(SRCDIR)\pagesntp.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagesntp.c
pageautoexec.obj: $(SRCDIR)\pageautoexec.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pageautoexec.c
autoexechour.obj: $(SRCDIR)\autoexechour.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\autoexechour.c
autoexecmin.obj: $(SRCDIR)\autoexecmin.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\autoexecmin.c
autoexecday.obj: $(SRCDIR)\autoexecday.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\autoexecday.c
pagemisc.obj: $(SRCDIR)\pagemisc.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pagemisc.c
pageabout.obj: $(SRCDIR)\pageabout.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\pageabout.c
alarm.obj: $(SRCDIR)\alarm.c $(SRCDIR)\tclock.h
    $(CC) $(COPT)$@ $(SRCDIR)\alarm.c
menu.obj: $(SRCDIR)\menu.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\menu.c
mouse.obj: $(SRCDIR)\mouse.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\mouse.c
deskcal.obj: $(SRCDIR)\deskcal.c $(SRCDIR)\tclock.h
    $(CC) $(COPT)$@ $(SRCDIR)\deskcal.c
calendar.obj: $(SRCDIR)\calendar.c $(SRCDIR)\tclock.h
    $(CC) $(COPT)$@ $(SRCDIR)\calendar.c
timer.obj: $(SRCDIR)\timer.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\timer.c
sntp.obj: $(SRCDIR)\sntp.c $(SRCDIR)\tclock.h
    $(CC) $(COPT)$@ $(SRCDIR)\sntp.c
soundselect.obj: $(SRCDIR)\soundselect.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\soundselect.c
mixer.obj: $(SRCDIR)\mixer.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\mixer.c
filelist.obj: $(SRCDIR)\filelist.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\filelist.c
utl.obj: $(SRCDIR)\utl.c $(SRCDIR)\tclock.h $(RESOURCEH)
    $(CC) $(COPT)$@ $(SRCDIR)\utl.c
iccsupp.obj: $(SRCDIR)\iccsupp.c
    $(CC) $(COPT)$@ $(SRCDIR)\iccsupp.c

$(RESFILE): $(RCFILE)
    $(RC) /l $(LANGID) /fo $(RESFILE) $(RCFILE)
