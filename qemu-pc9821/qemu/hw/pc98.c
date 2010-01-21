/*
 * QEMU NEC PC-9821 System Emulator
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

#define IDE_FILE_NAME "pc98ide.bin"
#define ITF_FILE_NAME "pc98itf.bin"
#define BIOS_FILE_NAME "pc98bios.bin"
#define BANK_FILE_NAME "pc98bank%d.bin"

#define IDE_FILE_SIZE 0x2000
#define ITF_FILE_SIZE 0x8000
#define BIOS_FILE_SIZE 0x18000
#define BANK_FILE_SIZE 0x8000

enum {
    PCI_BANK  = 0,
    IDE_BANK  = 3,
    ITF_BANK  = 4,
    BIOS_BANK = 5,
    NONE_BANK = 8,
};
#define PCI_BANK_OFS (PCI_BANK * 0x8000)
#define IDE_BANK_OFS (IDE_BANK * 0x8000)
#define ITF_BANK_OFS (ITF_BANK * 0x8000)
#define BIOS_BANK_OFS (BIOS_BANK * 0x8000)
#define NONE_BANK_OFS (NONE_BANK * 0x8000)

#define REQUIRED_BANK ((1 << IDE_BANK) | (1 << ITF_BANK) | (7 << BIOS_BANK))

#define BANK_BITS 13
/* (1 << BANK_BITS) */
#define BANK_SIZE 0x2000

#define BANK_ROM 0x8000

static ram_addr_t ram_addr[2048]; /* 16MB  >> BANK_BITS     */
static ram_addr_t rom_addr[33];   /* 256KB >> BANK_BITS + 1 */
static uint16_t mem_bank[128];    /* 1MB   >> BANK_BITS     */

static uint8_t ide_selected = 1;
static uint8_t ide_ram_selected = 1;
static uint8_t pci_selected = 0;
static uint8_t bios_ram_selected = 0;
static uint8_t itf_selected = 1;
static uint8_t ram_window_map = 8;
static uint8_t hd_connect = 0;

#define MAX_IDE_BUS 2

static fdctrl_t *floppy_controller;
static PITState *pit;

typedef struct isa_irq_state {
    qemu_irq *i8259;
    qemu_irq *ioapic;
} IsaIrqState;

/* IRQ handling */
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

/* Ethernet */
#define NE2000_NB_MAX 4

static const int ne2000_io[NE2000_NB_MAX] = { 0x00d0, 0x10d0, 0x20d0, 0x30d0 };
static const int ne2000_irq[NE2000_NB_MAX] = { 3, 5, 6, 12 };

static void pc98_init_ne2k_isa(NICInfo *nd)
{
    static int nb_ne2k = 0;

    if (nb_ne2k == NE2000_NB_MAX) {
        return;
    }
    pc98_ne2000_init(ne2000_io[nb_ne2k], ne2000_irq[nb_ne2k], nd);
    nb_ne2k++;
}

/* CPU */
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

/* Memory */
static uint8_t *get_ram_ptr(uint32_t addr)
{
    uint8_t *ram = qemu_get_ram_ptr(ram_addr[addr >> BANK_BITS]);
    return &ram[addr & (BANK_SIZE - 1)];
}

static void register_ram(uint32_t top, uint32_t size, uint32_t addr)
{
    uint32_t a;
    for (a = top; a < top + size; a += BANK_SIZE) {
        if ((a >= 0xa0000 && a < 0xc0000) || (a >= 0xe0000 && a < 0xe8000)) {
            continue;
        }
        if (mem_bank[a >> BANK_BITS] != (addr >> BANK_BITS)) {
            ram_addr_t mem = ram_addr[addr >> BANK_BITS];
            cpu_register_physical_memory(a, BANK_SIZE, mem);
            if (a >= 0xc0000 && a < 0x100000) {
#ifdef PC98_USE_16MB_AREA
                cpu_register_physical_memory(0x00f00000 | a, BANK_SIZE, mem);
#endif
                cpu_register_physical_memory(0xfff00000 | a, BANK_SIZE, mem);
            }
            mem_bank[a >> BANK_BITS] = (addr >> BANK_BITS);
        }
        addr += BANK_SIZE;
    }
}

static void register_rom(uint32_t top, uint32_t size, uint32_t addr)
{
    uint32_t a;
    for (a = top; a < top + size; a += BANK_SIZE) {
        if ((a >= 0xa0000 && a < 0xc0000) || (a >= 0xe0000 && a < 0xe8000)) {
            continue;
        }
        if (mem_bank[a >> BANK_BITS] != ((addr >> BANK_BITS) | BANK_ROM)) {
            ram_addr_t mem = rom_addr[addr >> BANK_BITS] | IO_MEM_ROM;
            cpu_register_physical_memory(a, BANK_SIZE, mem);
            if (a >= 0xc0000 && a < 0x100000) {
#ifdef PC98_USE_16MB_AREA
                cpu_register_physical_memory(0x00f00000 | a, BANK_SIZE, mem);
#endif
                cpu_register_physical_memory(0xfff00000 | a, BANK_SIZE, mem);
            }
            mem_bank[a >> BANK_BITS] = ((addr >> BANK_BITS) | BANK_ROM);
        }
        if (addr != NONE_BANK_OFS) {
            addr += BANK_SIZE;
        }
    }
}

static void register_ide_rom(void)
{
    if (ide_ram_selected) {
        register_rom(0xd8000, 0x2000, IDE_BANK_OFS);
        register_ram(0xda000, 0x2000, 0xda000);
        register_rom(0xdc000, 0x4000, NONE_BANK_OFS);
    } else {
        register_rom(0xd8000, 0x2000, IDE_BANK_OFS);
        register_rom(0xda000, 0x6000, NONE_BANK_OFS);
    }
}

static void register_pci_rom(void)
{
    register_rom(0xd8000, 0x8000, PCI_BANK_OFS);
}

static void register_bios_rom(void)
{
    register_rom(0xe8000, 0x18000, BIOS_BANK_OFS);
}

static void register_bios_ram(void)
{
    register_ram(0xe8000, 0x18000, 0xe8000);
}

static void register_itf_rom(void)
{
    register_rom(0xe8000, 0x10000, NONE_BANK_OFS);
    register_rom(0xf8000, 0x8000, ITF_BANK_OFS);
}

/* I/O */
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

static uint32_t ioport_43b_read(void *opaque, uint32_t addr)
{
#ifdef PC98_USE_16MB_AREA
    return 0x00;
#else
    return 0x04;
#endif
}

static void ioport_43d_write(void *opaque, uint32_t addr, uint32_t data)
{
    switch (data) {
    case 0x00:
    case 0x10:
        if (!itf_selected) {
            itf_selected = 1;
            register_itf_rom();
        }
        break;
    case 0x02:
    case 0x12:
        if (itf_selected) {
            itf_selected = 0;
            if (bios_ram_selected) {
                if (hd_connect) {
                    /* IDE BIOS patch */
                    if (hd_connect & 1) {
                        *get_ram_ptr(0x457) = 0x90;
                        *get_ram_ptr(0x45d) |= 0x08;
                        *get_ram_ptr(0x55d) |= 0x01;
                    } else {
                        *get_ram_ptr(0x457) = 0x38;
                    }
                    if (hd_connect & 2) {
                        *get_ram_ptr(0x457) |= 0x42;
                        *get_ram_ptr(0x45d) |= 0x10;
                        *get_ram_ptr(0x55d) |= 0x02;
                    } else {
                        *get_ram_ptr(0x457) |= 0x07;
                    }
                    *get_ram_ptr(0xf8e90) |= (hd_connect & 0x0f);
                }
                register_bios_ram();
            } else {
                register_bios_rom();
            }
        }
        break;
    }
}

static void ioport_461_write(void *opaque, uint32_t addr, uint32_t data)
{
    if (ram_window_map != (data & 0xfe)) {
        ram_window_map = data & 0xfe;
        register_ram(0x80000, 0x20000, ram_window_map * 0x10000);
    }
}

static uint32_t ioport_461_read(void *opaque, uint32_t addr)
{
    return ram_window_map;
}

static uint32_t ioport_463_read(void *opaque, uint32_t addr)
{
    return 0x0a;
}

static uint32_t ioport_534_read(void *opaque, uint32_t addr)
{
    return 0xec;
}

static void ioport_53d_write(void *opaque, uint32_t addr, uint32_t data)
{
    if (!bios_ram_selected && (data & 0x02)) {
        bios_ram_selected = 1;
        if (!itf_selected) {
            register_bios_ram();
        }
    } else if (bios_ram_selected && !(data & 0x02)) {
        bios_ram_selected = 0;
        if (!itf_selected) {
            register_bios_rom();
        }
    }
}

static void ioport_63c_write(void *opaque, uint32_t addr, uint32_t data)
{
    switch (data & 0x03) {
    case 1:
        if (!ide_selected) {
            ide_selected = 1;
            register_ide_rom();
        }
        pci_selected = 0;
        break;
    case 2:
        if (!pci_selected) {
            pci_selected = 1;
            register_pci_rom();
        }
        ide_selected = 0;
        break;
    }
}

static uint32_t ioport_63c_read(void *opaque, uint32_t addr)
{
    if (ide_selected) {
        return 0x01;
    } else if (pci_selected) {
        return 0x02;
    }
    return 0x00;
}

static uint32_t ioport_63d_read(void *opaque, uint32_t addr)
{
    return 0x04;
}

static void ioport_1e8e_write(void *opaque, uint32_t addr, uint32_t data)
{
    switch (data) {
    case 0x80:
        if (ide_ram_selected) {
            ide_ram_selected = 0;
            if (ide_selected) {
                register_ide_rom();
            }
        }
        break;
    case 0x81:
        if (!ide_ram_selected) {
            ide_ram_selected = 1;
            if (ide_selected) {
                register_ide_rom();
            }
        }
        break;
    }
}

static uint32_t ioport_1e8e_read(void *opaque, uint32_t addr)
{
    if (ide_ram_selected) {
        return 0xdf;
    } else {
        return 0xde;
    }
}

static uint32_t ioport_9894_read(void *opaque, uint32_t addr)
{
    return 0x90;
}

static void pc98_save(QEMUFile* f, void *opaque)
{
    qemu_put_8s(f, &ide_selected);
    qemu_put_8s(f, &ide_ram_selected);
    qemu_put_8s(f, &pci_selected);
    qemu_put_8s(f, &bios_ram_selected);
    qemu_put_8s(f, &itf_selected);
    qemu_put_8s(f, &ram_window_map);
    qemu_put_buffer(f, (const uint8_t *)mem_bank, sizeof(mem_bank));
}

static int pc98_load(QEMUFile* f, void *opaque, int version_id)
{
    uint32_t a;

    qemu_get_8s(f, &ide_selected);
    qemu_get_8s(f, &ide_ram_selected);
    qemu_get_8s(f, &pci_selected);
    qemu_get_8s(f, &bios_ram_selected);
    qemu_get_8s(f, &itf_selected);
    qemu_get_8s(f, &ram_window_map);
    qemu_get_buffer(f, (uint8_t *)mem_bank, sizeof(mem_bank));

    /* restore memory bank */
    for (a = 0; a < 0x10000; a += BANK_SIZE) {
        uint16_t bank = mem_bank[a >> BANK_BITS];
        ram_addr_t mem;
        if ((a >= 0xa0000 && a < 0xc0000) || (a >= 0xe000 && a < 0xe8000)) {
            /* vram area */
            continue;
        }
        if (bank & BANK_ROM) {
            mem = rom_addr[bank & ~BANK_ROM] | IO_MEM_ROM;
        } else {
            mem = ram_addr[bank];
        }
        cpu_register_physical_memory(a, BANK_SIZE, mem);
        if (a >= 0xc0000 && a < 0x100000) {
#ifdef PC98_USE_16MB_AREA
            cpu_register_physical_memory(0x00f00000 | a, BANK_SIZE, mem);
#endif
            cpu_register_physical_memory(0xfff00000 | a, BANK_SIZE, mem);
        }
    }

    return 0;
}

static void pc98_reset(void *opaque)
{
    if (!(ide_selected && ide_ram_selected)) {
        ide_selected = 1;
        ide_ram_selected = 1;
        pci_selected = 0;
        register_ide_rom();
    }
    if (!itf_selected || bios_ram_selected) {
       itf_selected = 1;
        bios_ram_selected = 0;
        register_itf_rom();
    }
    if (ram_window_map != 0x08) {
        ram_window_map = 0x08;
        register_ram(0x80000, 0x20000, 0x80000);
    }
}

static int patch_itf(uint8_t buf[], const char msg[])
{
    uint8_t op[12] = {
        0xbe, 0x00, 0x00, //     mov si,msg
        0xbd, 0x00, 0x00, //     mov bp,retaddr
        0xe9, 0x00, 0x00, //     jmp disp
                          // retaddr:
        0xf4,             //     hlt
        0xeb, 0xfe,       //     jmp $-2
    };
    int len = strlen(msg);
    int i, a1, a2;
    int ret = 0;

    for (a1 = 1; a1 < 0x8000 - len; a1++) {
        for (i = 0; i < len; i++) {
            if (buf[a1 + i] != msg[i]) {
                break;
            }
        }
        if (i == len && buf[a1 - 1] == 0x41) {
            op[1] = (a1 - 1) & 0xff;
            op[2] = (a1 - 1) >> 8;
            for (a2 = 0; a2 < 0x8000 - 12; a2++) {
                op[4] = (a2 + 9) & 0xff;
                op[5] = (a2 + 9) >> 8;
                for (i = 0; i < 12; i++) {
                    if (!(buf[a2 + i] == op[i] || op[i] == 0x00)) {
                        break;
                    }
                }
                if (i == 12) {
                    memset(buf + a2, 0x90, 12);
                    ret = 1;
                }
            }
        }
    }
    return ret;
}

/* PC-9821 hardware initialisation */
static void pc98_init1(ram_addr_t ram_size,
                       const char *cpu_model,
                       int pci_enabled)
{
    char cpu_model_opt[64];
    uint8_t *buf;
    char filename[16], *filepath;
    uint8_t loaded = 0;
    int i, j;
    uint32_t a;
    PCIBus *pci_bus;
    int piix3_devfn = -1;
    CPUState *env = NULL;
    qemu_irq *cpu_irq;
    qemu_irq *isa_irq;
    qemu_irq *i8259;
    IsaIrqState *isa_irq_state;
    DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
    DriveInfo *fd[MAX_FD];

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

    /* init memory */
    for (i = 0; i < 2048; i++) {
        ram_addr[i] = qemu_ram_alloc(BANK_SIZE);
    }
    for (i = 0; i < 33; i++) {
        rom_addr[i] = qemu_ram_alloc(BANK_SIZE);
    }
    for (i = 0; i < 128; i++) {
        mem_bank[i] = -1;
    }

    buf = qemu_malloc(0x42000);
    memset(buf, 0xff, 0x42000);
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, IDE_FILE_NAME);
    if (filepath) {
        if (load_image(filepath, buf + IDE_BANK_OFS) == IDE_FILE_SIZE) {
            loaded |= (1 << IDE_BANK);
        }
        qemu_free(filepath);
    }
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, ITF_FILE_NAME);
    if (filepath) {
        if (load_image(filepath, buf + ITF_BANK_OFS) == ITF_FILE_SIZE) {
            loaded |= (1 << ITF_BANK);
        }
        qemu_free(filepath);
    }
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, BIOS_FILE_NAME);
    if (filepath) {
        if (load_image(filepath, buf + BIOS_BANK_OFS) == BIOS_FILE_SIZE) {
            loaded |= (7 << BIOS_BANK);
        }
        qemu_free(filepath);
    }
    for (i = 0; i < 8; i++) {
        sprintf(filename, BANK_FILE_NAME, i);
        filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, filename);
        if (filepath) {
            if (load_image(filepath, buf + 0x8000 * i) == BANK_FILE_SIZE) {
                loaded |= (1 << i);
            }
            qemu_free(filepath);
        }
    }
    if ((loaded & REQUIRED_BANK) == REQUIRED_BANK) {
        /* ITF: ignore TIMER INTERRUPT ERROR */
        uint8_t *itf = buf + ITF_BANK_OFS;
        int patched = 0;
        patched |= patch_itf(itf, "TIMER INTERRUPT ERROR");
        if (patched) {
            uint8_t l = 0, h = 0;
            for (j = 0; j < 0x8000; j += 2) {
                l += itf[j + 0];
                h += itf[j + 1];
            }
            itf[0x7ffe] -= l;
            itf[0x7fff] -= h;
        }
        /* copy to rom_addr */
        for (i = 0; i < 33; i++) {
            memcpy(qemu_get_ram_ptr(rom_addr[i]), buf + BANK_SIZE * i, BANK_SIZE);
        }
    } else {
        fprintf(stderr, "qemu: could not load PC-9821 BIOS\n");
        exit(1);
    }
    qemu_free(buf);

    /* 0x0a0000 - 0x09ffff : ram */
    register_ram(0, 0xa0000, 0);
    /* 0x0a0000 - 0x0bffff : vram */
    /* 0x0c0000 - 0x0d7fff : none */
    register_rom(0xc0000, 0x18000, NONE_BANK_OFS);
    /* 0x0d8000 - 0x0dffff : ide/pci bios */
    register_ide_rom();
    /* 0x0e0000 - 0x0e7fff : vram */
    /* 0x0e8000 - 0x0fffff : itf/bios */
    register_itf_rom();
    /* 0x100000-           : ram */
#ifdef PC98_USE_16MB_AREA
    for (a = 0x100000; a < 0x0f00000; a += BANK_SIZE) {
#else
    for (a = 0x100000; a < 0x1000000; a += BANK_SIZE) {
#endif
        cpu_register_physical_memory(a, BANK_SIZE, ram_addr[a >> BANK_BITS]);
    }
    if (ram_size > 0x1000000) {
        ram_addr_t mem = qemu_ram_alloc(ram_size - 0x1000000);
        cpu_register_physical_memory(0x1000000, ram_size - 0x1000000, mem);
    }

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

    /* init basic PC-9821 hardware */
    register_ioport_write(0xf2, 1, 1, ioport_f2_write, env);
    register_ioport_read(0xf2, 1, 1, ioport_f2_read, env);
    register_ioport_write(0xf6, 1, 1, ioport_f6_write, env);
    register_ioport_read(0xf6, 1, 1, ioport_f6_read, env);
    register_ioport_read(0x43b, 1, 1, ioport_43b_read, env);
    register_ioport_write(0x43d, 1, 1, ioport_43d_write, env);
    register_ioport_write(0x461, 1, 1, ioport_461_write, env);
    register_ioport_read(0x461, 1, 1, ioport_461_read, env);
    register_ioport_read(0x463, 1, 1, ioport_463_read, env);
    register_ioport_read(0x534, 1, 1, ioport_534_read, env);
    register_ioport_write(0x53d, 1, 1, ioport_53d_write, env);
    register_ioport_write(0x63c, 1, 1, ioport_63c_write, env);
    register_ioport_read(0x63c, 1, 1, ioport_63c_read, env);
    register_ioport_read(0x63d, 1, 1, ioport_63d_read, env);
    register_ioport_write(0x1e8e, 1, 1, ioport_1e8e_write, env);
    register_ioport_read(0x1e8e, 1, 1, ioport_1e8e_read, env);
    register_ioport_read(0x9894, 1, 1, ioport_9894_read, env);

    qemu_register_reset(pc98_reset, env);
    register_savevm("pc98", 0, 1, pc98_save, pc98_load, env);

    if (pci_enabled) {
        isa_irq_state->ioapic = ioapic_init();
    }
    pit = pc98_pit_init(isa_reserve_irq(0));
    pc98_pcspk_init(pit);
#ifdef HAS_AUDIO
    pcspk_audio_init(isa_irq);
#endif

    for(i = 0; i < nb_nics; i++) {
        NICInfo *nd = &nd_table[i];

//        if (!pci_enabled || (nd->model && strcmp(nd->model, "ne2k_isa") == 0)) {
            pc98_init_ne2k_isa(nd);
//        } else {
//            pci_nic_init(nd, "ne2k_pci", NULL);
//        }
    }

    if (drive_get_max_bus(IF_IDE) >= MAX_IDE_BUS) {
        fprintf(stderr, "qemu: too many IDE bus\n");
        exit(1);
    }

    for(i = 0; i < MAX_IDE_BUS * MAX_IDE_DEVS; i++) {
        hd[i] = drive_get(IF_IDE, i / MAX_IDE_DEVS, i % MAX_IDE_DEVS);
        if (hd[i]) {
            hd_connect |= (1 << i);
        }
    }
    pc98_ide_init(9, hd);

    pc98_DMA_init(1);

    isa_create_simple("pc98-kbd");
    isa_create_simple("pc98-mouse");
    isa_create_simple("pc98-sys");
    isa_create_simple("pc98-vga");

    for(i = 0; i < MAX_FD; i++) {
        fd[i] = drive_get(IF_FLOPPY, 0, i);
    }
    floppy_controller = pc98_fdctrl_init(fd);

//    if (pci_enabled && usb_enabled) {
//        usb_uhci_piix3_init(pci_bus, piix3_devfn + 2);
//    }

    if (pci_enabled && acpi_enabled) {
        piix4_acpi_system_hot_add_init(pci_bus);
    }

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
