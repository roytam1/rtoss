#include "GSPlayer2.h"
#ifdef _WIN32_WCE_PPC
#include "EffectDlgPPC.h"
#else
#include "EffectDlg.h"
#endif

#define SECTION_GSPLAYERW	_T("GSPlayer")
#define SECTION_EQUALIZERW	_T("GSPlayer Equalizer")
#define SECTION_SURROUNDW	_T("GSPlayer Surround")
#define SECTION_3DCHORUSW	_T("GSPlayer 3D Chorus")
#define SECTION_REVERBW		_T("GSPlayer Reverb")
#define SECTION_ECHOW		_T("GSPlayer Echo")
#define SECTION_BASSBOOSTW	_T("GSPlayer BassBoost")
#define KEY_EFFECT_VERSIONW	_T("Version")
#define KEY_ENABLEW			_T("Enable")
#define KEY_PREAMPW			_T("Preamp")
#define KEY_RATEW			_T("Rate")
#define KEY_DELAYW			_T("Delay")
#define KEY_AUTOPREAMPW		_T("AutoPreamp")
#define SECTION_GSPLAYER	"GSPlayer"
#define SECTION_EQUALIZER	"GSPlayer Equalizer"
#define SECTION_SURROUND	"GSPlayer Surround"
#define SECTION_3DCHORUS	"GSPlayer 3D Chorus"
#define SECTION_REVERB		"GSPlayer Reverb"
#define SECTION_ECHO		"GSPlayer Echo"
#define SECTION_BASSBOOST	"GSPlayer BassBoost"
#define KEY_ENABLE			"Enable"
#define KEY_PREAMP			"Preamp"
#define KEY_RATE			"Rate"
#define KEY_DELAY			"Delay"
#define KEY_AUTOPREAMP		"AutoPreamp"
#define KEY_EFFECT_VERSION	"Version"
#define VALUE_VERSION		100
#define EFFECT_FILE_EXT		_T("gpe")
#define PRESET_DIR			_T("Preset")

BOOL GetPresetDir(LPTSTR pszPath)
{
#if defined(_WIN32_WCE) && _WIN32_WCE < 300
	_tcscpy(pszPath, _T("\\My Documents"));
#elif defined(_WIN32_WCE)
	SHGetSpecialFolderPath(NULL, pszPath, CSIDL_PERSONAL, FALSE);
#else // win32
	GetModuleFileName(GetInst(), pszPath, MAX_PATH);
	LPTSTR p = _tcsrchr(pszPath, _T('\\'));
	if (p) *p = NULL;

	// SHGetSpecialFolderPathがない環境 (WinNT4 SP6 without IE4)
	// のユーザーから苦情がきたため
	// こんなことしなくてもよいのでは？ (古い環境に対応していてはきりがない)
	BOOL (WINAPI *pSHGetSpecialFolderPath)(HWND hwndOwner, LPTSTR lpszPath, int nFolder, BOOL fCreate) = NULL;
	HINSTANCE hInstDll = (HINSTANCE)LoadLibrary(_T("shell32.dll"));
	if (hInstDll) {
#ifdef _UNICODE
#ifdef _WIN32_WCE
		(FARPROC&)pSHGetSpecialFolderPath = GetProcAddress(hInstDll, _T("SHGetSpecialFolderPathW"));
#else
		(FARPROC&)pSHGetSpecialFolderPath = GetProcAddress(hInstDll, "SHGetSpecialFolderPathW");
#endif
#else
		(FARPROC&)pSHGetSpecialFolderPath = GetProcAddress(hInstDll, _T("SHGetSpecialFolderPathA"));
#endif
		if (pSHGetSpecialFolderPath)
			pSHGetSpecialFolderPath(NULL, pszPath, CSIDL_PERSONAL, FALSE);
		FreeLibrary(hInstDll);
	}
#endif
	if (_tcslen(pszPath) + _tcslen(PRESET_DIR) + 2 > MAX_PATH)
		return FALSE;
	_tcscat(pszPath, _T("\\"));
	_tcscat(pszPath, PRESET_DIR);
	if (GetFileAttributes(pszPath) == 0xFFFFFFFF)
		CreateDirectory(pszPath, NULL);

	return TRUE;
}

void CEffectDlg::PresetOnInitDialog(HWND hwndDlg)
{
	TCHAR szPath[MAX_PATH];
	TCHAR sz[MAX_PATH];
	WIN32_FIND_DATA wfd;

	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_PRESET);

	RECT rc;
	GetClientRect(hwndLV, &rc);
	ListView_SetExtendedListViewStyle(hwndLV, 
		ListView_GetExtendedListViewStyle(hwndLV) | LVS_EX_FULLROWSELECT);
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - 2;
	lvc.pszText = _T("Name");
	lvc.iSubItem = 0;
	ListView_InsertColumn(hwndLV, 0, &lvc);

	SendMessage(hwndLV, WM_SETFONT, SendMessage(hwndDlg, WM_GETFONT, 0, 0), 0);

	if (!GetPresetDir(szPath))
		return;
	
	if (_tcslen(szPath) + _tcslen(EFFECT_FILE_EXT) + 4 > MAX_PATH)
		return;

	wsprintf(sz, _T("%s\\*.%s"), szPath, EFFECT_FILE_EXT);
	HANDLE hFind = FindFirstFile(sz, &wfd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			LPTSTR p = _tcsrchr(wfd.cFileName, _T('.'));
			if (p) *p = NULL;
			
			LVITEM li;
			memset(&li, 0, sizeof(LVITEM));
			li.mask = LVIF_TEXT;
			li.iItem = ListView_GetItemCount(hwndLV);
			li.pszText = wfd.cFileName;
			ListView_InsertItem(hwndLV, &li);
		}
		while (FindNextFile(hFind, &wfd));
		FindClose(hFind);
	}

	EnableWindow(GetDlgItem(hwndDlg, IDC_DEL), FALSE);
}

BOOL CEffectDlg::PresetOnNotify(HWND hwndDlg, NMHDR* phdr)
{
	switch (phdr->code) {
	case NM_RETURN:
	case NM_DBLCLK:
		PresetOnLoad(hwndDlg);
		return TRUE;
	case LVN_KEYDOWN:
		if(((NMLVKEYDOWN*)phdr)->wVKey == VK_DELETE) {
			PresetOnDelete(hwndDlg);
			return TRUE;
		}
	case LVN_ITEMCHANGED:
		EnableWindow(GetDlgItem(hwndDlg, IDC_DEL), 
			ListView_GetSelectedCount(GetDlgItem(hwndDlg, IDC_LIST_PRESET)) ? TRUE : FALSE);
		return TRUE;
	}
	return FALSE;
}

void CEffectDlg::PresetOnSave(HWND hwndDlg)
{
	TCHAR szPath[MAX_PATH];
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_PRESET);

	for (int i = 0; i < sizeof(m_fSaveParam) / sizeof(int); i++)
		m_fSaveParam[i] = TRUE;
	*m_szNewName = NULL;

	int nCount = ListView_GetItemCount(hwndLV);
	for (i = 0; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED) == LVIS_SELECTED) {
			ListView_GetItemText(hwndLV, i, 0, m_szNewName, MAX_PATH);
			break;
		}
	}

	if (DialogBoxParam(GetInst(), (LPCTSTR)IDD_PRESETNAME_DLG, hwndDlg, PresetNameDlgProc, (LPARAM)this) != IDOK)
		return;

	if (!GetPresetDir(szPath))
		return;

	if (_tcslen(szPath) + _tcslen(EFFECT_FILE_EXT) + 3 > MAX_PATH)
		return;

	_tcscat(szPath, _T("\\"));
	_tcscat(szPath, m_szNewName);
	_tcscat(szPath, _T("."));
	_tcscat(szPath, EFFECT_FILE_EXT);
	SavePresetFile(szPath, m_fSaveParam[SAVE_PARAM_EQ], m_fSaveParam[SAVE_PARAM_SURROUND],
			m_fSaveParam[SAVE_PARAM_3DCHORUS], m_fSaveParam[SAVE_PARAM_REVERB], m_fSaveParam[SAVE_PARAM_ECHO], m_fSaveParam[SAVE_PARAM_BASSBOOST]);

	DWORD dwAttr = GetFileAttributes(szPath);
	if (dwAttr != 0xFFFFFFFF && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
		TCHAR szName[MAX_PATH];
		int nCount = ListView_GetItemCount(hwndLV);
		for (int i = 0; i < nCount; i++) {
			ListView_GetItemText(hwndLV, i, 0, szName, MAX_PATH);
			if (_tcsicmp(szName, m_szNewName) == 0) {
				ListView_SetItemState(hwndLV, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				return;
			}
		}

		LVITEM li;
		memset(&li, 0, sizeof(LVITEM));
		li.mask = LVIF_TEXT;
		li.iItem = ListView_GetItemCount(hwndLV);
		li.pszText = m_szNewName;
		nCount = ListView_InsertItem(hwndLV, &li);
		ListView_SetItemState(hwndLV, nCount, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}
}

void CEffectDlg::PresetOnLoad(HWND hwndDlg)
{
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_PRESET);
	int nCount = ListView_GetItemCount(hwndLV);
	if (!nCount)
		return;
	
	TCHAR szPath[MAX_PATH];
	TCHAR szName[MAX_PATH];
	if (!GetPresetDir(szPath))
		return;

	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED) == LVIS_SELECTED) {
			ListView_GetItemText(hwndLV, i, 0, szName, MAX_PATH);

			_tcscat(szPath, _T("\\"));
			_tcscat(szPath, szName);
			_tcscat(szPath, _T("."));
			_tcscat(szPath, EFFECT_FILE_EXT);

			LoadPresetFile(szPath);
			return;
		}
	}
}

void CEffectDlg::PresetOnDelete(HWND hwndDlg)
{
	CTempStr str;
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_PRESET);
	int nCount = ListView_GetItemCount(hwndLV);
	if (!nCount)
		return;

	TCHAR szMsg[MAX_PATH*2];
	TCHAR szPath[MAX_PATH];
	TCHAR szName[MAX_PATH];
	if (!GetPresetDir(szPath))
		return;

	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED) == LVIS_SELECTED) {
			ListView_GetItemText(hwndLV, i, 0, szName, MAX_PATH);

			str.Load(IDS_FMT_DEL_PRESET);
			wsprintf(szMsg, str, szName);
			str.Load(IDS_TITLE_DEL_PRESET);
			if (MessageBox(hwndDlg, szMsg, str, MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
				return;

			_tcscat(szPath, _T("\\"));
			_tcscat(szPath, szName);
			_tcscat(szPath, _T("."));
			_tcscat(szPath, EFFECT_FILE_EXT);

			if (DeleteFile(szPath))
				ListView_DeleteItem(hwndLV, i);
			return;
		}
	}
}

void CEffectDlg::SavePresetFile(LPTSTR pszFile, BOOL fEQ, BOOL fSurround, BOOL f3DChorus, 
								BOOL fReverb, BOOL fEcho, BOOL fBassBoost)
{
#ifdef _UNICODE
//	char sz[MAX_PATH * 2];
//	WideCharToMultiByte(CP_ACP, NULL, pszFile, -1, sz, MAX_PATH * 2, NULL, NULL);
	FILE* fp = _tfopen(pszFile, _T("w"));
#else
	FILE* fp = fopen(pszFile, "w");
#endif
	if (!fp)
		return;

	EFFECT effect;
	EQUALIZER eq;

	fprintf(fp, "[%s]\n", SECTION_GSPLAYER);
	fprintf(fp, "%s=%d\n", KEY_EFFECT_VERSION, VALUE_VERSION);

	if (fEQ) {
		MAP_GetEqualizer(m_hMap, &eq);
		fprintf(fp, "\n[%s]\n", SECTION_EQUALIZER);
		fprintf(fp, "%s=%d\n", KEY_ENABLE, eq.fEnable);
		fprintf(fp, "%s=%d\n", KEY_PREAMP, eq.preamp);
		for (int i = 0; i < 10; i++)
			fprintf(fp, "%d=%d\n", i, eq.data[i]);
	}
	
	if (fSurround) {
		MAP_GetEffect(m_hMap, EFFECT_SURROUND, &effect);
		fprintf(fp, "\n[%s]\n", SECTION_SURROUND);
		fprintf(fp, "%s=%d\n", KEY_ENABLE, effect.fEnable);
		fprintf(fp, "%s=%d\n", KEY_RATE, effect.nRate);
	}

	if (f3DChorus) {
		MAP_GetEffect(m_hMap, EFFECT_3DCHORUS, &effect);
		fprintf(fp, "\n[%s]\n", SECTION_3DCHORUS);
		fprintf(fp, "%s=%d\n", KEY_ENABLE, effect.fEnable);
		fprintf(fp, "%s=%d\n", KEY_RATE, effect.nRate);
	}

	if (fReverb) {
		MAP_GetEffect(m_hMap, EFFECT_REVERB, &effect);
		fprintf(fp, "\n[%s]\n", SECTION_REVERB);
		fprintf(fp, "%s=%d\n", KEY_ENABLE, effect.fEnable);
		fprintf(fp, "%s=%d\n", KEY_DELAY, effect.nDelay);
		fprintf(fp, "%s=%d\n", KEY_RATE, effect.nRate);
	}

	if (fEcho) {
		MAP_GetEffect(m_hMap, EFFECT_ECHO, &effect);
		fprintf(fp, "\n[%s]\n", SECTION_ECHO);
		fprintf(fp, "%s=%d\n", KEY_ENABLE, effect.fEnable);
		fprintf(fp, "%s=%d\n", KEY_DELAY, effect.nDelay);
		fprintf(fp, "%s=%d\n", KEY_RATE, effect.nRate);
	}
	
	if (fBassBoost) {
		fprintf(fp, "\n[%s]\n", SECTION_BASSBOOST);
		fprintf(fp, "%s=%d\n", KEY_RATE, MAP_GetBassBoostLevel(m_hMap));
	}
	
	fclose(fp);
}

void CEffectDlg::LoadPresetFile(LPTSTR pszFile)
{
#ifdef _UNICODE
//	char sz[MAX_PATH * 2];
//	WideCharToMultiByte(CP_ACP, NULL, pszFile, -1, sz, MAX_PATH * 2, NULL, NULL);
	FILE* fp = _tfopen(pszFile, _T("r"));
#else
	FILE* fp = fopen(pszFile, "r");
#endif
	if (!fp)
		return;

	int i;
	EFFECT effect;
	EQUALIZER eq;
	TCHAR szBuf[MAX_PATH];

	if (!GetKeyString(fp, SECTION_GSPLAYERW, KEY_EFFECT_VERSIONW, szBuf))
		goto done;

	if (_tcstol(szBuf, NULL, 10) != VALUE_VERSION)
		goto done;

	MAP_GetEqualizer(m_hMap, &eq);
	if (GetKeyString(fp, SECTION_EQUALIZERW, KEY_ENABLEW, szBuf))
		eq.fEnable = _tcstol(szBuf, NULL, 10);
	if (GetKeyString(fp, SECTION_EQUALIZERW, KEY_PREAMPW, szBuf))
		eq.preamp = _tcstol(szBuf, NULL, 10);
	for (i = 0; i < 10; i++) {
		TCHAR szKey[64];
		wsprintf(szKey, _T("%d"), i);
		if (GetKeyString(fp, SECTION_EQUALIZERW, szKey, szBuf))
			eq.data[i] = _tcstol(szBuf, NULL, 10);
	}
	MAP_SetEqualizer(m_hMap, &eq);
	
	MAP_GetEffect(m_hMap, EFFECT_SURROUND, &effect);
	if (GetKeyString(fp, SECTION_SURROUNDW, KEY_ENABLEW, szBuf))
		effect.fEnable = _tcstol(szBuf, NULL, 10);
	if (GetKeyString(fp, SECTION_SURROUNDW, KEY_RATEW, szBuf))
		effect.nRate = _tcstol(szBuf, NULL, 10);
	MAP_SetEffect(m_hMap, EFFECT_SURROUND, &effect);

	MAP_GetEffect(m_hMap, EFFECT_3DCHORUS, &effect);
	if (GetKeyString(fp, SECTION_3DCHORUSW, KEY_ENABLEW, szBuf))
		effect.fEnable = _tcstol(szBuf, NULL, 10);
	if (GetKeyString(fp, SECTION_3DCHORUSW, KEY_RATEW, szBuf))
		effect.nRate = _tcstol(szBuf, NULL, 10);
	MAP_SetEffect(m_hMap, EFFECT_3DCHORUS, &effect);

	MAP_GetEffect(m_hMap, EFFECT_REVERB, &effect);
	if (GetKeyString(fp, SECTION_REVERBW, KEY_ENABLEW, szBuf))
		effect.fEnable = _tcstol(szBuf, NULL, 10);
	if (GetKeyString(fp, SECTION_REVERBW, KEY_DELAYW, szBuf))
		effect.nDelay = _tcstol(szBuf, NULL, 10);
	if (GetKeyString(fp, SECTION_REVERBW, KEY_RATEW, szBuf))
		effect.nRate = _tcstol(szBuf, NULL, 10);
	MAP_SetEffect(m_hMap, EFFECT_REVERB, &effect);

	MAP_GetEffect(m_hMap, EFFECT_ECHO, &effect);
	if (GetKeyString(fp, SECTION_ECHOW, KEY_ENABLEW, szBuf))
		effect.fEnable = _tcstol(szBuf, NULL, 10);
	if (GetKeyString(fp, SECTION_ECHOW, KEY_DELAYW, szBuf))
		effect.nDelay = _tcstol(szBuf, NULL, 10);
	if (GetKeyString(fp, SECTION_ECHOW, KEY_RATEW, szBuf))
		effect.nRate = _tcstol(szBuf, NULL, 10);
	MAP_SetEffect(m_hMap, EFFECT_ECHO, &effect);
	
	i = MAP_GetBassBoostLevel(m_hMap);
	if (GetKeyString(fp, SECTION_BASSBOOSTW, KEY_RATEW, szBuf))
		i = _tcstol(szBuf, NULL, 10);
	MAP_SetBassBoostLevel(m_hMap, i);
done:
	fclose(fp);
}

static TCHAR s_invalid_chars[] = {
	_T('\\'), _T('/'), _T(':'), _T(','), _T(';'), _T('*'), _T('?'), _T('\"'), _T('<'), _T('>'), _T('|'), NULL
};

BOOL CALLBACK CEffectDlg::PresetNameDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CEffectDlg* pDlg;
	switch (uMsg) {
	case WM_INITDIALOG:
	{
		pDlg = (CEffectDlg*)lParam;
		TCHAR szPath[MAX_PATH];
		if (GetPresetDir(szPath))
			SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_NAME), EM_LIMITTEXT, 
							MAX_PATH - _tcslen(szPath) - _tcslen(EFFECT_FILE_EXT) - 3, 0);

		if (_tcslen(pDlg->m_szNewName))
			SetDlgItemText(hwndDlg, IDC_EDIT_NAME, pDlg->m_szNewName);

		if (pDlg->m_fSaveParam[SAVE_PARAM_EQ])
			SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_EQUALIZER), BM_SETCHECK, 1, 0);
		if (pDlg->m_fSaveParam[SAVE_PARAM_SURROUND])
			SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SURROUND), BM_SETCHECK, 1, 0);
		if (pDlg->m_fSaveParam[SAVE_PARAM_3DCHORUS])
			SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_3DCHORUS), BM_SETCHECK, 1, 0);
		if (pDlg->m_fSaveParam[SAVE_PARAM_REVERB])
			SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_REVERB), BM_SETCHECK, 1, 0);
		if (pDlg->m_fSaveParam[SAVE_PARAM_ECHO])
			SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ECHO), BM_SETCHECK, 1, 0);
		if (pDlg->m_fSaveParam[SAVE_PARAM_BASSBOOST])
			SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_BASSBOOST), BM_SETCHECK, 1, 0);

		return TRUE;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK) {
			GetDlgItemText(hwndDlg, IDC_EDIT_NAME, pDlg->m_szNewName, MAX_PATH);
			for (int i = 0; s_invalid_chars[i]; i++) {
				if (_tcschr(pDlg->m_szNewName, s_invalid_chars[i])) {
					SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_NAME), EM_SETSEL, 0, -1);
					SetFocus(GetDlgItem(hwndDlg, IDC_EDIT_NAME));
					return TRUE;
				}
			}

			pDlg->m_fSaveParam[SAVE_PARAM_EQ] = 
				SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_EQUALIZER), BM_GETCHECK, 0, 0);
			pDlg->m_fSaveParam[SAVE_PARAM_SURROUND] = 
				SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SURROUND), BM_GETCHECK, 0, 0);
			pDlg->m_fSaveParam[SAVE_PARAM_3DCHORUS] = 
				SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_3DCHORUS), BM_GETCHECK, 0, 0);
			pDlg->m_fSaveParam[SAVE_PARAM_REVERB] = 
				SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_REVERB), BM_GETCHECK, 0, 0);
			pDlg->m_fSaveParam[SAVE_PARAM_ECHO] = 
				SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ECHO), BM_GETCHECK, 0, 0);
			pDlg->m_fSaveParam[SAVE_PARAM_BASSBOOST] = 
				SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_BASSBOOST), BM_GETCHECK, 0, 0);

			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hwndDlg, LOWORD(wParam));
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}
