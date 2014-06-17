// MemPmTabPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemPmPage.h"


// CMemPmTabPage 对话框

IMPLEMENT_DYNAMIC(CMemPmTabPage, CDialog)
CMemPmTabPage::CMemPmTabPage()
{
}

CMemPmTabPage::~CMemPmTabPage()
{
}

void CMemPmTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMemPmTabPage, CDialog)
END_MESSAGE_MAP()


// CMemPmTabPage 消息处理程序

void CMemPmTabPage::OnOK()
{
}

void CMemPmTabPage::OnCancel()
{
}

CPokemonCodec * CMemPmTabPage::GetPokemonCodec(VOID)
{
	CPokemonCodec *ppc = NULL;
	CMemPmPage *pParent = reinterpret_cast<CMemPmPage *>(GetParent()->GetParent());

	if(pParent &&
		g_MemRom.m_bOpened &&
		g_MemHack.m_pActivePokeCodec &&
		pParent->m_dwCurPm < ACTIVE_POKEMON_COUNT)
	{
		ppc = &(g_MemHack.m_pActivePokeCodec[pParent->m_dwCurPm]); 
	}

	return ppc;
}

PokemonStructActive * CMemPmTabPage::GetPokemonStructActive(VOID)
{
	PokemonStructActive *ppsa = NULL;
	CMemPmPage *pParent = reinterpret_cast<CMemPmPage *>(GetParent()->GetParent());

	if(pParent &&
		g_MemRom.m_bOpened &&
		g_MemHack.m_pActivePokemon &&
		pParent->m_dwCurPm < ACTIVE_POKEMON_COUNT)
	{
		ppsa = &(g_MemHack.m_pActivePokemon[pParent->m_dwCurPm]); 
	}

	return ppsa;
}
