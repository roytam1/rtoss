#include "win_inc.h"

#include "config.h"
#include "output.h"

int OpenOutput(void); /* 0=success, 1=warning, -1=fatal error */
void CloseOutput(void);
void OutputData(int32 *buf, int32 count);
void FlushOutput(void);
void PurgeOutput(void);

#define dpm gsp_play_mode

PlayMode dpm = {
    DEFAULT_RATE, PE_16BIT | PE_SIGNED,
    -1,
	{16},
    "Windows wave mapper", 'd',
    NULL,
    OpenOutput,
    CloseOutput,
    OutputData,
    FlushOutput,
    PurgeOutput
};

extern WAVEHDR* g_pHdr;

__inline void s32tos16_2(int32 *lp, int16 *sp, int32 c)
{
    int32 l;
    while (c--) {
        l = (*lp++) >> (32 - 16 - GUARD_BITS);
        if (l > 32767) l= 32767;
        else if (l < -32768) l= -32768;
        *sp++ = (int16)(l);
    }
}

__inline void s32tou8_2(int32 *lp, uint8 *cp, int32 c)
{
    int32 l;
    while (c--) {
        l = (*lp++) >> (32 - 8 - GUARD_BITS);
        if (l > 127) l= 127;
        else if (l < -128) l= -128;
        *cp++ = 0x80 ^ ((uint8)l);
    }
}

int OpenOutput(void)
{
	return 0;
}


void OutputData(int32 *buf, int32 count)
{
	int converted;
	if (dpm.encoding & PE_16BIT)
		converted = count * 2;
	else
		converted = count;
	if (!(dpm.encoding & PE_MONO))
		converted *= 2;

	if (dpm.encoding & PE_16BIT) {
		/* Convert data to signed 16-bit PCM */
		if (dpm.encoding & PE_MONO)
			s32tos16_2(buf, (int16*)(g_pHdr->lpData + g_pHdr->dwBytesRecorded), count);
		else
			s32tos16_2(buf, (int16*)(g_pHdr->lpData + g_pHdr->dwBytesRecorded), count * 2);
	}
	else {
		/* Convert to 8-bit unsigned. */
		if (dpm.encoding & PE_MONO)
			s32tou8_2(buf, (uint8*)(g_pHdr->lpData + g_pHdr->dwBytesRecorded), count);
		else
			s32tou8_2(buf, (uint8*)(g_pHdr->lpData + g_pHdr->dwBytesRecorded), count * 2);
	}

	g_pHdr->dwBytesRecorded += converted;
}

void CloseOutput(void)
{
}

void FlushOutput(void)
{
}

void PurgeOutput(void)
{
}

int GetSampleCount(int len)
{
	int ret = len;
	if (dpm.encoding & PE_16BIT)
		ret /= 2;

	if (!(dpm.encoding & PE_MONO))
		ret /= 2;

	return ret;
}