#include "GSPlayer2.h"
#include "MainWnd.h"

void RemoveReturn(LPSTR psz)
{
	// 文字列の最後の'\n'を削除
	LPSTR p = psz + strlen(psz) - 1;
	if (*p == '\n') *p = NULL;

	// 文字列の最後の' 'を削除
	while (psz[strlen(psz) - 1] == ' ')
		psz[strlen(psz) - 1] = NULL;
}

BOOL CMainWnd::IsPlayList(LPTSTR pszFile)
{
	TCHAR szExt[5];
	LPTSTR psz = _tcsrchr(pszFile, _T('.'));
	if (!psz)
		return FALSE;
	
	_tcsncpy(szExt, ++psz, 4);
	szExt[4] = NULL;
	_tcslwr(szExt);
	return _tcscmp(szExt, M3U_FILE_EXT) == 0 || _tcscmp(szExt, M3U8_FILE_EXT) == 0 || _tcscmp(szExt, PLS_FILE_EXT) == 0;
}

void CMainWnd::SavePlayList(LPTSTR pszFile)
{
	::DeleteFile(pszFile);
	if (!m_pListFile->GetCount())
		return;

	SaveM3uPlayList(pszFile); // M3Uのみ対応
}

void CMainWnd::LoadPlayList(LPTSTR pszFile)
{
	CWaitCursor wc;

	TCHAR szExt[5];
	LPTSTR psz = _tcsrchr(pszFile, _T('.'));
	if (!psz)
		return;
	
	_tcsncpy(szExt, ++psz, 4);
	szExt[4] = NULL;
	_tcslwr(szExt);
	if (_tcscmp(szExt, M3U_FILE_EXT) == 0 || _tcscmp(szExt, M3U8_FILE_EXT) == 0)
		LoadM3uPlayList(pszFile);
	else if (_tcscmp(szExt, PLS_FILE_EXT) == 0)
		LoadPlsPlayList(pszFile);
}

void CMainWnd::SaveM3uPlayList(LPTSTR pszFile)
{
	// M3Uの形式
	// #EXTM3U
	// #EXTINF:<演奏時間(秒)>,<タイトル>
	// <ファイルのパス>
	// ...

	// M3U8 Support
	BOOL isM3U8 = FALSE;
	TCHAR szExt[5];
	LPTSTR pszPos = _tcsrchr(pszFile, _T('.'));
	if (pszPos) {
		_tcsncpy(szExt, ++pszPos, 4);
		szExt[4] = NULL;
		_tcslwr(szExt);
		isM3U8 = _tcscmp(szExt, M3U8_FILE_EXT) == 0;
	}

#ifdef _UNICODE
	char szFile[MAX_PATH * 2];
//	WideCharToMultiByte(CP_ACP, NULL, pszFile, -1, szFile, MAX_PATH, NULL, NULL);
	FILE* fp = _tfopen(pszFile, _T("w"));
#else
	FILE* fp = fopen(pszFile, "w");
#endif
	if (!fp) return;

	TCHAR szPath[MAX_PATH];
	_tcscpy(szPath, pszFile);
	LPTSTR psz = _tcsrchr(szPath, _T('\\'));
	if (psz) *(psz + 1) = NULL;

	// ヘッダを記述
	fprintf(fp, "#EXTM3U\n");

	// リストを記述
	for (int i = 0; i < m_pListFile->GetCount(); i++) {
		FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(i);
		if (p) {
			TCHAR szTitle[MAX_PATH];
			GetTitle(i, szTitle, TRUE);
#ifdef _UNICODE
			TCHAR szBuff[MAX_PATH * 2]; // ...越えることないでしょ？

			// #EXTINF:<演奏時間(秒)>,<タイトル>
			wsprintf(szBuff, _T("#EXTINF:%d,%s\n"), 
				p->info.nDuration == 0 ? -1 : p->info.nDuration / 1000, szTitle);
			WideCharToMultiByte(isM3U8 ? CP_UTF8 : CP_ACP, NULL, szBuff, -1, szFile, MAX_PATH * 2, NULL, NULL);
			fprintf(fp, szFile);
			
			// <ファイルのパス>
			if (_tcsncmp(szPath, p->szPath, _tcslen(szPath)) == 0)
				WideCharToMultiByte(isM3U8 ? CP_UTF8 : CP_ACP, NULL, p->szPath + _tcslen(szPath), -1, szFile, MAX_PATH * 2, NULL, NULL);
			else
				WideCharToMultiByte(isM3U8 ? CP_UTF8 : CP_ACP, NULL, p->szPath, -1, szFile, MAX_PATH * 2, NULL, NULL);
			fprintf(fp, szFile);
			fprintf(fp, "\n");
#else
			// #EXTINF:<演奏時間(秒)>,<タイトル>
			fprintf(fp, "#EXTINF:%d", 
				p->info.nDuration == 0 ? -1 : p->info.nDuration / 1000);
			fprintf(fp, ",%s\n", szTitle);

			// <ファイルのパス>
			if (_tcsncmp(szPath, p->szPath, _tcslen(szPath)) == 0)
				fprintf(fp, p->szPath + _tcslen(szPath)); // 相対パス
			else
				fprintf(fp, p->szPath); // 絶対パス
			fprintf(fp, "\n");
#endif
		}
	}
	fclose(fp);
}

void CMainWnd::LoadM3uPlayList(LPTSTR pszFile)
{

	// M3U8 Support
	BOOL isM3U8 = FALSE;
	TCHAR szExt[5];
	LPTSTR pszPos = _tcsrchr(pszFile, _T('.'));
	if (pszPos) {
		_tcsncpy(szExt, ++pszPos, 4);
		szExt[4] = NULL;
		_tcslwr(szExt);
		isM3U8 = _tcscmp(szExt, M3U8_FILE_EXT) == 0;
	}

	char szBuff[MAX_PATH * 2];
	TCHAR szTitle[MAX_PATH] = {0};
	long nTime;
#ifdef _UNICODE
	TCHAR szFile[MAX_PATH];
//	WideCharToMultiByte(CP_ACP, NULL, pszFile, -1, szBuff, MAX_PATH, NULL, NULL);
	FILE* fp = _tfopen(pszFile, _T("r"));
#else
	FILE* fp = fopen(pszFile, "r");
#endif
	if (!fp) return;

	TCHAR szPath[MAX_PATH];
	_tcscpy(szPath, pszFile);
	LPTSTR psz = _tcsrchr(szPath, _T('\\'));
	if (psz) *psz = NULL;

#define EXTINF	"#EXTINF"
	while (fgets(szBuff, MAX_PATH * 2, fp)) {
		RemoveReturn(szBuff);
		if (szBuff[0] == '#') {
			if (strncmp(szBuff, EXTINF, strlen(EXTINF)) == 0) {
				LPSTR q = strchr(szBuff, ':');
				LPSTR p = strchr(szBuff, ',');
				char szTime[6] = {0};
				if (p && q) {
					strncpy(szTime, q+1, p-q-1);
					nTime = atoi(szTime);
					if (nTime == -1) {
						nTime = 0;
					} else {
						nTime *= 1000;
					}
				}
				if (p++) {
					while (*p == ' ') p++;
					if (strlen(p)) {
#ifdef _UNICODE
						MultiByteToWideChar(isM3U8 ? CP_UTF8 : CP_ACP, 0, p, -1, szTitle, MAX_PATH);
#else
						strncpy(szTitle, p, MAX_PATH);
						szTitle[MAX_PATH - 1] = NULL;
#endif
					}
				}
			}
			continue;
		}

		if (!strlen(szBuff))
			continue;
#ifdef _UNICODE
		MultiByteToWideChar(isM3U8 ? CP_UTF8 : CP_ACP, 0, szBuff, -1, szFile, MAX_PATH);
		AddFile2(szPath, szFile, _tcslen(szTitle) ? szTitle : NULL, nTime);
#else
		AddFile2(szPath, szBuff, _tcslen(szTitle) ? szTitle : NULL, nTime);
#endif
		szTitle[0] = NULL;
	}
	fclose(fp);
}

void CMainWnd::LoadPlsPlayList(LPTSTR pszFile)
{
	TCHAR sz[MAX_PATH];
	TCHAR szFile[MAX_PATH];
	TCHAR szTitle[MAX_PATH] = {0};
#ifdef _UNICODE
//	char szBuff[MAX_PATH * 2];
//	WideCharToMultiByte(CP_ACP, NULL, pszFile, -1, szBuff, MAX_PATH, NULL, NULL);
	FILE* fp = _tfopen(pszFile, _T("r"));
#else
	FILE* fp = fopen(pszFile, "r");
#endif
	if (!fp) return;

	TCHAR szPath[MAX_PATH];
	_tcscpy(szPath, pszFile);
	LPTSTR psz = _tcsrchr(szPath, _T('\\'));
	if (psz) *psz = NULL;

#define SECTION_PLAYLIST	_T("playlist")
#define KEY_FILE			_T("File%d")
#define KEY_TITLE			_T("Title%d")

	int nIndex = 1;
	while (TRUE) {
		// File
		wsprintf(sz, KEY_FILE, nIndex);
		if (!GetKeyString(fp, SECTION_PLAYLIST, sz, szFile))
			break;

		// Title
		wsprintf(sz, KEY_TITLE, nIndex);
		GetKeyString(fp, SECTION_PLAYLIST, sz, szTitle);
		AddFile2(szPath, szFile, _tcslen(szTitle) ? szTitle: NULL);
		szTitle[0] = NULL;

		nIndex++;
	}

	fclose(fp);
}

void CMainWnd::AddFile2(LPTSTR pszPath, LPTSTR pszFile, LPTSTR pszTitle, long nDur)
{
	if (!m_pOptions->m_fFastLoad) { 
		if (IsValidStream(pszFile)) {
			AddFile(pszFile, pszTitle, nDur);
			return;
		}
	} else {
		// FastLoad Hack for PPC
		if (*pszFile == '\\') {
			AddFile(pszFile, pszTitle, nDur);
			return;
		} else if (IsURL(pszFile)) {
			AddFile(pszFile, pszTitle, nDur);
			return;
		}
	}

	TCHAR szFile[MAX_PATH];
	wsprintf(szFile, _T("%s\\%s"), pszPath, pszFile);
	AddFile(szFile, pszTitle, nDur);
}