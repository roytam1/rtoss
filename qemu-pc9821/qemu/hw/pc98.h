#ifndef HW_PC98_H
#define HW_PC98_H

#define PC98_SYSCLOCK_5MHZ
//#define PC98_SYSCLOCK_8MHZ

#ifdef PC98_SYSCLOCK_5MHZ
    #define PC98_PIT_FREQ 2457600
#else
    #define PC98_PIT_FREQ 1996800
#endif

#define PC98_USE_SYSTEM_16MB_REGION
#define PC98_SUPPORT_640KB_FDD_IF

/* pc98bkd */
void pc98_kbd_init(int irq);

/* pc98mouse */
void pc98_mouse_init(int irq);

/* pc98sys */
void pc98_sys_init(int irq);

/* pc98vga */
void pc98_vga_init(int irq);

/* i8254.c */
PITState *pc98_pit_init(qemu_irq irq);

/* i8259.c */
qemu_irq *pc98_i8259_init(qemu_irq parent_irq);

/* pcspk.c */
void pc98_pcspk_init(PITState *);
void pc98_pcspk_write(uint32_t val);

/* piix_pci.c */
PCIBus *pc98_i440fx_init(PCII440FXState **pi440fx_state,
                         int *piix3_devfn, qemu_irq *pic, uint32_t ram_size);

/* ne2000.c */
void pc98_ne2000_init(int base, int irq, NICInfo *nd);

#endif
