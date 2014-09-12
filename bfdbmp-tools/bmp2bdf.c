/*
 * bmp2bdf  --	enable to modify glyphs in a bdf font
 * version: 0.2
 * date:    Wed Feb 07 00:32:31 2001
 * author:  ITOU Hiroki (itouh@lycos.ne.jp)
 */

/*
 * Copyright (c) 2001 ITOU Hiroki
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <stdio.h>  /* printf(), fopen(), fwrite() */
#include <stdlib.h> /* malloc(), EXIT_SUCCESS, strtol(), exit() */
#include <string.h> /* strncmp(), strcpy() */
#include <limits.h> /* strtol() */
#include <sys/stat.h> /* stat() */
#include <ctype.h> /* isspace() */

#define FILENAMECHARMAX 256
#define LINECHARMAX 1024
#define SIZEMAX 8
#define exiterror(msg,arg) {printf("error: " msg, arg); exitrm();}

#ifdef DEBUG
#define d_printf(msg,arg) printf(msg, arg)
#else /* not DEBUG */
#define d_printf(msg,arg)
#endif /* DEBUG */


/* global var */
struct imageinfo{
	long w;
	long h;
	int size;
	int offset; /* bfOffBits: offset to image-data array */
	int depth; /* bitdepth */
	int sp; /* width of spacing */

} img;

struct {
	int w; /* width (pixel) */
	int h; /* height */
	int offx; /* offset y (pixel) */
	int offy; /* offset y */
} font;
int numcol; /*	numberOfColumns */
char outfileBdfP[FILENAMECHARMAX];
FILE *outP;


/* func prototype */
int main(int argc, char *argv[]);
char *readBmp(char *infileBmpP);
void readwriteBdf(char *infileBdfP, char *imgP, char *outfileBdfP);
int getline(char* lineP, int max, FILE* inputP);
char *gettoken(char *strP);
void newglyphwrite(FILE *outP, int nglyph, char *imgP);
long mread(const void *varP, int size);
void exitrm(void);
void bmp8bit2img(unsigned char *bmpP, struct imageinfo bmp, char *imgP);
void bmp24bit2img(unsigned char *bmpP, struct imageinfo bmp, char *imgP);
void bmp4bit2img(unsigned char *bmpP, struct imageinfo bmp, char *imgP);

int main(int argc, char *argv[]){
	char *imgP;
	int arg;
	char infileBdfP[FILENAMECHARMAX];
	char infileBmpP[FILENAMECHARMAX];

	/* handling arguments */
	if((argc<4) || (argc>5)){
		printf("bmp2bdf version 0.2\n");
		printf("Usage: bmp2bdf [numberOfColumns] input.bdf input.bmp output.bdf\n");
		exit(EXIT_FAILURE);
	}
	if(argc == 4){
		numcol = 32;
		arg = 0;
	}else{
		numcol = atoi(argv[1]);
		arg = 1;
	}
	if((numcol<1) || (numcol>1024))
		exiterror("numberOfColumns(%d) is over the limit\n", numcol);
	strcpy(infileBdfP, argv[1+arg]);
	strcpy(infileBmpP, argv[2+arg]);
	strcpy(outfileBdfP, argv[3+arg]);

	if(strcmp(infileBdfP, outfileBdfP) == 0)
		exiterror("'%s' appears twice\n", infileBdfP);

	/* read BMP file and change to IMG file */
	imgP = readBmp(infileBmpP);

	/* read and write BDF file*/
	readwriteBdf(infileBdfP, imgP, outfileBdfP);

	return EXIT_SUCCESS;
}


char *readBmp(char *infileBmpP){
	struct stat fileinfo;
	unsigned char *bmpP;
	struct imageinfo bmp;
	int val;
	FILE *inP;
	char *imgP;
	unsigned long ul;

	/*
	 * open BMP file
	 */
	if(stat(infileBmpP, &fileinfo) != 0)
		exiterror("'%s' does not exist", infileBmpP);
	bmpP = malloc(fileinfo.st_size);
	if(bmpP == NULL)
		exiterror("cannot allocate memory in %s'\n", infileBmpP);

	inP = fopen(infileBmpP, "rb");
	if(inP == NULL)
		exiterror("cannot open '%s'\n", infileBmpP);

	val = fread(bmpP, 1, fileinfo.st_size, inP);
	if(val != fileinfo.st_size)
		exiterror("cannot read '%s'\n", infileBmpP);
	if(memcmp(bmpP, "BM", 2) != 0)
		exiterror("%s' is not a BMP file.\n", infileBmpP);
	fclose(inP);

	/*
	 * analyze BMP file
	 */
	bmp.offset = mread(bmpP+10, sizeof(long));
	ul = (unsigned long)mread(bmpP+14, sizeof(unsigned long));
	if(ul != 40)
		exiterror("'%s' is an OS/2-format BMP file\n", infileBmpP);
	bmp.w = img.w = mread(bmpP+18, sizeof(long));
	d_printf("BMP width = %ld ", bmp.w);
	bmp.h = img.h = mread(bmpP+22, sizeof(long));
	d_printf("height = %ld\n", bmp.h);
	if(bmp.h < 0)
		exiterror("bmp height(%ld) is minus-number\n", bmp.h);

	bmp.depth = (short)mread(bmpP+28, sizeof(short));
	if(bmp.depth!=4 && bmp.depth!=8 && bmp.depth!=24)
		exiterror("cannot accept %dbit BMP file\n",bmp.depth);
	ul = (unsigned long)mread(bmpP+30, sizeof(unsigned long));
	if(ul != 0L)
		exiterror("'%s' is a compressed BMP file\n", infileBmpP);
	bmp.size = fileinfo.st_size;

	/*
	 * malloc IMG
	 */
	img.size = img.w * img.h;
	imgP = malloc(img.size); /*  free(imageP) does not exist */
	if(imgP == NULL)
		exiterror("cannot allocate memory imageP %dbytes\n", img.size);

	/*
	 * BMP bitdepth
	 */
	switch(bmp.depth){
	case 4:
		bmp4bit2img(bmpP, bmp, imgP);
		break;
	case 8:
		bmp8bit2img(bmpP, bmp, imgP);
		break;
	case 24:
		bmp24bit2img(bmpP, bmp, imgP);
		break;
	default:
		exiterror("cannot accept %d bit BMP file\n",bmp.depth);
		break;
	}

	free(bmpP);
	return imgP;
}


void readwriteBdf(char *infileBdfP, char *imgP, char *outfileBdfP){
	FILE *inP;
	enum {INBITMAP, OUTBITMAP};
	struct stat fileinfo;
	char lineP[LINECHARMAX];
	char strP[LINECHARMAX]; /* for not-breaking lineP with '\0' */
	int st; /* status */
	int len, val;
	int nglyph; /* number of glyphs that appeared */
	char *tokenP; /* used for splitting tokens */

	/* file open */
	if(stat(infileBdfP, &fileinfo) != 0)
		exiterror("'%s' does not exist\n", infileBdfP);
	inP = fopen(infileBdfP, "r");
	if(inP == NULL)
		exiterror("cannot open '%s'\n", infileBdfP);

	if(stat(outfileBdfP, &fileinfo) == 0){
		printf("error: '%s' already exists\n", outfileBdfP);
		exit(EXIT_FAILURE);
	}
	outP = fopen(outfileBdfP, "wb");
	if(outP == NULL)
		exiterror("cannot write '%s'\n", outfileBdfP);

	/* file check */
	memset(lineP, 0x00, LINECHARMAX);
	len = getline(lineP, LINECHARMAX, inP);
	d_printf("len=%d\n", len);
	if((len == 0) || (strncmp(lineP, "STARTFONT", 9) != 0))
		exiterror("'%s' is not a BDF file.\n", infileBdfP);
	val = fwrite(lineP, 1, strlen(lineP), outP);
	if(val != (int)strlen(lineP))
		exiterror("cannot write '%s'\n", outfileBdfP);


	/* read bdf file */
	nglyph = 0;
	st = OUTBITMAP;
	for(;;){
		memset(lineP, 0x00, LINECHARMAX);
		len = getline(lineP, LINECHARMAX, inP);
		if(len == 0)
			break;

		if(st == INBITMAP){
			if(strncmp(lineP, "ENDCHAR", 7)==0){
				st = OUTBITMAP;
				newglyphwrite(outP, nglyph, imgP);
				nglyph++;
			}
		}else{ /*  stat != INBITMAP */
			switch(lineP[0]){
			case 'B':
				if(strncmp(lineP, "BBX", 3)==0)
					; /*  do nothing */
				else if(strncmp(lineP, "BITMAP", 6)==0)
					st = INBITMAP;
				break; /* lines of BBX, BITMAP is written later */
			case 'F':
				if(strncmp(lineP, "FONTBOUNDINGBOX ", 16)==0){
					strcpy(strP, lineP);
					tokenP = gettoken(strP);
					tokenP = gettoken(tokenP + (int)strlen(tokenP) + 1);
					font.w = atoi(tokenP);
					tokenP = gettoken(tokenP + (int)strlen(tokenP) + 1);
					font.h = atoi(tokenP);
					tokenP = gettoken(tokenP + (int)strlen(tokenP) + 1);
					font.offx = atoi(tokenP);
					tokenP = gettoken(tokenP + (int)strlen(tokenP) + 1);
					font.offy = atoi(tokenP);

					if( (img.w-numcol*font.w) % (numcol+1) != 0){
						d_printf("numcol = %d ", numcol);
						d_printf("numcol*font.h = %d\n", numcol*font.h);

						exiterror("NumberOfColumns(%d) and the BMP file do not match.\n", numcol);
					}
					img.sp = (img.w - numcol*font.w) / (numcol+1);
					d_printf("font.w=%d ", font.w);
					d_printf("font.h=%d ", font.h);
					d_printf("font.offx=%d ", font.offx);
					d_printf("font.offy=%d\n", font.offy);
				}
				/*  go next */
			default:
				val = fwrite(lineP, 1, strlen(lineP), outP);
				if(val != (int)strlen(lineP))
					exiterror("cannot write '%s'\n", outfileBdfP);
			} /* switch */
		} /* if(stat=.. */
	} /* for(;;) */

	fclose(inP);
	fclose(outP);
	d_printf("total glyphs = %d\n", nglyph);
}


/*
 * read oneline from textfile
 *    fgets returns strings included '\n'
 */
int getline(char *lineP, int max, FILE* inputP){
	char *p;

	if(fgets(lineP, max, inputP) == NULL){
		return 0;
	}else{
		/* change CR+LF to LF */
		for(p=lineP ; *p!='\0'; p++){
			if(*p == '\r'){
				*p = '\n';
				*(p+1) = '\0';
			}
		}
		return strlen(lineP);
	}
}


char *gettoken(char *strP){
	enum {BEFORE, AFTER};
	char *retP = strP;
	int st; /* status */

	st = BEFORE;
	while(*strP != '\0'){
		if((st==BEFORE) && (isspace(*strP)==0)){
			/* before first char of token, not space */
			retP = strP;
			st = AFTER;
		}else if((st==AFTER) && (isspace(*strP)!=0)){
			/* after first char of token, space */
			*strP = '\0';
			return retP;
		}
		strP++;
	}

	/* This may read over a given string. dangerous */
	return retP;
}


/*
 *  read glyphimage from IMG
 *     and change to BDF hexadecimal
 */
void newglyphwrite(FILE *outP, int nglyph, char *imgP){
	char *boxP;
	int i,j,k, x,y, val;
	int widthaligned; /* width of a glyph (byte-aligned) */
	char *bitstrP; /* BITMAP strings */
	char strP[LINECHARMAX]; /* tmp buffer */
	static char *hex2binP[]= {
		"0000","0001","0010","0011","0100","0101","0110","0111",
		"1000","1001","1010","1011","1100","1101","1110","1111"
	};

	/*
	 * allocate 'box' area for a glyph image
	 */
	widthaligned = (font.w+7) / 8 * 8;
	boxP = malloc(widthaligned*font.h);
	if(boxP == NULL)
		exiterror("cannot allocate memory newglyphwrite %dbytes\n", widthaligned*font.h);
	memset(boxP, '0', widthaligned*font.h);

	/*
	 * read from IMG
	 */
	/*   one glyph height  row */
	y = (font.h+img.sp) * (nglyph/numcol) + img.sp;
	/*   one glyph width   column */
	x = (font.w+img.sp) * (nglyph%numcol) + img.sp;

	for(i=0; i<font.h; i++)
		for(j=0; j<font.w; j++)
			*(boxP + i*widthaligned + j) = *(imgP + (y+i)*img.w + x+j);

	/*
	 * change to hexadecimal
	 */
	bitstrP = malloc(LINECHARMAX * font.h);
	if(bitstrP == NULL)
		exiterror("cannot allocate memory: bitstrP %dbytes\n",LINECHARMAX*font.h);
	memset(bitstrP, 0x00, LINECHARMAX*font.h);

	/* write BBX , BITMAP to tmpVariable */
	sprintf(bitstrP, "BBX %d %d %d %d\nBITMAP\n",
		font.w, font.h, font.offx, font.offy);

	/* write bitmapData to tmpVariable */
	for(i=0; i<font.h; i++){
		for(j=0; j<widthaligned; j+=4){
			for(k=0; k<16; k++){
				if(strncmp(boxP+i*widthaligned+j, hex2binP[k],4)==0)
					sprintf(strP, "%x", k);
			}
			strcat(bitstrP, strP);
		}
		strcat(bitstrP, "\n");
	}
	strcat(bitstrP, "ENDCHAR\n");

	/* write tmpVariable to disk */
	val = fwrite(bitstrP, 1, strlen(bitstrP), outP);
	if(val != (int)strlen(bitstrP))
		exiterror("cannot write a file: bitstrP %dbytes", (int)strlen(bitstrP));

	free(boxP);
	free(bitstrP);
	return;
}


/*
 * read memory
 */
long mread(const void *varP, int size){
	const unsigned char *ucharP = varP;
	char strP[LINECHARMAX];
	char tmpP[SIZEMAX];
	int i;

	strcpy(strP, "0x");

	/* Numbers in BMP file are LSB-ordered. */
	for(i=size-1; i>=0; i--){
		sprintf(tmpP, "%02x", *(ucharP+i));
		strcat(strP, tmpP);
	}


	d_printf("'%s' ", strP);
	return strtol(strP,(char **)NULL, 16);
}


void exitrm(void){
	struct stat fileinfo;
	int val;

	fclose(outP);
	if(stat(outfileBdfP, &fileinfo) == 0){
		val = remove(outfileBdfP);
		if(val != 0)
			printf("error: cannot remove '%s'\n", outfileBdfP);
	}
	exit(EXIT_FAILURE);
}


void bmp8bit2img(unsigned char *bmpP, struct imageinfo bmp, char *imgP){
	int idx;
	int nclut; /* number of CLUT(color lookup table) */
	int notblack[256]; /* index number of palette */
	long byteline; /* number of byte in BMP oneline */
	int i, j, k;
	char c;
	unsigned char *b;
	unsigned char buf;

	/* check CLUT; black(RGB=0,0,0) or not */
	/*   '54' means CLUT address */
	nclut = 0;
	for(b=bmpP+54,idx=0; b<bmpP+bmp.offset; b+=4,idx++){
		if(*b==0 && *(b+1)==0 && *(b+2)==0)
			; /* black; do nothing */
		else{
			/* not black */
			if(nclut >= 256)
				exiterror("number of CLUT(%d) is too many\n", nclut);
			notblack[nclut] = idx;
			d_printf("%02x ", idx);
			nclut++;
		}
	}
	d_printf("  Not-black CLUT: %d\n", nclut);

	/*
	 * change BMP format to IMG format
	 *   IMG format(this-src-only format):
	 *		 remove BMP-header,padding
	 *		 reverse down-top to top-down
	 *		 black pixels become '1'
	 *		 white and gray and violet pixels become '0'
	 */
	byteline = (bmp.w + 3) / 4 * 4;

	for(i=0; i<img.h; i++){
		for(j=0; j<img.w; j++){
			buf = *(bmpP + bmp.offset + (bmp.h-1-i)*byteline + j);

			/* black? or not? */
			c = '1';
			for(k=0; k<nclut; k++)
				if(buf == notblack[k])
					c = '0'; /* not black */

			*(imgP + i*img.w + j) = c;
		}
	}
	return;
}

void bmp24bit2img(unsigned char *bmpP, struct imageinfo bmp, char *imgP){
	unsigned char *bufP;
	char c;
	int i,j;

	long byteline = (bmp.w * 3 + 3) / 4 * 4;

	for(i=0; i<img.h; i++){
		for(j=0; j<img.w; j++){
			c = '0';
			bufP = bmpP + bmp.offset + (bmp.h-1-i)*byteline + j*3;

			/* black? or not? */
			if(*bufP==0 && *(bufP+1)==0 && *(bufP+2)==0)
				c = '1'; /* black */

			*(imgP + i*img.w + j) = c;
		}
	}
	return;
}

void bmp4bit2img(unsigned char *bmpP, struct imageinfo bmp, char *imgP){
	char c;
	unsigned char buf;
	unsigned char *b;
	unsigned char *bufP;
	int notblack[16];
	int idx, nclut;
	int i, j, k;
	long byteline;

	/*
	 * read CLUT
	 */
	nclut = 0;
	for(b=bmpP+54,idx=0; b<bmpP+bmp.offset; b+=4,idx++){
		if(*b==0 && *(b+1)==0 && *(b+2)==0)
			;/* black */
		else{
			/* not black */
			if(nclut >= 16)
				exiterror("number of CLUT(%d) is too many\n", nclut);
			notblack[nclut] = idx;
			d_printf("%02x ", idx);
			nclut++;
		}
	}
	d_printf("  Not-black CLUT: %d\n", nclut);

	/*
	 * change BMP to IMG
	 */
	byteline = ((bmp.w/2)+(bmp.w%2!=0)+3) / 4 * 4;

	for(i=0; i<img.h; i++){
		for(j=0; j<img.w; j++){
			bufP = bmpP + bmp.offset + (bmp.h-1-i)*byteline + j/2;
			if(j%2==0)
				/* left half */
				buf = ((*bufP)>>4) & 0x0f;
			else
				/* right half */
				buf = (*bufP) & 0x0f;

			c = '1';
			for(k=0; k<nclut; k++)
				if(buf == notblack[k])
					c = '0'; /* not black */
			*(imgP + i*img.w + j) = c;
		}
	}
	return;
}
