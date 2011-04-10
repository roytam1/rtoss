/*****************************************************************************
 *                                                                           *
 * DH_STR.C                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <stdlib.h>
#include <string.h>


/* Function stuff ************************************************************/

/* Compare strings, ignoring case, to a specific length ==================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int strncasecmp(char* string_One,char* string_Two,int string_Length)
{
	int string_Index = 0;

	/* Check for different characters */
	while (string_Index < string_Length)
	{
		if (toupper(string_One[string_Index]) != toupper(
		 string_Two[string_Index]))
		{
			return (toupper(string_One[string_Index]) < 
			 toupper(string_Two[string_Index]))?-1:1;
		}

		string_Index++;
	}

	/* The same! */
	return 0;
	
}
#endif

/* Compare strings, ignoring case ========================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int strcasecmp(char* string_One,char* string_Two)
{
	/* If they're not the same length, they're different */
	if (strlen(string_One) != strlen(string_Two))
	{
		return (strlen(string_One) < strlen(string_Two))?-1:1;
	}

	return strncasecmp(string_One,string_Two,strlen(string_One));
}
#endif

/* Find the first bit set ================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int ffs(int bit_Bitmap)
{
	int bit_Index;

	/* Find the first bit */
	for (bit_Index = 0;bit_Index < sizeof(int)*8;bit_Index++)
	{
		/* Found it, return the index */
		if (bit_Bitmap&(int)exp(bit_Index))
		{
			return bit_Index+1;
		}
	}

	/* Return nothing */
	return 0;
}
#endif

/* Find a character in a string ============================================ */
#if (_DOWNHILL_POSIX_SOURCE == 0)
char* index(char* search_String,char search_Character)
{
	return strchr(search_String,search_Character);
}
#endif

/* Find a character in a string, from the right ============================ */
#if (_DOWNHILL_POSIX_SOURCE == 0)
char* rindex(char* search_String,char search_Character)
{
	return strrchr(search_String,search_Character);
}
#endif
