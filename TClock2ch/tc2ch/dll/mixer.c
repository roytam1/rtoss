/*-------------------------------------------------------------
  mixer.c

  窓プログラミング　http://hp.vector.co.jp/authors/VA016117/
  Asroc=Launcherより。
---------------------------------------------------------------*/

#include "tcdll.h"
#include <mmsystem.h>

BOOL InitMasterVolumeControl(void);
BOOL InitMasterMuteControl(void);

#define	MAXCHANNEL	10		//チャンネルの最大数（左右だけなら２チャンネル）

HMIXER		g_hMixer=NULL;
MIXERCAPS	g_mxcaps;
DWORD		g_dwMinimum,g_dwMaximum;
DWORD		g_cChannels;
DWORD		g_dwVolumeControlID=(DWORD)-1;
DWORD		g_dwMuteControlID=(DWORD)-1;

typedef struct{
	MIXERCONTROLDETAILS_UNSIGNED	Volume[MAXCHANNEL];
	DWORD							Max;
} LASTVOLINFO,*LPLASTVOLINFO;

LASTVOLINFO	g_LastVolInfo;

BOOL InitMixer(void)
{
	if(g_hMixer) return TRUE;
	if(mixerOpen(&g_hMixer,0,0,0,MIXER_OBJECTF_MIXER)!=MMSYSERR_NOERROR) return FALSE;
	if(mixerGetDevCaps((UINT)g_hMixer, &g_mxcaps, sizeof(MIXERCAPS))!=MMSYSERR_NOERROR) return FALSE;
	return	TRUE;
}

void ReleaseMixer(void)
{
	if(g_hMixer) mixerClose(g_hMixer);
	g_hMixer=NULL;
	g_dwVolumeControlID=(DWORD)-1;
	g_dwMuteControlID=(DWORD)-1;
}

//-----------------------------------------------------------------------------
//             　　　　　　　　　　　　ボリューム
//-----------------------------------------------------------------------------

BOOL InitMasterVolumeControl(void)
{
	int	i, volComponentType;
	DWORD dwVolComponentType;
	MIXERLINE mxl;
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;

	if(g_dwVolumeControlID != -1) return TRUE;
	if(InitMixer() == FALSE) return FALSE;

		// 調節先切り替え
	volComponentType = GetMyRegLong("", "VolComponentType", 4);
	if ( volComponentType >= 0 && volComponentType <= 8 )
		dwVolComponentType = MIXERLINE_COMPONENTTYPE_DST_FIRST + volComponentType;
	else if ( volComponentType >= 9 && volComponentType <= 19 )
		dwVolComponentType = MIXERLINE_COMPONENTTYPE_SRC_FIRST + volComponentType - 9;
	else
		dwVolComponentType = MIXERLINE_COMPONENTTYPE_DST_FIRST + 4;

	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = dwVolComponentType;
	if(mixerGetLineInfo((HMIXEROBJ)g_hMixer,&mxl,
				MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE)!=MMSYSERR_NOERROR) return FALSE;

	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if(mixerGetLineControls((HMIXEROBJ)g_hMixer,&mxlc,
			   MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) return FALSE;

	g_cChannels = (mxl.cChannels > MAXCHANNEL)? MAXCHANNEL : mxl.cChannels;
	g_dwMinimum = mxc.Bounds.dwMinimum;
	g_dwMaximum = mxc.Bounds.dwMaximum;
	g_dwVolumeControlID = mxc.dwControlID;

	for(i = 0; i < MAXCHANNEL; i++) g_LastVolInfo.Volume[i].dwValue = 1;
	g_LastVolInfo.Max = 1;

	return TRUE;
}

BOOL GetMasterVolume(int *Val)
{
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;

	*Val = 0;
	if(InitMasterVolumeControl() == FALSE) return FALSE;

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = g_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if(mixerGetControlDetails((HMIXEROBJ)g_hMixer,&mxcd,
				 MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR) return FALSE;

	*Val = ((mxcdVolume.dwValue-g_dwMinimum)*100 + (g_dwMaximum-g_dwMinimum)/2)/(g_dwMaximum-g_dwMinimum);

	return TRUE;
}

//-----------------------------------------------------------------------------
//             　　　　　　　　　　　　ミュート
//-----------------------------------------------------------------------------

BOOL InitMasterMuteControl()
{
	MIXERLINE mxl;
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;

	if(g_dwMuteControlID != -1) return TRUE;
	if(InitMixer() == FALSE) return FALSE;

	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	if(mixerGetLineInfo((HMIXEROBJ)g_hMixer,&mxl,
				MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE)!=MMSYSERR_NOERROR) return FALSE;

	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if(mixerGetLineControls((HMIXEROBJ)g_hMixer,&mxlc,
			   MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) return FALSE;

	g_dwMuteControlID = mxc.dwControlID;

	return TRUE;
}

BOOL GetMasterMute(BOOL *Val)
{
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	MIXERCONTROLDETAILS mxcd;

	*Val = 0;
	if(InitMasterMuteControl() == FALSE) return FALSE;

	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = g_dwMuteControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxcd.paDetails = &mxcdMute;
	if (mixerGetControlDetails((HMIXEROBJ)g_hMixer,&mxcd,
				 MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR) return FALSE;

	*Val = mxcdMute.fValue;

	return TRUE;
}

