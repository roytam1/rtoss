
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef	signed char		int8_t;
typedef	unsigned char	uint8_t;
typedef	signed short	int16_t;
typedef	unsigned short	uint16_t_;
typedef	signed int		int32_t;
typedef	unsigned int	uint32_t_;
typedef	signed long long	int64_t;
typedef	unsigned long long	uint64_t;

typedef uint32_t_ ram_addr_t;
typedef uint32_t_ target_phys_addr_t;

typedef unsigned long console_ch_t;

typedef void (*vga_hw_update_ptr)(void *);
typedef void (*vga_hw_invalidate_ptr)(void *);
typedef void (*vga_hw_screen_dump_ptr)(void *, const char *);
typedef void (*vga_hw_text_update_ptr)(void *, console_ch_t *);

typedef struct PixelFormat {
    uint8_t bits_per_pixel;
    uint8_t bytes_per_pixel;
    uint8_t depth; /* color depth in bits */
    uint32_t_ rmask, gmask, bmask, amask;
    uint8_t rshift, gshift, bshift, ashift;
    uint8_t rmax, gmax, bmax, amax;
    uint8_t rbits, gbits, bbits, abits;
} PixelFormat;

typedef struct DisplaySurface {
    uint8_t flags;
    int width;
    int height;
    int linesize;        /* bytes per line */
    uint8_t *data;

    struct PixelFormat pf;
} DisplaySurface;

typedef struct DisplayChangeListener {
    int idle;
    uint64_t gui_timer_interval;

    void (*dpy_update)(struct DisplayState *s, int x, int y, int w, int h);
    void (*dpy_resize)(struct DisplayState *s);
    void (*dpy_setdata)(struct DisplayState *s);
    void (*dpy_refresh)(struct DisplayState *s);
    void (*dpy_copy)(struct DisplayState *s, int src_x, int src_y,
                     int dst_x, int dst_y, int w, int h);
    void (*dpy_fill)(struct DisplayState *s, int x, int y,
                     int w, int h, uint32_t_ c);
    void (*dpy_text_cursor)(struct DisplayState *s, int x, int y);

    struct DisplayChangeListener *next;
} DisplayChangeListener;

struct DisplayState {
    struct DisplaySurface *surface;
    void *opaque;
    //struct QEMUTimer *gui_timer;

    struct DisplayChangeListener* listeners;

    void (*mouse_set)(int x, int y, int on);
    void (*cursor_define)(int width, int height, int bpp, int hot_x, int hot_y,
                          uint8_t *image, uint8_t *mask);

    struct DisplayState *next;
};
typedef struct DisplayState DisplayState;

uint32_t_ cirrus_vga_mem_readb(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_vga_mem_readw(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_vga_mem_readl(void *opaque, target_phys_addr_t addr);
void cirrus_vga_mem_writeb(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_vga_mem_writew(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_vga_mem_writel(void *opaque, target_phys_addr_t addr, uint32_t_ val);

uint32_t_ cirrus_linear_readb(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_linear_readw(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_linear_readl(void *opaque, target_phys_addr_t addr);
void cirrus_linear_writeb(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_linear_writew(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_linear_writel(void *opaque, target_phys_addr_t addr, uint32_t_ val);

uint32_t_ cirrus_linear_bitblt_readb(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_linear_bitblt_readw(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_linear_bitblt_readl(void *opaque, target_phys_addr_t addr);
void cirrus_linear_bitblt_writeb(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_linear_bitblt_writew(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_linear_bitblt_writel(void *opaque, target_phys_addr_t addr, uint32_t_ val);

uint32_t_ cirrus_mmio_readb(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_mmio_readw(void *opaque, target_phys_addr_t addr);
uint32_t_ cirrus_mmio_readl(void *opaque, target_phys_addr_t addr);
void cirrus_mmio_writeb(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_mmio_writew(void *opaque, target_phys_addr_t addr, uint32_t_ val);
void cirrus_mmio_writel(void *opaque, target_phys_addr_t addr, uint32_t_ val);

DisplayState *graphic_console_init(vga_hw_update_ptr update,
                                   vga_hw_invalidate_ptr invalidate,
                                   vga_hw_screen_dump_ptr screen_dump,
                                   vga_hw_text_update_ptr text_update,
								   void *opaque);

extern REG8		ga_relay;
extern void		*cirrusvga_opaque;
//extern UINT8	*g_memptr;
extern UINT32	ga_VRAMWindowAddr;


// ñ≥óùñÓóùäOÇ©ÇÁåƒÇ◊ÇÈÇÊÇ§Ç…
void pc98_cirrus_vga_init();
void pc98_cirrus_vga_bind();
void pc98_cirrus_vga_shutdown();

void cirrusvga_setGAWindow(HWND hWnd, HDC hdc);

void np2vga_init(HINSTANCE g_hInstance);
void np2vga_drawframe();
void np2vga_shutdown();

void np2vga_resetRelay();

void cirrusvga_drawGraphic();

#ifdef __cplusplus
}
#endif

