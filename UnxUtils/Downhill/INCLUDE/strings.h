/*****************************************************************************
 *                                                                           *
 * strings.h                                                                 *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_STRINGS_H)
#define _DOWNHILL_STRINGS_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <string.h>


/* Prototype stuff ***********************************************************/
#if (_DOWNHILL_POSIX_SOURCE == 0)
int   ffs(int bit_Bitmap);
int   strcasecmp(char* string_One,char* string_Two);
int   strncasecmp(char* string_One,char* string_Two,int string_Length);
char* index(char* search_String,char search_Character);
char* rindex(char* search_String,char search_Character);
#endif


#endif /* _DOWNHILL_STRINGS_H */
