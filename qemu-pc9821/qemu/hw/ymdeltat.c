/*
**
** File: ymdeltat.c
**
** YAMAHA DELTA-T adpcm sound emulation subroutine
** used by fmopl.c(v0.37c-) and fm.c(v0.37a-)
**
** Base program is YM2610 emulator by Hiromitsu Shioya.
** Written by Tatsuyuki Satoh
**
** History:
** 05-08-2001 Jarek Burczynski:
**  - now_step is initialized with 0 at the start of play.
** 12-06-2001 Jarek Burczynski:
**  - corrected end of sample bug in YM_DELTAT_ADPCM_CALC.
**    Checked on real YM2610 chip - address register is 24 bits wide.
**    Thanks go to Stefan Jokisch (stefan.jokisch@gmx.de) for tracking down the problem.
**
** TO DO:
**		Check size of the address register on the other chips....
**
** Version 0.37e
**
** sound chips that have this unit
**
** YM2608   OPNA
** YM2610/B OPNB
** Y8950    MSX AUDIO
**
**
*/

/* This version of ay8910.c is a fork of the MAME 0.59 one, relicensed under the LGPL.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef YM_INLINE_BLOCK

#include "ymdeltat.h"

#define INLINE static inline

/* -------------------- log output  -------------------- */
/* log output level */
#define LOG_ERR  3      /* ERROR       */
#define LOG_WAR  2      /* WARNING     */
#define LOG_INF  1      /* INFORMATION */
#define LOG_LEVEL LOG_ERR

#define LOG(n,x)

UINT8 *ym_deltat_memory;      /* memory pointer */

/* Forecast to next Forecast (rate = *8) */
/* 1/8 , 3/8 , 5/8 , 7/8 , 9/8 , 11/8 , 13/8 , 15/8 */
const INT32 ym_deltat_decode_tableB1[16] = {
  1,   3,   5,   7,   9,  11,  13,  15,
  -1,  -3,  -5,  -7,  -9, -11, -13, -15,
};
/* delta to next delta (rate= *64) */
/* 0.9 , 0.9 , 0.9 , 0.9 , 1.2 , 1.6 , 2.0 , 2.4 */
const INT32 ym_deltat_decode_tableB2[16] = {
  57,  57,  57,  57, 77, 102, 128, 153,
  57,  57,  57,  57, 77, 102, 128, 153
};

/* DELTA-T-ADPCM write register */
void YM_DELTAT_ADPCM_Write(YM_DELTAT *DELTAT,int r,int v)
{
	if(r>=0x10) return;
	DELTAT->reg[r] = v; /* stock data */

	switch( r ){
	case 0x00:	/* START,REC,MEMDATA,REPEAT,SPOFF,--,--,RESET */
#if 0
		case 0x60:	/* write buffer MEMORY from PCM data port */
		case 0x20:	/* read  buffer MEMORY to   PCM data port */
#endif
		if( v&0x80 ){
			DELTAT->portstate = v&0x90; /* start req,memory mode,repeat flag copy */
			/**** start ADPCM ****/
			DELTAT->volume_w_step = (double)DELTAT->volume * DELTAT->step / (1<<YM_DELTAT_SHIFT);
			DELTAT->now_addr = (DELTAT->start)<<1;
			DELTAT->now_step = 0;
			DELTAT->acc      = 0;
			DELTAT->adpcml   = 0;
			DELTAT->adpcmd   = YM_DELTAT_DELTA_DEF;
			DELTAT->next_leveling=0;

			if( !DELTAT->step )
			{
				DELTAT->portstate = 0x00;
			}
			/**** PCM memory check & limit check ****/
			if(DELTAT->memory == 0){			/* Check memory Mapped */
				LOG(LOG_ERR,("YM Delta-T ADPCM rom not mapped\n"));
				DELTAT->portstate = 0x00;
			}else{
				if( DELTAT->end >= DELTAT->memory_size )
				{		/* Check End in Range */
					LOG(LOG_ERR,("YM Delta-T ADPCM end out of range: $%08x\n",DELTAT->end));
					DELTAT->end = DELTAT->memory_size - 1;
				}
				if( DELTAT->start >= DELTAT->memory_size )
				{		/* Check Start in Range */
					LOG(LOG_ERR,("YM Delta-T ADPCM start out of range: $%08x\n",DELTAT->start));
					DELTAT->portstate = 0x00;
				}
			}
		} else if( v&0x01 ){
			DELTAT->portstate = 0x00;
		}
		break;
	case 0x01:	/* L,R,-,-,SAMPLE,DA/AD,RAMTYPE,ROM */
		DELTAT->pan = &DELTAT->output_pointer[(v>>6)&0x03];
		break;
	case 0x02:	/* Start Address L */
	case 0x03:	/* Start Address H */
		DELTAT->start  = (DELTAT->reg[0x3]*0x0100 | DELTAT->reg[0x2]) << DELTAT->portshift;
			/*logerror("DELTAT start: 02=%2x 03=%2x addr=%8x\n",DELTAT->reg[0x2], DELTAT->reg[0x3],DELTAT->start );*/
		break;
	case 0x04:	/* Stop Address L */
	case 0x05:	/* Stop Address H */
		DELTAT->end    = (DELTAT->reg[0x5]*0x0100 | DELTAT->reg[0x4]) << DELTAT->portshift;
		DELTAT->end   += (1<<DELTAT->portshift) - 1;
			/*logerror("DELTAT end  : 04=%2x 05=%2x addr=%8x\n",DELTAT->reg[0x4], DELTAT->reg[0x5],DELTAT->end   );*/
		break;
	case 0x06:	/* Prescale L (PCM and Recoard frq) */
	case 0x07:	/* Proscale H */
	case 0x08:	/* ADPCM data */
	  break;
	case 0x09:	/* DELTA-N L (ADPCM Playback Prescaler) */
	case 0x0a:	/* DELTA-N H */
		DELTAT->delta  = (DELTAT->reg[0xa]*0x0100 | DELTAT->reg[0x9]);
			/*logerror("DELTAT deltan:09=%2x 0a=%2x\n",DELTAT->reg[0x9], DELTAT->reg[0xa]);*/
		DELTAT->step     = (UINT32)( (double)(DELTAT->delta /* *(1<<(YM_DELTAT_SHIFT-16)) */ ) * (DELTAT->freqbase) );
		DELTAT->volume_w_step = (double)DELTAT->volume * DELTAT->step / (1<<YM_DELTAT_SHIFT);
		break;
	case 0x0b:	/* Level control (volume , voltage flat) */
		{
			INT32 oldvol = DELTAT->volume;
			DELTAT->volume = (v&0xff) * (DELTAT->output_range/256) / YM_DELTAT_DECODE_RANGE;
//								v	  *		((1<<16)>>8)		>>	15;
//						thus:	v	  *		(1<<8)				>>	15;
//						thus: output_range must be (1 << (15+8)) at least
//								v     *		((1<<23)>>8)		>>	15;
//								v	  *		(1<<15)				>>	15;
			/*logerror("DELTAT vol = %2x\n",v&0xff);*/
			if( oldvol != 0 )
			{
				DELTAT->adpcml        = (int)((double)DELTAT->adpcml        / (double)oldvol * (double)DELTAT->volume);
				DELTAT->resample_step = (int)((double)DELTAT->resample_step / (double)oldvol * (double)DELTAT->volume);
			}
			DELTAT->volume_w_step = (double)DELTAT->volume * DELTAT->step / (1<<YM_DELTAT_SHIFT);
		}
		break;
	}
}

void YM_DELTAT_ADPCM_Reset(YM_DELTAT *DELTAT,int pan)
{
	DELTAT->now_addr  = 0;
	DELTAT->now_step  = 0;
	DELTAT->step      = 0;
	DELTAT->start     = 0;
	DELTAT->end       = 0;
	/* F2610->adpcm[i].delta     = 21866; */
	DELTAT->volume    = 0;
	DELTAT->pan       = &DELTAT->output_pointer[pan];
	/* DELTAT->flagMask  = 0; */
	DELTAT->acc       = 0;
	DELTAT->adpcmd    = 127;
	DELTAT->adpcml    = 0;
	DELTAT->volume_w_step = 0;
	DELTAT->next_leveling = 0;
	DELTAT->portstate = 0;
	/* DELTAT->portshift = 8; */
}

void YM_DELTAT_postload(YM_DELTAT *DELTAT,UINT8 *regs)
{
	int r;

	/* to keep adpcml and resample_step */
	DELTAT->volume = 0;
	/* update */
	for(r=1;r<16;r++)
		YM_DELTAT_ADPCM_Write(DELTAT,r,regs[r]);
	DELTAT->reg[0] = regs[0];
	/* current rom data */
	DELTAT->now_data = *(ym_deltat_memory+(DELTAT->now_addr>>1));

}

void YM_DELTAT_savestate(QEMUFile* f, YM_DELTAT *DELTAT)
{
	qemu_put_8s(f, &DELTAT->portstate);
	qemu_put_be32s(f, &DELTAT->now_addr);
	qemu_put_be32s(f, &DELTAT->now_step);
	qemu_put_be32s(f, &DELTAT->acc);
	qemu_put_be32s(f, &DELTAT->adpcmd);
	qemu_put_be32s(f, &DELTAT->adpcml);
	qemu_put_be32s(f, &DELTAT->next_leveling);
	qemu_put_be32s(f, &DELTAT->resample_step);
}

int YM_DELTAT_loadstate(QEMUFile* f, YM_DELTAT *DELTAT)
{
	qemu_get_8s(f, &DELTAT->portstate);
	qemu_get_be32s(f, &DELTAT->now_addr);
	qemu_get_be32s(f, &DELTAT->now_step);
	qemu_get_be32s(f, &DELTAT->acc);
	qemu_get_be32s(f, &DELTAT->adpcmd);
	qemu_get_be32s(f, &DELTAT->adpcml);
	qemu_get_be32s(f, &DELTAT->next_leveling);
	qemu_get_be32s(f, &DELTAT->resample_step);

	return 0;
}
#else /* YM_INLINE_BLOCK */

/* ---------- inline block ---------- */

/* DELTA-T particle adjuster */
#define YM_DELTAT_DELTA_MAX (24576)
#define YM_DELTAT_DELTA_MIN (127)
#define YM_DELTAT_DELTA_DEF (127)

#define YM_DELTAT_DECODE_RANGE 32768
#define YM_DELTAT_DECODE_MIN (-(YM_DELTAT_DECODE_RANGE))
#define YM_DELTAT_DECODE_MAX ((YM_DELTAT_DECODE_RANGE)-1)

extern const INT32 ym_deltat_decode_tableB1[];
extern const INT32 ym_deltat_decode_tableB2[];

#define YM_DELTAT_Limit(val,max,min)	\
{										\
	if ( val > max ) val = max;			\
	else if ( val < min ) val = min;	\
}

/**** ADPCM B (Delta-T control type) ****/
INLINE void YM_DELTAT_ADPCM_CALC(YM_DELTAT *DELTAT)
{
	UINT32 step;
	int data;
	INT32 prev_acc;
	INT32 now_leveling;

	DELTAT->now_step += DELTAT->step;
	if ( DELTAT->now_step >= (1<<YM_DELTAT_SHIFT) )
	{
		step = DELTAT->now_step >> YM_DELTAT_SHIFT;
		DELTAT->now_step &= (1<<YM_DELTAT_SHIFT)-1;
		do{
			if ( DELTAT->now_addr == (DELTAT->end<<1) ) {	/* 12-06-2001 JB: corrected comparison. Was > instead of == */
				if( DELTAT->portstate&0x10 ){
					/**** repeat start ****/
					DELTAT->now_addr = DELTAT->start<<1;
					DELTAT->acc      = 0;
					DELTAT->adpcmd   = YM_DELTAT_DELTA_DEF;
					DELTAT->next_leveling = 0;
				}else{
					if(DELTAT->arrivedFlagPtr)
						(*DELTAT->arrivedFlagPtr) |= DELTAT->flagMask;
					DELTAT->portstate = 0;
					DELTAT->adpcml = 0;
					now_leveling = 0;
					return;
				}
			}
			if( DELTAT->now_addr&1 ) data = DELTAT->now_data & 0x0f;
			else
			{
				DELTAT->now_data = *(ym_deltat_memory+(DELTAT->now_addr>>1));
				data = DELTAT->now_data >> 4;
			}

			DELTAT->now_addr++;
			/* 12-06-2001 JB: */
			/* YM2610 address register is 24 bits wide.*/
			/* The "+1" is there because we use 1 bit more for nibble calculations.*/
			/* WARNING: */
			/* Side effect: we should take the size of the mapped ROM into account */
			DELTAT->now_addr &= ( (1<<(24+1))-1);

			/* store accumulator value */
			prev_acc     = DELTAT->acc;
			/* Forecast to next Forecast */
			DELTAT->acc += (ym_deltat_decode_tableB1[data] * DELTAT->adpcmd / 8);
			YM_DELTAT_Limit(DELTAT->acc,YM_DELTAT_DECODE_MAX, YM_DELTAT_DECODE_MIN);
			/* delta to next delta */
			DELTAT->adpcmd = (DELTAT->adpcmd * ym_deltat_decode_tableB2[data] ) / 64;
			YM_DELTAT_Limit(DELTAT->adpcmd,YM_DELTAT_DELTA_MAX, YM_DELTAT_DELTA_MIN );
			/* calulate new leveling value */
			now_leveling      = DELTAT->next_leveling;
			DELTAT->next_leveling = prev_acc + ((DELTAT->acc - prev_acc) / 2 );

		}while(--step);

/* #define YM_DELTAT_CUT_RE_SAMPLING */
#ifdef YM_DELTAT_CUT_RE_SAMPLING
		DELTAT->adpcml  = DELTAT->next_leveling * DELTAT->volume;
		DELTAT->adpcml  = DELTAT->acc * DELTAT->volume;
	}
#else
		/* delta step of resampling */
		DELTAT->resample_step = (DELTAT->next_leveling - now_leveling) * DELTAT->volume_w_step;
//		DELTAT->resample_step = (DELTAT->next_leveling - now_leveling) *
//							((double)DELTAT->volume * DELTAT->step / (1<<YM_DELTAT_SHIFT));

		/* output of start point */
		DELTAT->adpcml  = now_leveling * DELTAT->volume;
		/* adjust to now */
		DELTAT->adpcml += (int)((double)DELTAT->resample_step * ((double)DELTAT->now_step/(double)DELTAT->step));
	}
	DELTAT->adpcml += DELTAT->resample_step;
#endif
	/* output for work of output channels (outd[OPNxxxx])*/
	*(DELTAT->pan) += DELTAT->adpcml;
}

#endif /* YM_INLINE_BLOCK */
