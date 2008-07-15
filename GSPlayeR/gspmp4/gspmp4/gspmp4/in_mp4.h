#ifndef __IN_MP4_H__
#define __IN_MP4_H__

#include "..\include\neaacdec.h"
#include "..\mp4ff\mp4ff.h"

int isourfile(TCHAR *fn);
int open(TCHAR *fn);
void close();
int setoutputtime(int time_in_ms);
int start();
void stop();
int decode(uint8_t *outbuf, uint32_t outbuflen, uint32_t *outlen);
int gettag(MAP_PLUGIN_FILETAG* tag);
int getfiletag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* tag);

BOOL open_streaming(LPBYTE pbInBuf, DWORD cbInBuf, MAP_PLUGIN_STREMING_INFO* pInfo);
int decode_streaming(LPBYTE pbInBuf, DWORD cbInBuf, DWORD* pcbInUsed, WAVEHDR* pHdr);
void close_streaming();


struct seek_list
{
    struct seek_list *next;
    __int64 offset;
};

typedef struct state
{
    /* general stuff */
    NeAACDecHandle hDecoder;
    int samplerate;
	int avg_bitrate;
	int is_seekable;
    unsigned char channels;
    double decode_pos_ms; // current decoding position, in milliseconds
    int paused; // are we paused?
    int seek_needed; // if != -1, it is the point that the decode thread should seek to, in ms.
    TCHAR filename[_MAX_PATH];
    int filetype; /* 0: MP4; 1: AAC */
    int last_frame;
    __int64 last_offset;

    /* MP4 stuff */
    mp4ff_t *mp4file;
    int mp4track;
    long numSamples;
    long sampleId;
    mp4ff_callback_t mp4cb;
    FILE *mp4File;

    /* AAC stuff */
    FILE *aacfile;
    long m_aac_bytes_into_buffer;
    long m_aac_bytes_consumed;
    __int64 m_file_offset;
    unsigned char *m_aac_buffer;
    int m_at_eof;
    double cur_pos_sec;
    int m_header_type;
    struct seek_list *m_head;
    struct seek_list *m_tail;
    unsigned long m_length;
	__int64 m_tagsize;

    /* for gapless decoding */
    unsigned int useAacLength;
    unsigned int framesize;
    unsigned int initial;
    unsigned long timescale;
} state;

extern state mp4state;

#endif // __IN_MP4_H__
