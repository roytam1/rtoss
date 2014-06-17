// BreedTabPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedTabPage.h"


// CBreedTabPage 对话框

IMPLEMENT_DYNAMIC(CBreedTabPage, CDialog)
CBreedTabPage::CBreedTabPage()
{
	m_pBreed = NULL;
	m_pBreedAbilities = NULL;
}

CBreedTabPage::~CBreedTabPage()
{
}

void CBreedTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBreedTabPage, CDialog)
END_MESSAGE_MAP()


// CBreedTabPage 消息处理程序
void CBreedTabPage::GetCurrentBreed()
{
	if(!rom.m_bOpened || theApp.m_dlgBreed.m_wCurBreed >= BREED_COUNT)
	{
		m_pBreed = NULL;
		m_pBreedAbilities = NULL;
	}
	else
	{
		m_pBreed = rom.GetBreedListEntry(theApp.m_dlgBreed.m_wCurBreed);
		if(theApp.m_dlgBreed.m_wCurBreed == 410)	// Special for ディオキシス
		{
			m_pBreedAbilities = rom.GetDeoxysBreedAbilities();
		}
		else
		{
			m_pBreedAbilities = NULL;
		}
	}
}

void CBreedTabPage::OnCancel()
{
}
