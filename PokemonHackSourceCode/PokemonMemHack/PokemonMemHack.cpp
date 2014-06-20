// PokemonMemHack.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "PokemonMemHackDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的一个 CPokemonMemHackApp 对象

CPokemonMemHackApp	theApp;
ConfigStruct		cfg;
CPokemonMemHackCore	g_MemHack;
CPokemonRom			g_MemRom;

COLORREF	g_rgBackClrTable[] =
{
	RGB(168,168,120),	// 00、ふつう
	RGB(192,48,40),		// 01、かくとう
	RGB(168,144,240),	// 02、ひこう
	RGB(160,64,160),	// 03、どく
	RGB(224,192,104),	// 04、じめん
	RGB(184,160,56),	// 05、いわ
	RGB(168,184,32),	// 06、むし
	RGB(112,88,152),	// 07、ゴースト
	RGB(184,184,208),	// 08、はがね
	RGB(104,160,144),	// 09、？？？
	RGB(240,128,48),	// 0A、ほのお
	RGB(104,144,240),	// 0B、みず
	RGB(120,200,80),	// 0C、くさ
	RGB(248,208,48),	// 0D、でんき
	RGB(248,88,136),	// 0E、エスパー
	RGB(152,216,216),	// 0F、こおり
	RGB(112,56,248),	// 10、ドラゴン
	RGB(112,88,112),	// 11、あく
};
DWORD	g_dwBackClrCount = sizeof(g_rgBackClrTable) / sizeof(g_rgBackClrTable[0]);

COLORREF	g_rgForeClrTable[] =
{
	RGB(0,0,0),			// 00、ふつう
	RGB(255,255,255),	// 01、かくとう
	RGB(0,0,0),			// 02、ひこう
	RGB(255,255,255),	// 03、どく
	RGB(0,0,0),			// 04、じめん
	RGB(0,0,0),			// 05、いわ
	RGB(0,0,0),			// 06、むし
	RGB(255,255,255),	// 07、ゴースト
	RGB(0,0,0),			// 08、はがね
	RGB(0,0,0),			// 09、？？？
	RGB(255,255,255),	// 0A、ほのお
	RGB(0,0,0),			// 0B、みず
	RGB(0,0,0),			// 0C、くさ
	RGB(0,0,0),			// 0D、でんき
	RGB(255,255,255),	// 0E、エスパー
	RGB(0,0,0),			// 0F、こおり
	RGB(255,255,255),	// 10、ドラゴン
	RGB(255,255,255),	// 11、あく
};
DWORD	g_dwForeClrCount = sizeof(g_rgForeClrTable) / sizeof(g_rgForeClrTable[0]);

// CPokemonMemHackApp

BEGIN_MESSAGE_MAP(CPokemonMemHackApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPokemonMemHackApp 构造

CPokemonMemHackApp::CPokemonMemHackApp()
{
	// 将所有重要的初始化放置在 InitInstance 中
#ifdef _DEBUG
	cfg.dwLang = ui_lang_jp;
#else
	cfg.dwLang = ui_lang_jp;
#endif
	cfg.dwCount = ui_count_none;
	cfg.pBreedNameList = NULL;
	cfg.pSkillNameList = NULL;
	cfg.pSkillDescList = NULL;
	cfg.pItemNameList = NULL;
	cfg.pItemDescList = NULL;
	cfg.pAreaNameList = NULL;
	cfg.pPsNameList = NULL;
	cfg.pSpecNameList = NULL;
	cfg.pSpecDescList = NULL;
}

CPokemonMemHackApp::~CPokemonMemHackApp()
{
	DestroyStringLists();
}

// CPokemonMemHackApp 初始化

BOOL CPokemonMemHackApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	if(!ReadStringLists())
	{
		AfxMessageBox(IDS_ERR_STRING_READ, MB_OK | MB_ICONERROR);
	}
	else if(!g_MemHack.Initialize())
	{
		AfxMessageBox(IDS_ERR_INIT, MB_OK | MB_ICONERROR);
	}
	else
	{
		CPokemonMemHackDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			//对话框的代码
		}
		else if (nResponse == IDCANCEL)
		{
			//对话框的代码
		}
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CPokemonMemHackApp::ReadStringLists()
{
	BOOL			bResult;
	CStringReader	sr;

	try
	{
		/////////////////////////////////////////
		cfg.pBreedNameList = new StringEntry[BREED_COUNT];
		if(!cfg.pBreedNameList)
			throw 0;

		bResult = sr.BatchReadString(_T("BreedNameList.txt"), cfg.pBreedNameList, BREED_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pSkillNameList = new StringEntry[SKILL_COUNT];
		if(!cfg.pSkillNameList)
			throw 0;

		bResult = sr.BatchReadString(_T("SkillNameList.txt"), cfg.pSkillNameList, SKILL_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pSkillDescList = new StringEntry[SKILL_COUNT];
		if(!cfg.pSkillDescList)
			throw 0;

		bResult = sr.BatchReadString(_T("SkillDescList.txt"), cfg.pSkillDescList, SKILL_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pItemNameList = new StringEntry[ITEM_COUNT];
		if(!cfg.pItemNameList)
			throw 0;

		bResult = sr.BatchReadString(_T("ItemNameList.txt"), cfg.pItemNameList, ITEM_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pItemDescList = new StringEntry[ITEM_COUNT];
		if(!cfg.pItemDescList)
			throw 0;

		bResult = sr.BatchReadString(_T("ItemDescList.txt"), cfg.pItemDescList, ITEM_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pAreaNameList = new StringEntry[AREA_COUNT];
		if(!cfg.pAreaNameList)
			throw 0;

		bResult = sr.BatchReadString(_T("AreaNameList.txt"), cfg.pAreaNameList, AREA_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pPsNameList = new StringEntry[PS_COUNT];
		if(!cfg.pPsNameList)
			throw 0;

		bResult = sr.BatchReadString(_T("PersonalityList.txt"), cfg.pPsNameList, PS_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pSpecNameList = new StringEntry[SPEC_COUNT];
		if(!cfg.pSpecNameList)
			throw 0;

		bResult = sr.BatchReadString(_T("SpecNameList.txt"), cfg.pSpecNameList, SPEC_COUNT);
		if(!bResult)
			throw 0;

		/////////////////////////////////////////
		cfg.pSpecDescList = new StringEntry[SPEC_COUNT];
		if(!cfg.pSpecDescList)
			throw 0;

		bResult = sr.BatchReadString(_T("SpecDescList.txt"), cfg.pSpecDescList, SPEC_COUNT);
		if(!bResult)
			throw 0;
	}
	catch(int)
	{
		bResult = FALSE;
	}

	if(!bResult)
		DestroyStringLists();

	return bResult;
}

VOID CPokemonMemHackApp::DestroyStringLists()
{
	if(cfg.pBreedNameList)
	{
		delete [](cfg.pBreedNameList);
		cfg.pBreedNameList = NULL;
	}
	if(cfg.pSkillNameList)
	{
		delete [](cfg.pSkillNameList);
		cfg.pSkillNameList = NULL;
	}
	if(cfg.pSkillDescList)
	{
		delete [](cfg.pSkillDescList);
		cfg.pSkillDescList = NULL;
	}
	if(cfg.pItemNameList)
	{
		delete [](cfg.pItemNameList);
		cfg.pItemNameList = NULL;
	}
	if(cfg.pItemDescList)
	{
		delete [](cfg.pItemDescList);
		cfg.pItemDescList = NULL;
	}
	if(cfg.pAreaNameList)
	{
		delete [](cfg.pAreaNameList);
		cfg.pAreaNameList = NULL;
	}
	if(cfg.pPsNameList)
	{
		delete [](cfg.pPsNameList);
		cfg.pPsNameList = NULL;
	}
	if(cfg.pSpecNameList)
	{
		delete [](cfg.pSpecNameList);
		cfg.pSpecNameList = NULL;
	}
	if(cfg.pSpecDescList)
	{
		delete [](cfg.pSpecDescList);
		cfg.pSpecDescList = NULL;
	}
}

