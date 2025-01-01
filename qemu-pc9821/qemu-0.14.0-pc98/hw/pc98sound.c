/*
 * QEMU Proxy for YM2608 sound generator 'fmgen' by cisc
 *
 * Copyright (c) 2004-2005 Vassili Karpov (malc)
 * Copyright (c) 2011      TAKEDA, toshiya
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
#include "audiodev.h"
#include "audio/audio.h"
#include "isa.h"
#include "qdev.h"
#include "pc.h"
#include "sysemu.h"
#include "qemu-timer.h"
#include "fmopna.h"

/* 1=mono, 2=stereo */
#define SHIFT 2
#define SAMPLE_RATE 44100

/* period to update timer */
#define TIMER_PERIOD 1

struct SoundState {
    /* isa */
    ISADevice dev;
    qemu_irq pic;
    uint32_t irq;

    /* sound */
    QEMUSoundCard card;
    SWVoiceOut *voice;
    QEMUTimer *timer;
    int16_t *mixbuf;
    int enabled;
    int active;
    int left, pos, samples;

    /* opna */
    void *opna;
    int regnum;
    int regnum_ex;
    int interrupt;
    int portb;
    int mask;

    int64_t time, accum;
};

typedef struct SoundState SoundState;

/* audio */

static int write_audio(SoundState *s, int samples)
{
    int net = 0;
    int pos = s->pos;

    while(samples) {
        int nbytes, wbytes, wsampl;

        nbytes = samples << SHIFT;
        wbytes = AUD_write(s->voice, s->mixbuf + (pos << (SHIFT - 1)), nbytes);

        if(wbytes) {
            wsampl = wbytes >> SHIFT;

            samples -= wsampl;
            pos = (pos + wsampl) % s->samples;

            net += wsampl;
        } else {
            break;
        }
    }

    return net;
}

static void sound_callback(void *opaque, int free)
{
    SoundState *s = opaque;
    int samples, net = 0, to_play, written;

    samples = free >> SHIFT;
    if(!(s->active && s->enabled) || !samples) {
        return;
    }

    to_play = audio_MIN(s->left, samples);
    while(to_play) {
        written = write_audio(s, to_play);

        if(written) {
            s->left -= written;
            samples -= written;
            to_play -= written;
            s->pos = (s->pos + written) % s->samples;
        } else {
            return;
        }
    }

    samples = audio_MIN(samples, s->samples - s->pos);
    if(!samples) {
        return;
    }

    opna_update_buffer(s->opna, s->mixbuf + (s->pos << (SHIFT - 1)), samples, (SHIFT == 2));

    while(samples) {
        written = write_audio(s, samples);

        if(written) {
            net += written;
            samples -= written;
            s->pos = (s->pos + written) % s->samples;
        } else {
            s->left = samples;
            return;
        }
    }
}

static void update_interrupt(SoundState *s)
{
    int interrupt = opna_read_interrupt(s->opna);

    if(!s->interrupt && interrupt) {
        qemu_irq_raise(s->pic);
    } else if(s->interrupt && !interrupt) {
        qemu_irq_lower(s->pic);
    }
    s->interrupt = interrupt;
}

static void advance_time(SoundState *s)
{
    int64_t time, delta;
    int usec;

    if(s->active && s->enabled) {
        time = qemu_get_clock_ns(rt_clock);
        if(!s->time) {
            s->time = time;
            return;
        }
        delta = time - s->time;
        s->time = time;

        s->accum += delta;
        usec = (int)(s->accum / 1000);
        s->accum -= usec * 1000;

        if(usec) {
            opna_advance_time(s->opna, usec);
            update_interrupt(s);
        }
    }
}

static void timer_handler(void *opaque)
{
    SoundState *s = opaque;

    /* run fmgen */
    advance_time(s);

    /* set next timer */
    qemu_mod_timer(s->timer, qemu_get_clock(rt_clock) + TIMER_PERIOD);
}

/* i/o ports */

static void sound_regnum_write(void *opaque, uint32_t addr, uint32_t value)
{
    SoundState *s = opaque;

    s->regnum = value;

    /* write dummy data for prescaler */
    if(s->regnum >= 0x2d && s->regnum <= 0x2f) {
        opna_write_reg(s->opna, s->regnum, 0);
        update_interrupt(s);
    }
}

static uint32_t sound_status_read(void *opaque, uint32_t addr)
{
    SoundState *s = opaque;

    /* run fmgen to update status */
    if(opna_timer_active(s->opna)) {
        advance_time(s);
    }
    return opna_read_status(s->opna);
}

static void sound_reg_write(void *opaque, uint32_t addr, uint32_t value)
{
    SoundState *s = opaque;

    /* don't write again for prescaler */
    if(!(s->regnum >= 0x2d && s->regnum <= 0x2f)) {
        if(s->regnum == 0x0f) {
            s->portb = value;
        }
        opna_write_reg(s->opna, s->regnum, value);
        update_interrupt(s);

        /* activate audio */
        if(s->enabled) {
            if(!s->active) {
                /* start timer */
                qemu_mod_timer(s->timer, qemu_get_clock(rt_clock) + TIMER_PERIOD);
                s->active = 1;
            }
            AUD_set_active_out(s->voice, 1);
        }
    }
}

static uint32_t sound_reg_read(void *opaque, uint32_t addr)
{
    SoundState *s = opaque;

    if(s->regnum == 0x0e) {
        uint32_t value = 0x3f;

        /* update joystick */
        if(s->portb & 0x80) {
            if(s->portb & 0x40) {
                /* joystick #2 */
            } else {
                /* joystick #1 */
            }
        }

        /* irq number */
        switch(s->irq) {
        case 13: value |= 0x40; break; /* IRQ13 */
        case 10: value |= 0x80; break; /* IRQ10 */
        case 12: value |= 0xc0; break; /* IRQ12 */
        }
        return value;
    } else if(s->regnum == 0xff) {
        return 0x01; /* YM2608 */
    } else {
        return opna_read_reg(s->opna, s->regnum);
    }
}

static void sound_regnum_ex_write(void *opaque, uint32_t addr, uint32_t value)
{
    SoundState *s = opaque;

    if(!(s->mask & 1)) {
        return;
    }
    s->regnum_ex = value;
}

static uint32_t sound_status_ex_read(void *opaque, uint32_t addr)
{
    SoundState *s = opaque;

    if(!(s->mask & 1)) {
        return 0xff;
    }

    /* run fmgen to update status */
    if(opna_timer_active(s->opna)) {
        advance_time(s);
    }
    return opna_read_status_ex(s->opna);
}

static void sound_reg_ex_write(void *opaque, uint32_t addr, uint32_t value)
{
    SoundState *s = opaque;

    if(!(s->mask & 1)) {
        return;
    }
    opna_write_reg_ex(s->opna, s->regnum_ex, value);
    update_interrupt(s);

    /* activate audio */
    if(s->enabled) {
        if(!s->active) {
            /* start timer */
            qemu_mod_timer(s->timer, qemu_get_clock(rt_clock) + TIMER_PERIOD);
            s->active = 1;
        }
        AUD_set_active_out(s->voice, 1);
    }
}

static uint32_t sound_reg_ex_read(void *opaque, uint32_t addr)
{
    SoundState *s = opaque;

    if(!(s->mask & 1)) {
        return 0xff;
    }
    return opna_read_reg_ex(s->opna, s->regnum_ex);
}

static void sound_mask_write(void *opaque, uint32_t addr, uint32_t value)
{
    SoundState *s = opaque;

    s->mask = value;
}

static uint32_t sound_id_read(void *opaque, uint32_t addr)
{
    SoundState *s = opaque;

    return 0x80 | (s->mask & 3); /* 98MULTi CanBe */
}

/* interface */

static const VMStateDescription vmstate_sound = {
    .name = "pc98-sound",
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField []) {
        VMSTATE_INT32(regnum, SoundState),
        VMSTATE_INT32(regnum_ex, SoundState),
        VMSTATE_INT32(portb, SoundState),
        VMSTATE_INT32(mask, SoundState),
        VMSTATE_END_OF_LIST()
    }
};

static int pc98_sound_initfn(ISADevice *dev)
{
    SoundState *s = DO_UPCAST(SoundState, dev, dev);
    struct audsettings as;
    char* temp_dir;

    isa_init_irq(dev, &s->pic, s->irq);

    s->enabled = 0;
    s->active = 0;
    s->left = s->pos = 0;

    as.freq = SAMPLE_RATE;
    as.nchannels = SHIFT;
    as.fmt = AUD_FMT_S16;
    as.endianness = AUDIO_HOST_ENDIANNESS;

    AUD_register_card("pc98sound", &s->card);

    s->voice = AUD_open_out(&s->card, s->voice, "pc98sound", s,
                            sound_callback, &as);
    if(s->voice) {
        s->samples = AUD_get_buffer_size_out(s->voice) >> SHIFT;
        s->mixbuf = qemu_mallocz(s->samples << SHIFT);
        s->timer = qemu_new_timer(rt_clock, timer_handler, s);
        s->enabled = 1;
    } else {
        AUD_remove_card(&s->card);
    }

    /* fmgen requires '/' at the end of wave file dir */
    temp_dir = qemu_find_file(QEMU_FILE_TYPE_BIOS, "");
    s->opna = opna_init(7987200, SAMPLE_RATE, temp_dir);

    register_ioport_write(0x188, 1, 1, sound_regnum_write, s);
    register_ioport_read(0x188, 1, 1, sound_status_read, s);
    isa_init_ioport(dev, 0x188);
    register_ioport_write(0x18a, 1, 1, sound_reg_write, s);
    register_ioport_read(0x18a, 1, 1, sound_reg_read, s);
    isa_init_ioport(dev, 0x18a);

    register_ioport_write(0x18c, 1, 1, sound_regnum_ex_write, s);
    register_ioport_read(0x18c, 1, 1, sound_status_ex_read, s);
    isa_init_ioport(dev, 0x18c);
    register_ioport_write(0x18e, 1, 1, sound_reg_ex_write, s);
    register_ioport_read(0x18e, 1, 1, sound_reg_ex_read, s);
    isa_init_ioport(dev, 0x18e);

    register_ioport_write(0xa460, 1, 1, sound_mask_write, s);
    register_ioport_read(0xa460, 1, 1, sound_id_read, s);
    isa_init_ioport(dev, 0xa460);

    s->regnum = s->regnum_ex = 0;
    s->portb = 0;
    s->mask = 0;
    s->interrupt = 0;

    s->time = s->accum = 0;

    return 0;
}

void pc98_sound_init(void)
{
    isa_create_simple("pc98-sound");
}

static ISADeviceInfo pc98_sound_info = {
    .qdev.name  = "pc98-sound",
    .qdev.desc  = "NEC PC-9821 Sound",
    .qdev.size  = sizeof(SoundState),
    .qdev.vmsd  = &vmstate_sound,
    .init       = pc98_sound_initfn,
    .qdev.props = (Property[]) {
        DEFINE_PROP_UINT32("irq", SoundState, irq, 3/*12*/),
        DEFINE_PROP_END_OF_LIST (),
    },
};

static void pc98_sound_register(void)
{
    isa_qdev_register(&pc98_sound_info);
}
device_init(pc98_sound_register)
