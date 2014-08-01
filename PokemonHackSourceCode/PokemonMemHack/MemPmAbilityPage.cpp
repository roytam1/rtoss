// MemPmAbilityPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemPmAbilityPage.h"


// CMemPmAbilityPage 对话框

IMPLEMENT_DYNAMIC(CMemPmAbilityPage, CMemPmTabPage)
CMemPmAbilityPage::CMemPmAbilityPage()
	:	m_ppc(NULL)
{
	m_uTemplateId = CMemPmAbilityPage::IDD;
}

CMemPmAbilityPage::~CMemPmAbilityPage()
{
}

void CMemPmAbilityPage::DoDataExchange(CDataExchange* pDX)
{
	CMemPmTabPage::DoDataExchange(pDX);

	BYTE	bIndex;

	for(bIndex = 0; bIndex < 6; ++bIndex)
	{
		DDX_Control(pDX, IDC_BB_HP + bIndex, m_ctrlBB[bIndex]);
	}
}


BEGIN_MESSAGE_MAP(CMemPmAbilityPage, CMemPmTabPage)
	ON_BN_CLICKED(IDC_INDV_MAX, OnBnClickedIndvMax)
	ON_BN_CLICKED(IDC_BB_85, OnBnClickedBb85)
	ON_BN_CLICKED(IDC_BB_MAX, OnBnClickedBbMax)
	ON_BN_CLICKED(IDC_AP_MAX, OnBnClickedApMax)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_BB_HP, IDC_BB_SDEF, GetBB)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_INDV_HP, IDC_INDV_SDEF, GetIndv)
END_MESSAGE_MAP()


// CMemPmAbilityPage 消息处理程序
void CMemPmAbilityPage::TransData(BOOL bToControls)
{
	BYTE	bIndex;
	DWORD	dwNum;
	CString	szText;

	m_ppc = GetPokemonCodec();
	if(m_ppc == NULL)
		return;

	if(bToControls)
	{
		DWORD	dwBreedAblSum = 0;
		for(bIndex = 0; bIndex < 6; ++bIndex)
		{
			dwNum = m_ppc->GetIndvAbility(bIndex);
			szText.Format(_T("%lu"), dwNum);
			SetDlgItemText(IDC_INDV_HP + bIndex, szText);

			dwNum = m_ppc->GetBattleBonus(bIndex);
			szText.Format(_T("%lu"), dwNum);
			SetDlgItemText(IDC_BB_HP + bIndex, szText);

			SetBBSum();

			dwNum = m_ppc->GetApealPoint(bIndex);
			szText.Format(_T("%lu"), dwNum);
			SetDlgItemText(IDC_AP_COOL + bIndex, szText);
		}

		SetBreedAbilities();
	}
	else
	{
		for(bIndex = 0; bIndex < 6; ++bIndex)
		{
			GetDlgItemText(IDC_AP_COOL + bIndex, szText);
			dwNum = min(_tcstoul(szText, 0, 0), 0xFF);
			m_ppc->SetApealPoint(bIndex, (BYTE)(dwNum));
		}
	}
}

void CMemPmAbilityPage::SetBreedAbilities()
{
	DWORD	dwAbl;
	DWORD	dwBreedAblSum = 0;
	BYTE	bIndex;
	CString	szText;
	BreedListEntry * bleTmp;

	if(!m_ppc || !g_MemRom.m_bOpened)
	{
		for(bIndex = 0; bIndex < 6; ++bIndex)
		{
			SetDlgItemText(IDC_BREED_HP + bIndex, _T(""));
		}
		SetDlgItemText(IDC_BREED_ABL_SUM, _T(""));
	}

	for(bIndex = 0; bIndex < 6; ++bIndex)
	{
		bleTmp = g_MemRom.GetBreedListEntry(m_ppc->GetBreed());
		if(bleTmp)
		{
			dwAbl = (&(bleTmp->bHP))[bIndex];
			szText.Format(_T("%lu"), dwAbl);
			SetDlgItemText(IDC_BREED_HP + bIndex, szText);
			dwBreedAblSum += dwAbl;
		}
	}

	szText.Format(_T("%lu"), dwBreedAblSum);
	SetDlgItemText(IDC_BREED_ABL_SUM, szText);
}

void CMemPmAbilityPage::OnChangeUILang(VOID)
{
}

void CMemPmAbilityPage::OnBnClickedIndvMax()
{
	if(m_ppc)
	{
		m_ppc->SetMaxIndvAbilities();
		TransData(TRUE);
	}
}

void CMemPmAbilityPage::OnBnClickedBb85()
{
	BYTE	bIndex;
	CString	szText;

	if(m_ppc)
	{
		szText = _T("85");
		for(bIndex = 0; bIndex < 6; ++bIndex)
		{
			SetDlgItemText(IDC_BB_HP + bIndex, szText);
		}
	}
}

void CMemPmAbilityPage::OnBnClickedBbMax()
{
	if(m_ppc)
	{
		m_ppc->SetMaxBattleBonuses();
		TransData(TRUE);
	}
}

void CMemPmAbilityPage::OnBnClickedApMax()
{
	if(m_ppc)
	{
		m_ppc->SetMaxApealPoints();
		TransData(TRUE);
	}
}

void CMemPmAbilityPage::GetBB(UINT uID)
{
	DWORD	dwIndex;
	BYTE	bBB;
	CString	szText;

	if(m_ppc)
	{
		dwIndex = uID - IDC_BB_HP;
		if(dwIndex < 6)
		{
			GetDlgItemText(uID, szText);
			bBB = (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));
			m_ppc->SetBattleBonus((BYTE)(dwIndex), bBB);
			SetBBSum();
		}
	}
}

void CMemPmAbilityPage::SetBBSum()
{
	BYTE	bIndex;
	DWORD	dwSum = 0;

	if(m_ppc)
	{
		for(bIndex = 0; bIndex < 6; ++bIndex)
		{
			dwSum += m_ppc->GetBattleBonus(bIndex);
			SetDlgItemInt(IDC_BB_SUM, dwSum, FALSE);
		}
	}
}

void CMemPmAbilityPage::GetIndv(UINT uID)
{
	DWORD	dwIndex;
	BYTE	bIndv;
	CString	szText;

	if(m_ppc)
	{
		dwIndex = uID - IDC_INDV_HP;
		if(dwIndex < 6)
		{
			GetDlgItemText(uID, szText);
			bIndv = (BYTE)(min(_tcstoul(szText, 0, 0), 0x1F));
			m_ppc->SetIndvAbility((BYTE)(dwIndex), bIndv);
		}
	}
}
