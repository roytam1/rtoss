#ifndef HW_PC98_H
#define HW_PC98_H

#if 1
    #define PC98_SYSCLOCK_5MHZ
#else
    #define PC98_SYSCLOCK_8MHZ
#endif

#ifdef PC98_SYSCLOCK_5MHZ
    #define PC98_PIT_FREQ 2457600
#else
    #define PC98_PIT_FREQ 1996800
#endif

//#define PC98_USE_16MB_AREA

/* i8254.c */
PITState *pc98_pit_init(qemu_irq irq);

/* i8259.c */
qemu_irq *pc98_i8259_init(qemu_irq parent_irq);

/* pcspk.c */
void pc98_pcspk_init(PITState *);
void pc98_pcspk_write(uint32_t val);

/* piix_pci.c */
PCIBus *pc98_i440fx_init(PCII440FXState **pi440fx_state, int *piix3_devfn, qemu_irq *pic);

/* ne2000.c */
void pc98_ne2000_init(int base, int irq, NICInfo *nd);

#endif
