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
#include "isa.h"
#include "qdev.h"
#include "pc.h"
#include "sysemu.h"
#include "loader.h"
#include "qemu-timer.h"

#define SDIP_FILE_NAME "pc98sdip.bin"

enum {
    RTC_STROBE = 0x08,
    RTC_CLOCK  = 0x10,
    RTC_DIN    = 0x20,
};

enum {
    RTC_CMD_SHIFT  = 0x01,
    RTC_CMD_READ   = 0x03,
    RTC_CMD_EXMODE = 0x07,
};

enum {
    RTC_MODE_UPD4993A = 0x20,
};

#define TSTMP_FREQ 307200

struct SysPortState {
    /* isa */
    ISADevice dev;
    qemu_irq pic;
    uint32_t irq;

    /* rtc */
    QEMUTimer *rtc_timer;
    uint8_t rtc_mode;
    uint8_t rtc_reg;
    uint8_t rtc_cmd;
    uint64_t rtc_shift_out;
    uint8_t rtc_shift_cmd;
    uint8_t rtc_irq_mode;
    uint8_t rtc_irq_count;

    /* system port */
    uint8_t sys_porta;
    uint8_t sys_portb;
    uint8_t sys_portc;
    int sys_portc_patch;
    uint8_t sys_mode;

    /* printer */
    uint8_t prn_porta;
    uint8_t prn_portb;
    uint8_t prn_portc;
    uint8_t prn_mode;

    /* time stamper */
    int64_t tstmp_init_clock;

    /* software dip-switch */
    uint8_t sdip[24];
    uint8_t sdip_bank;
};

typedef struct SysPortState SysPortState;

/* NEC uPD4993A RTC */

// 1000/64 = 15.625
static const int rtc_period[8] = { 15, 16, 16, 15, 16, 16, 15, 16 };
static int rtc_period_ptr = 0;

static void rtc_timer_handler(void *opaque)
{
    SysPortState *s = opaque;

    /* rtc_irq_mode = 0 : 1/64 sec
                    = 1 : 1/32 sec
                    = 2 : no irq
                    = 3 : 1/16 sec
    */
    if ((s->rtc_irq_mode & 0x03) != 2) {
        if (++s->rtc_irq_count > (s->rtc_irq_mode & 0x03)) {
            qemu_irq_raise(s->pic);
            s->rtc_irq_count = 0;
        }
    }

    /* set next timer */
    qemu_mod_timer(s->rtc_timer, qemu_get_clock(rt_clock) +
                   rtc_period[(rtc_period_ptr++) & 7]);
}

static void rtc_read_time(SysPortState *s)
{
    struct tm tm;

    qemu_get_timedate(&tm, 0);
    s->rtc_shift_out = (uint64_t)to_bcd(tm.tm_sec);
    s->rtc_shift_out |= (uint64_t)to_bcd(tm.tm_min) << 8;
    s->rtc_shift_out |= (uint64_t)to_bcd(tm.tm_hour) << 16;
    s->rtc_shift_out |= (uint64_t)to_bcd(tm.tm_mday) << 24;
    s->rtc_shift_out |= (uint64_t)tm.tm_wday << 32;
    s->rtc_shift_out |= (uint64_t)(tm.tm_mon + 1) << 36;
    s->rtc_shift_out |= (uint64_t)to_bcd(tm.tm_year % 100) << 40;
    if (s->rtc_mode & RTC_MODE_UPD4993A) {
        s->rtc_shift_out <<= 4;
    }
}

static void rtc_reg_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

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
    SysPortState *s = opaque;

    s->rtc_mode = value;
}

static uint32_t rtc_mode_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    return s->rtc_mode;
}

static void rtc_irq_mode_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->rtc_irq_mode = value;
    s->rtc_irq_count = 0;
}

static uint32_t rtc_irq_mode_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    qemu_irq_lower(s->pic);
    return s->rtc_irq_mode;
}

/* system port */

static void sys_porta_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->sys_porta = value;
}

static uint32_t sys_porta_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    if (s->sys_mode & 0x10) {
        return 0x73;
    } else {
        return s->sys_porta;
    }
}

static void sys_portb_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->sys_portb = value;
}

static uint32_t sys_portb_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    if (s->sys_mode & 0x02) {
        return 0xf8 | (s->rtc_shift_out & 1);
    } else {
        return s->sys_porta;
    }
}

static void sys_portc_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->sys_portc = value;
    pc98_pcspk_write(value);
}

static uint32_t sys_portc_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    if (s->sys_portc_patch) {
        /* itf protect mode patch */
        s->sys_portc_patch--;
        if (s->sys_portc_patch == 0 || s->sys_portc_patch == 4) {
            s->sys_portc |= 0x20;
        }
    }
    return s->sys_portc;
}

static void sys_ctrl_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

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
    } else {
        s->sys_mode = value;
    }
}

uint8_t pc98_sys_read_shut(void *opaque)
{
    /* this function is called in pc.c */
    SysPortState *s = opaque;

    return ((s->sys_portc & 0xa0) == 0xa0);
}

/* printer port */

static void prn_porta_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->prn_porta = value;
}

static uint32_t prn_porta_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    return s->prn_porta;
}

static void prn_portb_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->prn_portb = value;
}

static uint32_t prn_portb_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    if (s->prn_mode & 0x02) {
#ifdef PC98_SYSCLOCK_5MHZ
        return 0x94;
#else
        return 0xb4;
#endif
    } else {
        return s->prn_portb;
    }
}

static void prn_portc_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->prn_portc = value;
}

static uint32_t prn_portc_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;

    return s->prn_portc;
}

static void prn_ctrl_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

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
    } else {
        s->prn_mode = value;
    }
}

/* time stamper */

static uint32_t tstmp_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;
    uint64_t d = muldiv64(qemu_get_clock_ns(rt_clock) - s->tstmp_init_clock,
                          TSTMP_FREQ, get_ticks_per_sec());

    switch(addr) {
    case 0x5c:
        return d & 0xffff;
    case 0x5e:
        return (d >> 8) & 0xffff;
    }
    return 0xffff;
}

/* software dip-switch */

static void sdip_save(SysPortState *s)
{
    char *filename;
    FILE* fp;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, SDIP_FILE_NAME);
    if ((fp = fopen(filename, "wb")) != NULL) {
        fwrite(s->sdip, 24, 1, fp);
        fclose(fp);
    }
}

static int sdip_load(SysPortState *s)
{
    char *filename;
    FILE* fp;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, SDIP_FILE_NAME);
    if ((fp = fopen(filename, "rb")) != NULL) {
        fwrite(s->sdip, 24, 1, fp);
        fclose(fp);
        return 0;
    }
    return -1;
}

static void sdip_data_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;
    uint8_t bank;

    if (s->sdip_bank & 0x40) {
        bank = ((addr >> 8) & 0x0f) + 8;
    } else {
        bank = ((addr >> 8) & 0x0f) - 4;
    }
    if (s->sdip[bank] != value) {
        s->sdip[bank] = value;
        /* save software dip-switch */
//        sdip_save(s);
    }
}

static uint32_t sdip_data_read(void *opaque, uint32_t addr)
{
    SysPortState *s = opaque;
    uint8_t bank;
 
    if (s->sdip_bank & 0x40) {
        bank = ((addr >> 8) & 0x0f) + 8;
    } else {
        bank = ((addr >> 8) & 0x0f) - 4;
    }
    return s->sdip[bank];
}

static void sdip_bank_write(void *opaque, uint32_t addr, uint32_t value)
{
    SysPortState *s = opaque;

    s->sdip_bank = value;
}

/* interface */

static void pc98_sys_reset(void *opaque)
{
    SysPortState *s = opaque;

    s->rtc_mode = 0xff;
    s->rtc_reg = 0;
    s->rtc_cmd = 0;
    s->rtc_irq_mode = 2;

    s->sys_porta = 0x00;
    s->sys_portb = 0x00;
    s->sys_portc = 0xff;//b8;
    s->sys_portc_patch = 8;
    s->sys_mode = 0x92;

    s->prn_porta = 0xff;
    s->prn_portb = 0x00;
    s->prn_portc = 0x81;
    s->prn_mode = 0x82;

    s->sdip_bank = 0;
}

static const VMStateDescription vmstate_sys = {
    .name = "pc98-sys",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField []) {
        VMSTATE_UINT8(rtc_reg, SysPortState),
        VMSTATE_UINT8(rtc_cmd, SysPortState),
        VMSTATE_UINT64(rtc_shift_out, SysPortState),
        VMSTATE_UINT8(rtc_shift_cmd, SysPortState),
        VMSTATE_UINT8(rtc_irq_mode, SysPortState),
        VMSTATE_UINT8(rtc_irq_count, SysPortState),
        VMSTATE_UINT8(sys_porta, SysPortState),
        VMSTATE_UINT8(sys_portb, SysPortState),
        VMSTATE_UINT8(sys_portc, SysPortState),
        VMSTATE_UINT8(sys_mode, SysPortState),
        VMSTATE_UINT8(prn_porta, SysPortState),
        VMSTATE_UINT8(prn_portb, SysPortState),
        VMSTATE_UINT8(prn_portc, SysPortState),
        VMSTATE_UINT8(prn_mode, SysPortState),
        VMSTATE_UINT8_ARRAY(sdip, SysPortState, 24),
        VMSTATE_UINT8(sdip_bank, SysPortState),
        VMSTATE_END_OF_LIST()
    }
};

static const uint8_t sdip_default[] = {
    0x7c, 0x73, 0x76, 0x3e, 0xdc, 0x7f, 0xff, 0xbf, 0x7f, 0x7f, 0x49, 0x98,
    0x8f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

static int pc98_sys_initfn(ISADevice *dev)
{
    SysPortState *s = DO_UPCAST(SysPortState, dev, dev);
    int i;

    isa_init_irq(dev, &s->pic, s->irq);

//    if (sdip_load(s)) {
        memcpy(s->sdip, sdip_default, 24);
//    }

    register_ioport_write(0x20, 1, 1, rtc_reg_write, s);
    isa_init_ioport(dev, 0x20);
    register_ioport_write(0x22, 1, 1, rtc_mode_write, s);
    register_ioport_read(0x22, 1, 1, rtc_mode_read, s);
    isa_init_ioport(dev, 0x22);
    register_ioport_write(0x128, 1, 1, rtc_irq_mode_write, s);
    register_ioport_read(0x128, 1, 1, rtc_irq_mode_read, s);
    isa_init_ioport(dev, 0x128);

    register_ioport_write(0x31, 1, 1, sys_porta_write, s);
    register_ioport_read(0x31, 1, 1, sys_porta_read, s);
    isa_init_ioport(dev, 0x31);
    register_ioport_write(0x33, 1, 1, sys_portb_write, s);
    register_ioport_read(0x33, 1, 1, sys_portb_read, s);
    isa_init_ioport(dev, 0x33);
    register_ioport_write(0x35, 1, 1, sys_portc_write, s);
    register_ioport_read(0x35, 1, 1, sys_portc_read, s);
    isa_init_ioport(dev, 0x35);
    register_ioport_write(0x37, 1, 1, sys_ctrl_write, s);
    isa_init_ioport(dev, 0x37);

    register_ioport_write(0x40, 1, 1, prn_porta_write, s);
    register_ioport_read(0x40, 1, 1, prn_porta_read, s);
    isa_init_ioport(dev, 0x40);
    register_ioport_write(0x42, 1, 1, prn_portb_write, s);
    register_ioport_read(0x42, 1, 1, prn_portb_read, s);
    isa_init_ioport(dev, 0x42);
    register_ioport_write(0x44, 1, 1, prn_portc_write, s);
    register_ioport_read(0x44, 1, 1, prn_portc_read, s);
    isa_init_ioport(dev, 0x44);
    register_ioport_write(0x46, 1, 1, prn_ctrl_write, s);
    isa_init_ioport(dev, 0x46);

    register_ioport_read(0x5c, 2, 2, tstmp_read, s);
    isa_init_ioport_range(dev, 0x5c, 2);
    register_ioport_read(0x5e, 2, 2, tstmp_read, s);
    isa_init_ioport_range(dev, 0x5e, 2);

    for (i = 0; i < 12; i++) {
        register_ioport_write(0x841e + i * 0x100, 1, 1, sdip_data_write, s);
        register_ioport_read(0x841e + i * 0x100, 1, 1, sdip_data_read, s);
        isa_init_ioport(dev, 0x841e + i * 0x100);
    }
    register_ioport_write(0x8f1f, 1, 1, sdip_bank_write, s);
    isa_init_ioport(dev, 0x8f1f);
    register_ioport_write(0xf0f6, 1, 1, sdip_bank_write, s);
    isa_init_ioport(dev, 0xf0f6);

    s->rtc_timer = qemu_new_timer(rt_clock, rtc_timer_handler, s);
    qemu_mod_timer(s->rtc_timer, qemu_get_clock(rt_clock) +
                   rtc_period[(rtc_period_ptr++) & 7]);
    s->tstmp_init_clock = qemu_get_clock_ns(rt_clock);

    pc98_sys_reset(s);
    qemu_register_reset(pc98_sys_reset, s);

    return 0;
}

void *pc98_sys_init(void)
{
    ISADevice *dev;
    SysPortState *s;

    dev = isa_create_simple("pc98-sys");
    s = DO_UPCAST(SysPortState, dev, dev);

    return s;
}

static ISADeviceInfo pc98_sys_info = {
    .qdev.name  = "pc98-sys",
    .qdev.desc  = "NEC PC-9821 System Port",
    .qdev.size  = sizeof(SysPortState),
    .qdev.vmsd  = &vmstate_sys,
    .init       = pc98_sys_initfn,
    .qdev.props = (Property[]) {
        DEFINE_PROP_UINT32("irq", SysPortState, irq, 15),
        DEFINE_PROP_END_OF_LIST (),
    },
};

static void pc98_sys_register(void)
{
    isa_qdev_register(&pc98_sys_info);
}
device_init(pc98_sys_register)
