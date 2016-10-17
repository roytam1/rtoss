## Copyright (C) 1999 Lars Brinkhoff

SUBDIRS = lib
SUBDIRS += port

bin_PROGRAMS = htc hts
man_MANS = hts.1 htc.1

AM_CPPFLAGS=$(CPPFLAGS)
if SRCDIR
AM_CPPFLAGS += -I$(VPATH)/lib -I$(VPATH)/port
else
AM_CPPFLAGS += -Ilib -Iport
endif

ACLOCAL_AMFLAGS = -I m4

htc_SOURCES = htc.c common.c tunnel.c http.c base64.c
htc_LDADD = $(top_srcdir)/lib/libgnu.a $(top_srcdir)/port/libport.a $(GETADDRINFO_LIB) $(HOSTENT_LIB) $(INET_NTOP_LIB) $(LIBSOCKET) $(LIBINTL) $(SERVENT_LIB)
hts_SOURCES = hts.c common.c tunnel.c http.c
hts_LDADD = $(top_srcdir)/lib/libgnu.a $(top_srcdir)/port/libport.a $(GETADDRINFO_LIB) $(HOSTENT_LIB) $(INET_NTOP_LIB) $(LIBSOCKET) $(LIBINTL) $(SERVENT_LIB)

noinst_HEADERS = common.h tunnel.h http.h base64.h

EXTRA_DIST = TODO HACKING DISCLAIMER doc/rfc1945.txt doc/rfc2068.txt \
             FAQ doc/rfc2045.txt hts.1 htc.1 debian/changelog debian/control \
             debian/copyright debian/dirs debian/docs debian/rules \
             debian/prerm debian/postinst m4/gnulib-cache.m4
