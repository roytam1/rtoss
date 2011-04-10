/*****************************************************************************
 *                                                                           *
 * ranlib.h                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_RANLIB_H)
#define _DOWNHILL_RANLIB_H


/* Struct stuff **************************************************************/
struct ranlib
{
	union
	{
		off_t ran_strx;
		char* ran_name;
	} ran_un;
	off_t ran_off;
};


#endif /* _DOWNHILL_RANLIB_H */
