/*
 * QEMU NEC PC-98x1 keyboard
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
#include "console.h"
#include "qemu-timer.h"
#include "isa.h"
#include "qdev-addr.h"

#define SIO_BUFFER_SIZE 256
#define SIO_RECV_DELAY 1600

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

enum {
    KBD_LOCK_NUMLK = 1,
    KBD_LOCK_CAPS  = 4,
    KBD_LOCK_KANA  = 8,
    KBD_LOCK_MASK  = (1 | 4 | 8),
};

/*
    END -> HELP
    F11 -> STOP
    F12 -> COPY
*/

static const uint8_t kbd_table[128] = {
    0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x74, 0x1d, 0x1e,
    0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
    0x27, 0xff, 0x70, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
    0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x70, 0xff,
    0x70, 0x34, 0xff, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x6b, 0xff, 0xff, 0x3e,
    0x3a, 0x36, 0xff, 0x3b, 0xff, 0x3c, 0xff, 0x3f,
    0x3d, 0x37, 0x38, 0x39, 0xff, 0xff, 0xff, 0x60,
    0x61, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x33, 0xff, 0xff, 0xff,
    0xff, 0x35, 0xff, 0x51, 0xff, 0x0d, 0xff, 0xff,
};

struct kbd_t {
    /* keyboard */
    uint8_t lock;
    uint8_t pressed[128];

    /* sio */
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

    QEMUTimer *sio_timer;
    qemu_irq irq;
};

typedef struct kbd_isabus_t {
    ISADevice busdev;
    struct kbd_t state;
} kbd_isabus_t;

typedef struct kbd_t kbd_t;

/* keyboard */

static void kbd_recv(void *opaque, uint8_t value)
{
    kbd_t *s = opaque;

    if (s->recv_count < SIO_BUFFER_SIZE) {
        s->recv_buf[(s->recv_write++) & (SIO_BUFFER_SIZE - 1)] = value;
        s->recv_count++;
        if (s->recv_count == 1) {
            qemu_mod_timer(s->sio_timer, qemu_get_clock(vm_clock) +
                           get_ticks_per_sec() / SIO_RECV_DELAY);
        }
    }
}

static void kbd_send(void *opaque, uint8_t value)
{
    kbd_t *s = opaque;

    s->send_buf[(s->send_count++) & 1] = value;

    switch (s->send_buf[0]) {
    case 0x95:
    case 0x9c:
        kbd_recv(s, 0xfa);
        if (s->send_count == 2) {
            s->send_count = 0;
        }
        break;
    case 0x96:
        kbd_recv(s, 0xfa);
        kbd_recv(s, 0xa0);
        kbd_recv(s, 0x85);/*0x86*/
        s->send_count = 0;
        break;
    case 0x99:
        kbd_recv(s, 0xfa);
        kbd_recv(s, 0xfb);
        s->send_count = 0;
        break;
    case 0x9d:
        kbd_recv(s, 0xfa);
        if (s->send_count == 2) {
            if (s->send_buf[1] == 0x60) {
                kbd_recv(s, 0x70 | (s->lock & KBD_LOCK_MASK));
            } else {
                s->lock = value;
            }
            s->send_count = 0;
        }
        break;
    case 0x9f:
        kbd_recv(s, 0xfa);
        kbd_recv(s, 0xa0);
        kbd_recv(s, 0x80);
        s->send_count = 0;
        break;
    default:
        kbd_recv(s, 0xfc);
        s->send_count = 0;
        break;
    }
}

static void kbd_event_handler(void *opaque, int keycode)
{
    kbd_t *s = opaque;

    if (keycode & 0x80) {
        /* key released */
        if ((keycode = kbd_table[keycode & 0x7f]) != 0xff) {
            if(s->pressed[keycode]) {
                kbd_recv(s, keycode | 0x80);
            }
            s->pressed[keycode] = 0;
        }
    } else {
        /* key pressed */
        if (keycode == 0x3a) {
            s->lock ^= KBD_LOCK_CAPS;
            if (s->lock & KBD_LOCK_CAPS) {
                kbd_recv(s, 0x71);
            } else {
                kbd_recv(s, 0x71 | 0x80);
            }
        } else if (keycode == 0x70) {
            s->lock ^= KBD_LOCK_KANA;
            if (s->lock & KBD_LOCK_KANA) {
                kbd_recv(s, 0x72);
            } else {
                kbd_recv(s, 0x72 | 0x80);
            }
        } else if ((keycode = kbd_table[keycode]) != 0xff) {
            if(s->pressed[keycode]) {
                kbd_recv(s, keycode | 0x80);
            }
            kbd_recv(s, keycode);
            s->pressed[keycode] = 1;
        }
    }
}

/* sio */

static void sio_data_write(void *opaque, uint32_t addr, uint32_t value)
{
    kbd_t *s = opaque;

    if (s->status & SIO_STAT_TXRDY) {
        s->status &= ~SIO_STAT_TXE;
        kbd_send(s, value);
    }
}

static uint32_t sio_data_read(void *opaque, uint32_t addr)
{
    kbd_t *s = opaque;

    s->status &= ~SIO_STAT_RXRDY;
    qemu_set_irq(s->irq, 0);
    return s->recv_data;
}

static void sio_cmd_write(void *opaque, uint32_t addr, uint32_t value)
{
    kbd_t *s = opaque;

    switch(s->mode) {
    case SIO_MODE_CLEAR:
        if(value & 3) {
            s->mode = SIO_MODE_ASYNC;
        } else if(value & 0x80) {
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
        if(value & 0x40) {
            s->mode = SIO_MODE_CLEAR;
            break;
        }
        if(value & 0x10) {
            s->status &= ~(SIO_STAT_PE | SIO_STAT_OE | SIO_STAT_FE);
        }
        s->rxen = value & 4;
        s->txen = value & 1;
        break;
    }
}

static uint32_t sio_status_read(void *opaque, uint32_t addr)
{
    kbd_t *s = opaque;
    uint8_t value;

    value = s->status;
    if (s->txen) {
        s->status |= SIO_STAT_TXE;
    }
    return value;
}

static void sio_timer_handler(void *opaque)
{
    kbd_t *s = opaque;

    if (s->recv_count > 0) {
        uint8_t value = s->recv_buf[(s->recv_read++) & 0xff];
        s->recv_count--;
        if (s->rxen) {
            if (!(s->status & SIO_STAT_RXRDY)) {
                s->status |= SIO_STAT_RXRDY;
                s->recv_data = value;
                qemu_set_irq(s->irq, 1);
            } else {
                /* time out */
                s->status |= SIO_STAT_OE;
            }
        }
        if (s->recv_count > 0) {
            qemu_mod_timer(s->sio_timer, qemu_get_clock(vm_clock) + 
                           get_ticks_per_sec() / SIO_RECV_DELAY);
        }
    }
}

/* interface */

static void pc98_kbd_reset(void *opaque)
{
    kbd_t *s = opaque;

    s->lock = 0;
    memset(s->pressed, 0, 128);

    s->mode = SIO_MODE_CLEAR;
    s->status = (SIO_STAT_TXRDY | SIO_STAT_TXE);
    s->txen = s->rxen = 0;
    s->recv_data = 0xff;
    s->recv_count = s->recv_read = s->recv_write = 0;
    s->send_count = 0;
}

static int pc98_kbd_pre_load(void *opaque)
{
    pc98_kbd_reset(opaque);
    return 0;
}

static const VMStateDescription vmstate_kbd = {
    .name = "pc98-kbd",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .pre_load = pc98_kbd_pre_load,
    .fields      = (VMStateField []) {
        VMSTATE_UINT8(lock, kbd_t),
        VMSTATE_UINT8(mode, kbd_t),
        VMSTATE_UINT8(status, kbd_t),
        VMSTATE_UINT8(rxen, kbd_t),
        VMSTATE_UINT8(txen, kbd_t),
        VMSTATE_UINT8(recv_data, kbd_t),
        VMSTATE_END_OF_LIST()
    }
};

static int pc98_kbd_init1(ISADevice *dev)
{
    kbd_isabus_t *isa = DO_UPCAST(kbd_isabus_t, busdev, dev);
    kbd_t *s = &isa->state;
    int isairq = 1;

    register_ioport_write(0x41, 1, 1, sio_data_write, s);
    register_ioport_read(0x41, 1, 1, sio_data_read, s);
    register_ioport_write(0x43, 1, 1, sio_cmd_write, s);
    register_ioport_read(0x43, 1, 1, sio_status_read, s);

    isa_init_irq(&isa->busdev, &s->irq, isairq);

    s->sio_timer = qemu_new_timer(vm_clock, sio_timer_handler, s);
    qemu_add_kbd_event_handler(kbd_event_handler, s);

    vmstate_register(-1, &vmstate_kbd, s);
    pc98_kbd_reset(s);
    qemu_register_reset(pc98_kbd_reset, s);

    return 0;
}

static ISADeviceInfo pc98_kbd_info = {
    .init = pc98_kbd_init1,
    .qdev.name  = "pc98-kbd",
    .qdev.size  = sizeof(kbd_isabus_t),
};

static void pc98_kbd_register_devices(void)
{
    isa_qdev_register(&pc98_kbd_info);
}

device_init(pc98_kbd_register_devices)
