#ifndef __PPCSKIN_H__
#define __PPCSKIN_H__

#include "Skin.h"

#define SECTION_SKIN_PPC		_T("GSPlayer Skin PocketPC")
#define KEY_PLAYLIST			_T("Playlist")
#define KEY_PLAYLISTBACKCOLOR	_T("PlaylistBackColor")
#define KEY_PLAYLISTTEXTCOLOR	_T("PlaylistTextColor")

class CPpcSkin : public CSkin
{
public:
	CPpcSkin();
	virtual ~CPpcSkin();

	virtual void Load(LPCTSTR pszFile = NULL);

#define COLOR2_PLAYLISTBACK		0
#define COLOR2_PLAYLISTTEXT		1
#define COLOR2_MAX		COLOR2_PLAYLISTTEXT
	virtual COLORREF GetColor2(int nIndex);
	virtual BOOL IsPlaylistVisible() {return m_fPlaylistVisible;}
	
protected:
	virtual void ResetColors();

protected:
	int		m_nColors2[COLOR2_MAX + 1];
	BOOL	m_fPlaylistVisible;
};

#endif // __PPCSKIN_H__