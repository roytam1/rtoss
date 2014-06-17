// MemPmMiscPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemPmMiscPage.h"

// CMemPmMiscPage 对话框

IMPLEMENT_DYNAMIC(CMemPmMiscPage, CMemPmTabPage)
CMemPmMiscPage::CMemPmMiscPage()
{
	m_uTemplateId = CMemPmMiscPage::IDD;

	DWORD	dwIndex;

	for(dwIndex = 0; dwIndex < 4; ++dwIndex)
	{
		m_hMark[dwIndex] = NULL;
	}
}

CMemPmMiscPage::~CMemPmMiscPage()
{
	DWORD	dwIndex;

	for(dwIndex = 0; dwIndex < 4; ++dwIndex)
	{
		if(m_hMark[dwIndex])
			DeleteObject(m_hMark[dwIndex]);
	}
}

void CMemPmMiscPage::DoDataExchange(CDataExchange* pDX)
{
	CMemPmTabPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POKEBALL_LIST, m_ctrlPokeballList);
	DDX_Control(pDX, IDC_AREA_LIST, m_ctrlAreaList);
	DDX_Control(pDX, IDC_GAME_VERSION_LIST, m_ctrlGameVersionList);
	DDX_Control(pDX, IDC_MARK0, m_ctrlMark[0]);
	DDX_Control(pDX, IDC_MARK1, m_ctrlMark[1]);
	DDX_Control(pDX, IDC_MARK2, m_ctrlMark[2]);
	DDX_Control(pDX, IDC_MARK3, m_ctrlMark[3]);
	DDX_Control(pDX, IDC_DECODE, m_ctrlDecode);
	DDX_Control(pDX, IDC_ENCODE, m_ctrlEncode);
	DDX_Control(pDX, IDC_RESET, m_ctrlReset);
	DDX_Control(pDX, IDC_ADDRESS, m_ctrlAddress);
}


BEGIN_MESSAGE_MAP(CMemPmMiscPage, CMemPmTabPage)
	ON_BN_CLICKED(IDC_DECODE, OnBnClickedDecode)
	ON_BN_CLICKED(IDC_ENCODE, OnBnClickedEncode)
	ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
	ON_BN_CLICKED(IDC_AUTO_CALC, OnBnClickedAutoCalc)
	ON_STN_DBLCLK(IDC_SECRET, OnStnDblclickSecret)
END_MESSAGE_MAP()


// CMemPmMiscPage 消息处理程序
void CMemPmMiscPage::TransData(BOOL bToControls)
{
	BYTE	bIndex;
	DWORD	dwNum;
	CString	szText;

	m_ppc = GetPokemonCodec();
	if(m_ppc == NULL)
		return;

	if(bToControls)
	{
		dwNum = m_ppc->GetPokeBall();
		m_ctrlPokeballList.SetCurSel(dwNum);

		dwNum = m_ppc->GetCatchPlace();
		m_ctrlAreaList.SetCurSel(dwNum);

		dwNum = m_ppc->GetCatchLevel();
		szText.Format(_T("%lu"), dwNum);
		SetDlgItemText(IDC_INIT_LEVEL, szText);

		dwNum = m_ppc->GetGameVersion();
		m_ctrlGameVersionList.SetCurSel(dwNum);

		CheckDlgButton(IDC_POKERUS, m_ppc->GetPokerus() != 0 ? BST_CHECKED : BST_UNCHECKED);

		CheckDlgButton(IDC_BLACK_POINT, m_ppc->GetBlackPoint() != 0 ? BST_CHECKED : BST_UNCHECKED);

		for(bIndex = 0; bIndex < 4; ++bIndex)
		{
			CheckDlgButton(IDC_MARK0 + bIndex, m_ppc->GetMarking(bIndex) ? BST_CHECKED : BST_UNCHECKED);
		}

		SetPmRear();
	}
	else
	{
		dwNum = m_ctrlPokeballList.GetCurSel();
		if(dwNum <= 0x0C)
			m_ppc->SetPokeBall((BYTE)(dwNum));

		dwNum = m_ctrlAreaList.GetCurSel();
		if(dwNum < AREA_COUNT)
			m_ppc->SetCatchPlace((BYTE)(dwNum));

		GetDlgItemText(IDC_INIT_LEVEL, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0x64);
		m_ppc->SetCatchLevel((BYTE)(dwNum));

		dwNum = min((DWORD)(m_ctrlGameVersionList.GetCurSel()), dwGameVersionsCount - 1);
		m_ppc->SetGameVersion((BYTE)(dwNum));

		if(IsDlgButtonChecked(IDC_POKERUS))
			m_ppc->SetPokerus(0xF);
		else
			m_ppc->SetPokerus(0x0);

		if(IsDlgButtonChecked(IDC_BLACK_POINT))
			m_ppc->SetBlackPoint(0xF);
		else
			m_ppc->SetBlackPoint(0x0);

		for(bIndex = 0; bIndex < 4; ++bIndex)
		{
			m_ppc->SetMarking(bIndex, IsDlgButtonChecked(IDC_MARK0 + bIndex) == BST_CHECKED ? 1 : 0);
		}

		GetPmRear();
	}
}

void CMemPmMiscPage::OnChangeUILang(VOID)
{
	DWORD	dwIndex, dwCurSel;
	CString	szText;
	LPCTSTR	szFmt2[3] = { _T(""), _T("%-2lu: "), _T("%02lX: ") };
	LPCTSTR	szFmt3[3] = { _T(""), _T("%-3lu: "), _T("%02lX: ") };

	////////////////////////////////////////////////
	dwCurSel = m_ctrlPokeballList.GetCurSel();
	if(dwCurSel == LB_ERR)
		dwCurSel = 0;
	m_ctrlPokeballList.ResetContent();
	for(dwIndex = 0; dwIndex <= 0x0C; ++dwIndex)
	{
		szText.Format(szFmt2[cfg.dwCount], dwIndex);
		szText += cfg.pItemNameList[dwIndex].rgszText[cfg.dwLang];
		m_ctrlPokeballList.AddString(szText);
	}
	m_ctrlPokeballList.SetCurSel(dwCurSel);

	////////////////////////////////////////////////
	dwCurSel = m_ctrlAreaList.GetCurSel();
	if(dwCurSel == LB_ERR)
		dwCurSel = 0;
	m_ctrlAreaList.ResetContent();
	for(dwIndex = 0; dwIndex < AREA_COUNT; ++dwIndex)
	{
		szText.Format(szFmt3[cfg.dwCount], dwIndex);
		szText += cfg.pAreaNameList[dwIndex].rgszText[cfg.dwLang];
		m_ctrlAreaList.AddString(szText);
	}
	m_ctrlAreaList.SetCurSel(dwCurSel);
}

BOOL CMemPmMiscPage::OnInitDialog()
{
	CMemPmTabPage::OnInitDialog();

	DWORD	dwIndex;
	CString	szText;

#ifdef _DEBUG
	m_ctrlDecode.EnableWindow(TRUE);
	m_ctrlEncode.EnableWindow(FALSE);
	m_ctrlReset.EnableWindow(TRUE);
	m_ctrlAddress.EnableWindow(TRUE);

	m_ctrlDecode.ShowWindow(SW_SHOW);
	m_ctrlEncode.ShowWindow(SW_SHOW);
	m_ctrlReset.ShowWindow(SW_SHOW);
	m_ctrlAddress.ShowWindow(SW_SHOW);
#endif

	for(dwIndex = 0; dwIndex < 4; ++dwIndex)
	{
		m_hMark[dwIndex] = LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_MARK0 + dwIndex));
		m_ctrlMark[dwIndex].SetBitmap(m_hMark[dwIndex]);
	}

	for(dwIndex = 0; dwIndex < dwGameVersionsCount; ++dwIndex)
	{
		szText.Format(_T("%lX: %s"), dwIndex, szGameVersions[dwIndex]);
		m_ctrlGameVersionList.AddString(szText);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMemPmMiscPage::SetPmRear()
{
	BYTE	bIndex, bStatus;
	WORD *	pwAbilities = NULL;
	PokemonStructActive *ppsa = GetPokemonStructActive();

	if(ppsa)
	{
		// level
		SetDlgItemInt(IDC_CUR_LEVEL, ppsa->pmRear.bLevel, FALSE);

		// abilities
		pwAbilities = (LPWORD)(&(ppsa->pmRear.wHPcur));
		for(bIndex = 0; bIndex < 7; ++bIndex)
		{
			SetDlgItemInt(IDC_CUR_HP_CUR + bIndex, pwAbilities[bIndex], FALSE);
		}

		// status
		SetDlgItemInt(IDC_ST_SLEEP, ppsa->pmRear.bStatus & 0x07, FALSE);
		for(bIndex = 0; bIndex < 5; ++bIndex)
		{
			bStatus = (ppsa->pmRear.bStatus >> (bIndex + 3)) & 0x01;
			CheckDlgButton(IDC_ST_POISON + bIndex, bStatus ? BST_CHECKED : BST_UNCHECKED);
		}
	}
}

void CMemPmMiscPage::GetPmRear()
{
	BYTE	bIndex, bStatus;
	WORD *	pwAbilities = NULL;
	CString	szText;
	PokemonStructActive *ppsa = GetPokemonStructActive();

	if(ppsa)
	{
		// level
		GetDlgItemText(IDC_CUR_LEVEL, szText);
		ppsa->pmRear.bLevel = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));

		// abilities
		pwAbilities = (LPWORD)(&(ppsa->pmRear.wHPcur));
		for(bIndex = 0; bIndex < 7; ++bIndex)
		{
			GetDlgItemText(IDC_CUR_HP_CUR + bIndex, szText);
			pwAbilities[bIndex] = (WORD)(min(_tcstoul(szText, 0, 0), 999));
		}

		// status
		bStatus = 0;
		GetDlgItemText(IDC_ST_SLEEP, szText);
		bStatus |= (BYTE)(min(_tcstoul(szText, 0, 0), 0x07));
		for(bIndex = 0; bIndex < 5; ++bIndex)
		{
			if(IsDlgButtonChecked(IDC_ST_POISON + bIndex) == BST_CHECKED)
				bStatus |= 1 << (bIndex + 3);
		}
		ppsa->pmRear.bStatus = bStatus;
	}
}

void CMemPmMiscPage::SetAddress()
{
	DWORD	dwAddress;
	CString	szAddress;

	if(g_MemHack.GetFirstActivePokemonAddress(dwAddress))
	{
		szAddress.Format(_T("%08lX"), dwAddress);
		SetDlgItemText(IDC_ADDRESS, szAddress);
	}
	else
	{
		SetDlgItemText(IDC_ADDRESS, _T("无法获得地址！"));
	}
}

void CMemPmMiscPage::OnBnClickedDecode()
{
	if(!g_MemHack.DecodeFirstActivePokemon())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS);
	}
	else
	{
		m_ctrlDecode.EnableWindow(FALSE);
		m_ctrlEncode.EnableWindow(TRUE);
	}

	SetAddress();
}

void CMemPmMiscPage::OnBnClickedEncode()
{
	if(!g_MemHack.EncodeFirstActivePokemon())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS);
	}
	else
	{
		m_ctrlDecode.EnableWindow(TRUE);
		m_ctrlEncode.EnableWindow(FALSE);
	}

	SetAddress();
}

void CMemPmMiscPage::OnBnClickedReset()
{
	m_ctrlDecode.EnableWindow(TRUE);
	m_ctrlEncode.EnableWindow(TRUE);
}

void CMemPmMiscPage::OnBnClickedAutoCalc()
{
	BYTE	bExpType, bLevel, bPs, bIndex;
	WORD	wBreed, wBreedAbl, wIndvAbl, wBBAbl;
	DWORD	dwExp;
	LPWORD	pwAbilities = NULL;
	WORD	wPsMod[5] = { 10, 10, 10, 10, 10 };
	PokemonStructActive *ppsa = GetPokemonStructActive();
	BreedListEntry *	pBreed = NULL;
	DWORD *				pdwExpList = NULL;
	WORD *				pwDeoxysBreedAbilities = NULL;

	if(!m_ppc || !ppsa)
		return;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);

		// level
		bExpType = pBreed->bExpType;
		pdwExpList = g_MemRom.GetExpList(bExpType);
		dwExp = m_ppc->GetExp();

		for(bLevel = 1; bLevel < EXP_COUNT; ++bLevel)
		{
			if(dwExp < pdwExpList[bLevel])
				break;
		}
		--bLevel;
		ppsa->pmRear.bLevel = bLevel;

		// PS modification
		bPs = m_ppc->GetPersonality();
		wPsMod[bPs / 5] += 1;
		wPsMod[bPs % 5] -= 1;

		// HPcap, Atk, Def, Dex, SAtk, SDef
		// maximize HPcur
		pwDeoxysBreedAbilities = g_MemRom.GetDeoxysBreedAbilities();
		pwAbilities = (LPWORD)(&(ppsa->pmRear.wHPcap));

		for(bIndex = 0; bIndex < 6; ++bIndex)
		{
			if(wBreed == 410 && pwDeoxysBreedAbilities)	// ディオキシス
				wBreedAbl = pwDeoxysBreedAbilities[bIndex];
			else
				wBreedAbl = (&(pBreed->bHP))[bIndex];

			wIndvAbl = m_ppc->GetIndvAbility(bIndex);
			wBBAbl = m_ppc->GetBattleBonus(bIndex);

			if(bIndex == 0)	// HP
			{
				pwAbilities[bIndex] =
					((wBreedAbl << 1) + wIndvAbl + (wBBAbl >> 2)) * bLevel / 100 + bLevel + 10;
			}
			else	// Other
			{
				pwAbilities[bIndex] =
					(((wBreedAbl << 1) + wIndvAbl + (wBBAbl >> 2)) * bLevel / 100 + 5) * wPsMod[bIndex - 1] / 10;
			}
		}

		// current HP
		ppsa->pmRear.wHPcur = ppsa->pmRear.wHPcap;

		SetPmRear();
	}
}

void CMemPmMiscPage::OnStnDblclickSecret()
{
	m_ctrlDecode.EnableWindow(TRUE);
	m_ctrlEncode.EnableWindow(FALSE);
	m_ctrlReset.EnableWindow(TRUE);
	m_ctrlAddress.EnableWindow(TRUE);

	m_ctrlDecode.ShowWindow(SW_SHOW);
	m_ctrlEncode.ShowWindow(SW_SHOW);
	m_ctrlReset.ShowWindow(SW_SHOW);
	m_ctrlAddress.ShowWindow(SW_SHOW);
}
