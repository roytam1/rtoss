/* ReadCPI - Extract 8x16 fonts from a CPI file
 *
 * Quick hack written by Marc Vuilleumier StÅckelberg
 *
 * March 1998
 */

#include <stdio.h>
#include <string.h>

#define PACKED __attribute__((packed))

typedef struct {
	short PACKED cpeh_size;
	long  PACKED next_cpeh_offset;
	short PACKED device_type;
	char  PACKED device_name[8];
	short PACKED codepage;
	char  PACKED reserved[6];
	long  PACKED cpih_offset;
} CodePageEntryHeader;

typedef struct {
	short PACKED version;
	short PACKED num_fonts;
	short PACKED size;
} CodePageInfoHeader;

typedef struct {
	char  PACKED height;
	char  PACKED width;
	char  PACKED yaspect;
	char  PACKED xaspect;
	short PACKED num_chars;
} ScreenFontHeader;

typedef struct {
	char  PACKED id0;
	char  PACKED id[7];
	char  PACKED reserved[8];
	short PACKED pnum;
	char  PACKED ptyp;
	long  PACKED fih_offset;
} FontFileHeader;

typedef struct {
	short num_codepages;
} FontInfoHeader;


int main(int argc, char *argv[])
{
        FILE                *src, *dst;
        int                 nPages = 0, i;
        char                *c;
        char                fontname[32];
        FontFileHeader      ffh;
        FontInfoHeader      fih;
        CodePageEntryHeader cph;
		CodePageInfoHeader  cpih;
		ScreenFontHeader    sfh;
        unsigned char       buffer[16*256];

        argc--;
        if(argc != 1) {
                fprintf(stderr, "Usage: ReadCPI <cpifile>");
                return -1;
        }
        src = fopen(argv[1], "rb");
        if(!src) {
                fprintf(stderr, "Cannot open file\n");
                return -1;
        }
        fread(&ffh, sizeof(ffh), 1, src);
		fseek(src, ffh.fih_offset, SEEK_SET);
        fread(&fih, sizeof(fih), 1, src);

        printf("File contains %d codepages\n", fih.num_codepages);
        cph.next_cpeh_offset = ftell(src);
        while(fih.num_codepages-- && !feof(src)) {
                fseek(src, cph.next_cpeh_offset, SEEK_SET);
                fread(&cph, sizeof(cph), 1, src);
                if(feof(src)) break;
                printf("Code page %d for %s\n", cph.codepage,
                        cph.device_type == 1 ? "display" : "printer");
                if(cph.device_type != 1)
                        continue;
                fseek(src, cph.cpih_offset, SEEK_SET);
                fread(&cpih, sizeof(cpih), 1, src);
                if(feof(src)) break;
                if(cpih.version != 1) {
                        printf("  unknown font format, skipping\n");
                        continue;
                }
                printf("  %d fonts found\n", cpih.num_fonts);
                for(i = 0; i < cpih.num_fonts; i++) {
                        fread(&sfh, sizeof(sfh), 1, src);
                        printf("  - %d x %d (%d chars) ", sfh.width, sfh.height, sfh.num_chars);
                        /*if(sfh.width == 8 && sfh.height == 16 && sfh.num_chars == 256) {*/
                                fread(buffer, sfh.height, sfh.num_chars, src);
                                sprintf(fontname, "cp%d.f%02d", cph.codepage, sfh.height);
                                printf("SAVED as %s\n", fontname);
                                dst = fopen(fontname, "wb");
                                fwrite(buffer, sfh.height, sfh.num_chars, dst);
                                fclose(dst);
                        /*} else {
                                printf("SKIPPED (not the right size)\n");
                                fseek(src, sfh.height*sfh.nChars, SEEK_CUR);
                        }*/
                }
        }
        fclose(src);
        return 0;
}
