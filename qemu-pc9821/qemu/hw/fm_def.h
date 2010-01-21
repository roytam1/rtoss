/* M.A.M.E. definitions used for YM2608 */

#ifndef __FM_DEF_H__
#define __FM_DEF_H__

typedef unsigned char UINT8;   /* unsigned  8bit */
typedef unsigned short UINT16; /* unsigned 16bit */
typedef unsigned int UINT32;   /* unsigned 32bit */
typedef signed char INT8;      /* signed  8bit   */
typedef signed short INT16;    /* signed 16bit   */
typedef signed int INT32;      /* signed 32bit   */

typedef UINT8 (*mem_read_handler) (UINT32 offset);
typedef void (*mem_write_handler) (UINT32 offset, UINT8 data);

struct MachineSound {
    int sound_type;
    void *sound_interface;
    const char *tag;
};

/* defined in ym2608intf.c */
void YM2608UpdateRequest(int chip);

#endif
