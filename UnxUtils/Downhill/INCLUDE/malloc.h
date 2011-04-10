/*****************************************************************************
 *                                                                           *
 * malloc.h                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_MALLOC_H)
#define _DOWNHILL_MALLOC_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_INC.H"
#include "DOWNHILL/DH_POSIX.H"
#include _DOWNHILL_INCLUDE_DIR/malloc.h>


/* Prototype stuff ***********************************************************/
#if (_DOWNHILL_POSIX_SOURCE == 0)
void cfree(void* mem_Block);
#endif


#endif /* _DOWNHILL_MALLOC_H */
