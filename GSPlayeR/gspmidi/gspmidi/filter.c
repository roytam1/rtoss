/*

TiMidity -- Experimental MIDI to WAVE converter
Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

filter.c: written by Vincent Pagel ( pagel@loria.fr )

implements fir antialiasing filter : should help when setting sample
rates as low as 8Khz.

April 95
- first draft

22/5/95
- modify "filter" so that it simulate leading and trailing 0 in the buffer
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "config.h"
#include "common.h"
#include "instrum.h"
#include "filter.h"

/*  bessel  function   */
static FLOAT_T ino(float x)
{
    FLOAT_T y, de, e, sde;
    int i;
    
    y = x / 2;
    e = 1.0;
    de = 1.0;
    i = 1;
    do {
        de = de * y / (FLOAT_T) i;
        sde = de * de;
        e += sde;
    } while (!( (e * 1.0e-08 - sde > 0) || (i++ > 25) ));
    return(e);
}

/* Kaiser Window (symetric) */
static void kaiser(FLOAT_T *w,int n,FLOAT_T beta)
{
    FLOAT_T xind, xi;
    int i;
    
    xind = (2*n - 1) * (2*n - 1);
    for (i =0; i<n ; i++)
    {
        xi = i + 0.5;
        w[i] = ino((FLOAT_T)(beta * sqrt((double)(1. - 4 * xi * xi / xind))))
        / ino((FLOAT_T)beta);
    }
}

/*
* fir coef in g, cuttoff frequency in fc
*/
static void designfir(FLOAT_T *g , FLOAT_T fc)
{
    int i;
    FLOAT_T xi, omega, att, beta ;
    FLOAT_T w[ORDER2];
    
    for (i =0; i < ORDER2 ;i++)
    {
        xi = (FLOAT_T) i + 0.5;
        omega = PI * xi;
        g[i] = sin( (double) omega * fc) / omega;
    }
    
    att = 40.; /* attenuation  in  db */
    beta = (FLOAT_T) exp(log((double)0.58417 * (att - 20.96)) * 0.4) + 0.07886
    * (att - 20.96);
    kaiser( w, ORDER2, beta);
    
    /* Matrix product */
    for (i =0; i < ORDER2 ; i++)
    g[i] = g[i] * w[i];
}

/*
* FIR filtering -> apply the filter given by coef[] to the data buffer
* Note that we simulate leading and trailing 0 at the border of the
* data buffer
*/
static void filter(sample_t *result,sample_t *data, int32 length,FLOAT_T coef[])
{
    int32 sample,i,sample_window;
    int16 peak = 0;
    FLOAT_T sum;
    
    /* Simulate leading 0 at the begining of the buffer */
    for (sample = 0; sample < ORDER2 ; sample++ )
    {
        sum = 0.0;
        sample_window= sample - ORDER2;
        
        for (i = 0; i < ORDER ;i++)
        sum += coef[i] *
        ((sample_window<0)? 0.0 : data[sample_window++]) ;
        
        /* Saturation ??? */
    if (sum> 32767.) { sum=32767.; peak++; }
    if (sum< -32768.) { sum=-32768; peak++; }
        result[sample] = (sample_t) sum;
    }
    
    /* The core of the buffer  */
    for (sample = ORDER2; sample < length - ORDER + ORDER2 ; sample++ )
    {
        sum = 0.0;
        sample_window= sample - ORDER2;
        
        for (i = 0; i < ORDER ;i++)
        sum += data[sample_window++] * coef[i];
        
        /* Saturation ??? */
    if (sum> 32767.) { sum=32767.; peak++; }
    if (sum< -32768.) { sum=-32768; peak++; }
        result[sample] = (sample_t) sum;
    }
    
    /* Simulate 0 at the end of the buffer */
    for (sample = length - ORDER + ORDER2; sample < length ; sample++ )
    {
        sum = 0.0;
        sample_window= sample - ORDER2;
        
        for (i = 0; i < ORDER ;i++)
        sum += coef[i] *
        ((sample_window>=length)? 0.0 : data[sample_window++]) ;
        
        /* Saturation ??? */
    if (sum> 32767.) { sum=32767.; peak++; }
    if (sum< -32768.) { sum=-32768; peak++; }
        result[sample] = (sample_t) sum;
    }
 }

/***********************************************************************/
/* Prevent aliasing by filtering any freq above the output_rate        */
/*                                                                     */
/* I don't worry about looping point -> they will remain soft if they  */
/* were already                                                        */
/***********************************************************************/
void antialiasing(Sample *sp, int32 output_rate )
{
    sample_t *temp;
    int i;
    FLOAT_T fir_symetric[ORDER];
    FLOAT_T fir_coef[ORDER2];
    FLOAT_T freq_cut;  /* cutoff frequency [0..1.0] FREQ_CUT/SAMP_FREQ*/
        
    /* No oversampling  */
    if (output_rate>=sp->sample_rate)
		return;
    
    freq_cut= (FLOAT_T) output_rate / (FLOAT_T) sp->sample_rate;
    
    designfir(fir_coef,freq_cut);
    
    /* Make the filter symetric */
    for (i = 0 ; i<ORDER2 ;i++)
		fir_symetric[ORDER-1 - i] = fir_symetric[i] = fir_coef[ORDER2-1 - i];
    
    /* We apply the filter we have designed on a copy of the patch */
    temp = safe_malloc(sp->data_length);
    memcpy(temp,sp->data,sp->data_length);

    filter((int16 *)sp->data,temp,sp->data_length/sizeof(sample_t),fir_symetric);

    free(temp);
}
