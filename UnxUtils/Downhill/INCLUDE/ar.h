/*****************************************************************************
 *                                                                           *
 * ar.h                                                                      *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_AR_H)
#define _DOWNHILL_AR_H


/* Define stuff **************************************************************/
#define ARMAG       "!<arch>\012"
#define SARMAG      8
#define ARFMAG      "`\012"
#define AR_NAME_LEN 16


/* Struct stuff **************************************************************/
struct ar_hdr
{
	char ar_name[AR_NAME_LEN];
	char ar_date[12];
	char ar_uid[6];
	char ar_gid[6];
	char ar_mode[8];
	char ar_size[10];
	char ar_fmag[2];
};


#endif /* _DOWNHILL_AR_H */
