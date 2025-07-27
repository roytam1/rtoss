/* arg_func.c 
 *
 * Import      : #includes 
 * Export      : global functions
 * Description : functions for parsing the command line and getting
 *               arguments from the environment
 * Author      : Tormod Tjaberg
 *
 * Note: 
 * Not all programs may want both GetOpt and GetEnvArgs. Therefore I have
 * included special ifdef's for easy inclusion/exclusion.
 * 
 * define NO_GETOPT      ; if you don't want GetOpt
 * define NO_GETENVARG   ; if you don't want GetEnvArgs
 *
 * The default is both functions
 *
 * Copyright (C) 1992-2008 Tormod Tjaberg
 * This is free software, distributed under the terms of the
 * GNU General Public License. For details see the file COPYING
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "arg_func.h"

/* gsar does not parse the environment 
 */
#define NO_GETENVARG

#ifndef NO_GETOPT

/* variables used by GetOpt
 */
int   OptInx = 1;             /* index into argv, skip argv[0]; filename */
int   CurOpt;                 /* current option checked for validity */
char  *pOptArg;               /* argument associated with option */


/* GetOpt()
 *
 * Input  : argc - number of arguments on the command line
 *          argv - pointer to the argument vector
 *          pOptStr - pointer to a string containing valid option characters
 * Returns: Current option being processed or an error
 *
 * Scan elements of argv (length is argc) for option characters given in
 * 'pOptStr'. Each time GetOpt is called it returns the next option character
 * from the option elements. When there are no more option charactes left
 * GetOpt returns EOF.
 *
 * 'pOptStr' is a string containing all valid option characters. If an option
 * character is encountered that is not in 'pOptStr', GetOpt returns:
 * BAD_CHAR. The offending character is stored in 'CurOpt'.
 *
 * If a character in the option string is followed by a colon, it means
 * that this option requires an argument. The argument can be the remainding
 * text in the current argv element, or the next argv element. The argument
 * is returned in 'pOptArg'. If no argument was found GetOpt returns:
 * MISSING_ARG and 'pOptArg' is set to NULL. 'CurOpt' contains the offending
 * option char.
 *
 * If a character is followed by two colons it means that this option
 * wants an optional argument. If there is any text left in the current argv
 * element it is returned in 'pOptarg'. If no argument was found 'pOptArg'
 * is set to NULL and the option character is returned.
 *
 * If the option string contains '|' EOF will not be returned when a
 * non option if found. Instead GetOpt returns '|'. If '|' is not specified
 * NON_OPT is returned. In both cases 'pGetOpt' will poin to the non option.
 * and 'OptInx' is the index of argv that contains the non option.
 *
 * Note, the following command line arguments have a special meaning:
 *
 * '-' : GetOpt returns: MISSING_OPT, 'pOptArg' is set to NULL
 * '--': Forces an end of option scanning. GetOpt returns: EOF
 *       Can be detected by checking if `OptInx' != argc, argv[OptInx]
 *       points to the argv element following the '--'
 *
 * Heavily inspired by the UN?X getopt, but a bit simpler
 */
int GetOpt(int argc, char **argv, char *pOptStr)
{
   static char *pCurOpt = "";  /* pointer to current option */
   char        *pOptChar;      /* pointer to char in option string */

   if (*pCurOpt == '\0')     /* get a new pointer */
   {
      if (OptInx >= argc)   /* no more of arguments on command line */
         return EOF;

      pCurOpt = argv[OptInx];

      if (*(pCurOpt++) != '-')
      {
         pCurOpt = "";         /* next time, get a new argv */
         pOptArg = argv[OptInx];
         OptInx++;

         if (strchr(pOptStr, '|') != NULL)
            return '|';        /* This might be a filename */
         else
            return NON_OPT;
      }

      if (*pCurOpt == '\0')
         return MISSING_OPT;

      if (*pCurOpt == '-')   /* catch the '--' */
      {
         OptInx++;             /* point to the argument behind '--' */
         return EOF;
      }
   }

   CurOpt = *pCurOpt++;        /* CurOpt is the option character */

   if ((pOptChar = strchr(pOptStr, CurOpt)) == NULL)
      return BAD_CHAR;         /* char not in option string, culprit in CurOpt */

   if (*(++pOptChar) != ':') /* option doesn't need argument */
   {
      pOptArg = NULL;
      if (*pCurOpt == '\0')
         ++OptInx;
   }
   else
   {                           /* option needs an argument, but how bad ? */
      if (*pCurOpt != '\0')   /* remainder of argv is argument -d123 -> arg = 123 */
         pOptArg = pCurOpt;
      else
         if (*(++pOptChar) == ':') /* '::' optional arg, but no argument */
            pOptArg = NULL;
         else                        /* nothing behind option check next arg */
         {
            if (argc <= ++OptInx)
            {
               pOptArg = NULL;       /* no arguments left but we needed one */
               return MISSING_ARG;   /* return missing arg for option, culprit in CurOpt */
            }
            else                     /* white space separating arguments */
               pOptArg = argv[ OptInx ];
         }

      pCurOpt = "";                  /* next time get a new argv */
      ++OptInx;                      /* next argv */
   }
   return CurOpt;                    /* return option letter */
}

#endif

#ifndef NO_GETENVARG
/* GetEnvArgs()
 *
 * Input  : argc - number of arguments on the command line
 *          argv - pointer to the argument vector
 *          pEnvVar - environment variable we are to find
 * Returns: Number of arguments found in environment
 *
 * Create a new arvector with the enviroment arguments before the
 * program arguments. This way a command line option will override
 * the environment option. Any number of args can be given since we
 * malloc the new argument vector. The environment variable is not destroyed.
 *
 * This code originally used realloc, and did not make a copy of the
 * environment variable.
 */
int GetEnvArgs(int *argc, char ***argv, char *pEnvVar)
{
   char  *pEnvStr;      /* pointer to environment string */
   char  **pArgV;       /* pointer to our new argument vector */
   char  **pStartArgV;  /* pointer to our new argument vector */
   char  *cp;           /* character pointer */
   char  *pBuf;         /* pointer to copy of environment string */
   int count = 0;
   int envcount = 0;
   unsigned char ch;

   /* fetch contents of environment variable, if any */
   pEnvStr = getenv(pEnvVar);

   if (pEnvStr == NULL || *pEnvStr == 0)
      return 0 ;

   while (isspace(*pEnvStr)) /* avoid adding whitespace onto pArgV */
      pEnvStr++;

   if ((pBuf = (char *) malloc(1 + strlen(pEnvStr))) == NULL)
      Abort("error: unable to malloc memory for copy of environment string");

   strcpy(pBuf,pEnvStr);     /* copy the original string into buffer */

   /* count how many arguments there are... so we can malloc */
   cp = pBuf;
   while(*cp != '\0')        /* traverse environment string  */
   {
      while (!isspace(*cp) && *cp != '\0')
         cp++;
      while (isspace(*cp))
         cp++;
      count++;
   }

   if (count == 0)           /* if environment string is empty */
      return 0;

   envcount = count;

   /* malloc a vector that will hold all args + 1 to hold NULL */
   if ((pArgV = pStartArgV = (char **) malloc((count + *argc + 1) * sizeof(char *))) == NULL)
      Abort("error: unable to malloc for arguments");

   *(pArgV++) = *((*argv)++);  /* copy argv[0] (filename) from argv */

   do
   {
      *(pArgV++) = pBuf;       /* assign argument to new arg vector */
      while ((ch = *pBuf) != '\0' && !isspace(ch))
         pBuf++;               /* step over the argument */
      if (ch != '\0')
         *(pBuf++) = '\0';     /* create a null terminated string */
      while ((ch = *pBuf) != '\0' && isspace(ch))
         pBuf++;               /* step over the whitespace */
   }  while (ch != '\0');

   /* now save old argc and copy in the old args */
   count += *argc;

   /* copy original arg vector */
   while (--(*argc))
      *(pArgV++) = *((*argv)++);

   *pArgV = NULL;      /* The C-standard specifies that argv[argc] == NULL */
   *argc = count;
   *argv = pStartArgV; /* make argv point to our new vector */

   return  envcount;   /* number of args found in environment */
}
#endif

