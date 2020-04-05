/*
 * QEMU wrapper for YM2608 sound generator 'fmgen' by cisc
 *
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

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "fmopna.h"
#include "fmgen/opna.h"

#define BUFLEN 0x20000

#ifndef Sample
typedef FM_SAMPLETYPE Sample;
#endif

struct OPNAState {
    FM::OPNA *opna;

    int64_t accum, diff;

    Sample *mixbuf;
    Sample *fifo_l;
    Sample *fifo_r;
    int fifo_count;
    int fifo_ptr_r;
    int fifo_ptr_w;
};

typedef struct OPNAState OPNAState;

static inline void write_fifo_buffer(OPNAState *s, Sample l, Sample r)
{
    if(s->fifo_count < BUFLEN) {
        s->fifo_l[s->fifo_ptr_w] = l;
        s->fifo_r[s->fifo_ptr_w] = r;
        s->fifo_ptr_w = (s->fifo_ptr_w + 1) & (BUFLEN - 1);
        s->fifo_count++;
    }
}

static inline void read_fifo_buffer(OPNAState *s, Sample *l, Sample *r)
{
    if(s->fifo_count) {
        *l = s->fifo_l[s->fifo_ptr_r];
        *r = s->fifo_r[s->fifo_ptr_r];
        s->fifo_ptr_r = (s->fifo_ptr_r + 1) & (BUFLEN - 1);
        s->fifo_count--;
    }
}

static inline uint16_t limit_sample(Sample sample)
{
    uint16_t highlow = (uint16_t)(sample & 0x0000ffff);

    if((sample > 0) && (highlow >= 0x8000)) {
        return 0x7fff;
    }
    if((sample < 0) && (highlow < 0x8000)) {
        return 0x8000;
    }
    return highlow;
}

void *opna_init(int frequency, int sample_rate, const char* path)
{
    OPNAState *s = (OPNAState *)malloc(sizeof(OPNAState));

    s->opna = new FM::OPNA;
    s->mixbuf = (Sample *)malloc(sizeof(Sample) * BUFLEN * 2);
    s->fifo_l = (Sample *)malloc(sizeof(Sample) * BUFLEN);
    s->fifo_r = (Sample *)malloc(sizeof(Sample) * BUFLEN);

    s->opna->Init(frequency, sample_rate, false, path);
    s->opna->Reset();
    s->opna->SetReg(0x27, 0);

    s->accum = 0;
    s->diff = (int)(1000000. * 1024. * 1024. / sample_rate + 0.5);

    s->fifo_count = s->fifo_ptr_r = s->fifo_ptr_w = 0;

    return s;
}

void opna_reset(void *opaque)
{
    OPNAState *s = (OPNAState *)opaque;

    s->opna->Reset();
}

void opna_update_buffer(void *opaque, int16_t *buffer, int samples, int stereo)
{
    OPNAState *s = (OPNAState *)opaque;
    Sample l, r;

    if(samples > s->fifo_count) {
        // fill buffer
        int count = samples - s->fifo_count;
        if(count > BUFLEN) {
            count = BUFLEN;
        }
        memset(s->mixbuf, 0, sizeof(Sample) * 2 * count);
        s->opna->Mix(s->mixbuf, count);
        for(int i = 0, j = 0; i < count; i++, j += 2) {
            write_fifo_buffer(s, s->mixbuf[j], s->mixbuf[j + 1]);
        }
    }
    if(stereo) {
        for(int i = 0, j = 0; i < samples; i++, j += 2) {
            read_fifo_buffer(s, &l, &r);
            buffer[j    ] = limit_sample(l);
            buffer[j + 1] = limit_sample(r);
        }
    } else {
        for(int i = 0; i < samples; i++) {
            read_fifo_buffer(s, &l, &r);
            buffer[i] = limit_sample(l + r);
        }
    }
}

void opna_advance_time(void *opaque, int usec)
{
    OPNAState *s = (OPNAState *)opaque;
    int samples = 0;

    // mix buffer
    s->accum += usec << 20;
    if(s->accum > s->diff) {
        samples = (int)(s->accum / s->diff);
    }
    s->accum -= samples * s->diff;

    if(samples + s->fifo_count > BUFLEN) {
        samples = BUFLEN - s->fifo_count;
    }
    if(samples > 0) {
        memset(s->mixbuf, 0, sizeof(Sample) * 2 * samples);
        s->opna->Mix(s->mixbuf, samples);
        for(int i = 0, j = 0; i < samples; i++, j += 2) {
            write_fifo_buffer(s, s->mixbuf[j], s->mixbuf[j + 1]);
        }
    }

    // run timer
    if(usec) {
        s->opna->Count(usec);
    }
}

int opna_timer_active(void *opaque)
{
    OPNAState *s = (OPNAState *)opaque;

    return s->opna->TimerActive();
}

int opna_read_interrupt(void *opaque)
{
    OPNAState *s = (OPNAState *)opaque;

    return s->opna->ReadInterrupt();
}

void opna_write_reg(void *opaque, int regnum, int data)
{
    OPNAState *s = (OPNAState *)opaque;

    s->opna->SetReg(regnum, data);
}

int opna_read_reg(void *opaque, int regnum)
{
    OPNAState *s = (OPNAState *)opaque;

    return s->opna->GetReg(regnum);
}

int opna_read_status(void *opaque)
{
    OPNAState *s = (OPNAState *)opaque;

    return s->opna->ReadStatus();
}

void opna_write_reg_ex(void *opaque, int regnum, int data)
{
    OPNAState *s = (OPNAState *)opaque;

    if(regnum <= 0x10) {
        s->opna->SetReg(regnum | 0x100, data);
    } else {
        s->opna->SetReg(regnum, data);
    }
}

int opna_read_reg_ex(void *opaque, int regnum)
{
    OPNAState *s = (OPNAState *)opaque;

    if(regnum <= 0x10) {
        return s->opna->GetReg(regnum | 0x100);
    } else {
        return s->opna->GetReg(regnum);
    }
}

int opna_read_status_ex(void *opaque)
{
    OPNAState *s = (OPNAState *)opaque;

    return s->opna->ReadStatusEx();
}
