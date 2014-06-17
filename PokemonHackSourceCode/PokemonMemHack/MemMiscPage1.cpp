// MemMiscPage1.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemMiscPage1.h"

// CMemMiscPage1 对话框

IMPLEMENT_DYNAMIC(CMemMiscPage1, CPropertyPage)
CMemMiscPage1::CMemMiscPage1()
	: CPropertyPage(CMemMiscPage1::IDD)
{
	m_bCreated = FALSE;
}

CMemMiscPage1::~CMemMiscPage1()
{
}

void CMemMiscPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MISC_ITEM_LIST, m_ctrlItemList);
}


BEGIN_MESSAGE_MAP(CMemMiscPage1, CPropertyPage)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_ADD_ITEM, OnBnClickedAddItem)
	ON_BN_CLICKED(IDC_INC_ITEMS, OnBnClickedIncItems)
	ON_BN_CLICKED(IDC_GIVE_POKEBALLS, OnBnClickedGivePokeballs)
	ON_BN_CLICKED(IDC_GIVE_MACHINES, OnBnClickedGiveMachines)
	ON_BN_CLICKED(IDC_GIVE_BERRIES, OnBnClickedGiveBerries)
	ON_BN_CLICKED(IDC_GIVE_POKEBLOCKS, OnBnClickedGivePokeblock)
	ON_BN_CLICKED(IDC_GIVE_DECORATES, OnBnClickedGiveDecorates)
	ON_CBN_SELCHANGE(IDC_MISC_ITEM_LIST, SetItemDesc)
	ON_BN_CLICKED(IDC_DROP_LAST_KEYITEM, OnBnClickedDropLastKeyitem)
	ON_BN_CLICKED(IDC_ENABLE_ALL_TICKETS, OnBnClickedEnableAllTickets)
	ON_BN_CLICKED(IDC_ADD_ITEM_BP50, OnBnClickedAddItemBp50)
	ON_BN_CLICKED(IDC_ADD_ITEM_BP60, OnBnClickedAddItemBp60)
	ON_BN_CLICKED(IDC_MAX_VIEW_IN_BP, OnBnClickedMaxViewInBp)
END_MESSAGE_MAP()


// CMemMiscPage1 消息处理程序
void CMemMiscPage1::OnChangeUILang(VOID)
{
	if(!m_bCreated)
		return;

	DWORD	dwIndex, dwCurSel;
	CString	szText;
	LPCTSTR	szFmt3[3] = { _T(""), _T("%-3lu: "), _T("%03lX: ") };

	////////////////////////////////////////////////
	dwCurSel = m_ctrlItemList.GetCurSel();
	if(dwCurSel == LB_ERR)
		dwCurSel = 0;
	m_ctrlItemList.ResetContent();
	for(dwIndex = 0; dwIndex < ITEM_COUNT; ++dwIndex)
	{
		szText.Format(szFmt3[cfg.dwCount], dwIndex);
		szText += cfg.pItemNameList[dwIndex].rgszText[cfg.dwLang];
		m_ctrlItemList.AddString(szText);
	}
	m_ctrlItemList.SetCurSel(dwCurSel);

	SetItemDesc();
}

BOOL CMemMiscPage1::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	OnChangeUILang();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int CMemMiscPage1::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bCreated = TRUE;

	return 0;
}

void CMemMiscPage1::SetItemDesc()
{
	WORD	wItem;
	wItem = m_ctrlItemList.GetCurSel();
	if(wItem < ITEM_COUNT)
		SetDlgItemText(IDC_MISC_ITEM_DESC, cfg.pItemDescList[wItem].rgszText[cfg.dwLang]);
}

void CMemMiscPage1::OnBnClickedAddItem()
{
	WORD	wQuantity;
	CString	szText;

	GetDlgItemText(IDC_MISC_ITEM_QUANTITY, szText);
	wQuantity = (WORD)(min(_tcstoul(szText, 0, 0), 0xFFFF));
	if(wQuantity == 0)
	{
		wQuantity = 1;
		SetDlgItemText(IDC_MISC_ITEM_QUANTITY, _T("1"));
	}
	if(!g_MemHack.AddItem(m_ctrlItemList.GetCurSel(), wQuantity))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedAddItemBp50()
{
	BYTE	bQuantity;
	CString	szText;

	GetDlgItemText(IDC_MISC_ITEM_QUANTITY, szText);
	bQuantity = (BYTE)(min(_tcstoul(szText, 0, 0), 99));
	if(bQuantity == 0)
	{
		bQuantity = 1;
		SetDlgItemText(IDC_MISC_ITEM_QUANTITY, _T("1"));
	}
	if(!g_MemHack.AddItemToBattlePyramid50(m_ctrlItemList.GetCurSel(), bQuantity))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedAddItemBp60()
{
	BYTE	bQuantity;
	CString	szText;

	GetDlgItemText(IDC_MISC_ITEM_QUANTITY, szText);
	bQuantity = (BYTE)(min(_tcstoul(szText, 0, 0), 99));
	if(bQuantity == 0)
	{
		bQuantity = 1;
		SetDlgItemText(IDC_MISC_ITEM_QUANTITY, _T("1"));
	}
	if(!g_MemHack.AddItemToBattlePyramid60(m_ctrlItemList.GetCurSel(), bQuantity))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedMaxViewInBp()
{
	if(!g_MemHack.SetBattlePyramidViewRadius(0x78))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedIncItems()
{
	if(!g_MemHack.IncreaseNormalItems(50))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedGivePokeballs()
{
	if(!g_MemHack.GiveAllPokeball(50))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedGiveMachines()
{
	if(!g_MemHack.GiveAllSkillMachines(50))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedGiveBerries()
{
	if(!g_MemHack.GiveAllBerries(500))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedGivePokeblock()
{
	if(!g_MemHack.GiveAllPokeblocks())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedGiveDecorates()
{
	if(!g_MemHack.GiveAllDecorates())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedDropLastKeyitem()
{
	if(!g_MemHack.DropLastKeyItem())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage1::OnBnClickedEnableAllTickets()
{
	if(!g_MemHack.EnableAllTickets())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}
