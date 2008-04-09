#include "windows.h"
#include "PpcDisplay.h"

#define QUERYESCSUPPORT		8 
#define GETVFRAMEPHYSICAL	6144
#define GETVFRAMELEN		6145
#define DBGDRIVERSTAT		6146
#define SETPOWERMANAGEMENT	6147
#define GETPOWERMANAGEMENT	6148

typedef enum _VIDEO_POWER_STATE {
    VideoPowerOn = 1,
    VideoPowerStandBy,
    VideoPowerSuspend,
    VideoPowerOff
} VIDEO_POWER_STATE, *PVIDEO_POWER_STATE;

typedef struct _VIDEO_POWER_MANAGEMENT {
    ULONG Length;
    ULONG DPMSVersion;
    ULONG PowerState;
} VIDEO_POWER_MANAGEMENT, *PVIDEO_POWER_MANAGEMENT;

#if 0 // old versions

void ToggleDisplay()
{
#ifdef _WIN32_WCE_EMULATION
	return;
#endif

 	// check
	HDC hDC = ::GetDC(NULL);
	int nESC = SETPOWERMANAGEMENT;
	if (!ExtEscape(hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&nESC, 0, NULL)) {
		ReleaseDC(NULL, hDC);
		return; // cannot tern off;
	}

	// get current status 
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = VideoPowerOn;

	ExtEscape(hDC, GETPOWERMANAGEMENT, 0, NULL, vpm.Length, (LPSTR) &vpm);

	vpm.PowerState = (vpm.PowerState == VideoPowerOn) ?
								VideoPowerOff : VideoPowerOn;

	ExtEscape(hDC, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL);
	ReleaseDC(NULL, hDC);
}

BOOL GetDisplayStatus()
{
#ifdef _WIN32_WCE_EMULATION
	return TRUE;
#endif

 	// check
	HDC hDC = ::GetDC(NULL);
	int nESC = SETPOWERMANAGEMENT;
	if (!ExtEscape(hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&nESC, 0, NULL)) {
		ReleaseDC(NULL, hDC);
		return TRUE; // cannot tern off;
	}

	// get current status 
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = VideoPowerOn;

	ExtEscape(hDC, GETPOWERMANAGEMENT, 0, NULL, vpm.Length, (LPSTR) &vpm);
	ReleaseDC(NULL, hDC);

	return (vpm.PowerState == VideoPowerOn);
}

void TurnOnDisplay()
{
#ifdef _WIN32_WCE_EMULATION
	return;
#endif

	// check
	HDC hDC = ::GetDC(NULL);
	int nESC = SETPOWERMANAGEMENT;
	if (!ExtEscape(hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&nESC, 0, NULL)) {
		ReleaseDC(NULL, hDC);
		return; // cannot tern off;
	}

	// get current status 
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = VideoPowerOn;

	ExtEscape(hDC, GETPOWERMANAGEMENT, 0, NULL, vpm.Length, (LPSTR) &vpm);
	if (vpm.PowerState == VideoPowerOn) {
		ReleaseDC(NULL, hDC);
		return;
	}							
	vpm.PowerState = VideoPowerOn;
	ExtEscape(hDC, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL);
	ReleaseDC(NULL, hDC);
}

void TurnOffDisplay()
{
#ifdef _WIN32_WCE_EMULATION
	return;
#endif

	// check
	HDC hDC = ::GetDC(NULL);
	int nESC = SETPOWERMANAGEMENT;
	if (!ExtEscape(hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&nESC, 0, NULL)) {
		ReleaseDC(NULL, hDC);
		return; // cannot tern off;
	}

	// get current status 
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = VideoPowerOn;

	ExtEscape(hDC, GETPOWERMANAGEMENT, 0, NULL, vpm.Length, (LPSTR) &vpm);
	if (vpm.PowerState != VideoPowerOn) {
		ReleaseDC(NULL, hDC);
		return;
	}							
	vpm.PowerState = VideoPowerOff;
	ExtEscape(hDC, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL);
	ReleaseDC(NULL, hDC);
}

#else
// new version (no checking)
// GETPOWERMANAGEMENTがまともに実装されていないE-3000や
// ボタンを押すと勝手にディスプレイがONになるiPAQ h1937 (PocketPC2003)対策

ULONG g_fPowerState = VideoPowerOn;

void ToggleDisplay()
{
#ifdef _WIN32_WCE_EMULATION
	return;
#endif

	// iPAQ h1937 (PocketPC2003)はボタンを押すと勝手にディスプレイがなるため
	// 前回のステータスを元に切り替える

	HDC hDC = ::GetDC(NULL);
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = (g_fPowerState == VideoPowerOn) ?
								VideoPowerOff : VideoPowerOn;
	
	if (ExtEscape(hDC, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL) > 0)
		g_fPowerState = vpm.PowerState;
	ReleaseDC(NULL, hDC);
}

BOOL GetDisplayStatus()
{
#ifdef _WIN32_WCE_EMULATION
	return TRUE;
#endif
	// check
	HDC hDC = ::GetDC(NULL);
	
	// get current status 
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = 0;

	ExtEscape(hDC, GETPOWERMANAGEMENT, 0, NULL, vpm.Length, (LPSTR) &vpm);
	ReleaseDC(NULL, hDC);

	if (vpm.PowerState == 0)
		g_fPowerState = vpm.Length; // E-3000はvpm.Lengthに値を格納する (bug!!)
	else
		g_fPowerState = vpm.PowerState;

	return (g_fPowerState == VideoPowerOn);
}

void TurnOnDisplay()
{
#ifdef _WIN32_WCE_EMULATION
	return;
#endif

	HDC hDC = ::GetDC(NULL);
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = VideoPowerOn;
	if (ExtEscape(hDC, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL) > 0)
		g_fPowerState = vpm.PowerState;
	ReleaseDC(NULL, hDC);
}

void TurnOffDisplay()
{
#ifdef _WIN32_WCE_EMULATION
	return;
#endif

	HDC hDC = ::GetDC(NULL);
	VIDEO_POWER_MANAGEMENT vpm;
	vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
	vpm.DPMSVersion = 0x0001;
	vpm.PowerState = VideoPowerOff;
	if (ExtEscape(hDC, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL) > 0)
		g_fPowerState = vpm.PowerState;
	ReleaseDC(NULL, hDC);
}

#endif