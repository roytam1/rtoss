/*****************************************************************************
 *                                                                           *
 * netdb.h                                                                   *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_NETDB_H)
#define _DOWNHILL_NETDB_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <sys/socket.h>


/* Prototype stuff ***********************************************************/
#if (_DOWNHILL_POSIX_SOURCE == 0)
int sethostent(int host_Stayopen);
int endhostent(void);
#endif


#endif /* _DOWNHILL_NETDB_H */
