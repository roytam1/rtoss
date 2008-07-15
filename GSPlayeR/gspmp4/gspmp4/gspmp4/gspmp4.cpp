#include <windows.h>
#include <tchar.h>
#ifndef _WIN32_WCE
#include <mmsystem.h>
#endif
#include "mapplugin.h"

#include "in_mp4.h"

MAP_DEC_PLUGIN plugin = {0};
CRITICAL_SECTION g_csec;

void WINAPI mp4Init()
{
	InitializeCriticalSection(&g_csec);
}

void WINAPI mp4Quit()
{
	DeleteCriticalSection(&g_csec);
}

DWORD WINAPI mp4GetFunc()
{
	return PLUGIN_FUNC_DECFILE | PLUGIN_FUNC_SEEKFILE | PLUGIN_FUNC_FILETAG | PLUGIN_FUNC_DECSTREAMING;
}

BOOL WINAPI mp4GetPluginName(LPTSTR pszName)
{
	_tcscpy(pszName, _T("GreenSoftware AAC(MP4/M4A) Plug-in"));
	return FALSE;
}

BOOL WINAPI mp4SetEqualizer(MAP_PLUGIN_EQ* pEQ)
{
	return FALSE;
}

void WINAPI mp4ShowConfigDlg(HWND hwndParent)
{
	MessageBox(hwndParent, _T("GreenSoftware AAC(MP4/M4A) Plug-in v1.04\nCopyright(C) 2007 GreenSoftware"), _T("About"), MB_ICONINFORMATION);
}

int WINAPI mp4GetFileExtCount()
{
	return 4;
}

BOOL WINAPI mp4GetFileExt(int nIndex, LPTSTR pszExt, LPTSTR pszExtDesc)
{
	switch (nIndex) {
	case 0:
		_tcscpy(pszExt, _T("mp4"));
		_tcscpy(pszExtDesc, _T("MPEG-4 Files (*.mp4)"));
		return TRUE;
	case 1:
		_tcscpy(pszExt, _T("m4a"));
		_tcscpy(pszExtDesc, _T("MPEG-4 Audio Files (*.m4a)"));
		return TRUE;
	case 2:
		_tcscpy(pszExt, _T("aac"));
		_tcscpy(pszExtDesc, _T("AAC Files (*.aac)"));
		return TRUE;
	case 3:
		_tcscpy(pszExt, _T("3gp"));
		_tcscpy(pszExtDesc, _T("3GP Files (*.3gp)"));
		return TRUE;
	}
	return FALSE;
}

BOOL WINAPI mp4IsValidFile(LPCTSTR pszFile)
{
	return isourfile((TCHAR*)pszFile);
}

BOOL WINAPI mp4OpenFile(LPCTSTR pszFile, MAP_PLUGIN_FILE_INFO* pInfo)
{
	if (open((TCHAR*)pszFile)) {
		memset(&mp4state, 0, sizeof(state));
		return FALSE;
	}

	pInfo->nAvgBitrate = mp4state.avg_bitrate / 1000;
	pInfo->nBitsPerSample = 16;
	pInfo->nChannels = mp4state.channels;
	pInfo->nDuration = mp4state.m_length;
	pInfo->nSampleRate = mp4state.samplerate;

	return TRUE;
}

long WINAPI mp4SeekFile(long lTime)
{
	EnterCriticalSection(&g_csec);
	long ret = setoutputtime(lTime);
	LeaveCriticalSection(&g_csec);
	return ret;
}

BOOL WINAPI mp4StartDecodeFile()
{
	return start() ? TRUE : FALSE;
}

int  WINAPI mp4DecodeFile(WAVEHDR* pHdr)
{
	EnterCriticalSection(&g_csec);
	int ret = decode((uint8_t*)pHdr->lpData, pHdr->dwBufferLength, &pHdr->dwBytesRecorded);
	LeaveCriticalSection(&g_csec);
	return ret;
}

void WINAPI mp4StopDecodeFile()
{
	stop();
}

void WINAPI mp4CloseFile()
{
	close();
}

BOOL WINAPI mp4GetTag(MAP_PLUGIN_FILETAG* pTag)
{
	return gettag(pTag) ? TRUE : FALSE;
}

BOOL WINAPI mp4GetFileTag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* pTag)
{
	return getfiletag(pszFile, pTag) ? TRUE : FALSE;
}

BOOL WINAPI mp4OpenStreaming(LPBYTE pbBuf, DWORD cbBuf, MAP_PLUGIN_STREMING_INFO* pInfo)
{
	return open_streaming(pbBuf, cbBuf, pInfo) ? TRUE : FALSE;
}

int WINAPI mp4DecodeStreaming(LPBYTE pbInBuf, DWORD cbInBuf, DWORD* pcbInUsed, WAVEHDR* pHdr)
{
	return decode_streaming(pbInBuf, cbInBuf, pcbInUsed, pHdr);
}

void WINAPI mp4CloseStreaming()
{
	close_streaming();
}


MAP_DEC_PLUGIN* WINAPI mapGetDecoder()
{
	plugin.dwVersion = PLUGIN_DEC_VER;
	plugin.dwChar = sizeof(TCHAR);
	plugin.Init = mp4Init;
	plugin.Quit = mp4Quit;
	plugin.GetFunc = mp4GetFunc;
	plugin.GetPluginName = mp4GetPluginName;
	plugin.GetFileExtCount = mp4GetFileExtCount;
	plugin.GetFileExt = mp4GetFileExt;
	plugin.SetEqualizer = mp4SetEqualizer;
	plugin.IsValidFile = mp4IsValidFile;
	plugin.ShowConfigDlg = mp4ShowConfigDlg;
	plugin.OpenFile = mp4OpenFile;
	plugin.SeekFile = mp4SeekFile;
	plugin.StartDecodeFile = mp4StartDecodeFile;
	plugin.DecodeFile = mp4DecodeFile;
	plugin.StopDecodeFile = mp4StopDecodeFile;
	plugin.CloseFile = mp4CloseFile;
	plugin.OpenStreaming = mp4OpenStreaming;
	plugin.DecodeStreaming = mp4DecodeStreaming;
	plugin.CloseStreaming = mp4CloseStreaming;
	plugin.GetTag = mp4GetTag;
	plugin.GetFileTag = mp4GetFileTag;

	return &plugin;
}
