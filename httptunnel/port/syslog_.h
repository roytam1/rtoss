/*
port/syslog_.h

Copyright (C) 1999 Lars Brinkhoff.  See COPYING for terms and conditions.
*/

#ifndef PORT_SYSLOG_H
#define PORT_SYSLOG_H

#include "config.h"

#include <stdarg.h>

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#else
void syslog (int level, const char *fmt0, ...);
#endif

#ifndef HAVE_VSYSLOG
void vsyslog (int level, const char *fmt0, va_list ap);
#endif

#ifndef LOG_DAEMON
#define LOG_DAEMON 0
#endif

#ifndef LOG_PID
#define LOG_PID 0
#endif

#ifndef LOG_NOTICE
#define LOG_NOTICE 0
#endif

#ifndef LOG_ERROR
#define LOG_ERROR 0
#endif


#endif
