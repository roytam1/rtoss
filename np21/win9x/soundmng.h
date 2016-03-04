/**
 * @file	soundmng.h
 * @brief	サウンド マネージャの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * PCM 番号
 */
enum SoundPCMNumber
{
	SOUND_PCMSEEK		= 0,		/*!< ヘッド移動 */
	SOUND_PCMSEEK1					/*!< 1クラスタ移動 */
};

#ifdef __cplusplus
extern "C"
{
#endif

UINT soundmng_create(UINT rate, UINT ms);
void soundmng_destroy(void);
void soundmng_reset(void);
void soundmng_play(void);
void soundmng_stop(void);
void soundmng_sync(void);
void soundmng_setreverse(BOOL bReverse);

BRESULT soundmng_pcmplay(enum SoundPCMNumber nNum, BOOL bLoop);
void soundmng_pcmstop(enum SoundPCMNumber nNum);

#ifdef __cplusplus
}



// ---- for windows

BRESULT soundmng_initialize(void);
void soundmng_deinitialize(void);

void soundmng_pcmload(SoundPCMNumber nNum, LPCTSTR lpFilename);
void soundmng_pcmvolume(SoundPCMNumber nNum, int nVolume);

/**
 * サウンド プロシージャ
 */
enum SoundProc
{
	SNDPROC_MASTER		= 0,
	SNDPROC_MAIN,
	SNDPROC_TOOL,
	SNDPROC_SUBWIND
};

void soundmng_enable(SoundProc nProc);
void soundmng_disable(SoundProc nProc);

#endif
