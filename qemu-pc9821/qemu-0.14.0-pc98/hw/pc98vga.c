/*
 * QEMU NEC PC-9821 VGA
 *
 * Copyright (c) 2009 TAKEDA, toshiya
 * Copyright (c) 2008 Neko Project 2
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* TODO: TARGET_WORDS_BIGENDIAN */

#include "hw.h"
#include "isa.h"
#include "qdev.h"
#include "pc.h"
#include "console.h"
#include "pixel_ops.h"
#include "sysemu.h"
#include "loader.h"
#include "qemu-timer.h"

#define FONT_FILE_NAME  "pc98font.bin"
#define FONT_FILE_SIZE  0x46800

#define MEMSW_FILE_NAME "pc98memsw.bin"

#define TVRAM_SIZE      0x4000
#define VRAM16_SIZE     0x40000
#define VRAM256_SIZE    0x80000
#define EMS_SIZE        0x10000

#define GDC_BUFFERS     1024
#define GDC_TABLEMAX    0x1000

struct GDCState;
typedef struct GDCState GDCState;

struct EGCState;
typedef struct EGCState EGCState;

struct VGAState;
typedef struct VGAState VGAState;

typedef uint32_t (VGAReadFunc)(VGAState *s, target_phys_addr_t address);
typedef void (VGAWriteFunc)(VGAState *s, target_phys_addr_t address, uint32_t data);

struct GDCState {
    void *vga;

    /* vram access */
    VGAReadFunc *vram_read;
    VGAWriteFunc *vram_write;

    /* address */
    uint32_t address[480][80];

    /* registers */
    int cmdreg;
    uint8_t statreg;

    /* params */
    uint8_t sync[16];
    uint8_t zoom, zr, zw;
    uint8_t ra[16];
    uint8_t cs[3];
    uint8_t pitch;
    uint32_t lad;
    uint8_t vect[11];
    uint32_t ead, dad;
    uint8_t maskl, maskh;
    uint8_t mod;
    uint8_t start;
    uint8_t dirty;

    /* fifo buffers */
    uint8_t params[16];
    int params_count;
    uint8_t data[GDC_BUFFERS];
    int data_count, data_read, data_write;

    /* draw */
    int rt[GDC_TABLEMAX + 1];
    int dx, dy;
    int dir, diff, sl, dc, d, d2, d1, dm;
    uint16_t pattern;
};

typedef union {
    uint8_t b[2];
    uint16_t w;
} egcword_t;

typedef union {
    uint8_t b[4][2];
    uint16_t w[4];
    uint32_t d[2];
    uint64_t q;
} egcquad_t;

struct EGCState {
    void *vga;

    uint16_t access;
    uint16_t fgbg;
    uint16_t ope;
    uint16_t fg;
    egcword_t mask;
    uint16_t bg;
    uint16_t sft;
    uint16_t leng;
    egcquad_t lastvram;
    egcquad_t patreg;
    egcquad_t fgc;
    egcquad_t bgc;
    int func;
    uint32_t remain;
    uint32_t stack;
    uint8_t *inptr;
    int inptr_vmstate;
    uint8_t *outptr;
    int outptr_vmstate;
    egcword_t mask2;
    egcword_t srcmask;
    uint8_t srcbit;
    uint8_t dstbit;
    uint8_t sft8bitl;
    uint8_t sft8bitr;
    uint8_t buf[528];	/* 4096/8 + 4*4 */

    /* vram */
    uint8_t *vram_ptr;
    uint8_t *vram_b;
    uint8_t *vram_r;
    uint8_t *vram_g;
    uint8_t *vram_e;
    egcquad_t vram_src;
    egcquad_t vram_data;
};

struct VGAState {
    /* isa */
    ISADevice dev;
    qemu_irq pic;
    uint32_t irq;

    /* display */
    DisplayState *ds;

    /* vga */
    uint8_t tvram_buffer[480 * 640];
    uint8_t vram0_buffer[480 * 640];
    uint8_t vram1_buffer[480 * 640];
    uint8_t null_buffer[480 * 640];
    int width;
    int height;
    int last_width;
    int last_height;
    uint8_t dirty;
    uint8_t blink;
    uint32_t palette_chr[8];
    uint32_t palette_gfx[256];

    uint8_t font[0x84000];
    uint8_t tvram[TVRAM_SIZE];
    uint8_t vram16[VRAM16_SIZE];
    uint8_t vram256[VRAM256_SIZE];
    uint8_t ems[EMS_SIZE];
    uint8_t *vram16_disp_b;
    uint8_t *vram16_disp_r;
    uint8_t *vram16_disp_g;
    uint8_t *vram16_disp_e;
    uint8_t *vram16_draw_b;
    uint8_t *vram16_draw_r;
    uint8_t *vram16_draw_g;
    uint8_t *vram16_draw_e;
    uint8_t *vram256_disp;
    uint8_t *vram256_draw_0;
    uint8_t *vram256_draw_1;

    struct GDCState gdc_chr;
    struct GDCState gdc_gfx;
    struct EGCState egc;

    uint8_t grcg_mode;
    uint8_t grcg_tile_cnt;
    uint8_t grcg_tile_b[4];
    uint16_t grcg_tile_w[4];

    uint8_t crtv;
    uint8_t pl;
    uint8_t bl;
    uint8_t cl;
    uint8_t ssl;
    uint8_t sur;
    uint8_t sdr;

    uint8_t mode1[8];
    uint8_t mode2[128];
    uint8_t mode3[128];
    uint8_t mode_select;

    uint8_t digipal[4];
    uint8_t anapal[3][256];
    uint8_t anapal_select;

    uint8_t bank_draw;
    uint8_t bank_disp;
    uint8_t bank256_draw_0;
    uint8_t bank256_draw_1;
    uint16_t vram256_bank_0;
    uint16_t vram256_bank_1;
    uint8_t ems_selected;

    uint16_t font_code;
    uint8_t font_line;
    uint32_t cgwindow_addr_low;
    uint32_t cgwindow_addr_high;

    QEMUTimer *vsync_timer;
    int64_t vsync_clock;
};

enum {
    MODE1_ATRSEL        = 0x00,
    MODE1_GRAPHIC       = 0x01,
    MODE1_COLUMN        = 0x02,
    MODE1_FONTSEL       = 0x03,
    MODE1_200LINE       = 0x04,
    MODE1_KAC           = 0x05,
    MODE1_MEMSW         = 0x06,
    MODE1_DISP          = 0x07,
};

enum {
    MODE2_16COLOR       = 0x00,
    MODE2_EGC           = 0x02,
    MODE2_WRITE_MASK    = 0x03,
    MODE2_256COLOR      = 0x10,
    MODE2_480LINE       = 0x34,
};

enum {
    MODE3_WRITE_MASK    = 0x01,
    MODE3_LINE_COLOR    = 0x09,
    MODE3_NPC_COLOR     = 0x0b,
    MODE3_LINE_CONNECT  = 0x0f,
};

enum {
    GRCG_PLANE_0        = 0x01,
    GRCG_PLANE_1        = 0x02,
    GRCG_PLANE_2        = 0x04,
    GRCG_PLANE_3        = 0x08,
    GRCG_PLANE_SEL      = 0x30,
    GRCG_RW_MODE        = 0x40,
    GRCG_CG_MODE        = 0x80,
};

/***********************************************************/
/* NEC uPD7220 GDC (based on Neko Project 2) */

/*
 * PC-9821 CRT sync timing (24.83KHz/400L monitor)
 *
 * VCLOCK = 56.43;
 * VLINES = 440;
*/

#define GDC_VTICKS   18
#define GDC_VSTICKS  2

enum {
    GDC_CMD_RESET    = 0x00,
    GDC_CMD_SYNC     = 0x0e,
    GDC_CMD_SLAVE    = 0x6e,
    GDC_CMD_MASTER   = 0x6f,
    GDC_CMD_START    = 0x6b,
    GDC_CMD_BCTRL    = 0x0c,
    GDC_CMD_ZOOM     = 0x46,
    GDC_CMD_SCROLL   = 0x70,
    GDC_CMD_CSRFORM  = 0x4b,
    GDC_CMD_PITCH    = 0x47,
    GDC_CMD_LPEN     = 0xc0,
    GDC_CMD_VECTW    = 0x4c,
    GDC_CMD_VECTE    = 0x6c,
    GDC_CMD_TEXTW    = 0x78,
    GDC_CMD_TEXTE    = 0x68,
    GDC_CMD_CSRW     = 0x49,
    GDC_CMD_CSRR     = 0xe0,
    GDC_CMD_MASK     = 0x4a,
    GDC_CMD_WRITE    = 0x20,
    GDC_CMD_READ     = 0xa0,
    GDC_CMD_DMAR     = 0xa4,
    GDC_CMD_DMAW     = 0x24,
    /* unknown command (3 params) */
    GDC_CMD_UNK_5A   = 0x5a,
};

enum {
    GDC_STAT_DRDY    = 0x01,
    GDC_STAT_FULL    = 0x02,
    GDC_STAT_EMPTY   = 0x04,
    GDC_STAT_DRAW    = 0x08,
    GDC_STAT_DMA     = 0x10,
    GDC_STAT_VSYNC   = 0x20,
    GDC_STAT_HBLANK  = 0x40,
    GDC_STAT_LPEN    = 0x80,
};

enum {
    GDC_DIRTY_VRAM   = 0x01,
    GDC_DIRTY_START  = 0x02,
    GDC_DIRTY_SCROLL = 0x04,
    GDC_DIRTY_CURSOR = 0x08,
    GDC_DIRTY_GFX    = GDC_DIRTY_VRAM | GDC_DIRTY_SCROLL,
    GDC_DIRTY_CHR    = GDC_DIRTY_GFX | GDC_DIRTY_CURSOR,
};

#define GDC_MULBIT   15
#define GDC_TABLEBIT 12

/* draw command */

static const int gdc_vectdir[16][4] = {
    { 0, 1, 1, 0}, { 1, 1, 1,-1}, { 1, 0, 0,-1}, { 1,-1,-1,-1},
    { 0,-1,-1, 0}, {-1,-1,-1, 1}, {-1, 0, 0, 1}, {-1, 1, 1, 1},
    { 0, 1, 1, 1}, { 1, 1, 1, 0}, { 1, 0, 1,-1}, { 1,-1, 0,-1},
    { 0,-1,-1,-1}, {-1,-1,-1, 0}, {-1, 0,-1, 1}, {-1, 1, 0, 1}
};

static void gdc_draw_pset(GDCState *s, int x, int y)
{
    uint16_t dot = s->pattern & 1;
    uint32_t addr = y * 80 + (x >> 3) + 0x8000;
    uint8_t bit = 0x80 >> (x & 7);
    uint8_t cur = s->vram_read(s->vga, addr);

    s->pattern = (s->pattern >> 1) | (dot << 15);

    switch (s->mod) {
    case 0: /* replace */
        s->vram_write(s->vga, addr, (cur & ~bit) | (dot ? bit : 0));
        break;
    case 1: /* complement */
        s->vram_write(s->vga, addr, (cur & ~bit) | ((cur ^ (dot ? 0xff : 0)) & bit));
        break;
    case 2: /* reset */
        s->vram_write(s->vga, addr, cur & (dot ? ~bit : 0xff));
        break;
    case 3: /* set */
        s->vram_write(s->vga, addr, cur | (dot ? bit : 0));
        break;
    }
    s->dirty |= GDC_DIRTY_VRAM;
}

static void gdc_draw_vectl(GDCState *s)
{
    s->pattern = s->ra[8] | (s->ra[9] << 8);
    if (s->dc) {
        int x = s->dx, y = s->dy;
        int i;

        switch (s->dir) {
        case 0:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x + step, y++);
            }
            break;
        case 1:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x++, y + step);
            }
            break;
        case 2:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x++, y - step);
            }
            break;
        case 3:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x + step, y--);
            }
            break;
        case 4:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x - step, y--);
            }
            break;
        case 5:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x--, y - step);
            }
            break;
        case 6:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x--, y + step);
            }
            break;
        case 7:
            for (i = 0; i <= s->dc; i++) {
                int step = (int)((((s->d1 * i) / s->dc) + 1) >> 1);
                gdc_draw_pset(s, x - step, y++);
            }
            break;
        }
    } else {
        gdc_draw_pset(s, s->dx, s->dy);
    }
}

static void gdc_draw_vectt(GDCState *s)
{
    int vx1 = gdc_vectdir[s->dir][0];
    int vy1 = gdc_vectdir[s->dir][1];
    int vx2 = gdc_vectdir[s->dir][2];
    int vy2 = gdc_vectdir[s->dir][3];
    int muly = s->zw + 1;
    uint16_t draw = s->ra[8] | (s->ra[9] << 8);

    if (s->sl) {
        draw = (draw & 0x0001 ? 0x8000 : 0) | (draw & 0x0002 ? 0x4000 : 0) | 
               (draw & 0x0004 ? 0x2000 : 0) | (draw & 0x0008 ? 0x1000 : 0) | 
               (draw & 0x0010 ? 0x0800 : 0) | (draw & 0x0020 ? 0x0400 : 0) | 
               (draw & 0x0040 ? 0x0200 : 0) | (draw & 0x0080 ? 0x0100 : 0) | 
               (draw & 0x0100 ? 0x0080 : 0) | (draw & 0x0200 ? 0x0040 : 0) | 
               (draw & 0x0400 ? 0x0020 : 0) | (draw & 0x0800 ? 0x0010 : 0) | 
               (draw & 0x1000 ? 0x0008 : 0) | (draw & 0x2000 ? 0x0004 : 0) | 
               (draw & 0x8000 ? 0x0002 : 0) | (draw & 0x8000 ? 0x0001 : 0);
    }
    s->pattern = 0xffff;
    while (muly--) {
        int cx = s->dx;
        int cy = s->dy;
        int xrem = s->d;
        while (xrem--) {
            int mulx = s->zw + 1;
            if (draw & 1) {
                draw >>= 1;
                draw |= 0x8000;
                while (mulx--) {
                    gdc_draw_pset(s, cx, cy);
                    cx += vx1;
                    cy += vy1;
                }
            } else {
                draw >>= 1;
                while (mulx--) {
                    cx += vx1;
                    cy += vy1;
                }
            }
        }
        s->dx += vx2;
        s->dy += vy2;
    }
    s->ead = (s->dx >> 4) + s->dy * s->pitch;
    s->dad = s->dx & 0x0f;
}

static void gdc_draw_vectc(GDCState *s)
{
    int m = (s->d * 10000 + 14141) / 14142;
    int t = (s->dc > m) ? m : s->dc;
    int i;

    s->pattern = s->ra[8] | (s->ra[9] << 8);
    if (m) {
        switch (s->dir) {
        case 0:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx + c), (s->dy + i));
            }
            break;
        case 1:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx + i), (s->dy + c));
            }
            break;
        case 2:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx + i), (s->dy - c));
            }
            break;
        case 3:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx + c), (s->dy - i));
            }
            break;
        case 4:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx - c), (s->dy - i));
            }
            break;
        case 5:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx - i), (s->dy - c));
            }
            break;
        case 6:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx - i), (s->dy + c));
            }
            break;
        case 7:
            for (i = s->dm; i <= t; i++) {
                int c = (s->rt[(i << GDC_TABLEBIT) / m] * s->d);
                c = (c + (1 << (GDC_MULBIT - 1))) >> GDC_MULBIT;
                gdc_draw_pset(s, (s->dx - c), (s->dy + i));
            }
            break;
        }
    } else {
        gdc_draw_pset(s, s->dx, s->dy);
    }
}

static void gdc_draw_vectr(GDCState *s)
{
    int vx1 = gdc_vectdir[s->dir][0];
    int vy1 = gdc_vectdir[s->dir][1];
    int vx2 = gdc_vectdir[s->dir][2];
    int vy2 = gdc_vectdir[s->dir][3];
    int i;

    s->pattern = s->ra[8] | (s->ra[9] << 8);
    for (i = 0; i < s->d; i++) {
        gdc_draw_pset(s, s->dx, s->dy);
        s->dx += vx1;
        s->dy += vy1;
    }
    for (i = 0; i < s->d2; i++) {
        gdc_draw_pset(s, s->dx, s->dy);
        s->dx += vx2;
        s->dy += vy2;
    }
    for (i = 0; i < s->d; i++) {
        gdc_draw_pset(s, s->dx, s->dy);
        s->dx -= vx1;
        s->dy -= vy1;
    }
    for (i = 0; i < s->d2; i++) {
        gdc_draw_pset(s, s->dx, s->dy);
        s->dx -= vx2;
        s->dy -= vy2;
    }
    s->ead = (s->dx >> 4) + s->dy * s->pitch;
    s->dad = s->dx & 0x0f;
}

static void gdc_draw_text(GDCState *s)
{
    int dir = s->dir + (s->sl ? 8 : 0);
    int vx1 = gdc_vectdir[dir][0];
    int vy1 = gdc_vectdir[dir][1];
    int vx2 = gdc_vectdir[dir][2];
    int vy2 = gdc_vectdir[dir][3];
    int sx = s->d;
    int sy = s->dc + 1;
    int index = 15;

    while (sy--) {
        int muly = s->zw + 1;
        while (muly--) {
            int cx = s->dx;
            int cy = s->dy;
            uint8_t bit = s->ra[index];
            int xrem = sx;
            while (xrem--) {
                s->pattern = (bit & 1) ? 0xffff : 0;
                bit = (bit >> 1) | ((bit & 1) ? 0x80 : 0);
                int mulx = s->zw + 1;
                while (mulx--) {
                    gdc_draw_pset(s, cx, cy);
                    cx += vx1;
                    cy += vy1;
                }
            }
            s->dx += vx2;
            s->dy += vy2;
        }
        index = ((index - 1) & 7) | 8;
    }
    s->ead = (s->dx >> 4) + s->dy * s->pitch;
    s->dad = s->dx & 0x0f;
}

/* command sub */

static void gdc_update_vect(GDCState *s)
{
    s->dir = s->vect[0] & 7;
    s->diff = gdc_vectdir[s->dir][0] + gdc_vectdir[s->dir][1] * s->pitch;
    s->sl = s->vect[0] & 0x80;
    s->dc = (s->vect[1] | (s->vect[ 2] << 8)) & 0x3fff;
    s->d  = (s->vect[3] | (s->vect[ 4] << 8)) & 0x3fff;
    s->d2 = (s->vect[5] | (s->vect[ 6] << 8)) & 0x3fff;
    s->d1 = (s->vect[7] | (s->vect[ 8] << 8)) & 0x3fff;
    s->dm = (s->vect[9] | (s->vect[10] << 8)) & 0x3fff;
}

static void gdc_reset_vect(GDCState *s)
{
    s->vect[ 1] = 0;
    s->vect[ 2] = 0;
    s->vect[ 3] = 8;
    s->vect[ 4] = 0;
    s->vect[ 5] = 8;
    s->vect[ 6] = 0;
    s->vect[ 7] = 0;
    s->vect[ 8] = 0;
    s->vect[ 9] = 0;
    s->vect[10] = 0;
    gdc_update_vect(s);
}

static void gdc_write_sub(GDCState *s, uint32_t addr, uint8_t value)
{
    switch (s->mod) {
    case 0: /* replace */
        s->vram_write(s->vga, addr, value);
        break;
    case 1: /* complement */
        s->vram_write(s->vga, addr, s->vram_read(s->vga, addr) ^ value);
        break;
    case 2: /* reset */
        s->vram_write(s->vga, addr, s->vram_read(s->vga, addr) & ~value);
        break;
    case 3: /* set */
        s->vram_write(s->vga, addr, s->vram_read(s->vga, addr) | value);
        break;
    }
    s->dirty |= GDC_DIRTY_VRAM;
}

static uint8_t gdc_read_sub(GDCState *s, uint32_t addr)
{
    return s->vram_read(s->vga, addr);
}

static void gdc_fifo_write(GDCState *s, uint8_t value)
{
    if (s->data_count < GDC_BUFFERS) {
        s->data[(s->data_write++) & (GDC_BUFFERS - 1)] = value;
        s->data_count++;
    }
}

static uint8_t gdc_fifo_read(GDCState *s)
{
    if (s->data_count > 0) {
        uint8_t value = s->data[(s->data_read++) & (GDC_BUFFERS - 1)];
        s->data_count--;
        return value;
    }
    return 0;
}

/* command */

static void gdc_cmd_reset(GDCState *s)
{
    s->sync[6] = 0x90;
    s->sync[7] = 0x01;
    s->zoom = s->zr = s->zw = 0;
    s->ra[0] = s->ra[1] = s->ra[2] = 0;
    s->ra[3] = 0x1e; /*0x19;*/
    s->cs[0] = s->cs[1] = s->cs[2] = 0;
    s->ead = s->dad = 0;
    s->maskl = s->maskh = 0xff;
    s->mod = 0;
    s->start = 0;

    s->params_count = 0;
    s->data_count = s->data_read = s->data_write = 0;

    s->statreg = 0;
    s->cmdreg = -1;
    s->dirty = 0xff;
}

static void gdc_cmd_sync(GDCState *s)
{
    int i;

    for (i = 0; i < 8 && i < s->params_count; i++) {
        s->sync[i] = s->params[i];
    }
    s->cmdreg = -1;
}

static void gdc_cmd_master(GDCState *s)
{
    s->cmdreg = -1;
}

static void gdc_cmd_slave(GDCState *s)
{
    s->cmdreg = -1;
}

static void gdc_cmd_start(GDCState *s)
{
    if (!s->start) {
        s->start = 1;
        s->dirty |= GDC_DIRTY_START;
    }
    s->cmdreg = -1;
}

static void gdc_cmd_stop(GDCState *s)
{
    if (s->start) {
        s->start = 0;
        s->dirty |= GDC_DIRTY_START;
    }
    s->cmdreg = -1;
}

static void gdc_cmd_zoom(GDCState *s)
{
    if (s->params_count > 0) {
        uint8_t tmp = s->params[0];
        s->zr = tmp >> 4;
        s->zw = tmp & 0x0f;
        s->cmdreg = -1;
    }
}

static void gdc_cmd_scroll(GDCState *s)
{
    if (s->params_count > 0) {
        if (s->ra[s->cmdreg & 0x0f] != s->params[0]) {
            s->ra[s->cmdreg & 0x0f] = s->params[0];
            s->dirty |= GDC_DIRTY_SCROLL;
        }
        if (s->cmdreg < 0x7f) {
            s->cmdreg++;
            s->params_count = 0;
        } else {
            s->cmdreg = -1;
        }
    }
}

static void gdc_cmd_csrform(GDCState *s)
{
    int i;

    for (i = 0; i < s->params_count; i++) {
        if (s->cs[i] != s->params[i]) {
            s->cs[i] = s->params[i];
            s->dirty |= GDC_DIRTY_CURSOR;
        }
    }
    if (s->params_count > 2) {
        s->cmdreg = -1;
    }
}

static void gdc_cmd_pitch(GDCState *s)
{
    if (s->params_count > 0) {
        s->pitch = s->params[0];
        s->cmdreg = -1;
    }
}

static void gdc_cmd_lpen(GDCState *s)
{
    gdc_fifo_write(s, s->lad & 0xff);
    gdc_fifo_write(s, (s->lad >> 8) & 0xff);
    gdc_fifo_write(s, (s->lad >> 16) & 0xff);
    s->cmdreg = -1;
}

static void gdc_cmd_vectw(GDCState *s)
{
    int i;

    for (i = 0; i < 11 && i < s->params_count; i++) {
        s->vect[i] = s->params[i];
    }
    gdc_update_vect(s);
    s->cmdreg = -1;
}

static void gdc_cmd_vecte(GDCState *s)
{
    s->dx = ((s->ead % s->pitch) << 4) | (s->dad & 0x0f);
    s->dy = s->ead / s->pitch;
    if (!(s->vect[0] & 0x78)) {
        s->pattern = s->ra[8] | (s->ra[9] << 8);
        gdc_draw_pset(s, s->dx, s->dy);
    }
    if (s->vect[0] & 0x08) {
        gdc_draw_vectl(s);
    }
    if (s->vect[0] & 0x10) {
        gdc_draw_vectt(s);
    }
    if (s->vect[0] & 0x20) {
        gdc_draw_vectc(s);
    }
    if (s->vect[0] & 0x40) {
        gdc_draw_vectr(s);
    }
    gdc_reset_vect(s);
    s->statreg |= GDC_STAT_DRAW;
    s->cmdreg = -1;
}

static void gdc_cmd_texte(GDCState *s)
{
    s->dx = ((s->ead % s->pitch) << 4) | (s->dad & 0x0f);
    s->dy = s->ead / s->pitch;
    if (!(s->vect[0] & 0x78)) {
        s->pattern = s->ra[8] | (s->ra[9] << 8);
        gdc_draw_pset(s, s->dx, s->dy);
    }
    if (s->vect[0] & 0x08) {
        gdc_draw_vectl(s);
    }
    if (s->vect[0] & 0x10) {
        gdc_draw_text(s);
    }
    if (s->vect[0] & 0x20) {
        gdc_draw_vectc(s);
    }
    if (s->vect[0] & 0x40) {
        gdc_draw_vectr(s);
    }
    gdc_reset_vect(s);
    s->statreg |= GDC_STAT_DRAW;
    s->cmdreg = -1;
}

static void gdc_cmd_csrw(GDCState *s)
{
    if (s->params_count > 0) {
        s->ead = s->params[0];
        if (s->params_count > 1) {
            s->ead |= s->params[1] << 8;
            if (s->params_count > 2) {
                s->ead |= s->params[2] << 16;
                s->cmdreg = -1;
            }
        }
        s->dad = (s->ead >> 20) & 0x0f;
        s->ead &= 0x3ffff;
        s->dirty |= GDC_DIRTY_CURSOR;
    }
}

static void gdc_cmd_csrr(GDCState *s)
{
    gdc_fifo_write(s, s->ead & 0xff);
    gdc_fifo_write(s, (s->ead >> 8) & 0xff);
    gdc_fifo_write(s, (s->ead >> 16) & 0x03);
    gdc_fifo_write(s, s->dad & 0xff);
    gdc_fifo_write(s, (s->dad >> 8) & 0xff);
    s->cmdreg = -1;
}

static void gdc_cmd_mask(GDCState *s)
{
    if (s->params_count > 1) {
        s->maskl = s->params[0];
        s->maskh = s->params[1];
        s->cmdreg = -1;
    }
}

static void gdc_cmd_write(GDCState *s)
{
    uint8_t l, h;
    int i;

    s->mod = s->cmdreg & 3;
    switch (s->cmdreg & 0x18) {
    case 0x00: /* low and high */
        if (s->params_count > 1) {
            l = s->params[0] & s->maskl;
            h = s->params[1] & s->maskh;
            for (i = 0; i < s->dc + 1; i++) {
                gdc_write_sub(s, s->ead * 2 + 0, l);
                gdc_write_sub(s, s->ead * 2 + 1, h);
                s->ead += s->diff;
            }
            gdc_reset_vect(s);
            s->cmdreg = -1;
        }
        break;
    case 0x10: /* low byte */
        if (s->params_count > 0) {
            l = s->params[0] & s->maskl;
            for (i = 0; i < s->dc + 1; i++) {
                gdc_write_sub(s, s->ead * 2 + 0, l);
                s->ead += s->diff;
            }
            gdc_reset_vect(s);
            s->cmdreg = -1;
        }
        break;
    case 0x18: /* high byte */
        if (s->params_count > 0) {
            h = s->params[0] & s->maskh;
            for (i = 0; i < s->dc + 1; i++) {
                gdc_write_sub(s, s->ead * 2 + 1, h);
                s->ead += s->diff;
            }
            gdc_reset_vect(s);
            s->cmdreg = -1;
        }
        break;
    default:    /* invalid */
        s->cmdreg = -1;
        break;
    }
}

static void gdc_cmd_read(GDCState *s)
{
    int i;

    s->mod = s->cmdreg & 3;
    switch (s->cmdreg & 0x18) {
    case 0x00: /* low and high */
        for (i = 0; i < s->dc; i++) {
            gdc_fifo_write(s, gdc_read_sub(s, s->ead * 2 + 0));
            gdc_fifo_write(s, gdc_read_sub(s, s->ead * 2 + 1));
            s->ead += s->diff;
        }
        break;
    case 0x10: /* low byte */
        for (i = 0; i < s->dc; i++) {
            gdc_fifo_write(s, gdc_read_sub(s, s->ead * 2 + 0));
            s->ead += s->diff;
        }
        break;
    case 0x18: /* high byte */
        for (i = 0; i < s->dc; i++) {
            gdc_fifo_write(s, gdc_read_sub(s, s->ead * 2 + 1));
            s->ead += s->diff;
        }
        break;
    default: /* invalid */
        break;
    }
    gdc_reset_vect(s);
    s->cmdreg = -1;
}

static void gdc_cmd_dmaw(GDCState *s)
{
    s->mod = s->cmdreg & 3;
    gdc_reset_vect(s);
//    s->statreg |= GDC_STAT_DMA;
    s->cmdreg = -1;
}

static void gdc_cmd_dmar(GDCState *s)
{
    s->mod = s->cmdreg & 3;
    gdc_reset_vect(s);
//    s->statreg |= GDC_STAT_DMA;
    s->cmdreg = -1;
}

static void gdc_cmd_unk_5a(GDCState *s)
{
    if (s->params_count > 2) {
        s->cmdreg = -1;
    }
}

static void gdc_check_cmd(GDCState *s)
{
    switch (s->cmdreg) {
    case GDC_CMD_RESET:
        gdc_cmd_reset(s);
        break;
    case GDC_CMD_SYNC + 0:
    case GDC_CMD_SYNC + 1:
        if (s->params_count > 7) {
            gdc_cmd_sync(s);
        }
        break;
    case GDC_CMD_MASTER:
        gdc_cmd_master(s);
        break;
    case GDC_CMD_SLAVE:
        gdc_cmd_slave(s);
        break;
    case GDC_CMD_START:
        gdc_cmd_start(s);
        break;
    case GDC_CMD_BCTRL + 0:
        gdc_cmd_stop(s);
        break;
    case GDC_CMD_BCTRL + 1:
        gdc_cmd_start(s);
        break;
    case GDC_CMD_ZOOM:
        gdc_cmd_zoom(s);
        break;
    case GDC_CMD_SCROLL + 0:
    case GDC_CMD_SCROLL + 1:
    case GDC_CMD_SCROLL + 2:
    case GDC_CMD_SCROLL + 3:
    case GDC_CMD_SCROLL + 4:
    case GDC_CMD_SCROLL + 5:
    case GDC_CMD_SCROLL + 6:
    case GDC_CMD_SCROLL + 7:
    case GDC_CMD_TEXTW + 0:
    case GDC_CMD_TEXTW + 1:
    case GDC_CMD_TEXTW + 2:
    case GDC_CMD_TEXTW + 3:
    case GDC_CMD_TEXTW + 4:
    case GDC_CMD_TEXTW + 5:
    case GDC_CMD_TEXTW + 6:
    case GDC_CMD_TEXTW + 7:
        gdc_cmd_scroll(s);
        break;
    case GDC_CMD_CSRFORM:
        gdc_cmd_csrform(s);
        break;
    case GDC_CMD_PITCH:
        gdc_cmd_pitch(s);
        break;
    case GDC_CMD_LPEN:
        gdc_cmd_lpen(s);
        break;
    case GDC_CMD_VECTW:
        if (s->params_count > 10) {
            gdc_cmd_vectw(s);
        }
        break;
    case GDC_CMD_VECTE:
        gdc_cmd_vecte(s);
        break;
    case GDC_CMD_TEXTE:
        gdc_cmd_texte(s);
        break;
    case GDC_CMD_CSRW:
        gdc_cmd_csrw(s);
        break;
    case GDC_CMD_CSRR:
        gdc_cmd_csrr(s);
        break;
    case GDC_CMD_MASK:
        gdc_cmd_mask(s);
        break;
    case GDC_CMD_WRITE + 0x00:
    case GDC_CMD_WRITE + 0x01:
    case GDC_CMD_WRITE + 0x02:
    case GDC_CMD_WRITE + 0x03:
    case GDC_CMD_WRITE + 0x08:
    case GDC_CMD_WRITE + 0x09:
    case GDC_CMD_WRITE + 0x0a:
    case GDC_CMD_WRITE + 0x0b:
    case GDC_CMD_WRITE + 0x10:
    case GDC_CMD_WRITE + 0x11:
    case GDC_CMD_WRITE + 0x12:
    case GDC_CMD_WRITE + 0x13:
    case GDC_CMD_WRITE + 0x18:
    case GDC_CMD_WRITE + 0x19:
    case GDC_CMD_WRITE + 0x1a:
    case GDC_CMD_WRITE + 0x1b:
        gdc_cmd_write(s);
        break;
    case GDC_CMD_READ + 0x00:
    case GDC_CMD_READ + 0x01:
    case GDC_CMD_READ + 0x02:
    case GDC_CMD_READ + 0x03:
    case GDC_CMD_READ + 0x08:
    case GDC_CMD_READ + 0x09:
    case GDC_CMD_READ + 0x0a:
    case GDC_CMD_READ + 0x0b:
    case GDC_CMD_READ + 0x10:
    case GDC_CMD_READ + 0x11:
    case GDC_CMD_READ + 0x12:
    case GDC_CMD_READ + 0x13:
    case GDC_CMD_READ + 0x18:
    case GDC_CMD_READ + 0x19:
    case GDC_CMD_READ + 0x1a:
    case GDC_CMD_READ + 0x1b:
        gdc_cmd_read(s);
        break;
    case GDC_CMD_DMAW + 0x00:
    case GDC_CMD_DMAW + 0x01:
    case GDC_CMD_DMAW + 0x02:
    case GDC_CMD_DMAW + 0x03:
    case GDC_CMD_DMAW + 0x08:
    case GDC_CMD_DMAW + 0x09:
    case GDC_CMD_DMAW + 0x0a:
    case GDC_CMD_DMAW + 0x0b:
    case GDC_CMD_DMAW + 0x10:
    case GDC_CMD_DMAW + 0x11:
    case GDC_CMD_DMAW + 0x12:
    case GDC_CMD_DMAW + 0x13:
    case GDC_CMD_DMAW + 0x18:
    case GDC_CMD_DMAW + 0x19:
    case GDC_CMD_DMAW + 0x1a:
    case GDC_CMD_DMAW + 0x1b:
        gdc_cmd_dmaw(s);
        break;
    case GDC_CMD_DMAR + 0x00:
    case GDC_CMD_DMAR + 0x01:
    case GDC_CMD_DMAR + 0x02:
    case GDC_CMD_DMAR + 0x03:
    case GDC_CMD_DMAR + 0x08:
    case GDC_CMD_DMAR + 0x09:
    case GDC_CMD_DMAR + 0x0a:
    case GDC_CMD_DMAR + 0x0b:
    case GDC_CMD_DMAR + 0x10:
    case GDC_CMD_DMAR + 0x11:
    case GDC_CMD_DMAR + 0x12:
    case GDC_CMD_DMAR + 0x13:
    case GDC_CMD_DMAR + 0x18:
    case GDC_CMD_DMAR + 0x19:
    case GDC_CMD_DMAR + 0x1a:
    case GDC_CMD_DMAR + 0x1b:
        gdc_cmd_dmar(s);
        break;
    case GDC_CMD_UNK_5A:
        gdc_cmd_unk_5a(s);
        break;
    }
}

static void gdc_process_cmd(GDCState *s)
{
    switch (s->cmdreg) {
    case GDC_CMD_RESET:
        gdc_cmd_reset(s);
        break;
    case GDC_CMD_SYNC + 0:
    case GDC_CMD_SYNC + 1:
        gdc_cmd_sync(s);
        break;
    case GDC_CMD_SCROLL + 0:
    case GDC_CMD_SCROLL + 1:
    case GDC_CMD_SCROLL + 2:
    case GDC_CMD_SCROLL + 3:
    case GDC_CMD_SCROLL + 4:
    case GDC_CMD_SCROLL + 5:
    case GDC_CMD_SCROLL + 6:
    case GDC_CMD_SCROLL + 7:
    case GDC_CMD_TEXTW + 0:
    case GDC_CMD_TEXTW + 1:
    case GDC_CMD_TEXTW + 2:
    case GDC_CMD_TEXTW + 3:
    case GDC_CMD_TEXTW + 4:
    case GDC_CMD_TEXTW + 5:
    case GDC_CMD_TEXTW + 6:
    case GDC_CMD_TEXTW + 7:
        gdc_cmd_scroll(s);
        break;
    case GDC_CMD_VECTW:
        gdc_cmd_vectw(s);
        break;
    case GDC_CMD_CSRW:
        gdc_cmd_csrw(s);
        break;
    }
}

/* i/o */

static void gdc_param_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x60(chr), 0xa0(gfx) */
    GDCState *s = opaque;

    if (s->cmdreg != -1) {
        if (s->params_count < 16) {
            s->params[s->params_count++] = (uint8_t)(value & 0xff);
        }
        gdc_check_cmd(s);
        if (s->cmdreg == -1) {
            s->params_count = 0;
        }
    }
}

static uint32_t gdc_statreg_read(void *opaque, uint32_t addr)
{
    /* ioport 0x60(chr), 0xa0(gfx) */
    GDCState *s = opaque;
    VGAState *v = s->vga;
    uint32_t value = s->statreg;
    int64_t vticks = qemu_get_clock(rt_clock) - v->vsync_clock;

    if (vticks < GDC_VSTICKS) {
        value |= GDC_STAT_VSYNC;
    }
    /*if (s->params_count == 0) {*/
        value |= GDC_STAT_EMPTY;
    /*}*/
    if (s->params_count == 16) {
        value |= GDC_STAT_FULL;
    }
    if (s->data_count > 0) {
        value |= GDC_STAT_DRDY;
    }
    s->statreg &= ~(GDC_STAT_DMA | GDC_STAT_DRAW);
    /* toggle hblank bit */
    s->statreg ^= GDC_STAT_HBLANK;
    return value;
}

static void gdc_cmdreg_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x62(chr), 0xa2(gfx) */
    GDCState *s = opaque;

    if (s->cmdreg != -1) {
        gdc_process_cmd(s);
    }
    s->cmdreg = (uint8_t)(value & 0xff);
    s->params_count = 0;
    gdc_check_cmd(s);
}

static uint32_t gdc_data_read(void *opaque, uint32_t addr)
{
    /* ioport 0x62(chr), 0xa2(gfx) */
    GDCState *s = opaque;

    return gdc_fifo_read(s);
}

/* display */

static uint32_t *gdc_get_address(GDCState *s, int ofs, uint32_t mask)
{
    if (s->dirty & GDC_DIRTY_SCROLL) {
        int x, y, ytop = 0, i;
        uint32_t ra, sad;
        int len;

        for (i = 0; i < 4; i++) {
            ra  = s->ra[4 * i + 0];
            ra |= s->ra[4 * i + 1] << 8;
            ra |= s->ra[4 * i + 2] << 16;
            ra |= s->ra[4 * i + 3] << 24;
            sad = (ra << 1) & mask;
            len = (ra >> 20) & 0x3ff;

            for (y = ytop; y < (ytop + len) && y < 480; y++) {
                for (x = 0; x < 80; x++) {
                    s->address[y][x] = sad;
                    sad = (sad + ofs) & mask;
                }
            }
            ytop += len;
        }
        s->dirty &= ~GDC_DIRTY_SCROLL;
    }
    return s->address[0];
}

static void gdc_get_cursor_address(GDCState *s, uint32_t mask,
                                   uint32_t *addr, int *top, int *bottom)
{
    VGAState *v = s->vga;

    if ((s->cs[0] & 0x80) && ((s->cs[1] & 0x20) || !(v->blink & 0x20))) {
        *addr = (s->ead << 1) & mask;
        *top = s->cs[1] & 0x1f;
        *bottom = s->cs[2] >> 3;
    } else {
        *addr = -1;
    }
}

/* interface */

static int gdc_post_load(void *opaque)
{
    GDCState *s = opaque;

    /* force update address */
    s->dirty = 0xff;
    return 0;
}

static const VMStateDescription vmstate_gdc = {
    .name = "pc98-gdc",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField []) {
        VMSTATE_INT32(cmdreg, struct GDCState),
        VMSTATE_UINT8(statreg, struct GDCState),
        VMSTATE_UINT8_ARRAY(sync, GDCState, 16),
        VMSTATE_UINT8(zoom, GDCState),
        VMSTATE_UINT8(zr, GDCState),
        VMSTATE_UINT8(zw, GDCState),
        VMSTATE_UINT8_ARRAY(ra, GDCState, 16),
        VMSTATE_UINT8_ARRAY(cs, GDCState, 3),
        VMSTATE_UINT8(pitch, GDCState),
        VMSTATE_UINT32(lad, GDCState),
        VMSTATE_UINT8_ARRAY(vect, GDCState, 11),
        VMSTATE_UINT32(ead, GDCState),
        VMSTATE_UINT32(dad, GDCState),
        VMSTATE_UINT8(maskl, GDCState),
        VMSTATE_UINT8(maskh, GDCState),
        VMSTATE_UINT8(mod, GDCState),
        VMSTATE_UINT8(start, GDCState),
        VMSTATE_UINT8(dirty, GDCState),
        VMSTATE_UINT8_ARRAY(params, GDCState, 16),
        VMSTATE_INT32(params_count, GDCState),
        VMSTATE_UINT8_ARRAY(data, GDCState, GDC_BUFFERS),
        VMSTATE_INT32(data_count, GDCState),
        VMSTATE_INT32(data_read, GDCState),
        VMSTATE_INT32(data_write, GDCState),
        VMSTATE_END_OF_LIST()
    }
};

static void gdc_reset(GDCState *s)
{
    gdc_cmd_reset(s);
}

static void gdc_init(GDCState *s, void *vga,
                     VGAReadFunc *vram_read, VGAWriteFunc *vram_write)
{
    int i;

    for (i = 0; i <= GDC_TABLEMAX; i++) {
        s->rt[i] = (int)((double)(1 << GDC_MULBIT) * (1 - sqrt(1 - pow((0.70710678118654 * i) / GDC_TABLEMAX, 2))));
    }
    s->vga = vga;
    s->vram_read = vram_read;
    s->vram_write = vram_write;
}

/***********************************************************/
/* EGC (based on Neko Project 2) */

/* shift sub */

static const uint8_t egc_bytemask_u0[64] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
    0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01,
    0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x07, 0x03, 0x01,
    0xf0, 0x78, 0x3c, 0x1e, 0x0f, 0x07, 0x03, 0x01,
    0xf8, 0x7c, 0x3e, 0x1f, 0x0f, 0x07, 0x03, 0x01,
    0xfc, 0x7e, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01,
    0xfe, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01,
    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
};
static const uint8_t egc_bytemask_u1[8] =  {
    0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};
static const uint8_t egc_bytemask_d0[64] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80,
    0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xc0, 0x80,
    0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0xe0, 0xc0, 0x80,
    0x1f, 0x3e, 0x7c, 0xf8, 0xf0, 0xe0, 0xc0, 0x80,
    0x3f, 0x7e, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80,
    0x7f, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80,
    0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80
};
static const uint8_t egc_bytemask_d1[8] = {
    0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

static void egc_shift(EGCState *s)
{
    uint8_t src8, dst8;

    s->remain = (s->leng & 0xfff) + 1;
    s->func = (s->sft >> 12) & 1;
    if (!s->func) {
        s->inptr = s->buf;
        s->outptr = s->buf;
    } else {
        s->inptr = s->buf + 4096 / 8 + 3;
        s->outptr = s->buf + 4096 / 8 + 3;
    }
    s->srcbit = s->sft & 0x0f;
    s->dstbit = (s->sft >> 4) & 0x0f;

    src8 = s->srcbit & 0x07;
    dst8 = s->dstbit & 0x07;
    if (src8 < dst8) {
        s->func += 2;
        s->sft8bitr = dst8 - src8;
        s->sft8bitl = 8 - s->sft8bitr;
    }
    else if (src8 > dst8) {
        s->func += 4;
        s->sft8bitl = src8 - dst8;
        s->sft8bitr = 8 - s->sft8bitl;
    }
    s->stack = 0;
}

static void egc_sftb_upn_sub(EGCState *s, uint32_t ext)
{
    if (s->dstbit >= 8) {
        s->dstbit -= 8;
        s->srcmask.b[ext] = 0;
        return;
    }
    if (s->dstbit) {
        if ((s->dstbit + s->remain) >= 8) {
            s->srcmask.b[ext] = egc_bytemask_u0[s->dstbit + (7 * 8)];
            s->remain -= (8 - s->dstbit);
            s->dstbit = 0;
        } else {
            s->srcmask.b[ext] = egc_bytemask_u0[s->dstbit + (s->remain - 1) * 8];
            s->remain = 0;
            s->dstbit = 0;
        }
    } else {
        if (s->remain >= 8) {
            s->remain -= 8;
        } else {
            s->srcmask.b[ext] = egc_bytemask_u1[s->remain - 1];
            s->remain = 0;
        }
    }
    s->vram_src.b[0][ext] = s->outptr[0];
    s->vram_src.b[1][ext] = s->outptr[4];
    s->vram_src.b[2][ext] = s->outptr[8];
    s->vram_src.b[3][ext] = s->outptr[12];
    s->outptr++;
}

static void egc_sftb_dnn_sub(EGCState *s, uint32_t ext)
{
    if (s->dstbit >= 8) {
        s->dstbit -= 8;
        s->srcmask.b[ext] = 0;
        return;
    }
    if (s->dstbit) {
        if ((s->dstbit + s->remain) >= 8) {
            s->srcmask.b[ext] = egc_bytemask_d0[s->dstbit + (7 * 8)];
            s->remain -= (8 - s->dstbit);
            s->dstbit = 0;
        } else {
            s->srcmask.b[ext] = egc_bytemask_d0[s->dstbit + (s->remain - 1) * 8];
            s->remain = 0;
            s->dstbit = 0;
        }
    } else {
        if (s->remain >= 8) {
            s->remain -= 8;
        } else {
            s->srcmask.b[ext] = egc_bytemask_d1[s->remain - 1];
            s->remain = 0;
        }
    }
    s->vram_src.b[0][ext] = s->outptr[0];
    s->vram_src.b[1][ext] = s->outptr[4];
    s->vram_src.b[2][ext] = s->outptr[8];
    s->vram_src.b[3][ext] = s->outptr[12];
    s->outptr--;
}

static void egc_sftb_upr_sub(EGCState *s, uint32_t ext)
{
    if (s->dstbit >= 8) {
        s->dstbit -= 8;
        s->srcmask.b[ext] = 0;
        return;
    }
    if (s->dstbit) {
        if ((s->dstbit + s->remain) >= 8) {
            s->srcmask.b[ext] = egc_bytemask_u0[s->dstbit + (7 * 8)];
            s->remain -= (8 - s->dstbit);
        } else {
            s->srcmask.b[ext] = egc_bytemask_u0[s->dstbit + (s->remain - 1) * 8];
            s->remain = 0;
        }
        s->dstbit = 0;
        s->vram_src.b[0][ext] = (s->outptr[0] >> s->sft8bitr);
        s->vram_src.b[1][ext] = (s->outptr[4] >> s->sft8bitr);
        s->vram_src.b[2][ext] = (s->outptr[8] >> s->sft8bitr);
        s->vram_src.b[3][ext] = (s->outptr[12] >> s->sft8bitr);
    } else {
        if (s->remain >= 8) {
            s->remain -= 8;
        } else {
            s->srcmask.b[ext] = egc_bytemask_u1[s->remain - 1];
            s->remain = 0;
        }
        s->vram_src.b[0][ext] = (s->outptr[0] << s->sft8bitl) | (s->outptr[1] >> s->sft8bitr);
        s->vram_src.b[1][ext] = (s->outptr[4] << s->sft8bitl) | (s->outptr[5] >> s->sft8bitr);
        s->vram_src.b[2][ext] = (s->outptr[8] << s->sft8bitl) | (s->outptr[9] >> s->sft8bitr);
        s->vram_src.b[3][ext] = (s->outptr[12] << s->sft8bitl) | (s->outptr[13] >> s->sft8bitr);
        s->outptr++;
    }
}

static void egc_sftb_dnr_sub(EGCState *s, uint32_t ext)
{
    if (s->dstbit >= 8) {
        s->dstbit -= 8;
        s->srcmask.b[ext] = 0;
        return;
    }
    if (s->dstbit) {
        if ((s->dstbit + s->remain) >= 8) {
            s->srcmask.b[ext] = egc_bytemask_d0[s->dstbit + (7 * 8)];
            s->remain -= (8 - s->dstbit);
        } else {
            s->srcmask.b[ext] = egc_bytemask_d0[s->dstbit + (s->remain - 1) * 8];
            s->remain = 0;
        }
        s->dstbit = 0;
        s->vram_src.b[0][ext] = (s->outptr[0] << s->sft8bitr);
        s->vram_src.b[1][ext] = (s->outptr[4] << s->sft8bitr);
        s->vram_src.b[2][ext] = (s->outptr[8] << s->sft8bitr);
        s->vram_src.b[3][ext] = (s->outptr[12] << s->sft8bitr);
    } else {
        if (s->remain >= 8) {
            s->remain -= 8;
        } else {
            s->srcmask.b[ext] = egc_bytemask_d1[s->remain - 1];
            s->remain = 0;
        }
        s->outptr--;
        s->vram_src.b[0][ext] = (s->outptr[1] >> s->sft8bitl) | (s->outptr[0] << s->sft8bitr);
        s->vram_src.b[1][ext] = (s->outptr[5] >> s->sft8bitl) | (s->outptr[4] << s->sft8bitr);
        s->vram_src.b[2][ext] = (s->outptr[9] >> s->sft8bitl) | (s->outptr[8] << s->sft8bitr);
        s->vram_src.b[3][ext] = (s->outptr[13] >> s->sft8bitl) | (s->outptr[12] << s->sft8bitr);
    }
}

static void egc_sftb_upl_sub(EGCState *s, uint32_t ext)
{
    if (s->dstbit >= 8) {
        s->dstbit -= 8;
        s->srcmask.b[ext] = 0;
        return;
    }
    if (s->dstbit) {
        if ((s->dstbit + s->remain) >= 8) {
            s->srcmask.b[ext] = egc_bytemask_u0[s->dstbit + (7 * 8)];
            s->remain -= (8 - s->dstbit);
            s->dstbit = 0;
        } else {
            s->srcmask.b[ext] = egc_bytemask_u0[s->dstbit + (s->remain - 1) * 8];
            s->remain = 0;
            s->dstbit = 0;
        }
    } else {
        if (s->remain >= 8) {
            s->remain -= 8;
        } else {
            s->srcmask.b[ext] = egc_bytemask_u1[s->remain - 1];
            s->remain = 0;
        }
    }
    s->vram_src.b[0][ext] = (s->outptr[0] << s->sft8bitl) | (s->outptr[1] >> s->sft8bitr);
    s->vram_src.b[1][ext] = (s->outptr[4] << s->sft8bitl) | (s->outptr[5] >> s->sft8bitr);
    s->vram_src.b[2][ext] = (s->outptr[8] << s->sft8bitl) | (s->outptr[9] >> s->sft8bitr);
    s->vram_src.b[3][ext] = (s->outptr[12] << s->sft8bitl) | (s->outptr[13] >> s->sft8bitr);
    s->outptr++;
}

static void egc_sftb_dnl_sub(EGCState *s, uint32_t ext)
{
    if (s->dstbit >= 8) {
        s->dstbit -= 8;
        s->srcmask.b[ext] = 0;
        return;
    }
    if (s->dstbit) {
        if ((s->dstbit + s->remain) >= 8) {
            s->srcmask.b[ext] = egc_bytemask_d0[s->dstbit + (7 * 8)];
            s->remain -= (8 - s->dstbit);
            s->dstbit = 0;
        } else {
            s->srcmask.b[ext] = egc_bytemask_d0[s->dstbit + (s->remain - 1) * 8];
            s->remain = 0;
            s->dstbit = 0;
        }
    } else {
        if (s->remain >= 8) {
            s->remain -= 8;
        } else {
            s->srcmask.b[ext] = egc_bytemask_d1[s->remain - 1];
            s->remain = 0;
        }
    }
    s->outptr--;
    s->vram_src.b[0][ext] = (s->outptr[1] >> s->sft8bitl) | (s->outptr[0] << s->sft8bitr);
    s->vram_src.b[1][ext] = (s->outptr[5] >> s->sft8bitl) | (s->outptr[4] << s->sft8bitr);
    s->vram_src.b[2][ext] = (s->outptr[9] >> s->sft8bitl) | (s->outptr[8] << s->sft8bitr);
    s->vram_src.b[3][ext] = (s->outptr[13] >> s->sft8bitl) | (s->outptr[12] << s->sft8bitr);
}

static void egc_sftb_upn0(EGCState *s, uint32_t ext)
{
    if (s->stack < (uint32_t)(8 - s->dstbit)) {
        s->srcmask.b[ext] = 0;
        return;
    }
    s->stack -= (8 - s->dstbit);
    egc_sftb_upn_sub(s, ext);
    if (!s->remain) {
        egc_shift(s);
    }
}

static void egc_sftw_upn0(EGCState *s)
{
    if (s->stack < (uint32_t)(16 - s->dstbit)) {
        s->srcmask.w = 0;
        return;
    }
    s->stack -= (16 - s->dstbit);
    egc_sftb_upn_sub(s, 0);
    if (s->remain) {
        egc_sftb_upn_sub(s, 1);
        if (s->remain) {
            return;
        }
    } else {
        s->srcmask.b[1] = 0;
    }
    egc_shift(s);
}

static void egc_sftb_dnn0(EGCState *s, uint32_t ext)
{
    if (s->stack < (uint32_t)(8 - s->dstbit)) {
        s->srcmask.b[ext] = 0;
        return;
    }
    s->stack -= (8 - s->dstbit);
    egc_sftb_dnn_sub(s, ext);
    if (!s->remain) {
        egc_shift(s);
    }
}

static void egc_sftw_dnn0(EGCState *s)
{
    if (s->stack < (uint32_t)(16 - s->dstbit)) {
        s->srcmask.w = 0;
        return;
    }
    s->stack -= (16 - s->dstbit);
    egc_sftb_dnn_sub(s, 1);
    if (s->remain) {
        egc_sftb_dnn_sub(s, 0);
        if (s->remain) {
            return;
        }
    } else {
        s->srcmask.b[0] = 0;
    }
    egc_shift(s);
}

static void egc_sftb_upr0(EGCState *s, uint32_t ext)
{
    if (s->stack < (uint32_t)(8 - s->dstbit)) {
        s->srcmask.b[ext] = 0;
        return;
    }
    s->stack -= (8 - s->dstbit);
    egc_sftb_upr_sub(s, ext);
    if (!s->remain) {
        egc_shift(s);
    }
}

static void egc_sftw_upr0(EGCState *s)
{
    if (s->stack < (uint32_t)(16 - s->dstbit)) {
        s->srcmask.w = 0;
        return;
    }
    s->stack -= (16 - s->dstbit);
    egc_sftb_upr_sub(s, 0);
    if (s->remain) {
        egc_sftb_upr_sub(s, 1);
        if (s->remain) {
            return;
        }
    } else {
        s->srcmask.b[1] = 0;
    }
    egc_shift(s);
}

static void egc_sftb_dnr0(EGCState *s, uint32_t ext)
{
    if (s->stack < (uint32_t)(8 - s->dstbit)) {
        s->srcmask.b[ext] = 0;
        return;
    }
    s->stack -= (8 - s->dstbit);
    egc_sftb_dnr_sub(s, ext);
    if (!s->remain) {
        egc_shift(s);
    }
}

static void egc_sftw_dnr0(EGCState *s)
{
    if (s->stack < (uint32_t)(16 - s->dstbit)) {
        s->srcmask.w = 0;
        return;
    }
    s->stack -= (16 - s->dstbit);
    egc_sftb_dnr_sub(s, 1);
    if (s->remain) {
        egc_sftb_dnr_sub(s, 0);
        if (s->remain) {
            return;
        }
    } else {
        s->srcmask.b[0] = 0;
    }
    egc_shift(s);
}

static void egc_sftb_upl0(EGCState *s, uint32_t ext)
{
    if (s->stack < (uint32_t)(8 - s->dstbit)) {
        s->srcmask.b[ext] = 0;
        return;
    }
    s->stack -= (8 - s->dstbit);
    egc_sftb_upl_sub(s, ext);
    if (!s->remain) {
        egc_shift(s);
    }
}

static void egc_sftw_upl0(EGCState *s)
{
    if (s->stack < (uint32_t)(16 - s->dstbit)) {
        s->srcmask.w = 0;
        return;
    }
    s->stack -= (16 - s->dstbit);
    egc_sftb_upl_sub(s, 0);
    if (s->remain) {
        egc_sftb_upl_sub(s, 1);
        if (s->remain) {
            return;
        }
    } else {
        s->srcmask.b[1] = 0;
    }
    egc_shift(s);
}

static void egc_sftb_dnl0(EGCState *s, uint32_t ext)
{
    if (s->stack < (uint32_t)(8 - s->dstbit)) {
        s->srcmask.b[ext] = 0;
        return;
    }
    s->stack -= (8 - s->dstbit);
    egc_sftb_dnl_sub(s, ext);
    if (!s->remain) {
        egc_shift(s);
    }
}

static void egc_sftw_dnl0(EGCState *s)
{
    if (s->stack < (uint32_t)(16 - s->dstbit)) {
        s->srcmask.w = 0;
        return;
    }
    s->stack -= (16 - s->dstbit);
    egc_sftb_dnl_sub(s, 1);
    if (s->remain) {
        egc_sftb_dnl_sub(s, 0);
        if (s->remain) {
            return;
        }
    } else {
        s->srcmask.b[0] = 0;
    }
    egc_shift(s);
}

/* shift */

typedef void (*PC98EGCSFTB)(EGCState *s, uint32_t ext);
typedef void (*PC98EGCSFTW)(EGCState *s);

static const PC98EGCSFTB egc_sftb[6] = {
    egc_sftb_upn0, egc_sftb_dnn0,
    egc_sftb_upr0, egc_sftb_dnr0,
    egc_sftb_upl0, egc_sftb_dnl0
};
static const PC98EGCSFTW egc_sftw[6] = {
    egc_sftw_upn0, egc_sftw_dnn0,
    egc_sftw_upr0, egc_sftw_dnr0,
    egc_sftw_upl0, egc_sftw_dnl0
};

static void egc_shiftinput_byte(EGCState *s, uint32_t ext)
{
    if (s->stack <= 16) {
        if (s->srcbit >= 8) {
            s->srcbit -= 8;
        } else {
            s->stack += (8 - s->srcbit);
            s->srcbit = 0;
        }
        if (!(s->sft & 0x1000)) {
            s->inptr++;
        } else {
            s->inptr--;
        }
    }
    s->srcmask.b[ext] = 0xff;
    (*egc_sftb[s->func])(s, ext);
}

static void egc_shiftinput_incw(EGCState *s)
{
    if (s->stack <= 16) {
        s->inptr += 2;
        if (s->srcbit >= 8) {
            s->outptr++;
        }
        s->stack += (16 - s->srcbit);
        s->srcbit = 0;
    }
    s->srcmask.w = 0xffff;
    (*egc_sftw[s->func])(s);
}

static void egc_shiftinput_decw(EGCState *s)
{
    if (s->stack <= 16) {
        s->inptr -= 2;
        if (s->srcbit >= 8) {
            s->outptr--;
        }
        s->stack += (16 - s->srcbit);
        s->srcbit = 0;
    }
    s->srcmask.w = 0xffff;
    (*egc_sftw[s->func])(s);
}

/* operation */

#define PC98EGC_OPE_SHIFTB \
    do { \
        if (s->ope & 0x400) { \
            s->inptr[ 0] = (uint8_t)value; \
            s->inptr[ 4] = (uint8_t)value; \
            s->inptr[ 8] = (uint8_t)value; \
            s->inptr[12] = (uint8_t)value; \
            egc_shiftinput_byte(s, addr & 1); \
        } \
    } while (0)

#define PC98EGC_OPE_SHIFTW \
    do { \
        if (s->ope & 0x400) { \
            if (!(s->sft & 0x1000)) { \
                s->inptr[ 0] = (uint8_t)value; \
                s->inptr[ 1] = (uint8_t)(value >> 8); \
                s->inptr[ 4] = (uint8_t)value; \
                s->inptr[ 5] = (uint8_t)(value >> 8); \
                s->inptr[ 8] = (uint8_t)value; \
                s->inptr[ 9] = (uint8_t)(value >> 8); \
                s->inptr[12] = (uint8_t)value; \
                s->inptr[13] = (uint8_t)(value >> 8); \
                egc_shiftinput_incw(s); \
            } else { \
                s->inptr[-1] = (uint8_t)value; \
                s->inptr[ 0] = (uint8_t)(value >> 8); \
                s->inptr[ 3] = (uint8_t)value; \
                s->inptr[ 4] = (uint8_t)(value >> 8); \
                s->inptr[ 7] = (uint8_t)value; \
                s->inptr[ 8] = (uint8_t)(value >> 8); \
                s->inptr[11] = (uint8_t)value; \
                s->inptr[12] = (uint8_t)(value >> 8); \
                egc_shiftinput_decw(s); \
            }  \
        } \
    } while (0)

static uint64_t egc_ope_00(EGCState *s, uint8_t ope, uint32_t addr)
{
    return 0;
}

static uint64_t egc_ope_0f(EGCState *s, uint8_t ope, uint32_t addr)
{
    s->vram_data.d[0] = ~s->vram_src.d[0];
    s->vram_data.d[1] = ~s->vram_src.d[1];
    return s->vram_data.q;
}

static uint64_t egc_ope_c0(EGCState *s, uint8_t ope, uint32_t addr)
{
    egcquad_t dst;

    dst.w[0] = *(uint16_t *)(&s->vram_b[addr]);
    dst.w[1] = *(uint16_t *)(&s->vram_r[addr]);
    dst.w[2] = *(uint16_t *)(&s->vram_g[addr]);
    dst.w[3] = *(uint16_t *)(&s->vram_e[addr]);
    s->vram_data.d[0] = (s->vram_src.d[0] & dst.d[0]);
    s->vram_data.d[1] = (s->vram_src.d[1] & dst.d[1]);
    return s->vram_data.q;
}

static uint64_t egc_ope_f0(EGCState *s, uint8_t ope, uint32_t addr)
{
    return s->vram_src.q;
}

static uint64_t egc_ope_fc(EGCState *s, uint8_t ope, uint32_t addr)
{
    egcquad_t dst;

    dst.w[0] = *(uint16_t *)(&s->vram_b[addr]);
    dst.w[1] = *(uint16_t *)(&s->vram_r[addr]);
    dst.w[2] = *(uint16_t *)(&s->vram_g[addr]);
    dst.w[3] = *(uint16_t *)(&s->vram_e[addr]);
    s->vram_data.d[0] = s->vram_src.d[0];
    s->vram_data.d[0] |= ((~s->vram_src.d[0]) & dst.d[0]);
    s->vram_data.d[1] = s->vram_src.d[1];
    s->vram_data.d[1] |= ((~s->vram_src.d[1]) & dst.d[1]);
    return s->vram_data.q;
}

static uint64_t egc_ope_ff(EGCState *s, uint8_t ope, uint32_t addr)
{
    return ~0;
}

static uint64_t egc_ope_nd(EGCState *s, uint8_t ope, uint32_t addr)
{
    egcquad_t pat;

    switch(s->fgbg & 0x6000) {
    case 0x2000:
        pat.d[0] = s->bgc.d[0];
        pat.d[1] = s->bgc.d[1];
        break;
    case 0x4000:
        pat.d[0] = s->fgc.d[0];
        pat.d[1] = s->fgc.d[1];
        break;
    default:
        if ((s->ope & 0x0300) == 0x0100) {
            pat.d[0] = s->vram_src.d[0];
            pat.d[1] = s->vram_src.d[1];
        } else {
            pat.d[0] = s->patreg.d[0];
            pat.d[1] = s->patreg.d[1];
        }
        break;
    }
    s->vram_data.d[0] = 0;
    s->vram_data.d[1] = 0;
    if (ope & 0x80) {
        s->vram_data.d[0] |= (pat.d[0] & s->vram_src.d[0]);
        s->vram_data.d[1] |= (pat.d[1] & s->vram_src.d[1]);
    }
    if (ope & 0x40) {
        s->vram_data.d[0] |= ((~pat.d[0]) & s->vram_src.d[0]);
        s->vram_data.d[1] |= ((~pat.d[1]) & s->vram_src.d[1]);
    }
    if (ope & 0x08) {
        s->vram_data.d[0] |= (pat.d[0] & (~s->vram_src.d[0]));
        s->vram_data.d[1] |= (pat.d[1] & (~s->vram_src.d[1]));
    }
    if (ope & 0x04) {
        s->vram_data.d[0] |= ((~pat.d[0]) & (~s->vram_src.d[0]));
        s->vram_data.d[1] |= ((~pat.d[1]) & (~s->vram_src.d[1]));
    }
    return s->vram_data.q;
}

static uint64_t egc_ope_np(EGCState *s, uint8_t ope, uint32_t addr)
{
    egcquad_t dst;

    dst.w[0] = *(uint16_t *)(&s->vram_b[addr]);
    dst.w[1] = *(uint16_t *)(&s->vram_r[addr]);
    dst.w[2] = *(uint16_t *)(&s->vram_g[addr]);
    dst.w[3] = *(uint16_t *)(&s->vram_e[addr]);

    s->vram_data.d[0] = 0;
    s->vram_data.d[1] = 0;
    if (ope & 0x80) {
        s->vram_data.d[0] |= (s->vram_src.d[0] & dst.d[0]);
        s->vram_data.d[1] |= (s->vram_src.d[1] & dst.d[1]);
    }
    if (ope & 0x20) {
        s->vram_data.d[0] |= (s->vram_src.d[0] & (~dst.d[0]));
        s->vram_data.d[1] |= (s->vram_src.d[1] & (~dst.d[1]));
    }
    if (ope & 0x08) {
        s->vram_data.d[0] |= ((~s->vram_src.d[0]) & dst.d[0]);
        s->vram_data.d[1] |= ((~s->vram_src.d[1]) & dst.d[1]);
    }
    if (ope & 0x02) {
        s->vram_data.d[0] |= ((~s->vram_src.d[0]) & (~dst.d[0]));
        s->vram_data.d[1] |= ((~s->vram_src.d[1]) & (~dst.d[1]));
    }
    return s->vram_data.q;
}

static uint64_t egc_ope_xx(EGCState *s, uint8_t ope, uint32_t addr)
{
    egcquad_t pat;
    egcquad_t dst;

    switch(s->fgbg & 0x6000) {
    case 0x2000:
        pat.d[0] = s->bgc.d[0];
        pat.d[1] = s->bgc.d[1];
        break;
    case 0x4000:
        pat.d[0] = s->fgc.d[0];
        pat.d[1] = s->fgc.d[1];
        break;
    default:
        if ((s->ope & 0x0300) == 0x0100) {
            pat.d[0] = s->vram_src.d[0];
            pat.d[1] = s->vram_src.d[1];
        } else {
            pat.d[0] = s->patreg.d[0];
            pat.d[1] = s->patreg.d[1];
        }
        break;
    }
    dst.w[0] = *(uint16_t *)(&s->vram_b[addr]);
    dst.w[1] = *(uint16_t *)(&s->vram_r[addr]);
    dst.w[2] = *(uint16_t *)(&s->vram_g[addr]);
    dst.w[3] = *(uint16_t *)(&s->vram_e[addr]);

    s->vram_data.d[0] = 0;
    s->vram_data.d[1] = 0;
    if (ope & 0x80) {
        s->vram_data.d[0] |= (pat.d[0] & s->vram_src.d[0] & dst.d[0]);
        s->vram_data.d[1] |= (pat.d[1] & s->vram_src.d[1] & dst.d[1]);
    }
    if (ope & 0x40) {
        s->vram_data.d[0] |= ((~pat.d[0]) & s->vram_src.d[0] & dst.d[0]);
        s->vram_data.d[1] |= ((~pat.d[1]) & s->vram_src.d[1] & dst.d[1]);
    }
    if (ope & 0x20) {
        s->vram_data.d[0] |= (pat.d[0] & s->vram_src.d[0] & (~dst.d[0]));
        s->vram_data.d[1] |= (pat.d[1] & s->vram_src.d[1] & (~dst.d[1]));
    }
    if (ope & 0x10) {
        s->vram_data.d[0] |= ((~pat.d[0]) & s->vram_src.d[0] & (~dst.d[0]));
        s->vram_data.d[1] |= ((~pat.d[1]) & s->vram_src.d[1] & (~dst.d[1]));
    }
    if (ope & 0x08) {
        s->vram_data.d[0] |= (pat.d[0] & (~s->vram_src.d[0]) & dst.d[0]);
        s->vram_data.d[1] |= (pat.d[1] & (~s->vram_src.d[1]) & dst.d[1]);
    }
    if (ope & 0x04) {
        s->vram_data.d[0] |= ((~pat.d[0]) & (~s->vram_src.d[0]) & dst.d[0]);
        s->vram_data.d[1] |= ((~pat.d[1]) & (~s->vram_src.d[1]) & dst.d[1]);
    }
    if (ope & 0x02) {
        s->vram_data.d[0] |= (pat.d[0] & (~s->vram_src.d[0]) & (~dst.d[0]));
        s->vram_data.d[1] |= (pat.d[1] & (~s->vram_src.d[1]) & (~dst.d[1]));
    }
    if (ope & 0x01) {
        s->vram_data.d[0] |= ((~pat.d[0]) & (~s->vram_src.d[0]) & (~dst.d[0]));
        s->vram_data.d[1] |= ((~pat.d[1]) & (~s->vram_src.d[1]) & (~dst.d[1]));
    }
    return s->vram_data.q;
}

typedef uint64_t (*PC98EGCOPEFN)(EGCState *s, uint8_t ope, uint32_t addr);

static const PC98EGCOPEFN egc_opefn[256] = {
    egc_ope_00, egc_ope_xx, egc_ope_xx, egc_ope_np,
    egc_ope_xx, egc_ope_nd, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_nd, egc_ope_xx,
    egc_ope_np, egc_ope_xx, egc_ope_xx, egc_ope_0f,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_np, egc_ope_xx, egc_ope_xx, egc_ope_np,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_np, egc_ope_xx, egc_ope_xx, egc_ope_np,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_nd, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_nd, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_nd, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_nd,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_nd, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_nd, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_nd, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_nd,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_c0, egc_ope_xx, egc_ope_xx, egc_ope_np,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_np, egc_ope_xx, egc_ope_xx, egc_ope_np,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_xx, egc_ope_xx,
    egc_ope_f0, egc_ope_xx, egc_ope_xx, egc_ope_np,
    egc_ope_xx, egc_ope_nd, egc_ope_xx, egc_ope_xx,
    egc_ope_xx, egc_ope_xx, egc_ope_nd, egc_ope_xx,
    egc_ope_fc, egc_ope_xx, egc_ope_xx, egc_ope_ff
};

static uint64_t egc_opeb(EGCState *s, uint32_t addr, uint8_t value)
{
    uint32_t tmp;

    s->mask2.w = s->mask.w;
    switch(s->ope & 0x1800) {
    case 0x0800:
        PC98EGC_OPE_SHIFTB;
        s->mask2.w &= s->srcmask.w;
        tmp = s->ope & 0xff;
        return (*egc_opefn[tmp])(s, (uint8_t)tmp, addr & (~1));
    case 0x1000:
        switch(s->fgbg & 0x6000) {
        case 0x2000:
            return s->bgc.q;
        case 0x4000:
            return s->fgc.q;
        default:
            PC98EGC_OPE_SHIFTB;
            s->mask2.w &= s->srcmask.w;
            return s->vram_src.q;
        }
        break;
    default:
        tmp = value & 0xff;
        tmp = tmp | (tmp << 8);
        s->vram_data.w[0] = (uint16_t)tmp;
        s->vram_data.w[1] = (uint16_t)tmp;
        s->vram_data.w[2] = (uint16_t)tmp;
        s->vram_data.w[3] = (uint16_t)tmp;
        return s->vram_data.q;
    }
}

static uint64_t egc_opew(EGCState *s, uint32_t addr, uint16_t value)
{
    uint32_t tmp;

    s->mask2.w = s->mask.w;
    switch(s->ope & 0x1800) {
    case 0x0800:
        PC98EGC_OPE_SHIFTW;
        s->mask2.w &= s->srcmask.w;
        tmp = s->ope & 0xff;
        return (*egc_opefn[tmp])(s, (uint8_t)tmp, addr);
    case 0x1000:
        switch(s->fgbg & 0x6000) {
        case 0x2000:
            return s->bgc.q;
        case 0x4000:
            return s->fgc.q;
        default:
            PC98EGC_OPE_SHIFTW;
            s->mask2.w &= s->srcmask.w;
            return s->vram_src.q;
        }
        break;
    default:
#ifdef WORDS_BIGENDIAN
        value = ((value >> 8) & 0xff) | ((value & 0xff) << 8);
#endif
        s->vram_data.w[0] = (uint16_t)value;
        s->vram_data.w[1] = (uint16_t)value;
        s->vram_data.w[2] = (uint16_t)value;
        s->vram_data.w[3] = (uint16_t)value;
        return s->vram_data.q;
    }
}

/* memory */

static const uint16_t egc_maskword[16][4] = {
    {0x0000, 0x0000, 0x0000, 0x0000}, {0xffff, 0x0000, 0x0000, 0x0000},
    {0x0000, 0xffff, 0x0000, 0x0000}, {0xffff, 0xffff, 0x0000, 0x0000},
    {0x0000, 0x0000, 0xffff, 0x0000}, {0xffff, 0x0000, 0xffff, 0x0000},
    {0x0000, 0xffff, 0xffff, 0x0000}, {0xffff, 0xffff, 0xffff, 0x0000},
    {0x0000, 0x0000, 0x0000, 0xffff}, {0xffff, 0x0000, 0x0000, 0xffff},
    {0x0000, 0xffff, 0x0000, 0xffff}, {0xffff, 0xffff, 0x0000, 0xffff},
    {0x0000, 0x0000, 0xffff, 0xffff}, {0xffff, 0x0000, 0xffff, 0xffff},
    {0x0000, 0xffff, 0xffff, 0xffff}, {0xffff, 0xffff, 0xffff, 0xffff}
};

static uint32_t egc_mem_readb(EGCState *s, uint32_t addr1)
{
    uint32_t addr = addr1 & 0x7fff;
    uint32_t ext = addr1 & 1;

    s->lastvram.b[0][ext] = s->vram_b[addr];
    s->lastvram.b[1][ext] = s->vram_r[addr];
    s->lastvram.b[2][ext] = s->vram_g[addr];
    s->lastvram.b[3][ext] = s->vram_e[addr];

    if (!(s->ope & 0x400)) {
        s->inptr[0] = s->lastvram.b[0][ext];
        s->inptr[4] = s->lastvram.b[1][ext];
        s->inptr[8] = s->lastvram.b[2][ext];
        s->inptr[12] = s->lastvram.b[3][ext];
        egc_shiftinput_byte(s, ext);
    }
    if ((s->ope & 0x0300) == 0x0100) {
        s->patreg.b[0][ext] = s->vram_b[addr];
        s->patreg.b[1][ext] = s->vram_r[addr];
        s->patreg.b[2][ext] = s->vram_g[addr];
        s->patreg.b[3][ext] = s->vram_e[addr];
    }
    if (!(s->ope & 0x2000)) {
        int pl = (s->fgbg >> 8) & 3;
        if (!(s->ope & 0x400)) {
            return s->vram_src.b[pl][ext];
        } else {
            return s->vram_ptr[addr | (0x8000 * pl)];
        }
    }
    return s->vram_ptr[addr1];
}

static uint32_t egc_mem_readw(EGCState *s, uint32_t addr1)
{
    uint32_t addr = addr1 & 0x7fff;

    if (!(addr & 1)) {
        s->lastvram.w[0] = *(uint16_t *)(&s->vram_b[addr]);
        s->lastvram.w[1] = *(uint16_t *)(&s->vram_r[addr]);
        s->lastvram.w[2] = *(uint16_t *)(&s->vram_g[addr]);
        s->lastvram.w[3] = *(uint16_t *)(&s->vram_e[addr]);

        if (!(s->ope & 0x400)) {
            if (!(s->sft & 0x1000)) {
                s->inptr[ 0] = s->lastvram.b[0][0];
                s->inptr[ 1] = s->lastvram.b[0][1];
                s->inptr[ 4] = s->lastvram.b[1][0];
                s->inptr[ 5] = s->lastvram.b[1][1];
                s->inptr[ 8] = s->lastvram.b[2][0];
                s->inptr[ 9] = s->lastvram.b[2][1];
                s->inptr[12] = s->lastvram.b[3][0];
                s->inptr[13] = s->lastvram.b[3][1];
                egc_shiftinput_incw(s);
            } else {
                s->inptr[-1] = s->lastvram.b[0][0];
                s->inptr[ 0] = s->lastvram.b[0][1];
                s->inptr[ 3] = s->lastvram.b[1][0];
                s->inptr[ 4] = s->lastvram.b[1][1];
                s->inptr[ 7] = s->lastvram.b[2][0];
                s->inptr[ 8] = s->lastvram.b[2][1];
                s->inptr[11] = s->lastvram.b[3][0];
                s->inptr[12] = s->lastvram.b[3][1];
                egc_shiftinput_decw(s);
            }
        }
        if ((s->ope & 0x0300) == 0x0100) {
            s->patreg.d[0] = s->lastvram.d[0];
            s->patreg.d[1] = s->lastvram.d[1];
        }
        if (!(s->ope & 0x2000)) {
            int pl = (s->fgbg >> 8) & 3;
            if (!(s->ope & 0x400)) {
                return s->vram_src.w[pl];
            } else {
                return *(uint16_t *)(&s->vram_ptr[addr | (0x8000 * pl)]);
            }
        }
        return *(uint16_t *)(&s->vram_ptr[addr1]);
    } else if (!(s->sft & 0x1000)) {
        uint16_t value = egc_mem_readb(s, addr1);
        value |= egc_mem_readb(s, addr1 + 1) << 8;
        return value;
    } else {
        uint16_t value = egc_mem_readb(s, addr1) << 8;
        value |= egc_mem_readb(s, addr1 + 1);
        return value;
    }
}

static void egc_mem_writeb(EGCState *s, uint32_t addr1, uint8_t value)
{
    uint32_t addr = addr1 & 0x7fff;
    uint32_t ext = addr1 & 1;
    egcquad_t data;

    if ((s->ope & 0x0300) == 0x0200) {
        s->patreg.b[0][ext] = s->vram_b[addr];
        s->patreg.b[1][ext] = s->vram_r[addr];
        s->patreg.b[2][ext] = s->vram_g[addr];
        s->patreg.b[3][ext] = s->vram_e[addr];
    }
    data.q = egc_opeb(s, addr, value);
    if (s->mask2.b[ext]) {
        if (!(s->access & 1)) {
            s->vram_b[addr] &= ~s->mask2.b[ext];
            s->vram_b[addr] |= data.b[0][ext] & s->mask2.b[ext];
        }
        if (!(s->access & 2)) {
            s->vram_r[addr] &= ~s->mask2.b[ext];
            s->vram_r[addr] |= data.b[1][ext] & s->mask2.b[ext];
        }
        if (!(s->access & 4)) {
            s->vram_g[addr] &= ~s->mask2.b[ext];
            s->vram_g[addr] |= data.b[2][ext] & s->mask2.b[ext];
        }
        if (!(s->access & 8)) {
            s->vram_e[addr] &= ~s->mask2.b[ext];
            s->vram_e[addr] |= data.b[3][ext] & s->mask2.b[ext];
        }
    }
}

static void egc_mem_writew(EGCState *s, uint32_t addr1, uint16_t value)
{
    uint32_t addr = addr1 & 0x7fff;
    egcquad_t data;

    if (!(addr & 1)) {
        if ((s->ope & 0x0300) == 0x0200) {
            s->patreg.w[0] = *(uint16_t *)(&s->vram_b[addr]);
            s->patreg.w[1] = *(uint16_t *)(&s->vram_r[addr]);
            s->patreg.w[2] = *(uint16_t *)(&s->vram_g[addr]);
            s->patreg.w[3] = *(uint16_t *)(&s->vram_e[addr]);
        }
        data.q = egc_opew(s, addr, value);
        if (s->mask2.w) {
            if (!(s->access & 1)) {
                *(uint16_t *)(&s->vram_b[addr]) &= ~s->mask2.w;
                *(uint16_t *)(&s->vram_b[addr]) |= data.w[0] & s->mask2.w;
            }
            if (!(s->access & 2)) {
                *(uint16_t *)(&s->vram_r[addr]) &= ~s->mask2.w;
                *(uint16_t *)(&s->vram_r[addr]) |= data.w[1] & s->mask2.w;
            }
            if (!(s->access & 4)) {
                *(uint16_t *)(&s->vram_g[addr]) &= ~s->mask2.w;
                *(uint16_t *)(&s->vram_g[addr]) |= data.w[2] & s->mask2.w;
            }
            if (!(s->access & 8)) {
                *(uint16_t *)(&s->vram_e[addr]) &= ~s->mask2.w;
                *(uint16_t *)(&s->vram_e[addr]) |= data.w[3] & s->mask2.w;
            }
        }
    } else if (!(s->sft & 0x1000)) {
        egc_mem_writeb(s, addr1, (uint8_t)value);
        egc_mem_writeb(s, addr1 + 1, (uint8_t)(value >> 8));
    } else {
        egc_mem_writeb(s, addr1, (uint8_t)(value >> 8));
        egc_mem_writeb(s, addr1 + 1, (uint8_t)value);
    }
}

/* i/o */

static void egc_ioport_writeb(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x4a0 - 0x4af */
    EGCState *s = opaque;
    VGAState *v = s->vga;

    if (!((v->grcg_mode & GRCG_CG_MODE) && v->mode2[MODE2_EGC])) {
        return;
    }
    switch(addr) {
    case 0x4a0:
        s->access &= 0xff00;
        s->access |= value;
        break;
    case 0x4a1:
        s->access &= 0x00ff;
        s->access |= value << 8;
        break;
    case 0x4a2:
        s->fgbg &= 0xff00;
        s->fgbg |= value;
        break;
    case 0x4a3:
        s->fgbg &= 0x00ff;
        s->fgbg |= value << 8;
        break;
    case 0x4a4:
        s->ope &= 0xff00;
        s->ope |= value;
        break;
    case 0x4a5:
        s->ope &= 0x00ff;
        s->ope |= value << 8;
        break;
    case 0x4a6:
        s->fg &= 0xff00;
        s->fg |= value;
        s->fgc.d[0] = *(uint32_t *)(egc_maskword[value & 0x0f] + 0);
        s->fgc.d[1] = *(uint32_t *)(egc_maskword[value & 0x0f] + 2);
        break;
    case 0x4a7:
        s->fg &= 0x00ff;
        s->fg |= value << 8;
        break;
    case 0x4a8:
        if (!(s->fgbg & 0x6000)) {
            s->mask.b[0] = value;
        }
        break;
    case 0x4a9:
        if (!(s->fgbg & 0x6000)) {
            s->mask.b[1] = value;
        }
        break;
    case 0x4aa:
        s->bg &= 0xff00;
        s->bg |= value;
        s->bgc.d[0] = *(uint32_t *)(egc_maskword[value & 0x0f] + 0);
        s->bgc.d[1] = *(uint32_t *)(egc_maskword[value & 0x0f] + 2);
        break;
    case 0x4ab:
        s->bg &= 0x00ff;
        s->bg |= value << 8;
        break;
    case 0x4ac:
        s->sft &= 0xff00;
        s->sft |= value;
        egc_shift(s);
        s->srcmask.w = 0xffff;
        break;
    case 0x4ad:
        s->sft &= 0x00ff;
        s->sft |= value << 8;
        egc_shift(s);
        s->srcmask.w = 0xffff;
        break;
    case 0x4ae:
        s->leng &= 0xff00;
        s->leng |= value;
        egc_shift(s);
        s->srcmask.w = 0xffff;
        break;
    case 0x4af:
        s->leng &= 0x00ff;
        s->leng |= value << 8;
        egc_shift(s);
        s->srcmask.w = 0xffff;
        break;
    }
}

static void egc_ioport_writew(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x4a0 - 0x4af */
    EGCState *s = opaque;
    VGAState *v = s->vga;

    if (!((v->grcg_mode & GRCG_CG_MODE) && v->mode2[MODE2_EGC])) {
        return;
    }
    switch(addr) {
    case 0x4a0:
        s->access = value;
        break;
    case 0x4a2:
        s->fgbg = value;
        break;
    case 0x4a4:
        s->ope = value;
        break;
    case 0x4a6:
        s->fg = value;
        s->fgc.d[0] = *(uint32_t *)(egc_maskword[value & 0x0f] + 0);
        s->fgc.d[1] = *(uint32_t *)(egc_maskword[value & 0x0f] + 2);
        break;
    case 0x4a8:
        if (!(s->fgbg & 0x6000)) {
            s->mask.w = value;
        }
        break;
    case 0x4aa:
        s->bg = value;
        s->bgc.d[0] = *(uint32_t *)(egc_maskword[value & 0x0f] + 0);
        s->bgc.d[1] = *(uint32_t *)(egc_maskword[value & 0x0f] + 2);
        break;
    case 0x4ac:
        s->sft = value;
        egc_shift(s);
        s->srcmask.w = 0xffff;
        break;
    case 0x4ae:
        s->leng = value;
        egc_shift(s);
        s->srcmask.w = 0xffff;
        break;
    }
}

/* interface */

static void egc_set_vram(EGCState *s, uint8_t *vram_ptr)
{
    s->vram_ptr = vram_ptr;
    s->vram_b = vram_ptr + 0x00000;
    s->vram_r = vram_ptr + 0x08000;
    s->vram_g = vram_ptr + 0x10000;
    s->vram_e = vram_ptr + 0x18000;
}

static void egc_pre_save(void *opaque)
{
    EGCState *s = opaque;

    s->inptr_vmstate = s->inptr - s->buf;
    s->outptr_vmstate = s->outptr - s->buf;
}

static int egc_post_load(void *opaque)
{
    EGCState *s = opaque;

    s->inptr = s->buf + s->inptr_vmstate;
    s->outptr = s->buf + s->outptr_vmstate;
    return 0;
}

static const VMStateDescription vmstate_egc = {
    .name = "pc98-egc",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField []) {
        VMSTATE_UINT16(access, EGCState),
        VMSTATE_UINT16(fgbg, EGCState),
        VMSTATE_UINT16(ope, EGCState),
        VMSTATE_UINT16(fg, EGCState),
        VMSTATE_UINT16(mask.w, EGCState),
        VMSTATE_UINT16(bg, EGCState),
        VMSTATE_UINT16(sft, EGCState),
        VMSTATE_UINT16(leng, EGCState),
        VMSTATE_UINT64(lastvram.q, EGCState),
        VMSTATE_UINT64(patreg.q, EGCState),
        VMSTATE_UINT64(fgc.q, EGCState),
        VMSTATE_UINT64(bgc.q, EGCState),
        VMSTATE_INT32(func, EGCState),
        VMSTATE_UINT32(remain, EGCState),
        VMSTATE_UINT32(stack, EGCState),
        VMSTATE_INT32(inptr_vmstate, EGCState),
        VMSTATE_INT32(outptr_vmstate, EGCState),
        VMSTATE_UINT16(mask2.w, EGCState),
        VMSTATE_UINT16(srcmask.w, EGCState),
        VMSTATE_UINT8(srcbit, EGCState),
        VMSTATE_UINT8(dstbit, EGCState),
        VMSTATE_UINT8(sft8bitl, EGCState),
        VMSTATE_UINT8(sft8bitr, EGCState),
        VMSTATE_UINT8_ARRAY(buf, EGCState, 528),
        VMSTATE_END_OF_LIST()
    }
};

static void egc_reset(EGCState *s)
{
    VGAState *v = s->vga;

    memset(s, 0, sizeof(EGCState));
    s->vga = v;
    s->access = 0xfff0;
    s->fgbg = 0x00ff;
    s->mask.w = 0xffff;
    s->leng = 0x000f;
    egc_shift(s);
    s->srcmask.w = 0xffff;
}

static void egc_init(EGCState *s, void *vga)
{
    s->vga = vga;
}

/***********************************************************/
/* NEC PC-9821 VGA */

enum {
    PALETTE_G           = 0,
    PALETTE_R           = 1,
    PALETTE_B           = 2,
};

enum {
    DIRTY_TVRAM         = 0x01,
    DIRTY_VRAM0         = 0x02,
    DIRTY_VRAM1         = 0x04,
    DIRTY_PALETTE       = 0x10,
    DIRTY_DISPLAY       = 0x80,
};

enum {
    ATTR_ST = 0x01,
    ATTR_BL = 0x02,
    ATTR_RV = 0x04,
    ATTR_UL = 0x08,
    ATTR_VL = 0x10,
    ATTR_COL = 0xe0,
};

/* vsync */

static void vsync_timer_handler(void *opaque)
{
    VGAState *s = opaque;
    uint8_t prev_blink = s->blink++;

    /* blink */
    if ((prev_blink & 0x20) != (s->blink & 0x20)) {
        s->dirty |= DIRTY_TVRAM;
    }

    /* vsync irq */
    if (s->crtv) {
        qemu_irq_raise(s->pic);
        qemu_irq_lower(s->pic);
        s->crtv = 0;
    }

    /* set next timer */
    s->vsync_clock = qemu_get_clock(rt_clock);
    qemu_mod_timer(s->vsync_timer, s->vsync_clock + GDC_VTICKS);
}

static void vsync_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x64 */
    VGAState *s = opaque;

    s->crtv = 1;
}

/* crtc */

static void crtc_pl_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x70 */
    VGAState *s = opaque;

    if (s->pl != value) {
        s->pl = value;
        s->dirty |= DIRTY_TVRAM;
    }
}

static void crtc_bl_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x72 */
    VGAState *s = opaque;

    if (s->bl != value) {
        s->bl = value;
        s->dirty |= DIRTY_TVRAM;
    }
}

static void crtc_cl_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x74 */
    VGAState *s = opaque;

    if (s->cl != value) {
        s->cl = value;
        s->dirty |= DIRTY_TVRAM;
    }
}

static void crtc_ssl_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x76 */
    VGAState *s = opaque;

    if (s->ssl != value) {
        s->ssl = value;
        s->dirty |= DIRTY_TVRAM;
    }
}

static void crtc_sur_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x78 */
    VGAState *s = opaque;

    if (s->sur != value) {
        s->sur = value;
        s->dirty |= DIRTY_TVRAM;
    }
}

static void crtc_sdr_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x7a */
    VGAState *s = opaque;

    if (s->sdr != value) {
        s->sdr = value;
        s->dirty |= DIRTY_TVRAM;
    }
}

/* grcg */

static void grcg_mode_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x7c */
    VGAState *s = opaque;

    s->grcg_mode = value;
    s->grcg_tile_cnt = 0;
}

static void grcg_tile_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x7e */
    VGAState *s = opaque;

    s->grcg_tile_b[s->grcg_tile_cnt] = value;
    s->grcg_tile_w[s->grcg_tile_cnt] = (value & 0xff) | (value << 8);
    s->grcg_tile_cnt = (s->grcg_tile_cnt + 1) & 3;
}

static uint32_t grcg_mem_readb(VGAState *s, uint32_t addr1)
{
    uint32_t addr = addr1 & 0x7fff;

    if (s->grcg_mode & GRCG_RW_MODE) {
        /* invalid */
        return s->vram16_draw_b[addr];
    } else {
        /* TCR */
        uint8_t value = 0;
        if (!(s->grcg_mode & GRCG_PLANE_0)) {
            value |= s->vram16_draw_b[addr] ^ s->grcg_tile_b[0];
        }
        if (!(s->grcg_mode & GRCG_PLANE_1)) {
            value |= s->vram16_draw_r[addr] ^ s->grcg_tile_b[1];
        }
        if (!(s->grcg_mode & GRCG_PLANE_2)) {
            value |= s->vram16_draw_g[addr] ^ s->grcg_tile_b[2];
        }
        if (!(s->grcg_mode & GRCG_PLANE_3)) {
            value |= s->vram16_draw_e[addr] ^ s->grcg_tile_b[3];
        }
        return value ^ 0xff;
    }
}

static uint32_t grcg_mem_readw(VGAState *s, uint32_t addr1)
{
    uint32_t addr = addr1 & 0x7fff;

    if (s->grcg_mode & GRCG_RW_MODE) {
        /* invalid */
        return *(uint16_t *)(s->vram16_draw_b + addr);
    } else {
        /* TCR */
        uint16_t value = 0;
        if (!(s->grcg_mode & GRCG_PLANE_0)) {
            value |= *(uint16_t *)(s->vram16_draw_b + addr) ^ s->grcg_tile_w[0];
        }
        if (!(s->grcg_mode & GRCG_PLANE_1)) {
            value |= *(uint16_t *)(s->vram16_draw_r + addr) ^ s->grcg_tile_w[1];
        }
        if (!(s->grcg_mode & GRCG_PLANE_2)) {
            value |= *(uint16_t *)(s->vram16_draw_g + addr) ^ s->grcg_tile_w[2];
        }
        if (!(s->grcg_mode & GRCG_PLANE_3)) {
            value |= *(uint16_t *)(s->vram16_draw_e + addr) ^ s->grcg_tile_w[3];
        }
        return ~value;
    }
}

static void grcg_mem_writeb(VGAState *s, uint32_t addr1, uint8_t value)
{
    uint32_t addr = addr1 & 0x7fff;

    if (s->grcg_mode & GRCG_RW_MODE) {
        /* RMW */
        if (!(s->grcg_mode & GRCG_PLANE_0)) {
            s->vram16_draw_b[addr] &= ~value;
            s->vram16_draw_b[addr] |= s->grcg_tile_b[0] & value;
        }
        if (!(s->grcg_mode & GRCG_PLANE_1)) {
            s->vram16_draw_r[addr] &= ~value;
            s->vram16_draw_r[addr] |= s->grcg_tile_b[1] & value;
        }
        if (!(s->grcg_mode & GRCG_PLANE_2)) {
            s->vram16_draw_g[addr] &= ~value;
            s->vram16_draw_g[addr] |= s->grcg_tile_b[2] & value;
        }
        if (!(s->grcg_mode & GRCG_PLANE_3)) {
            s->vram16_draw_e[addr] &= ~value;
            s->vram16_draw_e[addr] |= s->grcg_tile_b[3] & value;
        }
    } else {
        /* TDW */
        if (!(s->grcg_mode & GRCG_PLANE_0)) {
            s->vram16_draw_b[addr] = s->grcg_tile_b[0];
        }
        if (!(s->grcg_mode & GRCG_PLANE_1)) {
            s->vram16_draw_r[addr] = s->grcg_tile_b[0];
        }
        if (!(s->grcg_mode & GRCG_PLANE_2)) {
            s->vram16_draw_g[addr] = s->grcg_tile_b[0];
        }
        if (!(s->grcg_mode & GRCG_PLANE_3)) {
            s->vram16_draw_e[addr] = s->grcg_tile_b[0];
        }
    }
}

static void grcg_mem_writew(VGAState *s, uint32_t addr1, uint16_t value)
{
    uint32_t addr = addr1 & 0x7fff;

    if (s->grcg_mode & GRCG_RW_MODE) {
        /* RMW */
        if (!(s->grcg_mode & GRCG_PLANE_0)) {
            *(uint16_t *)(s->vram16_draw_b + addr) &= ~value;
            *(uint16_t *)(s->vram16_draw_b + addr) |= s->grcg_tile_w[0] & value;
        }
        if (!(s->grcg_mode & GRCG_PLANE_1)) {
            *(uint16_t *)(s->vram16_draw_r + addr) &= ~value;
            *(uint16_t *)(s->vram16_draw_r + addr) |= s->grcg_tile_w[1] & value;
        }
        if (!(s->grcg_mode & GRCG_PLANE_2)) {
            *(uint16_t *)(s->vram16_draw_g + addr) &= ~value;
            *(uint16_t *)(s->vram16_draw_g + addr) |= s->grcg_tile_w[2] & value;
        }
        if (!(s->grcg_mode & GRCG_PLANE_3)) {
            *(uint16_t *)(s->vram16_draw_e + addr) &= ~value;
            *(uint16_t *)(s->vram16_draw_e + addr) |= s->grcg_tile_w[3] & value;
        }
    } else {
        /* TDW */
        if (!(s->grcg_mode & GRCG_PLANE_0)) {
            *(uint16_t *)(s->vram16_draw_b + addr) = s->grcg_tile_w[0];
        }
        if (!(s->grcg_mode & GRCG_PLANE_1)) {
            *(uint16_t *)(s->vram16_draw_r + addr) = s->grcg_tile_w[1];
        }
        if (!(s->grcg_mode & GRCG_PLANE_2)) {
            *(uint16_t *)(s->vram16_draw_g + addr) = s->grcg_tile_w[2];
        }
        if (!(s->grcg_mode & GRCG_PLANE_3)) {
            *(uint16_t *)(s->vram16_draw_e + addr) = s->grcg_tile_w[3];
        }
    }
}

/* cg window */

static void cgwindow_set_addr(VGAState *s)
{
    uint32_t low = 0x7fff0, high;
    uint8_t code = s->font_code & 0x7f;
    uint16_t lr = ((~s->font_line) & 0x20) << 6;

    if (!(s->font_code & 0xff00)) {
        high = 0x80000 + (s->font_code << 4);
        if (!s->mode1[MODE1_FONTSEL]) {
            high += 0x2000;
        }
    } else {
        high = (s->font_code & 0x7f7f) << 4;
        if (code >= 0x56 && code < 0x58) {
            high += lr;
        } else if(code >= 0x09 && code < 0x0c) {
            if (lr) {
                high = low;
            }
        } else if ((code >= 0x0c && code < 0x10) || (code >= 0x58 && code < 0x60)) {
            high += lr;
        } else {
            low = high;
            high += 0x800;
        }
    }
    s->cgwindow_addr_low = low;
    s->cgwindow_addr_high = high;
}

static void cgwindow_code2_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xa1 */
    VGAState *s = opaque;

    s->font_code = (value << 8) | (s->font_code & 0xff);
    cgwindow_set_addr(s);
}

static uint32_t cgwindow_code2_read(void *opaque, uint32_t addr)
{
    /* ioport 0xa1 */
    VGAState *s = opaque;

    return (s->font_code >> 8) & 0xff;
}

static void cgwindow_code1_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xa3 */
    VGAState *s = opaque;

    s->font_code = (s->font_code & 0xff00) | value;
    cgwindow_set_addr(s);
}

static uint32_t cgwindow_code1_read(void *opaque, uint32_t addr)
{
    /* ioport 0xa3 */
    VGAState *s = opaque;

    return s->font_code & 0xff;
}

static void cgwindow_line_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xa5 */
    VGAState *s = opaque;

    s->font_line = value;
    cgwindow_set_addr(s);
}

static uint32_t cgwindow_line_read(void *opaque, uint32_t addr)
{
    /* ioport 0xa5 */
    VGAState *s = opaque;

    return s->font_line;
}

static void cgwindow_pattern_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xa9 */
    VGAState *s = opaque;
    uint16_t lr = ((~s->font_line) & 0x20) << 6;

    if ((s->font_code & 0x7e) == 0x56) {
        s->font[((s->font_code & 0x7f7f) << 4) + lr + (s->font_line & 0x0f)] = value;
    }
}

static uint32_t cgwindow_pattern_read(void *opaque, uint32_t addr)
{
    /* ioport 0xa9 */
    VGAState *s = opaque;
    uint8_t type = s->font_code & 0xff;
    uint16_t lr = ((~s->font_line) & 0x20) << 6;

    if (type >= 0x09 && type < 0x0c) {
        if (!lr) {
            return s->font[((s->font_code & 0x7f7f) << 4) + (s->font_line & 0x0f)];
        }
    } else if (s->font_code & 0xff00) {
        return s->font[((s->font_code & 0x7f7f) << 4) + lr + (s->font_line & 0x0f)];
    } else if (!(s->font_line & 0x10)) {
        return s->font[0x80000 + (s->font_code << 4) + (s->font_line & 0x1f)];
    }
    return 0;
}

static uint32_t cgwindow_readb(VGAState *s, target_phys_addr_t addr)
{
    if (addr < 0x1000) {
        if (addr & 1) {
            return s->font[s->cgwindow_addr_high + ((addr >> 1) & 0x0f)];
        } else {
            return s->font[s->cgwindow_addr_low + ((addr >> 1) & 0x0f)];
        }
    } else {
        return 0xff;
    }
}

static uint32_t cgwindow_readw(VGAState *s, target_phys_addr_t addr)
{
    if (addr < 0x1000) {
        uint16_t value = cgwindow_readb(s, addr + 0);
        value |= cgwindow_readb(s, addr + 1) << 8;
        return value;
    } else {
        return 0xffff;
    }
}

static uint32_t cgwindow_readl(VGAState *s, target_phys_addr_t addr)
{
    if (addr < 0x1000) {
        uint32_t value = cgwindow_readb(s, addr + 0);
        value |= cgwindow_readb(s, addr + 1) << 8;
        value |= cgwindow_readb(s, addr + 2) << 16;
        value |= cgwindow_readb(s, addr + 3) << 24;
        return value;
    } else {
        return 0xffffffff;
    }
}

static void cgwindow_writeb(VGAState *s, target_phys_addr_t addr, uint32_t value)
{
    if (addr < 0x1000 && (s->font_code & 0x7e) == 0x56) {
        if (addr & 1) {
            s->font[s->cgwindow_addr_high + ((addr >> 1) & 0x0f)] = value;
        } else {
            s->font[s->cgwindow_addr_low + ((addr >> 1) & 0x0f)] = value;
        }
    }
}

static void cgwindow_writew(VGAState *s, target_phys_addr_t addr, uint32_t value)
{
    if (addr < 0x1000) {
        cgwindow_writeb(s, addr + 0, value & 0xff);
        cgwindow_writeb(s, addr + 1, (value >> 8) & 0xff);
    }
}

static void cgwindow_writel(VGAState *s, target_phys_addr_t addr, uint32_t value)
{
    if (addr < 0x1000) {
        cgwindow_writeb(s, addr + 0, value & 0xff);
        cgwindow_writeb(s, addr + 1, (value >> 8) & 0xff);
        cgwindow_writeb(s, addr + 2, (value >> 16) & 0xff);
        cgwindow_writeb(s, addr + 3, (value >> 24) & 0xff);
    }
}

/* mode flip-flop */

static void mode_flipflop1_write(void *opaque, uint32_t addr, uint32_t value)
{
    // ioport 0x68
    VGAState *s = opaque;
    int num = (value >> 1) & 7;

    if (s->mode1[num] != (value & 1)) {
        switch (num) {
        case MODE1_ATRSEL:
        case MODE1_COLUMN:
            s->dirty |= DIRTY_TVRAM;
            break;
        case MODE1_200LINE:
            s->dirty |= DIRTY_VRAM0 | DIRTY_VRAM1;
            break;
        case MODE1_DISP:
            s->dirty |= DIRTY_DISPLAY;
            break;
        }
        s->mode1[num] = value & 1;
    }
}

static uint32_t mode_flipflop1_read(void *opaque, uint32_t addr)
{
    /* ioport 0x68 */
    VGAState *s = opaque;
    uint32_t value = 0;

    if (s->mode1[MODE1_ATRSEL]) {
        value |= 0x01;
    }
    if (s->mode1[MODE1_GRAPHIC]) {
        value |= 0x04;
    }
    if (s->mode1[MODE1_COLUMN]) {
        value |= 0x08;
    }
    if (s->mode1[MODE1_MEMSW]) {
        value |= 0x40;
    }
    if (s->mode1[MODE1_KAC]) {
        value |= 0x80;
    }
    return value | 0x32;
}

static void mode_flipflop2_write(void *opaque, uint32_t addr, uint32_t value1)
{
    /* ioport 0x6a */
    VGAState *s = opaque;
    uint8_t value = value1 & 1;
    int num = (value1 >> 1) & 0x7f;

    switch (num) {
    case 0x00:
        /* select 8/16 color */
        if (s->mode2[num] != value) {
            if (!s->mode2[MODE2_256COLOR]) {
                s->dirty |= DIRTY_PALETTE;
            }
            s->mode2[num] = value;
        }
        break;
    case 0x02:
        /* select grcg/egc mode */
        if (s->mode2[MODE2_WRITE_MASK]) {
            s->mode2[num] = value;
        }
        break;
    case 0x10:
        /* select 16/256 color */
        if (s->mode2[MODE2_WRITE_MASK]) {
            if (s->mode2[num] != value) {
                s->dirty |= DIRTY_PALETTE | DIRTY_VRAM0 | DIRTY_VRAM1;
                s->mode2[num] = value;
            }
        }
        break;
    case 0x34:
        /* select 400/480 lines */
        if (s->mode2[MODE2_WRITE_MASK]) {
            if (s->mode2[num] != value) {
                s->dirty |= DIRTY_VRAM0 | DIRTY_VRAM1;
                s->mode2[num] = value;
            }
        }
        break;
    case 0x11: case 0x12: case 0x13: case 0x15: case 0x16:
    case 0x30: case 0x31: case 0x33: case 0x65:
        if (s->mode2[MODE2_WRITE_MASK]) {
            s->mode2[num] = value;
        }
        break;
    default:
        s->mode2[num] = value;
        break;
    }
}

static uint32_t mode_flipflop2_read(void *opaque, uint32_t addr)
{
    /* ioport 0x6a */
    VGAState *s = opaque;
    int value = 0;

    if (s->mode3[MODE3_LINE_CONNECT]) {
        value |= 0x01;
    }
    if (s->mode3[MODE3_WRITE_MASK]) {
        value |= 0x10;
    }
    return value | 0xee;
}

static void mode_flipflop3_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x6e */
    VGAState *s = opaque;

    switch (value & 0xfe) {
    case 0x02:
        s->mode3[MODE3_WRITE_MASK] = value & 1;
        break;
    default:
        if (s->mode3[MODE3_WRITE_MASK]) {
            s->mode3[(value >> 1) & 0x7f] = value & 1;
        }
        break;
    }
}

static uint32_t mode_flipflop3_read(void *opaque, uint32_t addr)
{
    /* ioport 0x6e */
    VGAState *s = opaque;
    int value = 0;

    if (s->mode3[MODE3_WRITE_MASK]) {
        value |= 0x01;
    }
    if (s->mode3[MODE3_LINE_COLOR]) {
        value |= 0x10;
    }
    if (s->mode3[MODE3_NPC_COLOR]) {
        value |= 0x80;
    }
    return value | 0x6e;
}

static void mode_select_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x9a0 */
    VGAState *s = opaque;

    s->mode_select = value;
}

static uint32_t mode_status_read(void *opaque, uint32_t addr)
{
    /* ioport 0x9a0 */
    VGAState *s = opaque;
    uint8_t value = 0;

    if (s->mode2[0x42]) {
        value |= 2;
    }
    switch (s->mode_select) {
    case 0x01: if (s->mode1[0x01]) { value |= 1; } break;
    case 0x02: if (s->mode1[0x04]) { value |= 1; } break;
    case 0x03: if (s->mode1[0x07]) { value |= 1; } break;
    case 0x04: if (s->mode2[0x00]) { value |= 1; } break;
    case 0x05: if (s->mode2[0x20]) { value |= 1; } break;
    case 0x06: if (s->mode2[0x22]) { value |= 1; } break;
    case 0x07: if (s->mode2[0x02]) { value |= 1; } break;
    case 0x08: if (s->mode2[0x03]) { value |= 1; } break;
    case 0x09: if (s->mode2[0x41]) { value |= 1; } break;
    case 0x0a: if (s->mode2[0x10]) { value |= 1; } break;
    case 0x0b: if (s->mode2[0x31]) { value |= 1; } break;
    case 0x0d: if (s->mode2[0x34]) { value |= 1; } break;
    case 0x0e: if (s->mode2[0x11]) { value |= 1; } break;
    case 0x0f: if (s->mode2[0x12]) { value |= 1; } break;
    case 0x10: if (s->mode2[0x35]) { value |= 1; } break;
    case 0x11: if (s->mode2[0x13]) { value |= 1; } break;
    case 0x12: if (s->mode2[0x16]) { value |= 1; } break;
    case 0x13: if (s->mode2[0x14]) { value |= 1; } break;
    case 0x14: if (s->mode2[0x33]) { value |= 1; } break;
    case 0x15: if (s->mode2[0x30]) { value |= 1; } break;
    case 0x16: if (s->mode2[0x61]) { value |= 1; } break;
    case 0x17: if (s->mode2[0x36]) { value |= 1; } break;
    case 0x18: if (s->mode2[0x15]) { value |= 1; } break;
    case 0x19: if (s->mode2[0x24]) { value |= 1; } break;
    case 0x1a: if (s->mode2[0x64]) { value |= 1; } break;
    case 0x1b: if (s->mode2[0x17]) { value |= 1; } break;
    case 0x1c: if (s->mode2[0x37]) { value |= 1; } break;
    case 0x1d: if (s->mode2[0x60]) { value |= 1; } break;
    case 0x1e: if (s->mode2[0x23]) { value |= 1; } break;
    case 0x1f: if (s->mode2[0x35]) { value |= 1; } break;
    }
    return value;
}

/* vram bank */

static void vram_disp_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xa4 */
    VGAState *s = opaque;

    if (value & 1) {
        if (s->bank_disp != DIRTY_VRAM1) {
            s->vram16_disp_b = s->vram16 + 0x20000;
            s->vram16_disp_r = s->vram16 + 0x28000;
            s->vram16_disp_g = s->vram16 + 0x30000;
            s->vram16_disp_e = s->vram16 + 0x38000;
            s->vram256_disp = s->vram256 + 0x40000;
            s->bank_disp = DIRTY_VRAM1;
            s->dirty |= DIRTY_DISPLAY;
        }
    } else {
        if (s->bank_disp != DIRTY_VRAM0) {
            s->vram16_disp_b = s->vram16 + 0x00000;
            s->vram16_disp_r = s->vram16 + 0x08000;
            s->vram16_disp_g = s->vram16 + 0x10000;
            s->vram16_disp_e = s->vram16 + 0x18000;
            s->vram256_disp = s->vram256 + 0x00000;
            s->bank_disp = DIRTY_VRAM0;
            s->dirty |= DIRTY_DISPLAY;
        }
    }
}

static uint32_t vram_disp_read(void *opaque, uint32_t addr)
{
    /* ioport 0xa4 */
    VGAState *s = opaque;

    if (s->bank_disp == DIRTY_VRAM0) {
        /*return 0;*/
        return 0xfe;
    } else {
        /*return 1;*/
        return 0xff;
    }
}

static void vram_draw_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xa6 */
    VGAState *s = opaque;

    if (value & 1) {
        s->vram16_draw_b = s->vram16 + 0x20000;
        s->vram16_draw_r = s->vram16 + 0x28000;
        s->vram16_draw_g = s->vram16 + 0x30000;
        s->vram16_draw_e = s->vram16 + 0x38000;
        s->bank_draw = DIRTY_VRAM1;
    } else {
        s->vram16_draw_b = s->vram16 + 0x00000;
        s->vram16_draw_r = s->vram16 + 0x08000;
        s->vram16_draw_g = s->vram16 + 0x10000;
        s->vram16_draw_e = s->vram16 + 0x18000;
        s->bank_draw = DIRTY_VRAM0;
    }
    egc_set_vram(&s->egc, s->vram16_draw_b);
}

static uint32_t vram_draw_read(void *opaque, uint32_t addr)
{
    /* ioport 0xa6 */
    VGAState *s = opaque;

    if (s->bank_draw == DIRTY_VRAM0) {
        /*return 0;*/
        return 0xfe;
    } else {
        /*return 1;*/
        return 0xff;
    }
}

/* palette */

static void palette_a8_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xa8 */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        s->anapal_select = value;
    } else if (s->mode2[MODE2_16COLOR]) {
        s->anapal_select = value;
    } else {
        if (s->digipal[0] != value) {
            s->digipal[0] = value;
            s->dirty |= DIRTY_PALETTE;
        }
    }
}

static uint32_t palette_a8_read(void *opaque, uint32_t addr)
{
    /* ioport 0xa8 */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        return s->anapal_select;
    } else if (s->mode2[MODE2_16COLOR]) {
        return s->anapal_select;
    } else {
        return s->digipal[0];
    }
}

static void palette_aa_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xaa */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (s->anapal[PALETTE_G][s->anapal_select] != value) {
            s->anapal[PALETTE_G][s->anapal_select] = value;
            s->dirty |= DIRTY_PALETTE;
        }
    } else if (s->mode2[MODE2_16COLOR]) {
        if (s->anapal[PALETTE_G][s->anapal_select & 0x0f] != (value & 0x0f)) {
            s->anapal[PALETTE_G][s->anapal_select & 0x0f] = value & 0x0f;
            s->dirty |= DIRTY_PALETTE;
        }
    } else {
        if (s->digipal[1] != value) {
            s->digipal[1] = value;
            s->dirty |= DIRTY_PALETTE;
        }
    }
}

static uint32_t palette_aa_read(void *opaque, uint32_t addr)
{
    /* ioport 0xaa */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        return s->anapal[PALETTE_G][s->anapal_select];
    } else if (s->mode2[MODE2_16COLOR]) {
        return s->anapal[PALETTE_G][s->anapal_select & 0x0f];
    } else {
        return s->digipal[1];
    }
}

static void palette_ac_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xac */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (s->anapal[PALETTE_R][s->anapal_select] != value) {
            s->anapal[PALETTE_R][s->anapal_select] = value;
            s->dirty |= DIRTY_PALETTE;
        }
    } else if (s->mode2[MODE2_16COLOR]) {
        if (s->anapal[PALETTE_R][s->anapal_select & 0x0f] != (value & 0x0f)) {
            s->anapal[PALETTE_R][s->anapal_select & 0x0f] = value & 0x0f;
            s->dirty |= DIRTY_PALETTE;
        }
    } else {
        if (s->digipal[2] != value) {
            s->digipal[2] = value;
            s->dirty |= DIRTY_PALETTE;
        }
    }
}

static uint32_t palette_ac_read(void *opaque, uint32_t addr)
{
    /* ioport 0xac */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        return s->anapal[PALETTE_R][s->anapal_select];
    } else if (s->mode2[MODE2_16COLOR]) {
        return s->anapal[PALETTE_R][s->anapal_select & 0x0f];
    } else {
        return s->digipal[2];
    }
}

static void palette_ae_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0xae */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (s->anapal[PALETTE_B][s->anapal_select] != value) {
            s->anapal[PALETTE_B][s->anapal_select] = value;
            s->dirty |= DIRTY_PALETTE;
        }
    } else if (s->mode2[MODE2_16COLOR]) {
        if (s->anapal[PALETTE_B][s->anapal_select & 0x0f] != (value & 0x0f)) {
            s->anapal[PALETTE_B][s->anapal_select & 0x0f] = value & 0x0f;
            s->dirty |= DIRTY_PALETTE;
        }
    } else {
        if (s->digipal[3] != value) {
            s->digipal[3] = value;
            s->dirty |= DIRTY_PALETTE;
        }
    }
}

static uint32_t palette_ae_read(void *opaque, uint32_t addr)
{
    /* ioport 0xae */
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        return s->anapal[PALETTE_B][s->anapal_select];
    } else if (s->mode2[MODE2_16COLOR]) {
        return s->anapal[PALETTE_B][s->anapal_select & 0x0f];
    } else {
        return s->digipal[3];
    }
}

/* horizontal frequency */

static void horiz_freq_write(void *opaque, uint32_t addr, uint32_t value)
{
    /* ioport 0x9a8 */
}

static uint32_t horiz_freq_read(void *opaque, uint32_t addr)
{
    /* ioport 0x9a8 */
    return 0;
}

/* memory */

static void memsw_save(VGAState *s)
{
    char *filename;
    FILE* fp;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, MEMSW_FILE_NAME);
    if ((fp = fopen(filename, "wb")) != NULL) {
        fwrite(s->tvram + 0x3fe0, 32, 1, fp);
        fclose(fp);
    }
}

static int memsw_load(VGAState *s)
{
    char *filename;
    FILE* fp;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, MEMSW_FILE_NAME);
    if ((fp = fopen(filename, "rb")) != NULL) {
        fread(s->tvram + 0x3fe0, 32, 1, fp);
        fclose(fp);
        return 0;
    }
    return -1;
}

static uint32_t tvram_readb(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (addr < 0x4000) {
        return s->tvram[addr];
    } else {
        return cgwindow_readb(s, addr & 0x3fff);
    }
}

static uint32_t tvram_readw(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (addr < 0x4000) {
        return *(uint16_t *)(s->tvram + addr);
    } else {
        return cgwindow_readw(s, addr & 0x3fff);
    }
}

static uint32_t tvram_readl(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (addr < 0x4000) {
        return *(uint32_t *)(s->tvram + addr);
    } else {
        return cgwindow_readl(s, addr & 0x3fff);
    }
}

static void tvram_writeb(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (addr < 0x3fe2) {
        s->tvram[addr] = value;
        s->dirty |= DIRTY_TVRAM;
    } else if (addr < 0x4000) {
        /* memory switch */
        if (s->mode1[MODE1_MEMSW]) {
            if (s->tvram[addr] != value) {
                s->tvram[addr] = value;
                /* save memory switch */
//                memsw_save(s);
            }
        }
    } else {
        cgwindow_writeb(s, addr & 0x3fff, value);
    }
}

static void tvram_writew(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (addr < 0x3fe2) {
        *(uint16_t *)(s->tvram + addr) = value;
        s->dirty |= DIRTY_TVRAM;
    } else if (addr < 0x4000) {
        /* memory switch */
        if (s->mode1[MODE1_MEMSW]) {
            if (*(uint16_t *)(s->tvram + addr) != value) {
                *(uint16_t *)(s->tvram + addr) = value;
                /* save memory switch */
//                memsw_save(s);
            }
        }
    } else {
        cgwindow_writew(s, addr & 0x3fff, value);
    }
}

static void tvram_writel(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (addr < 0x3fe2) {
        *(uint32_t *)(s->tvram + addr) = value;
        s->dirty |= DIRTY_TVRAM;
    } else if (addr < 0x4000) {
        /* memory switch */
        if (s->mode1[MODE1_MEMSW]) {
            if (*(uint32_t *)(s->tvram + addr) != value) {
                *(uint32_t *)(s->tvram + addr) = value;
                /* save memory switch */
//                memsw_save(s);
            }
        }
    } else {
        cgwindow_writel(s, addr & 0x3fff, value);
    }
}

static uint32_t vram_readb(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (addr < 0x8000) {
            return s->vram256_draw_0[addr];
        } else if (addr < 0x10000) {
            return s->vram256_draw_1[addr & 0x7fff];
        }
        return 0xff;
    } else if (s->grcg_mode & GRCG_CG_MODE) {
        if (s->mode2[MODE2_EGC]) {
            return egc_mem_readb(&s->egc, addr);
        } else {
            return grcg_mem_readb(s, addr);
        }
    } else {
        return s->vram16_draw_b[addr];
    }
}

static uint32_t vram_readw(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (addr < 0x8000) {
            return *(uint16_t *)(s->vram256_draw_0 + addr);
        } else if (addr < 0x10000) {
            return *(uint16_t *)(s->vram256_draw_1 + (addr & 0x7fff));
        }
        return 0xffff;
    } else if (s->grcg_mode & GRCG_CG_MODE) {
        if (s->mode2[MODE2_EGC]) {
            return egc_mem_readw(&s->egc, addr);
        } else {
            return grcg_mem_readw(s, addr);
        }
    } else {
        return *(uint16_t *)(s->vram16_draw_b + addr);
    }
}

static uint32_t vram_readl(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;
    uint32_t value;

    if (s->mode2[MODE2_256COLOR]) {
        if (addr < 0x8000) {
            return *(uint32_t *)(s->vram256_draw_0 + addr);
        } else if (addr < 0x10000) {
            return *(uint32_t *)(s->vram256_draw_1 + (addr & 0x7fff));
        }
        return 0xffffffff;
    } else if (s->grcg_mode & GRCG_CG_MODE) {
        if (s->mode2[MODE2_EGC]) {
            value = egc_mem_readw(&s->egc, addr);
            value |= egc_mem_readw(&s->egc, addr + 2) << 16;
            return value;
        } else {
            value = grcg_mem_readw(s, addr);
            value |= grcg_mem_readw(s, addr + 2) << 16;
            return value;
        }
    } else {
        return *(uint32_t *)(s->vram16_draw_b + addr);
    }
}

static void vram_writeb(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (addr < 0x8000) {
            s->vram256_draw_0[addr] = value;
            s->dirty |= s->bank256_draw_0;
        } else if (addr < 0x10000) {
            s->vram256_draw_1[addr & 0x7fff] = value;
            s->dirty |= s->bank256_draw_1;
        }
    } else if (s->grcg_mode & GRCG_CG_MODE) {
        if (s->mode2[MODE2_EGC]) {
            egc_mem_writeb(&s->egc, addr, value);
        } else {
            grcg_mem_writeb(s, addr, value);
        }
        s->dirty |= s->bank_draw;
    } else {
        s->vram16_draw_b[addr] = value;
        s->dirty |= s->bank_draw;
    }
}

static void vram_writew(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (addr < 0x8000) {
            *(uint16_t *)(s->vram256_draw_0 + addr) = value;
            s->dirty |= s->bank256_draw_0;
        } else if (addr < 0x10000) {
            *(uint16_t *)(s->vram256_draw_1 + (addr & 0x7fff)) = value;
            s->dirty |= s->bank256_draw_1;
        }
    } else if (s->grcg_mode & GRCG_CG_MODE) {
        if (s->mode2[MODE2_EGC]) {
            egc_mem_writew(&s->egc, addr, value);
        } else {
            grcg_mem_writew(s, addr, value);
        }
        s->dirty |= s->bank_draw;
    } else {
        *(uint16_t *)(s->vram16_draw_b + addr) = value;
        s->dirty |= s->bank_draw;
    }
}

static void vram_writel(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        if (addr < 0x8000) {
            *(uint32_t *)(s->vram256_draw_0 + addr) = value;
            s->dirty |= s->bank256_draw_0;
        } else if (addr < 0x10000) {
            *(uint32_t *)(s->vram256_draw_1 + (addr & 0x7fff)) = value;
            s->dirty |= s->bank256_draw_1;
        }
    } else if (s->grcg_mode & GRCG_CG_MODE) {
        if (s->mode2[MODE2_EGC]) {
            egc_mem_writew(&s->egc, addr, value & 0xffff);
            egc_mem_writew(&s->egc, addr + 2, value >> 16);
        } else {
            grcg_mem_writew(s, addr, value & 0xffff);
            grcg_mem_writew(s, addr + 2, value >> 16);
        }
        s->dirty |= s->bank_draw;
    } else {
        *(uint32_t *)(s->vram16_draw_b + addr) = value;
        s->dirty |= s->bank_draw;
    }
}

static uint32_t vram_b0000_readb(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->ems_selected) {
        return s->ems[addr];
    } else {
        return vram_readb(opaque, addr + 0x8000);
    }
}

static uint32_t vram_b0000_readw(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->ems_selected) {
        return *(uint16_t *)(s->ems + addr);
    } else {
        return vram_readw(opaque, addr + 0x8000);
    }
}

static uint32_t vram_b0000_readl(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->ems_selected) {
        return *(uint32_t *)(s->ems + addr);
    } else {
        return vram_readl(opaque, addr + 0x8000);
    }
}

static void vram_b0000_writeb(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->ems_selected) {
        s->ems[addr] = value;
    } else {
        vram_writeb(opaque, addr + 0x8000, value);
    }
}

static void vram_b0000_writew(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->ems_selected) {
        *(uint16_t *)(s->ems + addr) = value;
    } else {
        vram_writew(opaque, addr + 0x8000, value);
    }
}

static void vram_b0000_writel(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->ems_selected) {
        *(uint32_t *)(s->ems + addr) = value;
    } else {
        vram_writel(opaque, addr + 0x8000, value);
    }
}

static uint32_t vram_e0000_readb(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        switch (addr & 0x7fff) {
        case 0x0004:
            return s->vram256_bank_0 & 0xff;
        case 0x0005:
            return s->vram256_bank_0 >> 8;
        case 0x0006:
            return s->vram256_bank_1 & 0xff;
        case 0x0007:
            return s->vram256_bank_1 >> 8;
        case 0x0100:
            return 0; /* support packed pixel only */
        }
        return 0xff;
    } else {
        return vram_readb(opaque, addr + 0x18000);
    }
}

static uint32_t vram_e0000_readw(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        switch (addr & 0x7fff) {
        case 0x0004:
            return s->vram256_bank_0;
        case 0x0006:
            return s->vram256_bank_1;
        case 0x0100:
            return 0; /* support packed pixel only */
        }
        return 0xffff;
    } else {
        return vram_readw(opaque, addr + 0x18000);
    }
}

static uint32_t vram_e0000_readl(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        uint32_t value;
        value = vram_e0000_readw(opaque, addr);
        value |= vram_e0000_readw(opaque, addr + 1) << 16;
        return value;
    } else {
        return vram_readl(opaque, addr + 0x18000);
    }
}

static void vram_e0000_writeb(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        switch (addr & 0x7fff) {
        case 0x0004:
            s->vram256_bank_0 = value;
            if (value & 0x08) {
                s->bank256_draw_0 = DIRTY_VRAM1;
            } else {
                s->bank256_draw_0 = DIRTY_VRAM0;
            }
            s->vram256_draw_0 = s->vram256 + (s->vram256_bank_0 & 0x0f) * 0x8000;
            break;
        case 0x0006:
            s->vram256_bank_1 = value;
            if (value & 0x08) {
                s->bank256_draw_1 = DIRTY_VRAM1;
            } else {
                s->bank256_draw_1 = DIRTY_VRAM0;
            }
            s->vram256_draw_1 = s->vram256 + (s->vram256_bank_1 & 0x0f) * 0x8000;
            break;
        }
    } else {
        vram_writeb(opaque, addr + 0x18000, value);
    }
}

static void vram_e0000_writew(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        vram_e0000_writeb(opaque, addr, value);
    } else {
        vram_writew(opaque, addr + 0x18000, value);
    }
}

static void vram_e0000_writel(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        vram_e0000_writew(opaque, addr, value & 0xffff);
        vram_e0000_writew(opaque, addr + 2, value >> 16);
    } else {
        vram_writel(opaque, addr + 0x18000, value);
    }
}

static uint32_t vram_f00000_readb(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        return s->vram256[addr];
    }
    return 0xff;
}

static uint32_t vram_f00000_readw(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        return *(uint16_t *)(s->vram256 + addr);
    }
    return 0xffff;
}

static uint32_t vram_f00000_readl(void *opaque, target_phys_addr_t addr)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        return *(uint32_t *)(s->vram256 + addr);
    }
    return 0xffffffff;
}

static void vram_f00000_writeb(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        s->vram256[addr] = value;
        if (addr & 0x40000) {
            s->dirty |= DIRTY_VRAM1;
        } else if (addr < 0x10000) {
            s->dirty |= DIRTY_VRAM0;
        }
    }
}

static void vram_f00000_writew(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        *(uint16_t *)(s->vram256 + addr) = value;
        if (addr & 0x40000) {
            s->dirty |= DIRTY_VRAM1;
        } else if (addr < 0x10000) {
            s->dirty |= DIRTY_VRAM0;
        }
    }
}

static void vram_f00000_writel(void *opaque, target_phys_addr_t addr, uint32_t value)
{
    VGAState *s = opaque;

    if (s->mode2[MODE2_256COLOR]) {
        *(uint32_t *)(s->vram256 + addr) = value;
        if (addr & 0x40000) {
            s->dirty |= DIRTY_VRAM1;
        } else if (addr < 0x10000) {
            s->dirty |= DIRTY_VRAM0;
        }
    }
}

/* gdc */

static uint32_t gdc_tvram_read(VGAState *s, target_phys_addr_t addr1)
{
    uint32_t addr = addr1 & 0x3fff;

    return s->tvram[addr];
}

static void gdc_tvram_write(VGAState *s, target_phys_addr_t addr1, uint32_t value)
{
    uint32_t addr = addr1 & 0x3fff;

    if (addr < 0x3fe2) {
        s->tvram[addr] = value;
    }
}

static uint32_t gdc_vram_read(VGAState *s, target_phys_addr_t addr1)
{
    uint32_t addr = addr1 & 0x1ffff;

    if (addr < 0x18000) {
        return vram_readb(s, addr);
    } else {
        return vram_e0000_readb(s, addr & 0x7fff);
    }
}

static void gdc_vram_write(VGAState *s, target_phys_addr_t addr1, uint32_t value)
{
    uint32_t addr = addr1 & 0x1ffff;

    if (addr < 0x18000) {
        return vram_writeb(s, addr, value);
    } else {
        return vram_e0000_writeb(s, addr & 0x7fff, value);
    }
}

/* render */

static inline uint32_t rgb_to_pixel(int depth, uint8_t r, uint8_t g, uint8_t b)
{
    switch(depth) {
    default:
    case 8:
        return rgb_to_pixel8(r, g, b);
    case 15:
        return rgb_to_pixel15(r, g, b);
    case 16:
        return rgb_to_pixel16(r, g, b);
    case 32:
        return rgb_to_pixel32(r, g, b);
    }
}

static void update_palette(VGAState *s)
{
    int depth;
    int i;
    uint8_t r, g, b;

    depth = ds_get_bits_per_pixel(s->ds);
    for (i = 0; i < 8; i++) {
        r = (i & 2) ? 0xff : 0;
        g = (i & 4) ? 0xff : 0;
        b = (i & 1) ? 0xff : 0;
        s->palette_chr[i] = rgb_to_pixel(depth, r, g, b);
    }
    if (s->mode2[MODE2_256COLOR]) {
        for (i = 0; i < 256; i++) {
            r = s->anapal[PALETTE_R][i];
            g = s->anapal[PALETTE_G][i];
            b = s->anapal[PALETTE_B][i];
            s->palette_gfx[i] = rgb_to_pixel(depth, r, g, b);
        }
    } else if (s->mode2[MODE2_16COLOR]) {
        for (i = 0; i < 16; i++) {
            r = s->anapal[PALETTE_R][i] << 4;
            g = s->anapal[PALETTE_G][i] << 4;
            b = s->anapal[PALETTE_B][i] << 4;
            s->palette_gfx[i] = rgb_to_pixel(depth, r, g, b);
        }
    } else {
        for (i = 0; i < 4; i++) {
            static int lo[4] = {7, 5, 6, 4};
            static int hi[4] = {3, 1, 2, 0};
            r = (s->digipal[i] & 0x02) ? 0xff : 0;
            g = (s->digipal[i] & 0x04) ? 0xff : 0;
            b = (s->digipal[i] & 0x01) ? 0xff : 0;
            s->palette_gfx[lo[i]] = rgb_to_pixel(depth, r, g, b);
            r = (s->digipal[i] & 0x20) ? 0xff : 0;
            g = (s->digipal[i] & 0x40) ? 0xff : 0;
            b = (s->digipal[i] & 0x10) ? 0xff : 0;
            s->palette_gfx[hi[i]] = rgb_to_pixel(depth, r, g, b);
        }
    }
}

static void render_chr_screen(VGAState *s)
{
    int pl, bl, cl;
    int sur, sdr;
    uint32_t *addr, *addr2;
    uint32_t cursor_addr;
    int cursor_top, cursor_bottom;
    int ytop, ysur, ysdr;
    int l, x, y;
    int xofs, addrofs;

    pl = s->pl & 31;
    if (pl) {
        pl = 32 - pl;
    }
    bl = s->bl + pl + 1;
    cl = s->cl;
    sur = s->sur & 31;
    if (sur) {
        sur = 32 - sur;
    }
    sdr = s->sdr + 1;

    addr = gdc_get_address(&s->gdc_chr, 2, 0x1fff);
    addr2 = addr + 160 * (sur + sdr);
    gdc_get_cursor_address(&s->gdc_chr, 0x1fff,
                           &cursor_addr, &cursor_top, &cursor_bottom);
    ytop = 0;
    ysur = bl * sur;
    ysdr = bl * (sur + sdr);

    if (s->mode1[MODE1_COLUMN]) {
        xofs = 16;
        addrofs = 2;
    } else {
        xofs = 8;
        addrofs = 1;
    }
    memset(s->tvram_buffer, 0, 640 * 480);

    for (y = 0; y < 400; y += bl) {
        uint32_t gaiji1st = 0, last = 0, offset;
        int kanji2nd = 0;
        if (y == ysur) {
            ytop = y;
            y -= s->ssl;
            ysur = 400;
        }
        if (y >= ysdr) {
            y = ytop = ysdr;
            addr = addr2;
            ysdr = 400;
        }
        for (x = 0; x < 640; x += xofs) {
            uint16_t code = *(uint16_t *)(s->tvram + *addr);
            uint8_t attr = s->tvram[*addr | 0x2000];
            uint8_t color = (attr & ATTR_COL) ? (attr >> 5) : 8;
            uint8_t cursor = (*addr == cursor_addr);
            addr += addrofs;
            if (kanji2nd) {
                kanji2nd = 0;
                offset = last + 0x800;
            } else if (code & 0xff00) {
                uint16_t lo = code & 0x7f;
                uint16_t hi = (code >> 8) & 0x7f;
                offset = (lo << 4) | (hi << 12);
                if (lo == 0x56 || lo == 0x57) {
                    offset += gaiji1st;
                    gaiji1st ^= 0x800;
                } else {
                    uint16_t lo = code & 0xff;
                    if (lo < 0x09 || lo >= 0x0c) {
                        kanji2nd = 1;
                    }
                    gaiji1st = 0;
                }
            } else {
                uint16_t lo = code & 0xff;
                if (s->mode1[MODE1_FONTSEL]) {
                    offset = 0x80000 | (lo << 4);
                } else {
                    offset = 0x82000 | (lo << 4);
                }
                gaiji1st = 0;
            }
            last = offset;
            for (l = 0; l < cl && l < 16; l++) {
                int yy = y + l + pl;
                if (yy >= ytop && yy < 480) {
                    uint8_t *dest = s->tvram_buffer + yy * 640 + x;
                    uint8_t pattern = s->font[offset + l];
                    if (!(attr & ATTR_ST)) {
                        pattern = 0;
                    } else if (((attr & ATTR_BL) && (s->blink & 0x20)) ||
                               (attr & ATTR_RV)) {
                        pattern = ~pattern;
                    }
                    if ((attr & ATTR_UL) && l == 15) {
                        pattern = 0xff;
                    }
                    if (attr & ATTR_VL) {
                        pattern |= 0x08;
                    }
                    if (cursor && l >= cursor_top && l < cursor_bottom) {
                        pattern = ~pattern;
                    }
                    if (s->mode1[MODE1_COLUMN]) {
                        if (pattern & 0x80) dest[ 0] = dest[ 1] = color;
                        if (pattern & 0x40) dest[ 2] = dest[ 3] = color;
                        if (pattern & 0x20) dest[ 4] = dest[ 5] = color;
                        if (pattern & 0x10) dest[ 6] = dest[ 7] = color;
                        if (pattern & 0x08) dest[ 8] = dest[ 9] = color;
                        if (pattern & 0x04) dest[10] = dest[11] = color;
                        if (pattern & 0x02) dest[12] = dest[13] = color;
                        if (pattern & 0x01) dest[14] = dest[15] = color;
                    } else {
                        if (pattern & 0x80) dest[0] = color;
                        if (pattern & 0x40) dest[1] = color;
                        if (pattern & 0x20) dest[2] = color;
                        if (pattern & 0x10) dest[3] = color;
                        if (pattern & 0x08) dest[4] = color;
                        if (pattern & 0x04) dest[5] = color;
                        if (pattern & 0x02) dest[6] = color;
                        if (pattern & 0x01) dest[7] = color;
                    }
                }
            }
        }
    }
}

static void render_gfx_screen(VGAState *s)
{
    uint8_t *dest;
    int x, y;
    uint8_t b, r, g, e = 0;

    static int prev_mode = -1;
    int mode;

    if (s->mode2[MODE2_256COLOR]) {
        int addr = 0;
        if (s->mode2[MODE2_480LINE]) {
            dest = s->vram0_buffer;
            for (y = 0; y < 480; y++) {
                for (x = 0; x < 640; x++) {
                    *dest++ = s->vram256[addr++];
                }
                addr += 128 * 3;
            }
            mode = 2;
        } else {
            if (s->bank_disp == DIRTY_VRAM0) {
                dest = s->vram0_buffer;
            } else {
                dest = s->vram1_buffer;
            }
            for (y = 0; y < 400; y++) {
                for (x = 0; x < 640; x++) {
                    *dest++ = s->vram256_disp[addr++];
                }
            }
            mode = 1;
        }
    } else {
        uint32_t *addr = gdc_get_address(&s->gdc_gfx, 1, 0x7fff);
        if (s->bank_disp == DIRTY_VRAM0) {
            dest = s->vram0_buffer;
        } else {
            dest = s->vram1_buffer;
        }
        for (y = 0; y < 400; y++) {
            for (x = 0; x < 640; x += 8) {
                b = s->vram16_draw_b[*addr];
                r = s->vram16_draw_r[*addr];
                g = s->vram16_draw_g[*addr];
                if (s->mode2[MODE2_16COLOR]) {
                    e = s->vram16_draw_e[*addr];
                }
                addr++;
                *dest++ = ((b & 0x80) >> 7) | ((r & 0x80) >> 6) | ((g & 0x80) >> 5) | ((e & 0x80) >> 4);
                *dest++ = ((b & 0x40) >> 6) | ((r & 0x40) >> 5) | ((g & 0x40) >> 4) | ((e & 0x40) >> 3);
                *dest++ = ((b & 0x20) >> 5) | ((r & 0x20) >> 4) | ((g & 0x20) >> 3) | ((e & 0x20) >> 2);
                *dest++ = ((b & 0x10) >> 4) | ((r & 0x10) >> 3) | ((g & 0x10) >> 2) | ((e & 0x10) >> 1);
                *dest++ = ((b & 0x08) >> 3) | ((r & 0x08) >> 2) | ((g & 0x08) >> 1) | ((e & 0x08) >> 0);
                *dest++ = ((b & 0x04) >> 2) | ((r & 0x04) >> 1) | ((g & 0x04) >> 0) | ((e & 0x04) << 1);
                *dest++ = ((b & 0x02) >> 1) | ((r & 0x02) >> 0) | ((g & 0x02) << 1) | ((e & 0x02) << 2);
                *dest++ = ((b & 0x01) >> 0) | ((r & 0x01) << 1) | ((g & 0x01) << 2) | ((e & 0x01) << 3);
            }
            if (s->mode1[MODE1_200LINE]) {
                memset(dest, 0, 640);
                dest += 640;
                y++;
            }
        }
        mode = 0;
    }
    if (prev_mode != mode) {
        switch (mode) {
        case 0:
            fprintf (stderr, "pc98vga: 640x400, 4bpp\n");
            break;
        case 1:
            fprintf (stderr, "pc98vga: 640x400, 8bpp\n");
            break;
        case 2:
            fprintf (stderr, "pc98vga: 640x480, 8bpp\n");
            break;
        }
        prev_mode = mode;
    }
}

static void update_display(void *opaque)
{
    VGAState *s = opaque;

    /* render screen */
    if (s->mode2[MODE2_256COLOR] && s->mode2[MODE2_480LINE]) {
        s->height = 480;
    } else {
        s->height = 400;
    }
    if (s->mode1[MODE1_DISP]) {
        if (s->dirty & DIRTY_PALETTE) {
            /* update palette */
            update_palette(s);
            s->dirty &= ~DIRTY_PALETTE;
            s->dirty |= DIRTY_DISPLAY;
        }
        if (s->gdc_chr.dirty & GDC_DIRTY_START) {
            s->gdc_chr.dirty &= ~GDC_DIRTY_START;
            s->dirty |= DIRTY_DISPLAY;
        }
        if (s->gdc_chr.start) {
            if ((s->gdc_chr.dirty & GDC_DIRTY_CHR) || (s->dirty & DIRTY_TVRAM)) {
                /* update text screen */
                render_chr_screen(s);
                s->gdc_chr.dirty &= ~GDC_DIRTY_CHR;
                s->dirty &= ~DIRTY_TVRAM;
                s->dirty |= DIRTY_DISPLAY;
            }
        }
        if (s->gdc_gfx.dirty & GDC_DIRTY_START) {
            s->gdc_gfx.dirty &= ~GDC_DIRTY_START;
            s->dirty |= DIRTY_DISPLAY;
        }
        if (s->gdc_gfx.start) {
            uint8_t dirty = s->bank_disp;
            if (s->mode2[MODE2_256COLOR]) {
                if (s->mode2[MODE2_480LINE]) {
                    dirty = DIRTY_VRAM0 | DIRTY_VRAM1;
                }
            }
            if ((s->gdc_gfx.dirty & GDC_DIRTY_GFX) || (s->dirty & dirty)) {
                /* update cg screen */
                render_gfx_screen(s);
                s->gdc_gfx.dirty &= ~GDC_DIRTY_GFX;
                s->dirty &= ~dirty;
                s->dirty |= DIRTY_DISPLAY;
            }
        }
    }

    /* resize screen */
    if (s->width != s->last_width || s->height != s->last_height) {
        s->last_width = s->width;
        s->last_height = s->height;
        qemu_console_resize(s->ds, s->width, s->height);
        s->dirty |= DIRTY_DISPLAY;
    }

    /* update screen */
    if (s->dirty & DIRTY_DISPLAY) {
        int x, y;
        int depth = ds_get_bits_per_pixel(s->ds);
        int size = ds_get_linesize(s->ds);
        uint8_t *dest1 = ds_get_data(s->ds);

        if (s->mode1[MODE1_DISP]) {
            /* output screen */
            uint8_t *src_chr;
            uint8_t *src_gfx;
            if (!s->gdc_chr.start || s->mode2[MODE2_256COLOR]) {
                src_chr = s->null_buffer;
            } else {
                src_chr = s->tvram_buffer;
            }
            if (!s->gdc_gfx.start) {
                src_gfx = s->null_buffer;
            } else if (s->mode2[MODE2_256COLOR] && s->mode2[MODE2_480LINE]) {
                src_gfx = s->vram0_buffer;
            } else if (s->bank_disp == DIRTY_VRAM0) {
                src_gfx = s->vram0_buffer;
            } else {
                src_gfx = s->vram1_buffer;
            }
            for (y = 0; y < s->height; y++) {
                uint8_t *dest = dest1;
                switch(depth) {
                case 8:
                    for (x = 0; x < 640; x++) {
                        if (*src_chr) {
                            *((uint8_t *)dest) = s->palette_chr[*src_chr & 0x07];
                        } else {
                            *((uint8_t *)dest) = s->palette_gfx[*src_gfx];
                        }
                        src_chr++;
                        src_gfx++;
                        dest++;
                    }
                    break;
                case 15:
                case 16:
                    for (x = 0; x < 640; x++) {
                        if (*src_chr) {
                            *((uint16_t *)dest) = s->palette_chr[*src_chr & 0x07];
                        } else {
                            *((uint16_t *)dest) = s->palette_gfx[*src_gfx];
                        }
                        src_chr++;
                        src_gfx++;
                        dest += 2;
                    }
                    break;
                case 32:
                    for (x = 0; x < 640; x++) {
                        if (*src_chr) {
                            *((uint32_t *)dest) = s->palette_chr[*src_chr & 0x07];
                        } else {
                            *((uint32_t *)dest) = s->palette_gfx[*src_gfx];
                        }
                        src_chr++;
                        src_gfx++;
                        dest += 4;
                    }
                    break;
                }
                dest1 += size;
            }
        } else {
            for (y = 0; y < s->height; y++) {
                memset(dest1, 0, size);
                dest1 += size;
            }
        }
        dpy_update(s->ds, 0, 0, s->width, s->height);
        s->dirty &= ~DIRTY_DISPLAY;
    }
}

static void invalidate_display(void *opaque)
{
    VGAState *s = opaque;

    s->dirty |= DIRTY_DISPLAY;
}

static void screen_dump(void *opaque, const char *filename)
{
    // VGAState *s = opaque;
}

static void update_text(void *opaque, console_ch_t *chardata)
{
    // VGAState *s = opaque;
}

/* font (based on Neko Project 2) */

static void kanji_copy(uint8_t *dst, uint8_t *src, int from, int to)
{
    int i, j, k;
    uint8_t *p, *q;

    for (i = from; i < to; i++) {
        p = src + 0x1800 + (0x60 * 32 * (i - 1));
        q = dst + 0x20000 + (i << 4);
        for (j = 0x20; j < 0x80; j++) {
            for (k = 0; k < 16; k++) {
                *(q + 0x800) = *(p + 16);
                *q++ = *p++;
            }
            p += 16;
            q += 0x1000 - 16;
        }
    }
}

static void font_init(VGAState *s)
{
    char *filename;
    uint8_t *buf;
    uint8_t *p, *q;
    int i, j;

    p = s->font + 0x81000;
    q = s->font + 0x82000;
    for (i = 0; i < 256; i++) {
        q += 8;
        for (j = 0; j < 4; j++) {
            uint32_t bit = 0;
            if (i & (1 << j))
                bit |= 0xf0f0f0f0;
            if (i & (0x10 << j))
                bit |= 0x0f0f0f0f;
            *(uint32_t *)p = bit;
            p += 4;
            *(uint16_t *)q = (uint16_t)bit;
            q += 2;
        }
    }
    for (i = 0; i < 0x80; i++) {
        q = s->font + (i << 12);
        memset(q + 0x000, 0, 0x0560 - 0x000);
        memset(q + 0x580, 0, 0x0d60 - 0x580);
        memset(q + 0xd80, 0, 0x1000 - 0xd80);
    }
    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, FONT_FILE_NAME);
    buf = qemu_malloc(FONT_FILE_SIZE);
    if (load_image(filename, buf) == FONT_FILE_SIZE) {
        /* 8x8 font */
        uint8_t *dst = s->font + 0x82000;
        uint8_t *src = buf;
        int cnt = 256;
        while (cnt--) {
            memcpy(dst, src, 8);
            dst += 16;
            src += 8;
        }
        /* 8x16 font */
        memcpy(s->font + 0x80000, buf + 0x0800, 16 * 128);
        memcpy(s->font + 0x80800, buf + 0x1000, 16 * 128);
        /* kanji font */
        kanji_copy(s->font, buf, 0x01, 0x30);
        kanji_copy(s->font, buf, 0x30, 0x56);
        kanji_copy(s->font, buf, 0x58, 0x5d);
    }
    qemu_free(buf);
}

/* interface */

void pc98_vga_select_ems(void *opaque, uint32_t value)
{
    /* this function is called in pc.c */
    VGAState *s = opaque;

    s->ems_selected = value & 0x02;
}

static void pc98_vga_reset(void *opaque)
{
    VGAState *s = opaque;
    int i;

    s->bank_disp = DIRTY_VRAM0;
    s->bank_draw = DIRTY_VRAM0;
    s->bank256_draw_0 = DIRTY_VRAM0;
    s->bank256_draw_1 = DIRTY_VRAM0;

    s->vram16_disp_b = s->vram16 + 0x00000;
    s->vram16_disp_r = s->vram16 + 0x08000;
    s->vram16_disp_g = s->vram16 + 0x10000;
    s->vram16_disp_e = s->vram16 + 0x18000;
    s->vram16_draw_b = s->vram16 + 0x00000;
    s->vram16_draw_r = s->vram16 + 0x08000;
    s->vram16_draw_g = s->vram16 + 0x10000;
    s->vram16_draw_e = s->vram16 + 0x18000;

    s->vram256_disp = s->vram256;
    s->vram256_draw_0 = s->vram256;
    s->vram256_draw_1 = s->vram256;

    egc_set_vram(&s->egc, s->vram16_draw_b);

    gdc_reset(&s->gdc_chr);
    gdc_reset(&s->gdc_gfx);
    egc_reset(&s->egc);

    s->vsync_clock = qemu_get_clock(rt_clock);
    qemu_mod_timer(s->vsync_timer, s->vsync_clock + GDC_VTICKS);
    s->crtv = 1;

    s->grcg_mode = 0;
    s->pl = 0;
    s->bl = 0x0f;
    s->cl = 0x10;
    s->ssl = 0;
    s->sur = 0;
    s->sdr = 24;

    s->ems_selected = 0;

    memset(s->mode1, 0, sizeof(s->mode1));
    memset(s->mode2, 0, sizeof(s->mode2));
    memset(s->mode3, 0, sizeof(s->mode3));
    s->mode_select = 0;

    s->font_code = 0;
    s->font_line = 0;
    cgwindow_set_addr(s);

    /* reset palette */
    for (i = 0; i < 8; i++) {
        s->anapal[PALETTE_B][i] = (i & 1) ? 0x07 : 0;
        s->anapal[PALETTE_R][i] = (i & 2) ? 0x07 : 0;
        s->anapal[PALETTE_G][i] = (i & 4) ? 0x07 : 0;
    }
    for (i = 8; i < 16; i++) {
        s->anapal[PALETTE_B][i] = (i & 1) ? 0x0f : 0;
        s->anapal[PALETTE_R][i] = (i & 2) ? 0x0f : 0;
        s->anapal[PALETTE_G][i] = (i & 4) ? 0x0f : 0;
    }
    s->anapal_select = 0;

    /* force redraw */
    s->dirty = 0xff;
    s->blink = 0;

    s->width = 640;
    s->height = 400;
}

static void pc98_vga_pre_save(void *opaque)
{
    VGAState *s = opaque;

    egc_pre_save(&s->egc);
}

static int pc98_vga_post_load(void *opaque, int version_id)
{
    VGAState *s = opaque;
    int i;

    gdc_post_load(&s->gdc_chr);
    gdc_post_load(&s->gdc_gfx);
    egc_post_load(&s->egc);

    if (s->bank_draw == DIRTY_VRAM0) {
        s->vram16_draw_b = s->vram16 + 0x00000;
        s->vram16_draw_r = s->vram16 + 0x08000;
        s->vram16_draw_g = s->vram16 + 0x10000;
        s->vram16_draw_e = s->vram16 + 0x18000;
    } else {
        s->vram16_draw_b = s->vram16 + 0x20000;
        s->vram16_draw_r = s->vram16 + 0x28000;
        s->vram16_draw_g = s->vram16 + 0x30000;
        s->vram16_draw_e = s->vram16 + 0x38000;
    }
    if (s->bank_disp == DIRTY_VRAM0) {
        s->vram16_disp_b = s->vram16 + 0x00000;
        s->vram16_disp_r = s->vram16 + 0x08000;
        s->vram16_disp_g = s->vram16 + 0x10000;
        s->vram16_disp_e = s->vram16 + 0x18000;
        s->vram256_disp = s->vram256 + 0x00000;
    } else {
        s->vram16_disp_b = s->vram16 + 0x20000;
        s->vram16_disp_r = s->vram16 + 0x28000;
        s->vram16_disp_g = s->vram16 + 0x30000;
        s->vram16_disp_e = s->vram16 + 0x38000;
        s->vram256_disp = s->vram256 + 0x40000;
    }
    if (s->vram256_bank_0 & 0x08) {
        s->bank256_draw_0 = DIRTY_VRAM0;
    } else {
        s->bank256_draw_0 = DIRTY_VRAM1;
    }
    if (s->vram256_bank_1 & 0x08) {
        s->bank256_draw_1 = DIRTY_VRAM0;
    } else {
        s->bank256_draw_1 = DIRTY_VRAM1;
    }
    s->vram256_draw_0 = s->vram256 + (s->vram256_bank_0 & 0x0f) * 0x8000;
    s->vram256_draw_1 = s->vram256 + (s->vram256_bank_1 & 0x0f) * 0x8000;

    egc_set_vram(&s->egc, s->vram16_draw_b);

    for (i = 0; i < 4; i++) {
        uint16_t tmp = s->grcg_tile_b[i];
        s->grcg_tile_w[i] = tmp | (tmp << 8);
    }

    /* update cgwindow addr */
    cgwindow_set_addr(s);

    /* reset vsync timer */
    s->vsync_clock = qemu_get_clock(rt_clock);
    qemu_mod_timer(s->vsync_timer, s->vsync_clock + GDC_VTICKS);

    /* force redraw */
    s->dirty = 0xff;

    return 0;
}

static const VMStateDescription vmstate_vga = {
    .name = "pc98-vga",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .pre_save = pc98_vga_pre_save,
    .post_load = pc98_vga_post_load,
    .fields = (VMStateField []) {
        VMSTATE_STRUCT(gdc_chr, VGAState, 0, vmstate_gdc, GDCState),
        VMSTATE_STRUCT(gdc_gfx, VGAState, 0, vmstate_gdc, GDCState),
        VMSTATE_STRUCT(egc, VGAState, 0, vmstate_egc, EGCState),
        VMSTATE_UINT8_ARRAY(tvram, VGAState, TVRAM_SIZE),
        VMSTATE_UINT8_ARRAY(vram16, VGAState, VRAM16_SIZE),
        VMSTATE_UINT8_ARRAY(vram256, VGAState, VRAM256_SIZE),
        VMSTATE_UINT8_ARRAY(ems, VGAState, EMS_SIZE),
        VMSTATE_INT32(width, VGAState),
        VMSTATE_INT32(height, VGAState),
        VMSTATE_UINT8(grcg_mode, VGAState),
        VMSTATE_UINT8(grcg_tile_cnt, VGAState),
        VMSTATE_UINT8_ARRAY(grcg_tile_b, VGAState, 4),
        VMSTATE_UINT8(crtv, VGAState),
        VMSTATE_UINT8(pl, VGAState),
        VMSTATE_UINT8(bl, VGAState),
        VMSTATE_UINT8(cl, VGAState),
        VMSTATE_UINT8(ssl, VGAState),
        VMSTATE_UINT8(sur, VGAState),
        VMSTATE_UINT8(sdr, VGAState),
        VMSTATE_UINT8_ARRAY(mode1, VGAState, 8),
        VMSTATE_UINT8_ARRAY(mode2, VGAState, 128),
        VMSTATE_UINT8_ARRAY(mode3, VGAState, 128),
        VMSTATE_UINT8(mode_select, VGAState),
        VMSTATE_UINT8_ARRAY(digipal, VGAState, 4),
        VMSTATE_UINT8_ARRAY(anapal[0], VGAState, 256),
        VMSTATE_UINT8_ARRAY(anapal[1], VGAState, 256),
        VMSTATE_UINT8_ARRAY(anapal[2], VGAState, 256),
        VMSTATE_UINT8(anapal_select, VGAState),
        VMSTATE_UINT8(bank_draw, VGAState),
        VMSTATE_UINT8(bank_disp, VGAState),
        VMSTATE_UINT16(vram256_bank_0, VGAState),
        VMSTATE_UINT16(vram256_bank_1, VGAState),
        VMSTATE_UINT8(ems_selected, VGAState),
        VMSTATE_UINT8_ARRAY(font, VGAState, 0x84000),
        VMSTATE_UINT16(font_code, VGAState),
        VMSTATE_UINT8(font_line, VGAState),
        VMSTATE_END_OF_LIST()
    }
};

static CPUReadMemoryFunc *tvram_read[] = {
    &tvram_readb,
    &tvram_readw,
    &tvram_readl,
};
static CPUWriteMemoryFunc *tvram_write[] = {
    &tvram_writeb,
    &tvram_writew,
    &tvram_writel,
};
static CPUReadMemoryFunc *vram_a8000_read[] = {
    &vram_readb,
    &vram_readw,
    &vram_readl,
};
static CPUWriteMemoryFunc *vram_a8000_write[] = {
    &vram_writeb,
    &vram_writew,
    &vram_writel,
};
static CPUReadMemoryFunc *vram_b0000_read[] = {
    &vram_b0000_readb,
    &vram_b0000_readw,
    &vram_b0000_readl,
};
static CPUWriteMemoryFunc *vram_b0000_write[] = {
    &vram_b0000_writeb,
    &vram_b0000_writew,
    &vram_b0000_writel,
};
static CPUReadMemoryFunc *vram_e0000_read[] = {
    &vram_e0000_readb,
    &vram_e0000_readw,
    &vram_e0000_readl,
};
static CPUWriteMemoryFunc *vram_e0000_write[] = {
    &vram_e0000_writeb,
    &vram_e0000_writew,
    &vram_e0000_writel,
};
static CPUReadMemoryFunc *vram_f00000_read[] = {
    &vram_f00000_readb,
    &vram_f00000_readw,
    &vram_f00000_readl,
};
static CPUWriteMemoryFunc *vram_f00000_write[] = {
    &vram_f00000_writeb,
    &vram_f00000_writew,
    &vram_f00000_writel,
};

static const uint8_t memsw_default[] = {
    0xe1, 0x48, 0xe1, 0x05, 0xe1, 0x0c, 0xe1, 0x00,
    0xe1, 0x01, 0xe1, 0x40, 0xe1, 0x00, 0xe1, 0x00,
};

static int pc98_vga_initfn(ISADevice *dev)
{
    VGAState *s = DO_UPCAST(VGAState, dev, dev);
    int tvram_io_memory;
    int vram_a8000_io_memory;
    int vram_b0000_io_memory;
    int vram_e0000_io_memory;
    int vram_f00000_io_memory;
    int i;

    isa_init_irq(dev, &s->pic, s->irq);

    font_init(s);

//    if (memsw_load(s)) {
        for (i = 0; i < 16; i++) {
            s->tvram[0x3fe0 + (i << 1)] = memsw_default[i];
        }
//    }

    gdc_init(&s->gdc_chr, s, gdc_tvram_read, gdc_tvram_write);
    gdc_init(&s->gdc_gfx, s, gdc_vram_read, gdc_vram_write);
    egc_init(&s->egc, s);

    register_ioport_write(0x60, 1, 1, gdc_param_write, &s->gdc_chr);
    register_ioport_read(0x60, 1, 1, gdc_statreg_read, &s->gdc_chr);
    isa_init_ioport(dev, 0x60);
    register_ioport_write(0x62, 1, 1, gdc_cmdreg_write, &s->gdc_chr);
    register_ioport_read(0x62, 1, 1, gdc_data_read, &s->gdc_chr);
    isa_init_ioport(dev, 0x62);

    register_ioport_write(0x64, 1, 1, vsync_write, s);
    isa_init_ioport(dev, 0x64);

    register_ioport_write(0x68, 1, 1, mode_flipflop1_write, s);
    register_ioport_read(0x68, 1, 1, mode_flipflop1_read, s);
    isa_init_ioport(dev, 0x68);
    register_ioport_write(0x6a, 1, 1, mode_flipflop2_write, s);
    register_ioport_read(0x6a, 1, 1, mode_flipflop2_read, s);
    isa_init_ioport(dev, 0x6a);
    register_ioport_write(0x6e, 1, 1, mode_flipflop3_write, s);
    register_ioport_read(0x6e, 1, 1, mode_flipflop3_read, s);
    isa_init_ioport(dev, 0x6e);

    register_ioport_write(0x70, 1, 1, crtc_pl_write, s);
    isa_init_ioport(dev, 0x70);
    register_ioport_write(0x72, 1, 1, crtc_bl_write, s);
    isa_init_ioport(dev, 0x72);
    register_ioport_write(0x74, 1, 1, crtc_cl_write, s);
    isa_init_ioport(dev, 0x74);
    register_ioport_write(0x76, 1, 1, crtc_ssl_write, s);
    isa_init_ioport(dev, 0x76);
    register_ioport_write(0x78, 1, 1, crtc_sur_write, s);
    isa_init_ioport(dev, 0x78);
    register_ioport_write(0x7a, 1, 1, crtc_sdr_write, s);
    isa_init_ioport(dev, 0x7a);

    register_ioport_write(0x7c, 1, 1, grcg_mode_write, s);
    isa_init_ioport(dev, 0x7c);
    register_ioport_write(0x7e, 1, 1, grcg_tile_write, s);
    isa_init_ioport(dev, 0x7e);

    register_ioport_write(0xa0, 1, 1, gdc_param_write, &s->gdc_gfx);
    register_ioport_read(0xa0, 1, 1, gdc_statreg_read, &s->gdc_gfx);
    isa_init_ioport(dev, 0xa0);
    register_ioport_write(0xa2, 1, 1, gdc_cmdreg_write, &s->gdc_gfx);
    register_ioport_read(0xa2, 1, 1, gdc_data_read, &s->gdc_gfx);
    isa_init_ioport(dev, 0xa2);

    register_ioport_write(0xa1, 1, 1, cgwindow_code2_write, s);
    register_ioport_read(0xa1, 1, 1, cgwindow_code2_read, s);
    isa_init_ioport(dev, 0xa1);
    register_ioport_write(0xa3, 1, 1, cgwindow_code1_write, s);
    register_ioport_read(0xa3, 1, 1, cgwindow_code1_read, s);
    isa_init_ioport(dev, 0xa3);
    register_ioport_write(0xa5, 1, 1, cgwindow_line_write, s);
    register_ioport_read(0xa5, 1, 1, cgwindow_line_read, s);
    isa_init_ioport(dev, 0xa5);
    register_ioport_write(0xa9, 1, 1, cgwindow_pattern_write, s);
    register_ioport_read(0xa9, 1, 1, cgwindow_pattern_read, s);
    isa_init_ioport(dev, 0xa9);

    register_ioport_write(0xa4, 1, 1, vram_disp_write, s);
    register_ioport_read(0xa4, 1, 1, vram_disp_read, s);
    isa_init_ioport(dev, 0xa4);
    register_ioport_write(0xa6, 1, 1, vram_draw_write, s);
    register_ioport_read(0xa6, 1, 1, vram_draw_read, s);
    isa_init_ioport(dev, 0xa6);

    register_ioport_write(0xa8, 1, 1, palette_a8_write, s);
    register_ioport_read(0xa8, 1, 1, palette_a8_read, s);
    isa_init_ioport(dev, 0xa8);
    register_ioport_write(0xaa, 1, 1, palette_aa_write, s);
    register_ioport_read(0xaa, 1, 1, palette_aa_read, s);
    isa_init_ioport(dev, 0xaa);
    register_ioport_write(0xac, 1, 1, palette_ac_write, s);
    register_ioport_read(0xac, 1, 1, palette_ac_read, s);
    isa_init_ioport(dev, 0xac);
    register_ioport_write(0xae, 1, 1, palette_ae_write, s);
    register_ioport_read(0xae, 1, 1, palette_ae_read, s);
    isa_init_ioport(dev, 0xae);

    register_ioport_write(0x4a0, 16, 1, egc_ioport_writeb, &s->egc);
    register_ioport_write(0x4a0, 16, 2, egc_ioport_writew, &s->egc);
    isa_init_ioport_range(dev, 0x4a0, 16);

    register_ioport_write(0x9a0, 1, 1, mode_select_write, s);
    register_ioport_read(0x9a0, 1, 1, mode_status_read, s);
    isa_init_ioport(dev, 0x9a0);

    register_ioport_write(0x9a8, 1, 1, horiz_freq_write, s);
    register_ioport_read(0x9a8, 1, 1, horiz_freq_read, s);
    isa_init_ioport(dev, 0x9a8);

    tvram_io_memory = cpu_register_io_memory(tvram_read, tvram_write, s, DEVICE_LITTLE_ENDIAN);
    vram_a8000_io_memory = cpu_register_io_memory(vram_a8000_read, vram_a8000_write, s, DEVICE_LITTLE_ENDIAN);
    vram_b0000_io_memory = cpu_register_io_memory(vram_b0000_read, vram_b0000_write, s, DEVICE_LITTLE_ENDIAN);
    vram_e0000_io_memory = cpu_register_io_memory(vram_e0000_read, vram_e0000_write, s, DEVICE_LITTLE_ENDIAN);
    vram_f00000_io_memory = cpu_register_io_memory(vram_f00000_read, vram_f00000_write, s, DEVICE_LITTLE_ENDIAN);

    cpu_register_physical_memory(isa_mem_base + 0x000a0000, 0x08000, tvram_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0x000a8000, 0x08000, vram_a8000_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0x000b0000, 0x10000, vram_b0000_io_memory);
    qemu_register_coalesced_mmio(isa_mem_base + 0x000a0000, 0x20000);
    cpu_register_physical_memory(isa_mem_base + 0x000e0000, 0x08000, vram_e0000_io_memory);
    qemu_register_coalesced_mmio(isa_mem_base + 0x000e0000, 0x08000);
    cpu_register_physical_memory(isa_mem_base + 0x00f00000, 0xa0000, vram_f00000_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0x00fa0000, 0x08000, tvram_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0x00fa8000, 0x08000, vram_a8000_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0x00fb0000, 0x10000, vram_b0000_io_memory);
    qemu_register_coalesced_mmio(isa_mem_base + 0x00f00000, 0xc0000);
    cpu_register_physical_memory(isa_mem_base + 0x00fe0000, 0x08000, vram_e0000_io_memory);
    qemu_register_coalesced_mmio(isa_mem_base + 0x00fe0000, 0x08000);
    cpu_register_physical_memory(isa_mem_base + 0xfff00000, 0xa0000, vram_f00000_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0xfffa0000, 0x08000, tvram_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0xfffa8000, 0x08000, vram_a8000_io_memory);
    cpu_register_physical_memory(isa_mem_base + 0xfffb0000, 0x10000, vram_b0000_io_memory);
    qemu_register_coalesced_mmio(isa_mem_base + 0xfff00000, 0xc0000);
    cpu_register_physical_memory(isa_mem_base + 0xfffe0000, 0x08000, vram_e0000_io_memory);
    qemu_register_coalesced_mmio(isa_mem_base + 0xfffe0000, 0x08000);

    s->vsync_timer = qemu_new_timer(rt_clock, vsync_timer_handler, s);

    pc98_vga_reset(s);
    qemu_register_reset(pc98_vga_reset, s);

    s->ds = graphic_console_init(update_display, invalidate_display,
                                 screen_dump, update_text, s);

    return 0;
}

void *pc98_vga_init(void)
{
    ISADevice *dev;
    VGAState *s;

    dev = isa_create_simple("pc98-vga");
    s = DO_UPCAST(VGAState, dev, dev);

    return s;
}

static ISADeviceInfo pc98_vga_info = {
    .qdev.name  = "pc98-vga",
    .qdev.desc  = "NEC PC-9821 VGA",
    .qdev.size  = sizeof(VGAState),
    .qdev.vmsd  = &vmstate_vga,
    .init       = pc98_vga_initfn,
    .qdev.props = (Property[]) {
        DEFINE_PROP_UINT32("irq", VGAState, irq, 2),
        DEFINE_PROP_END_OF_LIST (),
    },
};

static void pc98_vga_register(void)
{
    isa_qdev_register(&pc98_vga_info);
}
device_init(pc98_vga_register)
