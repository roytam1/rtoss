/*
 * QEMU NEC PC-9821 systemp port
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
#include "qemu-timer.h"
#include "isa.h"
#include "qdev-addr.h"

enum {
    RTC_STROBE          = 0x08,
    RTC_CLOCK           = 0x10,
    RTC_DIN             = 0x20,
};

enum {
    RTC_CMD_SHIFT       = 0x01,
    RTC_CMD_READ        = 0x03,
    RTC_CMD_EXMODE      = 0x07,
};

enum {
    RTC_MODE_UPD4993A   = 0x20,
};

#define TSTMP_FREQ      307200

struct sysport_t {
    uint8_t rtc_mode;
    uint8_t rtc_reg;
    uint8_t rtc_cmd;
    uint64_t rtc_shift_out;
    uint8_t rtc_shift_cmd;
    uint8_t rtc_irq_mode;
    uint8_t rtc_irq_count;

    uint8_t sys_portc;
    int sys_portc_patch;
    uint8_t prn_porta;
    uint8_t prn_portc;

    QEMUTimer *rtc_timer;
    qemu_irq irq;
    int64_t initial_clock;
};

typedef struct sysport_isabus_t {
    ISADevice busdev;
    struct sysport_t state;
} sysport_isabus_t;

typedef struct sysport_t sysport_t;

/* NEC uPD4993A RTC */

static void rtc_timer_handler(void *opaque)
{
    sysport_t *s = opaque;

    /* rtc_irq_mode = 0 : 1/64 sec
                    = 1 : 1/32 sec
                    = 2 : no irq
                    = 3 : 1/16 sec
    */
    if (s->rtc_irq_mode != 2) {
        if (++s->rtc_irq_count > s->rtc_irq_mode) {
            qemu_set_irq(s->irq, 1);
            qemu_set_irq(s->irq, 0);
            s->rtc_irq_count = 0;
        }
    }

    /* set next timer */
    qemu_mod_timer(s->rtc_timer, qemu_get_clock(vm_clock) +
                   get_ticks_per_sec() / 64);
}

static inline uint64_t to_bcd(int a)
{
    return ((a / 10) << 4) | (a % 10);
}

static void rtc_read_time(void *opaque)
{
    sysport_t *s = opaque;
    struct tm tm;

    qemu_get_timedate(&tm, 0);
    s->rtc_shift_out = to_bcd(tm.tm_sec);
    s->rtc_shift_out |= to_bcd(tm.tm_min) << 8;
    s->rtc_shift_out |= to_bcd(tm.tm_hour) << 16;
    s->rtc_shift_out |= to_bcd(tm.tm_mday) << 24;
    s->rtc_shift_out |= (uint64_t)tm.tm_wday << 32;
    s->rtc_shift_out |= (uint64_t)(tm.tm_mon + 1) << 36;
    s->rtc_shift_out |= to_bcd(tm.tm_year % 100) << 40;
    if (s->rtc_mode & RTC_MODE_UPD4993A) {
        s->rtc_shift_out <<= 4;
    }
}

static void rtc_reg_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    if ((s->rtc_reg & RTC_STROBE) && !(value & RTC_STROBE)) {
        s->rtc_cmd = s->rtc_reg & 0x07;
        if (s->rtc_cmd == RTC_CMD_READ) {
            rtc_read_time(s);
        } else if (s->rtc_cmd == RTC_CMD_EXMODE) {
            s->rtc_cmd = s->rtc_shift_cmd & 0x0f;
            if (s->rtc_cmd == RTC_CMD_READ) {
                rtc_read_time(s);
            }
        }
    }
    if ((s->rtc_reg & RTC_CLOCK) && !(value & RTC_CLOCK)) {
        uint8_t din = ((s->rtc_reg & RTC_DIN) != 0);
//        if ((s->rtc_reg & 0x07) == RTC_CMD_EXMODE) {
            s->rtc_shift_cmd |= din << 4;
            s->rtc_shift_cmd >>= 1;
//        } else if (s->rtc_cmd == RTC_CMD_SHIFT) {
            s->rtc_shift_out >>= 1;
//        }
    }
    s->rtc_reg = value;
}

static void rtc_mode_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->rtc_mode = value;
}

static uint32_t rtc_mode_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->rtc_mode;
}

static void rtc_irq_mode_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->rtc_irq_mode = value & 0x03;
    s->rtc_irq_count = 0;
}

/* system port */

static uint32_t sys_porta_read(void *opaque, uint32_t addr)
{
    return 0x73;
}

static uint32_t sys_portb_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return 0xf8 | (s->rtc_shift_out & 1);
}

static void sys_portc_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->sys_portc = value;
    pc98_pcspk_write(value);
}

static uint32_t sys_portc_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    if (s->sys_portc_patch) {
        /* patch for itf protect mode patch */
        s->sys_portc_patch--;
        if (s->sys_portc_patch == 0 || s->sys_portc_patch == 4) {
            s->sys_portc |= 0x20;
        }
    }
    return s->sys_portc;
}

static void sys_ctrl_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    if (!(value & 0x80)) {
        /* set/reset portc bit */
        uint32_t portc = s->sys_portc;
        uint32_t bit = 1 << ((value >> 1) & 7);
        if (value & 1) {
            portc |= bit;
        } else {
            portc &= ~bit;
        }
        sys_portc_write(s, 0, portc);
    }
}

/* printer port */

static void prn_porta_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->prn_porta = value;
}

static uint32_t prn_porta_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->prn_porta;
}

static uint32_t prn_portb_read(void *opaque, uint32_t addr)
{
#ifdef PC98_SYSCLOCK_5MHZ
    return 0x94;
#else
    return 0xb4;
#endif
}

static void prn_portc_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->prn_portc = value;
}

static uint32_t prn_portc_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->prn_portc;
}

static void prn_ctrl_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    if (!(value & 0x80)) {
        /* set/reset portc bit */
        uint8_t portc = s->prn_portc;
        uint8_t bit = 1 << ((value >> 1) & 7);
        if (value & 1) {
            portc |= bit;
        } else {
            portc &= ~bit;
        }
        prn_portc_write(s, 0, portc);
    }
}

/* time stamper */

static uint32_t tstmp_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;
    uint64_t d = muldiv64(qemu_get_clock(vm_clock) - s->initial_clock,
                          TSTMP_FREQ, get_ticks_per_sec());

    switch(addr) {
    case 0x5c:
        return d & 0xffff;
    case 0x5e:
        return (d >> 8) & 0xffff;
    }
    return 0xffff;
}

/* interface */

static void pc98_sys_reset(void *opaque)
{
    sysport_t *s = opaque;

    s->rtc_mode = 0xff;
    s->rtc_reg = 0;
    s->rtc_cmd = 0;
    s->rtc_irq_mode = 2;

    s->sys_portc = 0xff;//b8;
    s->sys_portc_patch = 8;
    s->prn_porta = 0xff;
    s->prn_portc = 0x81;
}

static const VMStateDescription vmstate_sysport = {
    .name = "pc98-sys",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields      = (VMStateField []) {
        VMSTATE_UINT8(rtc_reg, sysport_t),
        VMSTATE_UINT8(rtc_cmd, sysport_t),
        VMSTATE_UINT64(rtc_shift_out, sysport_t),
        VMSTATE_UINT8(rtc_shift_cmd, sysport_t),
        VMSTATE_UINT8(rtc_irq_mode, sysport_t),
        VMSTATE_UINT8(rtc_irq_count, sysport_t),
        VMSTATE_UINT8(sys_portc, sysport_t),
        VMSTATE_UINT8(prn_porta, sysport_t),
        VMSTATE_UINT8(prn_portc, sysport_t),
        VMSTATE_END_OF_LIST()
    }
};

static int pc98_sys_init1(ISADevice *dev)
{
    sysport_isabus_t *isa = DO_UPCAST(sysport_isabus_t, busdev, dev);
    sysport_t *s = &isa->state;
    int isairq = 15;

    register_ioport_write(0x20, 1, 1, rtc_reg_write, s);
    register_ioport_write(0x22, 1, 1, rtc_mode_write, s);
    register_ioport_read(0x22, 1, 1, rtc_mode_read, s);
    register_ioport_write(0x128, 1, 1, rtc_irq_mode_write, s);

    register_ioport_read(0x31, 1, 1, sys_porta_read, s);
    register_ioport_read(0x33, 1, 1, sys_portb_read, s);
    register_ioport_write(0x35, 1, 1, sys_portc_write, s);
    register_ioport_read(0x35, 1, 1, sys_portc_read, s);
    register_ioport_write(0x37, 1, 1, sys_ctrl_write, s);

    register_ioport_write(0x40, 1, 1, prn_porta_write, s);
    register_ioport_read(0x40, 1, 1, prn_porta_read, s);
    register_ioport_read(0x42, 1, 1, prn_portb_read, s);
    register_ioport_write(0x44, 1, 1, prn_portc_write, s);
    register_ioport_read(0x44, 1, 1, prn_portc_read, s);
    register_ioport_write(0x46, 1, 1, prn_ctrl_write, s);

    register_ioport_read(0x5c, 2, 2, tstmp_read, s);
    register_ioport_read(0x5e, 2, 2, tstmp_read, s);

    isa_init_irq(&isa->busdev, &s->irq, isairq);

    s->rtc_timer = qemu_new_timer(vm_clock, rtc_timer_handler, s);
    qemu_mod_timer(s->rtc_timer, qemu_get_clock(vm_clock) +
                   get_ticks_per_sec() / 64);
    s->initial_clock = qemu_get_clock(vm_clock);

    vmstate_register(-1, &vmstate_sysport, s);
    pc98_sys_reset(s);
    qemu_register_reset(pc98_sys_reset, s);

    return 0;
}

static ISADeviceInfo pc98_sys_info = {
    .init = pc98_sys_init1,
    .qdev.name  = "pc98-sys",
    .qdev.size  = sizeof(sysport_isabus_t),
};

static void pc98_sys_register_devices(void)
{
    isa_qdev_register(&pc98_sys_info);
}

device_init(pc98_sys_register_devices)
