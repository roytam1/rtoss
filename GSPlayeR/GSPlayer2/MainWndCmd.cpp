#include "GSPlayer2.h"
#include "MainWnd.h"
#ifdef _WIN32_WCE
#include "FolderDlg.h"
#endif

#ifdef _WIN32_WCE_PPC
#include "EffectDlgPPC.h"
#else
#include "EffectDlg.h"
#endif

#define FILE_BUFF_LEN		2048
#define OPEN_FILTER_BUF		1024

typedef struct _EXT_INFO {
	TCHAR szExt[MAX_PATH];
	TCHAR szExtDesc[MAX_PATH];
} EXT_INFO;

void CMainWnd::MakeOpenFilter(LPTSTR* ppszFilter, BOOL fGsGetFile)
{
	CTempStr str;
	TCHAR szExt[MAX_PATH];
	TCHAR szExtDesc[MAX_PATH];
	TCHAR szTemp[MAX_PATH * 2];
	MAP_DEC_PLUGIN* pPlugIn;

	EXT_INFO* pExt;
	CMultiBuff extbuff;

	BOOL fFound;
	int i, j;
	int nBuf = OPEN_FILTER_BUF;
	int nPlugIn = MAP_GetDecoderPlugInCount(m_hMap);
	int nExt;

	*ppszFilter = new TCHAR[nBuf];
	*ppszFilter[0] = NULL;

	if (fGsGetFile) {
		str.LoadNormal(IDS_FILTER_OPEN_SUPPORTED);
		_tcscpy(*ppszFilter, str);
	}

	for (i = 0; i < nPlugIn; i++) {
		pPlugIn = MAP_GetDecoderPlugIn(m_hMap, i);
		if (!pPlugIn)
			continue;

		nExt = pPlugIn->GetFileExtCount();
		for (j = 0; j < nExt; j++) {
			memset(szExt, 0, sizeof(szExt));
			memset(szExtDesc, 0, sizeof(szExtDesc));
			if (pPlugIn->GetFileExt(j, szExt, szExtDesc) && _tcslen(szExt)) {
				if (!IsDefSupportedExt(szExt)) {
					fFound = FALSE;
					for (int k = 0; k < extbuff.GetCount(); k++) {
						pExt = (EXT_INFO*)extbuff.GetAt(k);
						if (_tcsicmp(pExt->szExt, szExt) == 0) {
							fFound = TRUE;
							break;
						}
					}
					if (!fFound) {
						if (fGsGetFile) {
							if (nBuf - 1 < (int)(_tcslen(*ppszFilter) + _tcslen(szExt) + 3)) {
								nBuf += OPEN_FILTER_BUF;
								LPTSTR pszNew = new TCHAR[nBuf];
								_tcscpy(pszNew, *ppszFilter);
								delete *ppszFilter;
								*ppszFilter = pszNew;
							}

							_tcscat(*ppszFilter, _T(";*."));
							_tcscat(*ppszFilter, szExt);
						}

						pExt = new EXT_INFO;
						_tcscpy(pExt->szExt, szExt);
						_tcscpy(pExt->szExtDesc, szExtDesc);
						extbuff.Add((DWORD)pExt);
					}
				}
			}
		}
	}

	str.LoadNormal(IDS_FILTER_OPEN_INTERNAL);
	if (nBuf - 1 < (int)(_tcslen(*ppszFilter) + _tcslen(str))) {
		nBuf += OPEN_FILTER_BUF;
		LPTSTR pszNew = new TCHAR[nBuf];
		_tcscpy(pszNew, *ppszFilter);
		delete *ppszFilter;
		*ppszFilter = pszNew;
	}
	if (_tcslen(*ppszFilter))
		_tcscat(*ppszFilter, str);
	else
		_tcscpy(*ppszFilter, (LPTSTR)str + 1);

	while (!extbuff.IsEmpty()) {
		pExt = (EXT_INFO*)extbuff.RemoveAt(0);
		if (!_tcslen(pExt->szExtDesc)) {
			str.LoadNormal(IDS_FMT_NULL_FILE_TYPE);
			wsprintf(pExt->szExtDesc, str, pExt->szExt, pExt->szExt);
		}
		wsprintf(szTemp, _T("|%s|*.%s"), pExt->szExtDesc, pExt->szExt);
		if (nBuf - 1 < (int)(_tcslen(*ppszFilter) + _tcslen(szTemp))) {
			nBuf += OPEN_FILTER_BUF;
			LPTSTR pszNew = new TCHAR[nBuf];
			_tcscpy(pszNew, *ppszFilter);
			delete *ppszFilter;
			*ppszFilter = pszNew;
		}
		_tcscat(*ppszFilter, szTemp);

		delete pExt;
	}

	str.LoadNormal(IDS_FILTER_OPEN_ALL);
	if (nBuf - 1 < (int)(_tcslen(*ppszFilter) + _tcslen(str))) {
		nBuf += OPEN_FILTER_BUF;
		LPTSTR pszNew = new TCHAR[nBuf];
		_tcscpy(pszNew, *ppszFilter);
	}
	_tcscat(*ppszFilter, str);

	LPTSTR psz = *ppszFilter;
	while (psz = _tcschr(psz, _T('|'))) {
		*psz++ = NULL;
	}
}

BOOL CMainWnd::OpenMediaFile(BOOL fAdd, HWND hwndParent)
{
	if (m_fShowOpenDlg)
		return FALSE;

	BOOL fRet;

	m_fShowOpenDlg = TRUE;
	CTempStr strTitle(fAdd ? IDS_TITLE_ADD_FILE : IDS_TITLE_OPEN_FILE);
	CTempStr strFilter;
	TCHAR szFile[FILE_BUFF_LEN] = {0};
#ifdef _WIN32_WCE
	OPENFILENAME ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner   = hwndParent ? hwndParent : m_hWnd;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = FILE_BUFF_LEN;	
	ofn.lpstrTitle  = strTitle;

	BOOL (*gsGetOpenFileName)(OPENFILENAME*) = NULL;
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		(FARPROC&)gsGetOpenFileName = GetProcAddress(hInst, _T("gsGetOpenFileName"));
	}

	if (gsGetOpenFileName) {
		if (!m_pszOpenFilter) MakeOpenFilter(&m_pszOpenFilter, TRUE);
		ofn.lpstrFilter = m_pszOpenFilter;
		ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
		ofn.lpstrInitialDir = m_pOptions->m_szLastDir;
		fRet = gsGetOpenFileName(&ofn);
	}
	else {
		if (!m_pszOpenFilter) MakeOpenFilter(&m_pszOpenFilter, FALSE);
		ofn.lpstrFilter = m_pszOpenFilter;
		ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrInitialDir = m_pOptions->m_szLastDir;
		fRet = GetOpenFileName(&ofn);
	}

	if (hInst) FreeLibrary(hInst);
#else
	if (!m_pszOpenFilter) MakeOpenFilter(&m_pszOpenFilter, TRUE);

	OPENFILENAME ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner   = hwndParent ? hwndParent : m_hWnd;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = FILE_BUFF_LEN;	
	ofn.lpstrTitle  = strTitle;
	ofn.lpstrFilter = m_pszOpenFilter;
	ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
	ofn.lpstrInitialDir = m_pOptions->m_szLastDir;

	fRet = GetOpenFileName(&ofn);

#endif
	m_fShowOpenDlg = FALSE;
	if (!fRet)
		return FALSE;

	// 既存のリストを削除
	if (!fAdd) DeleteAllFile();

	// 追加
	DWORD dw = GetFileAttributes(szFile);
	if (dw != 0xFFFFFFFF) {
		if (!(dw & FILE_ATTRIBUTE_DIRECTORY)) {
			// single
			if (IsPlayList(szFile))
				LoadPlayList(szFile);
			else 
				AddFile(szFile);
			LPTSTR psz = _tcsrchr(szFile, _T('\\'));
			if (psz) *psz = _T('\0');
				_tcscpy(m_pOptions->m_szLastDir, szFile);
		}
		else {
			// multi
			TCHAR szPath[MAX_PATH];

			LPTSTR p = szFile;
			_tcscpy(m_pOptions->m_szLastDir, szFile);
			while (TRUE) {
				p += _tcslen(p);
				if (*p != NULL || *(p + 1) == NULL)
					break;

				wsprintf(szPath, _T("%s\\%s"), m_pOptions->m_szLastDir, ++p);
				if (IsPlayList(szPath))
					LoadPlayList(szPath);
				else
					AddFile(szPath);
			}
		}
	}
	return TRUE;
}

void CMainWnd::SaveMediaFile(HWND hwndParent)
{
	if (m_fShowOpenDlg)
		return;

	BOOL fRet;
	CTempStr strTitle(IDS_TITLE_SAVE_LIST);
	CTempStr strFilter(IDS_FILTER_SAVE_LIST);
	TCHAR szFile[MAX_PATH] = {0};

	m_fShowOpenDlg = TRUE;
#ifdef _WIN32_WCE
	OPENFILENAME ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner   = hwndParent ? hwndParent : m_hWnd;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = MAX_PATH;	
	ofn.lpstrTitle  = strTitle;
	ofn.lpstrFilter = strFilter;
	ofn.lpstrDefExt = M3U8_FILE_EXT;
	ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = m_pOptions->m_szLastDir;

	BOOL (*gsGetSaveFileName)(OPENFILENAME*) = NULL;
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		(FARPROC&)gsGetSaveFileName = GetProcAddress(hInst, _T("gsGetSaveFileName"));
	}

	if (gsGetSaveFileName)
		fRet = gsGetSaveFileName(&ofn);
	else
		fRet = GetSaveFileName(&ofn);

	if (hInst) FreeLibrary(hInst);
#else
	OPENFILENAME ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner   = m_hWnd;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = MAX_PATH;	
	ofn.lpstrTitle  = strTitle;
	ofn.lpstrFilter = strFilter;
	ofn.lpstrDefExt = M3U8_FILE_EXT;
	ofn.Flags       = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = m_pOptions->m_szLastDir;

	fRet = GetSaveFileName(&ofn);
#endif
	m_fShowOpenDlg = FALSE;
	if (!fRet)
		return;

	SavePlayList(szFile);
}

////////////////////////////////////////////////////////////////////////////////
void CMainWnd::OnFileOpen()
{
	if (OpenMediaFile(FALSE) && m_pListFile->GetCount())
		Play();

	if (m_pPlayListDlg)
		m_pPlayListDlg->UpdatePlayList();
}

void CMainWnd::OnFileOpenLocation()
{
	TCHAR szLocation[MAX_URL];
	if (m_pOptions->ShowLocationDlg(m_hWnd, szLocation) == IDOK) {
		DeleteAllFile();
		AddFile(szLocation);
		if (m_pListFile->GetCount())
			Play();

		if (m_pPlayListDlg)
			m_pPlayListDlg->UpdatePlayList();
	}
}

void CMainWnd::OnFileAdd()
{
	OpenMediaFile(TRUE);

	if (m_pPlayListDlg)
		m_pPlayListDlg->UpdatePlayList();
}

void CMainWnd::OnFileFind()
{
	TCHAR szPath[MAX_PATH] = {0};
#ifdef _WIN32_WCE
	CFolderDlg dlg;
	dlg.SetSubFolder(m_pOptions->m_fSearchSubFolders);
	if (dlg.ShowFolderDlg(m_hWnd, szPath, MAX_PATH) == IDCANCEL)
		return;

	m_pOptions->m_fSearchSubFolders = dlg.GetSubFolder();
#else
	CTempStr strTitle(IDS_TITLE_FIND_FILE);

	//フォルダ選択ダイアログの表示
	LPMALLOC pmalloc;
	LPITEMIDLIST lPIDL;
	LPITEMIDLIST pIDL;
	if (SUCCEEDED(SHGetMalloc(&pmalloc))) {
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &lPIDL)))	{
			TCHAR szBuff[MAX_PATH];
			_tcscpy(szBuff, m_pOptions->m_szLastDir);
			BROWSEINFO binfo = {0};
			binfo.hwndOwner = m_hWnd;
			binfo.lpszTitle = strTitle;
			binfo.pidlRoot = lPIDL;
			binfo.pszDisplayName = szBuff;
			binfo.ulFlags = BIF_RETURNONLYFSDIRS;
			pIDL = SHBrowseForFolder(&binfo);
			if (pIDL)
				SHGetPathFromIDList(pIDL, szPath);
			pmalloc->Free(pIDL);
		}
		pmalloc->Release();
	}

	if (!_tcslen(szPath))
		return;
#endif
	_tcscpy(m_pOptions->m_szLastDir, szPath);
	AddDir(szPath);

	if (m_pPlayListDlg)
		m_pPlayListDlg->UpdatePlayList();
}

void CMainWnd::OnFileList()
{
	if (!m_pPlayListDlg)
		m_pPlayListDlg = new CPlayListDlg();
	m_pPlayListDlg->Show(this);
}

void CMainWnd::OnFileSaveList()
{
	SaveMediaFile();
}

void CMainWnd::OnPlayPlayPause()
{
	if (MAP_GetStatus(m_hMap) == MAP_STATUS_STOP) {
		Play();
	}
	else 
		Pause();
}

void CMainWnd::OnPlayStopAfterCurrent()
{
	m_fStopAfterCurr = m_fStopAfterCurr == TRUE ? FALSE : TRUE;
}

void CMainWnd::OnPlayStop()
{
	// レジューム用の情報を取得しておく
	if (m_pOptions->m_fResume && MAP_GetStatus(m_hMap) != MAP_STATUS_STOP) {
		m_pOptions->m_nResumeTime = MAP_GetCurrent(m_hMap);
	}

	if (MAP_GetStatus(m_hMap) == MAP_STATUS_STOP)
		MAP_AudioDeviceClose(m_hMap);

	// 停止処理
	Stop();

	// シャッフルフラグをリセット
	if (m_pOptions->m_fShuffle) {
		ResetShuffle();
		FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(m_nCurrent);
		if (p) p->fShuffle = TRUE;
	}

	// レジュームの場合時刻をセット
	if (m_pOptions->m_fResume) {
		MAP_Seek(m_hMap, m_pOptions->m_nResumeTime);
		m_pOptions->m_nResumeTime = 0;
	}

	UpdateTime();
	UpdateStatus();
	UpdateStreamTitle();
}

void CMainWnd::OnPlayPrev()
{
	BOOL fPlay = MAP_GetStatus(m_hMap) != MAP_STATUS_STOP;

	// ファイル再生の時に再生時間が2秒以上なら先頭に
	// 2秒未満の場合は前の曲
	FILEINFO* pInfo = GetInfo(GetCurrentFile());
	if (pInfo && !IsURL(pInfo->szPath)) {
		long lCurrent = MAP_GetCurrent(m_hMap);
		if (fPlay && lCurrent > 2000) {
			MAP_Seek(m_hMap, 0);
			return;
		}
	}

	int nPrev = GetPrevIndex();
	if (nPrev == OPEN_NONE)
		return;

	Open(nPrev);
	if (fPlay)
		Play();
}

void CMainWnd::OnPlayNext()
{
	int nNext = GetNextIndex();
	if (nNext == OPEN_NONE)
		return;
	
	BOOL fPlay = MAP_GetStatus(m_hMap) != MAP_STATUS_STOP;
	Open(nNext);
	if (m_pPlayListDlg) {
		m_pPlayListDlg->UpdatePlayList();
	}
	
	// Hack
	m_pOptions->m_nResumeTime = 0;
	OnMsgStatus(MAP_STATUS_PLAY, FALSE);
	if (fPlay)
		Play();
}

void CMainWnd::OnPlayRew()
{
	Seek((MAP_GetCurrent(m_hMap) - SKIP_INTERVAL));
}

void CMainWnd::OnPlayFf()
{
	Seek((MAP_GetCurrent(m_hMap) + SKIP_INTERVAL));
}

void CMainWnd::OnPlayRepeat()
{
	switch (m_pOptions->m_fRepeat) {
	case COptions::RepeatNone:
		m_pOptions->m_fRepeat = COptions::RepeatAll;
		break;
	case COptions::RepeatAll:
		m_pOptions->m_fRepeat = COptions::RepeatOne;
		break;
	default:
		m_pOptions->m_fRepeat = COptions::RepeatNone;
		break;
	}
	UpdatePlayOption();
}

void CMainWnd::OnPlayRepeatNone()
{
	m_pOptions->m_fRepeat = COptions::RepeatNone;
	UpdatePlayOption();
}

void CMainWnd::OnPlayRepeatAll()
{
	m_pOptions->m_fRepeat = COptions::RepeatAll;
	UpdatePlayOption();
}

void CMainWnd::OnPlayRepeatOne()
{
	m_pOptions->m_fRepeat = COptions::RepeatOne;
	UpdatePlayOption();
}

void CMainWnd::OnPlayShuffle()
{
	m_pOptions->m_fShuffle = !m_pOptions->m_fShuffle;
	if (m_pOptions->m_fShuffle) {
		srand(GetTickCount());
		FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(m_nCurrent);
		if (p) p->fShuffle = TRUE;
	}
	else {
		ResetShuffle();
	}
	UpdatePlayOption();
}

#define VOLUME_INT ((double)0xFFFF * 2 / 100)
void CMainWnd::OnToolVolUp()
{
	DWORD dwVolume;
	waveOutGetVolume(NULL, &dwVolume);

	double dLeft = LOWORD(dwVolume) + 1;
	double dRight = HIWORD(dwVolume) + 1;

	//left channnel (low word)
	dLeft = min(dLeft + VOLUME_INT, 0xFFFF);

	//right channel (high word)
	dRight = min(dRight + VOLUME_INT, 0xFFFF);

	waveOutSetVolume(NULL, MAKELONG((int)dLeft, (int)dRight));

	// 表示を更新 (数秒後に元に戻す)
	KillTimer(m_hWnd, ID_TIMER_VOLUME);
	m_fDrawVolume = TRUE;
	UpdateTitle();
	SetTimer(m_hWnd, ID_TIMER_VOLUME, TIMER_VOLUME_INTERVAL, NULL);

	UpdateVolumeSlider();
}

void CMainWnd::OnToolVolDown()
{
	DWORD dwVolume;
	waveOutGetVolume(NULL, &dwVolume);

	double dLeft = LOWORD(dwVolume) + 1;
	double dRight = HIWORD(dwVolume) + 1;

	//left channnel (low word)
	dLeft = max(dLeft - VOLUME_INT, 0);

	//right channel (high word)
	dRight = max(dRight - VOLUME_INT, 0);

	waveOutSetVolume(NULL, MAKELONG((int)dLeft, (int)dRight));

	// 表示を更新 (数秒後に元に戻す)
	KillTimer(m_hWnd, ID_TIMER_VOLUME);
	m_fDrawVolume = TRUE;
	UpdateTitle();
	SetTimer(m_hWnd, ID_TIMER_VOLUME, TIMER_VOLUME_INTERVAL, NULL);

	UpdateVolumeSlider();
}

void CMainWnd::OnToolEffect()
{
	CEffectDlg dlg;
	dlg.ShowEffectDlg(m_hWnd, m_hMap);
}

void CMainWnd::OnToolTopMost()
{
	m_pOptions->m_fTopMost = !m_pOptions->m_fTopMost;
	SetWindowPos(m_hWnd, m_pOptions->m_fTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 
						0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CMainWnd::OnToolOption()
{
	m_pOptions->ShowOptionDlg(m_hWnd, m_hMap);
	MAP_SetScanPeek(m_hMap, m_pOptions->m_fDrawPeek);

	// 再描画する
	DeleteImages();
	InitImages();
	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::OnToolSleepTimer()
{
	if (m_pOptions->ShowSleepTimerDlg(m_hWnd)) {
		if (m_pOptions->m_nSleepTime != -1) {
			m_pOptions->m_nSleepTime = 0;
			m_pOptions->m_dwSleepLast = GetTickCount();
			SetTimer(m_hWnd, ID_TIMER_SLEEPTIMER, TIMER_SLEEPTIMER_INTERVAL, NULL);
		}
		else {
			KillTimer(m_hWnd, ID_TIMER_SLEEPTIMER);
		}
	}
}

void CMainWnd::OnShowMenu(POINT* ppt)
{
	if (m_hMenu)
		return;

	m_hMenu = LoadMenu(GetInst(), (LPCTSTR)IDR_POPUP);
	HMENU hSub = GetSubMenu(m_hMenu, IDM_SUBMENU_MAIN);

	POINT pt;
	if (ppt)
		pt = *ppt;
	else {
		if (m_pSkin->GetImageList(IMAGELIST_BUTTON)) {
			pt.x = BTN_LEFT + BTN_MENU * BTN_WIDTH + (BTN_WIDTH / 2);
			pt.y = BTN_TOP + (BTN_HEIGHT / 2);
		}
		else {
			pt.x = 0;
			pt.y = 0;
		}
	}
	ClientToScreen(m_hWnd, &pt);

	switch (m_pOptions->m_fRepeat) {
	case COptions::RepeatAll:
		CheckMenuItem(hSub, IDM_PLAY_REPEAT_ALL, MF_BYCOMMAND | MF_CHECKED);
		break;
	case COptions::RepeatOne:
		CheckMenuItem(hSub, IDM_PLAY_REPEAT_ONE, MF_BYCOMMAND | MF_CHECKED);
		break;
	default:
		CheckMenuItem(hSub, IDM_PLAY_REPEAT_NONE, MF_BYCOMMAND | MF_CHECKED);
		break;
	}
	if (m_pOptions->m_fShuffle)
		CheckMenuItem(hSub, IDM_PLAY_SHUFFLE, MF_BYCOMMAND | MF_CHECKED);
	if (m_pOptions->m_fTopMost)
		CheckMenuItem(hSub, IDM_TOOL_TOPMOST, MF_BYCOMMAND | MF_CHECKED);

	SetForegroundWindow(m_hWnd);
	TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
	DestroyMenu(m_hMenu);
	m_hMenu = NULL;
}

