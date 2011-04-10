/*****************************************************************************
 *                                                                           *
 * utime.h                                                                   *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_UTIME_H)
#define _DOWNHILL_UTIME_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <windows.h>
#include <sys/utime.h>


/* Prototype stuff ***********************************************************/
#if (_DOWNHILL_POSIX_SOURCE == 0)
int utimes(char* file_Name,struct timeval* file_Time);
#endif


#endif /* _DOWNHILL_UTIME_H */
