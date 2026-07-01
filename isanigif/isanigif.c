/*
 * gifdump.c
 *
 * Copyright (C) 2003-2009 Cosmin Truta.
 * This software is distributed under the same licensing and warranty terms
 * as gifread.c.
 */


#include <stdio.h>
#include "gifread.h"


static int exitCode = 0;
static int frameCount = 0;

void GIFDump(const char *filename)
{
    FILE *stream;
    struct GIFScreen screen;
    struct GIFImage image;
    struct GIFExtension ext;
    struct GIFGraphicCtlExt graphicExt;

    stream = fopen(filename, "rb");
    if (stream == NULL)
    {
        fprintf(stderr, "Error: Can't open %s\n", filename);
        exitCode = 1;
        return;
    }


    GIFReadScreen(&screen, stream);
    GIFInitImage(&image, &screen, NULL);
    GIFInitExtension(&ext, &screen, NULL, 0);

    for ( ; ; )
    {
        switch (GIFReadNextBlock(&image, &ext, stream))
        {
        case GIF_TERMINATOR:  /* ';' */
            fclose(stream);
            return;
        case GIF_IMAGE:       /* ',' */
           frameCount++;
           break;
        case GIF_EXTENSION:   /* '!' */
           break;
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: isanigif [-s] <file.gif>\n");
        return 1;
    }

    if (strcmp(argv[1], "-s") == 0) {
        GIFDump(argv[2]);
	    printf("%d Frame(s).\n", frameCount);
    }
    else {
        GIFDump(argv[1]);
    }

	if (!exitCode)
		exitCode = frameCount > 1 ? 1 : 0;

    return exitCode;
}
