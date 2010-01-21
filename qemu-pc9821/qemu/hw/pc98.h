#ifndef HW_PC98_H
#define HW_PC98_H

//#define DEBUG_PC98_SYSTEM
#ifdef DEBUG_PC98_SYSTEM
#define pc98_system_log(...) fprintf(stderr, "pc98sys: " __VA_ARGS__)
#else
#define pc98_system_log(...)
#endif

#define PC98_SYSCLOCK_5MHZ
//#define PC98_SYSCLOCK_8MHZ

#ifdef PC98_SYSCLOCK_5MHZ
    #define PC98_PIT_FREQ 2457600
#else
    #define PC98_PIT_FREQ 1996800
#endif

//#define PC98_VGA_FORCE_REDRAW
#define PC98_CIRRUS_VRAM_SIZE 0x100000

/* pc98.c */
void pc98_cpu_shutdown(void);

/* pc98bkd.c */
void pc98_kbd_init(void);

/* pc98mouse.c */
void pc98_mouse_init(void);

/* pc98sound.c */
int pc98_sound_init(qemu_irq *pic);

/* pc98sys.c */
void *pc98_sys_init(void);
uint8_t pc98_sys_read_shut(void *opaque);

/* pc98vga.c */
void pc98_vga_init(void);

/* cirrus_vga.c */
void *pc98_cirrus_vga_init(DisplayState *ds);
void pc98_cirrus_vga_invalidate_display_size(void *opaque);
void pc98_cirrus_vga_update_display(void *opaque);
void pc98_cirrus_vga_invalidate_display(void *opaque);

uint32_t pc98_cirrus_vram_readb(void *opaque, target_phys_addr_t addr);
uint32_t pc98_cirrus_vram_readw(void *opaque, target_phys_addr_t addr);
uint32_t pc98_cirrus_vram_readl(void *opaque, target_phys_addr_t addr);
void pc98_cirrus_vram_writeb(void *opaque,
                             target_phys_addr_t addr, uint32_t value);
void pc98_cirrus_vram_writew(void *opaque,
                             target_phys_addr_t addr, uint32_t value);
void pc98_cirrus_vram_writel(void *opaque,
                             target_phys_addr_t addr, uint32_t value);

/* i8254.c */
PITState *pc98_pit_init(qemu_irq irq);

/* i8259.c */
qemu_irq *pc98_i8259_init(qemu_irq parent_irq);

/* pcspk.c */
void pc98_pcspk_init(PITState *);
void pc98_pcspk_write(uint32_t val);

/* piix_pci.c */
PCIBus *pc98_i440fx_init(PCII440FXState **pi440fx_state,
                         int *piix3_devfn, qemu_irq *pic, ram_addr_t ram_size);

/* ne2000.c */
void pc98_ne2000_init(int base, int irq, NICInfo *nd);

#endif
