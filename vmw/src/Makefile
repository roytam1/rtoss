# master makefile for vmware command tools
# calls system specific makefile (Makefile.`uname`)
#
# Copyright (c) 2006 Ken Kato

.PHONY : all clean vmw vmshrink vmftp
all clean vmw vmshrink vmftp :: make_force
	@echo Making \'$@\' for `uname` \(Makefile.`uname`\)
	@$(MAKE) $(MAKEFILAGS) -f Makefile.`uname` $@
	-@rm -f make_force

make_force ::
	-@touch $@

.SUFFIXES :
