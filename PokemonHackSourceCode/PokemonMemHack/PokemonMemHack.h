// PokemonMemHack.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

/////////////////////////////////////////////////
//#define BREED_COUNT	(0x19C)	// (412)
//#define SKILL_COUNT	(0x163)	// (355)
#define AREA_COUNT	(0x100)	// (256)
#define ITEM_COUNT	(0x179)	// (375)
#define PS_COUNT	(25)
//#define SPEC_COUNT	(0x4E)	// (78)
#define SPEC_COUNT	(0x8F)	// (143)

/////////////////////////////////////////////////
#include "resource.h"		// 主符号
#include "StringReader.h"
#include "SkillListBox.h"
#include "SkillComboBox.h"
#include "PokemonMemHackCore.h"
#include "PokemonMemHackDlg.h"

#define GUI_FONT_NAME	"MS Gothic"
#define GUI_FONT_HEIGHT	12

enum { ui_count_none=0, ui_count_dec=1, ui_count_hex=2 };

struct ConfigStruct
{
	DWORD			dwLang;
	DWORD			dwCount;
	StringEntry *	pBreedNameList;
	StringEntry *	pSkillNameList;
	StringEntry *	pSkillDescList;
	StringEntry *	pAreaNameList;
	StringEntry *	pItemNameList;
	StringEntry *	pItemDescList;
	StringEntry *	pPsNameList;		// persionality list
	StringEntry *	pSpecNameList;		// specialty
	StringEntry *	pSpecDescList;
};

// CPokemonMemHackApp:
// 有关此类的实现，请参阅 PokemonMemHack.cpp
//

class CPokemonMemHackApp : public CWinApp
{
public:
	CPokemonMemHackApp();
	~CPokemonMemHackApp();

// 重写
public:
	virtual BOOL InitInstance();

	BOOL ReadStringLists();
	VOID DestroyStringLists();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CPokemonMemHackApp theApp;
extern ConfigStruct	cfg;
extern CPokemonMemHackCore g_MemHack;
extern CPokemonRom g_MemRom;

extern COLORREF	g_rgBackClrTable[];
extern COLORREF	g_rgForeClrTable[];
extern DWORD	g_dwBackClrCount;
extern DWORD	g_dwForeClrCount;
