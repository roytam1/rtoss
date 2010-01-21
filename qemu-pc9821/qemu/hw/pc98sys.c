/*
 * QEMU NEC PC-98x1 systemp port
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

#define RTC_STROBE      0x08
#define RTC_CLOCK       0x10
#define RTC_DIN         0x20

#define TSTMP_FREQ      307200

static const int64_t rtc_expire[3] = {64, 256, 2048};

struct sysport_t {
    uint8_t rtc_cmd;
    uint8_t rtc_mode;
    uint8_t rtc_tpmode;
    uint64_t rtc_shift;
    uint8_t sys_portc;
    int sys_portc_patch;
    uint8_t prn_porta;
    uint8_t prn_portc;

    uint8_t mbcfg1[256];
    uint8_t mbcfg1_bank;
    uint8_t mbcfg2[256];
    uint8_t mbcfg2_bank;

    uint16_t unkreg[4][256];
    uint8_t unkreg_bank1;
    uint8_t unkreg_bank2;

    int64_t initial_clock;
    int64_t expire_clock;
    QEMUTimer *rtc_timer;
    qemu_irq irq;
};

typedef struct sysport_isabus_t {
    ISADevice busdev;
    struct sysport_t state;
} sysport_isabus_t;

typedef struct sysport_t sysport_t;

/* rtc */

static void rtc_timer_handler(void *opaque)
{
    sysport_t *s = opaque;

    if (s->rtc_tpmode != 3) {
        qemu_set_irq(s->irq, 1);
        qemu_set_irq(s->irq, 0);

        /* set next timer */
        s->expire_clock += get_ticks_per_sec() / rtc_expire[s->rtc_tpmode];
        qemu_mod_timer(s->rtc_timer, s->expire_clock);
    }
}

static inline int64_t to_bcd(int a)
{
    return ((a / 10) << 4) | (a % 10);
}

static void rtc_cmd_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    if ((s->rtc_cmd & RTC_STROBE) && !(value & RTC_STROBE)) {
        if (!(s->rtc_cmd & RTC_CLOCK)) {
            if (s->rtc_cmd & 4) {
                uint8_t tpmode = s->rtc_cmd & 3;
                if (s->rtc_tpmode == 3 && tpmode != 3) {
                    s->expire_clock = qemu_get_clock(vm_clock) + 
                                      get_ticks_per_sec() / rtc_expire[tpmode];
                    qemu_mod_timer(s->rtc_timer, s->expire_clock);
                } else if (s->rtc_tpmode != 3 && tpmode == 3) {
                    qemu_del_timer(s->rtc_timer);
                }
                s->rtc_tpmode = tpmode;
            } else {
                s->rtc_mode = s->rtc_cmd & 3;
                if (s->rtc_mode == 3) {
                    struct tm tm;
                    qemu_get_timedate(&tm, 0);
                    s->rtc_shift = to_bcd(tm.tm_sec);
                    s->rtc_shift |= to_bcd(tm.tm_min) << 8;
                    s->rtc_shift |= to_bcd(tm.tm_hour) << 16;
                    s->rtc_shift |= to_bcd(tm.tm_mday) << 24;
                    s->rtc_shift |= (int64_t)tm.tm_wday << 32;
                    s->rtc_shift |= (int64_t)tm.tm_mon << 36;
                    s->rtc_shift |= to_bcd(tm.tm_year % 100) << 40;
                }
            }
        }
    }
    if (!(s->rtc_cmd & RTC_CLOCK) && (value & RTC_CLOCK)) {
        if (s->rtc_mode == 1) {
            uint64_t din = ((s->rtc_cmd & 0x20) != 0);
            s->rtc_shift |= (din << 40);
            s->rtc_shift >>= 1;
        }
    }
    s->rtc_cmd = value;
}

static uint32_t rtc_mode_read(void *opaque, uint32_t addr)
{
    return 0xef;
}

/* system port */

static uint32_t sys_porta_read(void *opaque, uint32_t addr)
{
    return 0x73;
}

static uint32_t sys_portb_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return 0xf8 | (s->rtc_shift & 1);
}

static void sys_portc_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->sys_portc = value;
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

/* mother board configs */

static void mbcfg1_bank_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->mbcfg1_bank = value;
}

static uint32_t mbcfg1_bank_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->mbcfg1_bank;
}

static void mbcfg1_data_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->mbcfg1[s->mbcfg1_bank] = value;
}

static uint32_t mbcfg1_data_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    if (s->mbcfg1_bank == 0) {
        uint32_t value = s->mbcfg1[0];
        s->mbcfg1[0] += 0x40;
        return value;
    }
    return s->mbcfg1[s->mbcfg1_bank];
}

static void mbcfg2_bank_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->mbcfg2_bank = value;
}

static uint32_t mbcfg2_bank_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->mbcfg2_bank;
}

static void mbcfg2_data_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->mbcfg2[s->mbcfg2_bank] = value;
}

static uint32_t mbcfg2_data_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->mbcfg2[s->mbcfg2_bank];
}

static void unkreg_bank_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->unkreg_bank1 = value;
    s->unkreg_bank2 = 0;
}

static uint32_t unkreg_bank_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->unkreg_bank1;
}

static void unkreg_data_write(void *opaque, uint32_t addr, uint32_t value)
{
    sysport_t *s = opaque;

    s->unkreg[(s->unkreg_bank2++) & 3][s->unkreg_bank1] = value;
}

static uint32_t unkreg_data_read(void *opaque, uint32_t addr)
{
    sysport_t *s = opaque;

    return s->unkreg[(s->unkreg_bank2++) & 3][s->unkreg_bank1];
}

/* interface */

static void pc98_sys_reset(void *opaque)
{
    sysport_t *s = opaque;

    s->rtc_cmd = RTC_CLOCK;
    s->rtc_mode = 0;
    s->rtc_tpmode = 3;
    s->rtc_shift = 0;

    s->sys_portc = 0xff;//b8;
    s->sys_portc_patch = 8;
    s->prn_porta = 0xff;
    s->prn_portc = 0x81;

    memset(s->mbcfg1, 0, sizeof(s->mbcfg1));
    s->mbcfg1[0] = 0x10;
    s->mbcfg1_bank = 0;
    memset(s->mbcfg2, 0, sizeof(s->mbcfg2));
    s->mbcfg2_bank = 0;

    memset(s->unkreg, 0, sizeof(s->unkreg));
    s->unkreg_bank1 = s->unkreg_bank2 = 0;
}

static int pc98_sys_post_load(void *opaque, int version_id)
{
    sysport_t *s = opaque;

    if (s->rtc_tpmode != 3) {
        s->expire_clock = qemu_get_clock(vm_clock) +
                          get_ticks_per_sec() / rtc_expire[s->rtc_tpmode];
        qemu_mod_timer(s->rtc_timer, s->expire_clock);
    } else {
        qemu_del_timer(s->rtc_timer);
    }
    return 0;
}

static const VMStateDescription vmstate_sysport = {
    .name = "pc98-sys",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .post_load = pc98_sys_post_load,
    .fields      = (VMStateField []) {
        VMSTATE_UINT8(rtc_cmd, sysport_t),
        VMSTATE_UINT8(rtc_mode, sysport_t),
        VMSTATE_UINT8(rtc_tpmode, sysport_t),
        VMSTATE_UINT64(rtc_shift, sysport_t),
        VMSTATE_UINT8(sys_portc, sysport_t),
        VMSTATE_UINT8(prn_porta, sysport_t),
        VMSTATE_UINT8(prn_portc, sysport_t),
        VMSTATE_BUFFER(mbcfg1, sysport_t),
        VMSTATE_UINT8(mbcfg1_bank, sysport_t),
        VMSTATE_BUFFER(mbcfg2, sysport_t),
        VMSTATE_UINT8(mbcfg2_bank, sysport_t),
        VMSTATE_END_OF_LIST()
    }
};

static int pc98_sys_init1(ISADevice *dev)
{
    sysport_isabus_t *isa = DO_UPCAST(sysport_isabus_t, busdev, dev);
    sysport_t *s = &isa->state;
    int isairq = 15;

    register_ioport_write(0x20, 1, 1, rtc_cmd_write, s);
    register_ioport_read(0x22, 1, 1, rtc_mode_read, s);

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

    register_ioport_write(0x411, 1, 1, mbcfg1_bank_write, s);
    register_ioport_read(0x411, 1, 1, mbcfg1_bank_read, s);
    register_ioport_write(0x413, 1, 1, mbcfg1_data_write, s);
    register_ioport_read(0x413, 1, 1, mbcfg1_data_read, s);

    register_ioport_write(0xb00, 2, 2, mbcfg2_bank_write, s);
    register_ioport_read(0xb00, 2, 2, mbcfg2_bank_read, s);
    register_ioport_write(0xb02, 1, 1, mbcfg2_data_write, s);
    register_ioport_read(0xb02, 1, 1, mbcfg2_data_read, s);

    register_ioport_write(0x18f0, 2, 2, unkreg_bank_write, s);
    register_ioport_read(0x18f0, 2, 2, unkreg_bank_read, s);
    register_ioport_write(0x18f2, 2, 2, unkreg_data_write, s);
    register_ioport_read(0x18f2, 2, 2, unkreg_data_read, s);

    isa_init_irq(&isa->busdev, &s->irq, isairq);

    s->rtc_timer = qemu_new_timer(vm_clock, rtc_timer_handler, s);
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
