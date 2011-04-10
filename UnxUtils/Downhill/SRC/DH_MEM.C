/*****************************************************************************
 *                                                                           *
 * DH_MEM.C                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <malloc.h>


/* Function stuff ************************************************************/

/* Free a memory block ===================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
void cfree(void* mem_Block)
{
	free(mem_Block);
}
#endif
