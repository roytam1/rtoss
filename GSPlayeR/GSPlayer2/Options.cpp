#include "GSPlayer2.h"
#include "MainWnd.h"
#include "Options.h"

#define	NAME_WINDOW_TOPMOST	_T("Window Top Most")
#define	NAME_WINDOW_LEFT	_T("Window Left")
#define	NAME_WINDOW_TOP		_T("Window Top")
#define NAME_PLAY_REPEAT	_T("Repeat")
#define NAME_PLAY_SHUFFLE	_T("Shuffle")
#define NAME_LAST_DIR		_T("LastDir")
#define NAME_SKIN_FILE		_T("SkinFile")
#define NAME_SURROUND_ENABLE	_T("SurroundEnable")
#define NAME_SURROUND_RATE		_T("SurroundRate")
#define NAME_EQ_ENABLE			_T("EqEnable")
#define NAME_EQ_PREAMP			_T("Preamp")
#define NAME_EQ_BANK1			_T("EqBank1")
#define NAME_EQ_BANK2			_T("EqBank2")
#define NAME_EQ_BANK3			_T("EqBank3")
#define NAME_EQ_BANK4			_T("EqBank4")
#define NAME_EQ_BANK5			_T("EqBank5")
#define NAME_EQ_BANK6			_T("EqBank6")
#define NAME_EQ_BANK7			_T("EqBank7")
#define NAME_EQ_BANK8			_T("EqBank8")
#define NAME_EQ_BANK9			_T("EqBank9")
#define NAME_EQ_BANK10			_T("EqBank10")
#define NAME_BASSBOOST			_T("BassBoost")
#define NAME_3DCHORUS_ENABLE	_T("3DChorusEnable")
#define NAME_3DCHORUS_RATE		_T("3DChorusRate")
#define NAME_REVERB_ENABLE		_T("ReverbEnable")
#define NAME_REVERB_RATE		_T("ReverbRate")
#define NAME_REVERB_DELAY		_T("ReverbDelay")
#define NAME_ECHO_ENABLE		_T("EchoEnable")
#define NAME_ECHO_RATE			_T("EchoRate")
#define NAME_ECHO_DELAY			_T("EchoDelay")
#define NAME_DRAW_PEEK			_T("DrawPeek")
#define NAME_TIME_REVERSE		_T("TimeReverse")
#define NAME_SAVE_DEFLIST		_T("SaveDefPlayList")
#define NAME_ADD_EXISTING		_T("AddExisting")
#define NAME_RESUME				_T("Resume")
#define NAME_RESUME_TRACK		_T("ResumeTrack")
#define NAME_RESUME_TIME		_T("ResumeTime")
#define NAME_PLAYONSTART		_T("PlayOnStart")
#define NAME_TRAYICON			_T("TrayIcon")
#define NAME_SEARCHSUBFOLDERS	_T("SearchSubFolders")
#define NAME_SCROLLTITLE		_T("ScrollTitle")
#define NAME_OUTPUT_BUFF		_T("OutputBufferLen")
#define NAME_OUTPUT_PREBUFF		_T("OutputPrebuffer")
#define NAME_OUTPUT_FADE		_T("OutputFade")
#define NAME_SCAN_COMPLETELY	_T("ScanCompletely")
#define NAME_SUPPRESS_ZERO		_T("SuppressZeroSamples")
#define NAME_ALWAYSOPENDEV		_T("AlwaysOpenDev")
#define NAME_STREAMING_BUFF		_T("StreamingBuffer")
#define NAME_STREAMING_PREBUFF	_T("StreamingPreBuffer")
#define NAME_STREAMING_PROXY	_T("StreamingProxy")
#define NAME_STREAMING_USEPROXY	_T("StreamingUseProxy")
#define NAME_SLEEP_MINUTES		_T("SleepMinutes")
#define NAME_SLEEP_POWEROFF		_T("SleepPowerOff")

#define NAME_PLAYLIST_TRACKNO	_T("ShowTrackNo")
#define NAME_PLAYLIST_TIME		_T("ShowTime")
#define NAME_PLAYLIST_FASTLOAD	_T("FastLoad")
#define NAME_RSRATE				_T("RandomSlopeRate")
#define NAME_RSBASE				_T("RandomSlopeBase")

#define MAX_LOCATION			5
#define KEY_LOCATION			_T("Software\\GreenSoftware\\GSPlayer\\MP3\\Location")
#define NAME_LOCATION			_T("Location")

#define KEY_AUDIOFILE			_T("GSPlayer AudioFile")
#define NAME_AUDIOFILE			_T("Audio File")
#define KEY_PLAYLIST			_T("GSPlayer Playlist")
#define NAME_PLAYLIST			_T("Play List File")
#define VALUE_BACKUP			_T("GSPlayer Backup")

#define OUTPUT_BUFF_MAX			5000
#define OUTPUT_BUFF_MIN			20
#define OUTPUT_PREBUFF_MAX		100
#define OUTPUT_PREBUFF_MIN		0
#define STREAMING_BUFF_MAX		1024
#define STREAMING_BUFF_MIN		4

#ifndef ListView_SetCheckState
#define ListView_SetCheckState(hwndLV, i, fCheck) \
   ListView_SetItemState(hwndLV, i, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), LVIS_STATEIMAGEMASK)
#endif

#ifndef ListView_GetCheckState
#define ListView_GetCheckState(hwndLV, i) \
   ((((UINT)(SNDMSG((hwndLV), LVM_GETITEMSTATE, (WPARAM)i, LVIS_STATEIMAGEMASK))) >> 12) -1)
#endif

COptions::COptions()
{
	m_fTopMost = FALSE;
	m_ptInitWnd.x = (GetSystemMetrics(SM_CXSCREEN) - MAINWND_WIDTH) / 2;
	m_ptInitWnd.y = (GetSystemMetrics(SM_CYSCREEN) - MAINWND_HEIGHT) / 2;
	*m_szLastDir = NULL;
	*m_szSkinFile = NULL;
	m_fDrawPeek = FALSE;
	m_fShuffle = FALSE;
	m_fRepeat = RepeatNone;
	m_fResume = FALSE;
	m_fPlayOnStart = FALSE;
	m_fTimeReverse = FALSE;
	m_fSaveDefList = TRUE;
	m_fAddExisting = FALSE;
	m_fTrayIcon = FALSE;
	m_fSearchSubFolders = FALSE;
#ifdef _WIN32_WCE
	m_fScrollTitle = FALSE;
#else
	m_fScrollTitle = TRUE;
#endif

	m_nSleepMinutes = 30;
	m_fSleepPowerOff = TRUE;
	m_nSleepTime = -1;
	m_dwSleepLast = 0;
	
	// RT Hack
	m_fShowTrackNo = FALSE;
	m_fFastLoad = FALSE;
	m_fRandomSlopeRate = 0;
	m_fRandomSlopeBase = 50;

	m_nResumeTrack = -1;
	m_nResumeTime = 0;

	m_hMap = NULL;
	m_pszLocation = NULL;
}

COptions::~COptions()
{
}

void COptions::Load(HANDLE hMap)
{
	HKEY hKey = 0;
	TCHAR szProxy[MAX_URL] = {0};
	MAP_GetOptions(hMap, &m_Options);
	MAP_GetStreamingOptions(hMap, &m_StreamingOptions);
	if (RegOpenKeyEx(HKEY_CURRENT_USER, KEY_SETTINGS, 
						0, KEY_READ, &hKey) == ERROR_SUCCESS) {

		DWORD dwType, dwBuff, dwSize;
		dwSize = sizeof(dwBuff);

		// effect
		EFFECT effect;
		if (RegQueryValueEx(hKey, NAME_SURROUND_ENABLE, 0, &dwType, (LPBYTE)&effect.fEnable, &dwSize) != ERROR_SUCCESS)
			effect.fEnable = FALSE;
		if (RegQueryValueEx(hKey, NAME_SURROUND_RATE, 0, &dwType, (LPBYTE)&effect.nRate, &dwSize) != ERROR_SUCCESS)
			effect.nRate = 25;
		MAP_SetEffect(hMap, EFFECT_SURROUND, &effect);

		if (RegQueryValueEx(hKey, NAME_3DCHORUS_ENABLE, 0, &dwType, (LPBYTE)&effect.fEnable, &dwSize) != ERROR_SUCCESS)
			effect.fEnable = FALSE;
		if (RegQueryValueEx(hKey, NAME_3DCHORUS_RATE, 0, &dwType, (LPBYTE)&effect.nRate, &dwSize) != ERROR_SUCCESS)
			effect.nRate = 2;
		MAP_SetEffect(hMap, EFFECT_3DCHORUS, &effect);

		if (RegQueryValueEx(hKey, NAME_REVERB_ENABLE, 0, &dwType, (LPBYTE)&effect.fEnable, &dwSize) != ERROR_SUCCESS)
			effect.fEnable = FALSE;
		if (RegQueryValueEx(hKey, NAME_REVERB_RATE, 0, &dwType, (LPBYTE)&effect.nRate, &dwSize) != ERROR_SUCCESS)
			effect.nRate = 20;
		if (RegQueryValueEx(hKey, NAME_REVERB_DELAY, 0, &dwType, (LPBYTE)&effect.nDelay, &dwSize) != ERROR_SUCCESS)
			effect.nDelay = 100;
		MAP_SetEffect(hMap, EFFECT_REVERB, &effect);

		if (RegQueryValueEx(hKey, NAME_ECHO_ENABLE, 0, &dwType, (LPBYTE)&effect.fEnable, &dwSize) != ERROR_SUCCESS)
			effect.fEnable = FALSE;
		if (RegQueryValueEx(hKey, NAME_ECHO_RATE, 0, &dwType, (LPBYTE)&effect.nRate, &dwSize) != ERROR_SUCCESS)
			effect.nRate = 20;
		if (RegQueryValueEx(hKey, NAME_ECHO_DELAY, 0, &dwType, (LPBYTE)&effect.nDelay, &dwSize) != ERROR_SUCCESS)
			effect.nDelay = 100;
		MAP_SetEffect(hMap, EFFECT_ECHO, &effect);

		EQUALIZER eq;
		if (RegQueryValueEx(hKey, NAME_EQ_ENABLE, 0, &dwType, (LPBYTE)&eq.fEnable, &dwSize) != ERROR_SUCCESS)
			eq.fEnable = FALSE;
		if (RegQueryValueEx(hKey, NAME_EQ_PREAMP, 0, &dwType, (LPBYTE)&eq.preamp, &dwSize) != ERROR_SUCCESS)
			eq.preamp = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK1, 0, &dwType, (LPBYTE)&eq.data[0], &dwSize) != ERROR_SUCCESS)
			eq.data[0] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK2, 0, &dwType, (LPBYTE)&eq.data[1], &dwSize) != ERROR_SUCCESS)
			eq.data[1] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK3, 0, &dwType, (LPBYTE)&eq.data[2], &dwSize) != ERROR_SUCCESS)
			eq.data[2] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK4, 0, &dwType, (LPBYTE)&eq.data[3], &dwSize) != ERROR_SUCCESS)
			eq.data[3] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK5, 0, &dwType, (LPBYTE)&eq.data[4], &dwSize) != ERROR_SUCCESS)
			eq.data[4] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK6, 0, &dwType, (LPBYTE)&eq.data[5], &dwSize) != ERROR_SUCCESS)
			eq.data[5] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK7, 0, &dwType, (LPBYTE)&eq.data[6], &dwSize) != ERROR_SUCCESS)
			eq.data[6] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK8, 0, &dwType, (LPBYTE)&eq.data[7], &dwSize) != ERROR_SUCCESS)
			eq.data[7] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK9, 0, &dwType, (LPBYTE)&eq.data[8], &dwSize) != ERROR_SUCCESS)
			eq.data[8] = 31;
		if (RegQueryValueEx(hKey, NAME_EQ_BANK10, 0, &dwType, (LPBYTE)&eq.data[9], &dwSize) != ERROR_SUCCESS)
			eq.data[9] = 31;
		MAP_SetEqualizer(hMap, &eq);

		// bassboost
		int nLevel;
		if (RegQueryValueEx(hKey, NAME_BASSBOOST, 0, &dwType, (LPBYTE)&nLevel, &dwSize) != ERROR_SUCCESS)
			nLevel = 0;
		MAP_SetBassBoostLevel(hMap, nLevel);

		// window positions
		if (RegQueryValueEx(hKey, NAME_WINDOW_TOPMOST, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fTopMost = dwBuff;
		if (RegQueryValueEx(hKey, NAME_WINDOW_LEFT, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_ptInitWnd.x = dwBuff;
		if (RegQueryValueEx(hKey, NAME_WINDOW_TOP, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_ptInitWnd.y = dwBuff;

		// general settings
		if (RegQueryValueEx(hKey, NAME_PLAY_REPEAT, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fRepeat = dwBuff;
		if (RegQueryValueEx(hKey, NAME_PLAY_SHUFFLE, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fShuffle = dwBuff;
		if (RegQueryValueEx(hKey, NAME_DRAW_PEEK, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fDrawPeek = dwBuff;
		if (RegQueryValueEx(hKey, NAME_TIME_REVERSE, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fTimeReverse = dwBuff;
		if (RegQueryValueEx(hKey, NAME_SAVE_DEFLIST, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fSaveDefList = dwBuff;
		if (RegQueryValueEx(hKey, NAME_ADD_EXISTING, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fAddExisting = dwBuff;
		if (RegQueryValueEx(hKey, NAME_RESUME, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fResume = dwBuff;
		if (RegQueryValueEx(hKey, NAME_RESUME_TRACK, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_nResumeTrack = dwBuff;
		if (RegQueryValueEx(hKey, NAME_RESUME_TIME, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_nResumeTime = dwBuff;
		if (RegQueryValueEx(hKey, NAME_PLAYONSTART, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fPlayOnStart = dwBuff;
		if (RegQueryValueEx(hKey, NAME_TRAYICON, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fTrayIcon = dwBuff;
		if (RegQueryValueEx(hKey, NAME_SEARCHSUBFOLDERS, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fSearchSubFolders = dwBuff;
		if (RegQueryValueEx(hKey, NAME_SCROLLTITLE, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fScrollTitle = dwBuff;
		if (RegQueryValueEx(hKey, NAME_SLEEP_MINUTES, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_nSleepMinutes = dwBuff;

		// RT Hack
		if (RegQueryValueEx(hKey, NAME_PLAYLIST_TRACKNO, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fShowTrackNo = dwBuff;
		if (RegQueryValueEx(hKey, NAME_PLAYLIST_TIME, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fShowTime = dwBuff;
		if (RegQueryValueEx(hKey, NAME_PLAYLIST_FASTLOAD, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fFastLoad = dwBuff;
		if (RegQueryValueEx(hKey, NAME_RSRATE, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fRandomSlopeRate = dwBuff;
		if (RegQueryValueEx(hKey, NAME_RSBASE, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fRandomSlopeBase = dwBuff;
#ifdef _WIN32_WCE
		if (RegQueryValueEx(hKey, NAME_SLEEP_POWEROFF, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_fSleepPowerOff = dwBuff;
#endif


		MAP_SetScanPeek(hMap, m_fDrawPeek);

		// decoder
		if (RegQueryValueEx(hKey, NAME_OUTPUT_BUFF, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_Options.nOutputBufferLen = dwBuff;
		if (RegQueryValueEx(hKey, NAME_OUTPUT_PREBUFF, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_Options.nOutputPrebuffer = dwBuff;
		if (RegQueryValueEx(hKey, NAME_OUTPUT_FADE, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_Options.fFadeIn = dwBuff;
		if (RegQueryValueEx(hKey, NAME_SCAN_COMPLETELY, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_Options.fScanMpegCompletely = dwBuff;
		if (RegQueryValueEx(hKey, NAME_SUPPRESS_ZERO, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_Options.fSuppressZeroSamples = dwBuff;
		if (RegQueryValueEx(hKey, NAME_ALWAYSOPENDEV, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_Options.fAlwaysOpenDevice = dwBuff;

		dwSize = sizeof(m_szLastDir);
		if (RegQueryValueEx(hKey, NAME_LAST_DIR, 0, &dwType, (LPBYTE)&m_szLastDir, &dwSize) != ERROR_SUCCESS)
			*m_szLastDir = NULL;

		// skin
		dwSize = sizeof(m_szSkinFile);
		if (RegQueryValueEx(hKey, NAME_SKIN_FILE, 0, &dwType, (LPBYTE)&m_szSkinFile, &dwSize) != ERROR_SUCCESS)
			*m_szSkinFile = NULL;

		// streaming
		dwSize = sizeof(dwBuff);
		if (RegQueryValueEx(hKey, NAME_STREAMING_BUFF, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_StreamingOptions.nBuf = dwBuff;
		if (RegQueryValueEx(hKey, NAME_STREAMING_PREBUFF, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_StreamingOptions.nPreBuf = dwBuff;
		if (RegQueryValueEx(hKey, NAME_STREAMING_USEPROXY, 0, &dwType, (LPBYTE)&dwBuff, &dwSize) == ERROR_SUCCESS)
			m_StreamingOptions.fUseProxy = dwBuff;

		dwSize = sizeof(szProxy);
		if (RegQueryValueEx(hKey, NAME_STREAMING_PROXY, 0, &dwType, (LPBYTE)&szProxy, &dwSize) != ERROR_SUCCESS)
			*szProxy = NULL;
		_tcscpy(m_StreamingOptions.szProxy, szProxy);

		RegCloseKey(hKey);
	}

	MAP_SetOptions(hMap, &m_Options);
	MAP_SetStreamingOptions(hMap, &m_StreamingOptions);
}

void COptions::Save(HANDLE hMap)
{
	HKEY hKey = 0;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, KEY_SETTINGS, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {

		// effect
		EFFECT effect;
		MAP_GetEffect(hMap, EFFECT_SURROUND, &effect);
		RegSetValueEx(hKey, NAME_SURROUND_ENABLE, 0, REG_DWORD, (LPBYTE)&effect.fEnable, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_SURROUND_RATE, 0, REG_DWORD, (LPBYTE)&effect.nRate, sizeof(DWORD));
		MAP_GetEffect(hMap, EFFECT_3DCHORUS, &effect);
		RegSetValueEx(hKey, NAME_3DCHORUS_ENABLE, 0, REG_DWORD, (LPBYTE)&effect.fEnable, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_3DCHORUS_RATE, 0, REG_DWORD, (LPBYTE)&effect.nRate, sizeof(DWORD));
		MAP_GetEffect(hMap, EFFECT_REVERB, &effect);
		RegSetValueEx(hKey, NAME_REVERB_ENABLE, 0, REG_DWORD, (LPBYTE)&effect.fEnable, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_REVERB_DELAY, 0, REG_DWORD, (LPBYTE)&effect.nDelay, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_REVERB_RATE, 0, REG_DWORD, (LPBYTE)&effect.nRate, sizeof(DWORD));
		MAP_GetEffect(hMap, EFFECT_ECHO, &effect);
		RegSetValueEx(hKey, NAME_ECHO_ENABLE, 0, REG_DWORD, (LPBYTE)&effect.fEnable, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_ECHO_DELAY, 0, REG_DWORD, (LPBYTE)&effect.nDelay, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_ECHO_RATE, 0, REG_DWORD, (LPBYTE)&effect.nRate, sizeof(DWORD));

		EQUALIZER eq;
		MAP_GetEqualizer(hMap, &eq);
		RegSetValueEx(hKey, NAME_EQ_ENABLE, 0, REG_DWORD, (LPBYTE)&eq.fEnable, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_PREAMP, 0, REG_DWORD, (LPBYTE)&eq.preamp, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK1, 0, REG_DWORD, (LPBYTE)&eq.data[0], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK2, 0, REG_DWORD, (LPBYTE)&eq.data[1], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK3, 0, REG_DWORD, (LPBYTE)&eq.data[2], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK4, 0, REG_DWORD, (LPBYTE)&eq.data[3], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK5, 0, REG_DWORD, (LPBYTE)&eq.data[4], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK6, 0, REG_DWORD, (LPBYTE)&eq.data[5], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK7, 0, REG_DWORD, (LPBYTE)&eq.data[6], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK8, 0, REG_DWORD, (LPBYTE)&eq.data[7], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK9, 0, REG_DWORD, (LPBYTE)&eq.data[8], sizeof(DWORD));
		RegSetValueEx(hKey, NAME_EQ_BANK10, 0, REG_DWORD, (LPBYTE)&eq.data[9], sizeof(DWORD));

		int nLevel = MAP_GetBassBoostLevel(hMap);
		RegSetValueEx(hKey, NAME_BASSBOOST, 0, REG_DWORD, (LPBYTE)&nLevel, sizeof(DWORD));

		// window positions
		RegSetValueEx(hKey, NAME_WINDOW_TOPMOST, 0, REG_DWORD, (LPBYTE)&m_fTopMost, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_WINDOW_LEFT, 0, REG_DWORD, (LPBYTE)&m_ptInitWnd.x, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_WINDOW_TOP, 0, REG_DWORD, (LPBYTE)&m_ptInitWnd.y, sizeof(DWORD));

		// general settings
		RegSetValueEx(hKey, NAME_PLAY_REPEAT, 0, REG_DWORD, (LPBYTE)&m_fRepeat, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_PLAY_SHUFFLE, 0, REG_DWORD, (LPBYTE)&m_fShuffle, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_DRAW_PEEK, 0, REG_DWORD, (LPBYTE)&m_fDrawPeek, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_TIME_REVERSE, 0, REG_DWORD, (LPBYTE)&m_fTimeReverse, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_SAVE_DEFLIST, 0, REG_DWORD, (LPBYTE)&m_fSaveDefList, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_ADD_EXISTING, 0, REG_DWORD, (LPBYTE)&m_fAddExisting, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_RESUME, 0, REG_DWORD, (LPBYTE)&m_fResume, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_RESUME_TRACK, 0, REG_DWORD, (LPBYTE)&m_nResumeTrack, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_RESUME_TIME, 0, REG_DWORD, (LPBYTE)&m_nResumeTime, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_PLAYONSTART, 0, REG_DWORD, (LPBYTE)&m_fPlayOnStart, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_TRAYICON, 0, REG_DWORD, (LPBYTE)&m_fTrayIcon, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_SEARCHSUBFOLDERS, 0, REG_DWORD, (LPBYTE)&m_fSearchSubFolders, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_SCROLLTITLE, 0, REG_DWORD, (LPBYTE)&m_fScrollTitle, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_LAST_DIR, 0, REG_SZ, (LPBYTE)m_szLastDir, sizeof(TCHAR) * (_tcslen(m_szLastDir) + 1));
		RegSetValueEx(hKey, NAME_SKIN_FILE, 0, REG_SZ, (LPBYTE)m_szSkinFile, sizeof(TCHAR) * (_tcslen(m_szSkinFile) + 1));
		RegSetValueEx(hKey, NAME_SLEEP_MINUTES, 0, REG_DWORD, (LPBYTE)&m_nSleepMinutes, sizeof(DWORD));
		// RT Hack
		RegSetValueEx(hKey, NAME_PLAYLIST_TRACKNO, 0, REG_DWORD, (LPBYTE)&m_fShowTrackNo, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_PLAYLIST_TIME, 0, REG_DWORD, (LPBYTE)&m_fShowTime, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_PLAYLIST_FASTLOAD, 0, REG_DWORD, (LPBYTE)&m_fFastLoad, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_RSRATE, 0, REG_DWORD, (LPBYTE)&m_fRandomSlopeRate, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_RSBASE, 0, REG_DWORD, (LPBYTE)&m_fRandomSlopeBase, sizeof(DWORD));
#ifdef _WIN32_WCE
		RegSetValueEx(hKey, NAME_SLEEP_POWEROFF, 0, REG_DWORD, (LPBYTE)&m_fSleepPowerOff, sizeof(DWORD));
#endif

		// decoder
		RegSetValueEx(hKey, NAME_OUTPUT_BUFF, 0, REG_DWORD, 
							(LPBYTE)&m_Options.nOutputBufferLen, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_OUTPUT_PREBUFF, 0, REG_DWORD, 
							(LPBYTE)&m_Options.nOutputPrebuffer, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_OUTPUT_FADE, 0, REG_DWORD, 
							(LPBYTE)&m_Options.fFadeIn, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_SCAN_COMPLETELY, 0, REG_DWORD, 
							(LPBYTE)&m_Options.fScanMpegCompletely, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_SUPPRESS_ZERO, 0, REG_DWORD, 
							(LPBYTE)&m_Options.fSuppressZeroSamples, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_ALWAYSOPENDEV, 0, REG_DWORD, 
							(LPBYTE)&m_Options.fAlwaysOpenDevice, sizeof(DWORD));

		// streaming
		RegSetValueEx(hKey, NAME_STREAMING_BUFF, 0, REG_DWORD, 
							(LPBYTE)&m_StreamingOptions.nBuf, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_STREAMING_PREBUFF, 0, REG_DWORD, 
							(LPBYTE)&m_StreamingOptions.nPreBuf, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_STREAMING_USEPROXY, 0, REG_DWORD, 
							(LPBYTE)&m_StreamingOptions.fUseProxy, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_STREAMING_PROXY, 0, REG_SZ, (LPBYTE)m_StreamingOptions.szProxy, 
							sizeof(TCHAR) * (_tcslen(m_StreamingOptions.szProxy) + 1));

		RegCloseKey(hKey);
	}
}

void COptions::UpdateBufferInfo(HWND hwndDlg, HANDLE hMap)
{
	TCHAR sz[128];
	CTempStr strFmt(IDS_FMT_BUFINFO);
	DWORD cbTotalAudio, cbBufferedAudio, cbTotalStream, cbBufferedStream;
	MAP_GetBufferInfo(hMap, &cbTotalAudio, &cbBufferedAudio, &cbTotalStream, &cbBufferedStream);

	wsprintf(sz, strFmt, cbBufferedStream / 1024, cbTotalStream / 1024, cbTotalStream ? cbBufferedStream * 100 / cbTotalStream : 0,
		cbBufferedAudio / 1024, cbTotalAudio / 1024, cbTotalAudio ? cbBufferedAudio * 100 / cbTotalAudio : 0);
	SetDlgItemText(hwndDlg, IDC_STATIC_BUFINFO, sz);
}

void COptions::ShowAboutDlg(HWND hwndParent, HANDLE hMap)
{
	DialogBoxParam(GetInst(), (LPCTSTR)IDD_ABOUT_DLG, hwndParent, AboutDlgProc, (LPARAM)hMap);
}

BOOL CALLBACK COptions::AboutDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HANDLE hMap;
	switch (uMsg) {
	case WM_INITDIALOG:
		hMap = (HANDLE)lParam;
		ShellInitDialog(hwndDlg);
		COptions::UpdateBufferInfo(hwndDlg, hMap);
		SetTimer(hwndDlg, ID_TIMER_BUFINFO, 500, NULL);
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			KillTimer(hwndDlg, ID_TIMER_BUFINFO);
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		}
	case WM_TIMER:
		COptions::UpdateBufferInfo(hwndDlg, hMap);
		return TRUE;
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
#ifdef _WIN32_WCE_PPC
	case WM_SIZE:
	{
		RECT rc;
		HWND hwnd = GetDlgItem(hwndDlg, IDC_STATIC_BUFINFO);
		GetWindowRect(hwnd, &rc);

		POINT pt = {rc.left, rc.top};
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(GetDlgItem(hwndDlg, IDC_STATIC_BUFINFO), 
				pt.x, pt.y, LOWORD(lParam) - pt.x * 2, HIWORD(lParam) - pt.y - pt.x, TRUE);
		return TRUE;
	}
#endif
	default:
		return FALSE;
	}
}

void COptions::ShowOptionDlg(HWND hwndParent, HANDLE hMap)
{
	m_hMap = hMap;
	CTempStr strTitle(IDS_TITLE_OPTION);

	PROPSHEETPAGE* pPage = NULL;
	int nPages = GetPropPages(&pPage);
	if (!pPage || !nPages)
		return;
	
	PROPSHEETHEADER psh;
	psh.dwSize = sizeof(PROPSHEETHEADER);
#ifdef _WIN32_WCE_PPC
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_USECALLBACK | PSH_MAXIMIZE;
#else
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW | PSH_USECALLBACK;
#endif
	psh.pfnCallback = PropSheetCallback;
	psh.nPages = nPages;
	psh.nStartPage = 0;
	psh.pszIcon = NULL;
	psh.pszCaption = strTitle;
	psh.hwndParent = hwndParent;
	psh.hInstance = GetInst();
	psh.ppsp = pPage;
	PropertySheet(&psh);

	delete [] pPage;
	m_hMap = NULL;
}

int COptions::GetPropPages(PROPSHEETPAGE** ppPage)
{
#define DEF_NUM_OF_PAGES	6
	*ppPage = new PROPSHEETPAGE[DEF_NUM_OF_PAGES];
	memset(*ppPage, 0, sizeof(PROPSHEETPAGE) * DEF_NUM_OF_PAGES);
	(*ppPage)[0].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[0].dwFlags = PSP_DEFAULT;
	(*ppPage)[0].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_PLAYER);
	(*ppPage)[0].pfnDlgProc = (DLGPROC)PlayerPageProc;
	(*ppPage)[0].hInstance = GetInst();
	(*ppPage)[0].pszIcon = NULL;
	(*ppPage)[0].lParam = (LPARAM)this;
	(*ppPage)[1].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[1].dwFlags = PSP_DEFAULT;
	(*ppPage)[1].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_DECODER);
	(*ppPage)[1].pfnDlgProc = (DLGPROC)DecoderPageProc;
	(*ppPage)[1].hInstance = GetInst();
	(*ppPage)[1].pszIcon = NULL;
	(*ppPage)[1].lParam = (LPARAM)this;
	(*ppPage)[2].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[2].dwFlags = PSP_DEFAULT;
	(*ppPage)[2].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_STREAMING);
	(*ppPage)[2].pfnDlgProc = (DLGPROC)StreamingPageProc;
	(*ppPage)[2].hInstance = GetInst();
	(*ppPage)[2].pszIcon = NULL;
	(*ppPage)[2].lParam = (LPARAM)this;
	(*ppPage)[3].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[3].dwFlags = PSP_DEFAULT;
	(*ppPage)[3].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_SKIN);
	(*ppPage)[3].pfnDlgProc = (DLGPROC)SkinPageProc;
	(*ppPage)[3].hInstance = GetInst();
	(*ppPage)[3].pszIcon = NULL;
	(*ppPage)[3].lParam = (LPARAM)this;
	(*ppPage)[4].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[4].dwFlags = PSP_DEFAULT;
	(*ppPage)[4].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_ASSOCIATE);
	(*ppPage)[4].pfnDlgProc = (DLGPROC)AssociatePageProc;
	(*ppPage)[4].hInstance = GetInst();
	(*ppPage)[4].pszIcon = NULL;
	(*ppPage)[4].lParam = (LPARAM)this;
	(*ppPage)[5].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[5].dwFlags = PSP_DEFAULT;
	(*ppPage)[5].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_PLUGIN);
	(*ppPage)[5].pfnDlgProc = (DLGPROC)PlugInPageProc;
	(*ppPage)[5].hInstance = GetInst();
	(*ppPage)[5].pszIcon = NULL;
	(*ppPage)[5].lParam = (LPARAM)this;
	return DEF_NUM_OF_PAGES;
}

BOOL CALLBACK COptions::PlayerPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (COptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->PlayerPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->PlayerPageOnOK(hwndDlg);
				return TRUE;
			}
			return FALSE;
		}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	default:
		return FALSE;
	}
}

void COptions::PlayerPageOnInitDialog(HWND hwndDlg)
{
	if (m_fDrawPeek)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PEEK), BM_SETCHECK, 1, 0);
	if (m_fSaveDefList)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SAVE_DEFLIST), BM_SETCHECK, 1, 0);
	if (m_fAddExisting)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ADD_EXISTING), BM_SETCHECK, 1, 0);
	if (m_fResume)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_RESUME), BM_SETCHECK, 1, 0);
	if (m_fPlayOnStart)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PLAYONSTART), BM_SETCHECK, 1, 0);
	if (m_fTrayIcon)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_TRAYICON), BM_SETCHECK, 1, 0);
	if (m_fScrollTitle)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SCROLLTITLE), BM_SETCHECK, 1, 0);
	if (m_fShowTrackNo)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWTRACKNO), BM_SETCHECK, 1, 0);
	if (m_fShowTime)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWTIME), BM_SETCHECK, 1, 0);
	if (m_fFastLoad)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_FASTLOAD), BM_SETCHECK, 1, 0);
	TCHAR szBuf[32];
	wsprintf(szBuf, _T("%d"), m_fRandomSlopeRate);
	SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RSRATE), szBuf);
	wsprintf(szBuf, _T("%d"), m_fRandomSlopeBase);
	SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RSBASE), szBuf);
}

void COptions::PlayerPageOnOK(HWND hwndDlg)
{
	m_fDrawPeek = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PEEK), BM_GETCHECK, 0, 0);
	m_fSaveDefList = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SAVE_DEFLIST), BM_GETCHECK, 0, 0);
	m_fAddExisting = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ADD_EXISTING), BM_GETCHECK, 0, 0);
	m_fResume = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_RESUME), BM_GETCHECK, 0, 0);
	m_fTrayIcon = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_TRAYICON), BM_GETCHECK, 0, 0);
	m_fPlayOnStart = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PLAYONSTART), BM_GETCHECK, 0, 0);
	m_fScrollTitle = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SCROLLTITLE), BM_GETCHECK, 0, 0);
	m_fShowTrackNo = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWTRACKNO), BM_GETCHECK, 0, 0);
	m_fShowTime = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOWTIME), BM_GETCHECK, 0, 0);
	m_fFastLoad = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_FASTLOAD), BM_GETCHECK, 0, 0);
	TCHAR szBuf[32];
	int n;
	GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RSRATE), szBuf, 32);
	n = _tcstol(szBuf, 0, 10);
	if (n < -32767 || n > 32767)
		n = min(max(n, -32767), 32767);
		
	m_fRandomSlopeRate = n;
	GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_RSBASE), szBuf, 32);
	n = _tcstol(szBuf, 0, 10);
	if (n < 0 || n > 100)
		n = min(max(n, 0), 100);
		
	m_fRandomSlopeBase = n;
}

BOOL CALLBACK COptions::DecoderPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (COptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->DecoderPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->DecoderPageOnOK(hwndDlg);
				return TRUE;
			}
			else if (pnmh->code == UDN_DELTAPOS) {
				pOptions->DecoderPageOnDeltaPos(hwndDlg, (NM_UPDOWN*)pnmh);
				return TRUE;
			}
			return FALSE;
		}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	default:
		return FALSE;
	}
}

void COptions::DecoderPageOnInitDialog(HWND hwndDlg)
{
	ShellInitDialog(hwndDlg);

	SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_OUT_BUFF_LEN), EM_LIMITTEXT, 4, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_OUT_PREBUFF), EM_LIMITTEXT, 3, 0);

	TCHAR szBuf[32];
	wsprintf(szBuf, _T("%d"), m_Options.nOutputBufferLen);
	SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_OUT_BUFF_LEN), szBuf);
	wsprintf(szBuf, _T("%d"), m_Options.nOutputPrebuffer);
	SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_OUT_PREBUFF), szBuf);
	if (m_Options.fFadeIn)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_OUTPUT_FADE), BM_SETCHECK, 1, 0);
	if (m_Options.fScanMpegCompletely)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SCANCOMPLETELY), BM_SETCHECK, 1, 0);
	if (m_Options.fSuppressZeroSamples)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SUPZERO), BM_SETCHECK, 1, 0);
	if (m_Options.fAlwaysOpenDevice)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ALWAYSOPENDEV), BM_SETCHECK, 1, 0);
}

void COptions::DecoderPageOnOK(HWND hwndDlg)
{
	TCHAR szBuf[32];
	int n;
	GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_OUT_BUFF_LEN), szBuf, 32);
	n = _tcstol(szBuf, 0, 10);
	if (n < OUTPUT_BUFF_MIN || n > OUTPUT_BUFF_MAX)
		n = min(max(n, OUTPUT_BUFF_MIN), OUTPUT_BUFF_MAX);
		
	m_Options.nOutputBufferLen = n;

	GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_OUT_PREBUFF), szBuf, 32);
	n = _tcstol(szBuf, 0, 10);
	if (n < 0 || n > 100)
		n = min(max(n, 0), 100);

	m_Options.nOutputPrebuffer = n;

	m_Options.fFadeIn = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_OUTPUT_FADE), BM_GETCHECK, 0, 0);
	m_Options.fScanMpegCompletely = 
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SCANCOMPLETELY), BM_GETCHECK, 0, 0);
	m_Options.fSuppressZeroSamples = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SUPZERO), BM_GETCHECK, 0, 0);
	m_Options.fAlwaysOpenDevice = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ALWAYSOPENDEV), BM_GETCHECK, 0, 0);
}

void COptions::DecoderPageOnDeltaPos(HWND hwndDlg, NM_UPDOWN* pNmud)
{
	TCHAR sz[32];
	if (pNmud->hdr.idFrom == IDC_SPIN_OUT_BUFF_LEN) {
		GetDlgItemText(hwndDlg, IDC_EDIT_OUT_BUFF_LEN, sz, 32);

		int n = _tcstol(sz, NULL, 10);
		if (pNmud->iDelta < 0)
			n = min(n + 50, OUTPUT_BUFF_MAX);
		else if (pNmud->iDelta > 0)
			n = max(n - 50, OUTPUT_BUFF_MIN);
		wsprintf(sz, _T("%d"), n);
		SetDlgItemText(hwndDlg, IDC_EDIT_OUT_BUFF_LEN, sz);
	}
	else if (pNmud->hdr.idFrom == IDC_SPIN_OUT_PREBUFF) {
		GetDlgItemText(hwndDlg, IDC_EDIT_OUT_PREBUFF, sz, 32);

		int n = _tcstol(sz, NULL, 10);
		if (pNmud->iDelta < 0)
			n = min(n + 5, OUTPUT_PREBUFF_MAX);
		else if (pNmud->iDelta > 0)
			n = max(n - 5, OUTPUT_PREBUFF_MIN);
		wsprintf(sz, _T("%d"), n);
		SetDlgItemText(hwndDlg, IDC_EDIT_OUT_PREBUFF, sz);
	}
}

BOOL CALLBACK COptions::StreamingPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (COptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->StreamingPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->StreamingPageOnOK(hwndDlg);
				return TRUE;
			}
			else if (pnmh->code == UDN_DELTAPOS) {
				pOptions->StreamingPageOnDeltaPos(hwndDlg, (NM_UPDOWN*)pnmh);
				return TRUE;
			}
			return FALSE;
		}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	default:
		return FALSE;
	}
}

void COptions::StreamingPageOnInitDialog(HWND hwndDlg)
{
	TCHAR sz[MAX_URL];

	SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_BUFF), EM_LIMITTEXT, 4, 0);
	SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_PREBUFF), EM_LIMITTEXT, 4, 0);

	wsprintf(sz, _T("%d"), m_StreamingOptions.nBuf * 2);
	SetDlgItemText(hwndDlg, IDC_EDIT_BUFF, sz);
	wsprintf(sz, _T("%d"), m_StreamingOptions.nPreBuf * 2);
	SetDlgItemText(hwndDlg, IDC_EDIT_PREBUFF, sz);
	SetDlgItemText(hwndDlg, IDC_EDIT_PROXY, m_StreamingOptions.szProxy);

	if (m_StreamingOptions.fUseProxy)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PROXY), BM_SETCHECK, 1, 0);
}

void COptions::StreamingPageOnDeltaPos(HWND hwndDlg, NM_UPDOWN* pNmud)
{
	TCHAR sz[32];
	int nEditID = pNmud->hdr.idFrom == IDC_SPIN_BUFF ? IDC_EDIT_BUFF : IDC_EDIT_PREBUFF;
	GetDlgItemText(hwndDlg, nEditID, sz, 32);

	int n = _tcstol(sz, NULL, 10);
	if (pNmud->iDelta < 0)
		n = min(n + 2, STREAMING_BUFF_MAX);
	else if (pNmud->iDelta > 0)
		n = max(n - 2, STREAMING_BUFF_MIN);
	wsprintf(sz, _T("%d"), n);
	SetDlgItemText(hwndDlg, nEditID, sz);
}

void COptions::StreamingPageOnOK(HWND hwndDlg)
{
	TCHAR sz[MAX_URL];
	GetDlgItemText(hwndDlg, IDC_EDIT_BUFF, sz, MAX_URL);
	int n = _tcstol(sz, NULL, 10);
	if (n >= STREAMING_BUFF_MIN) {
		n = max(min(n, STREAMING_BUFF_MAX), STREAMING_BUFF_MIN);
		m_StreamingOptions.nBuf = n / 2;
	}
	GetDlgItemText(hwndDlg, IDC_EDIT_PREBUFF, sz, MAX_URL);
	n = _tcstol(sz, NULL, 10);
	if (n >= STREAMING_BUFF_MIN) {
		n = max(min(n, STREAMING_BUFF_MAX), STREAMING_BUFF_MIN);
		m_StreamingOptions.nPreBuf = n / 2;
	}
	GetDlgItemText(hwndDlg, IDC_EDIT_PROXY, m_StreamingOptions.szProxy, MAX_URL);
	m_StreamingOptions.fUseProxy = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PROXY), BM_GETCHECK, 0, 0);
}

BOOL CALLBACK COptions::SkinPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (COptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->SkinPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BROWSE:
			pOptions->SkinPageOnBrowse(hwndDlg);
			return TRUE;
		}
		return FALSE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->SkinPageOnOK(hwndDlg);
				return TRUE;
			}
			return FALSE;
		}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	default:
		return FALSE;
	}
}

void COptions::SkinPageOnInitDialog(HWND hwndDlg)
{
	SetDlgItemText(hwndDlg, IDC_EDIT_FILE, m_szSkinFile);
}

void COptions::SkinPageOnBrowse(HWND hwndDlg)
{
	TCHAR szFile[MAX_PATH] = {0};
	CTempStr strTitle(IDS_TITLE_SKIN_FILE);
	CTempStr strFilter(IDS_FILTER_SKIN_FILE);

	OPENFILENAME ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner   = hwndDlg;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = MAX_PATH;	
	ofn.lpstrTitle  = strTitle;
	ofn.lpstrFilter = strFilter;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrInitialDir = m_szLastDir;
#ifdef _WIN32_WCE
	BOOL (*gsGetOpenFileName)(OPENFILENAME*) = NULL;
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		(FARPROC&)gsGetOpenFileName = GetProcAddress(hInst, _T("gsGetOpenFileName"));
	}

	if (gsGetOpenFileName) {
		if (!gsGetOpenFileName(&ofn)) return;
	}
	else {
		if (!GetOpenFileName(&ofn)) return;
	}

	if (hInst) FreeLibrary(hInst);
#else
	if (!GetOpenFileName(&ofn)) return;
#endif

	SetDlgItemText(hwndDlg, IDC_EDIT_FILE, szFile);

	LPTSTR psz = _tcsrchr(szFile, _T('\\'));
	if (psz) *psz = NULL;
	_tcscpy(m_szLastDir, szFile);
}

void COptions::SkinPageOnOK(HWND hwndDlg)
{
	GetDlgItemText(hwndDlg, IDC_EDIT_FILE, m_szSkinFile, MAX_PATH);
}

BOOL CALLBACK COptions::AssociatePageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (COptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->AssociatePageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->AssociatePageOnOK(hwndDlg);
				return TRUE;
			}
			return FALSE;
		}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_CHECK_ALL) {
			HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_EXT);
			for (int i = 0; i < ListView_GetItemCount(hwndLV); i++) {
				ListView_SetCheckState(hwndLV, i , TRUE);
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_CLEAR_ALL) {
			HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_EXT);
			for (int i = 0; i < ListView_GetItemCount(hwndLV); i++) {
				ListView_SetCheckState(hwndLV, i , FALSE);
			}
			return TRUE;
		}
		return FALSE;
#ifdef _WIN32_WCE_PPC
	case WM_SIZE:
		{
			RECT rc;
			HWND hwnd;
			POINT pt;

#define BUTTON_HEIGHT	20
#define BUTTON_MARGIN	2
			hwnd = GetDlgItem(hwndDlg, IDC_LIST_EXT);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hwndDlg, &pt);
			MoveWindow(hwnd, pt.x, pt.y, LOWORD(lParam) - pt.x * 2, 
				HIWORD(lParam) - pt.y - (BUTTON_HEIGHT + BUTTON_MARGIN * 2), TRUE);

			GetWindowRect(hwnd, &rc);
			SendMessage(hwnd, LVM_SETCOLUMNWIDTH, 0, 
				MAKELPARAM(RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXBORDER) * 2, 0));

			hwnd = GetDlgItem(hwndDlg, IDC_CHECK_ALL);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hwndDlg, &pt);
			MoveWindow(hwnd, pt.x, HIWORD(lParam) - (BUTTON_HEIGHT + BUTTON_MARGIN), RECT_WIDTH(&rc), BUTTON_HEIGHT, TRUE);

			hwnd = GetDlgItem(hwndDlg, IDC_CLEAR_ALL);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hwndDlg, &pt);
			MoveWindow(hwnd, pt.x, HIWORD(lParam) - (BUTTON_HEIGHT + BUTTON_MARGIN), RECT_WIDTH(&rc), BUTTON_HEIGHT, TRUE);

			return FALSE;
		}
#endif
	default:
		return FALSE;
	}
}

void COptions::AssociatePageOnInitDialog(HWND hwndDlg)
{
	RECT rc;
	LVCOLUMN lvc;
	BOOL fAssociate;
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_EXT);
	ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	GetClientRect(hwndLV, &rc);
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT; 
	lvc.pszText = NULL;
	lvc.iSubItem = 0;
	lvc.cx = RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - 1;
	ListView_InsertColumn(hwndLV, 0, &lvc);

	LVITEM lvi = {0};
	TCHAR szExt[MAX_PATH], szExtDesc[MAX_PATH], szFind[MAX_PATH];
	MAP_DEC_PLUGIN* pPlugIn;
	int nExt, nIndex;
	int nPlugIn = MAP_GetDecoderPlugInCount(m_hMap);

	// internal
	int i = 0;
	while (*s_szDefSupportedExt2[i] != NULL) {
		lvi.mask = LVIF_TEXT;
		lvi.pszText = (LPTSTR)s_szDefSupportedExt2[i];
		nIndex = ListView_InsertItem(hwndLV, &lvi);

		if (IsDefSupportedExt((LPTSTR)s_szDefSupportedExt2[i], FALSE))
			fAssociate = IsAssociatedFile((LPTSTR)s_szDefSupportedExt2[i], KEY_AUDIOFILE);
		else
			fAssociate = IsAssociatedFile((LPTSTR)s_szDefSupportedExt2[i], KEY_PLAYLIST);
		ListView_SetCheckState(hwndLV, nIndex, fAssociate);

		i++;
	}

	for (i = 0; i < nPlugIn; i++) {		
		pPlugIn = MAP_GetDecoderPlugIn(m_hMap, i);
		if (!pPlugIn)
			continue;

		nExt = pPlugIn->GetFileExtCount();
		for (int j = 0; j < nExt; j++) {
			if (pPlugIn->GetFileExt(j, szExt, szExtDesc) && _tcslen(szExt) && !IsDefSupportedExt(szExt, FALSE)) {
				BOOL fFind = FALSE;
				for (int k = 0; k < ListView_GetItemCount(hwndLV); k++) {
					lvi.mask = LVIF_TEXT;
					lvi.pszText = szFind;
					lvi.cchTextMax = MAX_PATH;
					lvi.iItem = k;
					if (ListView_GetItem(hwndLV, &lvi) && _tcsicmp(szFind, szExt) == 0) {
						fFind = TRUE;
						break;
					}
				}
				if (!fFind) {
					lvi.mask = LVIF_TEXT;
					lvi.pszText = szExt;
					nIndex = ListView_InsertItem(hwndLV, &lvi);

					fAssociate = IsAssociatedFile((LPTSTR)szExt, KEY_AUDIOFILE);
					ListView_SetCheckState(hwndLV, nIndex, fAssociate);
				}
			}
		}
	}
}

void COptions::AssociatePageOnOK(HWND hwndDlg)
{
	LVITEM lvi = {0};
	BOOL fPlayList, fAssociate;
	TCHAR szExt[MAX_PATH];
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_EXT);
	for (int i = 0; i < ListView_GetItemCount(hwndLV); i++) {
		lvi.mask = LVIF_TEXT;
		lvi.pszText = szExt;
		lvi.cchTextMax = MAX_PATH;
		lvi.iItem = i;
		ListView_GetItem(hwndLV, &lvi);

		fPlayList = IsDefSupportedExt(szExt) && !IsDefSupportedExt(szExt, FALSE);
		if (fPlayList)
			fAssociate = IsAssociatedFile(szExt, KEY_PLAYLIST);
		else
			fAssociate = IsAssociatedFile(szExt, KEY_AUDIOFILE);

		if (ListView_GetCheckState(hwndLV, i)) {
			if (!fAssociate) {
				if (fPlayList)
					AssociateFile(szExt, KEY_PLAYLIST, NAME_PLAYLIST);
				else
					AssociateFile(szExt, KEY_AUDIOFILE, NAME_AUDIOFILE);
			}
		}
		else {
			if (fAssociate) {
				if (fPlayList)
					UnassociateFile(szExt, KEY_PLAYLIST);
				else
					UnassociateFile(szExt, KEY_AUDIOFILE);
			}
		}
	}
}

int COptions::ShowLocationDlg(HWND hwndParent, LPTSTR pszLocation)
{
	m_pszLocation = pszLocation;
	int nRet = DialogBoxParam(GetInst(), (LPCTSTR)IDD_LOCATION_DLG, hwndParent, LocationDlgProc, (LPARAM)this);
	m_pszLocation = NULL;
	return nRet;
}

BOOL CALLBACK COptions::LocationDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		pOptions = (COptions*)lParam;
		pOptions->LocationDlgOnInitDialog(hwndDlg);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			pOptions->LocationDlgOnOK(hwndDlg);
			return TRUE;
		case IDCANCEL:
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
#ifdef _WIN32_WCE_PPC
	case WM_SIZE:
	{
		RECT rc;
		HWND hwnd;
		POINT pt;

		hwnd = GetDlgItem(hwndDlg, IDCANCEL);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd, LOWORD(lParam) - RECT_WIDTH(&rc) - 2, pt.y, RECT_WIDTH(&rc), RECT_HEIGHT(&rc), TRUE);

		hwnd = GetDlgItem(hwndDlg, IDC_CMB_LOCATION);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd, pt.x, pt.y, LOWORD(lParam) - pt.x * 2, RECT_HEIGHT(&rc), TRUE);
		return TRUE;
	}
#endif
	default:
		return FALSE;
	}
}

void COptions::LocationDlgOnInitDialog(HWND hwndDlg)
{
	ShellInitDialog(hwndDlg);
	HWND hwndCmb = GetDlgItem(hwndDlg, IDC_CMB_LOCATION);

	// ?ジスト?からURL?歴をﾃｯみ?み追加する
	HKEY hKey = 0;
	TCHAR szLocation[MAX_URL], szName[32];
	if (RegOpenKeyEx(HKEY_CURRENT_USER, KEY_LOCATION, 
						0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD dwType, dwSize;
		for (int i = 0; i < MAX_LOCATION; i++) {
			dwSize = sizeof(szLocation);
			wsprintf(szName, _T("%s%d"), NAME_LOCATION, i);
			if (RegQueryValueEx(hKey, szName, 0, &dwType, (LPBYTE)szLocation, &dwSize) == ERROR_SUCCESS) {
				SendMessage(hwndCmb, CB_ADDSTRING, 0, (LPARAM)szLocation);
			}
		}
		RegCloseKey(hKey);
	}
}

void COptions::LocationDlgOnOK(HWND hwndDlg)
{
	HWND hwndCmb = GetDlgItem(hwndDlg, IDC_CMB_LOCATION);
	int nIndex = SendMessage(hwndCmb, CB_GETCURSEL, 0, 0);
	if (nIndex == CB_ERR) {
		// 入力されたものを取ｱﾓする
		TCHAR szLocation[MAX_URL], szName[32];
		GetDlgItemText(hwndDlg, IDC_CMB_LOCATION, szLocation, MAX_URL);
		if (!_tcslen(szLocation))
			return;

		if (_tcsncmp(szLocation, HTTP_PREFIX, _tcslen(HTTP_PREFIX)) == 0)
			_tcscpy(m_pszLocation, szLocation);
		else
			wsprintf(m_pszLocation, _T("%s%s"), HTTP_PREFIX, szLocation);
	
		// ?歴?ストをﾊｶする
		HKEY hKey = 0;
		DWORD dwDisposition;
		if (RegCreateKeyEx(HKEY_CURRENT_USER, KEY_LOCATION, 0, NULL, 
			REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_READ, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {
			DWORD dwType, dwSize;
			for (int i = MAX_LOCATION; i >= 0; i--) {
				wsprintf(szName, _T("%s%d"), NAME_LOCATION, i - 1);
				if (RegQueryValueEx(hKey, szName, 0, &dwType, (LPBYTE)szLocation, &dwSize) == ERROR_SUCCESS) {
					wsprintf(szName, _T("%s%d"), NAME_LOCATION, i);
					RegSetValueEx(hKey, szName, 0, dwType, (LPBYTE)szLocation, dwSize);
				}
			}
			wsprintf(szName, _T("%s%d"), NAME_LOCATION, 0);
			RegSetValueEx(hKey, szName, 0, REG_SZ, (LPBYTE)m_pszLocation, (_tcslen(m_pszLocation) + 1) * sizeof(TCHAR));
		}
	}
	else {
		SendMessage(hwndCmb, CB_GETLBTEXT, nIndex, (LPARAM)m_pszLocation);
	}
	EndDialog(hwndDlg, IDOK);
}

void COptions::AssociateFile(LPCTSTR pszExt, LPCTSTR pszKey, LPCTSTR pszName, UINT uIconID)
{
	TCHAR szModule[MAX_PATH], szTemp[MAX_PATH];
	GetModuleFileName(GetInst(), szModule, MAX_PATH);

	TCHAR szExt[MAX_PATH];
	if (*pszExt == _T('.'))
		_tcscpy(szExt, pszExt);
	else {
		_tcscpy(szExt, _T("."));
		_tcscat(szExt, pszExt);
	}

	// フﾆﾍイ?の関連付け
	HKEY hKey, hKeySub;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CLASSES_ROOT, pszKey, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {		
		wsprintf(szTemp, _T("%s"), pszName);
		RegSetValueEx(hKey, NULL, 0, REG_SZ, (LPBYTE)szTemp, (_tcslen(szTemp) + 1) * sizeof(TCHAR));

		// DefaultIcon
		if (RegCreateKeyEx(hKey, _T("DefaultIcon"), 0, NULL, 
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeySub, &dwDisposition) == ERROR_SUCCESS) {
			wsprintf(szTemp, _T("%s,-%d"), szModule, uIconID);
			RegSetValueEx(hKeySub, NULL, 0, REG_SZ, (LPBYTE)szTemp, (_tcslen(szTemp) + 1) * sizeof(TCHAR));
			RegCloseKey(hKeySub);
		}

		// Shell-Open-Command
		if (RegCreateKeyEx(hKey, _T("Shell\\Open\\Command"), 0, NULL, 
			REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKeySub, &dwDisposition) == ERROR_SUCCESS) {
			wsprintf(szTemp, _T("\"%s\" \"%%1\""), szModule);
			RegSetValueEx(hKeySub, NULL, 0, REG_SZ, (LPBYTE)szTemp, (_tcslen(szTemp) + 1) * sizeof(TCHAR));
			RegCloseKey(hKeySub);
		}
		RegCloseKey(hKey);

		if (RegCreateKeyEx(HKEY_CLASSES_ROOT, szExt, 0, NULL, 
			REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {
		
			TCHAR szBackup[MAX_PATH] = _T("");
			DWORD dwType, dwSize = sizeof(szBackup);
			RegQueryValueEx(hKey, NULL, 0, &dwType, (LPBYTE)szBackup, &dwSize);	
			if (_tcscmp(szBackup, pszKey) != 0)
				RegSetValueEx(hKey, VALUE_BACKUP, 0, REG_SZ, (LPBYTE)szBackup, (_tcslen(szBackup) + 1) * sizeof(TCHAR));

			RegSetValueEx(hKey, NULL, 0, REG_SZ, (LPBYTE)pszKey, (_tcslen(pszKey) + 1) * sizeof(TCHAR));
			RegCloseKey(hKey);
		}
	}
}

void COptions::UnassociateFile(LPCTSTR pszExt, LPCTSTR pszKey)
{
	HKEY hKey;

	TCHAR szExt[MAX_PATH];
	if (*pszExt == _T('.'))
		_tcscpy(szExt, pszExt);
	else {
		_tcscpy(szExt, _T("."));
		_tcscat(szExt, pszExt);
	}

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExt, 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
		TCHAR szBackup[MAX_PATH] = _T("");
		TCHAR szValue[MAX_PATH] = _T("");

		DWORD dwType, dwSize = sizeof(szBackup);
		RegQueryValueEx(hKey, VALUE_BACKUP, 0, &dwType, (LPBYTE)szBackup, &dwSize);	

		dwSize = sizeof(szValue);
		RegQueryValueEx(hKey, NULL, 0, &dwType, (LPBYTE)szValue, &dwSize);	
		if (_tcscmp(szValue, pszKey) == 0)
			RegSetValueEx(hKey, NULL, 0, REG_SZ, (LPBYTE)szBackup, (_tcslen(szBackup) + 1) * sizeof(TCHAR));

		RegDeleteValue(hKey, VALUE_BACKUP);
		RegCloseKey(hKey);
	}
}

BOOL COptions::IsAssociatedFile(LPCTSTR pszExt, LPCTSTR pszKey)
{
	HKEY hKey;

	TCHAR szExt[MAX_PATH];
	if (*pszExt == _T('.'))
		_tcscpy(szExt, pszExt);
	else {
		_tcscpy(szExt, _T("."));
		_tcscat(szExt, pszExt);
	}

	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExt, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;

	TCHAR szValue[MAX_PATH] = _T("");
	DWORD dwType, dwSize = sizeof(szValue);
	RegQueryValueEx(hKey, NULL, 0, &dwType, (LPBYTE)szValue, &dwSize);
	RegCloseKey(hKey);

	if (_tcscmp(szValue, pszKey) != 0)
		return FALSE;

	return TRUE;
}

BOOL COptions::ShowSleepTimerDlg(HWND hwndParent)
{
	return DialogBoxParam(GetInst(), (LPCTSTR)IDD_SLEEPTIMER_DLG, 
						hwndParent, SleepTimerDlgProc, (LPARAM)this) == IDOK ? TRUE : FALSE;
}

BOOL CALLBACK COptions::SleepTimerDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		pOptions = (COptions*)lParam;
		pOptions->SleepTimerDlgOnInitDialog(hwndDlg);
		return TRUE;
	case WM_COMMAND:
		return pOptions->SleepTimerDlgOnCommand(hwndDlg, wParam, lParam);
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == UDN_DELTAPOS) {
				pOptions->SleepTimerDlgOnDeltaPos(hwndDlg, (NM_UPDOWN*)pnmh);
				return TRUE;
			}
			return FALSE;
		}
	default:
		return FALSE;
	}
}

void COptions::SleepTimerDlgOnInitDialog(HWND hwndDlg)
{
	TCHAR sz[32];

	SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_TIMER), EM_LIMITTEXT, 3, 0);
	wsprintf(sz, _T("%d"), m_nSleepMinutes);
	SetDlgItemText(hwndDlg, IDC_EDIT_TIMER, sz);

	if (m_nSleepTime != -1) {
		EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_TIMER), FALSE);
#ifdef _WIN32_WCE
		EnableWindow(GetDlgItem(hwndDlg, IDC_CHECK_POWEROFF), FALSE);
#endif
		EnableWindow(GetDlgItem(hwndDlg, IDOK), FALSE);
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_TIMER), BM_SETCHECK, 1, 0);
	}

#ifdef _WIN32_WCE
	if (m_fSleepPowerOff)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_POWEROFF), BM_SETCHECK, 1, 0);
#endif
}

BOOL COptions::SleepTimerDlgOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	int n;
	TCHAR szBuf[32];

	switch (LOWORD(wParam)) {
	case IDCANCEL:
		EndDialog(hwndDlg, IDCANCEL);
		return TRUE;
	case IDOK:
		if (!IsWindowEnabled(GetDlgItem(hwndDlg, IDC_EDIT_TIMER)))
			return TRUE;

		GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT_TIMER), szBuf, 32);
		n = _tcstol(szBuf, 0, 10);
		if (n == 0) {
			SetFocus(GetDlgItem(hwndDlg, IDC_EDIT_TIMER));
			return TRUE;
		}
		m_nSleepMinutes = n;

		m_nSleepTime = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_TIMER), BM_GETCHECK, 0, 0) ? 0 : -1;
#ifdef _WIN32_WCE
		m_fSleepPowerOff = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_POWEROFF), BM_GETCHECK, 0, 0);
#endif
		EndDialog(hwndDlg, IDOK);
		return TRUE;
	case IDC_CHECK_TIMER:
		if (HIWORD(wParam) == BN_CLICKED) {
			if (!IsWindowEnabled(GetDlgItem(hwndDlg, IDC_EDIT_TIMER))) {
				EnableWindow(GetDlgItem(hwndDlg, IDC_EDIT_TIMER), TRUE);
#ifdef _WIN32_WCE
				EnableWindow(GetDlgItem(hwndDlg, IDC_CHECK_POWEROFF), TRUE);
#endif
				EnableWindow(GetDlgItem(hwndDlg, IDOK), TRUE);
			}
		}
		return FALSE;	
	}
	return FALSE;
}

void COptions::SleepTimerDlgOnDeltaPos(HWND hwndDlg, NM_UPDOWN* pNmud)
{
	TCHAR sz[32];
	GetDlgItemText(hwndDlg, IDC_EDIT_TIMER, sz, 32);

	int n = _tcstol(sz, NULL, 10);
	if (pNmud->iDelta < 0)
		n = min(n + 1, 999);
	else if (pNmud->iDelta > 0)
		n = max(n - 1, 1);
	wsprintf(sz, _T("%d"), n);
	SetDlgItemText(hwndDlg, IDC_EDIT_TIMER, sz);
}

BOOL CALLBACK COptions::PlugInPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static COptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (COptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->PlugInPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->PlugInPageOnOK(hwndDlg);
				return TRUE;
			}
			if (pnmh->code == NM_DBLCLK) {
				pOptions->PlugInPageOnCommand(hwndDlg, IDC_CONFIG, 0);
				return TRUE;
			}
			else if (pnmh->code == LVN_ITEMCHANGED) {
				EnableWindow(GetDlgItem(hwndDlg, IDC_CONFIG), 
					ListView_GetSelectedCount(GetDlgItem(hwndDlg, IDC_LIST_PLUGIN)) ? TRUE : FALSE);
			}
			return FALSE;
		}
	case WM_COMMAND:
		return pOptions->PlugInPageOnCommand(hwndDlg, wParam, lParam);
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
#ifdef _WIN32_WCE_PPC
	case WM_SIZE:
		{
			RECT rc;
			HWND hwnd;
			POINT pt;

#define BUTTON_HEIGHT	20
#define BUTTON_MARGIN	2
			hwnd = GetDlgItem(hwndDlg, IDC_LIST_PLUGIN);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hwndDlg, &pt);
			MoveWindow(hwnd, pt.x, pt.y, LOWORD(lParam) - pt.x * 2, 
				HIWORD(lParam) - pt.y - (BUTTON_HEIGHT + BUTTON_MARGIN * 2), TRUE);

			GetWindowRect(hwnd, &rc);
			SendMessage(hwnd, LVM_SETCOLUMNWIDTH, 0, 
				MAKELPARAM(RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXBORDER) * 2, 0));

			hwnd = GetDlgItem(hwndDlg, IDC_CONFIG);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hwndDlg, &pt);
			MoveWindow(hwnd, pt.x, HIWORD(lParam) - (BUTTON_HEIGHT + BUTTON_MARGIN), RECT_WIDTH(&rc), BUTTON_HEIGHT, TRUE);

			return FALSE;
		}
#endif
	default:
		return FALSE;
	}
}

void COptions::PlugInPageOnInitDialog(HWND hwndDlg)
{
	RECT rc;
	LVCOLUMN lvc;
	LVITEM lvi = {0};
	TCHAR szName[MAX_PATH];
	MAP_DEC_PLUGIN* pPlugIn;
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_PLUGIN);
	ListView_SetExtendedListViewStyle(hwndLV, LVS_EX_FULLROWSELECT);

	GetClientRect(hwndLV, &rc);
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT; 
	lvc.pszText = NULL;
	lvc.iSubItem = 0;
	lvc.cx = RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - 1;
	ListView_InsertColumn(hwndLV, 0, &lvc);

	int nIndex;
	int nPlugIn = MAP_GetDecoderPlugInCount(m_hMap);

	for (int i = 0; i < nPlugIn; i++) {		
		pPlugIn = MAP_GetDecoderPlugIn(m_hMap, i);
		if (!pPlugIn)
			continue;

		pPlugIn->GetPluginName(szName);
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.pszText = szName;
		lvi.lParam = i;
		lvi.iItem = ListView_GetItemCount(hwndLV);
		nIndex = ListView_InsertItem(hwndLV, &lvi);
	}

	EnableWindow(GetDlgItem(hwndDlg, IDC_CONFIG), FALSE);
}

void COptions::PlugInPageOnOK(HWND hwndDlg)
{
}

BOOL COptions::PlugInPageOnCommand(HWND hwndDlg, WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == IDC_CONFIG) {
		LVITEM lvi = {0};
		MAP_DEC_PLUGIN* pPlugIn;
		HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_PLUGIN);

		for (int i = 0; i < ListView_GetItemCount(hwndLV); i++) {
			if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED) == LVIS_SELECTED) {
				lvi.mask = LVIF_PARAM;
				lvi.iItem = i;
				ListView_GetItem(hwndLV, &lvi);
			
				pPlugIn = MAP_GetDecoderPlugIn(m_hMap, lvi.lParam);
				if (pPlugIn) pPlugIn->ShowConfigDlg(hwndDlg);
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}