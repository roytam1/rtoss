// gspmidi.cpp : DLL アプリケーション用のエントリ ポイントを定義します。
//

#include "gspmidi.h"
#include "config.h"
#include "output.h"

MAP_DEC_PLUGIN plugin;

extern "C" {
	int open_midi_file(char* fn, int* duration, int* bitrate);
	int start_play_midi();
	int play_midi(int maxlen);
	long seek_midi(long time);
	void stop_play_midi();
	void close_midi();

	int read_config_file(char *name);
	void free_instruments(void);
	int GetSampleCount(int len);

#include "common.h"

	WAVEHDR* g_pHdr = NULL;

	extern PlayMode gsp_play_mode;
	extern int free_instruments_afterwards;
	extern int32 control_ratio;
	extern int voices;
	extern int32 amplification;
	extern int adjust_panning_immediately;
	extern int antialiasing_allowed;

};

void ReadRegistory();
void WriteRegistory();
void ShowConfigDialog(HWND hwndParent);

char g_szCopyright[256] = {0};
char g_szTrack[256] = {0};

HINSTANCE g_hInst = NULL;
BOOL g_fUpdateConfig = FALSE;
TCHAR g_szConfigFile[MAX_PATH] = {0};
int g_nSampleRate = 44100;
int g_nBitsPerSample = 16;
int g_nChannels = 2;
int g_nVoices = 32;
int g_nAmp = 80;
BOOL g_fAdjustPanning = FALSE;
BOOL g_fFreeInst = FALSE;
BOOL g_fAntialiasing = FALSE;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInst = (HINSTANCE)hModule;
    return TRUE;
}

BOOL ReadMidiText(LPCTSTR pszFile, LPSTR pszCopyright, LPSTR pszTrack)
{
	//一番最初のトラックチャンクから著作権情報とトラック名を読み込む。
	//バッファ > 0xFFであること

	DWORD dwBuff;
	DWORD dwRead;
	HANDLE hFile;

	if (pszCopyright)
		pszCopyright[0] = '\0';
	if (pszTrack)
		pszTrack[0] = '\0';

	hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!ReadFile(hFile, &dwBuff, 4, &dwRead, NULL) || dwRead != 4 || dwBuff != 0x6468544d) {
		CloseHandle(hFile);
		return FALSE;
	}

	//トラックチャンクの検索
	while (ReadFile(hFile, &dwBuff, 4, &dwRead, NULL) && dwRead == 4)
	{
		if (dwBuff == 0x6B72544D)
			break;

		SetFilePointer(hFile, -3, NULL, FILE_CURRENT);
	}

	if (dwBuff != 0x6B72544D) {
		CloseHandle(hFile);
		return FALSE;
	}

	SetFilePointer(hFile, 4, NULL, FILE_CURRENT);

	while (ReadFile(hFile, &dwBuff, 4, &dwRead, NULL) && dwRead == 4)
	{
		if ((dwBuff & 0x0000FFFF) !=  0x0000FF00 || dwBuff == 0x6B72544D)
			break;

		switch (dwBuff & 0x00FFFF00)
		{
			case 0x02FF00: //Copyright
				dwBuff = (dwBuff&0xFF000000) >> 24;
				if (pszCopyright && !strlen(pszCopyright)) {
					ReadFile(hFile, pszCopyright, dwBuff, &dwRead, NULL);
					*(pszCopyright + dwRead) = '\0';
				}
				else
					SetFilePointer(hFile, dwBuff, NULL, FILE_CURRENT);
				break;
			case 0x03FF00: //Track
				dwBuff = (dwBuff&0xFF000000) >> 24;
				if (pszTrack && !strlen(pszTrack)) {
					ReadFile(hFile, pszTrack, dwBuff, &dwRead, NULL);
					*(pszTrack + dwRead) = '\0';
				}
				else
					SetFilePointer(hFile, dwBuff, NULL, FILE_CURRENT);
				break;
			case 0x01FF00: //other text
			case 0x04FF00: //Instrument
			case 0x05FF00: //Lylics
			case 0x06FF00: //Maker
			case 0x07FF00: //Cue
			default:
				dwBuff = (dwBuff&0xFF000000) >> 24;
				SetFilePointer(hFile, dwBuff, NULL, FILE_CURRENT);
				break;
		}
	}
	CloseHandle(hFile);
	return TRUE;
}

void ResetConfig()
{
	LPTSTR psz;
	char szAnsi[MAX_PATH];
	TCHAR szPath[MAX_PATH];

	free_instruments();

	gsp_play_mode.rate = g_nSampleRate;
	control_ratio = gsp_play_mode.rate / 1000;
	if (g_nBitsPerSample == 8)
		gsp_play_mode.encoding &= ~PE_16BIT;
	else
		gsp_play_mode.encoding |= PE_16BIT;

	if (g_nChannels == 2)
		gsp_play_mode.encoding &= ~PE_MONO;
	else
		gsp_play_mode.encoding |= PE_MONO;

	voices = g_nVoices;
	amplification = g_nAmp;
	adjust_panning_immediately = g_fAdjustPanning;
	free_instruments_afterwards = g_fFreeInst;
	antialiasing_allowed = g_fAntialiasing;

	_tcscpy(szPath, g_szConfigFile);
	psz = _tcsrchr(szPath, '\\');
	if (psz) *psz = NULL;

#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, szPath, -1, szAnsi, MAX_PATH, NULL, NULL);
#else
	strcpy(szAnsi, szPath);
#endif
	add_to_pathlist(szAnsi);

#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, g_szConfigFile, -1, szAnsi, MAX_PATH, NULL, NULL);
#else
	strcpy(szAnsi, g_szConfigFile);
#endif
	read_config_file(szAnsi);	

	g_fUpdateConfig = FALSE;
}

////////////////////////////////////////////////////////////////////
// export functions

void WINAPI midiInit()
{
	ReadRegistory();
	ResetConfig();
}

void WINAPI midiQuit()
{
	WriteRegistory();
}

DWORD WINAPI midiGetFunc()
{
	return PLUGIN_FUNC_DECFILE | PLUGIN_FUNC_SEEKFILE | PLUGIN_FUNC_FILETAG;
}

BOOL WINAPI midiGetPluginName(LPTSTR pszName)
{
	_tcscpy(pszName, _T("GreenSoftware MIDI plug-in"));
	return TRUE;
}

int WINAPI midiGetFileExtCount()
{
	return 1;
}

BOOL WINAPI midiGetFileExt(int nIndex, LPTSTR pszExt, LPTSTR pszExtDesc)
{
	if (nIndex != 0)
		return FALSE;

	_tcscpy(pszExt, _T("mid"));
	_tcscpy(pszExtDesc, _T("MIDI Files (*.mid)"));
	return TRUE; 
}

BOOL WINAPI midiSetEqualizer(MAP_PLUGIN_EQ* pEQ)
{
	return FALSE; 
}

BOOL WINAPI midiIsValidFile(LPCTSTR pszFile)
{
	return TRUE;
}

void WINAPI midiShowConfigDlg(HWND hwndParent)
{
	ShowConfigDialog(hwndParent);
}

BOOL WINAPI midiOpen(LPCTSTR pszFile, MAP_PLUGIN_FILE_INFO* pInfo)
{
	char szFile[MAX_PATH];
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, pszFile, -1, szFile, MAX_PATH, 0, 0);
#else
	strcpy(szFile, pszFile);
#endif

	if (g_fUpdateConfig)
		ResetConfig();

	if (!open_midi_file(szFile, &pInfo->nDuration, &pInfo->nAvgBitrate))
		return FALSE;

	pInfo->nBitsPerSample = g_nBitsPerSample;
	pInfo->nChannels = g_nChannels;
	pInfo->nSampleRate = g_nSampleRate;
	if (!pInfo->nAvgBitrate)
		pInfo->nAvgBitrate = 1;

	memset(g_szCopyright, 0, sizeof(g_szCopyright));
	memset(g_szTrack, 0, sizeof(g_szTrack));
	ReadMidiText(pszFile, g_szCopyright, g_szTrack);

	return TRUE;
}

long WINAPI midiSeek(long lTime)
{
	return seek_midi(lTime);
}

BOOL WINAPI midiStart()
{
	return start_play_midi() ? TRUE : FALSE;
}

int  WINAPI midiDecode(WAVEHDR* pHdr)
{
	int nLen, nRet;
	g_pHdr = pHdr;

	nLen = GetSampleCount(pHdr->dwBufferLength - pHdr->dwBytesRecorded);
	while (nLen >= AUDIO_BUFFER_SIZE) {
		nRet = play_midi(nLen);
		if (nRet == 1)
			return PLUGIN_RET_EOF;
		if (nRet == 2)
			return PLUGIN_RET_SUCCESS;

		nLen = GetSampleCount(pHdr->dwBufferLength - pHdr->dwBytesRecorded);
	}

	return PLUGIN_RET_SUCCESS;
}

void WINAPI midiStop()
{
	stop_play_midi();
	seek_midi(0);
}

void WINAPI midiClose()
{
	close_midi();

	memset(g_szCopyright, 0, sizeof(g_szCopyright));
	memset(g_szTrack, 0, sizeof(g_szTrack));
}

BOOL WINAPI midiGetTag(MAP_PLUGIN_FILETAG* pTag)
{
	if (!strlen(g_szCopyright) && !strlen(g_szTrack))
		return FALSE;

	pTag->nTrackNum = 0;
	pTag->nYear = 0;
	*pTag->szAlbum = NULL;
	*pTag->szArtist = NULL;
	*pTag->szGenre = NULL;

#ifdef _UNICODE
	if (strlen(g_szCopyright))
		MultiByteToWideChar(CP_ACP, 0, g_szCopyright, -1, pTag->szComment, MAX_PLUGIN_TAG_STR);
	if (strlen(g_szTrack))
		MultiByteToWideChar(CP_ACP, 0, g_szTrack, -1, pTag->szTrack, MAX_PLUGIN_TAG_STR);
#else
	_tcscpy(pTag->szComment, g_szCopyright);
	_tcscpy(pTag->szTrack, g_szTrack);
#endif
	return TRUE;
}

BOOL WINAPI midiGetFileTag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* pTag)
{
	char szTrack[256];
	char szCopyright[256];
	if (!ReadMidiText(pszFile, szCopyright, szTrack))
		return FALSE;

	if (!strlen(szCopyright) && !strlen(szTrack))
		return FALSE;

	pTag->nTrackNum = 0;
	pTag->nYear = 0;
	*pTag->szAlbum = NULL;
	*pTag->szArtist = NULL;
	*pTag->szGenre = NULL;

#ifdef _UNICODE
	if (strlen(g_szCopyright))
		MultiByteToWideChar(CP_ACP, 0, szCopyright, -1, pTag->szComment, MAX_PLUGIN_TAG_STR);
	if (strlen(g_szTrack))
		MultiByteToWideChar(CP_ACP, 0, szTrack, -1, pTag->szTrack, MAX_PLUGIN_TAG_STR);
#else
	_tcscpy(pTag->szComment, szCopyright);
	_tcscpy(pTag->szTrack, szTrack);
#endif
	return TRUE;
}

BOOL WINAPI midiOpenStreaming(LPBYTE pbBuf, DWORD cbBuf, MAP_PLUGIN_STREMING_INFO* pInfo)
{
	return FALSE;
}

int WINAPI midiDecodeStreaming(LPBYTE pbInBuf, DWORD cbInBuf, DWORD* pcbInUsed, WAVEHDR* pHdr)
{
	return PLUGIN_RET_ERROR;
}

void WINAPI midiCloseStreaming()
{
}

MAP_DEC_PLUGIN* WINAPI mapGetDecoder()
{
	plugin.dwVersion = PLUGIN_DEC_VER;
	plugin.dwChar = sizeof(TCHAR);
	plugin.Init = midiInit;
	plugin.Quit = midiQuit;
	plugin.GetFunc = midiGetFunc;
	plugin.GetPluginName = midiGetPluginName;
	plugin.GetFileExtCount = midiGetFileExtCount;
	plugin.GetFileExt = midiGetFileExt;
	plugin.SetEqualizer = midiSetEqualizer;
	plugin.IsValidFile = midiIsValidFile;
	plugin.ShowConfigDlg = midiShowConfigDlg;
	plugin.OpenFile = midiOpen;
	plugin.SeekFile = midiSeek;
	plugin.StartDecodeFile = midiStart;
	plugin.DecodeFile = midiDecode;
	plugin.StopDecodeFile = midiStop;
	plugin.CloseFile = midiClose;
	plugin.OpenStreaming = midiOpenStreaming;
	plugin.DecodeStreaming = midiDecodeStreaming;
	plugin.CloseStreaming = midiCloseStreaming;
	plugin.GetTag = midiGetTag;
	plugin.GetFileTag = midiGetFileTag;

	return &plugin;
}

