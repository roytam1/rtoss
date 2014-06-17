#pragma once

///////////////////////////////////////////////////////////////////////////////////////////////////
#define LANG_COUNT	(3)
enum { ui_lang_cn=0, ui_lang_jp=1, ui_lang_en=2 };

struct StringEntry
{
	DWORD	dwId;
	CString	rgszText[LANG_COUNT];	// CN, JP
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class CStringReader
{
public:
	CStringReader();
	virtual ~CStringReader();

	BOOL	Open(LPCTSTR szFileName);
	BOOL	BatchReadString(LPCTSTR szFileName, StringEntry *pSE, DWORD dwCount);
	BOOL	GetNextLine(StringEntry &se);
	BOOL	ReadLine(CString &szLine);
	VOID	Close();

protected:
	CFile	m_file;
	DWORD	m_dwLength;
	DWORD	m_dwPosition;
	LPBYTE	m_pbBuffer;
};
