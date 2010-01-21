/*
 * QEMU PC-98x1 System Emulator
 *
 * Copyright (c) 2009 TAKEDA, toshiya
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
#include "fdc.h"
#include "pci.h"
#include "block.h"
#include "sysemu.h"
//#include "audio/audio.h"
#include "net.h"
#include "boards.h"
#include "monitor.h"
#include "ide.h"
#include "loader.h"

#define BIOS_FILE_NAME "pc98bios.bin"
#define BIOS_FILE_SIZE 0x18000
#define ITF_FILE_NAME "pc98itf.bin"
#define ITF_FILE_SIZE 0x8000
#define IDE_FILE_NAME "pc98ide.bin"
#define IDE_FILE_SIZE 0x2000

static ram_addr_t ram_addr[512];
static ram_addr_t bios_addr;
static ram_addr_t itf_addr;
static ram_addr_t ide_addr;

static uint8_t bios_ram_selected;
static uint8_t itf_selected;
static uint8_t ram_window_map;

#define MAX_IDE_BUS 2

static fdctrl_t *floppy_controller;
static PITState *pit;

typedef struct isa_irq_state {
    qemu_irq *i8259;
    qemu_irq *ioapic;
} IsaIrqState;

static void isa_irq_handler(void *opaque, int n, int level)
{
    IsaIrqState *isa = (IsaIrqState *)opaque;

    if (n < 16) {
        qemu_set_irq(isa->i8259[n], level);
    }
    if (isa->ioapic) {
        qemu_set_irq(isa->ioapic[n], level);
    }
};

/* IRQ handling */
static void pic_irq_request(void *opaque, int irq, int level)
{
    CPUState *env = first_cpu;

    if (env->apic_state) {
        while (env) {
            if (apic_accept_pic_intr(env))
                apic_deliver_pic_intr(env, level);
            env = env->next_cpu;
        }
    } else {
        if (level)
            cpu_interrupt(env, CPU_INTERRUPT_HARD);
        else
            cpu_reset_interrupt(env, CPU_INTERRUPT_HARD);
    }
}

static CPUState *pc_new_cpu(const char *cpu_model)
{
    CPUState *env;

    env = cpu_init(cpu_model);
    if (!env) {
        fprintf(stderr, "Unable to find x86 CPU definition\n");
        exit(1);
    }
    if ((env->cpuid_features & CPUID_APIC) || smp_cpus > 1) {
        env->cpuid_apic_id = env->cpu_index;
        /* APIC reset callback resets cpu */
        apic_init(env);
    } else {
        qemu_register_reset((QEMUResetHandler*)cpu_reset, env);
    }
    return env;
}

static void pc98_reset(void *opaque)
{
    if (bios_ram_selected || !itf_selected) {
        cpu_register_physical_memory(0x000d8000, 0x2000, ide_addr | IO_MEM_ROM);
        cpu_register_physical_memory(0x000f8000, 0x8000, itf_addr | IO_MEM_ROM);
        cpu_register_physical_memory(0xfffd8000, 0x2000, ide_addr | IO_MEM_ROM);
        cpu_register_physical_memory(0xffff8000, 0x8000, itf_addr | IO_MEM_ROM);
        bios_ram_selected = 0;
        itf_selected = 1;
    }
    if (ram_window_map != 0x08) {
        cpu_register_physical_memory(0x00080000, 0x8000, ram_addr[16]);
        cpu_register_physical_memory(0x00088000, 0x8000, ram_addr[17]);
        cpu_register_physical_memory(0x00090000, 0x8000, ram_addr[18]);
        cpu_register_physical_memory(0x00098000, 0x8000, ram_addr[19]);
        cpu_register_physical_memory(0xfff80000, 0x8000, ram_addr[20]);
        cpu_register_physical_memory(0xfff88000, 0x8000, ram_addr[21]);
        cpu_register_physical_memory(0xfff90000, 0x8000, ram_addr[22]);
        cpu_register_physical_memory(0xfff98000, 0x8000, ram_addr[23]);
        ram_window_map = 0x08;
    }
}

static void ioport_f2_write(void *opaque, uint32_t addr, uint32_t data)
{
    ioport_set_a20(1);
}

static uint32_t ioport_f2_read(void *opaque, uint32_t addr)
{
    return (ioport_get_a20() ^ 1) | 0x2e;
}

static void ioport_f6_write(void *opaque, uint32_t addr, uint32_t data)
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

static uint32_t ioport_f6_read(void *opaque, uint32_t addr)
{
    return (ioport_get_a20() ^ 1) | 0x5e;
}

static void ioport_43d_write(void *opaque, uint32_t addr, uint32_t data)
{
    switch (data) {
    case 0x00:
    case 0x10:
        if (!itf_selected) {
            cpu_register_physical_memory(0x000f8000, 0x8000, itf_addr | IO_MEM_ROM);
            cpu_register_physical_memory(0xffff8000, 0x8000, itf_addr | IO_MEM_ROM);
            itf_selected = 1;
        }
        break;
    case 0x02:
    case 0x12:
        if (itf_selected) {
            if (bios_ram_selected) {
                cpu_register_physical_memory(0x000e8000, 0x8000, ram_addr[0xe8000 >> 15]);
                cpu_register_physical_memory(0x000f0000, 0x8000, ram_addr[0xf0000 >> 15]);
                cpu_register_physical_memory(0x000f8000, 0x8000, ram_addr[0xf8000 >> 15]);
                cpu_register_physical_memory(0xfffe8000, 0x8000, ram_addr[0xe8000 >> 15]);
                cpu_register_physical_memory(0xffff0000, 0x8000, ram_addr[0xf0000 >> 15]);
                cpu_register_physical_memory(0xffff8000, 0x8000, ram_addr[0xf8000 >> 15]);
            } else {
                cpu_register_physical_memory(0x000e8000, 0x18000, bios_addr | IO_MEM_ROM);
                cpu_register_physical_memory(0xfffe8000, 0x18000, bios_addr | IO_MEM_ROM);
            }
            itf_selected = 0;
        }
        break;
    }
}

static void ioport_461_write(void *opaque, uint32_t addr, uint32_t data)
{
    if ((ram_window_map & 0xfe) != (data & 0xfe)) {
        uint32_t bank = (data & 0xfe) << 1;
        cpu_register_physical_memory(0x00080000, 0x8000, ram_addr[bank + 0]);
        cpu_register_physical_memory(0x00088000, 0x8000, ram_addr[bank + 1]);
        cpu_register_physical_memory(0x00090000, 0x8000, ram_addr[bank + 2]);
        cpu_register_physical_memory(0x00098000, 0x8000, ram_addr[bank + 3]);
        cpu_register_physical_memory(0xfff80000, 0x8000, ram_addr[bank + 0]);
        cpu_register_physical_memory(0xfff88000, 0x8000, ram_addr[bank + 1]);
        cpu_register_physical_memory(0xfff90000, 0x8000, ram_addr[bank + 2]);
        cpu_register_physical_memory(0xfff98000, 0x8000, ram_addr[bank + 3]);
        ram_window_map = data;
    }
}

static uint32_t ioport_461_read(void *opaque, uint32_t addr)
{
    return ram_window_map;
}

static uint32_t ioport_534_read(void *opaque, uint32_t addr)
{
    return 0xec;
}

static void ioport_53d_write(void *opaque, uint32_t addr, uint32_t data)
{
    if (!bios_ram_selected && (data & 0x02)) {
        cpu_register_physical_memory(0x000c0000, 0x8000, ram_addr[0xc0000 >> 15]);
        cpu_register_physical_memory(0x000c8000, 0x8000, ram_addr[0xc8000 >> 15]);
        cpu_register_physical_memory(0x000d0000, 0x8000, ram_addr[0xd0000 >> 15]);
        cpu_register_physical_memory(0x000d8000, 0x8000, ram_addr[0xd8000 >> 15]);
        cpu_register_physical_memory(0xfffc0000, 0x8000, ram_addr[0xc0000 >> 15]);
        cpu_register_physical_memory(0xfffc8000, 0x8000, ram_addr[0xc8000 >> 15]);
        cpu_register_physical_memory(0xfffd0000, 0x8000, ram_addr[0xd0000 >> 15]);
        cpu_register_physical_memory(0xfffd8000, 0x8000, ram_addr[0xd8000 >> 15]);
        if (!itf_selected) {
            cpu_register_physical_memory(0x000e8000, 0x8000, ram_addr[0xe8000 >> 15]);
            cpu_register_physical_memory(0x000f0000, 0x8000, ram_addr[0xf0000 >> 15]);
            cpu_register_physical_memory(0x000f8000, 0x8000, ram_addr[0xf8000 >> 15]);
            cpu_register_physical_memory(0xfffe8000, 0x8000, ram_addr[0xe8000 >> 15]);
            cpu_register_physical_memory(0xffff0000, 0x8000, ram_addr[0xf0000 >> 15]);
            cpu_register_physical_memory(0xffff8000, 0x8000, ram_addr[0xf8000 >> 15]);
        }
        bios_ram_selected = 1;
    } else if (bios_ram_selected && !(data & 0x02)) {
        cpu_register_physical_memory(0x000d8000, 0x2000, ide_addr | IO_MEM_ROM);
        cpu_register_physical_memory(0xfffd8000, 0x2000, ide_addr | IO_MEM_ROM);
        if (!itf_selected) {
            cpu_register_physical_memory(0x000e8000, 0x18000, bios_addr | IO_MEM_ROM);
            cpu_register_physical_memory(0xfffe8000, 0x18000, bios_addr | IO_MEM_ROM);
        }
        bios_ram_selected = 0;
    }
}

static uint32_t ioport_63d_read(void *opaque, uint32_t addr)
{
    return 0x04;
}

static uint32_t ioport_9894_read(void *opaque, uint32_t addr)
{
    return 0x90;
}

/* PC hardware initialisation */
static void pc98_init1(ram_addr_t ram_size,
                       const char *cpu_model,
                       int pci_enabled)
{
    char cpu_model_opt[64];
    char *filename;
    int bios_size = -1;
    int itf_size = -1;
    int ide_size = -1;
    int i;
    uint32_t addr;
    PCIBus *pci_bus;
    int piix3_devfn = -1;
    CPUState *env = NULL;
    qemu_irq *cpu_irq;
    qemu_irq *isa_irq;
    qemu_irq *i8259;
    IsaIrqState *isa_irq_state;
    DriveInfo *dinfo;
    DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
    BlockDriverState *fd[MAX_FD];

    /* init CPUs */
    if (cpu_model == NULL) {
#ifdef TARGET_X86_64
        cpu_model = "qemu64";
#else
        cpu_model = "qemu32";
#endif
    }
    sprintf(cpu_model_opt, "%s,+a20mask", cpu_model);

    for (i = 0; i < smp_cpus; i++) {
        env = pc_new_cpu(cpu_model_opt);
    }

    /* allocate RAM */
    for (i = 0; i < 512; i++) {
        ram_addr[i] = qemu_ram_alloc(0x8000);
    }
    for (addr = 0; addr < 0xa0000; addr += 0x8000) {
        cpu_register_physical_memory(0x00000000 | addr, 0x8000, ram_addr[addr >> 15]);
        cpu_register_physical_memory(0xfff00000 | addr, 0x8000, ram_addr[addr >> 15]);
    }
    for (addr = 0x100000; addr < 0x1000000; addr += 0x8000) {
        cpu_register_physical_memory(0x00000000 | addr, 0x8000, ram_addr[addr >> 15]);
    }
    if (ram_size > 0x1000000) {
        ram_addr_t ram_addr = qemu_ram_alloc(ram_size - 0x1000000);
        cpu_register_physical_memory(0x1000000, ram_size - 0x1000000, ram_addr);
    }

    /* BIOS load */
    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, BIOS_FILE_NAME);
    bios_addr = qemu_ram_alloc(BIOS_FILE_SIZE);
    if (filename) {
        bios_size = load_image(filename, qemu_get_ram_ptr(bios_addr));
        qemu_free(filename);
    }
    if (bios_size != BIOS_FILE_SIZE) {
        fprintf(stderr, "qemu: could not load PC-9821 BIOS '%s'\n", filename);
        exit(1);
    }
    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, ITF_FILE_NAME);
    itf_addr = qemu_ram_alloc(ITF_FILE_SIZE);
    if (filename) {
        itf_size = load_image(filename, qemu_get_ram_ptr(itf_addr));
        qemu_free(filename);
    }
    if (itf_size != ITF_FILE_SIZE) {
        fprintf(stderr, "qemu: could not load PC-9821 ITF '%s'\n", filename);
        exit(1);
    }
    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, IDE_FILE_NAME);
    ide_addr = qemu_ram_alloc(IDE_FILE_SIZE);
    if (filename) {
        ide_size = load_image(filename, qemu_get_ram_ptr(ide_addr));
        qemu_free(filename);
    }
    if (ide_size != IDE_FILE_SIZE) {
        fprintf(stderr, "qemu: could not load PC-9821 IDE BIOS '%s'\n", filename);
        exit(1);
    }

    cpu_register_physical_memory(0x000d8000, 0x2000, ide_addr | IO_MEM_ROM);
    cpu_register_physical_memory(0x000f8000, 0x8000, itf_addr | IO_MEM_ROM);
    cpu_register_physical_memory(0xfffd8000, 0x2000, ide_addr | IO_MEM_ROM);
    cpu_register_physical_memory(0xffff8000, 0x8000, itf_addr | IO_MEM_ROM);

    bios_ram_selected = 0;
    itf_selected = 1;
    ram_window_map = 0x08;

    cpu_irq = qemu_allocate_irqs(pic_irq_request, NULL, 1);
    i8259 = pc98_i8259_init(cpu_irq[0]);
    isa_irq_state = qemu_mallocz(sizeof(*isa_irq_state));
    isa_irq_state->i8259 = i8259;
    isa_irq = qemu_allocate_irqs(isa_irq_handler, isa_irq_state, 24);

    if (pci_enabled) {
        pci_bus = pc98_i440fx_init(&i440fx_state, &piix3_devfn, isa_irq);
    } else {
        pci_bus = NULL;
        isa_bus_new(NULL);
    }
    isa_bus_irqs(isa_irq);

    ferr_irq = isa_reserve_irq(8);

    /* init basic PC-98x1 hardware */
    register_ioport_write(0xf2, 1, 1, ioport_f2_write, env);
    register_ioport_read(0xf2, 1, 1, ioport_f2_read, env);
    register_ioport_write(0xf6, 1, 1, ioport_f6_write, env);
    register_ioport_read(0xf6, 1, 1, ioport_f6_read, env);
    register_ioport_write(0x43d, 1, 1, ioport_43d_write, env);
    register_ioport_write(0x461, 1, 1, ioport_461_write, env);
    register_ioport_read(0x461, 1, 1, ioport_461_read, env);
    register_ioport_read(0x534, 1, 1, ioport_534_read, env);
    register_ioport_write(0x53d, 1, 1, ioport_53d_write, env);
    register_ioport_read(0x63d, 1, 1, ioport_63d_read, env);
    register_ioport_read(0x9894, 1, 1, ioport_9894_read, env);

    qemu_register_reset(pc98_reset, env);

    if (pci_enabled) {
        isa_irq_state->ioapic = ioapic_init();
    }
    pit = pc98_pit_init(isa_reserve_irq(0));
//    pcspk_init(pit);

    piix4_acpi_system_hot_add_init();

    if (drive_get_max_bus(IF_IDE) >= MAX_IDE_BUS) {
        fprintf(stderr, "qemu: too many IDE bus\n");
        exit(1);
    }

    for(i = 0; i < MAX_IDE_BUS * MAX_IDE_DEVS; i++) {
        hd[i] = drive_get(IF_IDE, i / MAX_IDE_DEVS, i % MAX_IDE_DEVS);
    }

//    if (pci_enabled) {
//        pci_piix3_ide_init(pci_bus, hd, piix3_devfn + 1);
//    } else {
        pc98_ide_init(9, hd[0], hd[1], hd[2], hd[3]);
//    }

    pc98_DMA_init(1);

    isa_create_simple("pc98-kbd");
    isa_create_simple("pc98-mouse");
    isa_create_simple("pc98-sys");
    isa_create_simple("pc98-vga");

    for(i = 0; i < MAX_FD; i++) {
        dinfo = drive_get(IF_FLOPPY, 0, i);
        fd[i] = dinfo ? dinfo->bdrv : NULL;
    }
    floppy_controller = pc98_fdctrl_init(fd);

//    if (pci_enabled && usb_enabled) {
//        usb_uhci_piix3_init(pci_bus, piix3_devfn + 2);
//    }

    if (i440fx_state) {
        i440fx_init_memory_mappings(i440fx_state);
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
    pc98_init1(ram_size, cpu_model, 0);
}

static QEMUMachine pc98pci_machine = {
    .name = "pc98pci",
    .desc = "NEC PC-9821 with PCI",
    .init = pc98_init_pci,
    .max_cpus = 2,
};

static QEMUMachine pc98_machine = {
    .name = "pc98",
    .desc = "NEC PC-9821",
    .init = pc98_init_isa,
    .max_cpus = 1,
};

static void pc98_machine_init(void)
{
    qemu_register_machine(&pc98pci_machine);
    qemu_register_machine(&pc98_machine);
}

machine_init(pc98_machine_init);
