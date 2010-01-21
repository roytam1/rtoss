/*
 * QEMU NEC PC-9821 memory
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
#include "isa.h"
#include "pc.h"
#include "sysemu.h"

#define PCI_FILE_NAME   "pc98pci.bin"
#define PCI_FILE_SIZE   0x8000
#define IDE_FILE_NAME   "pc98ide.bin"
#define IDE_FILE_SIZE   0x2000
#define ITF_FILE_NAME   "pc98itf.bin"
#define ITF_FILE_SIZE   0x8000
#define BIOS_FILE_NAME  "pc98bios.bin"
#define BIOS_FILE_SIZE  0x18000
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
#define NONE_OFS       (BANK_FILE_SIZE * ROM_BANK_NUM)
#define TOTAL_ROM_SIZE (NONE_OFS + BANK_SIZE)

enum {
    D8000_BANK_IDE = 1,
    D8000_BANK_PCI = 2,
    D8000_BANK_PnP = 3,
};

struct MemoryState {
    ram_addr_t ram_size;
    ram_addr_t ram_addr;
    ram_addr_t rom_addr;
    ram_addr_t mem_bank[0x100000 >> BANK_BITS]; /* 1MB */

    int tvram_io_memory;
    int vram_a8000_io_memory;
    int vram_b0000_io_memory;
    int vram_e0000_io_memory;
    int vram_f00000_io_memory;

    uint8_t ram_window_map1;
    uint8_t ram_window_map2;
    uint8_t d8000_bank;
    uint8_t ide_bios_enabled;
    uint8_t ide_ram_selected;
    uint8_t bios_ram_selected;
    uint8_t itf_selected;
    uint8_t use_system_16mb;

    uint8_t ide_bios_loaded;
    uint8_t hd_connect;
    uint8_t init_done;
};

typedef struct MemoryState MemoryState;

static void register_ram(void *opaque, target_phys_addr_t top, ram_addr_t size,
                         ram_addr_t ofs)
{
    MemoryState *s = opaque;
    target_phys_addr_t addr;
    int smram_update = 0;

    for (addr = top; addr < top + size; addr += BANK_SIZE) {
        ram_addr_t phys_offset = s->ram_addr + ofs;
        if (s->mem_bank[addr >> BANK_BITS] != phys_offset) {
            cpu_register_physical_memory(addr, BANK_SIZE, phys_offset);
            if (addr >= 0xa0000) {
                smram_update = 1;
                if (s->use_system_16mb) {
                    cpu_register_physical_memory(0xf00000 + addr, BANK_SIZE,
                                                 phys_offset);
                }
                cpu_register_physical_memory(0xfff00000 + addr, BANK_SIZE,
                                             phys_offset);
            }
            s->mem_bank[addr >> BANK_BITS] = phys_offset;
        }
        ofs += BANK_SIZE;
    }
    if (i440fx_state && smram_update && s->init_done) {
        i440fx_init_memory_mappings(i440fx_state);
    }
}

static void register_rom(void *opaque, target_phys_addr_t top, ram_addr_t size,
                         ram_addr_t ofs)
{
    MemoryState *s = opaque;
    target_phys_addr_t addr;
    int smram_update = 0;

    for (addr = top; addr < top + size; addr += BANK_SIZE) {
        ram_addr_t phys_offset = s->rom_addr + ofs;
        if (s->mem_bank[addr >> BANK_BITS] != phys_offset) {
            cpu_register_physical_memory(addr, BANK_SIZE,
                                         phys_offset | IO_MEM_ROM);
            if (addr >= 0xa0000) {
                smram_update = 1;
                if (s->use_system_16mb) {
                    cpu_register_physical_memory(0xf00000 + addr, BANK_SIZE,
                                                 phys_offset | IO_MEM_ROM);
                }
                cpu_register_physical_memory(0xfff00000 + addr, BANK_SIZE,
                                             phys_offset | IO_MEM_ROM);
            }
            s->mem_bank[addr >> BANK_BITS] = phys_offset;
        }
        if (ofs != NONE_OFS) {
            ofs += BANK_SIZE;
        }
    }
    if (i440fx_state && smram_update && s->init_done) {
        i440fx_init_memory_mappings(i440fx_state);
    }
}

static void register_io_memory(void *opaque, target_phys_addr_t top, ram_addr_t size,
                               ram_addr_t phys_offset)
{
    MemoryState *s = opaque;
    target_phys_addr_t addr;
    int smram_update = 0;

    if (s->mem_bank[top >> BANK_BITS] != phys_offset) {
        cpu_register_physical_memory(isa_mem_base + top, size, phys_offset);
        if (top >= 0xa0000) {
            smram_update = 1;
            if (s->use_system_16mb) {
                cpu_register_physical_memory(isa_mem_base + 0xf00000 + top,
                                             size, phys_offset);
            }
            cpu_register_physical_memory(isa_mem_base + 0xfff00000 + top,
                                         size, phys_offset);
        }
        for (addr = top + BANK_SIZE; addr < top + size; addr += BANK_SIZE) {
            s->mem_bank[addr >> BANK_BITS] = -1;
        }
        s->mem_bank[top >> BANK_BITS] = phys_offset;
    }
    if (i440fx_state && smram_update && s->init_done) {
        i440fx_init_memory_mappings(i440fx_state);
    }
}

static void register_ide_rom(void *opaque)
{
    MemoryState *s = opaque;

    if (!s->ide_bios_enabled) {
        register_rom(s, 0xd8000, 0x8000, NONE_OFS);
    } else if (s->ide_ram_selected) {
        register_rom(s, 0xd8000, 0x2000, IDE_BIOS_OFS);
        register_ram(s, 0xda000, 0x2000, 0xda000);
        register_rom(s, 0xdc000, 0x4000, NONE_OFS);
    } else {
        register_rom(s, 0xd8000, 0x2000, IDE_BIOS_OFS);
        register_rom(s, 0xda000, 0x6000, NONE_OFS);
    }
}

static void register_pci_rom(void *opaque)
{
    register_rom(opaque, 0xd8000, 0x8000, PCI_BIOS_OFS);
}

static void register_pnp_rom(void *opaque)
{
    /* XXX: register plug&play bios at 0xd8000-0xdffff */
}

static void register_bios_rom(void *opaque)
{
    register_rom(opaque, 0xe8000, 0x18000, BIOS_OFS);
}

static void register_bios_ram(void *opaque)
{
    register_ram(opaque, 0xe8000, 0x18000, 0xe8000);
}

static void register_itf_rom(void *opaque)
{
    register_rom(opaque, 0xe8000, 0x10000, NONE_OFS);
    register_rom(opaque, 0xf8000, 0x08000, ITF_OFS);
}

static void ioport_43b_write(void *opaque, uint32_t addr1, uint32_t data)
{
    MemoryState *s = opaque;
    target_phys_addr_t addr;

    if (data & 0x04) {
        if (s->use_system_16mb) {
            s->use_system_16mb = 0;
            qemu_unregister_coalesced_mmio(isa_mem_base + 0xf00000, 0xc0000);
            qemu_unregister_coalesced_mmio(isa_mem_base + 0xfe0000, 0x08000);
            cpu_register_physical_memory(0xf00000, 0x1000000 - 0xf00000,
                                         s->ram_addr + 0xf00000);
        }
    } else {
        if (!s->use_system_16mb) {
            s->use_system_16mb = 1;
            cpu_register_physical_memory(isa_mem_base + 0xf00000, 0xa0000,
                                         s->vram_f00000_io_memory);
            cpu_register_physical_memory(isa_mem_base + 0xfa0000, 0x08000,
                                         s->tvram_io_memory);
            cpu_register_physical_memory(isa_mem_base + 0xfa8000, 0x08000,
                                         s->vram_a8000_io_memory);
            cpu_register_physical_memory(isa_mem_base + 0xfb0000, 0x10000,
                                         s->vram_b0000_io_memory);
            cpu_register_physical_memory(isa_mem_base + 0xfe0000, 0x08000,
                                         s->vram_e0000_io_memory);
            for (addr = 0xfc0000; addr < 0xfe0000; addr += BANK_SIZE) {
                ram_addr_t phys_offset = cpu_get_physical_page_desc(addr & 0xfffff);
                cpu_register_physical_memory(addr, BANK_SIZE, phys_offset);
            }
            for (addr = 0xfe8000; addr < 0x1000000; addr += BANK_SIZE) {
                ram_addr_t phys_offset = cpu_get_physical_page_desc(addr & 0xfffff);
                cpu_register_physical_memory(addr, BANK_SIZE, phys_offset);
            }
            qemu_register_coalesced_mmio(isa_mem_base + 0xf00000, 0xc0000);
            qemu_register_coalesced_mmio(isa_mem_base + 0xfe0000, 0x08000);
        }
    }
}

static uint32_t ioport_43b_read(void *opaque, uint32_t addr)
{
    MemoryState *s = opaque;

    if (s->use_system_16mb) {
        return 0x00;
    } else {
        return 0x04;
    }
}

static void ioport_43d_write(void *opaque, uint32_t addr, uint32_t data)
{
    MemoryState *s = opaque;

    switch (data) {
    case 0x00:
    case 0x10:
    case 0x18:
        if (!s->itf_selected) {
            s->itf_selected = 1;
            register_itf_rom(s);
        }
        break;
    case 0x02:
    case 0x12:
        if (s->itf_selected) {
            s->itf_selected = 0;
            if (s->bios_ram_selected) {
                uint8_t *buf = phys_ram_base + s->ram_addr;
                /* protect memory size */
#ifdef PC98_DONT_USE_16MB_MEM
                buf[0x401] = 0x78;
#else
                buf[0x401] = 0x70;
#endif
                *(uint16_t *)(buf + 0x594) = ((s->ram_size - 0x1000000) >> 20);
                /* IDE BIOS patch */
                if (s->ide_bios_loaded && s->hd_connect) {
                    if (s->hd_connect & 1) {
                        buf[0x457] = 0x90;
                        buf[0x45d] |= 0x08;
                        buf[0x55d] |= 0x01;
                    } else {
                        buf[0x457] = 0x38;
                    }
                    if (s->hd_connect & 2) {
                        buf[0x457] |= 0x42;
                        buf[0x45d] |= 0x10;
                        buf[0x55d] |= 0x02;
                    } else {
                        buf[0x457] |= 0x07;
                    }
                    buf[0xf8e90] |= (s->hd_connect & 0x0f);
                }
                register_bios_ram(s);
            } else {
                register_bios_rom(s);
            }
        }
        break;
    }
}

static void ioport_461_write(void *opaque, uint32_t addr, uint32_t data)
{
    MemoryState *s = opaque;

    if (s->ram_window_map1 != (data & 0xfe)) {
        if (s->ram_window_map1 == 0x0a) {
            qemu_unregister_coalesced_mmio(isa_mem_base + 0x80000, 0x20000);
        } else if (s->ram_window_map1 == 0x0e) {
            qemu_unregister_coalesced_mmio(isa_mem_base + 0x80000, 0x08000);
        }
        s->ram_window_map1 = data & 0xfe;
        if (s->ram_window_map1 == 0x0a) {
            register_io_memory(s, 0x80000, 0x08000, s->tvram_io_memory);
            register_io_memory(s, 0x88000, 0x08000, s->vram_a8000_io_memory);
            register_io_memory(s, 0x90000, 0x10000, s->vram_b0000_io_memory);
            qemu_register_coalesced_mmio(isa_mem_base + 0x80000, 0x20000);
        } else if (s->ram_window_map1 == 0x0e) {
            register_io_memory(s, 0x80000, 0x08000, s->vram_e0000_io_memory);
            qemu_register_coalesced_mmio(isa_mem_base + 0x80000, 0x08000);
            register_ram(s, 0x88000, 0x18000, 0xe8000);
        } else {
            register_ram(s, 0x80000, 0x20000, s->ram_window_map1 * 0x10000);
        }
    }
}

static uint32_t ioport_461_read(void *opaque, uint32_t addr)
{
    MemoryState *s = opaque;

    return s->ram_window_map1;
}

static void ioport_463_write(void *opaque, uint32_t addr, uint32_t data)
{
    MemoryState *s = opaque;

    if (s->ram_window_map2 != (data & 0xfe)) {
        if (s->ram_window_map2 == 0x0a) {
            qemu_unregister_coalesced_mmio(isa_mem_base + 0xa0000, 0x20000);
        } else if (s->ram_window_map2 == 0x0e) {
            qemu_unregister_coalesced_mmio(isa_mem_base + 0xa0000, 0x08000);
        }
        s->ram_window_map2 = data & 0xfe;
        if (s->ram_window_map2 == 0x0a) {
            register_io_memory(s, 0xa0000, 0x08000, s->tvram_io_memory);
            register_io_memory(s, 0xa8000, 0x08000, s->vram_a8000_io_memory);
            register_io_memory(s, 0xb0000, 0x10000, s->vram_b0000_io_memory);
            qemu_register_coalesced_mmio(isa_mem_base + 0xa0000, 0x20000);
        } else if (s->ram_window_map2 == 0x0e) {
            register_io_memory(s, 0xa0000, 0x08000, s->vram_e0000_io_memory);
            qemu_register_coalesced_mmio(isa_mem_base + 0xa0000, 0x08000);
            register_ram(s, 0xa8000, 0x18000, 0xe8000);
        } else {
            register_ram(s, 0xa0000, 0x20000, s->ram_window_map2 * 0x10000);
        }
    }
}

static uint32_t ioport_463_read(void *opaque, uint32_t addr)
{
    MemoryState *s = opaque;

    return s->ram_window_map2;
}

static void ioport_53d_write(void *opaque, uint32_t addr, uint32_t data)
{
    MemoryState *s = opaque;

    if (data & 0x10) {
        if (!s->ide_bios_enabled) {
            s->ide_bios_enabled = 1;
            register_ide_rom(s);
        }
    } else {
        if (s->ide_bios_enabled) {
            s->ide_bios_enabled = 0;
            register_ide_rom(s);
        }
    }
    if (data & 0x02) {
        if (!s->bios_ram_selected) {
            s->bios_ram_selected = 1;
            if (!s->itf_selected) {
                register_bios_ram(s);
            }
        }
    } else {
        if (s->bios_ram_selected) {
            s->bios_ram_selected = 0;
            if (!s->itf_selected) {
                register_bios_rom(s);
            }
        }
    }
}

static void ioport_63c_write(void *opaque, uint32_t addr, uint32_t data)
{
    MemoryState *s = opaque;

    switch (data & 0x03) {
    case 1:
        if (s->d8000_bank != D8000_BANK_IDE) {
            register_ide_rom(s);
        }
        break;
    case 2:
        if (s->d8000_bank != D8000_BANK_PCI) {
            register_pci_rom(s);
        }
        break;
    case 3:
        if (s->d8000_bank != D8000_BANK_PnP) {
            register_pnp_rom(s);
        }
        break;
    }
    s->d8000_bank = data & 0x03;
}

static uint32_t ioport_63c_read(void *opaque, uint32_t addr)
{
    MemoryState *s = opaque;

    return s->d8000_bank;
}

static uint32_t ioport_63d_read(void *opaque, uint32_t addr)
{
    return 0x04;
}

static void ioport_1e8e_write(void *opaque, uint32_t addr, uint32_t data)
{
    MemoryState *s = opaque;

    switch (data) {
    case 0x80:
        if (s->ide_ram_selected) {
            s->ide_ram_selected = 0;
            if (s->d8000_bank == D8000_BANK_IDE) {
                register_ide_rom(s);
            }
        }
        break;
    case 0x81:
        if (!s->ide_ram_selected) {
            s->ide_ram_selected = 1;
            if (s->d8000_bank == D8000_BANK_IDE) {
                register_ide_rom(s);
            }
        }
        break;
    }
}

static uint32_t ioport_1e8e_read(void *opaque, uint32_t addr)
{
    MemoryState *s = opaque;

    if (s->ide_ram_selected) {
        return 0x81;
    } else {
        return 0x80;
    }
}

static void pc98_mem_save(QEMUFile *f, void *opaque)
{
    MemoryState *s = opaque;

    qemu_put_8s(f, &s->ram_window_map1);
    qemu_put_8s(f, &s->ram_window_map2);
    qemu_put_8s(f, &s->d8000_bank);
    qemu_put_8s(f, &s->ide_bios_enabled);
    qemu_put_8s(f, &s->ide_ram_selected);
    qemu_put_8s(f, &s->bios_ram_selected);
    qemu_put_8s(f, &s->itf_selected);
    qemu_put_8s(f, &s->use_system_16mb);
}

static int pc98_mem_load(QEMUFile *f, void *opaque, int version_id)
{
    MemoryState *s = opaque;
    target_phys_addr_t addr;

    if (version_id != 1) {
        return -EINVAL;
    }

    if (s->ram_window_map1 == 0x0a) {
        qemu_unregister_coalesced_mmio(isa_mem_base + 0x80000, 0x20000);
    } else if (s->ram_window_map1 == 0x0e) {
        qemu_unregister_coalesced_mmio(isa_mem_base + 0x80000, 0x08000);
    }
    if (s->ram_window_map2 == 0x0a) {
        qemu_unregister_coalesced_mmio(isa_mem_base + 0xa0000, 0x20000);
    } else if (s->ram_window_map2 == 0x0e) {
        qemu_unregister_coalesced_mmio(isa_mem_base + 0xa0000, 0x08000);
    }
    if (s->use_system_16mb) {
        qemu_unregister_coalesced_mmio(isa_mem_base + 0xf00000, 0xc0000);
        qemu_unregister_coalesced_mmio(isa_mem_base + 0xfe0000, 0x08000);
    }

    qemu_get_8s(f, &s->ram_window_map1);
    qemu_get_8s(f, &s->ram_window_map2);
    qemu_get_8s(f, &s->d8000_bank);
    qemu_get_8s(f, &s->ide_bios_enabled);
    qemu_get_8s(f, &s->ide_ram_selected);
    qemu_get_8s(f, &s->bios_ram_selected);
    qemu_get_8s(f, &s->itf_selected);
    qemu_get_8s(f, &s->use_system_16mb);

    /* restore memory bank */
    if (s->ram_window_map1 == 0x0a) {
        register_io_memory(s, 0x80000, 0x08000, s->tvram_io_memory);
        register_io_memory(s, 0x88000, 0x08000, s->vram_a8000_io_memory);
        register_io_memory(s, 0x90000, 0x10000, s->vram_b0000_io_memory);
        qemu_register_coalesced_mmio(isa_mem_base + 0x80000, 0x20000);
    } else if (s->ram_window_map1 == 0x0e) {
        register_io_memory(s, 0x80000, 0x08000, s->vram_e0000_io_memory);
        qemu_register_coalesced_mmio(isa_mem_base + 0x80000, 0x08000);
        register_ram(s, 0x88000, 0x18000, 0xe8000);
    } else {
        register_ram(s, 0x80000, 0x20000, s->ram_window_map1 * 0x10000);
    }
    if (s->ram_window_map2 == 0x0a) {
        register_io_memory(s, 0xa0000, 0x08000, s->tvram_io_memory);
        register_io_memory(s, 0xa8000, 0x08000, s->vram_a8000_io_memory);
        register_io_memory(s, 0xb0000, 0x10000, s->vram_b0000_io_memory);
        qemu_register_coalesced_mmio(isa_mem_base + 0xa0000, 0x20000);
    } else if (s->ram_window_map2 == 0x0e) {
        register_io_memory(s, 0xa0000, 0x08000, s->vram_e0000_io_memory);
        qemu_register_coalesced_mmio(isa_mem_base + 0xa0000, 0x08000);
        register_ram(s, 0xa8000, 0x18000, 0xe8000);
    } else {
        register_ram(s, 0xa0000, 0x20000, s->ram_window_map2 * 0x10000);
    }
    if (s->d8000_bank == D8000_BANK_IDE) {
        register_ide_rom(s);
    } else if (s->d8000_bank == D8000_BANK_PCI) {
        register_pci_rom(s);
    } else if (s->d8000_bank == D8000_BANK_PnP) {
        register_pnp_rom(s);
    }
    if (s->itf_selected) {
        register_itf_rom(s);
    } else if (s->bios_ram_selected) {
        register_bios_ram(s);
    } else {
        register_bios_rom(s);
    }
    if (s->use_system_16mb) {
        cpu_register_physical_memory(isa_mem_base + 0xf00000, 0xa0000,
                                     s->vram_f00000_io_memory);
        cpu_register_physical_memory(isa_mem_base + 0xfa0000, 0x08000,
                                     s->tvram_io_memory);
        cpu_register_physical_memory(isa_mem_base + 0xfa8000, 0x08000,
                                     s->vram_a8000_io_memory);
        cpu_register_physical_memory(isa_mem_base + 0xfb0000, 0x10000,
                                     s->vram_b0000_io_memory);
        cpu_register_physical_memory(isa_mem_base + 0xfe0000, 0x08000,
                                     s->vram_e0000_io_memory);
        for (addr = 0xfc0000; addr < 0xfe0000; addr += BANK_SIZE) {
            ram_addr_t phys_offset = cpu_get_physical_page_desc(addr & 0xfffff);
            cpu_register_physical_memory(addr, BANK_SIZE, phys_offset);
        }
        for (addr = 0xfe8000; addr < 0x1000000; addr += BANK_SIZE) {
            ram_addr_t phys_offset = cpu_get_physical_page_desc(addr & 0xfffff);
            cpu_register_physical_memory(addr, BANK_SIZE, phys_offset);
        }
        qemu_register_coalesced_mmio(isa_mem_base + 0xf00000, 0xc0000);
        qemu_register_coalesced_mmio(isa_mem_base + 0xfe0000, 0x08000);
    } else {
        cpu_register_physical_memory(0xf00000, 0x1000000 - 0xf00000,
                                     s->ram_addr + 0xf00000);
    }
    return 0;
}

static void pc98_mem_reset(void *opaque)
{
    MemoryState *s = opaque;

    if (s->ram_window_map1 != 0x08) {
        if (s->ram_window_map1 == 0x0a) {
            qemu_unregister_coalesced_mmio(isa_mem_base + 0x80000, 0x20000);
        } else if (s->ram_window_map1 == 0x0e) {
            qemu_unregister_coalesced_mmio(isa_mem_base + 0x80000, 0x08000);
        }
        s->ram_window_map1 = 0x08;
        register_ram(s, 0x80000, 0x20000, 0x80000);
    }
    if (s->ram_window_map2 != 0x0a) {
        if (s->ram_window_map2 == 0x0e) {
            qemu_unregister_coalesced_mmio(isa_mem_base + 0xa0000, 0x08000);
        }
        s->ram_window_map2 = 0x0a;
        register_io_memory(s, 0xa0000, 0x08000, s->tvram_io_memory);
        register_io_memory(s, 0xa8000, 0x08000, s->vram_a8000_io_memory);
        register_io_memory(s, 0xb0000, 0x10000, s->vram_b0000_io_memory);
        qemu_register_coalesced_mmio(isa_mem_base + 0xa0000, 0x20000);
    }
    if (!(s->d8000_bank == D8000_BANK_IDE && s->ide_bios_enabled && s->ide_ram_selected)) {
        s->d8000_bank = D8000_BANK_IDE;
        s->ide_bios_enabled = 1;
        s->ide_ram_selected = 1;
        register_ide_rom(s);
    }
    if (!s->itf_selected) {
        s->itf_selected = 1;
        register_itf_rom(s);
    }
    s->bios_ram_selected = 0;
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

void pc98_mem_init(ram_addr_t ram_size, uint8_t hd_connect)
{
    MemoryState *s;
    uint8_t *buf;
    char filename[1024];
    uint8_t loaded = 0;
    int i;
    ram_addr_t addr;

    s = qemu_mallocz(sizeof(MemoryState));

    /* memory must be initialized after vga is initialized */
    s->tvram_io_memory = cpu_get_physical_page_desc(0xa0000);
    s->vram_a8000_io_memory = cpu_get_physical_page_desc(0xa8000);
    s->vram_b0000_io_memory = cpu_get_physical_page_desc(0xb0000);
    s->vram_e0000_io_memory = cpu_get_physical_page_desc(0xe0000);
    s->vram_f00000_io_memory = cpu_get_physical_page_desc(0xf00000);

    s->ram_window_map1 = 0x08;
    s->ram_window_map2 = 0x0a;
    s->d8000_bank = D8000_BANK_IDE;
    s->ide_bios_enabled = 1;
    s->ide_ram_selected = 1;
    s->bios_ram_selected = 0;
    s->itf_selected = 1;
#ifdef PC98_DONT_USE_16MB_MEM
    s->use_system_16mb = 0;
#else
    s->use_system_16mb = 1;
#endif

    /* allocate RAM & ROM */
    s->ram_addr = qemu_ram_alloc(ram_size);
    s->rom_addr = qemu_ram_alloc(TOTAL_ROM_SIZE);

    /* BIOS load */
    buf = qemu_malloc(TOTAL_ROM_SIZE);
    memset(buf, 0xff, TOTAL_ROM_SIZE);

    for (i = 0; i < ROM_BANK_NUM; i++) {
        snprintf(filename, sizeof(filename), "%s/pc98bank%d.bin", bios_dir, i);
        if (load_image(filename, buf + BANK_FILE_SIZE * i) == BANK_FILE_SIZE) {
            loaded |= (1 << i);
        }
    }
    snprintf(filename, sizeof(filename), "%s/%s", bios_dir, PCI_FILE_NAME);
    if (load_image(filename, buf + PCI_BIOS_OFS) == PCI_FILE_SIZE) {
        loaded |= (1 << PCI_ROM_BANK);
    }
    snprintf(filename, sizeof(filename), "%s/%s", bios_dir, IDE_FILE_NAME);
    if (load_image(filename, buf + IDE_BIOS_OFS) == IDE_FILE_SIZE) {
        loaded |= (1 << IDE_ROM_BANK);
    }
    snprintf(filename, sizeof(filename), "%s/%s", bios_dir, ITF_FILE_NAME);
    if (load_image(filename, buf + ITF_OFS) == ITF_FILE_SIZE) {
        loaded |= (1 << ITF_ROM_BANK);
    }
    snprintf(filename, sizeof(filename), "%s/%s", bios_dir, BIOS_FILE_NAME);
    if (load_image(filename, buf + BIOS_OFS) == BIOS_FILE_SIZE) {
        loaded |= (7 << PCI_ROM_BANK);
    }
    if ((loaded & REQUIRED_ROM_BANK) == REQUIRED_ROM_BANK) {
        /* ITF: disable hardware check */
        static const char msg[][32] = {
            "TIMER INTERRUPT ERROR",
            "EXTENDED GVRAM ERROR",
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
        for (addr = 0x8000; addr < 0x18000; addr += 0x10) {
            uint8_t *p = buf + BIOS_OFS + addr;
            if (p[0] == 0x24 && p[1] == 'P' && p[2] == 'n' && p[3] == 'P') {
                p[0] = 'n';
                p[2] = 0x24;
                break;
            }
        }
        /* copy to rom_addr */
        memcpy(phys_ram_base + s->rom_addr, buf, TOTAL_ROM_SIZE);
        s->ide_bios_loaded = ((loaded & (1 << IDE_ROM_BANK)) != 0);
    } else {
        fprintf(stderr, "qemu: could not load PC-9821 BIOS\n");
        exit(1);
    }
    qemu_free(buf);

    /* memory map */
    for (i = 0; i < (0x100000 >> BANK_BITS); i++) {
        s->mem_bank[i] = -1;
    }
    register_ram(s, 0, 0xa0000, 0);
#ifdef PC98_DONT_USE_16MB_MEM
    cpu_register_physical_memory(0x100000, ram_size - 0x100000,
                                 s->ram_addr + 0x100000);
#else
    cpu_register_physical_memory(0x100000, 0xf00000 - 0x100000,
                                 s->ram_addr + 0x100000);
    if (ram_size > 0x1000000) {
        cpu_register_physical_memory(0x1000000, ram_size - 0x1000000,
                                     s->ram_addr + 0x1000000);
    }
#endif
    register_rom(s, 0xc0000, 0x18000, NONE_OFS);
    register_ide_rom(s);
    register_itf_rom(s);

    register_ioport_write(0x43b, 1, 1, ioport_43b_write, s);
    register_ioport_read(0x43b, 1, 1, ioport_43b_read, s);
    register_ioport_write(0x43d, 1, 1, ioport_43d_write, s);
    register_ioport_write(0x461, 1, 1, ioport_461_write, s);
    register_ioport_read(0x461, 1, 1, ioport_461_read, s);
    register_ioport_write(0x463, 1, 1, ioport_463_write, s);
    register_ioport_read(0x463, 1, 1, ioport_463_read, s);
    register_ioport_write(0x53d, 1, 1, ioport_53d_write, s);
    register_ioport_write(0x63c, 1, 1, ioport_63c_write, s);
    register_ioport_read(0x63c, 1, 1, ioport_63c_read, s);
    register_ioport_read(0x63d, 1, 1, ioport_63d_read, s);
    register_ioport_write(0x1e8e, 1, 1, ioport_1e8e_write, s);
    register_ioport_read(0x1e8e, 1, 1, ioport_1e8e_read, s);

    register_savevm("pc98mem", 0, 1, pc98_mem_save, pc98_mem_load, s);
    qemu_register_reset(pc98_mem_reset, s);

    /* initialize done */
    s->ram_size = ram_size;
    s->hd_connect = hd_connect;
    s->init_done = 1;
}
