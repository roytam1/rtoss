/* don't support sampling rythm sound yet */
//#define YM2608_USE_SAMPLES
/***************************************************************************

  2608intf.c

  The YM2608 emulator supports up to 2 chips.
  Each chip has the following connections:
  - Status Read / Control Write A
  - Port Read / Data Write A
  - Control Write B
  - Data Write B

***************************************************************************/

/* This version of 2608intf.c is a fork of the MAME 0.59 one, relicensed under the LGPL.
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

#include "hw.h"
#include "ay8910.h"
#include "ym2608intf.h"
#include "fm.h"

static signed short *rhythm_buf;

/* Global Interface holder */
static const struct YM2608interface *intf;

#ifdef YM2608_USE_SAMPLES
static const char *ym2608_pDrumNames[] =
{
	"2608_BD.wav",
	"2608_SD.wav",
	"2608_TOP.wav",
	"2608_HH.wav",
	"2608_TOM.wav",
	"2608_RIM.wav",
	0
};

/* definitions in commmon.c */
struct GameSample
{
	int length;
	int smpfreq;
	int resolution;
	signed char data[1]; /* extendable */
};
struct GameSamples
{
	int total; /* total number of samples */
	struct GameSample *sample[1]; /* extendable */
};
struct GameSamples *readsamples(const char **samplenames,const char *basename)
{
	return 0;
}
#define freesamples(samps)
#endif

/*------------------------- YM2608 -------------------------------*/
/* IRQ Handler */
static void IRQHandler(int n,int irq)
{
	if(intf->handler[n]) intf->handler[n](irq);
}

/* TimerHandler from fm.c */
static void TimerHandler(int n,int c,int count,int clock)
{
	if(intf->timer_handler[n]) intf->timer_handler[n](c, count);
}

/* update request from fm.c */
void YM2608UpdateRequest(int chip)
{
	if(intf->update_handler[chip]) intf->update_handler[chip]();
}

int YM2608_sh_start(const struct MachineSound *msound, int rate)
{
	int i;
	void *pcmbufa[YM2608_NUMBUF];
	int  pcmsizea[YM2608_NUMBUF];
	int rhythm_pos[6+1];
#ifdef YM2608_USE_SAMPLES
	int j;
	struct GameSamples	*psSamples;
	int total_size,r_offset,s_size;
#endif

	intf = msound->sound_interface;
	if( intf->num > MAX_2608 ) return 1;

	if (AY8910_sh_start_ym(msound, rate)) return 1;

	/* stream system initialize */
	for (i = 0;i < intf->num;i++)
	{
		/* setup adpcm buffers */
		pcmbufa[i] = (void *)intf->pcmram[i];
		pcmsizea[i] = intf->pcmram_size[i];
	}

	/* rythm rom build */
	rhythm_buf = 0;
#ifdef YM2608_USE_SAMPLES
	psSamples = readsamples(ym2608_pDrumNames,"ym2608");
	if( psSamples )
	{
		/* calcrate total data size */
		total_size = 0;
		for( i=0;i<6;i++)
		{
			s_size = psSamples->sample[i]->length;
			total_size += s_size ? s_size : 1;
		}
		/* aloocate rythm data */
		rhythm_buf = malloc(total_size * sizeof(signed short));
		if( rhythm_buf==0 ) return 0;

		r_offset = 0;
		/* merge sampling data */
		for(i=0;i<6;i++)
		{
			/* set start point */
			rhythm_pos[i] = r_offset*2;
			/* copy sample data */
			s_size = psSamples->sample[i]->length;
			if(s_size && psSamples->sample[i]->data)
			{
				if( psSamples->sample[i]->resolution==16 )
				{
					signed short *s_ptr = (signed short *)psSamples->sample[i]->data;
					for(j=0;j<s_size;j++) rhythm_buf[r_offset++] = *s_ptr++;
				}else{
					signed char *s_ptr = (signed char *)psSamples->sample[i]->data;
					for(j=0;j<s_size;j++) rhythm_buf[r_offset++] = (*s_ptr++)*0x0101;
				}
			}else rhythm_buf[r_offset++] = 0;
			/* set end point */
			rhythm_pos[i+1] = r_offset*2;
		}
		freesamples( psSamples );
	}else
	{
#endif
		/* aloocate rythm data */
		rhythm_buf = malloc(6 * sizeof(signed short));
		if( rhythm_buf==0 ) return 0;
		for(i=0;i<6;i++)
		{
			/* set start point */
			rhythm_pos[i] = i*2;
			rhythm_buf[i] = 0;
			/* set end point */
			rhythm_pos[i+1] = (i+1)*2;
		}
#ifdef YM2608_USE_SAMPLES
	}
#endif

	/**** initialize YM2608 ****/
	if (YM2608Init(intf->num,intf->baseclock,rate,
		           pcmbufa,pcmsizea,rhythm_buf,rhythm_pos,
		           TimerHandler,IRQHandler) == 0)
		return 0;

	/* error */
	return 1;
}

/************************************************/
/* Sound Hardware Stop							*/
/************************************************/
void YM2608_sh_stop(void)
{
	YM2608Shutdown();
	if( rhythm_buf ) free(rhythm_buf);
	rhythm_buf = 0;
	AY8910_sh_stop_ym();
}
/* reset */
void YM2608_sh_reset(void)
{
	int i;

	for (i = 0;i < intf->num;i++)
		YM2608ResetChip(i);
}

void YM2608_sh_postload(void)
{
	int i;

	for (i = 0;i < intf->num;i++) {
		YM2608_postload(i);
	}
}

void YM2608_sh_save_state(QEMUFile* f)
{
	int i;

	for (i = 0;i < intf->num;i++) {
		YM2608_save_state(f, i);
	}
}

int YM2608_sh_load_state(QEMUFile* f)
{
	int i, ret = 0;

	for (i = 0;i < intf->num;i++) {
		ret |= YM2608_load_state(f, i);
	}
	return ret;
}

/**************** end of file ****************/

