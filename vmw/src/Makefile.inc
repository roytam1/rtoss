# common makefile for vmware command tools
#
# Copyright (c) 2006 Ken Kato

# object files common to all target systems

VMWOBJ = vmw.o vmtool.o termutil.o
SHROBJ = vmshrink.o vmtool.o termutil.o
FTPOBJ = vmftp.o vmshf.o vmtool.o

# make rules

.PHONY : all clean

all : vmw vmshrink vmftp

vmw : $(VMWOBJ) $(VMCALL_O)
	$(LD) $(LDFLAGS) $(VMWOBJ) $(VMCALL_O) $(VMWLIB) -o $@

vmshrink : $(SHROBJ) $(VMCALL_O)
	$(LD) $(LDFLAGS) $(SHROBJ) $(VMCALL_O) $(SHRLIB) -o $@

vmftp : $(FTPOBJ) $(VMCALL_O) $(VMINT_O)
	$(LD) $(LDFLAGS) $(FTPOBJ) $(VMCALL_O) $(VMINT_O) $(FTPLIB) -o $@

.c.o :
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.s.o :
	$(AS) $(ASFLAGS) $< -o $@

clean :
	-rm -f core vmw vmshrink vmftp *.o

# source file dependencies

termutil.o: termutil.c termutil.h vmint.h
vmftp.o: vmftp.c vmshf.h vmtool.h vmint.h
vmshf.o: vmshf.c vmshf.h vmtool.h vmint.h
vmshrink.o: vmshrink.c vmtool.h vmint.h termutil.h
vmtool.o: vmtool.c vmcall.h vmint.h vmtool.h
vmw.o: vmw.c vmtool.h vmint.h termutil.h
vmcall.o: vmcall.c vmcall.h
