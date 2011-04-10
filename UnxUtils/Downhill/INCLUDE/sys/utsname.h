/*****************************************************************************
 *                                                                           *
 * sys/utsname.h                                                             *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_UTSNAME_H)
#define _DOWNHILL_SYS_UTSNAME_H


/* Include stuff *************************************************************/
#include <sys/param.h>


/* Struct stuff **************************************************************/

/* utsname struct ========================================================== */
struct utsname
{
	char sysname[64+1];
	char nodename[MAXHOSTNAMELEN+1];
	char version[64+1];
	char release[64+1];
	char machine[64+1];
	char idnumber[16+1];
};


/* Prototype stuff ***********************************************************/
int uname(struct utsname* utsname_Info);


#endif /* _DOWNHILL_SYS_UTSNAME_H */
