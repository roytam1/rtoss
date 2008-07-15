#include "gspmidi.h"

extern TCHAR g_szConfigFile[MAX_PATH];
extern int g_nSampleRate;
extern int g_nBitsPerSample;
extern int g_nChannels;
extern int g_nVoices;
extern int g_nAmp;
extern BOOL g_fAdjustPanning;
extern BOOL g_fFreeInst;
extern BOOL g_fAntialiasing;

#define REG_KEY_GSPMIDI			_T("Software\\GreenSoftware\\GSPlayer\\Plug-ins\\gspmidi")
#define REG_NAME_CONFIGFILE		_T("ConfigFile")
#define REG_NAME_SAMPLERATE		_T("SampleRate")
#define REG_NAME_BITS			_T("Bits")
#define REG_NAME_STEREO			_T("Stereo")
#define REG_NAME_VOICES			_T("Voices")
#define REG_NAME_AMP			_T("Amp")
#define REG_NAME_ADJUSTPAN		_T("AdjustPanning")
#define REG_NAME_FREEINST		_T("FreeInst")
#define REG_NAME_ANTIALIAS		_T("Antialiasing")

void ReadRegistory()
{
	HKEY hKey;
	DWORD dwType, dwSize, dwValue;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_GSPMIDI, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return;

	dwSize = sizeof(int);
	RegQueryValueEx(hKey, REG_NAME_SAMPLERATE, 0, &dwType, (LPBYTE)&g_nSampleRate, &dwSize);
	RegQueryValueEx(hKey, REG_NAME_BITS, 0, &dwType, (LPBYTE)&g_nBitsPerSample, &dwSize);
	if (RegQueryValueEx(hKey, REG_NAME_STEREO, 0, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS) {
		g_nChannels = dwValue ? 2 : 1;
	}
	RegQueryValueEx(hKey, REG_NAME_VOICES, 0, &dwType, (LPBYTE)&g_nVoices, &dwSize);
	RegQueryValueEx(hKey, REG_NAME_AMP, 0, &dwType, (LPBYTE)&g_nAmp, &dwSize);
	RegQueryValueEx(hKey, REG_NAME_ADJUSTPAN, 0, &dwType, (LPBYTE)&g_fAdjustPanning, &dwSize);
	RegQueryValueEx(hKey, REG_NAME_FREEINST, 0, &dwType, (LPBYTE)&g_fFreeInst, &dwSize);
	RegQueryValueEx(hKey, REG_NAME_ANTIALIAS, 0, &dwType, (LPBYTE)&g_fAntialiasing, &dwSize);

	dwSize = sizeof(g_szConfigFile);
	RegQueryValueEx(hKey, REG_NAME_CONFIGFILE, 0, &dwType, (LPBYTE)&g_szConfigFile, &dwSize);

	RegCloseKey(hKey);
}

void WriteRegistory()
{
	HKEY hKey;
	DWORD dwDispos, dwValue;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, REG_KEY_GSPMIDI, 
			0, NULL, 0, KEY_WRITE, NULL, &hKey, &dwDispos) != ERROR_SUCCESS)
		return;

	RegSetValueEx(hKey, REG_NAME_SAMPLERATE, 0, REG_DWORD, (LPBYTE)&g_nSampleRate, sizeof(DWORD));
	RegSetValueEx(hKey, REG_NAME_BITS, 0, REG_DWORD, (LPBYTE)&g_nBitsPerSample, sizeof(DWORD));

	dwValue = (g_nChannels == 2) ? TRUE : FALSE;
	RegSetValueEx(hKey, REG_NAME_STEREO, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
	RegSetValueEx(hKey, REG_NAME_VOICES, 0, REG_DWORD, (LPBYTE)&g_nVoices, sizeof(DWORD));
	RegSetValueEx(hKey, REG_NAME_AMP, 0, REG_DWORD, (LPBYTE)&g_nAmp, sizeof(DWORD));
	RegSetValueEx(hKey, REG_NAME_ADJUSTPAN, 0, REG_DWORD, (LPBYTE)&g_fAdjustPanning, sizeof(DWORD));
	RegSetValueEx(hKey, REG_NAME_FREEINST, 0, REG_DWORD, (LPBYTE)&g_fFreeInst, sizeof(DWORD));
	RegSetValueEx(hKey, REG_NAME_ANTIALIAS, 0, REG_DWORD, (LPBYTE)&g_fAntialiasing, sizeof(DWORD));
	RegSetValueEx(hKey, REG_NAME_CONFIGFILE, 0, REG_SZ, 
				(LPBYTE)&g_szConfigFile, sizeof(TCHAR) * (_tcslen(g_szConfigFile) + 1));
}
