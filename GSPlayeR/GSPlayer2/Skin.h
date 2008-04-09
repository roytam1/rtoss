#ifndef __SKIN_H__
#define __SKIN_H__

#include "MainWnd.h"

#define SKIN_CURRENT_VERSION	2
#define SECTION_SKIN			_T("GSPlayer Skin")
#define KEY_VERSION				_T("Version")
#define KEY_BKGROUND			_T("Background")
#define KEY_NUMBER				_T("Number")
#define KEY_BUTTON				_T("Button")
#define KEY_BUTTONNORMAL		_T("ButtonNormal")
#define KEY_BUTTONPUSH			_T("ButtonPush")
#define KEY_STATUS				_T("Status")
#define KEY_PLAYOPT				_T("PlayOption")
#define KEY_DISP				_T("Disp")
#define KEY_DISPCOLOR			_T("DispColor")
#define KEY_PEEKCOLORL			_T("PeekColorL")
#define KEY_PEEKCOLORR			_T("PeekColorR")
#define KEY_INFOCOLOR			_T("InfoColor")
#define KEY_TITLECOLOR			_T("TitleColor")
#define KEY_SEEKCOLOR			_T("SeekColor")
#define KEY_VOLUME_SLIDER		_T("VolumeSlider")
#define KEY_VOLUME_TRACK		_T("VolumeTrack")

#define IML_NUMBER_WIDTH		8
#define IML_NUMBER_HEIGHT		15
#define IML_BUTTON_WIDTH		16
#define IML_BUTTON_HEIGHT		16
#define IML_BUTTON2_WIDTH		25
#define IML_BUTTON2_HEIGHT		20
#define IML_STATUS_WIDTH		9
#define IML_STATUS_HEIGHT		9
#define IML_PLAYOPT_WIDTH		10
#define IML_PLAYOPT_HEIGHT		7
#define IML_VOLSLIDER_WIDTH		51
#define IML_VOLSLIDER_HEIGHT	20
#define IML_VOLTRACK_WIDTH		10
#define IML_VOLTRACK_HEIGHT		20

class CSkin
{
public:
	CSkin();
	virtual ~CSkin();
	virtual void Load(LPCTSTR pszFile = NULL);
	virtual void Unload();

	virtual HBITMAP		GetBkImage();
#define IMAGELIST_NUMBER		0
#define IMAGELIST_BUTTON		1
#define IMAGELIST_BUTTON_NORMAL	2
#define IMAGELIST_BUTTON_PUSH	3
#define IMAGELIST_STATUS		4
#define IMAGELIST_PLAYOPT		5
#define IMAGELIST_VOLSLIDER		6
#define IMAGELIST_VOLTRACK		7
#define IMAGELIST_MAX			IMAGELIST_VOLTRACK
	virtual HIMAGELIST	GetImageList(int nIndex);
#define COLOR_DISP			0
#define COLOR_INFO			1
#define COLOR_TITLE			2
#define COLOR_SEEK			3
#define COLOR_PEEKL			4
#define COLOR_PEEKR			5
#define COLOR_MAX			COLOR_PEEKR
	virtual COLORREF	GetColor(int nIndex);
	virtual BOOL IsDispVisible();

protected:
	virtual void ResetColors();
	virtual HIMAGELIST LoadImageList(int cx, int cy, LPCTSTR pszFile, UINT uDef);
	virtual HBITMAP LoadBitmap(LPCTSTR pszFile);
	virtual COLORREF StrToColor(LPTSTR pszColor);

protected:
	HBITMAP		m_hbmpBkgnd;
	HIMAGELIST	m_himlImageLists[IMAGELIST_MAX + 1];
	int			m_nColors[COLOR_MAX + 1];
	BOOL		m_fDispVisible;
};

#endif // __SKIN_H__