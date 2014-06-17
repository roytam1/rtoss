// MemPmRibbonPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemPmRibbonPage.h"

// CMemPmRibbonPage 对话框

IMPLEMENT_DYNAMIC(CMemPmRibbonPage, CMemPmTabPage)
CMemPmRibbonPage::CMemPmRibbonPage()
{
	m_uTemplateId = CMemPmRibbonPage::IDD;
	m_ppc = NULL;
}

CMemPmRibbonPage::~CMemPmRibbonPage()
{
}

void CMemPmRibbonPage::DoDataExchange(CDataExchange* pDX)
{
	CMemPmTabPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMemPmRibbonPage, CMemPmTabPage)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_COOL0, IDC_UNK_RIBBON6, OnClickRadioButtons)
	ON_BN_CLICKED(IDC_RIBBONS_32, OnBnClickedRibbons32)
	ON_BN_CLICKED(IDC_RIBBONS_25, OnBnClickedRibbons25)
	ON_BN_CLICKED(IDC_RIBBONS_0, OnBnClickedRibbons0)
END_MESSAGE_MAP()


// CMemPmRibbonPage 消息处理程序
void CMemPmRibbonPage::TransData(BOOL bToControls)
{
	m_ppc = GetPokemonCodec();
	if(m_ppc == NULL)
		return;

	if(bToControls)
	{
		SetRibbons();
	}
}

void CMemPmRibbonPage::OnChangeUILang(VOID)
{
}

void CMemPmRibbonPage::SetRibbons()
{
	CONST BYTE *	pRibbons = NULL;
	DWORD	dwPos, uID;
	DWORD	dwContestType, dwCount;
	DWORD	dwIndex;

	if(!m_ppc)
		return;

	pRibbons = m_ppc->GetRibbonPointer();
	if(!pRibbons)
		return;

	dwPos = 0;
	uID = IDC_COOL0;
	for(dwContestType = 0; dwContestType < 5; ++dwContestType)
	{
		dwCount = GetBitField(pRibbons, dwPos, 3);
		for(dwIndex = 0; dwIndex < 5; ++dwIndex)
		{
			if(IsDlgButtonChecked(uID + dwIndex) == BST_CHECKED)
			{
				if(dwIndex != dwCount)
				{
					CheckDlgButton(uID + dwIndex, BST_UNCHECKED);
				}
				break;
			}
		}
		CheckDlgButton(uID + dwCount, BST_CHECKED);
		dwPos += 3;
		uID += IDC_BEAUTIFUL0 - IDC_COOL0;
	}

	for(dwIndex = 0; dwIndex < 12; ++dwIndex)
	{
		CheckDlgButton(uID, GetBit(pRibbons, dwPos) ? BST_CHECKED : BST_UNCHECKED);
		++dwPos;
		++uID;
	}
}

void CMemPmRibbonPage::GetRibbons()
{
	BYTE *	pRibbons = NULL;
	DWORD	dwPos, uID;
	DWORD	dwContestType, dwCount;
	DWORD	dwIndex;

	if(!m_ppc)
		return;

	pRibbons = m_ppc->GetRibbonPointer();
	if(!pRibbons)
		return;

	dwPos = 0;
	uID = IDC_COOL0;
	for(dwContestType = 0; dwContestType < 5; ++dwContestType)
	{
		for(dwCount = 0; dwCount < 5; ++dwCount)
		{
			if(IsDlgButtonChecked(uID + dwCount) == BST_CHECKED)
			{
				SetBitField(pRibbons, dwPos, 3, dwCount);
				break;
			}
		}
		dwPos += 3;
		uID += IDC_BEAUTIFUL0 - IDC_COOL0;
	}

	for(dwIndex = 0; dwIndex < 12; ++dwIndex)
	{
		if(IsDlgButtonChecked(uID) == BST_CHECKED)
		{
			SetBit(pRibbons, dwPos);
		}
		else
		{
			ClrBit(pRibbons, dwPos);
		}
		++dwPos;
		++uID;
	}
}

void CMemPmRibbonPage::OnClickRadioButtons(UINT uID)
{
	GetRibbons();
	SetRibbons();
}

void CMemPmRibbonPage::OnBnClickedRibbons32()
{
	if(m_ppc)
	{
		m_ppc->SetMaxRibbon32();
		SetRibbons();
	}
}

void CMemPmRibbonPage::OnBnClickedRibbons25()
{
	if(m_ppc)
	{
		m_ppc->SetMaxRibbon();
		SetRibbons();
	}
}

void CMemPmRibbonPage::OnBnClickedRibbons0()
{
	if(m_ppc)
	{
		m_ppc->ClearRibbons();
		SetRibbons();
	}
}
