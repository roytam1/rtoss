#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Using standard types for C89 compatibility */
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;

#pragma pack(push, 1)
/* BMP structures */
typedef struct {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BMP_FILEHEADER;

typedef struct {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BMP_INFOHEADER;

/* MSP Version 2 Header */
typedef struct {
    char  key[4];        /* "DanM" */
    WORD  width;         /* Width in pixels */
    WORD  height;        /* Height in pixels */
    WORD  XARBitmap;     /* X Aspect ratio of the bitmap   */
    WORD  YARBitmap;     /* Y Aspect ratio of the bitmap   */
    WORD  XARPrinter;    /* X Aspect ratio of the printer   */
    WORD  YARPrinter;    /* Y Aspect ratio of the printer   */
    WORD  PrinterWidth;  /* Width of the printer in pixels   */
    WORD  PrinterHeight; /* Height of the printer in pixels   */
    WORD  XAspectCorr;   /* X aspect correction (unused)     */
    WORD  YAspectCorr;   /* Y aspect correction (unused)     */
    WORD  Checksum;      /* Checksum of previous 24 bytes   */
    WORD  Padding[3];    /* Unused padding    */
} MSP_HEADER;
#pragma pack(pop)

/* Compresses a single row using MSP v1 RLE. Returns the compressed size. */
int compress_msp_row(const unsigned char *src, int src_len, unsigned char *dest) {
    int src_idx = 0;
    int dest_idx = 0;

    while (src_idx < src_len) {
        int run_len = 1;

        /* Find how long the current byte repeats */
        while (src_idx + run_len < src_len && 
               src[src_idx] == src[src_idx + run_len] && 
               run_len < 256) {
            run_len++;
        }

        if (run_len > 1) {
            /* Encode Run: Control byte is 00 */
            dest[dest_idx++] = 0;
            dest[dest_idx++] = (unsigned char)(run_len);
            dest[dest_idx++] = src[src_idx];
            src_idx += run_len;
        } else {
            /* Encode Literal: Find how many unique bytes follow */
            int lit_len = 0;
            while (src_idx + lit_len < src_len && lit_len < 256) {
                /* Stop if a run of 3 or more repeating bytes is found ahead */
                if (src_idx + lit_len + 2 < src_len &&
                    src[src_idx + lit_len] == src[src_idx + lit_len + 1] &&
                    src[src_idx + lit_len] == src[src_idx + lit_len + 2]) {
                    break;
                }
                lit_len++;
            }

            /* Encode Literal: Control byte is lit_len */
            dest[dest_idx++] = (unsigned char)lit_len;
            memcpy(&dest[dest_idx], &src[src_idx], lit_len);
            dest_idx += lit_len;
            src_idx += lit_len;
        }
    }

    /* End of line marker required by MSP v1 */
    //dest[dest_idx++] = 0x00; 
    return dest_idx;
}

int convert_bmp_to_msp(const char *bmp_filename, const char *msp_filename) {
    FILE *f_bmp, *f_msp;
    BMP_FILEHEADER bfh;
    BMP_INFOHEADER bih;
    MSP_HEADER msh;
    WORD* msphdr;
    
    int bmp_row_stride, msp_row_stride;
    int y, x;
    unsigned char *bmp_row_buf = NULL;
    unsigned char *msp_row_buf = NULL;
    unsigned char *comp_row_buf = NULL;
    WORD *offset_table = NULL;

    f_bmp = fopen(bmp_filename, "rb");
    if (!f_bmp) {
        fprintf(stderr, "Error: Cannot open input BMP file.\n");
        return 0;
    }

    /* Read BMP Headers */
    if (fread(&bfh, sizeof(BMP_FILEHEADER), 1, f_bmp) != 1 ||
        fread(&bih, sizeof(BMP_INFOHEADER), 1, f_bmp) != 1) {
        fprintf(stderr, "Error: Failed to read BMP headers.\n");
        fclose(f_bmp);
        return 0;
    }

    /* Validate 1bpp uncompressed BMP */
    if (bfh.bfType != 0x4D42 || bih.biBitCount != 1 || bih.biCompression != 0) {
        fprintf(stderr, "Error: Input must be an uncompressed 1bpp BMP file.\n");
        fclose(f_bmp);
        return 0;
    }

    /* Handle bottom-up BMPs (standard) vs top-down BMPs (negative height) */
    if (bih.biHeight < 0) {
        bih.biHeight = -bih.biHeight;
    }

    f_msp = fopen(msp_filename, "wb");
    if (!f_msp) {
        fprintf(stderr, "Error: Cannot create output MSP file.\n");
        fclose(f_bmp);
        return 0;
    }

    /* Setup MSP v2 Header */
    memset(&msh, 0, sizeof(MSP_HEADER));
    memcpy(msh.key, "LinS", 4);
    msh.width  = (WORD)bih.biWidth;
    msh.height = (WORD)bih.biHeight;

    msphdr = (WORD*)&msh;
    msh.Checksum = *msphdr;
    for (y = 1; y < 12; y++) {
        msh.Checksum ^= *(msphdr+y);
    }

    /* Write MSP Header */
    fwrite(&msh, sizeof(MSP_HEADER), 1, f_msp);

    /* Allocate and reserve space for the Line Offset Table */
    offset_table = (WORD *)calloc(bih.biHeight, sizeof(WORD));
    /* Write blank table for now; we will overwrite it later */
    fwrite(offset_table, sizeof(WORD), bih.biHeight, f_msp);

    /* Calculate row strides (BMP pads to 4 bytes, MSP pads to 2 bytes) */
    bmp_row_stride = ((bih.biWidth + 31) / 32) * 4;
    msp_row_stride = ((bih.biWidth + 15) / 16) * 2;

    bmp_row_buf = (unsigned char *)malloc(bmp_row_stride);
    msp_row_buf = (unsigned char *)malloc(msp_row_stride);
    /* Worst-case compressed size is roughly double uncompressed size */
    comp_row_buf = (unsigned char *)malloc(msp_row_stride * 2 + 2);

    if (!bmp_row_buf || !msp_row_buf || !comp_row_buf || !offset_table) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        free(bmp_row_buf); free(msp_row_buf); free(comp_row_buf); free(offset_table);
        fclose(f_bmp); fclose(f_msp);
        return 0;
    }

    /* Read BMP pixel data and write to MSP */
    /* Note: BMP stores rows bottom-to-top, but MSP is top-to-bottom. */
    for (y = (int)bih.biHeight - 1; y >= 0; y--) {
        int msp_y = (int)bih.biHeight - 1 - y; /* Target row index in MSP */
        int comp_size;
        long bmp_row_offset = bfh.bfOffBits + (y * bmp_row_stride);
        /* Seek to the specific BMP row */
        fseek(f_bmp, bmp_row_offset, SEEK_SET);
        
        if (fread(bmp_row_buf, 1, bmp_row_stride, f_bmp) != (size_t)bmp_row_stride) {
            fprintf(stderr, "Error: Failed reading BMP pixel data.\n");
            break;
        }

        /* Clear MSP buffer row for fresh packing */
        memset(msp_row_buf, 0, msp_row_stride);

        /* Process bytes. Invert the bits because MSP is 1=Black, 0=White */
        for (x = 0; x < msp_row_stride; x++) {
            if (x < bmp_row_stride) {
                msp_row_buf[x] = bmp_row_buf[x];
            } else {
                msp_row_buf[x] = 0xFF; /* Pad out remaining bits if needed */
            }
        }

        /* Compress the row */
        comp_size = compress_msp_row(msp_row_buf, msp_row_stride, comp_row_buf);

        /* Record the current row length into the offset table */
        offset_table[msp_y] = (WORD)comp_size;

        /* Write compressed data to file */
        fwrite(comp_row_buf, 1, comp_size, f_msp);
    }

    /* Rewind and update the correct Line Offset Table in the file */
    fseek(f_msp, sizeof(MSP_HEADER), SEEK_SET);
    fwrite(offset_table, sizeof(WORD), bih.biHeight, f_msp);

    /* Clean up */
    free(bmp_row_buf);
    free(msp_row_buf);
    free(comp_row_buf);
    free(offset_table);
    fclose(f_bmp);
    fclose(f_msp);

    printf("Success: Converted %s to %s successfully.\n", bmp_filename, msp_filename);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input.bmp> <output.msp>\n", argv[0]);
        return 1;
    }

    if (!convert_bmp_to_msp(argv[1], argv[2])) {
        return 1;
    }

    return 0;
}
