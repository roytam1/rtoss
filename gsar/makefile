#
# Makefile for project gsar
#
# Written by Tormod Tjaberg 
# 
CC=gcc
CFLAGS=-pedantic -Wall  
DEFS=
LD = gcc
EXE=gsar
  
OBJS = gsar.o arg_func.o gsarbmg.o

$(EXE): $(OBJS)
	$(LD) $(OBJS) -o $@ 

gsar.o: gsar.c arg_func.h gsar.h
	$(CC) $(DEFS) $(CFLAGS) $< -c -o $@

gsarbmg.o: gsarbmg.c gsar.h
	$(CC) $(DEFS) $(CFLAGS) $< -c -o $@

arg_func.o: arg_func.c arg_func.h
	$(CC) $(DEFS) $(CFLAGS) $< -c -o $@
clean:
	rm $(EXE) *.o
