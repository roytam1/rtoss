#ifndef __2608INTF_H__
#define __2608INTF_H__

#include "hw.h"
#include "fm.h"
#include "ay8910.h"
#include "fm_def.h"

#define   MAX_2608    (2)

#ifndef VOL_YM3012
/* #define YM3014_VOL(Vol,Pan) VOL_YM3012((Vol)/2,Pan,(Vol)/2,Pan) */
#define YM3012_VOL(LVol,LPan,RVol,RPan) (MIXER(LVol,LPan)|(MIXER(RVol,RPan) << 16))
#endif

struct YM2608interface{
	int num;	/* total number of 8910 in the machine */
	int baseclock;
	mem_read_handler portAread[MAX_8910];
	mem_read_handler portBread[MAX_8910];
	mem_write_handler portAwrite[MAX_8910];
	mem_write_handler portBwrite[MAX_8910];
	void ( *handler[MAX_8910] )( int irq );	/* IRQ handler for the YM2608 */
	void ( *timer_handler[MAX_8910] )( int c, int count );
	void ( *update_handler[MAX_8910] )( void );
	unsigned char *pcmram[MAX_2608];
	int pcmram_size[MAX_2608];
};

/************************************************/
/* Sound Hardware Start							*/
/************************************************/
int YM2608_sh_start(const struct MachineSound *msound, int rate);

/************************************************/
/* Sound Hardware Stop							*/
/************************************************/
void YM2608_sh_stop(void);

void YM2608_sh_reset(void);

void YM2608_sh_postload(void);
void YM2608_sh_save_state(QEMUFile* f);
int YM2608_sh_load_state(QEMUFile* f);

#endif
/**************** end of file ****************/
