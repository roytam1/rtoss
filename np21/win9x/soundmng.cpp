/**
 * @file	soundmng.cpp
 * @brief	Sound Manager (DirectSound3)
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#include <map>
#include <dsound.h>
#include "parts.h"
#include "wavefile.h"
#include "np2.h"
#include "soundmng.h"
#include "misc\extrom.h"
#include "sound.h"
#if defined(SUPPORT_ROMEO)
#include "ext\externalchipmanager.h"
#endif
#if defined(VERMOUTH_LIB)
#include "sound\vermouth\vermouth.h"
#endif
#if defined(MT32SOUND_DLL)
#include "ext\mt32snd.h"
#endif

#if !defined(__GNUC__)
#pragma comment(lib, "dsound.lib")
#endif	// !defined(__GNUC__)

#if defined(_M_IA64) || defined(_M_AMD64)
#define SOUNDBUFFERALIGN	(1 << 3)					/*!< バッファ アライメント */
#else
#define SOUNDBUFFERALIGN	(1 << 2)					/*!< バッファ アライメント */
#endif

#if !defined(_WIN64)
#ifdef __cplusplus
extern "C"
{
#endif
/**
 * satuation
 * @param[out] dst 出力バッファ
 * @param[in] src 入力バッファ
 * @param[in] size サイズ
 */
void __fastcall satuation_s16mmx(SINT16 *dst, const SINT32 *src, UINT size);
#ifdef __cplusplus
}
#endif
#endif

#if 1
#define DSBUFFERDESC_SIZE	20							/*!< DirectX3 Structsize */
#else
#define DSBUFFERDESC_SIZE	sizeof(DSBUFFERDESC)		/*!< DSBUFFERDESC Structsize */
#endif

#ifndef DSBVOLUME_MAX
#define DSBVOLUME_MAX		0							/*!< ヴォリューム最大値 */
#endif
#ifndef DSBVOLUME_MIN
#define DSBVOLUME_MIN		(-10000)					/*!< ヴォリューム最小値 */
#endif

/**
 * @brief Direct Sound3 クラス
 */
class CDSound3
{
public:
	static CDSound3* GetInstance();

	CDSound3();
	bool Initialize(HWND hWnd);
	void Deinitialize();

	UINT CreateStream(UINT rate, UINT ms);
	void ResetStream();
	void DestroyStream();
	void PlayStream();
	void StopStream();
	void Sync();
	void SetReverse(bool bReverse);
	void LoadPCM(SoundPCMNumber nNum, LPCTSTR lpFilename);
	void UnloadPCM(SoundPCMNumber nNum);
	void SetPCMVolume(SoundPCMNumber nNum, int nVolume);
	bool PlayPCM(SoundPCMNumber nNum, DWORD dwFlags);
	void StopPCM(SoundPCMNumber nNum);
	void EnableSound(SoundProc nProc);
	void DisableSound(SoundProc nProc);

private:
	static CDSound3 sm_instance;									//!< 唯一のインスタンスです


	LPDIRECTSOUND m_lpDSound;										//!< Direct Sound インタフェイス
	UINT m_nMute;													//!< ミュート フラグ
	LPDIRECTSOUNDBUFFER m_lpDSStream;								//!< ストリーム バッファ
	UINT m_dwHalfBufferSize;										//!< バッファ サイズ
	int m_nStreamEvent;												//!< ストリーム イベント
	std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER> m_pcm;			//!< PCM バッファ

	typedef void (PARTSCALL * FNMIX)(SINT16*, const SINT32*, UINT);	//!< satuation関数型宣言
	FNMIX m_fnMix;													//!< satuation関数ポインタ

private:
	void EnableStream(bool bEnable);
	void FillStream(DWORD dwPosition);
	void DestroyAllPCM();
	void StopAllPCM();
	LPDIRECTSOUNDBUFFER CreateWaveBuffer(LPCTSTR lpFilename);
};

#if defined(VERMOUTH_LIB)
	MIDIMOD		vermouth_module = NULL;
#endif

//! 唯一のインスタンスです
CDSound3 CDSound3::sm_instance;

/**
 * インスタンスを得る
 * @return インスタンス
 */
CDSound3* CDSound3::GetInstance()
{
	return &sm_instance;
}

/**
 * コンストラクタ
 */
CDSound3::CDSound3()
	: m_lpDSound(NULL)
	, m_nMute(0)
	, m_lpDSStream(NULL)
	, m_dwHalfBufferSize(0)
	, m_nStreamEvent(-1)
{
}

/**
 * 初期化
 * @param[in] hWnd ウィンドウ ハンドル
 * @retval true 成功
 * @retval false 失敗
 */
bool CDSound3::Initialize(HWND hWnd)
{
	// DirectSoundの初期化
	LPDIRECTSOUND lpDSound;
	if (FAILED(DirectSoundCreate(0, &lpDSound, 0)))
	{
		return false;
	}
	if (FAILED(lpDSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
	{
		if (FAILED(lpDSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL)))
		{
			lpDSound->Release();
			return false;
		}
	}

	m_lpDSound = lpDSound;

#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->Initialize();
#endif
	return true;
}

/**
 * 解放
 */
void CDSound3::Deinitialize()
{
#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->Deinitialize();
#endif
	DestroyAllPCM();
	DestroyStream();

	RELEASE(m_lpDSound);
}

/**
 * サウンド有効
 * @param[in] nProc プロシージャ
 */
void CDSound3::EnableSound(SoundProc nProc)
{
	const UINT nBit = 1 << nProc;
	if (!(m_nMute & nBit))
	{
		return;
	}
	m_nMute &= ~nBit;
	if (!m_nMute)
	{
		ResetStream();
		EnableStream(true);
	}
}

/**
 * サウンド無効
 * @param[in] nProc プロシージャ
 */
void CDSound3::DisableSound(SoundProc nProc)
{
	if (!m_nMute)
	{
		EnableStream(false);
		StopAllPCM();
	}
	m_nMute |= (1 << nProc);
}

/**
 * ストリームを作成
 * @param[in] rate サンプリング レート
 * @param[in] ms バッファ長(ミリ秒)
 * @return バッファ数
 */
UINT CDSound3::CreateStream(UINT rate, UINT ms)
{
	if ((m_lpDSound == NULL) || ((rate != 11025) && (rate != 22050) && (rate != 44100)))
	{
		return 0;
	}

	if (ms < 40) {
		ms = 40;
	}
	else if (ms > 1000) {
		ms = 1000;
	}

	// キーボード表示のディレイ設定
//	keydispr_delayinit((UINT8)((ms * 10 + 563) / 564));

	UINT samples = (rate * ms) / 2000;
	samples = (samples + SOUNDBUFFERALIGN - 1) & (~(SOUNDBUFFERALIGN - 1));
	m_dwHalfBufferSize = samples * 2 * sizeof(SINT16);
	SetReverse(false);

	PCMWAVEFORMAT pcmwf;
	ZeroMemory(&pcmwf, sizeof(pcmwf));
	pcmwf.wf.wFormatTag = WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels = 2;
	pcmwf.wf.nSamplesPerSec = rate;
	pcmwf.wBitsPerSample = 16;
	pcmwf.wf.nBlockAlign = 2 * sizeof(SINT16);
	pcmwf.wf.nAvgBytesPerSec = rate * 2 * sizeof(SINT16);

	int i;
	for (i = 0; i < 2; i++)
	{
		DSBUFFERDESC dsbdesc;
		ZeroMemory(&dsbdesc, sizeof(dsbdesc));
		dsbdesc.dwSize = i ? sizeof(dsbdesc) : DSBUFFERDESC_SIZE;
		dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME |
						DSBCAPS_CTRLFREQUENCY |
						DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
		dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;
		dsbdesc.dwBufferBytes = m_dwHalfBufferSize * 2;
		if (SUCCEEDED(m_lpDSound->CreateSoundBuffer(&dsbdesc, &m_lpDSStream, NULL)))
		{
			break;
		}
	}
	if (i >= 2)
	{
		return 0;
	}

#if defined(VERMOUTH_LIB)
	vermouth_module = midimod_create(rate);
	midimod_loadall(vermouth_module);
#endif

#if defined(MT32SOUND_DLL)
	MT32Sound::GetInstance()->SetRate(rate);
#endif

	m_nStreamEvent = -1;
	return samples;
}

/**
 * ストリームをリセット
 */
void CDSound3::ResetStream()
{
	if (m_lpDSStream)
	{
		LPVOID lpBlock1;
		DWORD cbBlock1;
		LPVOID lpBlock2;
		DWORD cbBlock2;
		if (SUCCEEDED(m_lpDSStream->Lock(0, m_dwHalfBufferSize * 2, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0)))
		{
			ZeroMemory(lpBlock1, cbBlock1);
			if ((lpBlock2) && (cbBlock2))
			{
				ZeroMemory(lpBlock2, cbBlock2);
			}
			m_lpDSStream->Unlock(lpBlock1, cbBlock1, lpBlock2, cbBlock2);
			m_lpDSStream->SetCurrentPosition(0);
			m_nStreamEvent = -1;
		}
	}
}

/**
 * ストリームを破棄
 */
void CDSound3::DestroyStream()
{
	if (m_lpDSStream)
	{
#if defined(VERMOUTH_LIB)
		midimod_destroy(vermouth_module);
		vermouth_module = NULL;
#endif
#if defined(MT32SOUND_DLL)
		MT32Sound::GetInstance()->SetRate(0);
#endif
		m_lpDSStream->Stop();
		m_lpDSStream->Release();
		m_lpDSStream = NULL;
	}
}

/**
 * ストリーム有効設定
 * @param[in] bEnable 有効フラグ
 */
void CDSound3::EnableStream(bool bEnable)
{
	if (m_lpDSStream)
	{
		if (bEnable)
		{
			m_lpDSStream->Play(0, 0, DSBPLAY_LOOPING);
		}
		else
		{
			m_lpDSStream->Stop();
		}
	}

#if defined(SUPPORT_ROMEO)
	CExternalChipManager::GetInstance()->Mute(!bEnable);
#endif
}

/**
 * ストリームの再生
 */
void CDSound3::PlayStream()
{
	if (!m_nMute)
	{
		EnableStream(true);
	}
}

/**
 * ストリームの停止
 */
void CDSound3::StopStream()
{
	if (!m_nMute)
	{
		EnableStream(false);
	}
}

/**
 * ストリームを更新する
 * @param[in] dwPosition 更新位置
 */
void CDSound3::FillStream(DWORD dwPosition)
{
	const SINT32* lpSource= ::sound_pcmlock();

	LPVOID lpBlock1;
	DWORD cbBlock1;
	LPVOID lpBlock2;
	DWORD cbBlock2;
	HRESULT hr = m_lpDSStream->Lock(dwPosition, m_dwHalfBufferSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
	if (hr == DSERR_BUFFERLOST)
	{
		m_lpDSStream->Restore();
		hr = m_lpDSStream->Lock(dwPosition, m_dwHalfBufferSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
	}
	if (SUCCEEDED(hr))
	{
		if (lpSource)
		{
			(*m_fnMix)(static_cast<SINT16*>(lpBlock1), lpSource, cbBlock1);
		}
		else
		{
			ZeroMemory(lpBlock1, cbBlock1);
		}
		m_lpDSStream->Unlock(lpBlock1, cbBlock1, lpBlock2, cbBlock2);
	}
	::sound_pcmunlock(lpSource);
}

/**
 * 同期
 */
void CDSound3::Sync()
{
	if (m_lpDSStream)
	{
		DWORD dwCurrentPlayCursor;
		DWORD dwCurrentWriteCursor;
		if (m_lpDSStream->GetCurrentPosition(&dwCurrentPlayCursor, &dwCurrentWriteCursor) == DS_OK)
		{
			if (dwCurrentPlayCursor >= m_dwHalfBufferSize)
			{
				if (m_nStreamEvent != 0)
				{
					m_nStreamEvent = 0;
					FillStream(0);
				}
			}
			else
			{
				if (m_nStreamEvent != 1)
				{
					m_nStreamEvent = 1;
					FillStream(m_dwHalfBufferSize);
				}
			}
		}
	}
}

/**
 * パン反転を設定する
 * @param[in] bReverse 反転フラグ
 */
void CDSound3::SetReverse(bool bReverse)
{
	if (!bReverse)
	{
#if !defined(_WIN64)
		if (mmxflag)
		{
			m_fnMix = satuation_s16;
		}
		else {
			m_fnMix = satuation_s16mmx;
		}
#else
		m_fnMix = satuation_s16;
#endif
	}
	else
	{
		m_fnMix = satuation_s16x;
	}
}

/**
 * PCM バッファを破棄する
 */
void CDSound3::DestroyAllPCM()
{
	for (std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.begin(); it != m_pcm.begin(); ++it)
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->Stop();
		lpDSBuffer->Release();
	}
	m_pcm.clear();
}

/**
 * PCM をストップ
 */
void CDSound3::StopAllPCM()
{
	for (std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.begin(); it != m_pcm.begin(); ++it)
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->Stop();
	}
}

/**
 * PCM データ読み込み
 * @param[in] nNum PCM 番号
 * @param[in] lpFilename ファイル名
 */
void CDSound3::LoadPCM(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	UnloadPCM(nNum);

	LPDIRECTSOUNDBUFFER lpDSBuffer = CreateWaveBuffer(lpFilename);
	if (lpDSBuffer)
	{
		m_pcm[nNum] = lpDSBuffer;
	}
}

/**
 * PCM データ読み込み
 * @param[in] lpFilename ファイル名
 * @return バッファ
 */
LPDIRECTSOUNDBUFFER CDSound3::CreateWaveBuffer(LPCTSTR lpFilename)
{
	LPDIRECTSOUNDBUFFER lpDSBuffer = NULL;
	CExtRom extrom;

	do
	{
		if (!extrom.Open(lpFilename))
		{
			break;
		}

		RIFF_HEADER riff;
		if (extrom.Read(&riff, sizeof(riff)) != sizeof(riff))
		{
			break;
		}
		if ((riff.sig != WAVE_SIG('R','I','F','F')) || (riff.fmt != WAVE_SIG('W','A','V','E')))
		{
			break;
		}

		bool bValid = false;
		WAVE_HEADER whead;
		WAVE_INFOS info;
		UINT nSize = 0;
		while (true /*CONSTCOND*/)
		{
			if (extrom.Read(&whead, sizeof(whead)) != sizeof(whead))
			{
				bValid = false;
				break;
			}
			nSize = LOADINTELDWORD(whead.size);
			if (whead.sig == WAVE_SIG('f','m','t',' '))
			{
				if (nSize >= sizeof(info))
				{
					if (extrom.Read(&info, sizeof(info)) != sizeof(info))
					{
						bValid = false;
						break;
					}
					nSize -= sizeof(info);
					bValid = true;
				}
			}
			else if (whead.sig == WAVE_SIG('d','a','t','a'))
			{
				break;
			}
			if (nSize)
			{
				extrom.Seek(nSize, FILE_CURRENT);
			}
		}
		if (!bValid)
		{
			break;
		}

		PCMWAVEFORMAT pcmwf;
		ZeroMemory(&pcmwf, sizeof(pcmwf));
		pcmwf.wf.wFormatTag = LOADINTELWORD(info.format);
		if (pcmwf.wf.wFormatTag != 1)
		{
			break;
		}
		pcmwf.wf.nChannels = LOADINTELWORD(info.channel);
		pcmwf.wf.nSamplesPerSec = LOADINTELDWORD(info.rate);
		pcmwf.wBitsPerSample = LOADINTELWORD(info.bit);
		pcmwf.wf.nBlockAlign = LOADINTELWORD(info.block);
		pcmwf.wf.nAvgBytesPerSec = LOADINTELDWORD(info.rps);

		DSBUFFERDESC dsbdesc;
		ZeroMemory(&dsbdesc, sizeof(dsbdesc));
		dsbdesc.dwSize = DSBUFFERDESC_SIZE;
		dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC | DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
		dsbdesc.dwBufferBytes = nSize;
		dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)&pcmwf;

		if (FAILED(m_lpDSound->CreateSoundBuffer(&dsbdesc, &lpDSBuffer, NULL)))
		{
			break;
		}

		LPVOID lpBlock1;
		DWORD cbBlock1;
		LPVOID lpBlock2;
		DWORD cbBlock2;
		HRESULT hr = lpDSBuffer->Lock(0, nSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
		if (hr == DSERR_BUFFERLOST)
		{
			lpDSBuffer->Restore();
			hr = lpDSBuffer->Lock(0, nSize, &lpBlock1, &cbBlock1, &lpBlock2, &cbBlock2, 0);
		}
		if (FAILED(hr))
		{
			lpDSBuffer->Release();
			lpDSBuffer = NULL;
			break;
		}

		extrom.Read(lpBlock1, cbBlock1);
		if ((lpBlock2) && (cbBlock2))
		{
			extrom.Read(lpBlock2, cbBlock2);
		}
		lpDSBuffer->Unlock(lpBlock1, cbBlock1, lpBlock2, cbBlock2);
	} while (0 /*CONSTCOND*/);

	return lpDSBuffer;
}

/**
 * PCM をアンロード
 * @param[in] nNum PCM 番号
 */
void CDSound3::UnloadPCM(SoundPCMNumber nNum)
{
	std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		m_pcm.erase(it);

		lpDSBuffer->Stop();
		lpDSBuffer->Release();
	}
}

/**
 * PCM ヴォリューム設定
 * @param[in] nNum PCM 番号
 * @param[in] nVolume ヴォリューム
 */
void CDSound3::SetPCMVolume(SoundPCMNumber nNum, int nVolume)
{
	std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
	if (it != m_pcm.end())
	{
		LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
		lpDSBuffer->SetVolume((((DSBVOLUME_MAX - DSBVOLUME_MIN) * nVolume) / 100) + DSBVOLUME_MIN);
	}
}

/**
 * PCM 再生
 * @param[in] nNum PCM 番号
 * @param[in] dwFlags 再生フラグ
 * @retval true 成功
 * @retval false 失敗
 */
bool CDSound3::PlayPCM(SoundPCMNumber nNum, DWORD dwFlags)
{
	if (!m_nMute)
	{
		std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
		if (it != m_pcm.end())
		{
			LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
//			lpDSBuffer->SetCurrentPosition(0);
			lpDSBuffer->Play(0, 0, dwFlags);
			return true;
		}
	}
	return false;
}

/**
 * PCM 停止
 * @param[in] nNum PCM 番号
 */
void CDSound3::StopPCM(SoundPCMNumber nNum)
{
	if (!m_nMute)
	{
		std::map<SoundPCMNumber, LPDIRECTSOUNDBUFFER>::iterator it = m_pcm.find(nNum);
		if (it != m_pcm.end())
		{
			LPDIRECTSOUNDBUFFER lpDSBuffer = it->second;
			lpDSBuffer->Stop();
		}
	}
}

// ---- C ラッパー

/**
 * ストリーム作成
 * @param[in] rate サンプリング レート
 * @param[in] ms バッファ長(ミリ秒)
 * @return バッファ サイズ
 */
UINT soundmng_create(UINT rate, UINT ms)
{
	return CDSound3::GetInstance()->CreateStream(rate, ms);
}

/**
 * ストリーム リセット
 */
void soundmng_reset(void)
{
	CDSound3::GetInstance()->ResetStream();
}

/**
 * ストリーム破棄
 */
void soundmng_destroy(void)
{
	CDSound3::GetInstance()->DestroyStream();
}

/**
 * ストリーム開始
 */
void soundmng_play(void)
{
	CDSound3::GetInstance()->PlayStream();
}

/**
 * ストリーム停止
 */
void soundmng_stop(void)
{
	CDSound3::GetInstance()->StopStream();
}

/**
 * ストリーム同期
 */
void soundmng_sync(void)
{
	CDSound3::GetInstance()->Sync();
}

/**
 * ストリーム パン反転設定
 * @param[in] bReverse 反転
 */
void soundmng_setreverse(BOOL bReverse)
{
	CDSound3::GetInstance()->SetReverse((bReverse) ? true : false);
}

/**
 * PCM ロード
 * @param[in] nNum PCM 番号
 * @param[in] lpFilename ファイル名
 */
void soundmng_pcmload(SoundPCMNumber nNum, LPCTSTR lpFilename)
{
	CDSound3::GetInstance()->LoadPCM(nNum, lpFilename);
}

/**
 * PCM ヴォリューム設定
 * @param[in] nNum PCM 番号
 * @param[in] nVolume ヴォリューム
 */
void soundmng_pcmvolume(SoundPCMNumber nNum, int nVolume)
{
	CDSound3::GetInstance()->SetPCMVolume(nNum, nVolume);
}

/**
 * PCM 再生
 * @param[in] nNum PCM 番号
 * @param[in] bLoop ループ
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
BRESULT soundmng_pcmplay(enum SoundPCMNumber nNum, BOOL bLoop)
{
	return (CDSound3::GetInstance()->PlayPCM(nNum, (bLoop) ? DSBPLAY_LOOPING : 0)) ? SUCCESS : FAILURE;
}

/**
 * PCM 停止
 * @param[in] nNum PCM 番号
 */
void soundmng_pcmstop(enum SoundPCMNumber nNum)
{
	CDSound3::GetInstance()->StopPCM(nNum);
}

/**
 * 初期化
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
BRESULT soundmng_initialize(void)
{
#if defined(SUPPORT_ROMEO)
	CExternalChipManager::GetInstance()->Initialize();
#endif	// defined(SUPPORT_ROMEO)

	return (CDSound3::GetInstance()->Initialize(g_hWndMain)) ? SUCCESS : FAILURE;
}

/**
 * 解放
 */
void soundmng_deinitialize(void)
{
#if defined(SUPPORT_ROMEO)
	CExternalChipManager::GetInstance()->Deinitialize();
#endif	// defined(SUPPORT_ROMEO)

	CDSound3::GetInstance()->Deinitialize();
}

/**
 * サウンド有効
 * @param[in] nProc プロシージャ
 */
void soundmng_enable(SoundProc nProc)
{
	CDSound3::GetInstance()->EnableSound(nProc);
}

/**
 * サウンド無効
 * @param[in] nProc プロシージャ
 */
void soundmng_disable(SoundProc nProc)
{
	CDSound3::GetInstance()->DisableSound(nProc);
}
