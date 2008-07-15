/*
 *  GSPlayer Windows Media Audio Plug-in
 *  Copyright (C) 2007  Y.Nagamidori
 */

#ifndef __WMA_DMO_H__
#define __WMA_DMO_H__

extern "C" {

BOOL DMOLoad(ASF_FILE_INFO* pInfo);
void DMOFree(ASF_FILE_INFO* pInfo);
BOOL DMOSetMediaType(ASF_FILE_INFO* pInfo);
BOOL DMOProcessInputFile(DWORD dwSize, ASF_FILE_INFO* pInfo);
BOOL DMOProcessOutput(WAVEHDR* pHdr, ASF_FILE_INFO* pInfo);
void DMOFlush(ASF_FILE_INFO* pInfo);
void DMODiscontinuity(ASF_FILE_INFO* pInfo);

}

#endif // __WMA_DMO_H__