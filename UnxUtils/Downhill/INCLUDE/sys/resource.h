/*****************************************************************************
 *                                                                           *
 * sys/resource.h                                                            *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_RESOURCE_H)
#define _DOWNHILL_SYS_RESOURCE_H


/* Include stuff *************************************************************/
#include <sys/time.h>


/* Struct stuff **************************************************************/
struct  rusage
{
	struct timeval ru_utime;
	struct timeval ru_stime;
	long           ru_maxrss;
#define ru_first       ru_ixrss
	long           ru_ixrss;
	long           ru_idrss;
	long           ru_isrss;
	long           ru_minflt;
	long           ru_majflt;
	long           ru_nswap;
	long           ru_inblock;
	long           ru_oublock;
	long           ru_ioch;
	long           ru_msgsnd;
	long           ru_msgrcv;
	long           ru_nsignals;
	long           ru_nvcsw;
	long           ru_nivcsw;
#define ru_last        ru_nivcsw
};


#endif /* _DOWNHILL_SYS_RESOURCE_H */
