#include <stdio.h>
#include <string.h>

#define PNG_HEADER_SIZE 8

/* C89 string constant for 8-byte PNG header */
static const unsigned char PNG_SIGNATURE[PNG_HEADER_SIZE] = {
    0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A
};

/* Converts big-endian 32-bit uint from PNG byte stream to host endianness */
static unsigned long read_uint32_be(const unsigned char *buffer) {
    return ((unsigned long)buffer[0] << 24) |
           ((unsigned long)buffer[1] << 16) |
           ((unsigned long)buffer[2] << 8)  |
            (unsigned long)buffer[3];
}

/* Returns 1 if APNG, 0 if not or on error */
int is_apng(const char *filename) {
    FILE *file;
    unsigned char header[PNG_HEADER_SIZE];
    unsigned char length_buf[4];
    char type_buf[5];
    unsigned long length;

    file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }

    /* 1. Verify PNG header */
    if (fread(header, 1, PNG_HEADER_SIZE, file) != PNG_HEADER_SIZE) {
        fclose(file);
        return 0;
    }

    if (memcmp(header, PNG_SIGNATURE, PNG_HEADER_SIZE) != 0) {
        fclose(file); /* Not a valid PNG */
        return 0;
    }

    /* Initialize null terminator for type string */
    type_buf[4] = '\0';

    /* 2. Iterate through chunks */
    while (fread(length_buf, 1, 4, file) == 4) {
        length = read_uint32_be(length_buf);

        if (fread(type_buf, 1, 4, file) != 4) {
            break;
        }

        /* Check chunk type */
        if (memcmp(type_buf, "acTL", 4) == 0) {
            fclose(file);
            return 1; /* Found Animation Control Chunk */
        }

        if (memcmp(type_buf, "IDAT", 4) == 0 || memcmp(type_buf, "IEND", 4) == 0) {
            fclose(file);
            return 0; /* Reached image data without encountering acTL */
        }

        /* Skip payload (length) + 4 bytes for CRC */
        if (fseek(file, (long)length + 4, SEEK_CUR) != 0) {
            break;
        }
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file.png>\n", argv[0]);
        return 0;
    }

    return !is_apng(argv[1]);

}
