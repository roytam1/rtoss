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
#include "audio/audio.h"
#include "audiodev.h"
#include "net.h"
#include "boards.h"
#include "monitor.h"
#include "ide.h"
#include "loader.h"

#define IDE_FILE_NAME   "pc98ide.bin"
#define IDE_FILE_SIZE   0x2000
#define ITF_FILE_NAME   "pc98itf.bin"
#define ITF_FILE_SIZE   0x8000
#define BIOS_FILE_NAME  "pc98bios.bin"
#define BIOS_FILE_SIZE  0x18000
/* reserved */
#define SOUND_FILE_NAME "pc98sound.bin"
#define SOUND_FILE_SIZE 0x2000
#define SCSI_FILE_NAME  "pc98scsi.bin"
#define SCSI_FILE_SIZE  0x1000

#define BANK_FILE_NAME  "pc98bank%d.bin"
#define BANK_FILE_SIZE  0x8000

#define PCI_ROM_BANK  0
#define IDE_ROM_BANK  3
#define ITF_ROM_BANK  4
#define BIOS_ROM_BANK 5
#define ROM_BANK_NUM  8

#define REQUIRED_ROM_BANK ((1 << ITF_ROM_BANK) | (7 << BIOS_ROM_BANK))

#define BANK_BITS 12
/* (1 << BANK_BITS) */
#define BANK_SIZE 0x1000

#define PCI_BIOS_OFS   (BANK_FILE_SIZE * PCI_ROM_BANK)
#define IDE_BIOS_OFS   (BANK_FILE_SIZE * IDE_ROM_BANK)
#define ITF_OFS        (BANK_FILE_SIZE * ITF_ROM_BANK)
#define BIOS_OFS       (BANK_FILE_SIZE * BIOS_ROM_BANK)
#define SOUND_BIOS_OFS (BANK_FILE_SIZE * ROM_BANK_NUM)
#define SCSI_BIOS_OFS  (SOUND_BIOS_OFS + SOUND_FILE_SIZE)
#define NONE_OFS       (SCSI_BIOS_OFS + SCSI_FILE_SIZE)
#define TOTAL_ROM_SIZE (NONE_OFS + BANK_SIZE)

static ram_addr_t ram_addr[0x1000000 >> BANK_BITS]; /* 16MB */
static ram_addr_t rom_addr[TOTAL_ROM_SIZE >> BANK_BITS];
static ram_addr_t mem_bank[0x100000 >> BANK_BITS]; /* 1MB */

enum {
    D8000_BANK_IDE = 1,
    D8000_BANK_PCI = 2,
    D8000_BANK_PnP = 3,
};

static void *sys;

static int tvram_io_memory;
static int vram_a8000_io_memory;
static int vram_b0000_io_memory;
static int vram_e0000_io_memory;
static int vram_f00000_io_memory;

static uint8_t ram_window_map1 = 0x08;
static uint8_t ram_window_map2 = 0x0a;
static uint8_t d8000_bank = D8000_BANK_IDE;
static uint8_t ide_ram_selected = 1;
static uint8_t bios_ram_selected = 0;
static uint8_t itf_selected = 1;
static uint8_t use_system_16mb = 1;

static uint8_t ide_bios_loaded = 0;
static uint8_t hd_connect = 0;

#define MAX_IDE_BUS 2

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
            if (apic_accept_pic_intr(env)) {
                apic_deliver_pic_intr(env, level);
            }
            env = env->next_cpu;
        }
    } else {
        if (level) {
            cpu_interrupt(env, CPU_INTERRUPT_HARD);
        } else {
            cpu_reset_interrupt(env, CPU_INTERRUPT_HARD);
        }
    }
}

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

/* memory */
static uint8_t *get_ram_ptr(ram_addr_t addr)
{
    uint8_t *ram = qemu_get_ram_ptr(ram_addr[addr >> BANK_BITS]);
    return &ram[addr & (BANK_SIZE - 1)];
}

static void register_ram(target_phys_addr_t top, ram_addr_t size,
                         ram_addr_t addr)
{
    target_phys_addr_t a;

    for (a = top; a < top + size; a += BANK_SIZE) {
        ram_addr_t phys_offset = ram_addr[addr >> BANK_BITS];
        if (mem_bank[a >> BANK_BITS] != phys_offset) {
            cpu_register_physical_memory(a, BANK_SIZE, phys_offset);
            if (i440fx_state) {
                i440fx_update_isa_page_descs(i440fx_state, a, BANK_SIZE);
            }
            if (a >= 0xa0000) {
                if (use_system_16mb) {
                    cpu_register_physical_memory(0xf00000 + a, BANK_SIZE,
                                                 phys_offset);
                }
                cpu_register_physical_memory(0xfff00000 + a, BANK_SIZE,
                                             phys_offset);
            }
            mem_bank[a >> BANK_BITS] = phys_offset;
        }
        addr += BANK_SIZE;
    }
}

static void register_rom(target_phys_addr_t top, ram_addr_t size,
                         ram_addr_t addr)
{
    target_phys_addr_t a;

    for (a = top; a < top + size; a += BANK_SIZE) {
        ram_addr_t phys_offset = rom_addr[addr >> BANK_BITS];
        if (mem_bank[a >> BANK_BITS] != phys_offset) {
            cpu_register_physical_memory(a, BANK_SIZE,
                                         phys_offset | IO_MEM_ROM);
            if (i440fx_state) {
                i440fx_update_isa_page_descs(i440fx_state, a, BANK_SIZE);
            }
            if (a >= 0xa0000) {
                if (use_system_16mb) {
                    cpu_register_physical_memory(0xf00000 + a, BANK_SIZE,
                                                 phys_offset | IO_MEM_ROM);
                }
                cpu_register_physical_memory(0xfff00000 + a, BANK_SIZE,
                                             phys_offset | IO_MEM_ROM);
            }
            mem_bank[a >> BANK_BITS] = phys_offset;
        }
        if (addr != NONE_OFS) {
            addr += BANK_SIZE;
        }
    }
}

static void register_io_memory(target_phys_addr_t top, ram_addr_t size,
                               ram_addr_t phys_offset)
{
    target_phys_addr_t a;

    if (mem_bank[top >> BANK_BITS] != phys_offset) {
        cpu_register_physical_memory(isa_mem_base + top, size, phys_offset);
        if (i440fx_state) {
            i440fx_update_isa_page_descs(i440fx_state, top, size);
        }
        if (top >= 0xa0000) {
            if (use_system_16mb) {
                cpu_register_physical_memory(isa_mem_base + 0xf00000 + top,
                                             size, phys_offset);
            }
            cpu_register_physical_memory(isa_mem_base + 0xfff00000 + top,
                                         size, phys_offset);
        }
        for (a = top + BANK_SIZE; a < top + size; a += BANK_SIZE) {
            mem_bank[a >> BANK_BITS] = -1;
        }
        mem_bank[top >> BANK_BITS] = phys_offset;
    }
}

static void register_ide_rom(void)
{
    if (ide_ram_selected) {
        register_rom(0xd8000, 0x2000, IDE_BIOS_OFS);
        register_ram(0xda000, 0x2000, 0xda000);
        register_rom(0xdc000, 0x4000, NONE_OFS);
    } else {
        register_rom(0xd8000, 0x2000, IDE_BIOS_OFS);
        register_rom(0xda000, 0x6000, NONE_OFS);
    }
}

static void register_pci_rom(void)
{
    register_rom(0xd8000, 0x8000, PCI_BIOS_OFS);
}

static void register_pnp_rom(void)
{
    /* XXX: register plug&play bios at 0xd8000-0xdffff */
}

static void register_bios_rom(void)
{
    register_rom(0xe8000, 0x18000, BIOS_OFS);
}

static void register_bios_ram(void)
{
    register_ram(0xe8000, 0x18000, 0xe8000);
}

static void register_itf_rom(void)
{
    register_rom(0xe8000, 0x10000, NONE_OFS);
    register_rom(0xf8000, 0x8000, ITF_OFS);
}

/* I/O */
static void ioport_f2_write(void *opaque, uint32_t addr, uint32_t data)
{
    pc98_system_log("a20mask=1\n");
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
        pc98_system_log("a20mask=0\n");
        ioport_set_a20(1);
        break;
    case 0x03:
        pc98_system_log("a20mask=1\n");
        ioport_set_a20(0);
        break;
    }
}

static uint32_t ioport_f6_read(void *opaque, uint32_t addr)
{
    return (ioport_get_a20() ^ 1) | 0x5e;
}

static void ioport_43b_write(void *opaque, uint32_t addr, uint32_t data)
{
    target_phys_addr_t a;
    ram_addr_t phys_offset;

    if (data & 0x04) {
        if (use_system_16mb) {
            pc98_system_log("16mb ram\n");
            use_system_16mb = 0;
            for (a = 0xf00000; a < 0x1000000; a += BANK_SIZE) {
                cpu_register_physical_memory(a, BANK_SIZE,
                                             ram_addr[a >> BANK_BITS]);
            }
        }
    } else {
        if (!use_system_16mb) {
            pc98_system_log("16mb system\n");
            use_system_16mb = 1;
            cpu_register_physical_memory(isa_mem_base + 0xf00000, 0xa0000,
                                         vram_f00000_io_memory);
            for (a = 0xfa0000; a < 0x1000000; a += BANK_SIZE) {
                phys_offset = cpu_get_physical_page_desc(a & 0xfffff);
                cpu_register_physical_memory(a, BANK_SIZE, phys_offset);
            }
        }
    }
}

static uint32_t ioport_43b_read(void *opaque, uint32_t addr)
{
    if (use_system_16mb) {
        return 0x00;
    } else {
        return 0x04;
    }
}

static void ioport_43d_write(void *opaque, uint32_t addr, uint32_t data)
{
    switch (data) {
    case 0x00:
    case 0x10:
        if (!itf_selected) {
            pc98_system_log("itfd\n");
            itf_selected = 1;
            register_itf_rom();
        }
        break;
    case 0x02:
    case 0x12:
        if (itf_selected) {
            pc98_system_log("bios\n");
            itf_selected = 0;
            if (bios_ram_selected) {
                if (ide_bios_loaded && hd_connect) {
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
    if (ram_window_map1 != (data & 0xfe)) {
        pc98_system_log("memmap[8]=%2x\n", data);
        ram_window_map1 = data & 0xfe;
        if (ram_window_map1 == 0x0a) {
            register_io_memory(0x80000, 0x08000, tvram_io_memory);
            register_io_memory(0x88000, 0x08000, vram_a8000_io_memory);
            register_io_memory(0x90000, 0x10000, vram_b0000_io_memory);
        } else if (ram_window_map1 == 0x0e) {
            register_io_memory(0x80000, 0x08000, vram_e0000_io_memory);
            register_ram(0x88000, 0x18000, 0xe8000);
        } else {
            register_ram(0x80000, 0x20000, ram_window_map1 * 0x10000);
        }
    }
}

static uint32_t ioport_461_read(void *opaque, uint32_t addr)
{
    return ram_window_map1;
}

static void ioport_463_write(void *opaque, uint32_t addr, uint32_t data)
{
    if (ram_window_map2 != (data & 0xfe)) {
        pc98_system_log("memmap[a]=%2x\n", data);
        ram_window_map2 = data & 0xfe;
        if (ram_window_map2 == 0x0a) {
            register_io_memory(0xa0000, 0x08000, tvram_io_memory);
            register_io_memory(0xa8000, 0x08000, vram_a8000_io_memory);
            register_io_memory(0xb0000, 0x10000, vram_b0000_io_memory);
        } else if (ram_window_map2 == 0x0e) {
            register_io_memory(0xa0000, 0x08000, vram_e0000_io_memory);
            register_ram(0xa8000, 0x18000, 0xe8000);
        } else {
            register_ram(0xa0000, 0x20000, ram_window_map2 * 0x10000);
        }
    }
}

static uint32_t ioport_463_read(void *opaque, uint32_t addr)
{
    return ram_window_map2;
}

static uint32_t ioport_534_read(void *opaque, uint32_t addr)
{
    return 0xec;
}

static void ioport_53d_write(void *opaque, uint32_t addr, uint32_t data)
{
    if (data & 0x02) {
        if (!bios_ram_selected) {
            pc98_system_log("bios ram\n");
            bios_ram_selected = 1;
            if (!itf_selected) {
                register_bios_ram();
            }
        }
    } else {
        if (bios_ram_selected) {
            pc98_system_log("bios rom\n");
            bios_ram_selected = 0;
            if (!itf_selected) {
                register_bios_rom();
            }
        }
    }
}

static void ioport_63c_write(void *opaque, uint32_t addr, uint32_t data)
{
    switch (data & 0x03) {
    case 1:
        pc98_system_log("ide bios\n");
        if (d8000_bank != D8000_BANK_IDE) {
            register_ide_rom();
        }
        break;
    case 2:
        pc98_system_log("pci bios\n");
        if (d8000_bank != D8000_BANK_PCI) {
            register_pci_rom();
        }
        break;
    case 3:
        pc98_system_log("plug&play bios\n");
        if (d8000_bank != D8000_BANK_PnP) {
            register_pnp_rom();
        }
        break;
    }
    d8000_bank = data & 0x03;
}

static uint32_t ioport_63c_read(void *opaque, uint32_t addr)
{
    return d8000_bank;
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
//            pc98_system_log("ide ram on\n");
            ide_ram_selected = 0;
            if (d8000_bank == D8000_BANK_IDE) {
                register_ide_rom();
            }
        }
        break;
    case 0x81:
        if (!ide_ram_selected) {
//            pc98_system_log("ide ram off\n");
            ide_ram_selected = 1;
            if (d8000_bank == D8000_BANK_IDE) {
                register_ide_rom();
            }
        }
        break;
    }
}

static uint32_t ioport_1e8e_read(void *opaque, uint32_t addr)
{
    if (ide_ram_selected) {
        return 0x81;
    } else {
        return 0x80;
    }
}

static uint32_t ioport_9894_read(void *opaque, uint32_t addr)
{
    return 0x90;
}

/* interface */

static void pc98_save(QEMUFile* f, void *opaque)
{
    target_phys_addr_t a;

    for (a = 0; a < 0x100000; a += BANK_SIZE) {
        mem_bank[a >> BANK_BITS] = cpu_get_physical_page_desc(a);
    }
    qemu_put_8s(f, &ram_window_map1);
    qemu_put_8s(f, &ram_window_map2);
    qemu_put_8s(f, &d8000_bank);
    qemu_put_8s(f, &ide_ram_selected);
    qemu_put_8s(f, &bios_ram_selected);
    qemu_put_8s(f, &itf_selected);
    qemu_put_8s(f, &use_system_16mb);
    qemu_put_buffer(f, (const uint8_t *)mem_bank, sizeof(mem_bank));
}

static int pc98_load(QEMUFile* f, void *opaque, int version_id)
{
    target_phys_addr_t a;

    qemu_get_8s(f, &ram_window_map1);
    qemu_get_8s(f, &ram_window_map2);
    qemu_get_8s(f, &d8000_bank);
    qemu_get_8s(f, &ide_ram_selected);
    qemu_get_8s(f, &bios_ram_selected);
    qemu_get_8s(f, &itf_selected);
    qemu_get_8s(f, &use_system_16mb);
    qemu_get_buffer(f, (uint8_t *)mem_bank, sizeof(mem_bank));

    /* restore memory bank */
    for (a = 0x80000; a < 0x100000; a += BANK_SIZE) {
        ram_addr_t phys_offset = mem_bank[a >> BANK_BITS];
        cpu_register_physical_memory(a, BANK_SIZE, phys_offset);
        if (a >= 0xa0000) {
            if (use_system_16mb) {
                cpu_register_physical_memory(0xf00000 + a, BANK_SIZE,
                                             phys_offset);
            }
            cpu_register_physical_memory(0xfff00000 + a, BANK_SIZE,
                                         phys_offset);
        }
    }
    if (use_system_16mb) {
        cpu_register_physical_memory(isa_mem_base + 0xf00000, 0xa0000,
                                     vram_f00000_io_memory);
    } else {
        for (a = 0xf00000; a < 0x1000000; a += BANK_SIZE) {
            cpu_register_physical_memory(a, BANK_SIZE,
                                         ram_addr[a >> BANK_BITS]);
        }
    }

    return 0;
}

static void pc98_reset(void *opaque)
{
    if (ram_window_map1 != 0x08) {
        ram_window_map1 = 0x08;
        register_ram(0x80000, 0x20000, 0x80000);
    }
    if (ram_window_map2 != 0x0a) {
        ram_window_map2 = 0x0a;
        register_io_memory(0xa0000, 0x08000, tvram_io_memory);
        register_io_memory(0xa8000, 0x08000, vram_a8000_io_memory);
        register_io_memory(0xb0000, 0x10000, vram_b0000_io_memory);
    }
    if (!(d8000_bank == D8000_BANK_IDE && ide_ram_selected)) {
        d8000_bank = D8000_BANK_IDE;
        ide_ram_selected = 1;
        register_ide_rom();
    }
    if (!itf_selected) {
        itf_selected = 1;
        register_itf_rom();
    }
    bios_ram_selected = 0;
}

void pc98_cpu_shutdown(void)
{
    if (pc98_sys_read_shut(sys)) {
        pc98_system_log("system reset\n");
        qemu_system_reset_request();
    } else {
        pc98_system_log("cpu reset\n");
        qemu_cpu_reset_request();
    }
}

static int patch_itf(uint8_t *buf, const char *msg)
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
        if (i == len) {
            op[1] = (a1 - 1) & 0xff;
            op[2] = (a1 - 1) >> 8;
            for (a2 = 0; a2 < 0x8000 - 12; a2++) {
                op[4] = (a2 + 9) & 0xff;
                op[5] = (a2 + 9) >> 8;
                for (i = 0; i < 12; i++) {
                    if (!(op[i] == buf[a2 + i] || op[i] == 0x00)) {
                        break;
                    }
                }
                if (i == 9 || i == 12) {
                    memset(buf + a2, 0x90, i);
                    ret = 1;
                }
            }
        }
    }
    return ret;
}

static void update_check_sum(uint8_t *buf)
{
    uint8_t l = 0, h = 0;
    int i;

    for (i = 0; i < 0x8000; i += 2) {
        l += buf[i + 0];
        h += buf[i + 1];
    }
    buf[0x7ffe] -= l;
    buf[0x7fff] -= h;
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
    int i;
    ram_addr_t a;
    PCIBus *pci_bus;
    int piix3_devfn = -1;
    CPUState *env = NULL;
    qemu_irq *cpu_irq;
    qemu_irq *isa_irq;
    qemu_irq *i8259;
    IsaIrqState *isa_irq_state;
    PITState *pit;
    DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
    DriveInfo *fd[MAX_FD];

    i440fx_state = NULL;

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
    if (ram_size < 0x1000000) {
        ram_size = 0x1000000; /* >= 16MB */
    }
    ram_size &= ~0x7fffff; /* 8MB * num */

    for (i = 0; i < (0x100000 >> BANK_BITS); i++) {
        mem_bank[i] = -1;
    }
    for (a = 0; a < 0xa0000; a += BANK_SIZE) {
        ram_addr[a >> BANK_BITS] = qemu_ram_alloc(BANK_SIZE);
    }
    qemu_ram_alloc(0x100000 - 0xa0000); /* for PAM and SMRAM */
    for (a = 0xa0000; a < 0x1000000; a += BANK_SIZE) {
        ram_addr[a >> BANK_BITS] = qemu_ram_alloc(BANK_SIZE);
    }

    register_ram(0, 0xa0000, 0);
    for (a = 0x100000; a < 0x0f00000; a += BANK_SIZE) {
        cpu_register_physical_memory(a, BANK_SIZE, ram_addr[a >> BANK_BITS]);
    }
    if (ram_size > 0x1000000) {
        ram_addr_t ram_addr = qemu_ram_alloc(ram_size - 0x1000000);
        cpu_register_physical_memory(0x1000000, ram_size - 0x1000000, ram_addr);
    }

    /* BIOS load */
    for (a = 0; a < TOTAL_ROM_SIZE; a += BANK_SIZE) {
        rom_addr[a >> BANK_BITS] = qemu_ram_alloc(BANK_SIZE);
    }
    buf = qemu_malloc(TOTAL_ROM_SIZE);
    memset(buf, 0xff, TOTAL_ROM_SIZE);

    for (i = 0; i < ROM_BANK_NUM; i++) {
        sprintf(filename, BANK_FILE_NAME, i);
        filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, filename);
        if (filepath) {
            if (load_image(filepath, buf + BANK_FILE_SIZE * i) == BANK_FILE_SIZE) {
                loaded |= (1 << i);
            }
            qemu_free(filepath);
        }
    }
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, IDE_FILE_NAME);
    if (filepath) {
        if (load_image(filepath, buf + IDE_BIOS_OFS) == IDE_FILE_SIZE) {
            loaded |= (1 << IDE_ROM_BANK);
        }
        qemu_free(filepath);
    }
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, ITF_FILE_NAME);
    if (filepath) {
        if (load_image(filepath, buf + ITF_OFS) == ITF_FILE_SIZE) {
            loaded |= (1 << ITF_ROM_BANK);
        }
        qemu_free(filepath);
    }
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, BIOS_FILE_NAME);
    if (filepath) {
        if (load_image(filepath, buf + BIOS_OFS) == BIOS_FILE_SIZE) {
            loaded |= (7 << BIOS_ROM_BANK);
        }
        qemu_free(filepath);
    }
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, SOUND_FILE_NAME);
    if (filepath) {
        load_image(filepath, buf + SOUND_BIOS_OFS);
        qemu_free(filepath);
    }
    filepath = qemu_find_file(QEMU_FILE_TYPE_BIOS, SCSI_FILE_NAME);
    if (filepath) {
        load_image(filepath, buf + SCSI_BIOS_OFS);
        qemu_free(filepath);
    }
    if ((loaded & REQUIRED_ROM_BANK) == REQUIRED_ROM_BANK) {
        /* ITF: disable hardware check */
        static const char msg[][32] = {
            "TIMER INTERRUPT ERROR", "SIMM SETTING ERROR",
            "CACHE RAM ERROR", "CACHE ERROR",
            "2ND CACHE RAM ERROR", "2ND CACHE ERROR",
            "MEMORY SWITCH ERROR", "SET THE SOFTWARE DIP-SWITCH",
            "PROCESSOR UPDATE ERROR",
            /* end of array */
            "",
        };
        int patched = 0;
        for (i = 0; msg[i][0] != '\0'; i++) {
            patched |= patch_itf(buf + ITF_OFS, msg[i]);
        }
        if (patched) {
            update_check_sum(buf + ITF_OFS);
        }
        /* BIOS: disable PnP BIOS */
        for (a = 0x8000; a < 0x18000; a += 0x10) {
            uint8_t *p = buf + BIOS_OFS + a;
            if (p[0] == 0x24 && p[1] == 'P' && p[2] == 'n' && p[3] == 'P') {
                p[0] = 'n';
                p[2] = 0x24;
                break;
            }
        }
        /* copy to rom_addr */
        for (a = 0; a < TOTAL_ROM_SIZE; a += BANK_SIZE) {
            memcpy(qemu_get_ram_ptr(rom_addr[a >> BANK_BITS]), buf + a, BANK_SIZE);
        }
        ide_bios_loaded = ((loaded & (1 << IDE_ROM_BANK)) != 0);
    } else {
        fprintf(stderr, "qemu: could not load PC-9821 BIOS\n");
        exit(1);
    }
    qemu_free(buf);

    register_rom(0xc0000, 0x18000, NONE_OFS);
    register_ide_rom();
    register_itf_rom();

    /* init basic PC-9821 hardware */
    register_ioport_write(0xf2, 1, 1, ioport_f2_write, env);
    register_ioport_read(0xf2, 1, 1, ioport_f2_read, env);
    register_ioport_write(0xf6, 1, 1, ioport_f6_write, env);
    register_ioport_read(0xf6, 1, 1, ioport_f6_read, env);
    register_ioport_write(0x43b, 1, 1, ioport_43b_write, env);
    register_ioport_read(0x43b, 1, 1, ioport_43b_read, env);
    register_ioport_write(0x43d, 1, 1, ioport_43d_write, env);
    register_ioport_write(0x461, 1, 1, ioport_461_write, env);
    register_ioport_read(0x461, 1, 1, ioport_461_read, env);
    register_ioport_write(0x463, 1, 1, ioport_463_write, env);
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

    cpu_irq = qemu_allocate_irqs(pic_irq_request, NULL, 1);
    i8259 = pc98_i8259_init(cpu_irq[0]);
    isa_irq_state = qemu_mallocz(sizeof(*isa_irq_state));
    isa_irq_state->i8259 = i8259;
    isa_irq = qemu_allocate_irqs(isa_irq_handler, isa_irq_state, 24);

    if (pci_enabled) {
        pci_bus = pc98_i440fx_init(&i440fx_state, &piix3_devfn, isa_irq, ram_size);
    } else {
        pci_bus = NULL;
        isa_bus_new(NULL);
    }
    isa_bus_irqs(isa_irq);

    ferr_irq = isa_reserve_irq(8);

    if (pci_enabled) {
        isa_irq_state->ioapic = ioapic_init();
    }
    pit = pc98_pit_init(isa_reserve_irq(0));
    pc98_pcspk_init(pit);
#ifdef HAS_AUDIO
    pcspk_audio_init(isa_irq);
    pc98_sound_init(isa_irq);
#endif
    pc98_DMA_init(1);

    pc98_kbd_init();
    pc98_mouse_init();
    sys = pc98_sys_init();
    pc98_vga_init();

    tvram_io_memory = cpu_get_physical_page_desc(0xa0000);
    vram_a8000_io_memory = cpu_get_physical_page_desc(0xa8000);
    vram_b0000_io_memory = cpu_get_physical_page_desc(0xb0000);
    vram_e0000_io_memory = cpu_get_physical_page_desc(0xe0000);
    vram_f00000_io_memory = cpu_get_physical_page_desc(0xf00000);

    for(i = 0; i < nb_nics; i++) {
        NICInfo *nd = &nd_table[i];

        if (!pci_enabled || (nd->model && strcmp(nd->model, "ne2k_isa") == 0)) {
            pc98_init_ne2k_isa(nd); /* MELCO LGY-98 */
        } else {
            pci_nic_init(nd, "ne2k_pci", NULL);
        }
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
    pc98_ide_init(hd);

    for(i = 0; i < MAX_FD; i++) {
        fd[i] = drive_get(IF_FLOPPY, 0, i);
    }
    pc98_fdctrl_init(fd);

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
