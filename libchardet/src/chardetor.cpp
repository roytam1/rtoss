#include <stdio.h>
#ifndef _WIN32
# include <unistd.h>
#else
# ifdef _GUI
#  include <windows.h>
# endif
#endif
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "chardet.h"
#define CHARSET_MAX 1024

int main(int argc, char **argv)
{
    int i = 0;
	FILE* fd;
    long lSize;
    char *s = NULL, charset[CHARSET_MAX], sMsg[CHARSET_MAX];
    chardet_t pdet = NULL;

    if(argc < 2)
    {
#ifdef _GUI
		sprintf(sMsg,"Usage: %s file", argv[0]);
		::MessageBoxA(NULL,sMsg,argv[0],MB_OK);
#else
        fprintf(stderr, "Usage:%s file ...\n", argv[0]);
#endif
        _exit(-1);
    }

    if(chardet_create(&pdet) == 0)
    {
#ifdef _GUI
		i = 1;
#else
        for(i = 1; i < argc; i++)
#endif
        {
            if((fd = fopen(argv[i], "rb")) > 0)
            {
				fseek(fd, 0, SEEK_END);
                if((lSize = ftell(fd))
                        && (s = (char *)calloc(1, lSize + 1)))
                {
					rewind(fd);
                    if(fread(s, 1, lSize, fd) > 0 
                        && chardet_handle_data(pdet, s, lSize) == 0)
                    {
                        chardet_data_end(pdet);
                        chardet_get_charset(pdet, charset, CHARSET_MAX);
#ifdef _GUI
						sprintf(sMsg,"charset of %s: %s", argv[i],charset);
						::MessageBoxA(NULL,sMsg,"chardet",MB_OK);
#else
                        fprintf(stdout, "charset of %s:%s\n", argv[i], charset);
#endif
                        chardet_reset(pdet);
                    }
                    free(s);
                    s = NULL;
                }
                fclose(fd);
            }
        }
        chardet_destroy(pdet);
    }
}
