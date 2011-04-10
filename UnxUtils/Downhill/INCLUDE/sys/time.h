/*****************************************************************************
 *                                                                           *
 * sys/time.h                                                                *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_TIME_H)
#define _DOWNHILL_SYS_TIME_H


/* Include stuff *************************************************************/
#include <winsock.h>
#include <time.h>


/* Struct stuff **************************************************************/
struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};


/* Prototype stuff ***********************************************************/
int gettimeofday(struct timeval* time_Info,struct timezone* timezone_Info);


#endif /* _DOWNHILL_SYS_TIME_H */
