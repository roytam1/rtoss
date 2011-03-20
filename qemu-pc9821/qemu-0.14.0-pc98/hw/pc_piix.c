/*
 * QEMU PC System Emulator
 *
 * Copyright (c) 2003-2004 Fabrice Bellard
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

#include "hw.h"
#include "pc.h"
#include "audiodev.h"
#include "apic.h"
#include "pci.h"
#include "usb-uhci.h"
#include "usb-ohci.h"
#include "net.h"
#include "boards.h"
#include "ide.h"
#include "kvm.h"
#include "sysemu.h"
#include "sysbus.h"
#include "arch_init.h"
#include "blockdev.h"

#define MAX_IDE_BUS 2

static const int ide_iobase[MAX_IDE_BUS] = { 0x1f0, 0x170 };
static const int ide_iobase2[MAX_IDE_BUS] = { 0x3f6, 0x376 };
static const int ide_irq[MAX_IDE_BUS] = { 14, 15 };

static void ioapic_init(IsaIrqState *isa_irq_state)
{
    DeviceState *dev;
    SysBusDevice *d;
    unsigned int i;

    dev = qdev_create(NULL, "ioapic");
    qdev_init_nofail(dev);
    d = sysbus_from_qdev(dev);
    sysbus_mmio_map(d, 0, 0xfec00000);

    for (i = 0; i < IOAPIC_NUM_PINS; i++) {
        isa_irq_state->ioapic[i] = qdev_get_gpio_in(dev, i);
    }
}

/* PC hardware initialisation */
static void pc_init1(ram_addr_t ram_size,
                     const char *boot_device,
                     const char *kernel_filename,
                     const char *kernel_cmdline,
                     const char *initrd_filename,
                     const char *cpu_model,
                     int pci_enabled)
{
    int i;
    ram_addr_t below_4g_mem_size, above_4g_mem_size;
    PCIBus *pci_bus;
    PCII440FXState *i440fx_state;
    int piix3_devfn = -1;
    qemu_irq *cpu_irq;
    qemu_irq *isa_irq;
    qemu_irq *i8259;
    qemu_irq *cmos_s3;
    qemu_irq *smi_irq;
    IsaIrqState *isa_irq_state;
    DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
    FDCtrl *floppy_controller;
    BusState *idebus[MAX_IDE_BUS];
    ISADevice *rtc_state;

    pc_cpus_init(cpu_model);

    vmport_init();

    /* allocate ram and load rom/bios */
    pc_memory_init(ram_size, kernel_filename, kernel_cmdline, initrd_filename,
                   &below_4g_mem_size, &above_4g_mem_size);

    cpu_irq = pc_allocate_cpu_irq();
    i8259 = i8259_init(cpu_irq[0]);
    isa_irq_state = qemu_mallocz(sizeof(*isa_irq_state));
    isa_irq_state->i8259 = i8259;
    if (pci_enabled) {
        ioapic_init(isa_irq_state);
    }
    isa_irq = qemu_allocate_irqs(isa_irq_handler, isa_irq_state, 24);

    if (pci_enabled) {
        pci_bus = i440fx_init(&i440fx_state, &piix3_devfn, isa_irq, ram_size);
    } else {
        pci_bus = NULL;
        i440fx_state = NULL;
        isa_bus_new(NULL);
    }
    isa_bus_irqs(isa_irq);

    pc_register_ferr_irq(isa_reserve_irq(13));

    pc_vga_init(pci_enabled? pci_bus: NULL);

    /* init basic PC hardware */
    pc_basic_device_init(isa_irq, &floppy_controller, &rtc_state);

    for(i = 0; i < nb_nics; i++) {
        NICInfo *nd = &nd_table[i];

        if (!pci_enabled || (nd->model && strcmp(nd->model, "ne2k_isa") == 0))
            pc_init_ne2k_isa(nd);
        else
            pci_nic_init_nofail(nd, "e1000", NULL);
    }

    if (drive_get_max_bus(IF_IDE) >= MAX_IDE_BUS) {
        fprintf(stderr, "qemu: too many IDE bus\n");
        exit(1);
    }

    for(i = 0; i < MAX_IDE_BUS * MAX_IDE_DEVS; i++) {
        hd[i] = drive_get(IF_IDE, i / MAX_IDE_DEVS, i % MAX_IDE_DEVS);
    }

    if (pci_enabled) {
        PCIDevice *dev;
        dev = pci_piix3_ide_init(pci_bus, hd, piix3_devfn + 1);
        idebus[0] = qdev_get_child_bus(&dev->qdev, "ide.0");
        idebus[1] = qdev_get_child_bus(&dev->qdev, "ide.1");
    } else {
        for(i = 0; i < MAX_IDE_BUS; i++) {
            ISADevice *dev;
            dev = isa_ide_init(ide_iobase[i], ide_iobase2[i], ide_irq[i],
                               hd[MAX_IDE_DEVS * i], hd[MAX_IDE_DEVS * i + 1]);
            idebus[i] = qdev_get_child_bus(&dev->qdev, "ide.0");
        }
    }

    audio_init(isa_irq, pci_enabled ? pci_bus : NULL);

    pc_cmos_init(below_4g_mem_size, above_4g_mem_size, boot_device,
                 idebus[0], idebus[1], floppy_controller, rtc_state);

    if (pci_enabled && usb_enabled) {
        usb_uhci_piix3_init(pci_bus, piix3_devfn + 2);
    }

    if (pci_enabled && acpi_enabled) {
        uint8_t *eeprom_buf = qemu_mallocz(8 * 256); /* XXX: make this persistent */
        i2c_bus *smbus;

        cmos_s3 = qemu_allocate_irqs(pc_cmos_set_s3_resume, rtc_state, 1);
        smi_irq = qemu_allocate_irqs(pc_acpi_smi_interrupt, first_cpu, 1);
        /* TODO: Populate SPD eeprom data.  */
        smbus = piix4_pm_init(pci_bus, piix3_devfn + 3, 0xb100,
                              isa_reserve_irq(9), *cmos_s3, *smi_irq,
                              kvm_enabled());
        for (i = 0; i < 8; i++) {
            DeviceState *eeprom;
            eeprom = qdev_create((BusState *)smbus, "smbus-eeprom");
            qdev_prop_set_uint8(eeprom, "address", 0x50 + i);
            qdev_prop_set_ptr(eeprom, "data", eeprom_buf + (i * 256));
            qdev_init_nofail(eeprom);
        }
    }

    if (i440fx_state) {
        i440fx_init_memory_mappings(i440fx_state);
    }

    if (pci_enabled) {
        pc_pci_device_init(pci_bus);
    }
}

static void pc_init_pci(ram_addr_t ram_size,
                        const char *boot_device,
                        const char *kernel_filename,
                        const char *kernel_cmdline,
                        const char *initrd_filename,
                        const char *cpu_model)
{
    pc_init1(ram_size, boot_device,
             kernel_filename, kernel_cmdline,
             initrd_filename, cpu_model, 1);
}

static void pc_init_isa(ram_addr_t ram_size,
                        const char *boot_device,
                        const char *kernel_filename,
                        const char *kernel_cmdline,
                        const char *initrd_filename,
                        const char *cpu_model)
{
    if (cpu_model == NULL)
        cpu_model = "486";
    pc_init1(ram_size, boot_device,
             kernel_filename, kernel_cmdline,
             initrd_filename, cpu_model, 0);
}

static QEMUMachine pc_machine = {
    .name = "pc-0.14",
    .alias = "pc",
    .desc = "Standard PC",
    .init = pc_init_pci,
    .max_cpus = 255,
    .is_default = 1,
};

static QEMUMachine pc_machine_v0_13 = {
    .name = "pc-0.13",
    .desc = "Standard PC",
    .init = pc_init_pci,
    .max_cpus = 255,
    .compat_props = (GlobalProperty[]) {
        {
            .driver   = "virtio-9p-pci",
            .property = "vectors",
            .value    = stringify(0),
        },{
            .driver   = "VGA",
            .property = "rombar",
            .value    = stringify(0),
        },{
            .driver   = "vmware-svga",
            .property = "rombar",
            .value    = stringify(0),
        },{
            .driver   = "PCI",
            .property = "command_serr_enable",
            .value    = "off",
        },
        { /* end of list */ }
    },
};

static QEMUMachine pc_machine_v0_12 = {
    .name = "pc-0.12",
    .desc = "Standard PC",
    .init = pc_init_pci,
    .max_cpus = 255,
    .compat_props = (GlobalProperty[]) {
        {
            .driver   = "virtio-serial-pci",
            .property = "max_ports",
            .value    = stringify(1),
        },{
            .driver   = "virtio-serial-pci",
            .property = "vectors",
            .value    = stringify(0),
        },{
            .driver   = "VGA",
            .property = "rombar",
            .value    = stringify(0),
        },{
            .driver   = "vmware-svga",
            .property = "rombar",
            .value    = stringify(0),
        },{
            .driver   = "PCI",
            .property = "command_serr_enable",
            .value    = "off",
        },
        { /* end of list */ }
    }
};

static QEMUMachine pc_machine_v0_11 = {
    .name = "pc-0.11",
    .desc = "Standard PC, qemu 0.11",
    .init = pc_init_pci,
    .max_cpus = 255,
    .compat_props = (GlobalProperty[]) {
        {
            .driver   = "virtio-blk-pci",
            .property = "vectors",
            .value    = stringify(0),
        },{
            .driver   = "virtio-serial-pci",
            .property = "max_ports",
            .value    = stringify(1),
        },{
            .driver   = "virtio-serial-pci",
            .property = "vectors",
            .value    = stringify(0),
        },{
            .driver   = "ide-drive",
            .property = "ver",
            .value    = "0.11",
        },{
            .driver   = "scsi-disk",
            .property = "ver",
            .value    = "0.11",
        },{
            .driver   = "PCI",
            .property = "rombar",
            .value    = stringify(0),
        },{
            .driver   = "PCI",
            .property = "command_serr_enable",
            .value    = "off",
        },
        { /* end of list */ }
    }
};

static QEMUMachine pc_machine_v0_10 = {
    .name = "pc-0.10",
    .desc = "Standard PC, qemu 0.10",
    .init = pc_init_pci,
    .max_cpus = 255,
    .compat_props = (GlobalProperty[]) {
        {
            .driver   = "virtio-blk-pci",
            .property = "class",
            .value    = stringify(PCI_CLASS_STORAGE_OTHER),
        },{
            .driver   = "virtio-serial-pci",
            .property = "class",
            .value    = stringify(PCI_CLASS_DISPLAY_OTHER),
        },{
            .driver   = "virtio-serial-pci",
            .property = "max_ports",
            .value    = stringify(1),
        },{
            .driver   = "virtio-serial-pci",
            .property = "vectors",
            .value    = stringify(0),
        },{
            .driver   = "virtio-net-pci",
            .property = "vectors",
            .value    = stringify(0),
        },{
            .driver   = "virtio-blk-pci",
            .property = "vectors",
            .value    = stringify(0),
        },{
            .driver   = "ide-drive",
            .property = "ver",
            .value    = "0.10",
        },{
            .driver   = "scsi-disk",
            .property = "ver",
            .value    = "0.10",
        },{
            .driver   = "PCI",
            .property = "rombar",
            .value    = stringify(0),
        },{
            .driver   = "PCI",
            .property = "command_serr_enable",
            .value    = "off",
        },
        { /* end of list */ }
    },
};

static QEMUMachine isapc_machine = {
    .name = "isapc",
    .desc = "ISA-only PC",
    .init = pc_init_isa,
    .max_cpus = 1,
};

/* NEC PC-9821 */

#define SUPPORT_PC98_FMSOUND

#ifdef SUPPORT_PC98_FMSOUND
#define PC98_NE2000_NB_MAX 1
#else
#define PC98_NE2000_NB_MAX 2
#endif

static void *pc98_sys;
static void *pc98_vga;

static void ioport_set_a20(int enable)
{
    /* XXX: send to all CPUs ? */
    cpu_x86_set_a20(first_cpu, enable);
}

static int ioport_get_a20(void)
{
    return ((first_cpu->a20_mask >> 20) & 1);
}

static void pc98_ioport_f2_write(void *opaque, uint32_t addr, uint32_t data)
{
    ioport_set_a20(1);
}

static uint32_t pc98_ioport_f2_read(void *opaque, uint32_t addr)
{
    return (ioport_get_a20() ^ 1) | 0x2e;
}

static void pc98_ioport_f6_write(void *opaque, uint32_t addr, uint32_t data)
{
    switch (data) {
    case 0x02:
        ioport_set_a20(1);
        break;
    case 0x03:
        ioport_set_a20(0);
        break;
    }
}

static uint32_t pc98_ioport_f6_read(void *opaque, uint32_t addr)
{
    return (ioport_get_a20() ^ 1) | 0x5e;
}

static void pc98_ioport_534_write(void *opaque, uint32_t addr, uint32_t data)
{
    if (data & 0x01) {
        pc98_cpu_shutdown();
    }
}

static uint32_t pc98_ioport_534_read(void *opaque, uint32_t addr)
{
    return 0xec; /* cpu mode */
}

static uint32_t pc98_ioport_9894_read(void *opaque, uint32_t addr)
{
    return 0x90; /* cpu wait */
}

#ifndef SUPPORT_PC98_FMSOUND
static uint32_t pc98_ioport_a460_read(void *opaque, uint32_t addr)
{
    return 0x7f; /* Mate-X PCM */
}
#endif

void pc98_cpu_shutdown(void)
{
    if (pc98_sys_read_shut(pc98_sys)) {
        qemu_system_reset_request();
    } else {
        qemu_cpu_reset_request();
    }
}

void pc98_select_ems(uint32_t value)
{
    pc98_vga_select_ems(pc98_vga, value);
}

/* LGY-98 */
static const int pc98_ne2000_io[2] = { 0xd0, 0x10d0 };
static const int pc98_ne2000_irq[2] = { 6, 3 };

static void pc98_init_ne2k_isa(NICInfo *nd)
{
    static int nb_ne2k = 0;

    if (nb_ne2k == PC98_NE2000_NB_MAX) {
        return;
    }
    pc98_ne2000_init(pc98_ne2000_io[nb_ne2k], pc98_ne2000_irq[nb_ne2k], nd);
    nb_ne2k++;
}

static void pc98_cpu_request_exit(void *opaque, int irq, int level)
{
    CPUState *env = cpu_single_env;

    if (env && level) {
        cpu_exit(env);
    }
}

/* PC-9821 hardware initialisation */
static void pc98_init1(ram_addr_t ram_size, const char *cpu_model,
                       int pci_enabled)
{
    int i;
    PCIBus *pci_bus;
    PCII440FXState *i440fx_state;
    int piix3_devfn = -1;
    qemu_irq *cpu_irq;
    qemu_irq *isa_irq;
    qemu_irq *i8259;
    qemu_irq *cpu_exit_irq;
    IsaIrqState *isa_irq_state;
    DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
    DriveInfo *fd[MAX_FD];
    char cpu_model_opt[64];
    uint8_t hd_connect = 0;
    PITState *pit;

    /* init CPUs */
    if (cpu_model == NULL) {
#ifdef TARGET_X86_64
        cpu_model = "qemu64";
#else
        cpu_model = "qemu32";
#endif
    }
    sprintf(cpu_model_opt, "%s,+pc98_a20mask", cpu_model);
    pc_cpus_init(cpu_model_opt);

    /* allocate ram */
    if (ram_size < 0x1000000) {
        ram_size = 0x1000000; /* >= 16MB */
    }
    ram_size &= ~(0x800000 - 1); /* 8MB * num */

    qemu_ram_alloc(NULL, "pc.pam", 0x100000); /* for PAM and SMRAM */

    cpu_irq = pc_allocate_cpu_irq();
    i8259 = pc98_i8259_init(cpu_irq[0]);
    isa_irq_state = qemu_mallocz(sizeof(*isa_irq_state));
    isa_irq_state->i8259 = i8259;
    if (pci_enabled) {
        ioapic_init(isa_irq_state);
    }
    isa_irq = qemu_allocate_irqs(isa_irq_handler, isa_irq_state, 24);

    if (pci_enabled) {
        pci_bus = pc98_i440fx_init(&i440fx_state, &piix3_devfn, isa_irq, ram_size);
    } else {
        pci_bus = NULL;
        i440fx_state = NULL;
        isa_bus_new(NULL);
    }
    isa_bus_irqs(isa_irq);

    pc_register_ferr_irq(isa_reserve_irq(8));

    /* init drives */
    if (drive_get_max_bus(IF_IDE) >= MAX_IDE_BUS) {
        fprintf(stderr, "qemu: too many IDE bus\n");
        exit(1);
    }

    for(i = 0; i < MAX_IDE_BUS * MAX_IDE_DEVS; i++) {
        hd[i] = drive_get(IF_IDE, i / MAX_IDE_DEVS, i % MAX_IDE_DEVS);
        if(hd[i] != NULL) {
            hd_connect |= (1 << i);
        }
    }

    for(i = 0; i < MAX_FD; i++) {
        fd[i] = drive_get(IF_FLOPPY, 0, i);
    }

    /* init basic PC-9821 hardware */

    /* IRQ 0  PIT
           1  KEYBOARD
           2  CRTV
           3  PC-9801-86
              LGY-98 #2 (I/O=10D0h)
           4  RS-232C
           5  (SCSI)
           6  LGY-98 #1 (I/O=00D0h)
           7  SLAVE PIC
           8  FPU
           9  IDE
           10 FDC (640KB I/F)
           11 FDC (1MB I/F)
           12 CS4231A
           13 MOUSE
           14 
           15 RTC
       DRQ 0  
           1  CS4231A
           2  FDC (1MB I/F)
           3  FDC (640KB I/F)
    */

    register_ioport_write(0xf2, 1, 1, pc98_ioport_f2_write, NULL);
    register_ioport_read(0xf2, 1, 1, pc98_ioport_f2_read, NULL);
    register_ioport_write(0xf6, 1, 1, pc98_ioport_f6_write, NULL);
    register_ioport_read(0xf6, 1, 1, pc98_ioport_f6_read, NULL);
    register_ioport_write(0x534, 1, 1, pc98_ioport_534_write, NULL);
    register_ioport_read(0x534, 1, 1, pc98_ioport_534_read, NULL);
    register_ioport_read(0x9894, 1, 1, pc98_ioport_9894_read, NULL);
#ifdef SUPPORT_PC98_FMSOUND
    /* This port is assigned in pc98fmsound.c */
#else
    register_ioport_read(0xa460, 1, 1, pc98_ioport_a460_read, NULL);
#endif

    pit = pc98_pit_init(isa_reserve_irq(0));
    pc98_pcspk_init(pit);

    cpu_exit_irq = qemu_allocate_irqs(pc98_cpu_request_exit, NULL, 1);
    pc98_DMA_init(cpu_exit_irq);

    pc98_ide_init(hd);
    pc98_fdctrl_init(fd);

//#ifdef HAS_AUDIO
    pcspk_audio_init(isa_irq);
#ifdef SUPPORT_PC98_FMSOUND
    /* 98MULTi CanBe (YMF288 + CS4231) */
    pc98_sound_init();
    pc98_cs4231a_init(isa_irq);
#else
    /* Mate-X PCM */
    pc98_cs4231a_init(isa_irq);
#endif
//#endif

    pc98_kbd_init();
    pc98_mouse_init();
    pc98_serial_init();

    pc98_sys = pc98_sys_init();
    pc98_vga = pc98_vga_init();

    /* memory must be initialized after vga is initialized */
    pc98_mem_init(ram_size, hd_connect, i440fx_state);

    for(i = 0; i < nb_nics; i++) {
        NICInfo *nd = &nd_table[i];

//        if (!pci_enabled || (nd->model && strcmp(nd->model, "ne2k_isa") == 0))
            pc98_init_ne2k_isa(nd);
//        else
//            pci_nic_init_nofail(nd, "e1000", NULL);
    }

//    if (pci_enabled && usb_enabled) {
//        usb_uhci_piix3_init(pci_bus, piix3_devfn + 2);
//    }

    if (i440fx_state) {
        i440fx_init_memory_mappings(i440fx_state);
    }

    if (pci_enabled) {
        pc_pci_device_init(pci_bus);
    }
}

static void pc98_init_pci(ram_addr_t ram_size,
                          const char *boot_device,
                          const char *kernel_filename,
                          const char *kernel_cmdline,
                          const char *initrd_filename,
                          const char *cpu_model)
{
    pc98_init1(ram_size, cpu_model, 1);
}

static void pc98_init_isa(ram_addr_t ram_size,
                          const char *boot_device,
                          const char *kernel_filename,
                          const char *kernel_cmdline,
                          const char *initrd_filename,
                          const char *cpu_model)
{
    if (cpu_model == NULL) {
        cpu_model = "486";
    }
    pc98_init1(ram_size, cpu_model, 0);
}

static QEMUMachine pc98pci_machine = {
    .name = "pc98pci",
    .desc = "NEC PC-9821 with PCI",
    .init = pc98_init_pci,
    .max_cpus = 255,
};

static QEMUMachine pc98_machine = {
    .name = "pc98",
    .desc = "NEC PC-9821",
    .init = pc98_init_isa,
    .max_cpus = 1,
};

static void pc_machine_init(void)
{
    qemu_register_machine(&pc_machine);
    qemu_register_machine(&pc_machine_v0_13);
    qemu_register_machine(&pc_machine_v0_12);
    qemu_register_machine(&pc_machine_v0_11);
    qemu_register_machine(&pc_machine_v0_10);
    qemu_register_machine(&isapc_machine);
    qemu_register_machine(&pc98pci_machine);
    qemu_register_machine(&pc98_machine);
}

machine_init(pc_machine_init);
