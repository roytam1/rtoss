/*****************************************************************************
 *                                                                           *
 * sys/types.h                                                               *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_TYPES_H)
#define _DOWNHILL_SYS_TYPES_H


/* Typedef stuff *************************************************************/

/* Good typedefs =========================================================== */
typedef long           uid_t;
typedef long           gid_t;
typedef long           pid_t;
typedef unsigned short mode_t;
typedef short          nlink_t;
typedef char*          caddr_t;

/* Bad typedefs ============================================================ */
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;
typedef u_int          uint;
typedef u_short        ushort;


/* Include stuff *************************************************************/
/* Note: This stuff has to go after the typedefs because these files include
         files that use the new types. */
#include "DOWNHILL/DH_INC.H"
#include _DOWNHILL_INCLUDE_DIR/sys/types.h>
#include _DOWNHILL_INCLUDE_DIR/windows.h>


#endif /* _DOWNHILL_SYS_TYPES_H */
