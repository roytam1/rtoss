#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SKIP(c) do { (buf)+=(c); (size)-=(c); } while(0);
#define __MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define Decode16(p) ( (*(p))<<8 | (*((p)+1)) )

/* flags */
static int debug = 0;
static int newfile = 0;

/* buffer */
static int size;
static int length;
static unsigned char *buf;

/* prop. */
static int found_soi = 0;
static int found_eoi = 0;
static int comp = 0;
static int colsp = 0;
static int found_adobe = 0;
static int found_jfif = 0;
static int adobe_transform = 0;
static int done = 0;
static int compid[4];
static int data_start = 0;

/* dynamic list of marker-length-pos struct */
struct markers{
 int marker;
 int len;
 char *buf;
 struct markers *next;
};

static struct markers *allmarkers = NULL;

void markers_append(int marker, int len, char *buf) {
	struct markers *m, *t;

	m = (struct markers*) malloc(sizeof(struct markers));
	m->marker = marker;
	m->len = len;
	m->buf = buf;
	m->next = NULL;

	if(!allmarkers) {
		allmarkers = m;
	} else {
		t = allmarkers;
		while(t->next != NULL) t = t->next;
		t->next = m;
    }
}

struct markers* markers_find(int marker) {
	struct markers *m;
	if(allmarkers) {
		m = allmarkers;
		while(m != NULL) {
			if(m->marker == marker) return m;
			m = m->next;
		}
	}
	return NULL;
}
void markers_remove(struct markers *k) {
    struct markers *m, *t;
    if(allmarkers) {
		if(allmarkers == k) {
			allmarkers = k->next;
			free(k);
		} else {
			m = allmarkers;
			while(m != NULL) {
				if(m->next == k) {
					if(k->next) {
						m->next = k->next;
					} else {
						m->next = NULL;
					}
					free(k);
				}
				m = m->next;
    	    }
		}
    }
}

void markers_free() {
	struct markers *m, *t;
	if(allmarkers) {
		m = allmarkers;
		while(m->next != NULL) {
			t = m;
			m = m->next;
			free(t);
		}
		allmarkers = NULL;
	}
}


/* JPEG mark parsing */

static int DecodeLength(void) {
	int len;
    len = Decode16(buf);
	if(debug>0)
		fprintf(stderr," -> markerlength = %d\n",len);
	return len;
}

void SkipMarker(int marker) {
	int len;
    len = DecodeLength();
	markers_append(marker,len,buf);
    SKIP(len);
}

void DecodeSOF(int marker) {
	int len, i;
    len = DecodeLength();
	markers_append(marker,len,buf);
    comp = buf[7];

	if(comp*3+8 != len) {
		if(debug>-1)
			fprintf(stderr,"SOF marker length mismatch\n");
	} else {
		for(i=0; i<__MIN(comp,4); i++) {
			compid[i] = *(buf+8+i*3); // -2 bytes for length field
		}
	}

    SKIP(len);
}

void DecodeAPP0(int marker) {
	int len;
    len = DecodeLength();
	markers_append(marker,len,buf);
    SKIP(2);
	if(len < 14) {
		if(debug>-1)
			fprintf(stderr,"APP0 marker too short\n");
	} else {
		if(buf[0] == 0x4A && buf[1] == 0x46 &&
		 buf[2] == 0x49 && buf[3] == 0x46 &&
		 buf[4] == 0) {
			found_jfif = 1;
		}
	}
    SKIP(len-2);
}

void DecodeAPP14(int marker) {
	int len;
    len = DecodeLength();
	markers_append(marker,len,buf);
    SKIP(2);
	if(len < 12) {
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
    SKIP(len-2);
}

int main(int argc, char** argv){
	int i, newsize;
	unsigned char newname[256];
	unsigned char markers_kept[] = {0xC0,0xC1,0xC2,0xC9,0xCA,0xC3,0xC5,0xC6,0xC7,0xC8,0xCB,0xCD,0xCE,0xCF,0xCC,0xDD,0xDB,0xC4};
    FILE *f;

    if (argc < 2) {
        fprintf(stderr,"Usage: %s [-q|-v] [-n] <input.jpg>\n\t-q\tBe quiet\n\t-v\tBe verbose\n\t-n\tSave new file with '.new' suffix\n\t\twithout this switch, overwriting input.\n", argv[0]);
        return 2;
    }

	for(i=1;i<argc-1;i++) {
		if(strncmp(argv[i],"-v",2)==0)
			debug = 1;
		else if(strncmp(argv[i],"-q",2)==0)
			debug = -1;
		else if(strncmp(argv[i],"-n",2)==0)
			newfile = 1;
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

	do {
		if ((buf[0] == 0xFF) && (buf[1] == 0xD8)) {
			found_soi = buf;
		} else SKIP(1);
	} while(!found_soi && size > 0);

	if(!found_soi) {
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

			if(done && buf[-1] == 0xD9) {
				found_eoi = buf;
				done = 2;
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
						DecodeSOF(buf[-1]);
						break;
					case 0xE0: // APP0 marker
						DecodeAPP0(buf[-1]);
						break;
					case 0xEE: // APP14 marker
						DecodeAPP14(buf[-1]);
						break;
					case 0xDA: // SOS marker
						SkipMarker(buf[-1]);
						done = 1;
						data_start = buf;
						break;
					default:
						SkipMarker(buf[-1]);
				}
			}
		if(done == 2) break;
		}
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

	//printf("%d %d\n",comp,colsp);

	struct markers *m;
	if(debug>0 && allmarkers) {
		m = allmarkers;
		while(m != NULL) {
			printf("marker = %x, len = %d, buf = %p\n",m->marker,m->len,m->buf);
			m=m->next;
		}
	}


	/* write new file */
	strcpy(newname,argv[argc-1]);
	if(newfile)
		strcat(newname,".new");
    f = fopen(newname, "wb");
    if (!f) {
        fprintf(stderr,"Error opening the output file.\n");
        return 1;
    }

	fwrite((char*)found_soi,2,1,f);
	if(comp==4 && found_adobe) {
		m = markers_find(0xee);
		fwrite(m->buf-2,m->len+2,1,f);
		markers_remove(m);
	}
	for( i=0;i<(sizeof(markers_kept) / sizeof(markers_kept[0])); i++ ) {
		while(m = markers_find(markers_kept[i])) {
			fwrite(m->buf-2,m->len+2,1,f);
			markers_remove(m);
		}
	}
	if(m = markers_find(0xda)) {
		fwrite(m->buf-2,m->len+2,1,f);
		markers_remove(m);
		fwrite(data_start,found_eoi-data_start,1,f);
	}
	newsize = (int) ftell(f);
	printf("newsize = %d, diff = %d\n",newsize,length-newsize);
	fclose(f);


	if(debug>0) {
		if(found_jfif)
			fprintf(stderr,"app0: jfif marker found\n");
		if(found_adobe)
			fprintf(stderr,"adobe_transform = %d\n",adobe_transform);
		for(i=0; i<__MIN(comp,4); i++) {
			fprintf(stderr,"compid %d = %x\n",i,compid[i]);
		}
	}

	markers_free();
    return 0;
}
