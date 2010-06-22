/*-------------------------------------------
  alarm.c
    Sound a wave, a media file, or open a file
    KAZUBON 1997-1999
---------------------------------------------*/

#include "tclock.h"

static PALARMSTRUCT pAS = NULL;
static PAUTOEXECSTRUCT pAES = NULL;
static int maxAlarm = 1;
static int maxAutoExec = 1;
static BOOL bJihou, bJihouRepeat, bJihouBlink, bBlink_j;
static int nJihouPre,nJihouTime;

static WAVEFORMATEX *pFormat = NULL;
static HWAVEOUT hWaveOut = NULL;
static HPSTR pData = NULL;
static WAVEHDR wh;

static BOOL bMCIPlaying = FALSE;
static int countPlay = 0, countPlayNum = 0;
static int nTrack;
static BOOL bTrack;
BOOL bPlayingNonstop = FALSE;
BOOL bOnBootTime = TRUE;

void PlayWavePre(HWND hwnd, char *fname);
BOOL PlayWave(HWND hwnd, char *fname, DWORD dwLoops);
int PlayMCI(HWND hwnd, int nt);
void StopWave(void);

/*------------------------------------------------
　アラーム/時報の初期化
--------------------------------------------------*/
void InitAlarm(void)
{
	char subkey[20];
	int i;

	if(pAS) free(pAS); pAS = NULL;
	if(pAES) free(pAES); pAES = NULL;

	maxAlarm = GetMyRegLong("", "AlarmNum", 0);
	if(maxAlarm < 1) maxAlarm = 0;
	if(maxAlarm > 0)
	{
		pAS = malloc(sizeof(ALARMSTRUCT) * maxAlarm);
		for(i = 0; i < maxAlarm; i++)
		{
			wsprintf(subkey, "Alarm%d", i + 1);

			GetMyRegStr(subkey, "Name", pAS[i].name, 40, "");
			pAS[i].bAlarm = GetMyRegLong(subkey, "Alarm", FALSE);
			pAS[i].hour = GetMyRegLong(subkey, "Hour", 12);
			pAS[i].minute = GetMyRegLong(subkey, "Minute", 0);
			GetMyRegStr(subkey, "File", pAS[i].fname, 1024, "");
			pAS[i].bHour12 = GetMyRegLong(subkey, "Hour12", TRUE);
			pAS[i].bRepeat = GetMyRegLong(subkey, "Repeat", FALSE);
			pAS[i].bBlink = GetMyRegLong(subkey, "Blink", FALSE);
			pAS[i].days = GetMyRegLong(subkey, "Days", 0x7f);
		}
	}
	maxAutoExec = GetMyRegLong("", "AutoExecNum", 0);
	if(maxAutoExec < 1) maxAutoExec = 0;
	if(maxAutoExec > 0)
	{
		pAES = malloc(sizeof(AUTOEXECSTRUCT) * maxAutoExec);
		for(i = 0; i < maxAutoExec; i++)
		{
			wsprintf(subkey, "AutoExec%d", i + 1);

			GetMyRegStr(subkey, "Name", pAES[i].name, 40, "");
			pAES[i].bAutoExec = GetMyRegLong(subkey, "AutoExec", FALSE);
			pAES[i].hour = GetMyRegLong(subkey, "Hour", 0xffffff);
			pAES[i].minute1 = GetMyRegLong(subkey, "Minute1", 0);
			pAES[i].minute2 = GetMyRegLong(subkey, "Minute2", 0);
			pAES[i].bOnBoot = GetMyRegLong(subkey, "OnBoot", FALSE);
			GetMyRegStr(subkey, "File", pAES[i].fname, 1024, "");
			pAES[i].days = GetMyRegLong(subkey, "Days", 0x7f);
		}
	}

	bJihou = GetMyRegLong("", "Jihou", FALSE);
	if(bJihou)
	{
		bJihouRepeat = GetMyRegLong("", "JihouRepeat", FALSE);
		bJihouBlink = GetMyRegLong("", "JihouBlink", FALSE);
		nJihouPre = GetMyRegLong("", "JihouPre", 0);
		nJihouTime= GetMyRegLong("", "JihouTime", 0);
	}
}

void EndAlarm(void)
{
	if(pAS) free(pAS); pAS = NULL;
	if(pAES) free(pAES); pAES = NULL;
	StopFile();
}

/*------------------------------------------------
　アラーム/時報を鳴らす
--------------------------------------------------*/
void OnTimerAlarm(HWND hwnd, SYSTEMTIME* st)
{
	int i, rep, h, fday, fhour, fmin1, fmin2;

	if(st->wDayOfWeek > 0)
		fday = 1 << (st->wDayOfWeek - 1);
	else fday = 1 << 6;

	if(st->wHour >= 0)
		fhour = 1 << st->wHour;
	else fhour = 1 << 23;

	if(st->wMinute >= 0 && st->wMinute < 30)
	{
		fmin1 = 1 << st->wMinute;
		fmin2 = 0;
	}
	else if(st->wMinute >= 30 && st->wMinute < 60)
	{
		fmin1 = 0;
		fmin2 = 1 << (st->wMinute-30);
	}
	else
		fmin1 = fmin2 = 1 << 29;

	if(st->wSecond == 0)
	{
		for(i = 0; i < maxAutoExec; i++)
		{
			if(!pAES[i].bAutoExec) continue;
			if((pAES[i].hour & fhour) && ((pAES[i].minute1 & fmin1)||(pAES[i].minute2 & fmin2))
				&& (pAES[i].days & fday))
			{
				if(pAES[i].fname[0])
				{
					PlayFile(hwnd, pAES[i].fname, 0);
				}
			}
		}
		for(i = 0; i < maxAlarm; i++)
		{
			if(!pAS[i].bAlarm) continue;
			h = st->wHour;
			if(pAS[i].bHour12)
			{
				if(h == 0) h = 12;
				else if(h >= 13) h -= 12;
			}

			if(pAS[i].hour == h && pAS[i].minute == st->wMinute
				&& (pAS[i].days & fday))
			{
				if(pAS[i].bBlink)
					PostMessage(g_hwndClock, CLOCKM_BLINK, FALSE, 0);

				if(pAS[i].fname[0])
				{
					if(pAS[i].bRepeat) rep = -1; else rep = 0;
					if(PlayFile(hwnd, pAS[i].fname, rep)) return;
				}
			}
		}
	}
	if(bOnBootTime)
	{
		bOnBootTime = FALSE;
		for(i = 0; i < maxAutoExec; i++)
		{
			if(!pAES[i].bAutoExec) continue;
			if(!pAES[i].bOnBoot) continue;
			if(pAES[i].fname[0])
				PlayFile(hwnd, pAES[i].fname, 0);
		}
	}
	if(bJihou)
	{
		if((st->wMinute == 0 && nJihouPre == 0 && st->wSecond == 0) ||
			(st->wMinute == 59 && st->wSecond == (60 - nJihouPre)))
		{
			char fname[1024];
			if(nJihouPre == 0)	h = st->wHour;
			else
			{
				h = st->wHour +1;
				if(h == 24) h = 0;
			}
			if(bJihouBlink)
			{
				PostMessage(g_hwndClock, CLOCKM_BLINK, TRUE, 0);
				bBlink_j=TRUE;
			}
			if(h == 0) h = 12;
			else if(h >= 13) h -= 12;
			GetMyRegStr("", "JihouFile", fname, 1024, "");
			if(fname[0])
			{
				if(bJihouRepeat) rep = h; else rep = 0;
				PlayFile(hwnd, fname, rep);
			}
		}
		else if((st->wMinute == 59 && nJihouPre == 55 && st->wSecond == 0) ||
				(st->wMinute == 58 && st->wSecond == (60 - nJihouPre -5)))
		{
			char fname[1024];
			GetMyRegStr("", "JihouFile", fname, 1024, "");
			PlayFilePre(hwnd, fname);
		}
	}
	if( bBlink_j )
	{
		if(st->wSecond==(nJihouTime-nJihouPre+60)%60)
		{
			PostMessage(g_hwndClock, CLOCKM_BLINK, TRUE, TRUE);
			bBlink_j = FALSE;
		}
	}
}

/*------------------------------------------------
　ファイルの読み込みと解放
--------------------------------------------------*/
void PlayFilePre(HWND hwnd, char *fname)
{
	BOOL b;
	b = GetMyRegLong("", "MCIWave", FALSE);

	if(*fname == 0) return;

	if(!b && ext_cmp(fname, "wav") == 0)
	{
		if(bMCIPlaying) return;
		PlayWavePre(hwnd, fname);
		return;
	}
	else if(IsMMFile(fname))
	{
		char command[1024];
		if(bMCIPlaying) return;
		strcpy(command, "open \"");
		strcat(command, fname);
		strcat(command, "\" alias myfile");
		if(mciSendString(command, NULL, 0, NULL) == 0)
		{
			mciSendString("close myfile", NULL, 0, NULL);
		}
	}
}

PSTR FullPathFromTClockPath(PSTR pszCommand)
{
	int len;
	int tlen;
	char szTClockPath[MAX_PATH];
	PSTR pstr;

	// \\NAME\C\path\path\filename.txt
	// C:\path\path\filename.txt
	// 以上の絶対パス以外を相対パスと判断して
	// その前にTClockのパスを基準ディレクトリとして付加
	len = strlen(pszCommand);
	if (len >= 2) {
		if ((*pszCommand == '\\') && (*(pszCommand + 1) == '\\')) {
			//UNC name
			return NULL;
		} else if (*(pszCommand + 1) == ':') {
			return NULL;
		}
	}

	// TClockの位置を基準パスとして指定文字列を相対パスとして追加
	GetModuleFileName(g_hInst, szTClockPath, sizeof(szTClockPath));
	del_title(szTClockPath);
	tlen = strlen(szTClockPath);

	pstr = malloc(tlen + len + 2);
	if (pstr == NULL) {
		return NULL;
	}
	strcpy(pstr, szTClockPath);

	add_title(pstr, pszCommand);

	return pstr;
}

/*------------------------------------------------
　ファイルの再生/実行   dwLoops：繰り返し回数
--------------------------------------------------*/
BOOL PlayFile(HWND hwnd, char *fname, DWORD dwLoops)
{
	BOOL b;
	BOOL ret;
	PSTR pstr;

	b = GetMyRegLong("", "MCIWave", FALSE);

	if(*fname == 0) return FALSE;

	if (!b && ext_cmp(fname, "wav") == 0) {
		if(bMCIPlaying) return FALSE;

		pstr = FullPathFromTClockPath(fname);
		if (pstr == NULL) {
			return PlayWave(hwnd, fname, dwLoops);
		}
		ret = PlayWave(hwnd, pstr, dwLoops);
		free(pstr);
		return ret;
	} else if(IsMMFile(fname)) {
		char command[1024];
		if(bMCIPlaying) return FALSE;
		strcpy(command, "open \"");
		strcat(command, fname);
		strcat(command, "\" alias myfile");
		if(mciSendString(command, NULL, 0, NULL) == 0)
		{
			strcpy(command, "set myfile time format ");
			if(_stricmp(fname, "cdaudio") == 0 || ext_cmp(fname, "cda") == 0)
			{
				strcat(command, "tmsf"); bTrack = TRUE;
			}
			else
			{
				strcat(command, "milliseconds"); bTrack = FALSE;
			}
			mciSendString(command, NULL, 0, NULL);

			nTrack = -1;
			if(ext_cmp(fname, "cda") == 0)
			{
				char* p;
				p = fname; nTrack = 0;
				while(*p)
				{
					if('0' <= *p && *p <= '9') nTrack = nTrack * 10 + *p - '0';
					p++;
				}
			}
			if(PlayMCI(hwnd, nTrack) == 0)
			{
				bMCIPlaying = TRUE;
				countPlay = 1; countPlayNum = dwLoops;
			}
			else mciSendString("close myfile", NULL, 0, NULL);
		}
		return bMCIPlaying;
	} else {
		pstr = FullPathFromTClockPath(fname);
		if (pstr == NULL) {
			ExecFile(hwnd, fname);
			return FALSE;
		}
		ExecFile(hwnd, pstr);
		free(pstr);
	}
	return FALSE;
}

int PlayMCI(HWND hwnd, int nt)
{
	char command[80], s[80];
	char start[40], end[40];

	strcpy(command, "play myfile");
	if(nt >= 0)
	{
		wsprintf(s, "status myfile position track %d", nt);
		if(mciSendString(s, start, 40, NULL) == 0)
		{
			strcat(command, " from ");
			strcat(command, start);
			wsprintf(s, "status myfile position track %d", nt+1);
			if(mciSendString(s, end, 40, NULL) == 0)
			{
				strcat(command, " to ");
				strcat(command, end);
			}
		}
	}
	strcat(command, " notify");
	return mciSendString(command, NULL, 0, hwnd);
}

/*------------------------------------------------
　再生停止
--------------------------------------------------*/
void StopFile(void)
{
	StopWave();
	if(bMCIPlaying)
	{
		mciSendString("stop myfile", NULL, 0, NULL);
		mciSendString("close myfile", NULL, 0, NULL);
		bMCIPlaying = FALSE;
		countPlay = 0; countPlayNum = 0;
	}
	bPlayingNonstop = FALSE;
}

void OnMCINotify(HWND hwnd)
{
	if(bMCIPlaying)
	{
		if(countPlay < countPlayNum || countPlayNum < 0)
		{
			mciSendString("seek myfile to start wait", NULL, 0, NULL);
			if(PlayMCI(hwnd, nTrack) == 0)
			{
				countPlay++;
			}
			else
				StopFile();
		}
		else StopFile();
	}
}

/*--------------------------------------------------------
  Retreive a file name and option from a command string
----------------------------------------------------------*/
void GetFileAndOption(const char* command, char* fname, char* opt)
{
	const char *p, *pe;
	char *pd;
	WIN32_FIND_DATA fd;
	HANDLE hfind;

	p = command; pd = fname;
	pe = NULL;
	for(; ;)
	{
		if(*p == ' ' || *p == 0)
		{
			*pd = 0;
			hfind = FindFirstFile(fname, &fd);
			if(hfind != INVALID_HANDLE_VALUE)
			{
				FindClose(hfind);
				pe = p;
				break;
			}
			if(*p == 0) break;
		}
		*pd++ = *p++;
	}
	if(pe == NULL) pe = p;

	p = command; pd = fname;
	for(; p != pe; )
	{
		*pd++ = *p++;
	}
	*pd = 0;
	if(*p == ' ') p++;

	pd = opt;
	for(; *p; ) *pd++ = *p++;
	*pd = 0;
}

/*------------------------------------------------
  Open a file
--------------------------------------------------*/
BOOL ExecFile(HWND hwnd, char* command)
{
	char fname[MAX_PATH], fpath[MAX_PATH], *opt;
	SHELLEXECUTEINFO sei;
	if(*command == 0) return FALSE;

	opt = malloc(strlen(command));
	if(opt == NULL) return FALSE;
	GetFileAndOption(command, fname, opt);
	strcpy(fpath,fname);
	del_title(fpath);
	memset(&sei,0,sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = fname;
	sei.lpDirectory = fpath;
	sei.lpParameters = opt[0] ? opt : NULL;
	sei.nShow = SW_SHOW;
	ShellExecuteEx(&sei);
	free(opt);

	return ((int)sei.hInstApp > 32);
}

/*------------------------------------------------
　WAVEの読み込みと解放
--------------------------------------------------*/
void PlayWavePre(HWND hwnd, char *fname)
{
	HMMIO hmmio;
	MMCKINFO mmckinfoParent;
	MMCKINFO mmckinfoSubchunk;
	LONG lFmtSize;
	LONG lDataSize;

	if(hWaveOut != NULL) return;

	if(!(hmmio = mmioOpen(fname, NULL, MMIO_READ | MMIO_ALLOCBUF)))
		return;

	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, MMIO_FINDRIFF))
	{
		mmioClose(hmmio, 0);
		return;
	}

	mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent,
		MMIO_FINDCHUNK))
	{
		mmioClose(hmmio, 0);
		return;
	}

	lFmtSize = mmckinfoSubchunk.cksize;
	pFormat = (WAVEFORMATEX*)malloc(lFmtSize);
	if(pFormat == NULL)
	{
		mmioClose(hmmio, 0);
		return;
	}

	if(mmioRead(hmmio, (HPSTR)pFormat, lFmtSize) != lFmtSize)
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return;
	}

	/*
	if(pFormat->wFormatTag != WAVE_FORMAT_PCM)
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return;
	}
	*/

	if(waveOutOpen(&hWaveOut, (UINT)WAVE_MAPPER, (LPWAVEFORMATEX)pFormat,
		0, 0, (DWORD)WAVE_FORMAT_QUERY))
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return;
	}

	mmioAscend(hmmio, &mmckinfoSubchunk, 0);

	mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent,
		MMIO_FINDCHUNK))
    {
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return;
	}

	lDataSize = mmckinfoSubchunk.cksize;
	if(lDataSize == 0)
    {
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return;
	}

	pData = (HPSTR)malloc(lDataSize);
	if(pData == NULL)
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return;
	}

	if(mmioRead(hmmio, pData, lDataSize) != lDataSize)
	{
		free(pFormat); pFormat = NULL;
		free(pData); pData = NULL;
		mmioClose(hmmio, 0);
		return;
	}
	mmioClose(hmmio, 0);

	if(waveOutOpen((LPHWAVEOUT)&hWaveOut, (UINT)WAVE_MAPPER,
		(LPWAVEFORMATEX)pFormat, (UINT)hwnd, 0,
		(DWORD)CALLBACK_WINDOW))
    {
		free(pFormat); pFormat = NULL;
		free(pData); pData = NULL;
		return;
	}

	memset(&wh, 0, sizeof(WAVEHDR));
	wh.lpData = pData;
	wh.dwBufferLength = lDataSize;

	if(waveOutPrepareHeader(hWaveOut, &wh, sizeof(WAVEHDR)))
	{
		waveOutClose(hWaveOut); hWaveOut = NULL;
		free(pFormat); pFormat = NULL;
		free(pData); pData = NULL;
		return;
	}

	waveOutUnprepareHeader(hWaveOut, &wh, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);	hWaveOut = NULL;
	free(pFormat); pFormat = NULL;
	free(pData); pData = NULL;
}

/*------------------------------------------------
　WAVEの再生   dwLoops：繰り返し回数
　参考：Visual C++ 1.x のサンプルREVERSE
--------------------------------------------------*/
BOOL PlayWave(HWND hwnd, char *fname, DWORD dwLoops)
{
	HMMIO hmmio;
	MMCKINFO mmckinfoParent;
	MMCKINFO mmckinfoSubchunk;
	LONG lFmtSize;
	LONG lDataSize;

	if(hWaveOut != NULL) return FALSE;

	if(!(hmmio = mmioOpen(fname, NULL, MMIO_READ | MMIO_ALLOCBUF)))
		return FALSE;

	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if(mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, MMIO_FINDRIFF))
	{
		mmioClose(hmmio, 0);
		return FALSE;
	}

	mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if(mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent,
		MMIO_FINDCHUNK))
	{
		mmioClose(hmmio, 0);
		return FALSE;
	}

	lFmtSize = mmckinfoSubchunk.cksize;
	pFormat = (WAVEFORMATEX*)malloc(lFmtSize);
	if(pFormat == NULL)
	{
		mmioClose(hmmio, 0);
		return FALSE;
	}

	if(mmioRead(hmmio, (HPSTR)pFormat, lFmtSize) != lFmtSize)
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return FALSE;
	}

	/*
	if(pFormat->wFormatTag != WAVE_FORMAT_PCM)
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return FALSE;
	}
	*/

	if(waveOutOpen(&hWaveOut, (UINT)WAVE_MAPPER, (LPWAVEFORMATEX)pFormat,
		0, 0, (DWORD)WAVE_FORMAT_QUERY))
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return FALSE;
	}

	mmioAscend(hmmio, &mmckinfoSubchunk, 0);

	mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if(mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent,
		MMIO_FINDCHUNK))
    {
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return FALSE;
	}

	lDataSize = mmckinfoSubchunk.cksize;
	if(lDataSize == 0)
    {
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return FALSE;
	}

	pData = (HPSTR)malloc(lDataSize);
	if(pData == NULL)
	{
		free(pFormat); pFormat = NULL;
		mmioClose(hmmio, 0);
		return FALSE;
	}

	if(mmioRead(hmmio, pData, lDataSize) != lDataSize)
	{
		free(pFormat); pFormat = NULL;
		free(pData); pData = NULL;
		mmioClose(hmmio, 0);
		return FALSE;
	}
	mmioClose(hmmio, 0);

	if(waveOutOpen((LPHWAVEOUT)&hWaveOut, (UINT)WAVE_MAPPER,
		(LPWAVEFORMATEX)pFormat, (UINT)hwnd, 0,
		(DWORD)CALLBACK_WINDOW))
    {
		free(pFormat); pFormat = NULL;
		free(pData); pData = NULL;
		return FALSE;
	}

	memset(&wh, 0, sizeof(WAVEHDR));
	wh.lpData = pData;
	wh.dwBufferLength = lDataSize;
	if(dwLoops != 0)
	{
		wh.dwFlags = WHDR_BEGINLOOP|WHDR_ENDLOOP;
		wh.dwLoops = dwLoops;
	}
	if(waveOutPrepareHeader(hWaveOut, &wh, sizeof(WAVEHDR)))
	{
		waveOutClose(hWaveOut); hWaveOut = NULL;
		free(pFormat); pFormat = NULL;
		free(pData); pData = NULL;
		return FALSE;
	}

	if(waveOutWrite(hWaveOut, &wh, sizeof(WAVEHDR)) != 0)
	{
		waveOutUnprepareHeader(hWaveOut, &wh, sizeof(WAVEHDR));
		waveOutClose(hWaveOut);	hWaveOut = NULL;
		free(pFormat); pFormat = NULL;
		free(pData); pData = NULL;
		return FALSE;
	}

	return TRUE;
}

/*------------------------------------------------
　WAVE再生停止
--------------------------------------------------*/
void StopWave(void)
{
	if(hWaveOut == NULL) return;

	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &wh, sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
	hWaveOut = NULL;
	free(pFormat); pFormat = NULL;
	free(pData); pData = NULL;
}
