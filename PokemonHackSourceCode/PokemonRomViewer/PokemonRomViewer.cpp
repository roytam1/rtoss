// PokemonRomViewer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "MainFrm.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

StringEntry * GetEncNameEntry(WORD wAreaId)
{
	static DWORD	dwPreIndex = 0;
	DWORD			dwIndex;
	StringEntry *	pEntry = NULL;

	if(dwPreIndex + 1 < ENC_COUNT &&
		cfg.pEncNameList[dwPreIndex + 1].dwId == wAreaId)
	{
		++dwPreIndex;
		pEntry = &(cfg.pEncNameList[dwPreIndex]);
	}
	else if(dwPreIndex > 0 &&
		cfg.pEncNameList[dwPreIndex - 1].dwId == wAreaId)
	{
		--dwPreIndex;
		pEntry = &(cfg.pEncNameList[dwPreIndex]);
	}
	else if(cfg.pEncNameList[dwPreIndex].dwId == wAreaId)
	{
		pEntry = &(cfg.pEncNameList[dwPreIndex]);
	}
	else
	{
		for(dwIndex = 0; dwIndex < ENC_COUNT; ++dwIndex)
		{
			if(wAreaId == cfg.pEncNameList[dwIndex].dwId)
			{
				dwPreIndex = dwIndex;
				pEntry = &(cfg.pEncNameList[dwPreIndex]);
				break;
			}
		}
	}

	return pEntry;
}
/*
VOID SetBit(LPVOID pFlags, DWORD_PTR dwBitIndex)
{
	LPBYTE	pb = &((LPBYTE)(pFlags))[dwBitIndex >> 0x3];
	*pb |= ((BYTE)(1) << (BYTE)(dwBitIndex & 0x7));
}

VOID ClrBit(LPVOID pFlags, DWORD_PTR dwBitIndex)
{
	LPBYTE	pb = &((LPBYTE)(pFlags))[dwBitIndex >> 0x3];
	*pb &= ~((BYTE)(1) << (BYTE)(dwBitIndex & 0x7));
}

BYTE GetBit(LPCVOID pFlags, DWORD_PTR dwBitIndex)
{
	return ((((LPBYTE)(pFlags))[dwBitIndex >> 0x3] >> (BYTE)(dwBitIndex & 0x7)) & 0x1);
}
*/
WORD ConvertBreed(WORD wBreed, BOOL b387To412)
{
	WORD	wCvt = wBreed;

	if(b387To412)	// from PokemonStruct space to Rom space
	{
		if(wCvt > 386)
			wCvt = 0;
		else if(wCvt >= 252)
			wCvt += 25;
	}
	else		// from Rom space to PokemonStruct space
	{
		if(wCvt > 411)
			wCvt = 0;
		else if(wCvt >= 277)
			wCvt -= 25;
		else if(wCvt >= 252)
			wCvt = 0;
	}

	return wCvt;
}

WORD TextToBreed(LPCTSTR szText)
{
	BOOL	bFound = FALSE;
	DWORD	dwLang, dwLength;
	WORD	wBreed;
	LPCTSTR	szBreed = NULL;

	wBreed = (WORD)(_tcstoul(szText, 0, 0));
	if(wBreed >= BREED_COUNT)
	{
		wBreed = 0;
	}
	else if(wBreed == 0)
	{
		dwLength = lstrlen(szText);
		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			for(dwLang = 0; dwLang < 3; ++dwLang)
			{
				if(_tcsnicmp(szText, cfg.pBreedNameList[wBreed].rgszText[dwLang], dwLength) == 0)
				{
					bFound = TRUE;
					break;
				}
			}
			if(bFound)
				break;
		}
		if(wBreed >= BREED_COUNT)
			wBreed = 0;
	}

	return wBreed;
}

VOID FormatSkillDesc(WORD wSkill, CString &szDesc)
{
	SkillListEntry *pSkill = NULL;
	DWORD	dwTargetTypeIndex;
	CString	szTemp;

	szDesc = _T("");
	pSkill = rom.GetSkillListEntry(wSkill);
	if(pSkill)
	{
		// type
		if(pSkill->bType < dwTypesCount)
			szTemp.Format(_T("属性：%s"), szTypes[pSkill->bType]);
		else
			szTemp.Format(_T("属性：？？？(0x%02x)"), pSkill->bType);
		szDesc = szDesc + szTemp;

		// target
		dwTargetTypeIndex = GetTargetTypeIndex(pSkill->bTargetType);
		if(dwTargetTypeIndex < dwSkillTargetTypesCount)
			szTemp.Format(_T("对象：%s"), rgSkillTargetTypes[dwTargetTypeIndex].szName);
		else
			szTemp.Format(_T("对象：？？？(0x%02x)"), pSkill->bTargetType);
		szDesc = szDesc + _T("\r\n") + szTemp;

		// power
		if(pSkill->bPower > 1)
			szTemp.Format(_T("威力：%lu"), pSkill->bPower);
		else
			szTemp.Format(_T("威力："));
		szDesc = szDesc + _T("\r\n") + szTemp;

		// acc
		if(pSkill->bAccuracy > 0)
			szTemp.Format(_T("命中：%lu%%"), pSkill->bAccuracy);
		else
			szTemp.Format(_T("命中：必中"));
		szDesc = szDesc + _T("\r\n") + szTemp;

		// pp
		szTemp.Format(_T("ＰＰ：%lu"), pSkill->bPP);
		szDesc = szDesc + _T("\r\n") + szTemp;

		// priority
		szTemp.Format(_T("优先：%li"), (LONG)(pSkill->nPriority));
		szDesc = szDesc + _T("\r\n") + szTemp;

		// effect ratio
		if(pSkill->bPower > 1)
			szTemp.Format(_T("效果几率：%lu%%"), pSkill->bEffectRatio);
		else
			szTemp.Format(_T("效果几率：必然发动"));
		szDesc = szDesc + _T("\r\n") + szTemp;

		// effect
		szTemp.Format(_T("效果：%s"), cfg.pSkillEffList[pSkill->bEffect].rgszText[cfg.dwLang]);
		szDesc = szDesc + _T("\r\n") + szTemp;

		// special
		szTemp.Format(_T("\n特殊：%s"), cfg.pSkillSpecList[wSkill].rgszText[cfg.dwLang]);
		szDesc = szDesc + _T("\r\n") + szTemp;
	}
}

VOID GetPositionInBox(WORD wIndex, WORD &wPage, WORD &wRow, WORD &wColumn)
{
	if(wIndex > 0 && wIndex < 387)
	{
		wPage = wIndex / 30 + 1;
		wRow = (wIndex % 30) / 6 + 1;
		wColumn = wIndex % 6 + 1;
	}
	else
	{
		wPage = 0;
		wRow = 0;
		wColumn = 0;
	}
}

// CPokemonRomViewerApp

BEGIN_MESSAGE_MAP(CPokemonRomViewerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CONFIG, OnConfig)
	ON_COMMAND(ID_SHOW_BREED, OnShowBreed)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BREED, OnUpdateShowBreed)
	ON_COMMAND(ID_SHOW_EVO, OnShowEvo)
	ON_UPDATE_COMMAND_UI(ID_SHOW_EVO, OnUpdateShowEvo)
	ON_COMMAND(ID_SHOW_ITEM, OnShowItem)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ITEM, OnUpdateShowItem)
	ON_COMMAND(ID_SHOW_SKILL, OnShowSkill)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SKILL, OnUpdateShowSkill)
	ON_COMMAND(ID_SHOW_ENC, OnShowEnc)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ENC, OnUpdateShowEnc)
	ON_COMMAND(ID_SHOW_BERRY, OnShowBerry)
	ON_UPDATE_COMMAND_UI(ID_SHOW_BERRY, OnUpdateShowBerry)
END_MESSAGE_MAP()


// CPokemonRomViewerApp 构造

CPokemonRomViewerApp::CPokemonRomViewerApp()
{
	m_bFirstInstance = TRUE;
	m_hMutex = NULL;

	cfg.dwLang = ui_lang_cn;
	cfg.dwCount = ui_count_none;
	cfg.pBreedNameList = NULL;
	cfg.pSkillNameList = NULL;
	cfg.pSkillEffList = NULL;
	cfg.pSkillSpecList = NULL;
	cfg.pItemNameList = NULL;
	cfg.pItemDescList = NULL;
	cfg.pSpecNameList = NULL;
	cfg.pSpecDescList = NULL;
	cfg.pEncNameList = NULL;

	m_rgpDlgs[0] = &m_dlgSkill;
	m_rgpDlgs[1] = &m_dlgEnc;
	m_rgpDlgs[2] = &m_dlgBreed;
	m_rgpDlgs[3] = &m_dlgEvo;
	m_rgpDlgs[4] = &m_dlgItem;
	m_rgpDlgs[5] = &m_dlgBerry;
}

CPokemonRomViewerApp::~CPokemonRomViewerApp()
{
	DestroyStringLists();
	if(m_hMutex)
		CloseHandle(m_hMutex);
}

// 唯一的一个 CPokemonRomViewerApp 对象
CPokemonRomViewerApp theApp;
ConfigStruct	cfg;
CPokemonRom		rom;

#define REG_KEY_NAME	_T("PokemonRomViewer")
#define SECTION_NAME	_T("Configurations")
#define VAL_ROM_PATH	_T("Rom Path")
#define VAL_UI_LANG		_T("UI Language")
#define VAL_UI_COUNT	_T("UI Count")

// CPokemonRomViewerApp 初始化
BOOL CPokemonRomViewerApp::InitInstance()
{
	BOOL	bResult = TRUE;
	INITCOMMONCONTROLSEX	icc;
	DWORD	cchPathLength;
	LPTSTR	szPath;

	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_BAR_CLASSES |
				ICC_LISTVIEW_CLASSES |
				ICC_PROGRESS_CLASS |
				ICC_STANDARD_CLASSES |
				ICC_TAB_CLASSES;
	InitCommonControlsEx(&icc);

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	m_hMutex = CreateMutex(NULL, FALSE, _T("PokemonRomViewer"));
	if(m_hMutex)
	{
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			m_bFirstInstance = FALSE;
		}
	}
	else
	{
		AfxMessageBox(IDS_ERR_INIT);
		return FALSE;
	}

	szPath = m_szInitialPath.GetBufferSetLength(MAX_PATH);
	if(szPath)
	{
		cchPathLength = GetCurrentDirectory(MAX_PATH, szPath);
		m_szInitialPath.ReleaseBuffer(cchPathLength);
	}

	// read config
	SetRegistryKey(REG_KEY_NAME);
	ReadConfig();
	WriteConfig();

	// create frame
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// initialize
	if(bResult)
	{
#if 0
OutputDebugString(_T("1\n"));
#endif
		bResult = CreateDialogs();
	}

	#ifndef _DEBUG
	if(bResult)
	{
		bResult = ReadStringLists();
	}
	#endif

	if(bResult)
	{
#if 0
OutputDebugString(_T("2\n"));
#endif
		pFrame->ShowWindow(SW_SHOW);
		pFrame->UpdateWindow();
	}

	#ifdef _DEBUG
	if(bResult)
	{
#if 0
OutputDebugString(_T("3\n"));
#endif
		bResult = ReadStringLists();
	}
	#endif

	if(bResult)
	{
#if 0
OutputDebugString(_T("4\n"));
#endif
		ApplyConfig(TRUE, TRUE);
	}

	if(!bResult)
	{
		AfxMessageBox(IDS_ERR_INIT);
	}

	return bResult;
}


// CPokemonRomViewerApp 消息处理程序



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CPokemonRomViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CPokemonRomViewerApp 消息处理程序
BOOL CPokemonRomViewerApp::ReadStringLists()
{
	BOOL			bResult;
	CStringReader	sr;

	try
	{
		#ifdef _DEBUG
		m_dlgWait.BeginWait(TEXT_FILE_COUNT);
		#endif

		/////////////////////////////////////////
		cfg.pBreedNameList = new StringEntry[BREED_COUNT];
		if(!cfg.pBreedNameList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("BreedNameList.txt"), cfg.pBreedNameList, BREED_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pSkillNameList = new StringEntry[SKILL_COUNT];
		if(!cfg.pSkillNameList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("SkillNameList.txt"), cfg.pSkillNameList, SKILL_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pSkillEffList = new StringEntry[SKILL_EFFECT_COUNT];
		if(!cfg.pSkillEffList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("SkillEffList.txt"), cfg.pSkillEffList, SKILL_EFFECT_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pSkillSpecList = new StringEntry[SKILL_COUNT];
		if(!cfg.pSkillSpecList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("SkillSpecList.txt"), cfg.pSkillSpecList, SKILL_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pItemNameList = new StringEntry[ITEM_COUNT];
		if(!cfg.pItemNameList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("ItemNameList.txt"), cfg.pItemNameList, ITEM_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pItemDescList = new StringEntry[ITEM_COUNT];
		if(!cfg.pItemDescList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("ItemDescList.txt"), cfg.pItemDescList, ITEM_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pSpecNameList = new StringEntry[SPEC_COUNT];
		if(!cfg.pSpecNameList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("SpecNameList.txt"), cfg.pSpecNameList, SPEC_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pSpecDescList = new StringEntry[SPEC_COUNT];
		if(!cfg.pSpecDescList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("SpecDescList.txt"), cfg.pSpecDescList, SPEC_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif

		/////////////////////////////////////////
		cfg.pEncNameList = new StringEntry[ENC_COUNT];
		if(!cfg.pEncNameList)
		{
			throw 0;
		}

		bResult = sr.BatchReadString(_T("EncNameList.txt"), cfg.pEncNameList, ENC_COUNT);
		if(!bResult)
		{
			throw 0;
		}

		#ifdef _DEBUG
		m_dlgWait.IncStep();
		#endif
	}
	catch(int)
	{
		bResult = FALSE;
	}

	if(!bResult)
	{
		#ifdef _DEBUG
		m_dlgWait.ShowWindow(SW_HIDE);
		#endif

		DestroyStringLists();
	}

	return bResult;
}

VOID CPokemonRomViewerApp::DestroyStringLists()
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
	if(cfg.pSkillEffList)
	{
		delete [](cfg.pSkillEffList);
		cfg.pSkillEffList = NULL;
	}
	if(cfg.pSkillSpecList)
	{
		delete [](cfg.pSkillSpecList);
		cfg.pSkillSpecList = NULL;
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
	if(cfg.pEncNameList)
	{
		delete [](cfg.pEncNameList);
		cfg.pEncNameList = NULL;
	}
}


VOID CPokemonRomViewerApp::ReadConfig()
{
	cfg.szRomPath = GetProfileString(SECTION_NAME, VAL_ROM_PATH, NULL);
	cfg.dwCount = GetProfileInt(SECTION_NAME, VAL_UI_COUNT, ui_count_none);
	cfg.dwLang = GetProfileInt(SECTION_NAME, VAL_UI_LANG, ui_lang_jp);
}

VOID CPokemonRomViewerApp::WriteConfig()
{
	if(m_bFirstInstance)
	{
		WriteProfileString(SECTION_NAME, VAL_ROM_PATH, cfg.szRomPath);
		WriteProfileInt(SECTION_NAME, VAL_UI_COUNT, cfg.dwCount);
		WriteProfileInt(SECTION_NAME, VAL_UI_LANG, cfg.dwLang);
	}
}

BOOL CPokemonRomViewerApp::CreateDialogs()
{
	BOOL	bResult;
	DWORD	dwIndex;

	for(dwIndex = 0; dwIndex < APP_DLG_COUNT; ++dwIndex)
	{
		bResult = m_rgpDlgs[dwIndex]->Create(m_rgpDlgs[dwIndex]->m_uTemplateId, m_pMainWnd);
		if(!bResult)
			break;

		//m_rgpDlgs[dwIndex]->ShowWindow(SW_HIDE);
	}

	if(bResult)
	{
		bResult = m_dlgWait.Create(CWaitDlg::IDD, m_pMainWnd);
		//if(bResult)
		//	m_dlgWait.ShowWindow(SW_HIDE);
	}

	return bResult;
}

BOOL CPokemonRomViewerApp::ApplyConfig(BOOL bRomChanged, BOOL bUILangChanged)
{
	BOOL	bResult = TRUE;
	DWORD	dwIndex;
	CString	szCaption;
	CString	szRomName;

	if(bRomChanged)
	{
		// open rom
		bResult = rom.OpenRom(cfg.szRomPath);
		if(bResult)
		{
			if(rom.m_bIsFileReadOnly)
			{
				AfxMessageBox(IDS_WRN_ROM_READONLY);
			}
		}

		// refresh the caption of the main window
		szCaption = AfxGetAppName();
		szRomName = rom.GetRomName();
		if(szRomName.GetLength() > 0)
			szCaption = szCaption + _T(" - ") + rom.GetRomName();

		m_pMainWnd->SetWindowText(szCaption);
	}

	if(bResult)
	{
		// update each dialog
		m_dlgWait.BeginWait(APP_DLG_COUNT);
		for(dwIndex = 0; dwIndex < APP_DLG_COUNT; ++dwIndex)
		{
			m_rgpDlgs[dwIndex]->OnConfigChanged(bRomChanged, bUILangChanged);
			m_dlgWait.IncStep();
		}
	}
	else
	{
		// hide each dialog, and report error
		for(dwIndex = 0; dwIndex < APP_DLG_COUNT; ++dwIndex)
		{
			m_rgpDlgs[dwIndex]->ShowWindow(SW_HIDE);
		}

		AfxMessageBox(IDS_ERR_ROM_READ);
	}

	return bResult;
}

void CPokemonRomViewerApp::OnConfig()
{
	// TODO: 在此添加命令处理程序代码
	CConfigDlg	cd;
	BOOL	bRomChanged = FALSE;
	BOOL	bUILangChanged = FALSE;

	if(cd.DoModal() == IDOK)
	{
		if(cfg.szRomPath.CompareNoCase(cd.m_szRomPath) != 0)
		{
			cfg.szRomPath = cd.m_szRomPath;
			bRomChanged = TRUE;
		}

		if(cfg.dwLang != cd.m_dwLang ||
			cfg.dwCount != cd.m_dwCount)
		{
			bUILangChanged = TRUE;
			cfg.dwLang = cd.m_dwLang;
			cfg.dwCount = cd.m_dwCount;
		}

		ApplyConfig(bRomChanged, bUILangChanged);
	}

	if(bRomChanged || bUILangChanged)
		WriteConfig();
}

void CPokemonRomViewerApp::OnShowBreed()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgBreed.ShowWindow(SW_SHOWDEFAULT);
	m_dlgBreed.SetForegroundWindow();
}

void CPokemonRomViewerApp::OnUpdateShowBreed(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(rom.m_bOpened);
}

void CPokemonRomViewerApp::OnShowEvo()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgEvo.ShowWindow(SW_SHOWDEFAULT);
	m_dlgEvo.SetForegroundWindow();
}

void CPokemonRomViewerApp::OnUpdateShowEvo(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(rom.m_bOpened);
}

void CPokemonRomViewerApp::OnShowItem()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgItem.ShowWindow(SW_SHOWDEFAULT);
	m_dlgItem.SetForegroundWindow();
}

void CPokemonRomViewerApp::OnUpdateShowItem(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(rom.m_bOpened);
}

void CPokemonRomViewerApp::OnShowSkill()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgSkill.ShowWindow(SW_SHOWDEFAULT);
	m_dlgSkill.SetForegroundWindow();
}

void CPokemonRomViewerApp::OnUpdateShowSkill(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(rom.m_bOpened);
}

void CPokemonRomViewerApp::OnShowEnc()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgEnc.ShowWindow(SW_SHOWDEFAULT);
	m_dlgEnc.SetForegroundWindow();
}

void CPokemonRomViewerApp::OnUpdateShowEnc(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(rom.m_bOpened);
}

void CPokemonRomViewerApp::OnShowBerry()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgBerry.ShowWindow(SW_SHOWDEFAULT);
	m_dlgBerry.SetForegroundWindow();
}

void CPokemonRomViewerApp::OnUpdateShowBerry(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(rom.m_bOpened);
}
