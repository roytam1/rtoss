/*
 * QEMU NEC PC-9821 mouse
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
#include "console.h"
#include "sysemu.h"
#include "qemu-timer.h"

static const int64_t mouse_expire[4] = {120, 60, 30, 15};

struct MouseState {
    int button;
    int dx, dy;
    int lx, ly;
    uint8_t freq;
    uint8_t porta;
    uint8_t portb;
    uint8_t portc;
    uint8_t mode;

    QEMUTimer *mouse_timer;
    qemu_irq irq;
};

typedef struct MouseState MouseState;

/* mouse */

static void mouse_timer_handler(void *opaque)
{
    MouseState *s = opaque;

//    if (!(s->portc & 0x10)) {
        qemu_set_irq(s->irq, 1);
        qemu_set_irq(s->irq, 0);
//    }

    /* set next timer */
    qemu_mod_timer(s->mouse_timer, qemu_get_clock(rt_clock) +
                   1000 / mouse_expire[s->freq & 3]);
}

static void mouse_event_handler(void *opaque,
                                int dx, int dy, int dz, int buttons_state)
{
    MouseState *s = opaque;

    s->button = buttons_state;
    s->dx += dx;
    if (s->dx > 64) {
        s->dx = 64;
    } else if (s->dx < -64) {
        s->dx = -64;
    }
    s->dy += dy;
    if (s->dy > 64) {
        s->dy = 64;
    } else if (s->dy < -64) {
        s->dy = -64;
    }
}

static uint32_t mouse_irq_read(void *opaque, uint32_t addr)
{
    return 0x0d;
}

static void mouse_freq_write(void *opaque, uint32_t addr, uint32_t value)
{
    MouseState *s = opaque;

    if (!(value & 0xfc)) {
        s->freq = value;
    }
}

static uint32_t mouse_freq_read(void *opaque, uint32_t addr)
{
    MouseState *s = opaque;

    return s->freq & 3;
}

/* pio */

static void pio_porta_write(void *opaque, uint32_t addr, uint32_t value)
{
    MouseState *s = opaque;

    s->porta = value;
}

static uint32_t pio_porta_read(void *opaque, uint32_t addr)
{
    MouseState *s = opaque;
    uint32_t val = 0;

    if (s->mode & 0x10) {
        if (!(s->button & 1)) {
            val |= 0x80;
        }
        if (!(s->button & 4)) {
            val |= 0x40;
        }
        if (!(s->button & 2)) {
            val |= 0x20;
        }
        switch (s->portc & 0xe0) {
        case 0x00:
            val |= (s->dx >> 0) & 0x0f;
            break;
        case 0x20:
            val |= (s->dx >> 4) & 0x0f;
            break;
        case 0x40:
            val |= (s->dy >> 0) & 0x0f;
            break;
        case 0x60:
            val |= (s->dy >> 4) & 0x0f;
            break;
        case 0x80:
            val |= (s->lx >> 0) & 0x0f;
            break;
        case 0xa0:
            val |= (s->lx >> 4) & 0x0f;
            break;
        case 0xc0:
            val |= (s->ly >> 0) & 0x0f;
            break;
        case 0xe0:
            val |= (s->ly >> 4) & 0x0f;
            break;
        }
        return val;
    } else {
        return s->porta;
    }
}

static void pio_portb_write(void *opaque, uint32_t addr, uint32_t value)
{
    MouseState *s = opaque;

    s->portb = value;
}

static uint32_t pio_portb_read(void *opaque, uint32_t addr)
{
    MouseState *s = opaque;

    if (s->mode & 0x02) {
        return 0x40;
    } else {
        return s->portb;
    }
}

static void pio_portc_write(void *opaque, uint32_t addr, uint32_t value)
{
    MouseState *s = opaque;

    if (!(s->portc & 0x80) && (value & 0x80)) {
        /* latch mouse count */
        s->lx = s->dx;
        s->ly = s->dy;
        s->dx = s->dy = 0;
    }
    s->portc = value;
}

static uint32_t pio_portc_read(void *opaque, uint32_t addr)
{
    MouseState *s = opaque;
    uint32_t val = s->portc;

    if (s->mode & 0x08) {
        val &= 0x1f;
    }
    if (s->mode & 0x01) {
        val &= 0xf0;
        val |= 0x08;
    }
    return val;
}

static void pio_ctrl_write(void *opaque, uint32_t addr, uint32_t value)
{
    MouseState *s = opaque;

    if (!(value & 0x80)) {
        /* set/reset portc bit */
        uint8_t portc = s->portc;
        uint8_t bit = 1 << ((value >> 1) & 7);
        if (value & 1) {
            portc |= bit;
        } else {
            portc &= ~bit;
        }
        pio_portc_write(s, 0, portc);
    } else {
        s->mode = value;
    }
}

/* interface */

static void pc98_mouse_reset(void *opaque)
{
    MouseState *s = opaque;

    s->button = 0;
    s->dx = s->dy = 0;
    s->lx = s->ly = -1;
    s->freq = 0;
    s->porta = 0x00;
    s->portb = 0x00;
    s->portc = 0xf0;
    s->mode = 0x93;

    qemu_mod_timer(s->mouse_timer, qemu_get_clock(rt_clock) +
                   1000 / mouse_expire[0]);
}

static void pc98_mouse_save(QEMUFile *f, void *opaque)
{
    MouseState *s = opaque;

    qemu_put_be32s(f, &s->lx);
    qemu_put_be32s(f, &s->ly);
    qemu_put_8s(f, &s->freq);
    qemu_put_8s(f, &s->porta);
    qemu_put_8s(f, &s->portb);
    qemu_put_8s(f, &s->portc);
    qemu_put_8s(f, &s->mode);
}

static int pc98_mouse_load(QEMUFile *f, void *opaque, int version_id)
{
    MouseState *s = opaque;

    if (version_id != 1) {
        return -EINVAL;
    }

    pc98_mouse_reset(s);

    qemu_get_be32s(f, &s->lx);
    qemu_get_be32s(f, &s->ly);
    qemu_get_8s(f, &s->freq);
    qemu_get_8s(f, &s->porta);
    qemu_get_8s(f, &s->portb);
    qemu_get_8s(f, &s->portc);
    qemu_get_8s(f, &s->mode);

    return 0;
}

void pc98_mouse_init(qemu_irq irq)
{
    MouseState *s;

    s = qemu_mallocz(sizeof(MouseState));

    register_ioport_write(0x7fd9, 1, 1, pio_porta_write, s);
    register_ioport_read(0x7fd9, 1, 1, pio_porta_read, s);
    register_ioport_write(0x7fdb, 1, 1, pio_portb_write, s);
    register_ioport_read(0x7fdb, 1, 1, pio_portb_read, s);
    register_ioport_write(0x7fdd, 1, 1, pio_portc_write, s);
    register_ioport_read(0x7fdd, 1, 1, pio_portc_read, s);
    register_ioport_write(0x7fdf, 1, 1, pio_ctrl_write, s);
    register_ioport_read(0x9807, 1, 1, mouse_irq_read, s);
    register_ioport_write(0xbfdb, 1, 1, mouse_freq_write, s);
    register_ioport_read(0xbfdb, 1, 1, mouse_freq_read, s);

    s->irq = irq;

    s->mouse_timer = qemu_new_timer(rt_clock, mouse_timer_handler, s);
    qemu_add_mouse_event_handler(mouse_event_handler, s, 0, "pc98-mouse");

    register_savevm("pc98mouse", 0, 1, pc98_mouse_save, pc98_mouse_load, s);
    pc98_mouse_reset(s);
    qemu_register_reset(pc98_mouse_reset, s);
}
