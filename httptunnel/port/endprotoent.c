/*
port/endprotoent.c

Copyright (C) 1999 Lars Brinkhoff.  See COPYING for terms and conditions.
*/

#include "config.h"

#include <netdb_.h>

#ifndef HAVE_ENDPROTOENT
void
endprotoent (void)
{
  /* do nothing */
}
#endif
