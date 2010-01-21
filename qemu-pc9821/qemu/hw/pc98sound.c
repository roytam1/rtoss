/*
 * QEMU Proxy for YM2608 emulation by MAME team
 * QEMU NEC PC-9821 Sound (PC-9801-86)
 *
 * Copyright (c) 2004-2005 Vassili Karpov (malc)
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
#include "audio/audio.h"
#include "pc.h"
#include "isa.h"
#include "qemu-timer.h"
#include "ym2608intf.h"

#define SOUND_SAMPLE_RATE 44100
#define SHIFT 1

#define PCM86_SAMPLE_RATE 44100
#define PCM86_FIFO_SIZE 0x8000

#define YM2608_CLOCK 3993552
#define YM2608_CLOCK_2 (YM2608_CLOCK << 1)
#define YM2608_PCM_SIZE 0x40000

typedef struct {
    QEMUSoundCard card;
    int enabled;
    int active;
    int16_t *mix_buf;
    int16_t *ym2608_buf[2];
    int16_t *ay8910_buf[3];
    int16_t *pcm86_buf;
    uint8_t mix_vol[8];
    uint8_t ym2608_mask;
    uint8_t ym2608_ticking[2];
    double ym2608_err[2];
    uint8_t pcm86_fifo[PCM86_FIFO_SIZE];
    int pcm86_fifo_count;
    int pcm86_fifo_write, pcm86_fifo_read;
    int16_t pcm86_data;
    int pcm86_ptr;
    uint8_t pcm86_fifo_ctrl;
    uint8_t pcm86_dac_ctrl;
    uint8_t pcm86_irq_timing;
    uint8_t pcm86_mute;
    int pcm86_lr, pcm86_lr_half;
    uint8_t irq_ym2608, irq_pcm86;
    uint8_t irq_status;
    SWVoiceOut *voice;
    int left, pos, samples;
    int pre_samples;
    QEMUAudioTimeStamp ats;
    QEMUTimer *qemu_timer[2];
    int64_t update_clock;
    qemu_irq irq;
} sound_t;

typedef struct sound_isabus_t {
    ISADevice dev;
    uint32_t isairq;
    sound_t state;
} sound_isabus_t;

static sound_isabus_t *glob_isa;
static uint8_t ym2608_pcm_buf[YM2608_PCM_SIZE];

static void update_irq(void *opaque);
static void sound_mix(int samples);

/* ym2608 */

static void ym2608_stop_timer(int c)
{
    sound_t *s = &glob_isa->state;

    s->ym2608_ticking[c] = 0;
    YM2608TimerOver(0, c);
}

static void qemu_timer_a_handler(void *opaque)
{
    ym2608_stop_timer(0);
}

static void qemu_timer_b_handler(void *opaque)
{
    ym2608_stop_timer(1);
}

static void ym2608_timer_handler(int c, int clock)
{
    sound_t *s = &glob_isa->state;

    if (clock == 0) {
        if (s->ym2608_ticking[c]) {
            qemu_del_timer(s->qemu_timer[c]);
            s->ym2608_ticking[c] = 0;
        }
    } else {
        if (!s->ym2608_ticking[c]) {
            double ms = 1000. * clock / (double)YM2608_CLOCK_2 + s->ym2608_err[c];
            int ms_int = (int)ms;
            s->ym2608_err[c] = ms - ms_int;
            if (ms_int > 0) {
                qemu_mod_timer(s->qemu_timer[c], qemu_get_clock(rt_clock) + ms_int);
                s->ym2608_ticking[c] = 1;
            } else {
                ym2608_stop_timer(c);
            }
        }
    }
}

static void ym2608_irq_handler(int irq)
{
    sound_t *s = &glob_isa->state;

    s->irq_ym2608 = (irq != 0);
    update_irq(s);
}

static void ym2608_update_handler(void)
{
    sound_t *s = &glob_isa->state;

    if (s->update_clock == -1) {
        s->update_clock = qemu_get_clock(rt_clock);;
    } else {
        int64_t current = qemu_get_clock(rt_clock);
        int64_t period = current - s->update_clock;
        int64_t samples = muldiv64(SOUND_SAMPLE_RATE, period, 1000);
        if (samples) {
            sound_mix(samples);
            s->update_clock = current;
        }
    }
}

static uint8_t ym2608_port_a_read(uint32_t offset)
{
    return 0xff; /* INT5 */
}

static void ym2608_port_b_write(uint32_t offset, uint8 data)
{
}

static void ym2608_ioport_write(void *opaque, uint32_t nport, uint32_t val)
{
    sound_t *s = opaque;

    if (nport == 0xa460) {
        s->ym2608_mask = val;
    } else {
        s->active = 1;
        AUD_set_active_out(s->voice, 1);

        YM2608Write(0, (nport >> 1) & 0x03, val);
    }
}

static uint32_t ym2608_ioport_read(void *opaque, uint32_t nport)
{
    if (nport == 0xa460) {
        return 0x41; /* PC-9801-86 */
    } else {
        return YM2608Read(0, (nport >> 1) & 0x03);
    }
}

/* pcm */

static void pcm86_fifo_write(void *opaque, uint8_t val)
{
    sound_t *s = opaque;

    if (s->pcm86_fifo_count < PCM86_FIFO_SIZE) {
        s->pcm86_fifo[s->pcm86_fifo_write++] = val;
        s->pcm86_fifo_write &= (PCM86_FIFO_SIZE - 1);
        s->pcm86_fifo_count++;
    }
}

static uint8_t pcm86_fifo_read(void *opaque)
{
    sound_t *s = opaque;
    uint8_t val = 0;

    if (s->pcm86_fifo_count > 0) {
        val = s->pcm86_fifo[s->pcm86_fifo_read++];
        s->pcm86_fifo_read &= (PCM86_FIFO_SIZE - 1);
        s->pcm86_fifo_count--;
    }
    return val;
}

static int16_t pcm86_fifo_read8(void *opaque)
{
    sound_t *s = opaque;

    return (int16_t)(pcm86_fifo_read(s) << 8);
}

static int16_t pcm86_fifo_read16(void *opaque)
{
    sound_t *s = opaque;
    uint16_t val = 0;

    if (s->pcm86_fifo_count > 1) {
        val = pcm86_fifo_read(s) << 8;
        val |= pcm86_fifo_read(s);
    }
    return (int16_t)val;
}

/* 0 = 44.10kHz    PCM86_SAMPLE_RATE * 96 / 96
   1 = 33.08kHz    PCM86_SAMPLE_RATE * 72 / 96
   2 = 22.05kHz    PCM86_SAMPLE_RATE * 48 / 96
   3 = 16.54kHz    PCM86_SAMPLE_RATE * 36 / 96
   4 = 11.03kHz    PCM86_SAMPLE_RATE * 24 / 96
   5 =  8.27kHz    PCM86_SAMPLE_RATE * 16 / 96
   6 =  5.52kHz    PCM86_SAMPLE_RATE * 12 / 96
   7 =  4.13kHz    PCM86_SAMPLE_RATE *  9 / 96
*/
static const int pcm86_freq_table[8] = {96, 72, 48, 36, 24, 16, 12, 9};

#define PCM86_DISABLE(s) ((s->pcm86_dac_ctrl & 0x30) == 0x00)
#define PCM86_MONO_R(s) ((s->pcm86_dac_ctrl & 0x30) == 0x10)
#define PCM86_MONO_L(s) ((s->pcm86_dac_ctrl & 0x30) == 0x20)
#define PCM86_STEREO(s) ((s->pcm86_dac_ctrl & 0x30) == 0x30)
#define PCM86_8BIT(s) ((s->pcm86_dac_ctrl & 0x40) != 0x00)

#define PCM86_FIFO_ENB(s) ((s->pcm86_fifo_ctrl & 0x80) != 0x00)
#define PCM86_FIFO_PLAY(s) ((s->pcm86_fifo_ctrl & 0x40) == 0x00)
#define PCM86_FIFO_REC(s) ((s->pcm86_fifo_ctrl & 0x40) != 0x00)
#define PCM86_FIFO_EI(s) ((s->pcm86_fifo_ctrl & 0x20) != 0x00)

static void pcm86_mix(void *opaque, int16_t *buffer, int samples)
{
    sound_t *s = opaque;
    int i;
    int add = pcm86_freq_table[s->pcm86_fifo_ctrl & 0x07];

    if (PCM86_DISABLE(s) || !(PCM86_FIFO_ENB(s) && PCM86_FIFO_PLAY(s))) {
        memset(buffer, 0, sizeof(int16_t) * samples);
        return;
    }
    for (i = 0; i < samples; i++) {
        if ((s->pcm86_ptr += add) >= 96) {
            int16_t l, r;
            if (PCM86_8BIT(s)) {
                l = pcm86_fifo_read8(s);
                r = pcm86_fifo_read8(s);
            } else {
                l = pcm86_fifo_read16(s);
                r = pcm86_fifo_read16(s);
            }
            if (s->pcm86_mute) {
                s->pcm86_data = 0;
            } else if (PCM86_MONO_L(s)) {
                s->pcm86_data = l;
            } else if (PCM86_MONO_R(s)) {
                s->pcm86_data = r;
            } else {
                s->pcm86_data = (l + r) >> 1; /* XXX: stereo -> mono */
            }
            s->pcm86_ptr -= 96;
            /* irq */
            s->irq_pcm86 = (s->pcm86_fifo_count <= 128 * s->pcm86_irq_timing);
            if (PCM86_FIFO_EI(s)) {
                update_irq(s);
            }
        }
        buffer[i] = s->pcm86_data;
    }
}

static void pcm86_ioport_write(void *opaque, uint32_t nport, uint32_t val)
{
    sound_t *s = opaque;

    switch (nport) {
    case 0xa466:
        s->mix_vol[val >> 5] = val & 0x0f;
        break;
    case 0xa468:
        s->pcm86_fifo_ctrl = val;
        if (val & 0x08) {
            s->pcm86_fifo_count = 0;
            s->pcm86_fifo_write = s->pcm86_fifo_read = 0;
        }
        if (!(val & 0x10) && s->irq_pcm86) {
            s->irq_pcm86 = 0;
            update_irq(s);
        }
        /* ticks_per_sample */
        s->pcm86_lr = PCM86_SAMPLE_RATE * pcm86_freq_table[val & 0x07] / 96;
        s->pcm86_lr = get_ticks_per_sec() / s->pcm86_lr;
        s->pcm86_lr_half = s->pcm86_lr >> 1;
        break;
    case 0xa46a:
        if (s->pcm86_fifo_ctrl & 0x20) {
            s->pcm86_irq_timing = val;
        } else {
            s->pcm86_dac_ctrl = val;
        }
        break;
    case 0xa46c:
        pcm86_fifo_write(s, val);
        break;
    case 0xa66e:
        s->pcm86_mute = val & 1;
    }
}

static uint32_t pcm86_ioport_read(void *opaque, uint32_t nport)
{
    sound_t *s = opaque;
    uint32_t val = 0;

    switch (nport) {
    case 0xa466:
        if (s->pcm86_fifo_count == PCM86_FIFO_SIZE) {
            val |= 0x80;
        }
        if (s->pcm86_fifo_count == 0) {
            val |= 0x40;
        }
        if (s->pcm86_lr) {
            if ((qemu_get_clock(vm_clock) % s->pcm86_lr) < s->pcm86_lr_half) {
                val |= 0x01;
            }
        }
        return val;
    case 0xa468:
        if (s->irq_pcm86) {
            return s->pcm86_fifo_ctrl | 0x10;
        } else {
            return s->pcm86_fifo_ctrl & (~0x10);
        }
    case 0xa46a:
        return s->pcm86_dac_ctrl;
    case 0xa46c:
        return pcm86_fifo_read(s);
    case 0xa66e:
        return s->pcm86_mute;
    }
    return 0xff;
}

/* irq */

static void update_irq(void *opaque)
{
    sound_t *s = opaque;
    uint8_t irq_new = 0;

    if (s->irq_ym2608 || (s->irq_pcm86 && PCM86_FIFO_EI(s))) {
        irq_new = 1;
    }
    if (s->irq_status != irq_new) {
        s->irq_status = irq_new;
        qemu_set_irq(s->irq, s->irq_status);
    }
}

/* sound */

static void sound_mix(int samples)
{
    sound_t *s = &glob_isa->state;
    int i, limit = s->samples - (s->pos + s->pre_samples);

    if (samples > limit) {
        samples = limit;
    }
    if (samples > 0) {
        YM2608UpdateOne(0, s->ym2608_buf, samples);
        AY8910Update(0, s->ay8910_buf, samples);
        pcm86_mix(s, s->pcm86_buf, samples);
        for (i = 0; i < samples; i++) {
            /* XXX: mixer volume */
            int32_t data = 0;
            data += s->ym2608_buf[0][i] * 3;// * 2;
            data += s->ym2608_buf[1][i] * 3;// * 2;
            data += s->ay8910_buf[0][i] * 2;
            data += s->ay8910_buf[1][i] * 2;
            data += s->ay8910_buf[2][i] * 2;
            data += s->pcm86_buf[i] * 12;
            data >>= 4;
            if (data > 32767) {
                data = 32767;
            } else if (data < -32768) {
                data = -32768;
            }
            s->mix_buf[s->pos + s->pre_samples + i] = data;
        }
        s->pre_samples += samples;
    }
}

static int write_audio(sound_t *s, int samples)
{
    int net = 0;
    int pos = s->pos;

    while (samples) {
        int nbytes, wbytes, wsampl;

        nbytes = samples << SHIFT;
        wbytes = AUD_write(s->voice, s->mix_buf + (pos << (SHIFT - 1)), nbytes);

        if (wbytes) {
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
    sound_t *s = opaque;
    int samples, to_play, written;

    samples = free >> SHIFT;
    if (!(s->active && s->enabled) || !samples) {
        return;
    }

    to_play = audio_MIN(s->left, samples);
    while (to_play) {
        written = write_audio(s, to_play);

        if (written) {
            s->left -= written;
            samples -= written;
            to_play -= written;
            s->pos = (s->pos + written) % s->samples;
        } else {
            return;
        }
    }

    samples = audio_MIN(samples, s->samples - s->pos);
    if (!samples) {
        return;
    }

    if (s->pre_samples > samples) {
        s->pre_samples -= samples;
    } else {
        sound_mix(samples - s->pre_samples);
        s->pre_samples = 0;
        s->update_clock = qemu_get_clock(rt_clock);
    }

    while (samples) {
        written = write_audio(s, samples);

        if (written) {
            samples -= written;
            s->pos = (s->pos + written) % s->samples;
        } else {
            s->left = samples;
            return;
        }
    }
}

/* initialize */

static void pc98_sound_save(QEMUFile* f, void *opaque)
{
    sound_t *s = opaque;

    YM2608_sh_save_state(f);

    qemu_put_buffer(f, s->mix_vol, sizeof(s->mix_vol));
    qemu_put_8s(f, &s->ym2608_mask);
    qemu_put_buffer(f, s->ym2608_ticking, sizeof(s->ym2608_ticking));
    qemu_put_buffer(f, s->pcm86_fifo, sizeof(s->pcm86_fifo));
    qemu_put_be32s(f, &s->pcm86_fifo_count);
    qemu_put_be32s(f, &s->pcm86_fifo_write);
    qemu_put_be32s(f, &s->pcm86_fifo_read);
    qemu_put_be16s(f, &s->pcm86_data);
    qemu_put_8s(f, &s->pcm86_fifo_ctrl);
    qemu_put_8s(f, &s->pcm86_dac_ctrl);
    qemu_put_8s(f, &s->pcm86_irq_timing);
    qemu_put_8s(f, &s->pcm86_mute);
    qemu_put_be32s(f, &s->pcm86_lr);
    qemu_put_be32s(f, &s->pcm86_lr_half);
    qemu_put_8s(f, &s->irq_ym2608);
    qemu_put_8s(f, &s->irq_pcm86);
    qemu_put_8s(f, &s->irq_status);
}

static int pc98_sound_load(QEMUFile* f, void *opaque, int version_id)
{
    sound_t *s = opaque;

    YM2608_sh_load_state(f);
    YM2608_sh_postload();

    qemu_get_buffer(f, s->mix_vol, sizeof(s->mix_vol));
    qemu_get_8s(f, &s->ym2608_mask);
    qemu_get_buffer(f, s->ym2608_ticking, sizeof(s->ym2608_ticking));
    qemu_get_buffer(f, s->pcm86_fifo, sizeof(s->pcm86_fifo));
    qemu_get_be32s(f, &s->pcm86_fifo_count);
    qemu_get_be32s(f, &s->pcm86_fifo_write);
    qemu_get_be32s(f, &s->pcm86_fifo_read);
    qemu_get_be16s(f, &s->pcm86_data);
    qemu_get_8s(f, &s->pcm86_fifo_ctrl);
    qemu_get_8s(f, &s->pcm86_dac_ctrl);
    qemu_get_8s(f, &s->pcm86_irq_timing);
    qemu_get_8s(f, &s->pcm86_mute);
    qemu_get_be32s(f, &s->pcm86_lr);
    qemu_get_be32s(f, &s->pcm86_lr_half);
    qemu_get_8s(f, &s->irq_ym2608);
    qemu_get_8s(f, &s->irq_pcm86);
    qemu_get_8s(f, &s->irq_status);

    s->ym2608_err[0] = s->ym2608_err[1] = 0.0;
    s->pcm86_ptr = 0;
    s->left = s->pos = s->samples = 0;
    s->pre_samples = 0;
    s->update_clock = -1;

    return 0;
}

static struct YM2608interface ym2608_interface =
{
    1,
    YM2608_CLOCK,
    { ym2608_port_a_read },
    { 0 }, // port-b read handler
    { 0 }, // port-a write handler
    { ym2608_port_b_write },
    { ym2608_irq_handler },
    { ym2608_timer_handler },
    { ym2608_update_handler },
    { ym2608_pcm_buf },
    { YM2608_PCM_SIZE },
};

static struct MachineSound machine_sound =
{
    1,
    &ym2608_interface,
    "YM2608",
};

static void sound_release(void *opaque)
{
    sound_t *s = opaque;

    YM2608_sh_stop();

    if (s->ym2608_buf[0]) {
        qemu_free(s->ym2608_buf[0]);
    }
    if (s->ym2608_buf[1]) {
        qemu_free(s->ym2608_buf[1]);
    }
    if (s->ay8910_buf[0]) {
        qemu_free(s->ay8910_buf[0]);
    }
    if (s->ay8910_buf[1]) {
        qemu_free(s->ay8910_buf[1]);
    }
    if (s->ay8910_buf[2]) {
        qemu_free(s->ay8910_buf[2]);
    }
    if (s->mix_buf) {
        qemu_free(s->mix_buf);
    }

    s->active = 0;
    s->enabled = 0;
    AUD_remove_card(&s->card);
}

static int pc98_sound_initfn(ISADevice *dev)
{
    static int pcm86_nport[5] = { 0xa466, 0xa468, 0xa46a, 0xa46c, 0xa66e };
    sound_isabus_t *isa = DO_UPCAST(sound_isabus_t, dev, dev);
    sound_t *s = &isa->state;
    struct audsettings as;
    int i;

    glob_isa = isa;

    YM2608_sh_start(&machine_sound, SOUND_SAMPLE_RATE);
    s->enabled = 1;

    as.freq = SOUND_SAMPLE_RATE;
    as.nchannels = SHIFT;
    as.fmt = AUD_FMT_S16;
    as.endianness = AUDIO_HOST_ENDIANNESS;

    AUD_register_card("ym2608", &s->card);

    s->voice = AUD_open_out(&s->card, s->voice,
                            "ym2608", s, sound_callback, &as);
    if (!s->voice) {
        sound_release(s);
        return -1;
    }

    s->samples = AUD_get_buffer_size_out(s->voice) >> SHIFT;
    s->mix_buf = qemu_mallocz(s->samples << SHIFT);
    s->ym2608_buf[0] = qemu_mallocz(s->samples << SHIFT);
    s->ym2608_buf[1] = qemu_mallocz(s->samples << SHIFT);
    s->ay8910_buf[0] = qemu_mallocz(s->samples << SHIFT);
    s->ay8910_buf[1] = qemu_mallocz(s->samples << SHIFT);
    s->ay8910_buf[2] = qemu_mallocz(s->samples << SHIFT);
    s->pcm86_buf = qemu_mallocz(s->samples << SHIFT);

    memset(ym2608_pcm_buf, 0, sizeof(ym2608_pcm_buf));

    for (i = 0; i < 4; i++) {
        register_ioport_read(0x188 + (i << 1), 1, 1, ym2608_ioport_read, s);
        register_ioport_write(0x188 + (i << 1), 1, 1, ym2608_ioport_write, s);
    }
    register_ioport_read(0xa460, 1, 1, ym2608_ioport_read, s);
    register_ioport_write(0xa460, 1, 1, ym2608_ioport_write, s);

    for (i = 0; i < 5; i++) {
        register_ioport_read(pcm86_nport[i], 1, 1, pcm86_ioport_read, s);
        register_ioport_write(pcm86_nport[i], 1, 1, pcm86_ioport_write, s);
    }

    isa_init_irq(dev, &s->irq, isa->isairq);

    s->qemu_timer[0] = qemu_new_timer(rt_clock, qemu_timer_a_handler, s);
    s->qemu_timer[1] = qemu_new_timer(rt_clock, qemu_timer_b_handler, s);

    s->update_clock = -1;

    register_savevm("pc98-sound", 0, 1, pc98_sound_save, pc98_sound_load, s);

    return 0;
}

int pc98_sound_init(qemu_irq *pic)
{
    ISADevice *dev;

    dev = isa_create("pc98-sound");
    qdev_init_nofail(&dev->qdev);

    return 0;
}

static ISADeviceInfo pc98_sound_info = {
    .qdev.name  = "pc98-sound",
    .qdev.size  = sizeof(sound_isabus_t),
    .init       = pc98_sound_initfn,
    .qdev.props = (Property[]) {
        DEFINE_PROP_UINT32("irq", sound_isabus_t, isairq, 12),
        DEFINE_PROP_END_OF_LIST(),
    },
};

static void pc98_sound_register_devices(void)
{
    isa_qdev_register(&pc98_sound_info);
}

device_init(pc98_sound_register_devices)
