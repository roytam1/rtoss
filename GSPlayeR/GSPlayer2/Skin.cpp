#include "GSPlayer2.h"
#include "Skin.h"

CSkin::CSkin()
{
	m_hbmpBkgnd = NULL;

	int i;
	for (i = 0; i <= IMAGELIST_MAX; i++) {
		m_himlImageLists[i] = NULL;
	}
	ResetColors();
}

CSkin::~CSkin()
{
	Unload();
}

void CSkin::Load(LPCTSTR pszFile)
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
	if (GetKeyString(fp, SECTION_SKIN, KEY_VERSION, szBuff))
		nVersion = _tcstol(szBuff, NULL, 10);

	// Background
	GetKeyString(fp, SECTION_SKIN, KEY_BKGROUND, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_hbmpBkgnd = LoadBitmap(szPath);
	if (!m_hbmpBkgnd)
		m_hbmpBkgnd = ::LoadBitmap(hInst, (LPCTSTR)IDB_BACK);

	// Number
	GetKeyString(fp, SECTION_SKIN, KEY_NUMBER, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_himlImageLists[IMAGELIST_NUMBER] = 
		LoadImageList(IML_NUMBER_WIDTH, IML_NUMBER_HEIGHT, szPath, IDB_NUMBER);
	
	// Status
	GetKeyString(fp, SECTION_SKIN, KEY_STATUS, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_himlImageLists[IMAGELIST_STATUS] = 
		LoadImageList(IML_STATUS_WIDTH, IML_STATUS_HEIGHT, szPath, IDB_STATUS);
	
	// PlayOption
	GetKeyString(fp, SECTION_SKIN, KEY_PLAYOPT, szBuff);
	wsprintf(szPath, _T("%s%s"), szBase, szBuff);
	m_himlImageLists[IMAGELIST_PLAYOPT] = 
		LoadImageList(IML_PLAYOPT_WIDTH, IML_PLAYOPT_HEIGHT, szPath, IDB_PLAYOPTION);
	
	// Disp
	GetKeyString(fp, SECTION_SKIN, KEY_DISP, szBuff);
	if (_tcslen(szBuff))
		m_fDispVisible = _tcstol(szBuff, 0, 10) ? TRUE : FALSE; 

	// DispColor 
	GetKeyString(fp, SECTION_SKIN, KEY_DISPCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_DISP] = cr;
	
	// PeekColorL
	GetKeyString(fp, SECTION_SKIN, KEY_PEEKCOLORL, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_PEEKL] = cr;

	// PeekColorR
	GetKeyString(fp, SECTION_SKIN, KEY_PEEKCOLORR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_PEEKR] = cr;

	// InfoColor
	GetKeyString(fp, SECTION_SKIN, KEY_INFOCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_INFO] = cr;

	// TitleColor
	GetKeyString(fp, SECTION_SKIN, KEY_TITLECOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_TITLE] = cr;

	// SeekColor
	GetKeyString(fp, SECTION_SKIN, KEY_SEEKCOLOR, szBuff);
	cr = StrToColor(szBuff);
	if (cr != -1)
		m_nColors[COLOR_SEEK] = cr;

	if (nVersion < SKIN_CURRENT_VERSION) {
		// Button
		GetKeyString(fp, SECTION_SKIN, KEY_BUTTON, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_BUTTON] = 
			LoadImageList(IML_BUTTON_WIDTH, IML_BUTTON_HEIGHT, szPath, IDB_BUTTON);
	}
	else {
		// ButtonNormal
		GetKeyString(fp, SECTION_SKIN, KEY_BUTTONNORMAL, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_BUTTON_NORMAL] = 
			LoadImageList(IML_BUTTON2_WIDTH, IML_BUTTON2_HEIGHT, szPath, IDB_BUTTON_NORMAL);

		// ButtonPush
		GetKeyString(fp, SECTION_SKIN, KEY_BUTTONPUSH, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_BUTTON_PUSH] = 
			LoadImageList(IML_BUTTON2_WIDTH, IML_BUTTON2_HEIGHT, szPath, IDB_BUTTON_PUSH);

		// VolumeSlider
		GetKeyString(fp, SECTION_SKIN, KEY_VOLUME_SLIDER, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_VOLSLIDER] = 
			LoadImageList(IML_VOLSLIDER_WIDTH, IML_VOLSLIDER_HEIGHT, szPath, IDB_VOLSLIDER);

		// VolumeTrack
		GetKeyString(fp, SECTION_SKIN, KEY_VOLUME_TRACK, szBuff);
		wsprintf(szPath, _T("%s%s"), szBase, szBuff);
		m_himlImageLists[IMAGELIST_VOLTRACK] = 
			LoadImageList(IML_VOLTRACK_WIDTH, IML_VOLTRACK_HEIGHT, szPath, IDB_VOLTRACK);
	}

	if (fp) fclose(fp);
}

void CSkin::Unload()
{
	if (m_hbmpBkgnd) {
		DeleteObject(m_hbmpBkgnd);
		m_hbmpBkgnd = NULL;
	}

	int i;
	for (i = 0; i <= IMAGELIST_MAX; i++) {
		if (m_himlImageLists[i]) {
			ImageList_Destroy(m_himlImageLists[i]);
			m_himlImageLists[i] = NULL;
		}
	}
	ResetColors();
}

HBITMAP	CSkin::GetBkImage()
{
	return m_hbmpBkgnd;
}

HIMAGELIST CSkin::GetImageList(int nIndex)
{
	if (nIndex > IMAGELIST_MAX ||
		nIndex < 0)
		return 0;

	return m_himlImageLists[nIndex];
}

COLORREF CSkin::GetColor(int nIndex)
{
	if (nIndex > COLOR_MAX ||
		nIndex < 0)
		return 0;

	return m_nColors[nIndex];
}

BOOL CSkin::IsDispVisible()
{
	return m_fDispVisible;
}

void CSkin::ResetColors()
{
	m_fDispVisible = TRUE;
	m_nColors[COLOR_DISP] = DISP_COLOR;
	m_nColors[COLOR_INFO] = INFO_COLOR;
	m_nColors[COLOR_TITLE] = TITLE_COLOR;
	m_nColors[COLOR_SEEK] = SEEK_COLOR;
	m_nColors[COLOR_PEEKL] = PEEK_COLOR_L;
	m_nColors[COLOR_PEEKR] = PEEK_COLOR_R;
}

HIMAGELIST CSkin::LoadImageList(int cx, int cy, LPCTSTR pszFile, UINT uDef)
{
#ifdef _WIN32_WCE 
	UINT uFlags = ILC_COLOR|ILC_MASK;
#else
	UINT uFlags = ILC_COLOR24|ILC_MASK;
#endif
	HIMAGELIST himl = ImageList_Create(cx, cy, uFlags, 0, 0);
	if (!himl)
		return NULL;
	HBITMAP hbmp = LoadBitmap(pszFile);
	if (!hbmp)
		hbmp = ::LoadBitmap(GetInst(), (LPCTSTR)uDef);
	ImageList_AddMasked(himl, hbmp, MASK_COLOR);
	DeleteObject(hbmp);
	return himl;
}

HBITMAP CSkin::LoadBitmap(LPCTSTR pszFile)
{
#ifdef _WIN32_WCE
	return SHLoadDIBitmap(pszFile);
#else
	return (HBITMAP)LoadImage(NULL, pszFile, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
#endif
}

COLORREF CSkin::StrToColor(LPTSTR psz)
{
	int r, g, b;
	
	// r
	LPTSTR p = _tcschr(psz, _T(','));
	if (!p) return -1;
	
	*p = NULL;
	r = _tcstol(psz, NULL, 10);

	// g
	psz = p + 1;
	p = _tcschr(psz, _T(','));
	if (!p) return -1;

	*p = NULL;
	g = _tcstol(psz, NULL, 10);

	// b
	psz = p + 1;
	b = _tcstol(psz, NULL, 10);

	return RGB(r, g, b);
}