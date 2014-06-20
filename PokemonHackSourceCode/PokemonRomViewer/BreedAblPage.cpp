// BreedAblPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedAblPage.h"

// CBreedAblPage 对话框

IMPLEMENT_DYNAMIC(CBreedAblPage, CBreedTabPage)
CBreedAblPage::CBreedAblPage()
{
	m_uTemplateId = CBreedAblPage::IDD;
}

CBreedAblPage::~CBreedAblPage()
{
}

void CBreedAblPage::DoDataExchange(CDataExchange* pDX)
{
	DWORD	dwIndex;

	CBreedTabPage::DoDataExchange(pDX);

	for(dwIndex = 0; dwIndex < 6; ++dwIndex)
	{
		DDX_Control(pDX, IDC_EXPORT_BRD + dwIndex, m_ctrlExportBtn[dwIndex]);
	}
}


BEGIN_MESSAGE_MAP(CBreedAblPage, CBreedTabPage)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_BRD_HP, IDC_BRD_SDEF, OnEnChangeBrdAbl)
	ON_BN_CLICKED(IDC_EXPORT_BRD, OnBnClickedExportBrd)
	ON_BN_CLICKED(IDC_EXPORT_EVO, OnBnClickedExportEvo)
	ON_BN_CLICKED(IDC_EXPORT_LEARN, OnBnClickedExportLearn)
	ON_BN_CLICKED(IDC_EXPORT_MACHINE, OnBnClickedExportMachine)
	ON_BN_CLICKED(IDC_EXPORT_SKILL, OnBnClickedExportSkill)
	ON_BN_CLICKED(IDC_EXPORT_INDEX, OnBnClickedExportIndex)
	ON_STN_DBLCLK(IDC_SHOW_EXPORT_BUTTONS, OnStnDblclickShowExportButtons)
END_MESSAGE_MAP()


// CBreedAblPage 消息处理程序
void CBreedAblPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	if(bRomChanged)
		TransData(TRUE);
}

void CBreedAblPage::TransData(BOOL bToControls)
{
	GetCurrentBreed();
	if(m_pBreed == NULL)
		return;

	DWORD	dwIndex;
	CString	szText;
	LPBYTE	pbAbl = &(m_pBreed->bHP);

	if(bToControls)
	{
		for(dwIndex = 0; dwIndex < 6; ++dwIndex)
		{
			if(m_pBreedAbilities)
				szText.Format(_T("%lu"), (DWORD)(m_pBreedAbilities[dwIndex]));
			else
				szText.Format(_T("%lu"), (DWORD)(pbAbl[dwIndex]));
			SetDlgItemText(IDC_BRD_HP + dwIndex, szText);

			szText.Format(_T("%lu"), (DWORD)((m_pBreed->wBattleBonuses >> (2 * dwIndex)) & 0x3));
			SetDlgItemText(IDC_BB_HP + dwIndex, szText);
		}

		CalcIndvSum();
	}
	else
	{
		m_pBreed->wBattleBonuses &= 0xF000;
		for(dwIndex = 0; dwIndex < 6; ++dwIndex)
		{
			GetDlgItemText(IDC_BB_HP + dwIndex, szText);
			m_pBreed->wBattleBonuses |= (min(_tcstoul(szText, 0, 0), 0x3) << (2 * dwIndex));
		}
	}
}

void CBreedAblPage::CalcIndvSum()
{
	DWORD	dwIndex;
	DWORD	dwSum = 0;

	if(m_pBreed == NULL)
		return;

	for(dwIndex = 0; dwIndex < 6; ++dwIndex)
	{
		if(m_pBreedAbilities)
			dwSum += m_pBreedAbilities[dwIndex];
		else
			dwSum += (&(m_pBreed->bHP))[dwIndex];
	}

	SetDlgItemInt(IDC_BRD_SUM, dwSum, FALSE);
}

void CBreedAblPage::GetIndvAbl(DWORD dwIndex)
{
	if(m_pBreed == NULL || dwIndex >= 6)
		return;

	CString	szText;
	GetDlgItemText(IDC_BRD_HP + dwIndex, szText);
	if(m_pBreedAbilities)
		m_pBreedAbilities[dwIndex] = (WORD)(min(_tcstoul(szText, 0, 0), 0xFF));
	else
		(&(m_pBreed->bHP))[dwIndex] = (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));
}

void CBreedAblPage::OnEnChangeBrdAbl(UINT uID)
{
	// TODO:  在此添加控件通知处理程序代码
	GetIndvAbl(uID - IDC_BRD_HP);
	CalcIndvSum();
}

void CBreedAblPage::EnableExportButtons(BOOL bEnable)
{
	DWORD	dwIndex;
	INT	nShowCmd = bEnable ? SW_SHOW : SW_HIDE;

	for(dwIndex = 0; dwIndex < 6; ++dwIndex)
	{
		m_ctrlExportBtn[dwIndex].EnableWindow(bEnable);
		m_ctrlExportBtn[dwIndex].ShowWindow(nShowCmd);
	}
}

void CBreedAblPage::OnStnDblclickShowExportButtons()
{
	// TODO: 在此添加控件通知处理程序代码
	EnableExportButtons(TRUE);
}

void CBreedAblPage::OnBnClickedExportBrd()
{
	// TODO: 在此添加控件通知处理程序代码
	#ifndef UNICODE
	#error UNICODE Version ONLY!
	#endif

	static LPCTSTR szTypes[] =
	{
		L"ノーマル",	// 00
		L"かくとう",	// 01
		L"ひこう",		// 02
		L"どく",		// 03
		L"じめん",		// 04
		L"いわ",		// 05
		L"むし",		// 06
		L"ゴースト",	// 07
		L"はがね",		// 08
		L"？？？",		// 09
		L"ほのお",		// 0A
		L"みず",		// 0B
		L"くさ",		// 0C
		L"でんき",		// 0D
		L"エスパー",	// 0E
		L"こおり",		// 0F
		L"ドラゴン",	// 10
		L"あく",		// 11
		L"0x12",		// 12
		L"0x13",		// 13
		L"0x14",		// 14
		L"0x15",		// 15
		L"0x16",		// 16
		L"0x17",		// 17
		L"0x18"			// 18
	};

	static LPCTSTR szEggGroups[] =
	{
		L"？？？",		// 00
		L"怪獣",		// 01
		L"水中１",		// 02
		L"虫",			// 03
		L"飛行",		// 04
		L"陸上",		// 05
		L"妖精",		// 06
		L"植物",		// 07
		L"人形",		// 08
		L"水中３",		// 09
		L"鉱物",		// 0A
		L"不定形",		// 0B
		L"水中２",		// 0C
		L"メタモン",	// 0D
		L"竜",			// 0E
		L"未発見"		// 0F
	};

	BOOL	bResult;
	WCHAR	wUnicodePrefix = 0xFEFF;

	CFile	expFile;
	WORD	wBreed;
	CString	szText, szTemp;

	BreedListEntry *pBreed = NULL;

	try
	{
		if(!rom.m_bOpened)
			throw 0;

		// create export file
		SetCurrentDirectory(theApp.m_szInitialPath);
		szText = rom.GetRomName();
		szText += _T(" Breed.txt");
		bResult = expFile.Open(szText, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
		if(!bResult)
			throw 0;

		// write prefix of unicode text file
		expFile.Write(&wUnicodePrefix, sizeof(WCHAR));

		// write column headers
		szText = _T("ID\t");				// 1
		szText += _T("Kanto\t");			// 2
		szText += _T("Houen\t");			// 3
		szText += _T("Name\t");				// 4
		szText += _T("HP\t");				// 5
		szText += _T("Atk\t");				// 6
		szText += _T("Def\t");				// 7
		szText += _T("Dex\t");				// 8
		szText += _T("SAtk\t");				// 9
		szText += _T("SDef\t");				// 10
		szText += _T("Type1\t");			// 11
		szText += _T("Type2\t");			// 12
		szText += _T("Spec1\t");			// 13
		szText += _T("Spec2\t");			// 14
		szText += _T("Egg Group1\t");		// 15
		szText += _T("Egg Group2\t");		// 16
		szText += _T("Catch Ratio%\t");		// 17
		szText += _T("Escape Ratio%\t");	// 18
		szText += _T("Init Exp\t");			// 19
		szText += _T("Init Intimate\t");	// 20
		szText += _T("Female Ratio%\t");	// 21
		szText += _T("Exp Type\t");			// 22
		szText += _T("Hatch Time\t");		// 23
		szText += _T("Drop1\t");			// 24
		szText += _T("Drop2\r\n");			// 25
		expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));

		// format each field
		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			pBreed = rom.GetBreedListEntry(wBreed);
			if(!pBreed)
				throw 0;

			szText = _T("");

			// 1.id
			szTemp.Format(_T("%hu"), wBreed);
			szText += szTemp;

			// 2.kanto
			szTemp.Format(_T("%hu"), rom.m_pPokedexKantoOrder[wBreed]);
			szText = szText + _T("\t") + szTemp;

			// 3. houen
			szTemp.Format(_T("%hu"), rom.m_pPokedexHouenOrder[wBreed]);
			szText = szText + _T("\t") + szTemp;

			// 4.name
			szTemp.Format(_T("%s"), cfg.pBreedNameList[wBreed].rgszText[ui_lang_jp]);
			szText = szText + _T("\t") + szTemp;

			// 5.hp
			szTemp.Format(_T("%hu"), pBreed->bHP);
			szText += _T("\t") + szTemp;

			// 6.atk
			szTemp.Format(_T("%hu"), pBreed->bAtk);
			szText += _T("\t") + szTemp;

			// 7.def
			szTemp.Format(_T("%hu"), pBreed->bDef);
			szText += _T("\t") + szTemp;

			// 8.dex
			szTemp.Format(_T("%hu"), pBreed->bDex);
			szText += _T("\t") + szTemp;

			// 9.satk
			szTemp.Format(_T("%hu"), pBreed->bSAtk);
			szText += _T("\t") + szTemp;

			// 10.sdef
			szTemp.Format(_T("%hu"), pBreed->bSDef);
			szText += _T("\t") + szTemp;

			// 11.type1
			szTemp.Format(_T("%s"), szTypes[pBreed->bType1]);
			szText += _T("\t") + szTemp;

			// 12.type2
			szTemp.Format(_T("%s"), szTypes[pBreed->bType2]);
			szText += _T("\t") + szTemp;

			// 13.spec1
			if(pBreed->bSpecialty1)
				szTemp.Format(_T("%s"), cfg.pSpecNameList[pBreed->bSpecialty1].rgszText[ui_lang_jp]);
			else
				szTemp = _T("");
			szText += _T("\t") + szTemp;

			// 14.spec2
			if(pBreed->bSpecialty2)
				szTemp.Format(_T("%s"), cfg.pSpecNameList[pBreed->bSpecialty2].rgszText[ui_lang_jp]);
			else
				szTemp = _T("");
			szText += _T("\t") + szTemp;

			// 15.egg group1
			szTemp.Format(_T("%s"), szEggGroups[pBreed->bEggGroup1]);
			szText += _T("\t") + szTemp;

			// 16.egg group2
			szTemp.Format(_T("%s"), szEggGroups[pBreed->bEggGroup2]);
			szText += _T("\t") + szTemp;

			// 17.catch ratio
			szTemp.Format(_T("%lu"), ((DWORD)(pBreed->bCatchRatio) + 1) * 100 / 256);
			szText += _T("\t") + szTemp;

			// 18.escape ratio
			szTemp.Format(_T("%lu"), ((DWORD)(pBreed->bEscapeRatio) + 1) * 100 / 256);
			szText += _T("\t") + szTemp;

			// 19.init exp
			szTemp.Format(_T("%hu"), pBreed->bBaseExp);
			szText += _T("\t") + szTemp;

			// 20.init intimate
			szTemp.Format(_T("%hu"), pBreed->bInitIntimate);
			szText += _T("\t") + szTemp;

			// 21.female ratio
			switch(pBreed->bFemaleRatio)
			{
			case 0xFF:
				szTemp = _T("-1");
				break;
			case 0xFE:
				szTemp = _T("100");
				break;
			case 0x00:
				szTemp = _T("0");
				break;
			default:
				szTemp.Format(_T("%lu"), ((DWORD)(pBreed->bFemaleRatio) + 1) * 100 / 256);
				break;
			}
			szText += _T("\t") + szTemp;

			// 22.exp type
			szTemp.Format(_T("%lu"), (rom.GetExpList(pBreed->bExpType))[EXP_COUNT - 1] / 10000);
			szText += _T("\t") + szTemp;

			// 23.hatch time
			szTemp.Format(_T("%hu"), pBreed->bHatchTime);
			szText += _T("\t") + szTemp;

			// 24.drop1
			if(pBreed->wDrop1)
				szTemp.Format(_T("%s"), cfg.pItemNameList[pBreed->wDrop1].rgszText[ui_lang_jp]);
			else
				szTemp = _T("");
			szText += _T("\t") + szTemp;

			// 25.drop2
			if(pBreed->wDrop2)
				szTemp.Format(_T("%s"), cfg.pItemNameList[pBreed->wDrop2].rgszText[ui_lang_jp]);
			else
				szTemp = _T("");
			szText += _T("\t") + szTemp;
			
			szText += _T("\r\n");

			expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));
		}

		expFile.Close();

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	catch(CFileException *e)
	{
		e->Delete();
		bResult = FALSE;
	}

	if(bResult)
	{
		AfxMessageBox(_T("Succeeded!"));
	}
	else
	{
		AfxMessageBox(_T("Failed!"));
	}
}

namespace BRD_EXPORT
{
	struct PmEvoFrom
	{
		WORD	wFrom;
		WORD	wCondition;
		WORD	wParam;
	};

	struct PmSkillLearn
	{
		WORD	wFrom;
		WORD	rgwSkills[SKILL_COUNT];
	};

	void BuildLearnList(PmSkillLearn *pLearnList)
	{
		WORD	wBreed;
		WORD	wSkill;
		DWORD	dwIndex;
		BYTE	bEvoIndex;

		PmSkillLearn *	pLearn = NULL;
		EvoListEntry *	pEvo = NULL;
		WORD *	pMSkill = NULL;

		LvlupLearnListEntry *	pLvlupLearn = NULL;
		DWORD	dwLvlupLearnCount;

		WORD *	pDeriveLearn = NULL;
		DWORD	dwDeriveLearnCount;

		BYTE *	pMachineLearn = NULL;

		if(!rom.m_bOpened || pLearnList == NULL)
			return;

		pMSkill = rom.m_pMachineSkillList;

		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			pLearn = &(pLearnList[wBreed]);

			// 1.build self-learn list
			// 1)level up learn
			pLvlupLearn = rom.GetLvlupLearnList(wBreed);
			dwLvlupLearnCount = rom.GetLvlupLearnListEntryCount(wBreed);
			for(dwIndex = 0; dwIndex < dwLvlupLearnCount; ++dwIndex)
			{
				wSkill = pLvlupLearn[dwIndex].wSkill;
				pLearn->rgwSkills[wSkill] = 1;
			}
			// 2)derive learn
			pDeriveLearn = rom.GetDeriveLearnList(wBreed);
			dwDeriveLearnCount = rom.GetDeriveLearnListEntryCount(wBreed);
			for(dwIndex = 0; dwIndex < dwDeriveLearnCount; ++dwIndex)
			{
				wSkill = pDeriveLearn[dwIndex];
				pLearn->rgwSkills[wSkill] = 1;
			}
			// 3)machine learn
			pMachineLearn = rom.GetMachineLearnList(wBreed);
			for(dwIndex = 0; dwIndex < MACHINE_COUNT; ++dwIndex)
			{
				if(GetBit(pMachineLearn, dwIndex))
				{
					wSkill = pMSkill[dwIndex];
					pLearn->rgwSkills[wSkill] = 1;
				}
			}

			for(bEvoIndex = 0; bEvoIndex < EVO_LIST_COUNT; ++bEvoIndex)
			{
				pEvo = rom.GetEvoListEntry(wBreed, bEvoIndex);
				if(pEvo->wCondition != 0)
				{
					PmSkillLearn *	pLearn2 = &(pLearnList[pEvo->wBreed]);
					for(dwIndex = 0; dwIndex < SKILL_COUNT; ++dwIndex)
					{
						if(pLearn->rgwSkills[dwIndex])
							pLearn2->rgwSkills[dwIndex] = pLearn->rgwSkills[dwIndex];
					}
				}
			}
		}
	}
};

void CBreedAblPage::OnBnClickedExportEvo()
{
	// TODO: 在此添加控件通知处理程序代码
	#ifndef UNICODE
	#error UNICODE Version ONLY!
	#endif

	EvoConditions	rgEvoConditions[] =
	{
		{ L"進化不能",					L"",		evo_none },		// 00
		{ L"なつき度220",				L"",		evo_none },		// 01
		{ L"なつき度220、午前",			L"",		evo_none },		// 02
		{ L"なつき度220、午後",			L"",		evo_none },		// 03
		{ L"レベル%lu",					L"",		evo_level },	// 04
		{ L"通信交換",					L"",		evo_none },		// 05
		{ L"%sを持たせて通信交換",		L"",		evo_item },		// 06
		{ L"%s",						L"",		evo_item },		// 07
		{ L"攻撃>防御、レベル%lu",		L"",		evo_level },	// 08
		{ L"攻撃=防御、レベル%lu",		L"",		evo_level },	// 09
		{ L"攻撃<防御、レベル%lu",		L"",		evo_level },	// 0A
		{ L"性格値%%10<5、レベル%lu",	L"",		evo_level },	// 0B
		{ L"性格値%%10>4、レベル%lu",	L"",		evo_level },	// 0C
		{ L"手持ちがあり、レベル%lu",	L"",		evo_level },	// 0D
		{ L"手持ちがない、レベル%lu",	L"",		evo_level },	// 0E
		{ L"美しさ%lu",					L"",		evo_beauty }	// 0F
	};

	BOOL	bResult;
	WCHAR	wUnicodePrefix = 0xFEFF;

	CFile	expFile;
	BYTE	bEvoIndex;
	WORD	wBreed, wCondition;
	CString	szText, szTemp;

	BRD_EXPORT::PmEvoFrom *	pFrom = NULL;
	EvoListEntry *	pEvo = NULL;

	try
	{
		if(!rom.m_bOpened)
			throw 0;

		pFrom = new BRD_EXPORT::PmEvoFrom[BREED_COUNT];
		if(!pFrom)
			throw 0;
		ZeroMemory(pFrom, BREED_COUNT * sizeof(BRD_EXPORT::PmEvoFrom));

		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			for(bEvoIndex = 0; bEvoIndex < EVO_LIST_COUNT; ++bEvoIndex)
			{
				pEvo = rom.GetEvoListEntry(wBreed, bEvoIndex);
				if(pEvo->wCondition != 0)
				{
					pFrom[pEvo->wBreed].wFrom = wBreed;
					pFrom[pEvo->wBreed].wCondition = pEvo->wCondition;
					pFrom[pEvo->wBreed].wParam = pEvo->wValue;
				}
			}
		}

		// create export file
		SetCurrentDirectory(theApp.m_szInitialPath);
		szText = rom.GetRomName();
		szText += _T(" Evo.txt");
		bResult = expFile.Open(szText, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
		if(!bResult)
			throw 0;

		// write prefix of unicode text file
		expFile.Write(&wUnicodePrefix, sizeof(WCHAR));

		// write column headers
		szText = _T("ID\t");				// 1
		szText += _T("From\t");				// 2
		szText += _T("Condition\r\n");		// 3
		expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));

		// format each field
		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			szText = _T("");

			// 1.id
			szTemp.Format(_T("%hu"), wBreed);
			szText += szTemp;

			wCondition = pFrom[wBreed].wCondition;
			if(wCondition != 0)
			{
				// 2.from
				szTemp = cfg.pBreedNameList[pFrom[wBreed].wFrom].rgszText[ui_lang_jp];
				szText += _T("\t") + szTemp;

				// 3.condition
				switch(rgEvoConditions[wCondition].dwParamType)
				{
				case evo_none:
					szTemp.Format(rgEvoConditions[wCondition].szCondition);
					break;
				case evo_level:
				case evo_beauty:
					szTemp.Format(rgEvoConditions[wCondition].szCondition, pFrom[wBreed].wParam);
					break;
				case evo_item:
					szTemp.Format(rgEvoConditions[wCondition].szCondition, cfg.pItemNameList[pFrom[wBreed].wParam].rgszText[ui_lang_jp]);
					break;
				}
				szText += _T("\t") + szTemp;
			}

			szText += _T("\r\n");

			expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));
		}

		expFile.Close();

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	catch(CFileException *e)
	{
		e->Delete();
		bResult = FALSE;
	}

	if(pFrom)
		delete []pFrom;

	if(bResult)
	{
		AfxMessageBox(_T("Succeeded!"));
	}
	else
	{
		AfxMessageBox(_T("Failed!"));
	}
}

void CBreedAblPage::OnBnClickedExportLearn()
{
	// TODO: 在此添加控件通知处理程序代码
	#ifndef UNICODE
	#error UNICODE Version ONLY!
	#endif

	BOOL	bResult;
	WCHAR	wUnicodePrefix = 0xFEFF;

	CFile	expFile;
	WORD	wBreed, wSkill;
	CString	szText, szTemp;

	BRD_EXPORT::PmSkillLearn *	pLearn = NULL;

	try
	{
		if(!rom.m_bOpened)
			throw 0;

		// create export file
		SetCurrentDirectory(theApp.m_szInitialPath);
		szText = rom.GetRomName();
		szText += _T(" Learn.txt");
		bResult = expFile.Open(szText, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
		if(!bResult)
			throw 0;

		// build learn list
		pLearn = new BRD_EXPORT::PmSkillLearn[BREED_COUNT];
		if(!pLearn)
			throw 0;
		ZeroMemory(pLearn, BREED_COUNT * sizeof(BRD_EXPORT::PmSkillLearn));

		BRD_EXPORT::BuildLearnList(pLearn);

		// write prefix of unicode text file
		expFile.Write(&wUnicodePrefix, sizeof(WCHAR));

		// write column headers
		szText = _T("ID");			// 1
		szText += _T("\tSkill");	// 2
		szText += _T("\r\n");
		expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));

		// write data
		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			for(wSkill = 0; wSkill < SKILL_COUNT; ++wSkill)
			{
				if(pLearn[wBreed].rgwSkills[wSkill])
				{
					szText.Format(_T("%hu\t%hu\r\n"), wBreed, wSkill);
					expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));
				}
			}
		}

		expFile.Close();

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	catch(CFileException *e)
	{
		e->Delete();
		bResult = FALSE;
	}

	if(pLearn)
		delete []pLearn;

	if(bResult)
	{
		AfxMessageBox(_T("Succeeded!"));
	}
	else
	{
		AfxMessageBox(_T("Failed!"));
	}
}

void CBreedAblPage::OnBnClickedExportMachine()
{
	#ifndef UNICODE
	#error UNICODE Version ONLY!
	#endif

	BOOL	bResult;
	WCHAR	wUnicodePrefix = 0xFEFF;

	CFile	expFile;
	BYTE	bMachine;
	WORD	wSkill;
	CString	szText, szTemp;

	try
	{
		if(!rom.m_bOpened)
			throw 0;

		// create export file
		SetCurrentDirectory(theApp.m_szInitialPath);
		szText = rom.GetRomName();
		szText += _T(" Machine.txt");
		bResult = expFile.Open(szText, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
		if(!bResult)
			throw 0;

		// write prefix of unicode text file
		expFile.Write(&wUnicodePrefix, sizeof(WCHAR));

		// write column headers
		szText = _T("Name");		// 1
		szText += _T("\tSkill");	// 2
		szText += _T("\r\n");
		expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));

		// write data
		for(bMachine = 0; bMachine < MACHINE_COUNT; ++bMachine)
		{
			wSkill = rom.m_pMachineSkillList[bMachine];
			if(bMachine < 50)
				szText.Format(_T("わざマシン%02hu\t%hu\r\n"), bMachine + 1, wSkill);
			else
				szText.Format(_T("ひでんマシン%hu\t%hu\r\n"), bMachine - 49, wSkill);
			expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));
		}

		expFile.Close();

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	catch(CFileException *e)
	{
		e->Delete();
		bResult = FALSE;
	}

	if(bResult)
	{
		AfxMessageBox(_T("Succeeded!"));
	}
	else
	{
		AfxMessageBox(_T("Failed!"));
	}
}

void CBreedAblPage::OnBnClickedExportSkill()
{
	#ifndef UNICODE
	#error UNICODE Version ONLY!
	#endif

	static LPCTSTR szTypes[] =
	{
		L"ノーマル",	// 00
		L"かくとう",	// 01
		L"ひこう",		// 02
		L"どく",		// 03
		L"じめん",		// 04
		L"いわ",		// 05
		L"むし",		// 06
		L"ゴースト",	// 07
		L"はがね",		// 08
		L"？？？",		// 09
		L"ほのお",		// 0A
		L"みず",		// 0B
		L"くさ",		// 0C
		L"でんき",		// 0D
		L"エスパー",	// 0E
		L"こおり",		// 0F
		L"ドラゴン",	// 10
		L"あく",		// 11
		L"0x12",		// 12
		L"0x13",		// 13
		L"0x14",		// 14
		L"0x15",		// 15
		L"0x16",		// 16
		L"0x17",		// 17
		L"0x18"			// 18
	};

	static LPCTSTR szTargets[] =
	{
		_T("敵一体"),		// 00
		_T("不定"),			// 01
		_T("？？？"),		// 02
		_T("敵と自分"),		// 04
		_T("敵全体"),		// 08
		_T("自分"),			// 10
		_T("自分以外"),		// 20
		_T("全体"),			// 40
		_T("？？？")		// 80
	};

	BOOL	bResult;
	WCHAR	wUnicodePrefix = 0xFEFF;

	CFile	expFile;
	BYTE	bMachine, bTarget;
	WORD	wSkill;
	SkillListEntry *pSkill = NULL;
	CString	szText, szTemp;
	BYTE *	pSkills = NULL;

	try
	{
		if(!rom.m_bOpened)
			throw 0;

		pSkills = new BYTE[SKILL_COUNT];
		if(!pSkills)
			throw 0;
		ZeroMemory(pSkills, SKILL_COUNT * sizeof(BYTE));
		for(bMachine = 0; bMachine < MACHINE_COUNT; ++bMachine)
		{
			wSkill = rom.m_pMachineSkillList[bMachine];
			pSkills[wSkill] = bMachine + 1;
		}

		// create export file
		SetCurrentDirectory(theApp.m_szInitialPath);
		szText = rom.GetRomName();
		szText += _T(" Skill.txt");
		bResult = expFile.Open(szText, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
		if(!bResult)
			throw 0;

		// write prefix of unicode text file
		expFile.Write(&wUnicodePrefix, sizeof(WCHAR));

		// write column headers
		szText = _T("ID");			// 1
		szText += _T("\tName");		// 2
		szText += _T("\tType");		// 3
		szText += _T("\tPwr");		// 4
		szText += _T("\tAcc");		// 5
		szText += _T("\tPP");		// 6
		szText += _T("\tTarget");	// 7
		szText += _T("\tPrio");		// 8
		szText += _T("\tMachine");	// 9
		szText += _T("\tRatio");	// 10
		szText += _T("\tEff");		// 11
		szText += _T("\tSpec");		// 12
		szText += _T("\r\n");
		expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));

		// write data
		for(wSkill = 0; wSkill < SKILL_COUNT; ++wSkill)
		{
			pSkill = rom.GetSkillListEntry(wSkill);
			if(!pSkill)
				throw 0;

			szText = _T("");

			// 1.id
			szTemp.Format(_T("%hu"), wSkill);
			szText += szTemp;

			// 2.name
			szTemp = cfg.pSkillNameList[wSkill].rgszText[ui_lang_jp];
			szText += _T("\t") + szTemp;

			// 3.type
			szTemp = szTypes[pSkill->bType];
			szText += _T("\t") + szTemp;

			// 4.power
			szTemp.Format(_T("%hu"), pSkill->bPower);
			szText += _T("\t") + szTemp;

			// 5.accuracy
			szTemp.Format(_T("%hu"), pSkill->bAccuracy);
			szText += _T("\t") + szTemp;

			// 6.pp
			szTemp.Format(_T("%hu"), pSkill->bPP);
			szText += _T("\t") + szTemp;

			// 7.target
			bTarget = 0;
			while(pSkill->bTargetType >> bTarget != 0)
				++bTarget;
			szTemp = szTargets[bTarget];
			szText += _T("\t") + szTemp;

			// 8.priority
			szTemp.Format(_T("%ld"), (LONG)(pSkill->nPriority));
			szText += _T("\t") + szTemp;

			// 9.machine
			if(pSkills[wSkill] == 0)
			{
				szTemp = _T("");
			}
			else
			{
				if(pSkills[wSkill] <= 50)
					szTemp.Format(_T("わざマシン%02hu"), pSkills[wSkill]);
				else
					szTemp.Format(_T("ひでんマシン%hu"), pSkills[wSkill] - 50);
			}
			szText += _T("\t") + szTemp;

			// 10.ratio
			szTemp.Format(_T("%hu"), pSkill->bEffectRatio);
			szText += _T("\t") + szTemp;

			// 11.effect
			szTemp = cfg.pSkillEffList[pSkill->bEffect].rgszText[ui_lang_jp];
			szText += _T("\t") + szTemp;

			// 12 special
			szTemp = cfg.pSkillSpecList[wSkill].rgszText[ui_lang_jp];
			szText += _T("\t") + szTemp;

			szText += _T("\r\n");
			expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));
		}

		expFile.Close();

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	catch(CFileException *e)
	{
		e->Delete();
		bResult = FALSE;
	}

	if(pSkills)
		delete []pSkills;

	if(bResult)
	{
		AfxMessageBox(_T("Succeeded!"));
	}
	else
	{
		AfxMessageBox(_T("Failed!"));
	}
}

void CBreedAblPage::OnBnClickedExportIndex()
{
	#ifndef UNICODE
	#error UNICODE Version ONLY!
	#endif

	BOOL	bResult;
	WCHAR	wUnicodePrefix = 0xFEFF;

	CFile	expFile;
	WORD	wBreed;
	CString	szText, szTemp;

	try
	{
		if(!rom.m_bOpened)
			throw 0;

		// create export file
		SetCurrentDirectory(theApp.m_szInitialPath);
		szText = rom.GetRomName();
		szText += _T(" Index.txt");
		bResult = expFile.Open(szText, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
		if(!bResult)
			throw 0;

		// write prefix of unicode text file
		expFile.Write(&wUnicodePrefix, sizeof(WCHAR));

		// write column headers
		szText = _T("Inner");		// 1
		szText += _T("\tKanto");	// 2
		szText += _T("\tHouen");	// 3
		szText += _T("\r\n");
		expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));

		// write data
		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			szText.Format(_T("%hu\t%hu\t%hu\r\n"), wBreed, rom.m_pPokedexKantoOrder[wBreed], rom.m_pPokedexHouenOrder[wBreed]);
			expFile.Write((LPCTSTR)(szText), szText.GetLength() * sizeof(TCHAR));
		}

		expFile.Close();

		bResult = TRUE;
	}
	catch(int)
	{
		bResult = FALSE;
	}
	catch(CFileException *e)
	{
		e->Delete();
		bResult = FALSE;
	}

	if(bResult)
	{
		AfxMessageBox(_T("Succeeded!"));
	}
	else
	{
		AfxMessageBox(_T("Failed!"));
	}
}
