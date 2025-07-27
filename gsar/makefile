#
# Makefile for project gsar
#
# Written by Tormod Tjaberg 
# 
CC=gcc
CFLAGS=-Wall -Wpointer-arith -Wstrict-prototypes -pedantic -O3 \
 	-fms-extensions -mms-bitfields -fno-exceptions -fomit-frame-pointer \
	-march=i386 -ffast-math
DEFS=-D__USE_MINGW_ACCESS
VERSION = 1.21
MAJOR=$(shell echo $(VERSION) | sed -e "s/\..*$$//")
MINOR=$(shell echo $(VERSION) | sed -e "s/^[^\.]*\.0*\([0-9]\+\).*$$/\1/") 
VERSADD = -Wl,--major-image-version=$(MAJOR) -Wl,--minor-image-version=$(MINOR)
LD = gcc
LDFLAGS=-Wl,-s -Wl,--force-exe-suffix -Wl,--enable-auto-import \
	-Wl,--enable-runtime-pseudo-reloc \
 	-Wl,--allow-multiple-definition -Wl,--enable-stdcall-fixup $(VERSADD)
EXE=gsar
EXEEXT=.exe 
  
OBJS = gsar.o arg_func.o gsarbmg.o gsar-res.o

.SUFFIXES:
.SUFFIXES: .c .o .rc .exe

# Rule to make compiled resource (Windows)
.rc.o:
	windres $< $@

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ 

gsar.o: gsar.c arg_func.h gsar.h
	$(CC) $(DEFS) $(CFLAGS) $< -c -o $@

gsarbmg.o: gsarbmg.c gsar.h
	$(CC) $(DEFS) $(CFLAGS) $< -c -o $@

arg_func.o: arg_func.c arg_func.h
	$(CC) $(DEFS) $(CFLAGS) $< -c -o $@
clean:
	rm $(EXE)$(EXEEXT) *.o
