/*****************************************************************************
 *                                                                           *
 * DH.H                                                                      *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_DOWNHILL_DH_H)
#define _DOWNHILL_DOWNHILL_DH_H


/* Include stuff *************************************************************/
#include "downhill.h"


/* Define stuff **************************************************************/
#define DOWNHILL_SLEEP_TIME 100
#if !defined(_DOWNHILL_MACHINE_TYPE)
#define _DOWNHILL_MACHINE_TYPE "x86"
#endif
#if !defined(_DOWNHILL_GETOPT_SWITCH)
#define _DOWNHILL_GETOPT_SWITCH '/'
#endif
#if !defined(_DOWNHILL_HOSTSEQUIV_PATH)
#define _DOWNHILL_HOSTSEQUIV_PATH "C:\\ETC\\HOSTS.EQV"
#endif
#if !defined(_DOWNHILL_RHOSTS_FILE)
#define _DOWNHILL_RHOSTS_FILE "RHOSTS"
#endif
#if !defined(_DOWNHILL_NETRC_FILE)
#define _DOWNHILL_NETRC_FILE "NETRC"
#endif
#if !defined(_DOWNHILL_STAT_BLOCKSIZE)
#define _DOWNHILL_STAT_BLOCKSIZE 512
#endif
#if !defined(_DOWNHILL_USER_NAME)
#define _DOWNHILL_USER_NAME "user"
#endif
#if !defined(_DOWNHILL_USER_IDREAL)
#define _DOWNHILL_USER_IDREAL 0
#endif
#if !defined(_DOWNHILL_USER_IDEFFECTIVE)
#define _DOWNHILL_USER_IDEFFECTIVE 0
#endif
#if !defined(_DOWNHILL_USER_IDSAVED)
#define _DOWNHILL_USER_IDSAVED 0
#endif
#if !defined(_DOWNHILL_USER_PASSWORD)
#define _DOWNHILL_USER_PASSWORD "*"
#endif
#if !defined(_DOWNHILL_USER_GECOS)
#define _DOWNHILL_USER_GECOS "User"
#endif
#if !defined(_DOWNHILL_USER_DIR)
#define _DOWNHILL_USER_DIR "C:\\"
#endif
#if !defined(_DOWNHILL_USER_SHELL)
#define _DOWNHILL_USER_SHELL "C:\\COMMAND.COM"
#endif
#if !defined(_DOWNHILL_GROUP_NAME)
#define _DOWNHILL_GROUP_NAME "group"
#endif
#if !defined(_DOWNHILL_GROUP_IDREAL)
#define _DOWNHILL_GROUP_IDREAL 0
#endif
#if !defined(_DOWNHILL_GROUP_IDEFFECTIVE)
#define _DOWNHILL_GROUP_IDEFFECTIVE 0
#endif
#if !defined(_DOWNHILL_GROUP_IDSAVED)
#define _DOWNHILL_GROUP_IDSAVED 0
#endif
#if !defined(_DOWNHILL_GROUP_PASSWORD)
#define _DOWNHILL_GROUP_PASSWORD "*"
#endif


#define ARG_MAX 128*1024

/* Prototype stuff ***********************************************************/
pid_t downhill_Process_GetStatusAny(int* process_Status);
int   downhill_Process_IsSignaled(pid_t process_Id);
void  downhill_Process_Signal(pid_t process_Id);
int   downhill_Signal_IsInterrupted(void);


#endif /* _DOWNHILL_DOWNHILL_DH_H */
