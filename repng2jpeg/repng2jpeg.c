// 15:00 2004/08/05
#include "gd.h"
#include <stdio.h>
#include <stdlib.h> /* for atoi(),atof() */
#include <string.h>
#define VER "1.0.4.1"

int main(int argc, char *argv[]) {

	gdImagePtr dst,src;
	FILE *imgin,*imgout;
	int dstW,dstH,srcW,srcH,jpegq,imgouttype;
	double ratio;
	unsigned char num[4];

	if (argc >= 2 && (strcmp(argv[1],"--v") == 0 || strcmp(argv[1],"--ver") == 0 || strcmp(argv[1],"--version") == 0)){ 
		fprintf(stdout,"repng2jpeg version %s\n",VER);
		return 0;
	}

	if (argc >= 2 && strcmp(argv[1],"--test") == 0){ 
		fprintf(stdout,"repng2jpeg TestMode:OK version %s\n",VER);
		return 0;
	}

	if (argc >= 2 && strcmp(argv[1],"--help") == 0){ 
		fprintf(stdout,"usage: repng2jpeg inputfile outputfile (width height|Z ratio) [Quality|P|G]\n\ninput  JPEG,PNG,BMP,GIF\noutput JPEG,PNG,GIF\n\n[option]\n  Quality  Set the JPEG Quality (1-100 or Default.75)\n  P or p   Output a PNG image\n  G or g   Output a GIF image\n\nex.\nrepng2jpeg 1.jpg 2.jpg 400 300\nrepng2jpeg 1.jpg 2.jpg 400 300 90\nrepng2jpeg 1.jpg 2.png 400 300 P\nrepng2jpeg 1.jpg 2.gif Z 0.5 G\n\n",VER);
		return 0;
	}
	
	if (argc <= 4){ 
		fprintf(stderr,"repng2jpeg: missing file argument\nTry `repng2jpeg --help' for more information.\n");
		return 1;
	}

	if(!(imgin = fopen(argv[1],"rb"))) {
		fprintf(stderr,"%s open error!\n",argv[1]);
		return 2;
	}

	fread(num,1,4,imgin);
	fseek(imgin,0,0);

	if (num[0] == 0x89 && num[1] == 0x50 && num[2] == 0x4e && num[3] == 0x47){
		src = gdImageCreateFromPng(imgin);
	}else if (num[0] == 0xff && num[1] == 0xd8){
		src = gdImageCreateFromJpeg(imgin);
	}else if (num[0] == 0x42 && num[1] == 0x4d){
		src = gdImageCreateFromBmp(imgin);
	}else if (num[0] == 0x47 && num[1] == 0x49 && num[2] == 0x46){
		src = gdImageCreateFromGif(imgin);
	}else{
		fprintf(stderr,"Input is not in JPEG PNG BMP or GIF format!\n");
		return 3;
	}

	fclose(imgin);

	if(!src){
		fprintf(stderr,"Not Read Image!\n");
		return 4;
	}

	srcW = src->sx;
	srcH = src->sy;

	if(strcmp(argv[3],"Z") == 0||strcmp(argv[3],"z") == 0){ 
		ratio = atof(argv[4]);
		dstW = srcW * ratio;
		dstH = srcH * ratio;

	}else{
		dstW = atoi(argv[3]);
		dstH = atoi(argv[4]);
	} 

	jpegq = 0;
	imgouttype = 0;

	if(argc > 5){
		if(strcmp(argv[5],"P") == 0||strcmp(argv[5],"p") == 0){ imgouttype = 2; }
		else if(strcmp(argv[5],"G") == 0||strcmp(argv[5],"g") == 0){ imgouttype = 3; }
		else{ jpegq = atoi(argv[5]); imgouttype = 1; }
	}else{
		imgouttype = 1;
	}

/*
	if(dstW < 1||dstH < 1||dstW > 5000 ||dstH > 5000){
		fprintf(stderr,"Max Width or Height -> 5000px\n");
		gdImageDestroy(src);
		return 5;
	}
*/
	if(jpegq < 1||jpegq > 100){ jpegq = 75;	}

//	gd1 non Antialias
//	dst = gdImageCreate(dstW, dstH);
//	gdImageCopyResized(dst, src, 0, 0, 0, 0, dstW, dstH, srcW, srcH);

//	gd2 Antialias
	dst = gdImageCreateTrueColor(dstW, dstH);
	gdImageCopyResampled(dst, src, 0, 0, 0, 0, dstW, dstH, srcW, srcH);

	gdImageDestroy(src);

	if(!(imgout = fopen(argv[2], "wb"))) {
		fprintf(stderr,"%s open error!\n", argv[2]);
		gdImageDestroy(dst);
		return 6;
	}

	if(imgouttype == 1){
		gdImageJpeg(dst,imgout,jpegq);
	}else if(imgouttype == 2){
		gdImagePng(dst,imgout);
	}else if(imgouttype == 3){
		gdImageGif(dst,imgout);
	}

	fclose(imgout);
	gdImageDestroy(dst);

	return 0;
}
