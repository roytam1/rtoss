// PokemonRomViewer.h : PokemonRomViewer 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"
#include "StringReader.h"
#include "WaitDlg.h"
#include "BaseDialog.h"
#include "BreedDlg.h"		// 1
#include "EvoDlg.h"			// 2
#include "ItemDlg.h"		// 3
#include "BerryDlg.h"		// 4
#include "SkillDlg.h"		// 5
#include "EncDlg.h"			// 6

#define APP_DLG_COUNT	(6)

#define GUI_FONT_NAME	"MS Gothic"
#define GUI_FONT_HEIGHT	12

/////////////////////////////////////////////////
enum { ui_count_none=0, ui_count_dec=1, ui_count_hex=2 };

enum { pm_order_internal = 0, pm_order_kanto = 1, pm_order_houen = 2 };

struct ConfigStruct
{
	DWORD			dwLang;
	DWORD			dwCount;
	CString			szRomPath;
	StringEntry *	pBreedNameList;		// 1
	StringEntry *	pSkillNameList;		// 2
	StringEntry *	pSkillEffList;		// 3
	StringEntry *	pSkillSpecList;		// 4
	StringEntry *	pItemNameList;		// 5
	StringEntry *	pItemDescList;		// 6
	StringEntry *	pSpecNameList;		// 7
	StringEntry *	pSpecDescList;		// 8
	StringEntry *	pEncNameList;		// 9
};

#define TEXT_FILE_COUNT	(9)

/////////////////////////////////////////////////
StringEntry * GetEncNameEntry(WORD wAreaId);
/*
VOID SetBit(LPVOID pFlags, DWORD_PTR dwBitIndex);
VOID ClrBit(LPVOID pFlags, DWORD_PTR dwBitIndex);
BYTE GetBit(LPCVOID pFlags, DWORD_PTR dwBitIndex);
*/
WORD ConvertBreed(WORD wBreed, BOOL b387To412);
WORD TextToBreed(LPCTSTR szText);
VOID FormatSkillDesc(WORD wSkill, CString &szDesc);
VOID GetPositionInBox(WORD wIndex, WORD &wPage, WORD &wRow, WORD &wColumn);

/////////////////////////////////////////////////
// CPokemonRomViewerApp:
class CPokemonRomViewerApp : public CWinApp
{
public:
	CPokemonRomViewerApp();
	~CPokemonRomViewerApp();

protected:
	BOOL ReadStringLists();
	VOID DestroyStringLists();
	VOID ReadConfig();
	VOID WriteConfig();
	BOOL CreateDialogs();
	BOOL ApplyConfig(BOOL bRomChanged, BOOL bUILangChanged);

public:
	BOOL			m_bFirstInstance;
	HANDLE			m_hMutex;

	CString			m_szInitialPath;

	CBreedDlg		m_dlgBreed;
	CEvoDlg			m_dlgEvo;
	CItemDlg		m_dlgItem;
	CBerryDlg		m_dlgBerry;
	CSkillDlg		m_dlgSkill;
	CEncDlg			m_dlgEnc;
	CBaseDialog *	m_rgpDlgs[APP_DLG_COUNT];

	CWaitDlg		m_dlgWait;

public:
	virtual BOOL InitInstance();

	afx_msg void OnAppAbout();
	afx_msg void OnConfig();
	afx_msg void OnShowBreed();
	afx_msg void OnUpdateShowBreed(CCmdUI *pCmdUI);
	afx_msg void OnShowEvo();
	afx_msg void OnUpdateShowEvo(CCmdUI *pCmdUI);
	afx_msg void OnShowItem();
	afx_msg void OnUpdateShowItem(CCmdUI *pCmdUI);
	afx_msg void OnShowSkill();
	afx_msg void OnUpdateShowSkill(CCmdUI *pCmdUI);
	afx_msg void OnShowEnc();
	afx_msg void OnUpdateShowEnc(CCmdUI *pCmdUI);
	afx_msg void OnShowBerry();
	afx_msg void OnUpdateShowBerry(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
};

extern CPokemonRomViewerApp theApp;
extern ConfigStruct	cfg;
extern CPokemonRom	rom;
