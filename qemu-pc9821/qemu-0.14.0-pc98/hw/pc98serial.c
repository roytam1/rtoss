/*
 * QEMU NEC PC-9821 serial port
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
#include "qdev.h"
#include "pc.h"
#include "sysemu.h"
#include "qemu-timer.h"

#define SIO_BUFFER_SIZE 256
#define SIO_RECV_DELAY 8

enum {
    SIO_STAT_TXRDY  = 0x01,
    SIO_STAT_RXRDY  = 0x02,
    SIO_STAT_TXE    = 0x04,
    SIO_STAT_PE     = 0x08,
    SIO_STAT_OE     = 0x10,
    SIO_STAT_FE     = 0x20,
    SIO_STAT_SYNDET = 0x40,
    SIO_STAT_DSR    = 0x80,
};

enum {
    SIO_MODE_CLEAR,
    SIO_MODE_SYNC,
    SIO_MODE_ASYNC,
    SIO_MODE_SYNC1,
    SIO_MODE_SYNC2,
};

struct SerialPortState {
    /* isa */
    ISADevice dev;
    qemu_irq pic;
    uint32_t irq;

    /* sio */
    QEMUTimer *sio_timer;
    uint8_t mode;
    uint8_t status;
    uint8_t rxen;
    uint8_t txen;
    uint8_t recv_data;
    uint8_t recv_buf[SIO_BUFFER_SIZE];
    int recv_count;
    int recv_read;
    int recv_write;
    uint8_t send_buf[2];
    int send_count;
};

typedef struct SerialPortState SerialPortState;

static void serial_send(SerialPortState *s, uint8_t value)
{
}

static void serial_recv(SerialPortState *s, uint8_t value)
{
    if (s->recv_count < SIO_BUFFER_SIZE) {
        s->recv_buf[(s->recv_write++) & (SIO_BUFFER_SIZE - 1)] = value;
        s->recv_count++;
        if (s->recv_count == 1) {
            qemu_mod_timer(s->sio_timer, qemu_get_clock(rt_clock) +
                           SIO_RECV_DELAY);
        }
    }
}

static void sio_data_write(void *opaque, uint32_t addr, uint32_t value)
{
    SerialPortState *s = opaque;

    if (s->status & SIO_STAT_TXRDY) {
        s->status &= ~SIO_STAT_TXE;
        serial_send(s, value);
    }
}

static uint32_t sio_data_read(void *opaque, uint32_t addr)
{
    SerialPortState *s = opaque;

    s->status &= ~SIO_STAT_RXRDY;
    qemu_irq_lower(s->pic);
    return s->recv_data;
}

static void sio_cmd_write(void *opaque, uint32_t addr, uint32_t value)
{
    SerialPortState *s = opaque;

    switch(s->mode) {
    case SIO_MODE_CLEAR:
        if (value & 3) {
            s->mode = SIO_MODE_ASYNC;
        } else if (value & 0x80) {
            s->mode = SIO_MODE_SYNC2;	/* 1char */
        } else {
            s->mode = SIO_MODE_SYNC1;	/* 2chars */
        }
        break;
    case SIO_MODE_SYNC1:
        s->mode = SIO_MODE_SYNC2;
        break;
    case SIO_MODE_SYNC2:
        s->mode = SIO_MODE_SYNC;
        break;
    case SIO_MODE_ASYNC:
    case SIO_MODE_SYNC:
        if (value & 0x40) {
            s->mode = SIO_MODE_CLEAR;
            break;
        }
        if (value & 0x10) {
            s->status &= ~(SIO_STAT_PE | SIO_STAT_OE | SIO_STAT_FE);
        }
        s->rxen = value & 4;
        s->txen = value & 1;
        break;
    }
}

static uint32_t sio_status_read(void *opaque, uint32_t addr)
{
    SerialPortState *s = opaque;
    uint8_t value;

    value = s->status;
    if (s->txen) {
        s->status |= SIO_STAT_TXE;
    }
    return value;
}

static void sio_timer_handler(void *opaque)
{
    SerialPortState *s = opaque;

    if (s->recv_count > 0) {
        uint8_t value = s->recv_buf[(s->recv_read++) & 0xff];
        s->recv_count--;
        if (s->rxen) {
            if (!(s->status & SIO_STAT_RXRDY)) {
                s->status |= SIO_STAT_RXRDY;
                s->recv_data = value;
                qemu_irq_raise(s->pic);
            } else {
                /* time out */
                s->status |= SIO_STAT_OE;
            }
        }
        if (s->recv_count > 0) {
            qemu_mod_timer(s->sio_timer, qemu_get_clock(rt_clock) +
                           SIO_RECV_DELAY);
        }
    }
}

/* interface */

static void pc98_serial_reset(void *opaque)
{
    SerialPortState *s = opaque;

    s->mode = SIO_MODE_CLEAR;
    s->status = (SIO_STAT_TXRDY | SIO_STAT_TXE);
    s->txen = s->rxen = 0;
    s->recv_data = 0xff;
    s->recv_count = s->recv_read = s->recv_write = 0;
    s->send_count = 0;
}

static int pc98_serial_pre_load(void *opaque)
{
    pc98_serial_reset(opaque);
    return 0;
}

static const VMStateDescription vmstate_serial = {
    .name = "pc98-serial",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .pre_load = pc98_serial_pre_load,
    .fields = (VMStateField []) {
        VMSTATE_UINT8(mode, SerialPortState),
        VMSTATE_UINT8(status, SerialPortState),
        VMSTATE_UINT8(rxen, SerialPortState),
        VMSTATE_UINT8(txen, SerialPortState),
        VMSTATE_UINT8(recv_data, SerialPortState),
        VMSTATE_END_OF_LIST()
    }
};

static int pc98_serial_initfn(ISADevice *dev)
{
    SerialPortState *s = DO_UPCAST(SerialPortState, dev, dev);

    isa_init_irq(dev, &s->pic, s->irq);

    register_ioport_write(0x30, 1, 1, sio_data_write, s);
    register_ioport_read(0x30, 1, 1, sio_data_read, s);
    isa_init_ioport(dev, 0x30);
    register_ioport_write(0x32, 1, 1, sio_cmd_write, s);
    register_ioport_read(0x32, 1, 1, sio_status_read, s);
    isa_init_ioport(dev, 0x32);

    s->sio_timer = qemu_new_timer(rt_clock, sio_timer_handler, s);

    pc98_serial_reset(s);
    qemu_register_reset(pc98_serial_reset, s);

    return 0;
}

void pc98_serial_init(void)
{
    isa_create_simple("pc98-serial");
}

static ISADeviceInfo pc98_serial_info = {
    .qdev.name  = "pc98-serial",
    .qdev.desc  = "NEC PC-9821 Serial Port",
    .qdev.size  = sizeof(SerialPortState),
    .qdev.vmsd  = &vmstate_serial,
    .init       = pc98_serial_initfn,
    .qdev.props = (Property[]) {
        DEFINE_PROP_UINT32("irq", SerialPortState, irq, 4),
        DEFINE_PROP_END_OF_LIST (),
    },
};

static void pc98_serial_register(void)
{
    isa_qdev_register(&pc98_serial_info);
}
device_init(pc98_serial_register)
