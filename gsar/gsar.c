/* gsar.c 
 *
 * Description : General Search And Replace utility (gsar)
 * Author      : Tormod Tjaberg & Hans Peter Verne
 *
 * Copyright (C) 1992-2008 Tormod Tjaberg & Hans Peter Verne,
 * This is free software, distributed under the terms of the
 * GNU General Public License. For details see the file COPYING
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "comp_dep.h"
#include "arg_func.h"
#include "gsar.h"

#define PROGRAM_NAME "gsar"
#define PROGRAM_VER  "1.21"

/* Need setmode to turn my streams into binary Under MS-DOS
 */
#ifdef MSDOS
#include <fcntl.h>
#include <io.h>
#endif

#ifdef __UNIX__
#include <unistd.h>
#endif

/* Clever little option mimic wildcard expansion from
 * shell ONLY available with the Zortech compiler ver 3.0x and upwards
 * And in the Symantec compiler though it's been renamed
 */
#if defined(__ZTC__)
#include <dos.h>
#ifdef WILDCARDS
WILDCARDS
#endif
#ifdef EXPAND_WILDCARDS
EXPAND_WILDCARDS
#endif
#endif

/* Macro to determine if char is a path or drive delimiter
 */
#define IS_PATH_DELIM(c)   ((c) == '\\' || (c) == ':' || (c) == '/')

#define LF  0xa
#define CR  0xd

#define SETVBUF_SIZ  0x4000      

unsigned short nItemsSearch = 0;
unsigned short nItemsReplace = 0;

/* if the flag below is set we are not allowed to interrupt!
 */
#if defined(__UNIX__)
/* SunOS does not have this definition, so play it safe */
static int fCriticalPart = 0;
#else
static sig_atomic_t fCriticalPart = 0;
#endif

OUTPUT_CTRL Ctrl;

char **pFileList;           /* list of files found on the command line */
char *pOutFile = NULL;      /* current output file name make sure it's NULL */
char  SearchBuf[PAT_BUFSIZ] = "";
char  ReplaceBuf[PAT_BUFSIZ] = "";

char  fSearchReplace = 0;  /* default to a search initially */
char  fFolded        = 0;  /* fold pattern ie. ignore case */
char  fOverWrite     = 0;  /* overwrite input file */
char  fForce         = 0;  /* force overwrite of existing output file */
char  fBuffers       = 0;  /* just display search & replace buffers */
char  fFilter        = 0;  /* make GSAR act like a filter */

/* Usage text and GNU licence information
 */
char *Usage[] =
{
  PROGRAM_NAME ", ver " PROGRAM_VER " -- Copyright (C) 1992-2008 Tormod Tjaberg & Hans Peter Verne",
  "",
  "Usage: gsar [options] [infile(s)] [outfile]",
  "Options are:",
  "-s<string> Search string ",
  "-r[string] Replace string. Use '-r' to delete the search string from the file",
  "-i         Ignore case difference when comparing strings",
  "-B         just display search & replace Buffers",
  "-f         Force overwrite of an existing output file",
  "-o         Overwrite the existing input file",
  "-c[n]      show textual Context of match, 'n' is number of bytes in context",
  "-x[n]      show context as a heX dump, 'n' is number of bytes in context",
  "-b         display Byte offsets of matches in file",
  "-l         only List filespec and number of matches (default)",
  "-h         suppress display of filespec when displaying context or offsets",
  "-du        convert a DOS ASCII file to UNIX (strips carriage return)",
  "-ud        convert a UNIX ASCII file to DOS (adds carriage return)",
  "-F         'Filter' mode, input from stdin and eventual output to stdout",
  "-G         display the GNU General Public Licence",
  "",
  "Ctrl characters may be entered by using a ':' in the string followed by the",
  "ASCII value of the character. The value is entered using ':' followed by three",
  "decimal digits or ':x' followed by two hex numbers. To enter ':' use '::'",
  NULL
};

char *Licence[] =
{
  "This program is free software; you can redistribute it and/or modify",
  "it under the terms of the GNU General Public License as published by",
  "the Free Software Foundation; either version 2 of the License, or",
  "(at your option) any later version.",
  "",
  "This program is distributed in the hope that it will be useful,",
  "but WITHOUT ANY WARRANTY; without even the implied warranty of",
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
  "GNU General Public License for more details.",
  "",
  "You should have received a copy of the GNU General Public License",
  "with this program; if not, write to the Free Software Foundation, Inc.,",
  "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.",
  NULL
};


/* Display the GNU General Public License
 */
void ShowLicence(void)
{
   int i = 0;

   while (Licence[i] != NULL)
   {
      fputs(Licence[i++], Ctrl.fpMsg);
      fputc('\n', Ctrl.fpMsg);
   }
}


/* Input  : Message to be displayed with the format of standard printf
 * Returns: Nothing, but instead it exits to operating system
 *
 * Terminate program, return to OS with a suitable return value
 * exit() will close all open files. All fatal errors are written
 * to stderr.
 */
void Abort(char *pMessage, ...)
{
   va_list argp;

   fprintf(stderr, "gsar: ");
   va_start(argp, pMessage);
   vfprintf(stderr, pMessage, argp);
   va_end(argp);
   fprintf(stderr, "\n");
   exit(EXIT_FAILURE);  /* exit & tell operating system that we've failed */
}


/* Ctrl-Break handler. Returns to operating system
 */
void CtrlBreak(int Val)
{
   if (fCriticalPart)
      return;

   signal(SIGINT, SIG_IGN);

   /* Before we die try to clean up as much as possible
    * make sure we don't delete stdout...
    */
   if (fSearchReplace && pOutFile != NULL && !fFilter)
      remove(pOutFile);

   exit(EXIT_FAILURE);  /* exit & tell operating system that we've failed */
}


/* Input  : pActStr - pointer to actual string which is to be duplicated
 * Returns: pDupStr - pointer to malloc'd duplicate
 *          NULL - Out of memory
 *
 * Duplicates a string by malloc'ing and then doing a strcpy
 */
char *DupStr(char *pActStr)
{
   register char *pDupStr;

   pDupStr = (char *) malloc(strlen(pActStr) + 1);
   if (pDupStr)
      strcpy(pDupStr, pActStr);
   return pDupStr;
}


/* Input  : filespec - a filespec which contains a path and a filename
 * Returns: savfilespec - pointer to malloc'd string with path
 *
 * Extracts the path from a filespec
 */
char *ExtractPathFromFSpec(register char *filespec)
{
   register char *p;
   register char *savfilespec;

   savfilespec = DupStr(filespec); /* avoid destroying the original */

   /* Start at end of string and back up till we find the beginning */
   /* of the filename or a path.               */
   for (p = savfilespec + strlen(savfilespec);
        p != savfilespec && !IS_PATH_DELIM(*(p - 1));
        p--)
      ;
   *p = '\0';
   return savfilespec;
}


/* Input  : pPath - which has to be properly terminated
 *          pPrefix - tmp filename prefix maximum 4 chars
 * Returns: pointer to static buffer
 *          NULL - unable to generate tmp file name
 *
 * Generates a temporary filename by combining an optional path,
 * a prefix ,a number between 0 & SHRT_MAX and the suffix '.tmp'.
 */
char *TmpName(char *pPath, char *pPrefix)
{
   static char FileSpec[FILENAME_MAX ];
   static char Seed = 0;

   static short TmpNum;
   char *pDigits;
   unsigned short i;
   struct stat buf;

   *FileSpec = '\0'; /* Null terminate the buffer so strcat will always work */
   if (!Seed)      /* if first time through */
   {
      Seed++;
      TmpNum = -1; /* since we increment this will make the first number 0000 */
   }

   if (pPath != NULL)
      strcpy(FileSpec, pPath);     /* first the path  */
   if (pPrefix != NULL)
      strcat(FileSpec, pPrefix);   /* then the prefix */

   /* pointer to start of digits in filespec */
   pDigits = FileSpec + strlen(FileSpec);

   for (i = 0; i <= SHRT_MAX; i++)     /* try SHRT_MAX times */
   {
      TmpNum++;                        /* TmpNum is static ! */
      sprintf(pDigits, "%04x", TmpNum);/* convert to string */
      strcat(FileSpec, ".tmp");        /* add the suffix */
      if (stat(FileSpec, &buf) != 0)
         return FileSpec;                /* file not found, success */
      *(pDigits) = '\0';               /* pre_XXXX --> pre_ */
   }
   /* unable to create a temporary file ! more than SHRT_MAX files !!*/
   return NULL;
}


/* Input    : pBuffer pointer to character buffer
 *            nItem number of items in buffer
 *            base  1 = hex , 0 = ASCII
 * Returns  : nothing
 *
 * Prints the contents of a buffer in either ASCII or HEX. The
 * nItems variable is needed since we don't stop on a NUL
 */
void DumpBuffer(char *pBuffer, unsigned short nItem, unsigned char Base)
{
   unsigned short i;                 /* loop counter */

   if (!Base)
   {
      for (i = 0; i < nItem; i++)
      {
#ifdef MSDOS   /* MSDOS can display all characters except CTRL chars */
         if (!iscntrl((int) * pBuffer))
#else         /* its __UNIX__ */
         if (isprint((int) * pBuffer))
#endif
            fputc(*pBuffer, Ctrl.fpMsg);
         else
            fputc('.', Ctrl.fpMsg);

         pBuffer++;
      }
   }
   else
      for (i = 0; i < nItem; i++)
         fprintf(Ctrl.fpMsg, "%02x ", (unsigned char) * pBuffer++);

   fputc('\n', Ctrl.fpMsg);
}


/* Input    : pArgStr - pointer to command line search or replace string
 *            pBuffer - pointer to buffer to store the parsed string
 * Returns  : actual length of parsed string
 *
 * Takes a string & transforms it into the correct internal representation
 * ie :070OO -> FOO or :x46OO -> FOO
 */
unsigned short GetPattern(char *pArgStr, char *pBuffer)
{
   char  number[4 ];/* array to store number to convert */
   char *pEnd;      /* pointer to the result of the string to long conversion */
   char *pStart;    /* pointer to the start of the buffer */

   pStart = pBuffer;
   number[3] = '\0'; /* make sure number buffer is terminated */

   while (*pArgStr != '\0')
   {
      if (*pArgStr != ':')
         *pBuffer++ = *pArgStr++;       /* just copy the buffer */
      else
      {
         if (*(pArgStr + 1) == ':')    /* check for multiple :'s */
         {
            *pBuffer++ = *pArgStr;
            pArgStr = pArgStr + 2;       /* position ourselves past the 2 :'s */
         }
         else
         {  /* have we actually got three chars to copy ? */
            if (strlen(++pArgStr) > 2)
            {
               memcpy(number, pArgStr, 3);/* negative numbers are silently ignored*/
               pArgStr += 3;              /* jump past this number in the argument string */
               if ((char) tolower(number[0]) == 'x')
               {                          /* perform hex conversion */
                  number[0] = '0';        /* replace the x with a zero */
                  *pBuffer++ = (char) strtol(number, &pEnd, 16);
                  if (pEnd != number + 3)
                     Abort("command error, not a valid hexadecimal number : %s\n", &number[1]);
               }
               else                       /* decimal conversion */
               {
                  *pBuffer++ = (char) strtol(number, &pEnd, 10);
                  if (pEnd != number + 3)
                     Abort("command error, not a valid decimal number : %s\n", number);
               }
            }
            else                          /* strlen(pArgStr) < 3 so abort */
               Abort("command error, a single colon must be followed by three decimal digits or an 'x' followed by 2 hexadecimal numbers\n");
         }
      }
      if (pBuffer - pStart > PAT_BUFSIZ)
         Abort("command error, length of search or replace buffer must not exceed %d chars", PAT_BUFSIZ);
   }  /* while */
   return pBuffer - pStart;               /* actual length of buffer */
}


/* Input  : argc - number of arguments on the command line
 *          argv - pointer to the argument vevtor
 * Returns: number of actual filenames found
 *
 * Parses the command line & returns number of filenames found
 */
int GetArgs(int argc, char *argv[])
{
   int   i = 0;
   int   j = 0;          /* counters */
   int   c;              /* switch char */
   char *pEnd;           /* the result of the string to long conversion */
   long  longVal;


   pFileList = NULL;

   if (argc > 1)
      while ((c = GetOpt(argc, argv, "|s::r::iBfoc::x::blhd::u::FG")) != EOF)
      {
         switch (c)
         {
            case '|':
               /* create a vector of file pointers */
#if defined(__UNIX__)
               /* SunOS doesn't allow realloc to be called with NULL :-( 
                * Insert extra test on unix systems...
                */
               if (pFileList == NULL)
                  pFileList = (char **) malloc((j + 2) * sizeof(char *));
               else
#endif
                  pFileList = (char **) realloc(pFileList, (j + 2) * sizeof(char *));
               pFileList[j++] = pOptArg;
               pFileList[j] = NULL;   /* The C-standard specifies that argv[argc] == NULL */
               break;
            case MISSING_ARG:
               Abort("command error, the '%c' option requires an argument", (unsigned char) CurOpt);
               break;
            case BAD_CHAR:
               Abort("command error, unknown option '%c'. Type 'gsar' by itself for help", (unsigned char) CurOpt);
               break;
            case MISSING_OPT:
               i = 0;
               while (Usage[i] != NULL)
               {
                  fputs(Usage[i++], Ctrl.fpMsg);
                  fputc('\n', Ctrl.fpMsg);
               }
               exit(EXIT_SUCCESS);
               break;
            case 's':
               if (pOptArg == NULL)
                  Abort("command error, the '%c' option requires an argument", (unsigned char) CurOpt);
               
               nItemsSearch = GetPattern(pOptArg, SearchBuf);
               break;
            case 'r':
               if (pOptArg == NULL)
                  nItemsReplace = 0;    /* we are to remove occurrence of -s */
               else
                  nItemsReplace = GetPattern(pOptArg, ReplaceBuf);
               fSearchReplace = 1;     /* only search & replace if -r option */
               break;
            case 'u':                  /* replace LF with CR LF */
               if (pOptArg == NULL)
                  pOptArg = "";
               if (!(*pOptArg == 'd' && *(pOptArg + 1) == '\0'))
                  Abort("command error, unknown option 'u%s'. Type 'gsar' by itself for help", pOptArg);
               fSearchReplace = 1;
               nItemsSearch = 1;
               SearchBuf[0] = LF;
               nItemsReplace = 2;
               ReplaceBuf[0] = CR;
               ReplaceBuf[1] = LF;
               break;
            case 'd':                  /* replace CR LF with LF */
               if (pOptArg == NULL)
                  pOptArg = "";
               if (!(*pOptArg == 'u' && *(pOptArg + 1) == '\0'))
                  Abort("command error, unknown option 'd%s'. Type 'gsar' by itself for help", pOptArg);
               fSearchReplace = 1;
               nItemsSearch = 2;
               SearchBuf[0] = CR;
               SearchBuf[1] = LF;
               nItemsReplace = 1;
               ReplaceBuf[0] = LF;
               break;
            case 'i':
               fFolded = 1;
               break;
            case 'G':
               ShowLicence();
               exit(EXIT_SUCCESS);
               break;
            case 'l':
               Ctrl.fTextual = 0;      /* return to terse display */
               Ctrl.fHex = 0;
               Ctrl.fByteOffset = 0;
               break;
            case 'o':
               fOverWrite = 1;
               break;
            case 'f':
               fForce = 1;
               break;
            case 'F':
               fFilter = 1;
               break;
            case 'B':
               fBuffers = 1;
               break;
            case 'b':
               Ctrl.fByteOffset = 1;    /* display file offset */
               break;
            case 'h':
               Ctrl.fFileSpec = 0;      /* turn off filespec */
               break;
            case 'x':
               Ctrl.fTextual = -1;      /* signal fall through */
               Ctrl.fHex = 1;           /* display context in hex */
            case 'c':
               if (Ctrl.fTextual == -1)  /* entered through case 'x' */
                  Ctrl.fTextual = 0;        /* reset sentinel value  */
               else
               {
                  Ctrl.fHex = 0;        /* entered through case 'c' */
                  Ctrl.fTextual = 1;    /* display textual context */
               }

               if (pOptArg == NULL)
                  Ctrl.Context = (Ctrl.fHex == 1) ? HEX_CONTEXT : TXT_CONTEXT;
               else
               {
                  longVal = strtol(pOptArg, &pEnd, 0);
                  if (*pEnd != '\0')
                     Abort("command error, invalid number : %s", pOptArg);

                  if (longVal < 16 || longVal > BUFSIZ)
                     Abort("command error, context size must be in the range 16 to %d", BUFSIZ);

                  if (longVal > USHRT_MAX)
                     Ctrl.Context = USHRT_MAX;
                  else
                     Ctrl.Context = (unsigned short) longVal;
               }
               break;
            default:
               Abort("internal error, option '%c' not handled in switch", (unsigned char) CurOpt);
               break;
         }
      }
   else
   {
      i = 0;
      while (Usage[i] != NULL)
      {
         fputs(Usage[i++], Ctrl.fpMsg);
         fputc('\n', Ctrl.fpMsg);
      }
      exit(EXIT_SUCCESS);
   }

   Ctrl.fVerbose = (Ctrl.fTextual ||
                      Ctrl.fHex ||
                      Ctrl.fByteOffset) ? 1 : 0;

   return j;
}



/* Perform search or replace using stdin and stdout ie as a 'filter'
 * When gsar operates as a filter all output ie context, byte filenames
 * etc are all sent to stderr.
 */
void StreamSearchReplace(void)
{
   long  nMatches;

   Ctrl.pInputFile = "stdin";      /* proper filename   */
   Ctrl.fpMsg = stderr;            /* redirect messages */

#ifdef MSDOS
   /* when MSDOS operates on streams it translates characters and terminates
    * input and output when it encounters a CTRL Z. The code below makes
    * MSDOS treat the streams as binary. Stdin cannot be used since MSDOS
    * ignores the CTRL-Z. If you pipe a binary stream to MSDOS under some
    * compilers you might get a write error (BCC, Zortech)
    */
   if (isatty(fileno(stdin)))
      Abort("error, input from tty is not supported under MSDOS");
   setmode(fileno(stdin), O_BINARY);
   setmode(fileno(stdout), O_BINARY);
#endif

   Ctrl.fpIn = stdin;              /* input from stdin  */
   Ctrl.fpOut = stdout;            /* output to stdout  */

   if (!fSearchReplace)
   {
      nMatches = BMG_Search(&Ctrl);

      if (nMatches > 0)
         fprintf(Ctrl.fpMsg, "%s: %ld match%s found\n",
                 Ctrl.pInputFile, nMatches, (nMatches == 1) ? "" : "es");
   }
   else
   {
      nMatches = BMG_SearchReplace(&Ctrl, ReplaceBuf, nItemsReplace);

      if (nMatches == -1)   /* error in writing file */
         Abort("error in writing file to stdout\n");
      else
         if (nMatches > 1)
         {
            fflush(Ctrl.fpOut);
            fprintf(Ctrl.fpMsg, "%s: %ld occurrence%s changed\n",
                    Ctrl.pInputFile, nMatches, (nMatches > 1) ? "s" : "");
         }
   }
}

/* Input  : filename
 * Returns: 1 - file is OK, i.e we get a stat on it and it's a regular file
 *          0 - file is NOK
 *
 * Eventual errors are displayed here
 */
char fCheckFile(char *pFileName)
{
   struct stat buf;

   if (stat(pFileName, &buf) != 0)
   {
      fprintf(Ctrl.fpMsg, "gsar: unable to open input file '%s'\n", pFileName);
      return 0;
   }


   /* If it's not a regular file */
   if (!S_ISREG(buf.st_mode))
   {
      /* If it's NOT a dircetory flag warning */
      if (!S_ISDIR(buf.st_mode))
      {
         fprintf(Ctrl.fpMsg, "gsar: warning, not a regular file '%s'\n", pFileName);
      }
      return 0;
   }

   return 1;
}

/* Performs a BMG search on one or multiple files. The files
 * to process are found in pFileList. Files that cannot be
 * opened are just ignored.
 */
void FileSearch(void)
{
   long  nMatches;

   while (*pFileList != NULL)
   {
      Ctrl.pInputFile = *pFileList++;

      if (!fCheckFile(Ctrl.pInputFile))
         continue;

      if ((Ctrl.fpIn = fopen(Ctrl.pInputFile, "rb")) == NULL)
      {
         fprintf(Ctrl.fpMsg, "gsar: unable to open input file '%s'\n", Ctrl.pInputFile);
         continue;
      }

      Ctrl.fpMsg = stdout;

#if defined(MSDOS) && (!defined(__ZTC__)) || (defined(__ZTC__) && !defined(__SMALL__))
      /* Don't use setvbuf if we're compiling under Zortech small model
       */
      if (setvbuf(Ctrl.fpIn, NULL, _IOFBF, SETVBUF_SIZ) != 0)
         fprintf(Ctrl.fpMsg, "warning, unable to set up buffering for input file\n");
#endif

      nMatches = BMG_Search(&Ctrl);
      fclose(Ctrl.fpIn);

      if (nMatches > 0)
         fprintf(Ctrl.fpMsg, "%s: %ld match%s found\n",
                 Ctrl.pInputFile, nMatches, (nMatches == 1) ? "" : "es");
   }
}

/* Performs a search and replace on a specific outputfile
 */
void OneSearchReplace(void)
{
   long  nMatches = 0;
   struct stat StatBuf;    /* struct so we can access file information */

   Ctrl.fpMsg = stdout;    /* redirect messages */
   Ctrl.pInputFile = NULL; /* initially we haven't found a file */

   /* find the input and output file
    */
   while (*pFileList != NULL)
   {
      if (Ctrl.pInputFile == NULL)
         Ctrl.pInputFile = *pFileList;
      else
         if (pOutFile == NULL)
            pOutFile = *pFileList;

      pFileList++;
   }

   if (!fCheckFile(Ctrl.pInputFile))
   {
      /* Message has already been given */
      exit(EXIT_FAILURE);
   }
   else
      Ctrl.fpIn = fopen(Ctrl.pInputFile, "rb");

   if (Ctrl.fpIn == NULL)
      Abort("error, unable to open input file '%s'", Ctrl.pInputFile);

   if ((stat(pOutFile, &StatBuf)) == 0 && !fForce)   /* stat got the info ie. file exists */
      Abort("error, output file '%s' already exists. Use the 'f' option to force overwrite", pOutFile);

   if ((Ctrl.fpOut = fopen(pOutFile, "wb")) == NULL)
      Abort("error, unable to open output file '%s' ", pOutFile);

#if defined(MSDOS) && (!defined(__ZTC__)) || (defined(__ZTC__) && !defined(__SMALL__))
   /* Don't use setvbuf if we're compiling under Zortech small model
    */
   if (setvbuf(Ctrl.fpIn, NULL, _IOFBF, SETVBUF_SIZ) != 0)
      fprintf(Ctrl.fpMsg, "warning, unable to set up buffering for input file\n");
   if (setvbuf(Ctrl.fpOut, NULL, _IOFBF, SETVBUF_SIZ) != 0)
      fprintf(Ctrl.fpMsg, "warning, unable to set up buffering for output file\n");
#endif

   nMatches = BMG_SearchReplace(&Ctrl, ReplaceBuf, nItemsReplace);

   fclose(Ctrl.fpIn);
   fclose(Ctrl.fpOut);

   fCriticalPart = 1;    /* ignore interrupts here */

   if (nMatches == -1)   /* error in writing file */
   {
      fprintf(Ctrl.fpMsg, "gsar: error in writing file '%s' - cleaning up\n", pOutFile);
      if (remove(pOutFile) != 0)
         Abort("error, unable to remove output file '%s'", pOutFile);
      exit(EXIT_FAILURE);  /* exit & tell operating system that we've failed */
   }

   if (nMatches == 0)
   {
      if (remove(pOutFile) != 0)
         Abort("error, unable to remove output file '%s'", pOutFile);
   }
   else
      fprintf(Ctrl.fpMsg, "%s: %ld occurrence%s changed\n",
               Ctrl.pInputFile, nMatches, (nMatches > 1) ? "s" : "");

   /* Make sure the Ctrl-C handler does not delete a completly
    * processed output file
    */
   pOutFile = NULL;

   fCriticalPart = 0;   /* enable interrupts */
}


/* Performs a BMG search and replace on one or multiple files. The files
 * to process are found in pFileList. Files that cannot be opened
 * are ignored.
 */
void SearchReplace(void)
{
   long  nMatches;
#ifdef __UNIX__
   struct stat stat_buf;
   /*
     - some compilers mess this up...
     extern int chown(char *, uid_t, gid_t);
     extern int chmod(char *, mode_t);
   */
#endif

   Ctrl.fpMsg = stdout;    /* redirect messages */

   while (*pFileList != NULL)
   {
      Ctrl.pInputFile = *pFileList++;

      if (!fCheckFile(Ctrl.pInputFile))
         continue;

      if ((Ctrl.fpIn = fopen(Ctrl.pInputFile, "rb")) == NULL)
      {
         fprintf(Ctrl.fpMsg, "gsar: unable to open input file '%s'\n", Ctrl.pInputFile);
         continue;
      }

      /* generate a temporary file name with prefix 'gsr_'
         */
      pOutFile = ExtractPathFromFSpec(Ctrl.pInputFile);
      if ((pOutFile = TmpName(pOutFile, "gsr_")) == NULL)
         Abort("error, unable to create a temporary file name");

      if ((Ctrl.fpOut = fopen(pOutFile, "wb")) == NULL)
         Abort("error, unable to open output file '%s' ", pOutFile);

#ifdef __UNIX__
      /* In Unix, we try to preserve the mode and id's of the file : */
      if (stat(Ctrl.pInputFile, &stat_buf) != -1)
      {
         chown(pOutFile, stat_buf.st_uid, stat_buf.st_gid);
         chmod(pOutFile, stat_buf.st_mode);
      }
      /* We just ignore errors here ... (hpv) */
#endif

#if defined(MSDOS) && (!defined(__ZTC__)) || (defined(__ZTC__) && !defined(__SMALL__))
      /* Don't use setvbuf if we're compiling under Zortech small model
         */
      if (setvbuf(Ctrl.fpIn, NULL, _IOFBF, SETVBUF_SIZ) != 0)
         fprintf(Ctrl.fpMsg, "warning, unable to set up buffering for input file\n");
      if (setvbuf(Ctrl.fpOut, NULL, _IOFBF, SETVBUF_SIZ) != 0)
         fprintf(Ctrl.fpMsg, "warning, unable to set up buffering for output file\n");
#endif

      nMatches = BMG_SearchReplace(&Ctrl, ReplaceBuf, nItemsReplace);

      fclose(Ctrl.fpIn);
      fclose(Ctrl.fpOut);

      fCriticalPart = 1;      /* ignore interrupts here */

      if (nMatches == -1)   /* error in writing file */
      {
         fprintf(Ctrl.fpMsg, "gsar: error in writing file '%s' - cleaning up\n", pOutFile);
         if (remove(pOutFile) != 0)
            Abort("error, unable to remove output file '%s'", pOutFile);
         exit(EXIT_FAILURE);
      }

      if (nMatches == 0)
      {
         if (remove(pOutFile) != 0)
            Abort("error, unable to remove output file '%s'", pOutFile);
      }
      else
      {
         /* if we can't remove the input file delete the tmp output file
            */
         if (remove(Ctrl.pInputFile) != 0)
         {
            fprintf(Ctrl.fpMsg, "gsar: error, unable to remove input file '%s' before rename (read-only ?)", Ctrl.pInputFile);
            if (remove(pOutFile) != 0)
               Abort("error, unable to remove output file '%s'", pOutFile);
            exit(EXIT_FAILURE);
         }

         if (rename(pOutFile, Ctrl.pInputFile) != 0)
            Abort("error, unable to rename file '%s' to '%s'", pOutFile, Ctrl.pInputFile);

         fprintf(Ctrl.fpMsg, "%s: %ld occurrence%s changed\n",
                  Ctrl.pInputFile, nMatches, (nMatches > 1) ? "s" : "");
      }
      fCriticalPart = 0;
   }
}

int main(int argc, char *argv[])
{
   int i;            /* number of files on command line */

#if defined(__ZTC__)
   /* Automatic response file expansion */
   response_expand(&argc, &argv);
#endif

   Ctrl.fFileSpec = 1;
   Ctrl.fpMsg = stdout;

   if (signal(SIGINT, CtrlBreak) == SIG_ERR)
      Abort("internal error, unable to set SIGINT");

   /* parse command line arguments & set variables
    * i is number of files found
    */
   i = GetArgs(argc, argv);

   /* perform different health & sanity checks
    */

   if (nItemsSearch == 0)
      Abort("command error, no search string specified");

   /* display search and replace buffers if any
    */
   if (fBuffers)
   {
      fputc('\n', Ctrl.fpMsg);

      fprintf(Ctrl.fpMsg, "Search buffer  (ASCII) : ");
      DumpBuffer(SearchBuf, nItemsSearch, 0);
      fprintf(Ctrl.fpMsg, "Length = %3d   (Hex)   : ", nItemsSearch);
      DumpBuffer(SearchBuf, nItemsSearch, 1);
      fprintf(Ctrl.fpMsg, "\n");

      if (fSearchReplace)
      {
         if (nItemsReplace > 0)
         {
            fprintf(Ctrl.fpMsg, "Replace buffer (ASCII) : ");
            DumpBuffer(ReplaceBuf, nItemsReplace, 0);
            fprintf(Ctrl.fpMsg, "Length = %3d   (Hex)   : ", nItemsReplace);
            DumpBuffer(ReplaceBuf, nItemsReplace, 1);
         }
         else
            fprintf(Ctrl.fpMsg, "Note: Replace buffer is empty, search string will be removed from file!\n");
      }

      return EXIT_SUCCESS;
   }

   if (i == 0 && !fFilter)
      Abort("command error, no input file name specified");

   if (i != 2 && fForce && !fOverWrite)
      Abort("command error, two file names are required to use the 'f' option");

   /* set up the search pattern once and for all
    */
   BMG_Setup(SearchBuf, (int) nItemsSearch, fFolded);

   if (fFilter)
   {
      if (fOverWrite || fForce)
         Abort("command error, the 'o' or 'f' option is meaningless in 'filter' mode");

      StreamSearchReplace();
      return EXIT_SUCCESS;
   }

   /* just perform a file search
    */
   if (!fSearchReplace)
   {
      if (fOverWrite || fForce)
         Abort("command error, the 'o' or 'f' option is meaningless in 'search' mode");

      FileSearch();
      return EXIT_SUCCESS;
   }

   /* Do a search and replace with specific output file
    */
   if (i == 2 && !fOverWrite && fSearchReplace)
   {
      OneSearchReplace();
      return EXIT_SUCCESS;
   }

   if (!fOverWrite && fSearchReplace)
      Abort("command error, multiple search & replace requires the 'o' option");

   if (fOverWrite && fSearchReplace)
   {
      if (fForce)
         Abort("command error, the 'f' option is meaningless in multiple search and replace");
      SearchReplace();
   }
   return EXIT_SUCCESS;
}

