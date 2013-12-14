/* ReadCPI - Extract 8x16 fonts from a CPI file
 *
 * Quick hack written by Marc Vuilleumier StÅckelberg
 *
 * March 1998
 */

#include <stdio.h>
#include <string.h>

typedef struct {
        unsigned        hlen;
        unsigned long   nextOfs;
        unsigned        deviceType;
        char            deviceName[8];
        unsigned        codePage;
        unsigned        reserved[3];
        unsigned long   fontOfs;
} CpHeader;

typedef struct {
        unsigned        fontType;
        unsigned        nFonts;
        unsigned        fontSize;
} Fonts;

typedef struct {
        unsigned char   height;
        unsigned char   width;
        unsigned        aspectRatio;
        unsigned        nChars;
} FontHeader;

int main(int argc, char *argv[])
{
        FILE            *src, *dst;
        int             nPages = 0, i;
        char            *c;
        char            fontname[32];
        CpHeader        cph;
        Fonts           fonts;
        FontHeader      fh;
        unsigned char   buffer[16*256];

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
        fseek(src, 0x17, SEEK_SET);
        if(fread(&nPages, 2, 1, src) < 1) {
                fprintf(stderr, "Cannot read file\n");
                return -1;
        }
        printf("File contains %d codepages\n", nPages);
        cph.nextOfs = ftell(src);
        while(nPages-- && !feof(src)) {
                fseek(src, cph.nextOfs, SEEK_SET);
                fread(&cph, sizeof(cph), 1, src);
                if(feof(src)) break;
                printf("Code page %d for %s\n", cph.codePage,
                        cph.deviceType == 1 ? "display" : "printer");
                if(cph.deviceType != 1)
                        continue;
                fseek(src, cph.fontOfs, SEEK_SET);
                fread(&fonts, sizeof(fonts), 1, src);
                if(feof(src)) break;
                if(fonts.fontType != 1) {
                        printf("  unknown font format, skipping\n");
                        continue;
                }
                printf("  %d fonts found\n", fonts.nFonts);
                for(i = 0; i < fonts.nFonts; i++) {
                        fread(&fh, sizeof(fh), 1, src);
                        printf("  - %d x %d (%d chars) ", fh.width, fh.height, fh.nChars);
                        if(fh.width == 8 && fh.height == 16 && fh.nChars == 256) {
                                fread(buffer, 16, 256, src);
                                sprintf(fontname, "cp%d.fnt", cph.codePage);
                                printf("SAVED as %s\n", fontname);
                                dst = fopen(fontname, "wb");
                                fwrite(buffer, 16, 256, dst);
                                fclose(dst);
                        } else {
                                printf("SKIPPED (not the right size)\n");
                                fseek(src, fh.height*fh.nChars, SEEK_CUR);
                        }
                }
        }
        fclose(src);
        return 0;
}
