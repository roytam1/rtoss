/*
  File: fm.h -- header file for software emulation for FM sound generator

*/
#ifndef _H_FM_FM_
#define _H_FM_FM_

#include "hw.h"
#include "fm_def.h"

/* --- system optimize --- */
/* select stereo output buffer : 1=mixing / 0=separate */
#define FM_STEREO_MIX 0
/* select bit size of output : 8 or 16 */
#define FM_SAMPLE_BITS 16
/* select timer system internal or external */
#define FM_INTERNAL_TIMER 0

/* --- speedup optimize --- */
/* busy flag emulation , The definition of FM_GET_TIME_NOW() is necessary. */
#define FM_BUSY_FLAG_SUPPORT 0

/* --- external SSG(YM2149/AY-3-8910)emulator interface port */
/* used by YM2203,YM2608,and YM2610 */

/* SSGClk   : Set SSG Clock      */
/* int n    = chip number        */
/* int clk  = MasterClock(Hz)    */
/* int rate = sample rate(Hz) */
#define SSGClk(chip,clock) AY8910_set_clock((chip)+ay8910_index_ym,clock)

/* SSGWrite : Write SSG port     */
/* int n    = chip number        */
/* int a    = address            */
/* int v    = data               */
#define SSGWrite(n,a,v) AY8910Write((n)+ay8910_index_ym,a,v)

/* SSGRead  : Read SSG port */
/* int n    = chip number   */
/* return   = Read data     */
#define SSGRead(n) AY8910Read((n)+ay8910_index_ym)

/* SSGReset : Reset SSG chip */
/* int n    = chip number   */
#define SSGReset(chip) AY8910_reset((chip)+ay8910_index_ym)

/* --- external callback funstions for realtime update --- */

/* in 2608intf.c */
#define YM2608UpdateReq(chip) YM2608UpdateRequest(chip);

#if FM_STEREO_MIX
  #define YM2608_NUMBUF 1
#else
  #define YM2608_NUMBUF 2    /* FM L+R+ADPCM+RYTHM */
#endif

#if (FM_SAMPLE_BITS==16)
typedef INT16 FMSAMPLE;
typedef unsigned long FMSAMPLE_MIX;
#endif
#if (FM_SAMPLE_BITS==8)
typedef unsigned char  FMSAMPLE;
typedef unsigned short FMSAMPLE_MIX;
#endif

//typedef void (*FM_TIMERHANDLER)(int n,int c,int cnt,double stepTime);
typedef void (*FM_TIMERHANDLER)(int n,int c,int cnt,int clock);
typedef void (*FM_IRQHANDLER)(int n,int irq);
/* FM_TIMERHANDLER : Stop or Start timer         */
/* int n          = chip number                  */
/* int c          = Channel 0=TimerA,1=TimerB    */
/* int count      = timer count (0=stop)         */
/* doube stepTime = step time of one count (sec.)*/

/* FM_IRQHHANDLER : IRQ level changing sense     */
/* int n       = chip number                     */
/* int irq     = IRQ level 0=OFF,1=ON            */

/* -------------------- YM2608(OPNA) Interface -------------------- */
int YM2608Init(int num, int baseclock, int rate,
               void **pcmroma,int *pcmsizea,short *rhythmrom,int *rhythmpos,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2608Shutdown(void);
void YM2608ResetChip(int num);
void YM2608UpdateOne(int num, INT16 **buffer, int length);

void YM2608_postload(int num);
void YM2608_save_state(QEMUFile* f, int num);
int YM2608_load_state(QEMUFile* f, int num);

int YM2608Write(int n, int a,unsigned char v);
unsigned char YM2608Read(int n,int a);
int YM2608TimerOver(int n, int c );

#endif /* _H_FM_FM_ */
