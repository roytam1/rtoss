/*****************************************************************************
 *                                                                           *
 * sys/param.c                                                               *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_PARAM_H)
#define _DOWNHILL_SYS_PARAM_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <stdlib.h>


/* Define stuff **************************************************************/
#if (_DOWNHILL_POSIX_SOURCE == 0)
#define MAXPATHLEN     _MAX_PATH
#define MAXHOSTNAMELEN 64
#define howmany(x,y)   (((x)+((y)-1))/(y))
#define roundup(x,y)   ((((x)+((y)-1))/(y))*(y))
#endif


#endif /* _DOWNHILL_SYS_PARAM_H */
