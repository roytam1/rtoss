/* patcherfcb.c - patches files using output from DOS fc/b command */
/* Version 1.0 */

/* usage: patcherfcb <file> <patch file> */

/* the patchfile format is the same as the output from the DOS fc/b command: */

/* Comparing files file1.bin and file2.bin */
/* 00000000: 01 02 */
/* 00000001: 01 02 */

/* with the addition of comments, which have ; in first position of line */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Help flag */
int help = 0;

/* Error counter */
int err = 0;

int apply_patch(FILE *fpin, FILE *fppat, int reverse, int dryrun, int* ap) {
	char line[256];
	int myerr = 0;
	unsigned int i, l;
	unsigned char from, to, was, tmp;
	unsigned long offset;

	while (fgets(line, 255, fppat)) {
		l = strlen(line);
		for (i = 0; (i<l) && (line[i] == ' '); i++)
			;
		if((l >= 15) && (line[i] != ';') && (strncmp(&line[i], "Comparing files", 15))) {
			if(!strncmp(&line[i + 8], ":", 1) && !strncmp(&line[i + 9], " ", 1) && !strncmp(&line[i + 12], " ", 1)) {
				offset = strtoul(&line[i], NULL, 16);
				from = strtol(&line[i + 10], NULL, 16);
				to = strtol(&line[i + 13], NULL, 16);
				if(reverse) {
					tmp = from;
					from = to;
					to = tmp;
				}
				if (!fseek(fpin, offset, SEEK_SET)) {
					was = fgetc(fpin);
					if (was != EOF) {
						fseek(fpin, offset, SEEK_SET);
						if(!dryrun) {
							fputc(to, fpin);
						}
						if (was != to)
							*ap = 0;
						if (was != from) {
							printf("Offset $%04.4X was $%02.2X, should have been $%02.2X\n",
								offset, was, from);
							myerr = 1;
						}
					}
					else {
						printf("Offset $%04.4X EOF\n", offset);
						myerr = 2;
					}
				}
				else {
					printf("Offset $%04.4X seek error\n", offset);
					myerr = 3;
				}
			}
			else {
				printf("invalid line skipped '%s'\n", line);
			}
		}
	}
	return myerr;
}

/* `path` will be modified */
char* mybasename(char* path)
{
	char *uppercase;
	char *mybase = strrchr(path, '\\');
	char *ext;
	mybase = mybase ? mybase+1 : path;
	uppercase = strupr(mybase);
	ext = strstr(uppercase, ".EXE");
	if(ext)
		*ext = 0;
	ext = strstr(uppercase, ".COM");
	if(ext)
		*ext = 0;
	return uppercase;
}

void usage(char* myname)
{
	fprintf(stderr, "%s\n", myname);
	fputs("Patches files using output from DOS fc/b command.\n\n",
		  stderr);

	if (err)
	{
		fputs("Invalid parameter\n\n", stderr);
	}

	fprintf(stderr, "    %s [options] [drive:1][path1]file [drive2:][path2]patchfile\n",
	        myname);

	fprintf(stderr, "    %s [options] [drive:1][path1]file - < [drive2:][path2]patchfile\n",
	        myname);

	fprintf(stderr, "    other-command | %s [options] [drive:][path]file -\n", myname);
	fputs("\n	Options:\n", stderr);
	fputs("    /R   Reverse patch\n", stderr);
	fputs("    /D   Dry-run (i.e. not actually writing to target file)\n", stderr);
	fputs("    /?   Help\n", stderr);
	fputs("\n    Notice: \"-\" is indicator for stdin\n", stderr);
}

int main(int argc, char **argv) {
	FILE *fpin = 0, *fppat = 0;
	char *infname = 0, *patfname = 0;
	char *cp;
	char *myname = mybasename(argv[0]);

	int argnow = 0;
	int ap = 1;
	int rflag = 0;
	int dflag = 0;

	while (--argc) {
		++argnow;
		if (*(cp = *++argv) == '/') {
			switch (cp[1]) {
				case 'R':
				case 'r':
					rflag = 1;
					break;

				case 'D':
				case 'd':
					dflag = 1;
					break;

				case '?':
				case 'h':
				case 'H':
					help = 1;
					break;

				default:
					err++;
			}
		} else {
			/* try openning it, if it can be fopen()ed, record its name */
			FILE* fptest;
			if (!infname) {
				if((fptest = fopen(*argv, "rb+")) != NULL) {
					infname = *argv;
					fpin = fptest;
				} else {
					fprintf(stderr, "%s: input file not found\n", myname);
					exit(3);
				}
			} else if (!patfname) { /* patch file */
				if((fptest = fopen(*argv, "rb")) != NULL) {
					patfname = *argv;
					fppat = fptest;
				} else {
					if(!strcmp(*argv, "-")) {
					patfname = *argv;
					fppat = (FILE*)stdin;
					} else {
						fprintf(stderr, "%s: patch file not found\n", myname);
						exit(3);
					}
				}
			}
	   }
	}
	if (!help && (!infname || !patfname))
		err++;

	if (err || help)
	{
		usage(myname);
		exit(1);
	}

	err = apply_patch(fpin, fppat, rflag, dflag, &ap);
	if (!strcmp(patfname, "-"))
		fclose(fppat);

	switch (err) {
		case 0:
			printf("File patched OK\n");
			break;
		case 1:
			if (ap == 0)
				printf("Incorrect file for this patch!\n");
			else
				printf("File already patched\n");
			break;
		case 2:
			printf("End of file error\n");
			break;
		case 3:
			printf("Seek error\n");
			break;
	}

	fclose(fpin);

	return err;
}
