/*****************************************************************************
 *                                                                           *
 * unistd.h                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include <sys/unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <io.h>

/** KMS **/
#define popen _popen
#define pclose _pclose


#ifdef _MSC_VER 
#define inline __inline
#define alloca _alloca
#endif
