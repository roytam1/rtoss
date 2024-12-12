/*
 * PROJECT:   SORT - Adapted for ReactOS
 * LICENSE:   GPL - See COPYING in the top level directory
 * PURPOSE:   Reads line of a file and sorts them in order
 * COPYRIGHT: Copyright 1995 Jim Lynch
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXRECORDS 65536 /* maximum number of records that can be sorted */
#define MAXLEN 4095 /* maximum record length */

/* Reverse flag */
int rev;

/* Unique flag */
int uniq;

/* Help flag */
int help;

/* Sort column */
int sortcol;

/* Error counter */
int err = 0;

int cmpr(const void *a, const void *b)
{
    char *A, *B;

    A = *(char **) a;
    B = *(char **) b;

    if (sortcol > 0)
    {
        if (strlen(A) > sortcol)
        {
            A += sortcol;
        }
        else
        {
            A = "";
        }
        if (strlen(B) > sortcol)
        {
            B += sortcol;
        }
        else
        {
            B = "";
        }
    }

    if (!rev)
    {
        return strcmp(A, B);
    }
    else
    {
        return strcmp(B, A);
    }
}

void usage(void)
{
    fputs("SORT\n", stderr);
    fputs("Sorts input and writes output to a file, console or a device.\n",
          stderr);

    if (err)
    {
        fputs("Invalid parameter\n", stderr);
    }

    fputs("    SORT [options] < [drive:1][path1]file1 > [drive2:][path2]file2\n",
          stderr);

    fputs("    SORT [options] [drive:1][path1]file1 > [drive2:][path2]file2\n",
          stderr);

    fputs("    Command | SORT [options] > [drive:][path]file\n", stderr);
    fputs("    Options:\n", stderr);
    fputs("    /O <filename>   Output to <filename>\n", stderr);
    fputs("    /R   Reverse order\n", stderr);
    fputs("    /U   Unique ouput\n", stderr);
    fputs("    /+n  Start sorting with column n\n", stderr);
    fputs("    /?   Help\n", stderr);
}

int main(int argc, char **argv)
{
    char temp[MAXLEN + 1];
    char **list;

    int argnow = 0;
    char *infname = 0, *outfname = 0;

    FILE *fpin, *fpout;

    /* Option character pointer */
    char *cp;
    int i, nr;

    sortcol = 0;
    rev = 0;
    uniq = 0;

    fpin = (FILE*)stdin;
    fpout = (FILE*)stdout;

    while (--argc)
    {
        ++argnow;
        if (*(cp = *++argv) == '/')
        {
            switch (cp[1])
            {
                case 'R':
                case 'r':
                    rev = 1;
                    break;

                case 'O':
                case 'o':
                    // argv[argnow] already points to next argument
                    if(*argv[argnow]) {
                        outfname = argv[argnow];
                        ++argv; --argc; // properly moving pointers
                    }
                    break;

                case 'U':
                case 'u':
                    uniq = 1;
                    break;

                case '?':
                case 'h':
                case 'H':
                    help = 1;
                    break;

                case '+':
                    sortcol = atoi(cp + 1);
                    if (sortcol)
                    {
                        sortcol--;
                    }
                    break;

                default:
                    err++;
            }
        } else {
            // try openning it, if it can be fopen()ed, record its name
            FILE* fptest;
            if((fptest = fopen(*argv, "rb")) != NULL) {
                infname = *argv;
                fpin = fptest;
            } else {
                fputs("SORT: input file not found\n", stderr);
                exit(3);
            }
        }
    }

    if (err || help)
    {
        usage();
        exit(1);
    }

    if (outfname)
    {
        if (infname) {
            if (!strcmp(outfname, infname)) {
                fputs("SORT: input file and output file can not be the same\n", stderr);
                exit(3);
            }
        }

        if ((fpout = fopen(outfname, "wb")) == NULL) {
            fputs("SORT: can not open output file\n", stderr);
            exit(3);
        }
    }

    list = (char **) malloc(MAXRECORDS * sizeof(char *));
    if (list == NULL)
    {
        fputs("SORT: Insufficient memory\n", stderr);
        exit(3);
    }

    for (nr = 0; nr < MAXRECORDS; nr++)
    {
        if (fgets(temp, MAXLEN, fpin) == NULL)
        {
            break;
        }

        if(strlen(temp))
        {
            temp[strlen(temp) - 1] = '\0';
        }

        list[nr] = (char *) malloc(strlen(temp) + 1);
        if (list[nr] == NULL)
        {
            fputs("SORT: Insufficient memory\n", stderr);

            /* Cleanup memory */
            for (i = 0; i < nr; i++)
            {
                free(list[i]);
            }
            free(list);
            exit(3);
        }

        strcpy(list[nr], temp);
    }

    if (nr == MAXRECORDS)
    {
        fputs("SORT: number of records exceeds maximum\n", stderr);

        /* Cleanup memory */
        for (i = 0; i < nr; i++)
        {
            free(list[i]);
        }
        free(list);

        /* Bail out */
        exit(4);
    }

    qsort((void *)list, nr, sizeof(char *), cmpr);

    for (i = 0; i < nr; i++)
    {
        int print = 1;
        if(uniq && i > 0 && !strcmp(list[i-1],list[i]))
            print = 0;

        if(print) {
            fputs(list[i], fpout);
            fputs("\n", fpout);
        }
    }

    /* Cleanup memory */
    for (i = 0; i < nr; i++)
    {
        free(list[i]);
    }
    free(list);
    return 0;
}
/* EOF */
