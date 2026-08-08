#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_MATRIX_SIZE 49 // Version 8 is 49x49

// --- Galois Field GF(256) Math ---
static unsigned char gf_exp[512];
static unsigned char gf_log[256];

void init_gf() {
    int i, x = 1;
    for (i = 0; i < 255; i++) {
        gf_exp[i] = x;
        gf_exp[i + 255] = x;
        gf_log[x] = i;
        x <<= 1;
        if (x & 0x100) x ^= 0x11D;
    }
}

unsigned char gf_mul(unsigned char x, unsigned char y) {
    if (x == 0 || y == 0) return 0;
    return gf_exp[gf_log[x] + gf_log[y]];
}

// Dynamic Reed-Solomon Generator Polynomial
void generate_rs_poly(int ec_count, unsigned char *poly) {
    int i, j;
    memset(poly, 0, ec_count + 1);
    poly[0] = 1;
    for (i = 0; i < ec_count; i++) {
        unsigned char root = gf_exp[i];
        for (j = i + 1; j > 0; j--) {
            poly[j] = poly[j] ^ gf_mul(poly[j - 1], root);
        }
    }
}

void calculate_rs(const unsigned char *data, int data_len, unsigned char *ec_out, int ec_count) {
    int i, j;
    unsigned char *poly = (unsigned char *)malloc(ec_count + 1);
    generate_rs_poly(ec_count, poly);

    memset(ec_out, 0, ec_count);
    for (i = 0; i < data_len; i++) {
        unsigned char factor = data[i] ^ ec_out[0];
        for (j = 0; j < ec_count - 1; j++) {
            ec_out[j] = ec_out[j + 1] ^ gf_mul(poly[j + 1], factor);
        }
        ec_out[ec_count - 1] = gf_mul(poly[ec_count], factor);
    }
    free(poly);
}

// --- QR Version Specifications (Level M) ---
typedef struct {
    int version;
    int size;
    int total_data_bytes;
    int ec_per_block;
    int g1_blocks;
    int g1_data_bytes;
    int g2_blocks;
    int g2_data_bytes;
    int align_count;
    int align_coords[3];
    unsigned int version_info; // 18-bit code for V7+
} QRVersionSpec;

static const QRVersionSpec VERSION_SPECS[8] = {
    // V1
    {1, 21, 16, 10, 1, 16, 0,  0, 0, {0, 0, 0}, 0},
    // V2
    {2, 25, 28, 16, 1, 28, 0,  0, 2, {6, 18, 0}, 0},
    // V3
    {3, 29, 44, 26, 1, 44, 0,  0, 2, {6, 22, 0}, 0},
    // V4
    {4, 33, 64, 18, 2, 32, 0,  0, 2, {6, 26, 0}, 0},
    // V5
    {5, 37, 86, 24, 2, 43, 0,  0, 2, {6, 30, 0}, 0},
    // V6
    {6, 41, 108, 16, 4, 27, 0,  0, 2, {6, 34, 0}, 0},
    // V7
    {7, 45, 124, 18, 4, 31, 0,  0, 3, {6, 22, 38}, 0x07C94},
    // V8
    {8, 49, 154, 22, 2, 38, 2, 39, 3, {6, 24, 42}, 0x085BC}
};

// --- Matrix Operations ---
typedef struct {
    int size;
    int modules[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE]; // 0=Light, 1=Dark, -1=Unset
} QRMatrix;

void set_module(QRMatrix *m, int r, int c, int is_dark) {
    if (r >= 0 && r < m->size && c >= 0 && c < m->size) {
        m->modules[r][c] = is_dark ? 1 : 0;
    }
}

void draw_finder_pattern(QRMatrix *m, int row, int col) {
    int r, c;
    for (r = -1; r <= 7; r++) {
        for (c = -1; c <= 7; c++) {
            if (row + r < 0 || row + r >= m->size || col + c < 0 || col + c >= m->size) continue;
            if (r == -1 || r == 7 || c == -1 || c == 7) set_module(m, row + r, col + c, 0);
            else if (r == 0 || r == 6 || c == 0 || c == 6) set_module(m, row + r, col + c, 1);
            else if (r >= 2 && r <= 4 && c >= 2 && c <= 4) set_module(m, row + r, col + c, 1);
            else set_module(m, row + r, col + c, 0);
        }
    }
}

void draw_alignment_pattern(QRMatrix *m, int center_r, int center_c) {
    int r, c;
    // Avoid drawing over finder patterns
    if ((center_r - 2 < 8 && center_c - 2 < 8) ||
        (center_r - 2 < 8 && center_c + 2 >= m->size - 8) ||
        (center_r + 2 >= m->size - 8 && center_c - 2 < 8)) return;

    for (r = -2; r <= 2; r++) {
        for (c = -2; c <= 2; c++) {
            if (abs(r) == 2 || abs(c) == 2 || (r == 0 && c == 0)) {
                set_module(m, center_r + r, center_c + c, 1);
            } else {
                set_module(m, center_r + r, center_c + c, 0);
            }
        }
    }
}

// --- Matrix Assembly ---
void build_matrix(const unsigned char *text, int len, const QRVersionSpec *spec, QRMatrix *m) {
    int r, c, i, j, total_blocks, b, total_payload_bytes, max_data_bytes, col;
    int raw_offset = 0;
    unsigned char *raw_data, *final_stream;
    unsigned char **data_blocks, **ec_blocks;
    unsigned char pad[2] = {0xEC, 0x11};
    int p_idx = 0;
    int stream_idx = 0;
    int bit_idx = 0;
    int dir = -1; // Up
    unsigned short format = 0x5412;
    int fmt_bits[15];

    m->size = spec->size;
    for (r = 0; r < m->size; r++)
        for (c = 0; c < m->size; c++)
            m->modules[r][c] = -1;

    // 1. Finder Patterns
    draw_finder_pattern(m, 0, 0);
    draw_finder_pattern(m, 0, m->size - 7);
    draw_finder_pattern(m, m->size - 7, 0);

    // 2. Alignment Patterns
    for (i = 0; i < spec->align_count; i++) {
        for (j = 0; j < spec->align_count; j++) {
            draw_alignment_pattern(m, spec->align_coords[i], spec->align_coords[j]);
        }
    }

    // 3. Timing Patterns
    for (i = 8; i < m->size - 8; i++) {
        if (m->modules[6][i] == -1) m->modules[6][i] = (i % 2 == 0) ? 1 : 0;
        if (m->modules[i][6] == -1) m->modules[i][6] = (i % 2 == 0) ? 1 : 0;
    }

    // 4. Dark Module
    m->modules[4 * spec->version + 9][8] = 1;

    // 5. Reserve Format Information Area
    for (i = 0; i < 9; i++) {
        if (m->modules[8][i] == -1) m->modules[8][i] = 0;
        if (m->modules[i][8] == -1) m->modules[i][8] = 0;
    }
    for (i = m->size - 8; i < m->size; i++) {
        if (m->modules[8][i] == -1) m->modules[8][i] = 0;
        if (m->modules[i][8] == -1) m->modules[i][8] = 0;
    }

    // 6. Reserve Version Information Area (V7+)
    if (spec->version >= 7) {
        for (r = 0; r < 6; r++) {
            for (c = 0; c < 3; c++) {
                m->modules[r][m->size - 11 + c] = 0; // Top-Right
                m->modules[m->size - 11 + c][r] = 0; // Bottom-Left
            }
        }
    }

    // --- Data Stream Preparation ---
    raw_data = (unsigned char *)calloc(spec->total_data_bytes, 1);
    
    // Header: Byte Mode (0x4) + Length (8-bit)
    raw_data[0] = 0x40 | ((len >> 4) & 0x0F);
    raw_data[1] = (len & 0x0F) << 4;

    for (i = 0; i < len; i++) {
        raw_data[1 + i] |= (text[i] >> 4);
        raw_data[2 + i] = (text[i] & 0x0F) << 4;
    }

    // Padding Bytes
    for (i = 1 + len + 1; i < spec->total_data_bytes; i++) {
        raw_data[i] = pad[p_idx % 2];
        p_idx++;
    }

    // --- Multi-Block Error Correction & Interleaving ---
    total_blocks = spec->g1_blocks + spec->g2_blocks;
    data_blocks = (unsigned char **)malloc(total_blocks * sizeof(unsigned char *));
    ec_blocks = (unsigned char **)malloc(total_blocks * sizeof(unsigned char *));

    for (b = 0; b < total_blocks; b++) {
        int b_data_len = (b < spec->g1_blocks) ? spec->g1_data_bytes : spec->g2_data_bytes;
        data_blocks[b] = (unsigned char *)malloc(b_data_len);
        ec_blocks[b] = (unsigned char *)malloc(spec->ec_per_block);

        memcpy(data_blocks[b], &raw_data[raw_offset], b_data_len);
        calculate_rs(data_blocks[b], b_data_len, ec_blocks[b], spec->ec_per_block);
        raw_offset += b_data_len;
    }

    // Interleave Data and EC Bytes
    total_payload_bytes = spec->total_data_bytes + (total_blocks * spec->ec_per_block);
    final_stream = (unsigned char *)malloc(total_payload_bytes);

    max_data_bytes = (spec->g2_blocks > 0) ? spec->g2_data_bytes : spec->g1_data_bytes;
    for (i = 0; i < max_data_bytes; i++) {
        for (b = 0; b < total_blocks; b++) {
            int b_data_len = (b < spec->g1_blocks) ? spec->g1_data_bytes : spec->g2_data_bytes;
            if (i < b_data_len) {
                final_stream[stream_idx++] = data_blocks[b][i];
            }
        }
    }

    for (i = 0; i < spec->ec_per_block; i++) {
        for (b = 0; b < total_blocks; b++) {
            final_stream[stream_idx++] = ec_blocks[b][i];
        }
    }

    // Free Temporary Block Memory
    for (b = 0; b < total_blocks; b++) {
        free(data_blocks[b]);
        free(ec_blocks[b]);
    }
    free(data_blocks);
    free(ec_blocks);
    free(raw_data);

    // --- Zigzag Data Placement ---
    col = m->size - 1;

    while (col > 0) {
        int row_start, row_end;
        if (col == 6) col--; // Skip vertical timing column
        row_start = (dir == -1) ? (m->size - 1) : 0;
        row_end = (dir == -1) ? -1 : m->size;

        for (r = row_start; r != row_end; r += dir) {
            for (c = col; c > col - 2; c--) {
                if (m->modules[r][c] == -1) {
                    int byte_pos = bit_idx / 8;
                    int bit_pos = 7 - (bit_idx % 8);
                    int bit = (final_stream[byte_pos] >> bit_pos) & 1;

                    // Apply Mask 0: (row + col) % 2 == 0
                    if ((r + c) % 2 == 0) bit ^= 1;

                    m->modules[r][c] = bit;
                    bit_idx++;
                }
            }
        }
        dir = -dir;
        col -= 2;
    }

    free(final_stream);

    // --- Format Information Placement (Mask 0, EC Level M: 0x5412) ---
    for (i = 0; i < 15; i++) {
        fmt_bits[i] = (format >> (14 - i)) & 1; // fmt_bits[0] is MSB (bit 14)
    }

    // Top-Left Format Info
    m->modules[8][0] = fmt_bits[0];  m->modules[8][1] = fmt_bits[1];
    m->modules[8][2] = fmt_bits[2];  m->modules[8][3] = fmt_bits[3];
    m->modules[8][4] = fmt_bits[4];  m->modules[8][5] = fmt_bits[5];
    m->modules[8][7] = fmt_bits[6];  m->modules[8][8] = fmt_bits[7];
    m->modules[7][8] = fmt_bits[8];  m->modules[5][8] = fmt_bits[9];
    m->modules[4][8] = fmt_bits[10]; m->modules[3][8] = fmt_bits[11];
    m->modules[2][8] = fmt_bits[12]; m->modules[1][8] = fmt_bits[13];
    m->modules[0][8] = fmt_bits[14];

    // Bottom-Left / Top-Right Format Info
    m->modules[m->size - 1][8] = fmt_bits[0];  m->modules[m->size - 2][8] = fmt_bits[1];
    m->modules[m->size - 3][8] = fmt_bits[2];  m->modules[m->size - 4][8] = fmt_bits[3];
    m->modules[m->size - 5][8] = fmt_bits[4];  m->modules[m->size - 6][8] = fmt_bits[5];
    m->modules[m->size - 7][8] = fmt_bits[6];  m->modules[8][m->size - 8] = fmt_bits[7];
    m->modules[8][m->size - 7] = fmt_bits[8];  m->modules[8][m->size - 6] = fmt_bits[9];
    m->modules[8][m->size - 5] = fmt_bits[10]; m->modules[8][m->size - 4] = fmt_bits[11];
    m->modules[8][m->size - 3] = fmt_bits[12]; m->modules[8][m->size - 2] = fmt_bits[13];
    m->modules[8][m->size - 1] = fmt_bits[14];

    // --- Version Information Placement (V7+) ---
    if (spec->version >= 7) {
        unsigned int vinfo = spec->version_info;
        for (i = 0; i < 18; i++) {
            int bit = (vinfo >> i) & 1;
            int r = i / 3;
            int c = i % 3;

            // Top-Right Block
            m->modules[r][m->size - 11 + c] = bit;
            // Bottom-Left Block
            m->modules[m->size - 11 + c][r] = bit;
        }
    }
}

// --- Windows Console Renderer ---
void render_win32_console(const QRMatrix *m, int char_pre_dot) {
    int r, b, c;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    WORD WHITE_BG = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
    WORD BLACK_BG = 0;

    int border = 2;
    int total_dim = m->size + (border * 2);
    char dots[3];

    sprintf(dots, "  ");
    if(char_pre_dot == 1)
        dots[1] = 0;

    printf("\n");

    // Top Quiet Zone
    for (r = 0; r < border; r++) {
        SetConsoleTextAttribute(hConsole, WHITE_BG);
        for (c = 0; c < total_dim; c++) printf(dots);
        SetConsoleTextAttribute(hConsole, BLACK_BG);
        printf("\n");
    }

    // Matrix Modules
    for (r = 0; r < m->size; r++) {
        // Left Quiet Zone
        SetConsoleTextAttribute(hConsole, WHITE_BG);
        for (b = 0; b < border; b++) printf(dots);

        for (c = 0; c < m->size; c++) {
            if (m->modules[r][c] == 1) {
                SetConsoleTextAttribute(hConsole, BLACK_BG);
                printf(dots);
            } else {
                SetConsoleTextAttribute(hConsole, WHITE_BG);
                printf(dots);
            }
        }

        // Right Quiet Zone
        SetConsoleTextAttribute(hConsole, WHITE_BG);
        for (b = 0; b < border; b++) printf(dots);

        SetConsoleTextAttribute(hConsole, BLACK_BG);
        printf("\n");
    }

    // Bottom Quiet Zone
    for (r = 0; r < border; r++) {
        SetConsoleTextAttribute(hConsole, WHITE_BG);
        for (c = 0; c < total_dim; c++) printf(dots);
        SetConsoleTextAttribute(hConsole, BLACK_BG);
        printf("\n");
    }

    // Reset console text attribute
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

int main(int argc, char *argv[]) {
    const char *text = (argc > 2) ? argv[2] : "otpauth://totp/ExampleCo:alice@example.com?secret=JBSWY3DPEHPK3PXP&issuer=ExampleCo&algorithm=SHA1&digits=6&period=30";
    int i, len, char_pre_dot = 2;
    QRMatrix matrix;
    const QRVersionSpec *selected_spec = NULL;

    if(argc > 1 && *argv[1]=='1') char_pre_dot = 1;
    len = strlen(text);

    // Auto-select smallest fitting QR version (V1 to V8)
    for (i = 0; i < 8; i++) {
        int max_payload = VERSION_SPECS[i].total_data_bytes - 2; // -2 for mode & len headers
        if (len <= max_payload) {
            selected_spec = &VERSION_SPECS[i];
            break;
        }
    }

    if (!selected_spec) {
        printf("Error: Input length (%d bytes) exceeds Version 8 capacity (152 bytes).\n", len);
        return 1;
    }

    init_gf();

    build_matrix((const unsigned char *)text, len, selected_spec, &matrix);

    printf("QR Version: %d (%dx%d grid)\n", selected_spec->version, selected_spec->size, selected_spec->size);
    printf("Payload (%d bytes): \"%s\"\n", len, text);

    render_win32_console(&matrix, char_pre_dot);

    return 0;
}