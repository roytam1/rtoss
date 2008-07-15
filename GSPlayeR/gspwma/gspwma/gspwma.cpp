/*
 *  GSPlayer Windows Media Audio Plug-in
 *  Copyright (C) 2007  Y.Nagamidori
 */

#include <windows.h>
#include <tchar.h>
#ifndef _WIN32_WCE
#include <mmsystem.h>
#endif
#include "dmo.h"
#include "mapplugin.h"
#include "asfdef.h"
#include "mediabuf.h"
#include "asfparser.h"

MAP_DEC_PLUGIN plugin = {0};
ASF_FILE_INFO g_Info = {0};
CRITICAL_SECTION g_csec;

void WINAPI Init()
{
#ifdef _WIN32_WCE
	CoInitializeEx(0, 0);
#else
	CoInitialize(0);
#endif
	InitializeCriticalSection(&g_csec);
}

void WINAPI Quit()
{
	CoUninitialize();
	DeleteCriticalSection(&g_csec);
}

DWORD WINAPI GetFunc()
{
	return PLUGIN_FUNC_DECFILE | PLUGIN_FUNC_SEEKFILE | PLUGIN_FUNC_FILETAG;// | PLUGIN_FUNC_DECSTREAMING;
}

BOOL WINAPI GetPluginName(LPTSTR pszName)
{
	_tcscpy(pszName, _T("GreenSoftware WMA Plug-in"));
	return FALSE;
}

BOOL WINAPI SetEqualizer(MAP_PLUGIN_EQ* pEQ)
{
	return FALSE;
}

void WINAPI ShowConfigDlg(HWND hwndParent)
{
	MessageBox(hwndParent, _T("GreenSoftware WMA Plug-in v1.01\nCopyright(C) 2007 Y.Nagamidori"), _T("About"), MB_ICONINFORMATION);
}

int WINAPI GetFileExtCount()
{
	//return 3;
	return 1;
}

BOOL WINAPI GetFileExt(int nIndex, LPTSTR pszExt, LPTSTR pszExtDesc)
{
	switch (nIndex) {
	case 0:
		_tcscpy(pszExt, _T("wma"));
		_tcscpy(pszExtDesc, _T("Windows Media Audio Files (*.wma)"));
		return TRUE;
	case 1:
		_tcscpy(pszExt, _T("wmv"));
		_tcscpy(pszExtDesc, _T("Windows Media Video Files (*.wmv)"));
		return TRUE;
	case 2:
		_tcscpy(pszExt, _T("asf"));
		_tcscpy(pszExtDesc, _T("Advanced Systems Format Files (*.asf)"));
		return TRUE;
	}
	return FALSE;
}

BOOL WINAPI IsValidFile(LPCTSTR pszFile)
{
	return IsAsfFile(pszFile);
}

BOOL WINAPI OpenFile(LPCTSTR pszFile, MAP_PLUGIN_FILE_INFO* pInfo)
{
	if (!IsAsfFile(pszFile))
		return FALSE;

	if (!OpenAsfFile(pszFile, &g_Info))
		return FALSE;

	*pInfo = g_Info.FileInfo;
	return TRUE;
}

long WINAPI SeekFile(long lTime)
{
	long lRet;
	EnterCriticalSection(&g_csec);
	lRet = SeekAsfFile(&g_Info, lTime);
	LeaveCriticalSection(&g_csec);
	return lRet;
}

BOOL WINAPI StartDecodeFile()
{
	if (!g_Info.hFile)
		return FALSE;

	return StartDecodeAsfFile(&g_Info);
}

int WINAPI DecodeFile(WAVEHDR* pHdr)
{
	int nRet;
	EnterCriticalSection(&g_csec);
	nRet = DecodeAsfFile(&g_Info, pHdr);
	LeaveCriticalSection(&g_csec);
	return nRet;
}

void WINAPI StopDecodeFile()
{
	StopDecodeAsfFile(&g_Info);
}

void WINAPI CloseFile()
{
	CloseAsfFile(&g_Info);
}

BOOL WINAPI GetTag(MAP_PLUGIN_FILETAG* pTag)
{
	if (!g_Info.hFile)
		return FALSE;

	*pTag = g_Info.FileTag;
	return TRUE;
}

BOOL WINAPI GetFileTag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* pTag)
{
	memset(pTag, 0, sizeof(MAP_PLUGIN_FILETAG));
	return GetAsfFileTag(pszFile, pTag);
}

BOOL WINAPI OpenStreaming(LPBYTE pbBuf, DWORD cbBuf, MAP_PLUGIN_STREMING_INFO* pInfo)
{
	return FALSE;
}

int WINAPI DecodeStreaming(LPBYTE pbInBuf, DWORD cbInBuf, DWORD* pcbInUsed, WAVEHDR* pHdr)
{
	return -1;
}

void WINAPI CloseStreaming()
{
}

MAP_DEC_PLUGIN* WINAPI mapGetDecoder()
{
	plugin.dwVersion = PLUGIN_DEC_VER;
	plugin.dwChar = sizeof(TCHAR);
	plugin.Init = Init;
	plugin.Quit = Quit;
	plugin.GetFunc = GetFunc;
	plugin.GetPluginName = GetPluginName;
	plugin.GetFileExtCount = GetFileExtCount;
	plugin.GetFileExt = GetFileExt;
	plugin.SetEqualizer = SetEqualizer;
	plugin.IsValidFile = IsValidFile;
	plugin.ShowConfigDlg = ShowConfigDlg;
	plugin.OpenFile = OpenFile;
	plugin.SeekFile = SeekFile;
	plugin.StartDecodeFile = StartDecodeFile;
	plugin.DecodeFile = DecodeFile;
	plugin.StopDecodeFile = StopDecodeFile;
	plugin.CloseFile = CloseFile;
	plugin.OpenStreaming = OpenStreaming;
	plugin.DecodeStreaming = DecodeStreaming;
	plugin.CloseStreaming = CloseStreaming;
	plugin.GetTag = GetTag;
	plugin.GetFileTag = GetFileTag;

	return &plugin;
}
