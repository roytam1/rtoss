/*

TiMidity -- Experimental MIDI to WAVE converter
Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

common.c

*/

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32_WCE
#include <string.h>
#endif

#ifndef _WIN32_WCE
#include <errno.h>
#endif

#include "config.h"
#include "common.h"
#include "output.h"

/* I guess "rb" should be right for any libc */
#define OPEN_MODE "rb"

char *program_name, current_filename[1024];

#ifdef DEFAULT_PATH
/* The paths in this list will be tried whenever we're reading a file */
static PathList defaultpathlist={DEFAULT_PATH,0};
static PathList *pathlist=&defaultpathlist; /* This is a linked list */
#else
static PathList *pathlist=0;
#endif

/* Try to open a file for reading. If the filename ends in one of the
defined compressor extensions, pipe the file through the decompressor */
static FILE *try_to_open(char *name, int decompress, int noise_mode)
{
    FILE *fp;
    
    fp=fopen(name, OPEN_MODE); /* First just check that the file exists */
    
    if (!fp)
    return 0;
    
#ifdef DECOMPRESSOR_LIST
    if (decompress)
    {
        int l,el;
        static char *decompressor_list[] = DECOMPRESSOR_LIST, **dec;
        char tmp[1024], tmp2[1024], *cp, *cp2;
        /* Check if it's a compressed file */
        l=strlen(name);
        for (dec=decompressor_list; *dec; dec+=2)
        {
            el=strlen(*dec);
            if ((el>=l) || (strcmp(name+l-el, *dec)))
            continue;
            
            /* Yes. Close the file, open a pipe instead. */
            fclose(fp);
            
            /* Quote some special characters in the file name */
            cp=name;
            cp2=tmp2;
            while (*cp)
            {
                switch(*cp)
                {
                case '\'':
                case '\\':
                case ' ':
                case '`':
                case '!':
                case '"':
                case '&':
                case ';':
                    *cp2++='\\';
                }
                *cp2++=*cp++;
            }
            *cp2=0;
            
            sprintf(tmp, *(dec+1), tmp2);
            fp=popen(tmp, "r");
            break;
        }
    }
#endif
    
    return fp;
}

/* This is meant to find and open files for reading, possibly piping
them through a decompressor. */
FILE *open_file(char *name, int decompress, int noise_mode)
{
    FILE *fp;
    PathList *plp=pathlist;
    int l;
    
    if (!name || !(*name))
    {
        return 0;
    }
    
    /* First try the given name */
    
    strncpy(current_filename, name, 1023);
    current_filename[1023]='\0';
    
    if ((fp=try_to_open(current_filename, decompress, noise_mode)))
		return fp;

    if (name[0] != PATH_SEP) {
		while (plp)  /* Try along the path then */
		{
			*current_filename=0;
			l=strlen(plp->path);
			if(l)
			{
				strcpy(current_filename, plp->path);
				if(current_filename[l-1]!=PATH_SEP)
				strcat(current_filename, PATH_STRING);
			}
			strcat(current_filename, name);
			if ((fp=try_to_open(current_filename, decompress, noise_mode)))
				return fp;
			plp=plp->next;
		}
	}
    
    /* Nothing could be opened. */
    
    *current_filename=0;
    
    return 0;
}

/* This closes files opened with open_file */
void close_file(FILE *fp)
{
#ifdef DECOMPRESSOR_LIST
    if (pclose(fp)) /* Any better ideas? */
#endif
    fclose(fp);
}

/* This is meant for skipping a few bytes in a file or fifo. */
void skip(FILE *fp, size_t len)
{
    size_t c;
    char tmp[1024];
    while (len>0)
    {
        c=len;
        if (c>1024) c=1024;
        len-=c;
        fread(tmp, 1, c, fp);
    }
}

/* This'll allocate memory or die. */
void *safe_malloc(size_t count)
{
    void *p;
	if ((p=malloc(count)))
		return p;
    
	return NULL;
}

/* This adds a directory to the path list */
void add_to_pathlist(char *s)
{
    PathList *plp=safe_malloc(sizeof(PathList));
    strcpy((plp->path=safe_malloc(strlen(s)+1)),s);
    plp->next=pathlist;
    pathlist=plp;
}
