#ifndef __YMDELTAT_H_
#define __YMDELTAT_H_

#include "hw.h"
#include "fm_def.h"

#define YM_DELTAT_SHIFT    (16)

/* adpcm type A and type B struct */
typedef struct deltat_adpcm_state {
	UINT8 *memory;
	int memory_size;
	double freqbase;
	INT32 *output_pointer; /* pointer of output pointers */
	int output_range;

	UINT8 reg[16];			/* adpcm registers		*/
	UINT8 portstate;		/* port status	: stop==0*/
	int portshift;			/* address shift bits	*/
	UINT8 now_data;			/* current rom data		*/
	UINT32 now_addr;		/* current address		*/
	UINT32 now_step;		/* currect step			*/
	UINT32 step;			/* step					*/
	UINT32 start;			/* start address		*/
	UINT32 end;				/* end address			*/
	UINT32 delta;			/* delta scale			*/
	INT32 volume;			/* current volume		*/
	INT32 *pan;				/* pan : &output_pointer[pan] */
	INT32 acc;				/* shift Measurement value */
	INT32 adpcmd;			/* next Forecast		*/
	INT32 adpcml;			/* current value		*/
	/* leveling and re-sampling state for DELTA-T */
	INT32 volume_w_step;    /* volume with step rate */
	INT32 next_leveling;    /* leveling value        */
	INT32 resample_step;    /* step of re-sampling   */
	/* external flag controll (for YM2610) */
	UINT8 flagMask;			/* arrived flag mask	*/
	UINT8 *arrivedFlagPtr;  /* pointer of arrived end address flag */
}YM_DELTAT;

/* static state */
extern UINT8 *ym_deltat_memory;       /* memory pointer */

/* before YM_DELTAT_ADPCM_CALC(YM_DELTAT *DELTAT); */
#define YM_DELTAT_DECODE_PRESET(DELTAT) {ym_deltat_memory = DELTAT->memory;}

void YM_DELTAT_ADPCM_Write(YM_DELTAT *DELTAT,int r,int v);
void YM_DELTAT_ADPCM_Reset(YM_DELTAT *DELTAT,int pan);
void YM_DELTAT_postload(YM_DELTAT *DELTAT,UINT8 *regs);
void YM_DELTAT_savestate(QEMUFile* f, YM_DELTAT *DELTAT);
int YM_DELTAT_loadstate(QEMUFile* f, YM_DELTAT *DELTAT);

/* INLINE void YM_DELTAT_ADPCM_CALC(YM_DELTAT *DELTAT); */
#define YM_INLINE_BLOCK
#include "ymdeltat.c" /* include inline function section */
#undef  YM_INLINE_BLOCK

#endif
