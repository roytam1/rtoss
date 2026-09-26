#include <stdio.h>
#include <string.h>

/* Returns 1 if GIF has > 1 frame, 0 if single frame, standard GIF, or error */
int is_animated_gif(const char *filename) {
    FILE *file;
    unsigned char header[13];
    int frame_count;
    int byte;
    unsigned char packed_byte;
    int gct_flag;
    int gct_size;

    file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }

    /* 1. Read Header and Logical Screen Descriptor (13 bytes) */
    if (fread(header, 1, 13, file) != 13) {
        fclose(file);
        return 0;
    }

    /* Verify GIF signature ("GIF87a" or "GIF89a") */
    if (memcmp(header, "GIF87a", 6) != 0 && memcmp(header, "GIF89a", 6) != 0) {
        fclose(file);
        return 0; /* Not a valid GIF */
    }

    /* 2. Skip Global Color Table if present */
    packed_byte = header[10];
    gct_flag = (packed_byte & 0x80) >> 7;
    if (gct_flag) {
        gct_size = 1 << ((packed_byte & 0x07) + 1); /* 2^(N+1) entries */
        /* Each color entry is 3 bytes (RGB) */
        if (fseek(file, gct_size * 3, SEEK_CUR) != 0) {
            fclose(file);
            return 0;
        }
    }

    /* 3. Scan for frame markers */
    frame_count = 0;

    while ((byte = fgetc(file)) != EOF) {
        if (byte == 0x3B) {
            break; /* Reached GIF Trailer */
        }

        if (byte == 0x2C) {
            /* Image Descriptor marker found */
            frame_count++;
            if (frame_count > 1) {
                fclose(file);
                return 1; /* More than 1 frame detected */
            }

            /* Skip remaining Image Descriptor header (9 bytes: Left, Top, Width, Height, Packed) */
            if (fseek(file, 9, SEEK_CUR) != 0) {
                break;
            }

            /* Skip Local Color Table if present */
            byte = fgetc(file);
            if (byte == EOF) {
                break;
            }
            if (byte & 0x80) {
                int lct_size = 1 << ((byte & 0x07) + 1);
                if (fseek(file, lct_size * 3, SEEK_CUR) != 0) {
                    break;
                }
            } else {
                /* Put back byte if LCT is not present */
                ungetc(byte, file);
            }

            /* Skip Image Data LZW minimum code size (1 byte) */
            if (fgetc(file) == EOF) {
                break;
            }

            /* Skip LZW Data Sub-blocks */
            while ((byte = fgetc(file)) != EOF && byte > 0) {
                if (fseek(file, byte, SEEK_CUR) != 0) {
                    break;
                }
            }
        } else if (byte == 0x21) {
            /* Extension Introducer */
            int ext_label = fgetc(file);
            if (ext_label == EOF) {
                break;
            }

            /* Skip Extension Sub-blocks */
            while ((byte = fgetc(file)) != EOF && byte > 0) {
                if (fseek(file, byte, SEEK_CUR) != 0) {
                    break;
                }
            }
        }
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file.gif>\n", argv[0]);
        return 1;
    }

    return is_animated_gif(argv[1]);
}
