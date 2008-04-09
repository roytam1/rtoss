#include "GSPlayer2.h"
#include "PpcSkin.h"

CPpcSkin::CPpcSkin()
{
}

CPpcSkin::~CPpcSkin()
{
}

void CPpcSkin::Load(LPCTSTR pszFile)
{
	Unload();

	FILE* fp = NULL;
	COLORREF cr;
	TCHAR szBuff[MAX_PATH];
	TCHAR szBase[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	HINSTANCE hInst = GetInst();

	*szBase = NULL;
	if (pszFile && _tcslen(pszFile)) {
	// ファイルを開く
#ifdef _UNICODE
//		char szFile[MAX_PATH * 2];
//		WideCharToMultiByte(CP_ACP, NULL, pszFile, -1, szFile, MAX_PATH, NULL, NULL);
		fp = _tfopen(pszFile, _T("r"));
#else
		fp = fopen(pszFile, "r");
#endif
		_tcscpy(szBase, pszFile);
		LPTSTR p = _tcsrchr(szBase, _T('\\'));
		if (p) *(p + 1) = NULL;
	}
	
	// Version
	int nVersion = SKIN_CURRENT_VERSION;
	if (GetKeyString(fp, SECTION_SKIN_PPC, KEY_VERSION, szBuff))
		nVersion = _tcstol(szBuff, NULL, 10);

	// Background
	wsprintf(szPath, _T("%s%dx%d"), KEY_BKGROUND, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	GetKeyString(fp, SECTION_SKIN_PPC, szPath, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_hbmpBkgnd = LoadBitmap(szPath);
	if (!m_hbmpBkgnd) {
		GetKeyString(fp, SECTION_SKIN_PPC, KEY_BKGROUND, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_hbmpBkgnd = LoadBitmap(szPath);
	}

	// Number
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_NUMBER, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_himlImageLists[IMAGELIST_NUMBER] = 
		LoadImageList(IML_NUMBER_WIDTH, IML_NUMBER_HEIGHT, szPath, IDB_NUMBER);
	
	// Status
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_STATUS, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_himlImageLists[IMAGELIST_STATUS] = 
		LoadImageList(IML_STATUS_WIDTH, IML_STATUS_HEIGHT, szPath, IDB_STATUS);
	
	// PlayOption
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_PLAYOPT, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_himlImageLists[IMAGELIST_PLAYOPT] = 
		LoadImageList(IML_PLAYOPT_WIDTH, IML_PLAYOPT_HEIGHT, szPath, IDB_PLAYOPTION);
	
	// Disp
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_DISP, szBuff);
	if (_tcslen(szBuff))
		m_fDispVisible = _tcstol(szBuff, 0, 10) ? TRUE : FALSE; 

	// DispColor 
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_DISPCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_DISP] = cr;
	
	// PeekColorL
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_PEEKCOLORL, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_PEEKL] = cr;

	// PeekColorR
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_PEEKCOLORR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_PEEKR] = cr;

	// InfoColor
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_INFOCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_INFO] = cr;

	// TitleColor
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_TITLECOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_TITLE] = cr;

	// SeekColor
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_SEEKCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_SEEK] = cr;

	// PlayList
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_PLAYLIST, szBuff);
	if (_tcslen(szBuff))
		m_fPlaylistVisible = _tcstol(szBuff, 0, 10) ? TRUE : FALSE; 

	// PlayListBackColor
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_PLAYLISTBACKCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors2[COLOR2_PLAYLISTBACK] = cr;

	// PlayListTextColor
	GetKeyString(fp, SECTION_SKIN_PPC, KEY_PLAYLISTTEXTCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors2[COLOR2_PLAYLISTTEXT] = cr;
	
	if (nVersion < SKIN_CURRENT_VERSION) {
		// Button
		GetKeyString(fp, SECTION_SKIN_PPC, KEY_BUTTON, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_BUTTON] = 
			LoadImageList(IML_BUTTON_WIDTH, IML_BUTTON_HEIGHT, szPath, IDB_BUTTON);
	}
	else {
		// ButtonNormal
		GetKeyString(fp, SECTION_SKIN_PPC, KEY_BUTTONNORMAL, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_BUTTON_NORMAL] = 
			LoadImageList(IML_BUTTON2_WIDTH, IML_BUTTON2_HEIGHT, szPath, IDB_BUTTON_NORMAL);

		// ButtonPush
		GetKeyString(fp, SECTION_SKIN_PPC, KEY_BUTTONPUSH, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_BUTTON_PUSH] = 
			LoadImageList(IML_BUTTON2_WIDTH, IML_BUTTON2_HEIGHT, szPath, IDB_BUTTON_PUSH);

		// VolumeSlider
		GetKeyString(fp, SECTION_SKIN_PPC, KEY_VOLUME_SLIDER, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_VOLSLIDER] = 
			LoadImageList(IML_VOLSLIDER_WIDTH, IML_VOLSLIDER_HEIGHT, szPath, IDB_VOLSLIDER);

		// VolumeTrack
		GetKeyString(fp, SECTION_SKIN_PPC, KEY_VOLUME_TRACK, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_VOLTRACK] = 
			LoadImageList(IML_VOLTRACK_WIDTH, IML_VOLTRACK_HEIGHT, szPath, IDB_VOLTRACK);

	}

	if (fp) fclose(fp);
}

COLORREF CPpcSkin::GetColor2(int nIndex)
{
	if (nIndex > COLOR2_MAX || nIndex < 0)
		return 0;

	return m_nColors2[nIndex];
}
	
void CPpcSkin::ResetColors()
{
	CSkin::ResetColors();

	m_fPlaylistVisible = TRUE;
	m_nColors2[COLOR2_PLAYLISTBACK] = DISP_COLOR;
	m_nColors2[COLOR2_PLAYLISTTEXT] = TITLE_COLOR;
}
