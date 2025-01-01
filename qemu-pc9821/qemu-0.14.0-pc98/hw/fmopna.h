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

#ifndef HW_FMOPNA_H
#define HW_FMOPNA_H

//#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void *opna_init(int frequency, int sample_rate, const char* path);
void opna_reset(void *opaque);
void opna_update_buffer(void *opaque, int16_t *buffer, int samples, bool stereo);
void opna_advance_time(void *opaque, int usec);
bool opna_timer_active(void *opaque);
bool opna_read_interrupt(void *opaque);
void opna_write_reg(void *opaque, int regnum, int data);
int opna_read_reg(void *opaque, int regnum);
int opna_read_status(void *opaque);
void opna_write_reg_ex(void *opaque, int regnum, int data);
int opna_read_reg_ex(void *opaque, int regnum);
int opna_read_status_ex(void *opaque);

#ifdef __cplusplus
}
#endif

#endif
