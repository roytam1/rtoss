// MemMiscPage2.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemMiscPage2.h"

// CMemMiscPage2 对话框

IMPLEMENT_DYNAMIC(CMemMiscPage2, CPropertyPage)
CMemMiscPage2::CMemMiscPage2()
	: CPropertyPage(CMemMiscPage2::IDD)
{
	m_uTimerId = 0;
}

CMemMiscPage2::~CMemMiscPage2()
{
	UnsetupTimer();
}

void CMemMiscPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_VERSION_LIST, m_ctrlGameVersionList);
}


BEGIN_MESSAGE_MAP(CMemMiscPage2, CPropertyPage)
	ON_BN_CLICKED(IDC_UPGRADE_POKEDEX, OnBnClickedUpgradePokedex)
	ON_BN_CLICKED(IDC_FULL_POKEDEX, OnBnClickedFullPokedex)
	ON_BN_CLICKED(IDC_GIVE_BADGES, OnBnClickedGiveBadges)
	ON_BN_CLICKED(IDC_ENABLE_MIRAGE_ISLAND, OnBnClickedEnableMirageIsland)
	ON_BN_CLICKED(IDC_GIVE_ALL_POKEMON, OnBnClickedGiveAllPokemon)
	ON_BN_CLICKED(IDC_SET_POKEBALL, OnBnClickedSetPokeball)
	ON_BN_CLICKED(IDC_SET_INDV, OnBnClickedSetIndv)
	ON_BN_CLICKED(IDC_SET_OBEDIENCE, OnBnClickedSetObedience)
	ON_BN_CLICKED(IDC_ADJUST_CLOCK, OnBnClickedAdjustClock)
	ON_BN_CLICKED(IDC_LOCK_STEP_COUNTER_TO_FE, OnBnClickedLockStepCounterToFe)
	ON_BN_CLICKED(IDC_LOCK_DAYCARE_COUNTER2_TO_FE, OnBnClickedLockDaycareCounter2ToFe)
	ON_BN_CLICKED(IDC_LOCK_EXP_GAIN_TO_7FFF, OnBnClickedLockExpGainTo7fff)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMemMiscPage2 消息处理程序
void CMemMiscPage2::OnChangeUILang(VOID)
{
}

BOOL CMemMiscPage2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	DWORD	dwIndex;
	CString	szText;

	CheckDlgButton(IDC_PC_NAME_JP, BST_CHECKED);
	CheckDlgButton(IDC_PC_SEX_RANDOM, BST_CHECKED);

	for(dwIndex = 0; dwIndex < dwGameVersionsCount; ++dwIndex)
	{
		szText.Format(_T("%lX: %s"), dwIndex, szGameVersions[dwIndex]);
		m_ctrlGameVersionList.AddString(szText);
	}
	m_ctrlGameVersionList.SetCurSel(pm_emerald);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CMemMiscPage2::OnBnClickedUpgradePokedex()
{
	if(!g_MemHack.UpgradePokedex())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedFullPokedex()
{
	if(!g_MemHack.SetFullPokedex())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedGiveBadges()
{
	if(!g_MemHack.GiveAllBadges())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedEnableMirageIsland()
{
	if(!g_MemHack.EnableMirageIsland())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedGiveAllPokemon()
{
	BOOL	bRandId;
	BOOL	bShiny;
	BOOL	bMaxIndv;
	BOOL	bMaxBB;
	BOOL	bMaxAP;
	BOOL	bMaxInti;
	BOOL	bUseNumber;
	DWORD	dwLang;
	BYTE	bGameVersion;
	BYTE	bSex;
	BYTE	bLevel;
	BreedListEntry *	pBreed = NULL;

	if(!g_MemRom.m_bOpened)
	{
		AfxMessageBox(_T("游戏资料还未从内存读入，请先刷新队伍中的宠物！"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	bRandId     = (IsDlgButtonChecked(IDC_RAND_ID)			== BST_CHECKED) ? TRUE : FALSE;
	bShiny		= (IsDlgButtonChecked(IDC_SHINY)			== BST_CHECKED) ? TRUE : FALSE;
	bMaxIndv	= (IsDlgButtonChecked(IDC_MAX_INDV)			== BST_CHECKED) ? TRUE : FALSE;
	bMaxBB		= (IsDlgButtonChecked(IDC_MAX_BB)			== BST_CHECKED) ? TRUE : FALSE;
	bMaxAP		= (IsDlgButtonChecked(IDC_MAX_AP)			== BST_CHECKED) ? TRUE : FALSE;
	bMaxInti	= (IsDlgButtonChecked(IDC_MAX_INTI)			== BST_CHECKED) ? TRUE : FALSE;
	bUseNumber	= (IsDlgButtonChecked(IDC_PC_NAME_NUMBER)	== BST_CHECKED) ? TRUE : FALSE;

	if(IsDlgButtonChecked(IDC_PC_NAME_JP) == BST_CHECKED)
		dwLang = lang_jp;
	else
		dwLang = lang_en;

	bGameVersion = m_ctrlGameVersionList.GetCurSel();

	if(IsDlgButtonChecked(IDC_PC_SEX_FEMALE) == BST_CHECKED)
		bSex = 1;
	else if(IsDlgButtonChecked(IDC_PC_SEX_MALE) == BST_CHECKED)
		bSex = 2;
	else	// random
		bSex = 0;

	bLevel = (BYTE)(max(min(GetDlgItemInt(IDC_PC_LEVEL, NULL, 0), 100), 0));

	BOOL	bResult = TRUE;
	BOOL	bTrainerNameOk;
	WORD	wIndex, wBreed;
	DWORD	dwPlayerId = 0;
	CString	szName;
	CPokemonCodec *	ppc = NULL;
	BYTE	bTrainerName[POKEMON_TRAINER_NAME_SIZE];

	if(!bRandId)
		bResult = g_MemHack.GetPlayerId(dwPlayerId);

	if(bResult)
	{
		bResult = g_MemHack.ReadPokemonsInPC();
	}

	if(bResult)
	{
		bTrainerNameOk = g_MemHack.GetPlayerName(bTrainerName);

		for(wIndex = 0; wIndex < 386; ++wIndex)
		{
			ppc = &(g_MemHack.m_pStoredPokeCodec[wIndex]);

			wBreed = ConvertBreed(wIndex + 1, TRUE);
			pBreed = g_MemRom.GetBreedListEntry(wBreed);
			ppc->CreatePokemon(wBreed, dwLang, bGameVersion);

			if(pBreed->bSpecialty2 != 0)
				ppc->SetSpecialty(GenShortRandom() % 2);

			if(!bRandId)
				ppc->SetID(dwPlayerId);

			if(bShiny)
				ppc->SetChar(ppc->GenShinyChar());

			if(bMaxIndv)
				ppc->SetMaxIndvAbilities();
			else
				ppc->SetRandIndvAbilities();

			if(bMaxBB)
				ppc->SetMaxBattleBonuses();

			if(bMaxAP)
				ppc->SetMaxApealPoints();

			if(bMaxInti)
				ppc->SetIntimate(0xFF);

			if(bTrainerNameOk)
				ppc->SetCatcherName(bTrainerName);

			if(!bUseNumber)
			{
				switch(dwLang)
				{
				case lang_jp:
					ppc->SetNickName(cfg.pBreedNameList[wBreed].rgszText[ui_lang_jp]);
					break;
				case lang_en:
					ppc->SetNickName(cfg.pBreedNameList[wBreed].rgszText[ui_lang_en]);
					break;
				}
			}
			else
			{
				BYTE	bCode[POKEMON_TRAINER_NAME_SIZE];
				szName.Format(_T("%03hu"), wBreed);
				StringToCode(szName, bCode, POKEMON_TRAINER_NAME_SIZE, 0xFF, 0xFF, lang_en);
				ppc->SetNickName(bCode);
			}

			if(bSex == 1)	// female all
			{
				ppc->SetSex(pm_female, pBreed->bFemaleRatio);
			}
			else if(bSex == 2)	// male all
			{
				ppc->SetSex(pm_male, pBreed->bFemaleRatio);
			}

			ppc->SetExp((g_MemRom.GetExpList(pBreed->bExpType))[bLevel]);
		}

		bResult = g_MemHack.SavePokemonsInPC();
	}

	if(!bResult)
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedSetPokeball()
{
	BOOL	bResult = TRUE;
	WORD	wIndex, wBreed;
	CPokemonCodec *	ppc = NULL;

	bResult = g_MemHack.ReadPokemonsInPC();

	if(bResult)
	{
		for(wIndex = 0; wIndex < STORED_POKEMON_COUNT; ++wIndex)
		{
			ppc = &(g_MemHack.m_pStoredPokeCodec[wIndex]);

			wBreed = ppc->GetBreed();
			if(wBreed == 0 || wBreed >= BREED_COUNT)
				continue;

			ppc->SetPokeBall(0x00B);	// ゴージャスボール
		}

		bResult = g_MemHack.SavePokemonsInPC();
	}

	if(!bResult)
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedSetIndv()
{
	BOOL	bMaxIndv;
	BOOL	bResult = TRUE;
	WORD	wIndex, wBreed;
	CPokemonCodec *	ppc = NULL;

	bMaxIndv	= (IsDlgButtonChecked(IDC_MAX_INDV) == BST_CHECKED) ? TRUE : FALSE;

	bResult = g_MemHack.ReadPokemonsInPC();

	if(bResult)
	{
		for(wIndex = 0; wIndex < STORED_POKEMON_COUNT; ++wIndex)
		{
			ppc = &(g_MemHack.m_pStoredPokeCodec[wIndex]);

			wBreed = ppc->GetBreed();
			if(wBreed == 0 || wBreed >= BREED_COUNT)
				continue;

			if(bMaxIndv)
				ppc->SetMaxIndvAbilities();
			else
				ppc->SetRandIndvAbilities();
		}

		bResult = g_MemHack.SavePokemonsInPC();
	}

	if(!bResult)
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedSetObedience()
{
	BOOL	bResult = TRUE;
	WORD	wIndex, wBreed;
	CPokemonCodec *	ppc = NULL;

	bResult = g_MemHack.ReadPokemonsInPC();

	if(bResult)
	{
		for(wIndex = 0; wIndex < STORED_POKEMON_COUNT; ++wIndex)
		{
			ppc = &(g_MemHack.m_pStoredPokeCodec[wIndex]);

			wBreed = ppc->GetBreed();
			if(wBreed == 0 || wBreed >= BREED_COUNT)
				continue;

			ppc->SetObedience();
		}

		bResult = g_MemHack.SavePokemonsInPC();
	}

	if(!bResult)
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage2::OnBnClickedAdjustClock()
{
	if(!g_MemHack.AdjustClock())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

BOOL CMemMiscPage2::SetupTimer()
{
	BOOL	bResult = TRUE;

	if(m_uTimerId)
	{
		if(!m_dwTimerFlags)
		{
			UnsetupTimer();
		}
	}
	else
	{
		if(m_dwTimerFlags)
		{
			m_uTimerId = SetTimer(1, 200, NULL);
			if(!m_uTimerId)
			{
				CheckDlgButton(IDC_LOCK_STEP_COUNTER_TO_FE, BST_UNCHECKED);
				CheckDlgButton(IDC_LOCK_DAYCARE_COUNTER2_TO_FE, BST_UNCHECKED);
				CheckDlgButton(IDC_LOCK_EXP_GAIN_TO_7FFF, BST_UNCHECKED);
				m_dwTimerFlags = 0;
				AfxMessageBox(_T("无法创建时钟！"));
				bResult = FALSE;
			}
		}
	}

	return bResult;
}

VOID CMemMiscPage2::UnsetupTimer()
{
	if(m_uTimerId)
	{
		KillTimer(m_uTimerId);
		m_uTimerId = 0;
	}
}

void CMemMiscPage2::OnTimer(UINT nIDEvent)
{
	CPropertyPage::OnTimer(nIDEvent);

	BOOL	bResult = TRUE;

	if(nIDEvent == m_uTimerId)
	{
		if(m_dwTimerFlags & 1)
		{
			bResult = g_MemHack.SetStepCounter(0xFE);
		}
		if(bResult && (m_dwTimerFlags & 2))
		{
			bResult = g_MemHack.SetDaycareCenterStepByte(1, 0xFE);
		}
		if(bResult && (m_dwTimerFlags & 4))
		{
			bResult = g_MemHack.SetExpGain(0x7FFF);
		}

		if(!bResult)
		{
			m_dwTimerFlags = 0;
			SetupTimer();
		}
	}
}

void CMemMiscPage2::OnBnClickedLockStepCounterToFe()
{
	BOOL	bState;

	bState = (IsDlgButtonChecked(IDC_LOCK_STEP_COUNTER_TO_FE) == BST_CHECKED) ? TRUE : FALSE;
	if(bState)
		m_dwTimerFlags |= 1;
	else
		m_dwTimerFlags &= ~1;

	SetupTimer();
}

void CMemMiscPage2::OnBnClickedLockDaycareCounter2ToFe()
{
	BOOL	bState;

	bState = (IsDlgButtonChecked(IDC_LOCK_DAYCARE_COUNTER2_TO_FE) == BST_CHECKED) ? TRUE : FALSE;
	if(bState)
		m_dwTimerFlags |= 2;
	else
		m_dwTimerFlags &= ~2;

	SetupTimer();
}

void CMemMiscPage2::OnBnClickedLockExpGainTo7fff()
{
	BOOL	bState;

	bState = (IsDlgButtonChecked(IDC_LOCK_EXP_GAIN_TO_7FFF) == BST_CHECKED) ? TRUE : FALSE;
	if(bState)
	{
		m_dwTimerFlags |= 4;
	}
	else
	{
		m_dwTimerFlags &= ~4;
		g_MemHack.SetExpGain(0);
	}

	SetupTimer();
}
