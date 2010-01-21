/*
 * QEMU NEC PC-9821 IDE Bus
 *
 * Copyright (c) 2003 Fabrice Bellard
 * Copyright (c) 2006 Openedhand Ltd.
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
#include <hw/hw.h>
#include <hw/pc.h>
#include <hw/isa.h>
#include "block.h"
#include "block_int.h"
#include "sysemu.h"
#include "dma.h"

#include <hw/ide/internal.h>

/***********************************************************/
/* NEC PC-9821 IDE definitions */

typedef struct PC98IDEState {
    ISADevice dev;
    IDEBus bus[2];
    IDEBus *cur_bus;
    uint32_t  isairq;
    qemu_irq  irq;
} PC98IDEState;

static void pc98_ide_cpu_shutdown(void *opaque, uint32_t addr, uint32_t val)
{
    qemu_cpu_reset_request();
}

static uint32_t pc98_ide_connection_read(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;
    uint32_t ret;

    ret = 0x01;
    if (s->cur_bus->ifs[0].bs && !s->cur_bus->ifs[0].is_cdrom) {
        ret |= 0x20;
    }
    if (s->cur_bus->ifs[1].bs && !s->cur_bus->ifs[1].is_cdrom) {
        ret |= 0x40;
    }
    return ret;
}

static void pc98_ide_bank_write(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;

    if (!(val & 0x80)) {
        s->cur_bus = &s->bus[val & 1];
    }
}

static uint32_t pc98_ide_bank_read(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;

    if (s->cur_bus == &s->bus[1]) {
        return 0x01;
    }
    return 0x00;
}

static void pc98_ide_ioport_write(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;

    ide_ioport_write(s->cur_bus, addr >> 1, val);
}

static uint32_t pc98_ide_ioport_read(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;

    return ide_ioport_read(s->cur_bus, addr >> 1);
}

static void pc98_ide_data_writew(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;

    ide_data_writew(s->cur_bus, addr >> 1, val);
}

static uint32_t pc98_ide_data_readw(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;

    return ide_data_readw(s->cur_bus, addr >> 1);
}

static void pc98_ide_data_writel(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;

    ide_data_writel(s->cur_bus, addr >> 1, val);
}

static uint32_t pc98_ide_data_readl(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;

    return ide_data_readl(s->cur_bus, addr >> 1);
}

static void pc98_ide_digital_write(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;
    uint8_t prev = s->cur_bus->cmd;

    ide_cmd_write(s->cur_bus, addr, val);
    if ((prev & IDE_CMD_RESET) != (val & IDE_CMD_RESET)) {
        s->cur_bus->ifs[0].status = READY_STAT | SEEK_STAT;
        s->cur_bus->ifs[1].status = READY_STAT | SEEK_STAT;
    }
}

static uint32_t pc98_ide_status_read(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;

    return ide_status_read(s->cur_bus, addr);
}

static uint32_t pc98_ide_digital_read(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;
    uint32_t ret;

    ret = 0xc0;
    ret |= (~s->cur_bus->ifs[0].select & 0x0f) << 2;
    if (s->cur_bus->unit == 1) {
        ret |= 1;
    } else {
        ret |= 2;
    }
    return ret;
}

static void pc98_ide_save(QEMUFile* f, void *opaque)
{
    PC98IDEState *s = opaque;
    int i;
    uint8_t bus1_selected;

    for (i = 0; i < 2; i++) {
        idebus_save(f, &s->bus[i]);
        ide_save(f, &s->bus[i].ifs[0]);
        ide_save(f, &s->bus[i].ifs[1]);
    }
    bus1_selected = (s->cur_bus != &s->bus[0]);
    qemu_put_8s(f, &bus1_selected);
}

static int pc98_ide_load(QEMUFile* f, void *opaque, int version_id)
{
    PC98IDEState *s = opaque;
    int i;
    uint8_t bus1_selected;

    for (i = 0; i < 2; i++) {
        idebus_load(f, &s->bus[i], version_id);
        ide_load(f, &s->bus[i].ifs[0], version_id);
        ide_load(f, &s->bus[i].ifs[1], version_id);
    }
    qemu_get_8s(f, &bus1_selected);
    s->cur_bus = &s->bus[bus1_selected != 0];

    return 0;
}

static void pc98_ide_reset(void *opaque)
{
    PC98IDEState *s = opaque;
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            ide_reset(&s->bus[i].ifs[j]);
            s->bus[i].ifs[j].status = BUSY_STAT | SEEK_STAT;
            s->bus[i].ifs[j].error = 0x01;
        }
    }
    s->cur_bus = &s->bus[0];
}

static int pc98_ide_initfn(ISADevice *dev)
{
    PC98IDEState *s = DO_UPCAST(PC98IDEState, dev, dev);
    int i;

    ide_bus_new(&s->bus[0], &s->dev.qdev);
    ide_bus_new(&s->bus[1], &s->dev.qdev);

    isa_init_irq(dev, &s->irq, s->isairq);
    ide_init2(&s->bus[0], NULL, NULL, s->irq);
    ide_init2(&s->bus[1], NULL, NULL, s->irq);

    register_ioport_write(0xf0, 1, 1, pc98_ide_cpu_shutdown, s);
    register_ioport_read(0xf0, 1, 1, pc98_ide_connection_read, s);

    register_ioport_write(0x430, 1, 1, pc98_ide_bank_write, s);
    register_ioport_read(0x430, 1, 1, pc98_ide_bank_read, s);
    register_ioport_write(0x432, 1, 1, pc98_ide_bank_write, s);
    register_ioport_read(0x432, 1, 1, pc98_ide_bank_read, s);
    for (i = 0; i < 8; i++) {
        register_ioport_write(0x640 + (i << 1), 1, 1, pc98_ide_ioport_write, s);
        register_ioport_read(0x640 + (i << 1), 1, 1, pc98_ide_ioport_read, s);
    }
    register_ioport_write(0x640, 2, 2, pc98_ide_data_writew, s);
    register_ioport_read(0x640, 2, 2, pc98_ide_data_readw, s);
    register_ioport_write(0x640, 4, 4, pc98_ide_data_writel, s);
    register_ioport_read(0x640, 4, 4, pc98_ide_data_readl, s);
    register_ioport_write(0x74c, 1, 1, pc98_ide_digital_write, s);
    register_ioport_read(0x74c, 1, 1, pc98_ide_status_read, s);
    register_ioport_read(0x74e, 1, 1, pc98_ide_digital_read, s);

    register_savevm("pc98-ide", 0, 1, pc98_ide_save, pc98_ide_load, s);
    qemu_register_reset(pc98_ide_reset, s);
    pc98_ide_reset(s);

    return 0;
};

int pc98_ide_init(int isairq, DriveInfo **hd_table)
{
    ISADevice *dev;
    PC98IDEState *s;
    static const int bus[4]  = { 0, 0, 1, 1 };
    static const int unit[4] = { 0, 1, 0, 1 };
    int i;

    dev = isa_create("pc98-ide");
    qdev_prop_set_uint32(&dev->qdev, "irq", isairq);
    if (qdev_init(&dev->qdev) != 0)
        return -1;

    s = DO_UPCAST(PC98IDEState, dev, dev);

    for (i = 0; i < 4; i++) {
        if (hd_table[i] == NULL)
            continue;
        ide_create_drive(s->bus + bus[i], unit[i], hd_table[i]);
        if (!s->bus[bus[i]].ifs[unit[i]].is_cdrom &&
            !s->bus[bus[i]].ifs[unit[i]].is_cf
           ) {
            s->bus[bus[i]].ifs[unit[i]].chs = 1;
        }
    }
    return 0;
}

static ISADeviceInfo pc98_ide_info = {
    .qdev.name  = "pc98-ide",
    .qdev.size  = sizeof(PC98IDEState),
    .init       = pc98_ide_initfn,
    .qdev.props = (Property[]) {
        DEFINE_PROP_UINT32("irq", PC98IDEState, isairq, 9),
        DEFINE_PROP_END_OF_LIST(),
    },
};

static void pc98_ide_register_devices(void)
{
    isa_qdev_register(&pc98_ide_info);
}

device_init(pc98_ide_register_devices)
