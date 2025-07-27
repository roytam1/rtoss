/* gsarbmg.c 
 *
 * Subroutines for fast string searching; no regular expressions
 *
 * Adapted from:
 *
 * Boyer/Moore/Gosper-assisted 'egrep' search, with delta0 table as in
 * original paper (CACM, October, 1977).  No delta1 or delta2.  According to
 * experiment (Horspool, Soft. Prac. Exp., 1982), delta2 is of minimal
 * practical value.  However, to improve for worst case input, integrating
 * the improved Galil strategies (Apostolico/Giancarlo, Siam. J. Comput.,
 * February 1986) deserves consideration.
 *
 * James A. Woods
 * NASA Ames Research Center
 *
 * 29 April 1986 Jeff Mogul Stanford
 * Adapted as a set of subroutines for use by a program. No
 * regular-expression support.
 *
 * 12 February 1992 Tormod Tjaberg
 * Used parts of the original routines to implement extremely fast
 * file search & replace mechanisms on ASCII & non ASCII files taking
 * care not to 'chop' up the search pattern.
 *
 * Note:
 * If a file consists of the following bytes: 'abrabra' a search for
 * 'abra' will yield two matches. However if we are to replace 'abra'
 * with 'foobar' only one occurrence will be changed and the output
 * file will contain 'foobarbra'.
 *
 * Copyright (C) 1992-2008 Tormod Tjaberg
 * This is free software, distributed under the terms of the
 * GNU General Public License. For details see the file COPYING
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include "comp_dep.h"
#include "gsar.h"

#undef  BUFSIZ  
#define BUFSIZ  4096
#define LARGE   32767      /* overshoot, LARGE > (BUFSIZ + PAT_BUFSIZ) */

/* Variables needed to perform the BMG search. 
 */
int            BMG_Patlen;                        /* length of pattern */
unsigned char  BMG_Pattern[PAT_BUFSIZ];           /* actual pattern */
int            BMG_Delta0[256];                   /* ascii only */
unsigned char  BMG_Buffer[BUFSIZ + PAT_BUFSIZ];   /* search buffer */
unsigned char  BMG_Cmap[256];

/* Input  : pCtrl - pointer to structure containg output and ctrl info
 *          FileOfs - actual offset in file
 *          BufOfs - match offset in search buffer
 *          pStart - pointer to start of the search buffer
 *          pEnd - pointer to end of the search buffer
 *
 * Returns: nothing
 *
 * Displays buffer information (filename, offset, context) according
 * to the flags set in the structure. i.e. be a bit verbose.
 */
void Verbose(OUTPUT_CTRL *pCtrl, unsigned long FileOfs, int BufOfs,
              unsigned char *pStart, unsigned char *pEnd)
{
   unsigned char *pSC;        /* start of context */
   unsigned char *pEC;        /* end of context */
   unsigned char *pLastSC;    /* last start of context */

   unsigned long CtxOfs;      /* context offset */

   int i;                     /* loop counter */

   if (pCtrl->fFileSpec)                /* display file name */
      fprintf(pCtrl->fpMsg, "%s: ", pCtrl->pInputFile);

   if (pCtrl->fByteOffset)              /* display byte offset */
      fprintf(pCtrl->fpMsg, "0x%lx%s",
               FileOfs + BufOfs,
               (pCtrl->fTextual) ? ": " : "");

   /* Display a textual or a hexadecimal context
    */
   if (pCtrl->fTextual || pCtrl->fHex)
   {
      pSC = pStart + BufOfs - pCtrl->Context / 2 + BMG_Patlen / 2;
      if (pSC < pStart)                /* outside the buffer ? */
         pSC = pStart;

      pEC = pSC + pCtrl->Context;
      if (pEC > pEnd)                  /* outside the buffer ? */
      {
         pEC = pEnd;

         /* if we have to truncate to pEnd readjust the start
          * of the context if possible.
          */
         if (pEC - pCtrl->Context > pStart)
            pSC = pEC - pCtrl->Context;
      }

      /* display a hexadecimal context
       */
      if (pCtrl->fHex)
      {
         fputc('\n', pCtrl->fpMsg);

         CtxOfs = FileOfs + (pSC - pStart);

         while (pSC != pEC)
         {
            pLastSC = pSC;                /* remember where we started */

            fprintf(pCtrl->fpMsg, "0x%08lx: ", CtxOfs); /* hex offset */

            for (i = 0; i < 16; i++)        /* display 16 hex digits */
            {
               if (pSC != pEC)
                  fprintf(pCtrl->fpMsg, "%02x ", (unsigned char) * pSC++);
               else
                  fprintf(pCtrl->fpMsg, "   ");
            }

            pSC = pLastSC;                             /* start again */

            for (i = 0; i < 16; i++)       /* display 16 characters */
            {
               if (pSC != pEC)
               {
#ifdef MSDOS      /* MSDOS can display all characters except CTRL chars */
                  if (!iscntrl((int) * pSC))
#else             /* its __UNIX__ */
                  if (isprint((int) * pSC))
#endif
                     fputc(*pSC, pCtrl->fpMsg);
                  else
                     fputc('.', pCtrl->fpMsg);

                  pSC++;
               }
            }
            CtxOfs += 16;             /* increment context offset by 16 */
            fputc('\n', pCtrl->fpMsg);
         }

      }

      /* display textual context...
       */
      if (pCtrl->fTextual)
      {
         while (pSC != pEC)
         {
#ifdef MSDOS   /* MSDOS can display all characters except CTRL chars */
            if (!iscntrl((int) * pSC))
#else          /* its __UNIX__ */
            if (isprint((int) * pSC))
#endif
               fputc(*pSC, pCtrl->fpMsg);
            else
               fputc('.', pCtrl->fpMsg);

            pSC++;
         }
      }
   }

   if (!pCtrl->fHex)
      fputc('\n', pCtrl->fpMsg);
}


/* Input    : pCtrl - pointer to structure containg output and ctrl info
 * Returns  : number of matches found in file
 *
 * The pattern to search for must already have been set up using BMG_Setup
 *
 * Works by applying the BMG algorithm to a buffer. To ensure the pattern 
 * is not inadvertently chopped up, BMG_Patlen - 1 bytes is always moved 
 * to the start of the buffer. The next time we fill the buffer we fill it
 * with BUFSIZ - (BMG_Patlen - 1) bytes.
 */
long BMG_Search(OUTPUT_CTRL *pCtrl)
{
   register unsigned char *k;
   register unsigned char *s;
   register unsigned char *strend;

   register int j;

   int  nTrans = 0;   /* number of bytes to transfer to the start of the buffer */
   int  BufOfs;       /* buffer offset for each match */
   int  Cnt = BUFSIZ;

   long nMatches = 0;                  /* number of matches found */
   long nBytes;                        /* number of bytes read */
   unsigned long FileOfs = 0;          /* current file offset */

   for (;;)
   {
      nBytes = fread(&BMG_Buffer[nTrans], 1, (size_t) Cnt, pCtrl->fpIn);
 
      if (!nBytes)
         break;

      s = BMG_Buffer;

      strend = s + nBytes + nTrans;

      k = BMG_Buffer + BMG_Patlen - 1;

      for (;;)
      {
         while ((k += BMG_Delta0[*(unsigned char *) k]) < strend)
            ;

         if (k < (BMG_Buffer + LARGE))
            break;
         k -= LARGE;

         j = BMG_Patlen - 1;
         s = k - 1;

         while (BMG_Cmap[*s--] == BMG_Pattern[--j])
            ;
         if (j >= 0)
            k++;
         else
         {
            if (k >= strend)
               break;

            /* found submatch, k is on the last letter in the match */
            BufOfs = k - BMG_Buffer + 1 - BMG_Patlen;

            nMatches++;
            if (pCtrl->fVerbose)
               Verbose(pCtrl, FileOfs, BufOfs, BMG_Buffer, strend);

            k++;
         }
      }

      nTrans = BMG_Patlen - 1;

      memcpy(BMG_Buffer, strend - nTrans, nTrans); /* move remaining bytes to the start */
      Cnt = BUFSIZ - nTrans;
      FileOfs += Cnt;                              /* calculate file offset  */
   }

   return nMatches;
}


/* Input    : pCtrl - pointer to structure containg output and ctrl info
 *            ReplaceBuf - pointer to buffer which contains replacement
 *            nReplace - number of bytes in replace buffer
 *
 * Returns  : number of matches & replaces performed
 *            -1 if error in fwrite, disk might be full, or removed
 *
 * The pattern to search for must already have been set up using BMG_Setup
 *
 * Works by applying the BMG algorithm to a buffer. To ensure the pattern 
 * is not inadvertently chopped up we have to be a little careful. Since
 * we're doing a search and replace we can't copy BMG_Patlen - 1 bytes 
 * always. Consider the following:
 *
 * We're searching for 'aa' in the string 'aaaa'
 * This will give a result of 3 matches namely: a[0]a[1] a[1]a[2] a[2]a[3]
 *
 * But if we're searching for 'aa' and replacing with 'xx' in 'aaaa'
 * This will give a result of 2 matches and the new buffer:   'xxxx'
 * After a match of a[0]a[1] we must start the next search at a[2].
 *
 * So if we have a match at the exact end of the buffer. We must 
 * not transfer anything to the start.
 *
 * Use the following algorithm:
 *
 * Calculate the distance between the last match and the end of the buffer
 * and call this distance n.
 *
 * n = end of buffer - last match
 * if n >= Bmg_Patlen we transfer BMG_Patlen - 1 bytes to the start
 * if n < Bmg_Patlen we transfer n bytes to the start of the buffer
 */
long BMG_SearchReplace(OUTPUT_CTRL *pCtrl, char *pReplaceBuf, unsigned short nReplace)
{
   register unsigned char *k;
   register unsigned char *s;
   register unsigned char *strend;

   register int j;
   register int n;
   unsigned char *pLast;

   int  nTrans = 0;   /* number of bytes to transfer to the start of the buffer */
   int  BufOfs;       /* buffer offset for each match */
   int  Cnt = BUFSIZ;

   long nMatches = 0;                       /* number of matches found */
   long nBytes;                                /* number of bytes read */
   unsigned long FileOfs = 0;                   /* current file offset */

   for (;;)
   {
      nBytes = (unsigned long) fread(&BMG_Buffer[nTrans], sizeof(unsigned char), (size_t) Cnt, pCtrl->fpIn);

      if (!nBytes)
      {
         if (fwrite(BMG_Buffer, sizeof(unsigned char), nTrans, pCtrl->fpOut) != nTrans)
            return -1;
         break;
      }

      s = BMG_Buffer;
      pLast = s;

      strend = s + nBytes + nTrans;

      k = BMG_Buffer + BMG_Patlen - 1;

      for (;;)
      {
         while ((k += BMG_Delta0[*(unsigned char *) k]) < strend)
            ;

         if (k < (BMG_Buffer + LARGE))
            break;
         k -= LARGE;

         j = BMG_Patlen - 1;
         s = k - 1;

         while (BMG_Cmap[*s--] == BMG_Pattern[--j])
            ;
         if (j >= 0)
            k++;
         else
         {
            if (k >= strend)
               break;

            /* found submatch, k is on the last letter in the match */
            BufOfs = k - BMG_Buffer + 1 - BMG_Patlen;

            n = (BMG_Buffer + BufOfs) - pLast;
            k++;

            if (n >= 0)
            {
               nMatches++;

               if (pCtrl->fVerbose)
                  Verbose(pCtrl, FileOfs, BufOfs, BMG_Buffer, strend);

               if (fwrite(pLast, sizeof(unsigned char), n, pCtrl->fpOut) != n)
                  return -1;
            
               if (fwrite(pReplaceBuf, sizeof(unsigned char), nReplace, pCtrl->fpOut) != nReplace)
                  return -1;
               
               pLast = k;      /* set last marker to write from */
            }
         }
      }

      n = strend - pLast;

      if (n >= BMG_Patlen)
      {
         fwrite(pLast, sizeof(unsigned char), n - BMG_Patlen + 1, pCtrl->fpOut);
         nTrans = BMG_Patlen - 1;
      }
      else
      {
         nTrans = n;
      }

      memcpy(BMG_Buffer, strend - nTrans, nTrans); /* move remaining bytes to the start */

      Cnt = BUFSIZ - nTrans;
      FileOfs += Cnt;  /* calculate file offset  */
   }

   return nMatches;
}


/* Input    : pat - pointer to pattern string
 *            PatLen - actual length of the pattern
 *            fFolded - flag which determines case folding
 * Returns  : nothing
 *
 * Set up & compute Boyer-Moore delta (jump) table
 */
void BMG_Setup(char *pat, int PatLen, char fFolded)
{
   register int j;

   BMG_Patlen = PatLen;
   
   if (fFolded)
   {  /* fold case while saving pattern */
      for (j = 0; j < BMG_Patlen; j++)
         BMG_Pattern[j] = (isupper((int) pat[j])
                        ? (unsigned char) tolower((int) pat[j]) : pat[j]);
   }
   else
      memcpy(BMG_Pattern, (unsigned char *) pat, BMG_Patlen);

   for (j = 0; j < 256; j++)
   {
      BMG_Delta0[j] = BMG_Patlen;
      BMG_Cmap[j] = (unsigned char) j; /* could be done at compile time */
   }

   for (j = 0; j < BMG_Patlen - 1; j++)
      BMG_Delta0[BMG_Pattern[j]] = BMG_Patlen - j - 1;

   BMG_Delta0[BMG_Pattern[BMG_Patlen - 1]] = LARGE;

   if (fFolded)
   {
      for (j = 0; j < BMG_Patlen - 1; j++)
         if (islower((int) BMG_Pattern[j]))
            BMG_Delta0[toupper((int) BMG_Pattern[j])] = BMG_Patlen - j - 1;
      if (islower((int) BMG_Pattern[BMG_Patlen - 1]))
         BMG_Delta0[toupper((int) BMG_Pattern[BMG_Patlen - 1])] = LARGE;
      for (j = 'A'; j <= 'Z'; j++)
         BMG_Cmap[j] = (unsigned char) tolower((int) j);
   }
}
