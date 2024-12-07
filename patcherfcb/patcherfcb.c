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

void main(int argc, char *argv[]) {
	FILE *fpin, *fppat;
	char line[256];
	int i;
	int offset, from, to, was;
	int err = 0;
	int ap = 1;

	if (argc == 3) {
		if (fpin = fopen(argv[1], "rb+")) {
			if (fppat = fopen(argv[2], "rb")) {
				while (fgets(line, 255, fppat)) {
					for (i = 0; (i<strlen(line)) && (line[i] == ' '); i++)
						;
					if((line[i] != ';') && (strncmp(&line[i], "Comparing files", 15))) {
						if(!strncmp(&line[i + 8], ":", 1) && !strncmp(&line[i + 9], " ", 1) && !strncmp(&line[i + 12], " ", 1)) {
							offset = strtol(&line[i], NULL, 16);
							from = strtol(&line[i + 10], NULL, 16);
							to = strtol(&line[i + 13], NULL, 16);
							if (!fseek(fpin, offset, SEEK_SET)) {
								was = fgetc(fpin);
								if (was != EOF) {
									fseek(fpin, offset, SEEK_SET);
									fputc(to, fpin);
									if (was != to)
										ap = 0;
									if (was != from) {
										printf("Offset $%04.4X was $%02.2X, should have been $%02.2X\n",
											offset, was, from);
										err = 1;
									}
								}
								else {
									printf("Offset $%04.4X EOF\n", offset);
									err = 2;
								}
							}
							else {
								printf("Offset $%04.4X seek error\n", offset);
								err = 3;
							}
						}
						else {
							printf("invalid line skipped '%s'\n", line);
						}
					}
				}
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
			}
			else
				printf("Can't open patch file\n");
			fclose(fpin);
		}
		else
			printf("Can't open file\n");
	}
	else
		printf("Usage: `%s <file> <patch file>'\n", argv[0]);
}