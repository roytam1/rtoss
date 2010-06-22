all:
	-mkdir out
	-mkdir work
	cd work
	$(MAKE) -f ..\dll\tcdll.mak SRCDIR=..\dll
	$(MAKE) -f ..\language\langja.mak SRCDIR=..\language
	$(MAKE) -f ..\exe\tclock.mak SRCDIR=..\exe
	cd ..
