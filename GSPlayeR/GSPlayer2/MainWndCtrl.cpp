#include "GSPlayer2.h"
#include "MainWnd.h"
#include "multibuff.h"
#include "mt19937ar.h"
#include "sloperand.h"

// リスト
BOOL CMainWnd::AddFile(LPTSTR pszFile, LPTSTR pszTitle, long nDur)
{
	if (!m_hMap) return FALSE;

	// 存在チェック
	if (IsExisting(pszFile))
		return FALSE;

	// 有効性チェック
	if (!m_pOptions->m_fFastLoad && !IsValidStream(pszFile))
		return FALSE;

	// リストに追加
	FILEINFO* pInfo = new FILEINFO;

	if (nDur)
		pInfo->info.nDuration = nDur;

	if (pszTitle)
		_tcscpy(pInfo->szDisplayName, pszTitle);
	else
		MAP_GetId3TagFile(m_hMap, pszFile, &pInfo->tag);

	_tcscpy(pInfo->szPath, pszFile);
	m_pListFile->Add((DWORD)pInfo);

	// 開いていない場合は開く
	OpenFirstFile();

	return TRUE;
}

int CALLBACK AddDirSoftProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam)
{
	return _tcsicmp((LPTSTR)lParam1, (LPTSTR)lParam2);
}

void CMainWnd::AddDir(LPTSTR pszDir)
{
	TCHAR szPath[MAX_PATH];
	TCHAR szSearch[MAX_PATH] = {0};

	LPTSTR pszPath;
	CMultiBuff sortbuff;
	
	//検索する
	CWaitCursor wc;

	WIN32_FIND_DATA wfd;
	MAP_DEC_PLUGIN* pPlugIn;
	TCHAR szExt[MAX_PATH];
	TCHAR szExtDesc[MAX_PATH];
	int nExt;
	int nPlugIn = MAP_GetDecoderPlugInCount(m_hMap);

	int i = 0;
	while (*s_szDefSupportedExt[i] != NULL) {
		wsprintf(szSearch, _T("%s\\*.%s"), pszDir, s_szDefSupportedExt[i]);
		HANDLE hFind = FindFirstFile(szSearch, &wfd);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					pszPath = new TCHAR[MAX_PATH];
					wsprintf(pszPath, _T("%s\\%s"), pszDir, wfd.cFileName);
					sortbuff.Add((DWORD)pszPath);
				}
			}
			while (FindNextFile(hFind, &wfd));
			FindClose(hFind);
		}
		i++;
	}

	for (i = 0; i < nPlugIn; i++) {		
		pPlugIn = MAP_GetDecoderPlugIn(m_hMap, i);
		if (!pPlugIn)
			continue;

		nExt = pPlugIn->GetFileExtCount();
		for (int j = 0; j < nExt; j++) {
			if (pPlugIn->GetFileExt(j, szExt, szExtDesc) && _tcslen(szExt) && !IsDefSupportedExt(szExt, FALSE)) {
				wsprintf(szSearch, _T("%s\\*.%s"), pszDir, szExt);
				HANDLE hFind = FindFirstFile(szSearch, &wfd);
				if (hFind != INVALID_HANDLE_VALUE) {
					do {
						if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
							pszPath = new TCHAR[MAX_PATH];
							wsprintf(pszPath, _T("%s\\%s"), pszDir, wfd.cFileName);
							sortbuff.Add((DWORD)pszPath);
						}
					}
					while (FindNextFile(hFind, &wfd));
					FindClose(hFind);
				}		
			}
		}
	}

	sortbuff.Sort(AddDirSoftProc, 0);
	while (!sortbuff.IsEmpty()) {
		pszPath = (LPTSTR)sortbuff.RemoveAt(0);
		AddFile(pszPath);
		delete [] pszPath;
	}

	if (m_pOptions->m_fSearchSubFolders) {
		WIN32_FIND_DATA wfd;
		wsprintf(szSearch, _T("%s\\*"), pszDir);
		HANDLE hFind = FindFirstFile(szSearch, &wfd);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					wsprintf(szPath, _T("%s\\%s"), pszDir, wfd.cFileName);
					AddDir(szPath);
				}
			}
			while (FindNextFile(hFind, &wfd));
			FindClose(hFind);
		}
	}
}

BOOL CMainWnd::InsertFile(LPTSTR pszFile, int nIndex)
{
	if (!m_hMap) return FALSE;

	// 有効性チェック
	if (!IsValidStream(pszFile))
		return FALSE;

	// リストに挿入
	FILEINFO* pInfo = new FILEINFO;
	MAP_GetId3TagFile(m_hMap, pszFile, &pInfo->tag);
	_tcscpy(pInfo->szPath, pszFile);
	m_pListFile->Insert((DWORD)pInfo, nIndex);

	// 開いていない場合は開く
	OpenFirstFile();

	return TRUE;
}

BOOL CMainWnd::UpFile(int nIndex)
{
	if (nIndex <= 0)
		return FALSE;

	FILEINFO* p1 = (FILEINFO*)m_pListFile->GetAt(nIndex);
	FILEINFO* p2 = (FILEINFO*)m_pListFile->GetAt(nIndex - 1);
	if (!p1 || !p2)
		return FALSE;

	m_pListFile->SetAt(nIndex - 1, (DWORD)p1);
	m_pListFile->SetAt(nIndex, (DWORD)p2);

	if (m_nCurrent == nIndex)
		m_nCurrent--;
	else if (m_nCurrent == nIndex - 1)
		m_nCurrent++;

	UpdateTrack();
	return TRUE;
}

BOOL CMainWnd::DownFile(int nIndex)
{
	if (nIndex >= m_pListFile->GetCount() - 1)
		return FALSE;

	FILEINFO* p1 = (FILEINFO*)m_pListFile->GetAt(nIndex);
	FILEINFO* p2 = (FILEINFO*)m_pListFile->GetAt(nIndex + 1);
	if (!p1 || !p2)
		return FALSE;

	m_pListFile->SetAt(nIndex + 1, (DWORD)p1);
	m_pListFile->SetAt(nIndex, (DWORD)p2);

	if (m_nCurrent == nIndex)
		m_nCurrent++;
	else if (m_nCurrent == nIndex + 1)
		m_nCurrent--;

	UpdateTrack();
	return TRUE;
}

BOOL CMainWnd::DeleteFile(int nIndex)
{
	if (!m_pListFile->RemoveAt(nIndex))
		return FALSE;

	if (!m_pListFile->GetCount()) {
		Close();
		m_nCurrent = OPEN_NONE;
		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}
	else if (m_nCurrent == nIndex) {
		Stop();
		UpdateTime();
		UpdateStatus();
		UpdateStreamTitle();
		m_nCurrent = OPEN_NONE;

		Open(m_pListFile->GetCount() - 1 < nIndex ? 0 : nIndex);
	}
	else if (m_nCurrent > nIndex) {
		m_nCurrent--;
		UpdateTrack();
	}
	return TRUE;
}

void CMainWnd::DeleteAllFile()
{
	Stop();
	UpdateTime();
	UpdateStatus();
	UpdateStreamTitle();

	Close();

	m_nCurrent = OPEN_NONE;

	for (int i = 0; i < m_pListFile->GetCount(); i++) {
		FILEINFO* pInfo = (FILEINFO*)m_pListFile->GetAt(i);
		delete pInfo;
	}
	m_pListFile->RemoveAll();

	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

int CALLBACK CMainWnd::SortCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam)
{
	FILEINFO* p1 = (FILEINFO*)lParam1;
	FILEINFO* p2 = (FILEINFO*)lParam2;

	LPTSTR psz;
	TCHAR szName1[MAX_PATH];
	TCHAR szName2[MAX_PATH];

	switch (lParam) {
	case SORTFILE_BYNAME:
		psz = _tcsrchr(p1->szPath, _T('\\'));
		if (psz) _tcscpy(szName1, psz + 1);
		psz = _tcsrchr(p2->szPath, _T('\\'));
		if (psz) _tcscpy(szName2, psz + 1);
		return _tcsicmp(szName1, szName2);
	case SORTFILE_BYPATH:
		return _tcsicmp(p1->szPath, p2->szPath);
	case SORTFILE_BYTRACKNUM:
		if (p1->tag.nTrackNum < p2->tag.nTrackNum)
			return -1;
		else if (p1->tag.nTrackNum > p2->tag.nTrackNum)
			return 1;
		return 0;	
	case SORTFILE_BYARTIST:
		return _tcscmp(p1->tag.szArtist, p2->tag.szArtist);
	case SORTFILE_BYTRACK:
		return _tcscmp(p1->tag.szTrack, p2->tag.szTrack);
	}

	return 0;
}

void CMainWnd::SortFile(int nSort)
{
	int i;
	FILEINFO* p;
	for (i = 0; i < m_pListFile->GetCount(); i++) {
		p = (FILEINFO*)m_pListFile->GetAt(i);
		p->dwUser = (i == m_nCurrent) ? 1 : 0;
	}

	m_pListFile->Sort(SortCompareProc, nSort);
	for (i = 0; i < m_pListFile->GetCount(); i++) {
		p = (FILEINFO*)m_pListFile->GetAt(i);
		if (p->dwUser) {
			m_nCurrent = i;
			break;
		}
	}
	UpdateTrack();
}

int CMainWnd::GetNextIndex(BOOL fEos, BOOL fError)
{
	if (m_nCurrent == OPEN_NONE)
		return OPEN_NONE;

	// Enqueue
	if (m_nEnqueue) {
		// Search for queued entry
		int nNext = OPEN_NONE;
		for (int i = 0; i < m_pListFile->GetCount(); i++) {
			FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(i);
			// Decrease queue count
			if (p && p->dwQueue == 1)
				nNext = i;
			if (p && p->dwQueue > 0)
				p->dwQueue--;
		}
		if (nNext != OPEN_NONE) {
			m_nEnqueue--;
			return nNext;
		} else {
			// unset Enqueue flag
			m_nEnqueue = 0;
		}

	}

	if (m_pOptions->m_fShuffle) {
		// シャッフル再生時
		// すべてのフラグがセットされている場合
		int nNext = OPEN_NONE;
		for (int i = 0; i < m_pListFile->GetCount(); i++) {
			FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(i);
			if (p && !p->fShuffle) {
				nNext = i;
				break;
			}
		}
		if (nNext == OPEN_NONE) {
			ResetShuffle();
			if (fError || (fEos && m_pOptions->m_fRepeat == COptions::RepeatNone))
				return OPEN_NONE;
		}

		// シャッフル再生されていないものを探す
		while (TRUE) {
			nNext = frand(slopeP,m_pListFile->GetCount(),m_pOptions->m_fRandomSlopeRate,m_pOptions->m_fRandomSlopeBase);
			FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(nNext);
			if (!p->fShuffle) {
				return nNext;
			}
		}
		return OPEN_NONE;
	}
	else {
		// 通常再生時
		int nMax = m_pListFile->GetCount() - 1;
		if (m_nCurrent == nMax) {
			if (fError || (fEos && m_pOptions->m_fRepeat == COptions::RepeatNone))
				return OPEN_NONE;
			return 0;
		}
		return m_nCurrent + 1;
	}
}

int CMainWnd::GetPrevIndex()
{
	if (m_nCurrent == OPEN_NONE)
		return OPEN_NONE;

	if (m_pOptions->m_fShuffle) {
		// シャッフル再生時
		return m_nCurrent;
	}
	else {
		// 通常再生時
		if (m_nCurrent == 0)
			return m_pListFile->GetCount() - 1;
		return m_nCurrent - 1;
	}
}

void CMainWnd::ResetShuffle()
{
	for (int i = 0; i < m_pListFile->GetCount(); i++) {
		FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(i);
		if (p) p->fShuffle = FALSE;
	}
}

// 操作
BOOL CMainWnd::Play(int nIndex)
{
	if (nIndex != OPEN_NONE && m_nCurrent != nIndex) {
		if (!Open(nIndex))
			return FALSE;
	}

	MAP_SetOptions(m_hMap, &m_pOptions->m_Options);
	MAP_SetStreamingOptions(m_hMap, &m_pOptions->m_StreamingOptions);
	
	if (!MAP_Play(m_hMap)) {
		// retry
		Open(nIndex != OPEN_NONE ? nIndex : m_nCurrent);
		return MAP_Play(m_hMap);
	}
	return TRUE;
}

void CMainWnd::Pause()
{
	MAP_Pause(m_hMap);
}

void CMainWnd::Stop()
{
	MAP_Stop(m_hMap);
	//UpdateTime();
	//UpdateStatus();

	MSG msg;
	while (PeekMessage(&msg, NULL, MAP_MSG_STATUS, MAP_MSG_STATUS, PM_REMOVE));

	KillTimer(m_hWnd, ID_TIMER_STREAMINGWAIT);
}

void CMainWnd::OpenFirstFile()
{
	if (m_nCurrent != OPEN_NONE)
		return;
	
	Open(0);
}

BOOL CMainWnd::Enqueue(int nIndex)
{
	if (nIndex != OPEN_NONE) {
		FILEINFO* pInfo = (FILEINFO*)m_pListFile->GetAt(nIndex);
		if (pInfo && !pInfo->dwQueue) {
			pInfo->dwQueue = ++m_nEnqueue;
			return TRUE;
		} else {
			return FALSE;
		}

	}

	return TRUE;
}

BOOL CMainWnd::Dequeue(int nIndex)
{

	if (nIndex != OPEN_NONE && m_nEnqueue > 0) {
		FILEINFO* pInfo = (FILEINFO*)m_pListFile->GetAt(nIndex);
		DWORD dQueue;
		if (pInfo && pInfo->dwQueue) {
			dQueue = pInfo->dwQueue;
			pInfo->dwQueue = 0;
			m_nEnqueue--;

			// Decrease queue numbers
			if (m_nEnqueue) {
				for (int i = 0; i < m_pListFile->GetCount(); i++) {
				pInfo = (FILEINFO*)m_pListFile->GetAt(i);
					if (pInfo && pInfo->dwQueue && pInfo->dwQueue > dQueue) --pInfo->dwQueue;
				}
			}
			
			return TRUE;
		} else {
			return FALSE;
		}

	}

	return TRUE;
}

BOOL CMainWnd::ClearQueue()
{
	if(m_nEnqueue) {
		for (int i = 0; i < m_pListFile->GetCount(); i++) {
			FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(i);
			if (p && p->dwQueue) p->dwQueue = 0;
		}
		m_nEnqueue = 0;

		return TRUE;
	} else {
		return FALSE;
	}

	return TRUE;
}

// 情報
int CMainWnd::GetFileCount()
{
	return m_pListFile->GetCount();
}

FILEINFO* CMainWnd::GetInfo(int nIndex)
{
	return (FILEINFO*)m_pListFile->GetAt(nIndex);
}

BOOL CMainWnd::GetTitle(int nIndex, LPTSTR pszTitle, BOOL fForSaving)
{
	FILEINFO* pInfo = (FILEINFO*)m_pListFile->GetAt(nIndex);
	if (pInfo) {
		TCHAR szTitle[MAX_URL];
		if (IsURL(pInfo->szPath)) {
			if (MAP_GetStreamInfo(m_hMap, szTitle, NULL, NULL) &&
				nIndex == m_nCurrent) {
				_tcscpy(pszTitle, szTitle);
				_tcscpy(pInfo->szDisplayName, pszTitle);
			}
			else if (_tcslen(pInfo->szDisplayName))
				_tcscpy(pszTitle, pInfo->szDisplayName);
			else 
				_tcscpy(pszTitle, pInfo->szPath);
		}
		else {
			if (_tcslen(pInfo->tag.szTrack)) {
				if (_tcslen(pInfo->tag.szArtist))
					wsprintf(pszTitle, _T("%s - %s"), pInfo->tag.szArtist, pInfo->tag.szTrack);
				else
					wsprintf(pszTitle, _T("%s"), pInfo->tag.szTrack);
			}
			else if (_tcslen(pInfo->szDisplayName))
				_tcscpy(pszTitle, pInfo->szDisplayName);
			else {
				LPTSTR p = _tcsrchr(pInfo->szPath, _T('\\'));
				if (!p)
					p = pInfo->szPath;
				else 
					p++;

				_tcscpy(pszTitle, p);
				p = _tcsrchr(pszTitle, _T('.'));
				if (p) *p = NULL;
				_tcscpy(pInfo->szDisplayName, pszTitle);
			}
		}

		if(!fForSaving && m_pOptions->m_fShowTrackNo) {
			_tcscpy(szTitle, pszTitle);
			wsprintf(pszTitle, _T("%d. %s"), nIndex + 1, szTitle);
		}
		if(!fForSaving && m_pOptions->m_fShowTime && pInfo->info.nDuration)
			wsprintf(pszTitle, _T("%s (%d:%.2d)"), pszTitle, pInfo->info.nDuration / 60000, pInfo->info.nDuration / 1000 % 60);
		
		if(!fForSaving && pInfo->dwQueue)
			wsprintf(pszTitle, _T("%s [%d]"), pszTitle, pInfo->dwQueue);

		return TRUE;
	}
	return FALSE;
}

void CMainWnd::GetStreamTitle(LPTSTR pszTitle)
{
	*pszTitle = 0;

	MAP_STREAMING_STATUS status = MAP_GetStreamingStatus(m_hMap);
	if (status == MAP_STREAMING_CONNECTING)
		LoadString(GetInst(), IDS_MSG_CONNECTING, pszTitle, MAX_URL);
	else if (status == MAP_STREAMING_CONNECTED)
		MAP_GetStreamTitle(m_hMap, pszTitle);
	else if (status == MAP_STREAMING_BUFFERING) {
		MAP_STREAMING_OPTIONS options;
		MAP_GetStreamingOptions(m_hMap, &options);
		int n = MAP_GetStreamingBufferingCount(m_hMap) * 100 / options.nPreBuf;

		CTempStr str(IDS_FMT_BUFFERING);
		wsprintf(pszTitle, str, n);
	}
}

// protectedメンバ
BOOL CMainWnd::IsValidStream(LPTSTR pszFile)
{
	if (IsURL(pszFile))
		return TRUE;

	DWORD dwAttr = GetFileAttributes(pszFile);
	if (dwAttr == 0xFFFFFFFF)
		return FALSE;
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		return FALSE;
	return TRUE;

	//return MAP_IsValidStream(pszFile);
}

BOOL CMainWnd::Open(int nIndex)
{
	CWaitCursor wc;
	static BOOL s_fOpen = FALSE;

	FILEINFO* pInfo = (FILEINFO*)m_pListFile->GetAt(nIndex);
	if (!pInfo)
		return FALSE;

	Stop();
	UpdateTime();
	UpdateStatus();
	UpdateStreamTitle();

	MAP_SetOptions(m_hMap, &m_pOptions->m_Options);
	MAP_SetStreamingOptions(m_hMap, &m_pOptions->m_StreamingOptions);

	if (m_nCurrent == nIndex && s_fOpen)
		return TRUE;

	BOOL fRet = FALSE;
	if (IsURL(pInfo->szPath))
		fRet = MAP_OpenURL(m_hMap, pInfo->szPath);
	else
		fRet = MAP_Open(m_hMap, pInfo->szPath);

	if (fRet) {
		MAP_GetFileInfo(m_hMap, &pInfo->info);
		MAP_GetId3Tag(m_hMap, &pInfo->tag);

		if (m_pOptions->m_fShuffle)
			pInfo->fShuffle = TRUE;
	}

	m_fDrawVolume = FALSE;
	KillTimer(m_hWnd, ID_TIMER_VOLUME);

	m_nCurrent = nIndex;
	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
	if (m_pPlayListDlg) {
		m_pPlayListDlg->UpdatePlayList();
		m_pPlayListDlg->SetCurrent(nIndex);
	}

	s_fOpen = fRet;
	return fRet;
}

void CMainWnd::Close()
{
	Stop();
	MAP_Close(m_hMap);
}

void CMainWnd::Seek(int nMilisec)
{
	MAP_Seek(m_hMap, nMilisec);
}

BOOL CMainWnd::IsURL(LPTSTR pszFile)
{
	return _tcsncmp(pszFile, HTTP_PREFIX, _tcslen(HTTP_PREFIX)) == 0;
}