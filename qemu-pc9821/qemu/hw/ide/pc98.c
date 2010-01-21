/*
 * QEMU IDE Emulation: NEC PC-98x1 Bus support.
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
/* NEC PC-98x1 IDE definitions */

typedef struct PC98IDEState {
    ISADevice dev;
    IDEBus *bus[2];
    IDEBus *cur_bus;
    uint8_t connection;
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
    if (!(s->connection & 1)) {
        ret |= 0x20;
    }
    s->connection >>= 1;
    return ret;
}

static void pc98_ide_connection_init(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;

    s->connection = 0;
    if (s->cur_bus->ifs[0].bs) {
        s->connection |= 1;
    }
    if (s->cur_bus->ifs[1].bs) {
        s->connection |= 2;
    }
}

static void pc98_ide_bank_write(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;

    if (!(val & 0x80)) {
        s->cur_bus = s->bus[val & 1];
    }
}

static uint32_t pc98_ide_bank_read(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;
    uint32_t ret;

    if (s->cur_bus == s->bus[0]) {
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
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

static void pc98_ide_cmd_write(void *opaque, uint32_t addr, uint32_t val)
{
    PC98IDEState *s = opaque;

    ide_cmd_write(s->cur_bus, addr, val);
}

static uint32_t pc98_ide_status_read(void *opaque, uint32_t addr)
{
    PC98IDEState *s = opaque;

    return ide_status_read(s->cur_bus, addr);
}

static uint32_t pc98_ide_sdh_read(void *opaque, uint32_t addr)
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
        idebus_save(f, s->bus[i]);
        ide_save(f, &s->bus[i]->ifs[0]);
        ide_save(f, &s->bus[i]->ifs[1]);
    }
    bus1_selected = (s->cur_bus != s->bus[0]);
    qemu_put_8s(f, &bus1_selected);
}

static int pc98_ide_load(QEMUFile* f, void *opaque, int version_id)
{
    PC98IDEState *s = opaque;
    int i;
    uint8_t bus1_selected;

    for (i = 0; i < 2; i++) {
        idebus_load(f, s->bus[i], version_id);
        ide_load(f, &s->bus[i]->ifs[0], version_id);
        ide_load(f, &s->bus[i]->ifs[1], version_id);
    }
    qemu_get_8s(f, &bus1_selected);
    s->cur_bus = s->bus[bus1_selected != 0];

    return 0;
}

static void pc98_ide_reset(void *opaque)
{
    PC98IDEState *s = opaque;
    int i;

    for (i = 0; i < 2; i++) {
        ide_reset(&s->bus[i]->ifs[0]);
        ide_reset(&s->bus[i]->ifs[1]);
    }
    s->cur_bus = s->bus[0];
    s->connection = 0;
}

static int pc98_ide_initfn(ISADevice *dev)
{
    PC98IDEState *s = DO_UPCAST(PC98IDEState, dev, dev);
    int i;

    s->bus[0] = ide_bus_new(&s->dev.qdev);
    s->bus[1] = ide_bus_new(&s->dev.qdev);

    isa_init_irq(dev, &s->irq, s->isairq);
    ide_init2(s->bus[0], NULL, NULL, s->irq);
    ide_init2(s->bus[1], NULL, NULL, s->irq);

    register_ioport_write(0xf0, 1, 1, pc98_ide_cpu_shutdown, s);
    register_ioport_read(0xf0, 1, 1, pc98_ide_connection_read, s);
    register_ioport_write(0xf1, 1, 1, pc98_ide_connection_init, s);

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
    register_ioport_write(0x74c, 1, 1, pc98_ide_cmd_write, s);
    register_ioport_read(0x74c, 1, 1, pc98_ide_status_read, s);
    register_ioport_read(0x74e, 1, 1, pc98_ide_sdh_read, s);

    register_savevm("pc98-ide", 0, 1, pc98_ide_save, pc98_ide_load, s);
    qemu_register_reset(pc98_ide_reset, s);
    pc98_ide_reset(s);

    return 0;
};

int pc98_ide_init(int isairq,
                  DriveInfo *hd0, DriveInfo *hd1,
                  DriveInfo *hd2, DriveInfo *hd3)
{
    ISADevice *dev;
    PC98IDEState *s;

    dev = isa_create("pc98-ide");
    qdev_prop_set_uint32(&dev->qdev, "irq", isairq);
    if (qdev_init(&dev->qdev) != 0)
        return -1;

    s = DO_UPCAST(PC98IDEState, dev, dev);
    if (hd0) {
        ide_create_drive(s->bus[0], 0, hd0);
    }
    if (hd1) {
        ide_create_drive(s->bus[0], 1, hd1);
    }
    if (hd2) {
        ide_create_drive(s->bus[1], 0, hd2);
    }
    if (hd3) {
        ide_create_drive(s->bus[1], 1, hd3);
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
