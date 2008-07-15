/*
 *  GSPlayer Windows Media Audio Plug-in
 *  Copyright (C) 2007  Y.Nagamidori
 */

#include <windows.h>
#include <tchar.h>
#ifndef _WIN32_WCE
#include <mmsystem.h>
#endif
#include <initguid.h>
#include "dmo.h"
#include <uuids.h>
#ifndef _WIN32_WCE
//#include <wmcodecdsp.h>
#endif

#include "mapplugin.h"
#include "asfdef.h"
#include "mediabuf.h"
#include "asfparser.h"
#include "wmadmo.h"

#define WAVE_FORMAT_WMA8		0x161
#define WAVE_FORMAT_WMA9		0x162
#define WAVE_FORMAT_WMA9LL		0x163

DEFINE_GUID(CLSID_CWMADecMediaObject, 0x2eeb4adf, 0x4578, 0x4d10, 0xbc, 0xa7, 0xbb, 0x95, 0x5f, 0x56, 0x32, 0x0a);
DEFINE_GUID(MEDIASUBTYPE_AudioBase, 0x00000000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
const GUID IID_IMediaObject = { 0xd8ad0f58, 0x5494, 0x4102,{0x97, 0xc5, 0xec, 0x79, 0x8e, 0x59, 0xbc, 0xf4}};

HRESULT InitMediaType(DMO_MEDIA_TYPE* p, DWORD Size)
{
	p->pUnk = NULL;
	p->pbFormat = NULL;
	p->cbFormat = Size;
	if (Size) {
		p->pbFormat = (LPBYTE)CoTaskMemAlloc(Size);
		if (!p->pbFormat) {
			p->cbFormat = 0;
			return E_OUTOFMEMORY;
		}
	}
	return S_OK;
}

HRESULT FreeMediaType(DMO_MEDIA_TYPE* p)
{
	if (p->pUnk) {
		p->pUnk->Release();
		p->pUnk = NULL;
	}
	if (p->pbFormat) {
		CoTaskMemFree(p->pbFormat);
		p->pbFormat = NULL;
	}
	p->cbFormat = 0;
	return S_OK;
}

BOOL DMOLoad(ASF_FILE_INFO* pInfo)
{
	IMediaObject* pMediaObject = NULL;
#ifdef _WIN32_WCE
	int (__stdcall *pCreateCodecDMO)(IMediaObject** pMediaObject);
	pInfo->hCodecDll = LoadLibrary(_T("wma9prodecoder.dll"));
	if (pInfo->hCodecDll) {
		(FARPROC&)pCreateCodecDMO = GetProcAddress(pInfo->hCodecDll, _T("CreateCodecDMO"));
		if (pCreateCodecDMO) {
			pCreateCodecDMO(&pMediaObject);
		}
		if (!pMediaObject) {
			FreeLibrary(pInfo->hCodecDll);
			pInfo->hCodecDll = NULL;
		}
	}
	if (!pMediaObject) {
		pInfo->hCodecDll = LoadLibrary(_T("wmadecoder.dll"));
		if (pInfo->hCodecDll) {
			(FARPROC&)pCreateCodecDMO = GetProcAddress(pInfo->hCodecDll, _T("CreateCodecDMO"));
			if (pCreateCodecDMO) {
				pCreateCodecDMO(&pMediaObject);
			}
			if (!pMediaObject) {
				FreeLibrary(pInfo->hCodecDll);
				pInfo->hCodecDll = NULL;
			}
		}
	}

	pInfo->pMediaObject = pMediaObject;
	return pInfo->pMediaObject ? TRUE : FALSE;
#else
	CoCreateInstance(CLSID_CWMADecMediaObject, NULL, CLSCTX_INPROC_SERVER, IID_IMediaObject, (void**)&pMediaObject);
	pInfo->pMediaObject = pMediaObject;
	return pInfo->pMediaObject ? TRUE : FALSE;
#endif
}

void DMOFree(ASF_FILE_INFO* pInfo)
{
	if (pInfo->pmbOut) {
		pInfo->pmbOut->Release();
		pInfo->pmbOut = NULL;
	}
	if (pInfo->pMediaObject) {
		pInfo->pMediaObject->Flush();
		pInfo->pMediaObject->FreeStreamingResources();
		pInfo->pMediaObject->Release();
		pInfo->pMediaObject = NULL;
	}
	if (pInfo->hCodecDll) {
		FreeLibrary(pInfo->hCodecDll);
		pInfo->hCodecDll = NULL;
	}
}

BOOL SetInputMediaType(ASF_FILE_INFO* pInfo)
{
	WAVEFORMATEX* pwf;
	DMO_MEDIA_TYPE mt ={0};
	if (InitMediaType(&mt, pInfo->dwFormat) != S_OK)
		return FALSE;

	pwf = (WAVEFORMATEX*)pInfo->pbFormat;
	memcpy(mt.pbFormat, pInfo->pbFormat, pInfo->dwFormat);
	mt.majortype = MEDIATYPE_Audio;
	mt.subtype = MEDIASUBTYPE_AudioBase;
	mt.subtype.Data1 = pwf->wFormatTag;
	mt.formattype = FORMAT_WaveFormatEx;
	mt.bTemporalCompression = 0;
	mt.bFixedSizeSamples = 1;
	mt.lSampleSize = 0;

	if (pInfo->pMediaObject->SetInputType(0, &mt, 0) != S_OK) {
#ifdef _WIN32_WCE
		// from TCPMP
		if (pwf->wFormatTag == WAVE_FORMAT_WMA8 && pwf->cbSize == 10) {
			LPBYTE pb = mt.pbFormat + sizeof(WAVEFORMATEX);
			memset(pb, 0, pwf->cbSize);
			memcpy(pb + 2, pwf + 1, 6);
			if (pInfo->pMediaObject->SetInputType(0, &mt, 0) != S_OK) {
				FreeMediaType(&mt);
				return FALSE;
			}
		}
		else {
			FreeMediaType(&mt);
			return FALSE;
		}
#else
		FreeMediaType(&mt);
		return FALSE;
#endif
	}

	FreeMediaType(&mt);
	return TRUE;
}

BOOL SetOutputMediaType(ASF_FILE_INFO* pInfo)
{
	WAVEFORMATEX* pwfIn;
	WAVEFORMATEX* pwfOut;
	DMO_MEDIA_TYPE mt;
	if (InitMediaType(&mt, sizeof(WAVEFORMATEX)) != S_OK)
		return FALSE;

	pwfIn = (WAVEFORMATEX*)pInfo->pbFormat;
	pwfOut = (WAVEFORMATEX*)mt.pbFormat;
	pwfOut->wFormatTag = WAVE_FORMAT_PCM;
	pwfOut->nChannels = pwfIn->nChannels;
	pwfOut->nSamplesPerSec = pwfIn->nSamplesPerSec;
	pwfOut->wBitsPerSample = pwfIn->wBitsPerSample;
	pwfOut->nAvgBytesPerSec = pwfOut->wBitsPerSample * pwfOut->nSamplesPerSec * pwfOut->nChannels / 8;
	pwfOut->nBlockAlign = pwfOut->wBitsPerSample * pwfOut->nChannels / 8;
	pwfOut->cbSize = 0;

	mt.majortype = MEDIATYPE_Audio;
	mt.subtype = MEDIASUBTYPE_PCM;
	mt.formattype = FORMAT_WaveFormatEx;
	mt.bTemporalCompression = 0;
	mt.bFixedSizeSamples = 1;
	mt.lSampleSize = 0;

	if (pInfo->pMediaObject->SetOutputType(0, &mt, 0) != S_OK) {
		FreeMediaType(&mt);
		return FALSE;
	}

	pInfo->pmbOut = new CMediaBuffer(pwfOut->nAvgBytesPerSec * 2);
	if (!pInfo->pmbOut) {
		FreeMediaType(&mt);
		return FALSE;
	}
	
	FreeMediaType(&mt);

	return TRUE;
}

BOOL DMOSetMediaType(ASF_FILE_INFO* pInfo)
{
	if (!SetInputMediaType(pInfo))
		return FALSE;

	if (!SetOutputMediaType(pInfo))
		return FALSE;

	if (FAILED(pInfo->pMediaObject->AllocateStreamingResources()))
		return FALSE;

	pInfo->bDiscontinuity = FALSE;
	return TRUE;
}

BOOL DMOProcessInputFile(DWORD dwSize, ASF_FILE_INFO* pInfo)
{	
	HRESULT hr;
	LPBYTE pbInBuf;
	DWORD dwInBuf, dwRead;

	CMediaBuffer* pmbIn = new CMediaBuffer(dwSize);
	if (!pmbIn)
		return FALSE;

	pmbIn->GetBufferAndLength(&pbInBuf, &dwInBuf);
	if (!ReadFile(pInfo->hFile, pbInBuf, dwSize, &dwRead, NULL) || dwRead != dwSize)
		return FALSE;
		
	pmbIn->SetLength(dwSize);
	hr = pInfo->pMediaObject->ProcessInput(0, pmbIn, pInfo->PayloadInfo.Key ? DMO_INPUT_DATA_BUFFERF_SYNCPOINT : 0, 0, 0);
	
	pmbIn->Release();
	return hr == S_OK ? TRUE : FALSE;
}

BOOL DMOProcessOutput(WAVEHDR* pHdr, ASF_FILE_INFO* pInfo)
{
	HRESULT hr;
	LPBYTE pbOut;
	DWORD dwOut;
	DWORD dwStatus;
	DWORD dwCopy = 0;
	DMO_OUTPUT_DATA_BUFFER dodb = {0};

	pInfo->pmbOut->GetBufferAndLength(&pbOut, &dwOut);
	if (dwOut && pHdr) {
		dwCopy = min(pHdr->dwBufferLength - pHdr->dwBytesRecorded, dwOut);
		memcpy((LPBYTE)pHdr->lpData + pHdr->dwBytesRecorded, pbOut, dwCopy);
		memmove(pbOut, pbOut + dwCopy, dwOut - dwCopy);
		pInfo->pmbOut->SetLength(dwOut - dwCopy);
		pHdr->dwBytesRecorded += dwCopy;

		if (pHdr->dwBytesRecorded == pHdr->dwBufferLength)
			return TRUE;
	}

	dodb.pBuffer = pInfo->pmbOut;
	hr = pInfo->pMediaObject->ProcessOutput(0, 1, &dodb, &dwStatus);
	
	pInfo->pmbOut->GetBufferAndLength(&pbOut, &dwOut);
	if (dwOut && pHdr) {
		dwCopy = min(pHdr->dwBufferLength - pHdr->dwBytesRecorded, dwOut);
		memcpy((LPBYTE)pHdr->lpData + pHdr->dwBytesRecorded, pbOut, dwCopy);
		memmove(pbOut, pbOut + dwCopy, dwOut - dwCopy);
		pInfo->pmbOut->SetLength(dwOut - dwCopy);
		pHdr->dwBytesRecorded += dwCopy;

		if (pHdr->dwBytesRecorded == pHdr->dwBufferLength)
			return TRUE;
	}
	return FALSE;
}

void DMOFlush(ASF_FILE_INFO* pInfo)
{
	if (pInfo->pMediaObject) {
		pInfo->pMediaObject->Flush();
	}
	if (pInfo->pmbOut) {
		pInfo->pmbOut->SetLength(0);
	}
	pInfo->bDiscontinuity = FALSE;
}

void DMODiscontinuity(ASF_FILE_INFO* pInfo)
{
	pInfo->pMediaObject->Discontinuity(0);
	pInfo->bDiscontinuity = TRUE;
}
