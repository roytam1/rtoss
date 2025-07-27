/* gsar.h
 *
 * Description : Header file for gsar
 * Author      : Tormod Tjaberg
 *
 * Changes:
 *
 * Currently compiles under :
 * Turbo C 2.0, Turbo C++ 1.0, Turbo C++ 3.0, Zortech C++ 3.0,
 * Watcom C 386 8.0, Ultrix ANSI C, Microsoft 6.0, GCC
 */


/* System type defined upon the following
 * __TURBOC__ : All Borland C/C++ versions
 * __ZTC__    : Zortech/Symantec
 * __DJGPP__  : DJ Delorie's GNU port to the PC
 * __WATCOMC__: Watcom C/C++
 * __MINGW32__: MinGW
 * MSDOS      : Set by various DOS compilers... ( Watcom 386, Microsoft )
 */

#if defined(__TURBOC__) || defined(__ZTC__) || defined(MSDOS) || defined(__DJGPP__) || defined (__WATCOMC__) || defined (__MINGW32__)
#ifndef MSDOS
#define MSDOS     5
#endif
#else
#ifndef __UNIX__
#define __UNIX__  3
#endif
#endif

#ifndef BUFSIZ                            /* ANSI C predefined constant */
#ifdef  MSDOS
#define BUFSIZ 1024                       /* size of search buffer */
#else             /* its __UNIX__ */
#define BUFSIZ 4048                       /* size of search buffer */
#endif
#endif

#define TXT_CONTEXT 80      /* length of textual context     */
#define HEX_CONTEXT 64      /* amount of hex bytes displayed */
#define PAT_BUFSIZ  256     /* buffer limit < unsigned short */

typedef struct
{
   signed char fVerbose;    /* true if we are to be verbose */
   signed char fByteOffset; /* display byte offset in file */
   signed char fTextual;    /* display context textual */
   signed char fFileSpec;   /* display filespec */
   signed char fHex;        /* display contents in hex */
   char *pInputFile;        /* current input file name */
   FILE *fpIn;              /* input stream */
   FILE *fpOut;             /* output stream */
   FILE *fpMsg;             /* message stream */
   unsigned short Context;  /* length of context to display */
} OUTPUT_CTRL;

/* function prototypes
 */
void BMG_Setup(char *, int, char);
long BMG_Search(OUTPUT_CTRL * );
long BMG_SearchReplace(OUTPUT_CTRL *, char *, unsigned short);

