#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SKIP(c) do { (buf)+=(c); (size)-=(c); } while(0);
#define __MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define Decode16(p) ( (*(p))<<8 | (*((p)+1)) )

/* flags */
static int debug = 0;
static int numeric = 0;

/* buffer */
static int size;
static int length;
static unsigned char *buf;

/* prop. */
static int imgw = 0;
static int imgh = 0;
static int comp = 0;
static int colsp = 0;
static int found_adobe = 0;
static int found_jfif = 0;
static int adobe_transform = 0;
static int done = 0;
static int method = 0;
static int compid[4];


static const char		*cspaces[] =
				{	/* JPEG colorspaces... */
				  "UNKNOWN",
				  "GRAYSCALE",
				  "RGB",
				  "YCbCr",
				  "CMYK",
				  "YCCK"
				};

static const char		*methods[] =
				{	/* JPEG methods... */
				  "Baseline",
				  "Extended-sequential",
				  "Progressive",
				  "Lossless",
				  "", // 0xC4
				  "Differential-sequential",
				  "Differential-progressive",
				  "Differential-lossless",
				  "Reserved",
				  "Extended-sequential-arithmetic",
				  "Progressive-arithmetic",
				  "Lossless-arithmetic",
				  "", // 0xCC
				  "Differential-sequential-arithmetic",
				  "Differential-progressive-arithmetic",
				  "Differential-lossless-arithmetic"
				};



static int DecodeLength(void) {
	int length;
    length = Decode16(buf);
	if(debug>0)
		fprintf(stderr," -> markerlength = %d\n",length);
    SKIP(2);
	return length;
}

void SkipMarker(void) {
	int length;
    length = DecodeLength();
    SKIP(length-2);
}

void DecodeSOF(void) {
	int length; int i;
    length = DecodeLength();
    imgh = Decode16(buf+1);
    imgw = Decode16(buf+3);
    comp = buf[5];

	if(comp*3+8 != length) {
		if(debug>-1)
			fprintf(stderr,"SOF marker length mismatch\n");
	} else {
		for(i=0; i<__MIN(comp,4); i++) {
			compid[i] = *(buf+6+i*3); // -2 bytes for length field
		}
	}

    SKIP(length-2);
}

void DecodeAPP0(void) {
	int length;
    length = DecodeLength();
	if(length < 14) {
		if(debug>-1)
			fprintf(stderr,"APP0 marker too short\n");
	} else {
		if(buf[0] == 0x4A && buf[1] == 0x46 &&
		 buf[2] == 0x49 && buf[3] == 0x46 &&
		 buf[4] == 0) {
			found_jfif = 1;
		}
	}
    SKIP(length-2);
}

void DecodeAPP14(void) {
	int length;
    length = DecodeLength();
	if(length < 12) {
		if(debug>-1)
			fprintf(stderr,"APP14 marker too short\n");
	} else {
		if(buf[0] == 0x41 && buf[1] == 0x64 &&
		 buf[2] == 0x6F && buf[3] == 0x62 &&
		 buf[4] == 0x65) {
			found_adobe = 1;
			adobe_transform = buf[11];
		}
	}
    SKIP(length-2);
}

int main(int argc, char** argv){
	int i;
    FILE *f;

    if (argc < 2) {
        fprintf(stderr,"Usage: %s [-q|-v] [-n] <input.jpg>\n\t-q\tBe quiet\n\t-v\tBe verbose\n\t-n\tPrint numeric instead of string\n", argv[0]);
        return 2;
    }

	for(i=1;i<argc-1;i++) {
		if(strncmp(argv[i],"-v",2)==0)
			debug = 1;
		else if(strncmp(argv[i],"-q",2)==0)
			debug = -1;
		else if(strncmp(argv[i],"-n",2)==0)
			numeric = 1;
	}

    f = fopen(argv[argc-1], "rb");
    if (!f) {
        fprintf(stderr,"Error opening the input file.\n");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    length = size = (int) ftell(f);
    buf = malloc(size);
    fseek(f, 0, SEEK_SET);
    size = (int) fread(buf, 1, size, f);
    fclose(f);

	if (size < 2) {
		if(debug>-1)
			fprintf(stderr,"file too small\n");
		return 1;
	}
	if (!((buf[0] == 0xFF) && (buf[1] == 0xD8))) {
		if(debug>-1)
			fprintf(stderr,"file is not JPEG\n");
		return 1;
	}

	SKIP(2);
	if(debug>0)
		fprintf(stderr,"offset %d marker %02x%02x\n",length-size-2,buf[-2],buf[-1]);

	while (size > 0) {
		SKIP(1);
		if(buf[-2]==0xff) {
			if(debug>0)
				if((!done && buf[-1]>0xBF && buf[-1]!=0xFF) || (done && buf[-1] == 0xD9))
					fprintf(stderr,"offset %d marker %02x%02x\n",length-size-2,buf[-2],buf[-1]);

			if(done) {
				if(buf[-1] == 0xD9) done = 2;
				else SKIP(1);
			}

			if(!done && buf[-1]>0xBF && buf[-1]!=0xFF) {
				switch (buf[-1]) {
					case 0xC0: // SOF0 (baseline)
					case 0xC1: // SOF1 (Extended sequential, Huffman)
					case 0xC2: // SOF2 (Progressive, Huffman)
					case 0xC9: // SOF9 (Extended sequential, arithmetic)
					case 0xCA: // SOF10 (Progressive, arithmetic)
					// rarely used/unsupported SOFs
					case 0xC3: // SOF3 (Lossless, Huffman)
					case 0xC5: // SOF5 (Differential sequential, Huffman)
					case 0xC6: // SOF6 (Differential progressive, Huffman)
					case 0xC7: // SOF7 (Differential lossless, Huffman)
					case 0xC8: // SOF8 (Reserved for JPEG extensions)
					case 0xCB: // SOF11 (Lossless, arithmetic)
					case 0xCD: // SOF13 (Differential sequential, arithmetic)
					case 0xCE: // SOF14 (Differential progressive, arithmetic)
					case 0xCF: // SOF15 (Differential lossless, arithmetic)
						method = buf[-1] - 0xc0;
						DecodeSOF();
						break;
					case 0xE0: // APP0 marker
						DecodeAPP0();
						break;
					case 0xEE: // APP14 marker
						DecodeAPP14();
						break;
					case 0xDA: // SOS marker
						SkipMarker();
						done = 1;
						break;
					default:
						SkipMarker();
				}
			}
		if(done == 2) break;
		} else if(buf[-1]!=0xff) SKIP(1);
	}

	if(debug>0)
		fprintf(stderr,"remain size = %d\n",size);

	if(comp == 1)
		colsp = 1; // Grayscale
	else if (comp == 3) {
		if(found_jfif)
			colsp = 3;
		else if (found_adobe)
			colsp = !adobe_transform ? 2 : 3;
		else if(compid[0] == 1 && compid[1] == 2 && compid[2] == 3)
			colsp = 3; // YCbCr
		else if(compid[0] == 82 && compid[1] == 71 && compid[2] == 66)
			colsp = 2;  // RGB
	} else if (comp == 4) {
		if (found_adobe)
			colsp = !adobe_transform ? 4 : 5;
		else
			colsp = 4;  // CMYK
	}

	if(numeric)
		printf("%d %d %d %d %d\n",imgw,imgh,comp,colsp,method);
	else
		printf("%d %d %d %s %s\n",imgw,imgh,comp,cspaces[colsp],methods[method]);

	if(debug>0) {
		if(found_jfif)
			fprintf(stderr,"app0: jfif marker found\n");
		if(found_adobe)
			fprintf(stderr,"adobe_transform = %d\n",adobe_transform);
		for(i=0; i<__MIN(comp,4); i++) {
			fprintf(stderr,"compid %d = %x\n",i,compid[i]);
		}
	}

    return 0;
}
