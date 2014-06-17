// BreedDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedDlg.h"
#include ".\breeddlg.h"

// CBreedDlg 对话框

IMPLEMENT_DYNAMIC(CBreedDlg, CBaseDialog)
CBreedDlg::CBreedDlg()
{
	m_uTemplateId = CBreedDlg::IDD;

	m_dwCurPage = -1;
	m_wCurSel = -1;
	m_wCurBreed = -1;
	m_dwCurOrder = pm_order_internal;

	m_pageType.m_szPageName = _T("类型");
	m_rgpPages[0] = &m_pageType;

	m_pageAbl.m_szPageName = _T("能力");
	m_rgpPages[1] = &m_pageAbl;

	m_pageSpec.m_szPageName = _T("特性");
	m_rgpPages[2] = &m_pageSpec;

	m_pageItem.m_szPageName = _T("道具");
	m_rgpPages[3] = &m_pageItem;

	m_pageLvlupLearn.m_szPageName = _T("升级技");
	m_rgpPages[4] = &m_pageLvlupLearn;

	m_pageDeriveLearn.m_szPageName = _T("遗传技");
	m_rgpPages[5] = &m_pageDeriveLearn;

	m_pageMachineLearn.m_szPageName = _T("技能机器");
	m_rgpPages[6] = &m_pageMachineLearn;
}

CBreedDlg::~CBreedDlg()
{
}

void CBreedDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PM_LIST, m_ctrlPmList);
	DDX_Control(pDX, IDC_TAB, m_ctrlTab);
	DDX_Control(pDX, IDC_CONFIRM, m_ctrlConfirm);
}


BEGIN_MESSAGE_MAP(CBreedDlg, CBaseDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONFIRM, OnBnClickedConfirm)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PM_LIST, OnLvnItemchangedPmList)
	ON_BN_CLICKED(IDC_FROM_EVO, OnBnClickedFromEvo)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_PM_LIST, OnLvnGetInfoTipPmList)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ORDER_INTERNAL, IDC_ORDER_HOUEN, OnChangeOrder)
END_MESSAGE_MAP()


// CBreedDlg 消息处理程序
void CBreedDlg::RefreshPmList(BOOL bRomChanged)
{
	BOOL	bResult;
	WORD	wIndex, wBreed;
	DWORD	dwIcon;
	WORD	wCount = m_ctrlPmList.GetItemCount();
	LVITEM	lvi;
	CString	szIndex, szText;
	LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };
	WORD *	rgwOrderList = NULL;	// i-th element: the breed id of the i-th item of pm list

	if(!rom.m_bOpened)
		return;

	///////////////////////////////////////////////////////////////////
	// build order list if rom is changed
	if(bRomChanged)
	{
		if(m_dwCurOrder != pm_order_internal)
		{
			rgwOrderList = new WORD[BREED_COUNT];
			if(!rgwOrderList)
				return;
		}

		switch(m_dwCurOrder)
		{
		case pm_order_kanto:
			for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
				rgwOrderList[rom.m_pPokedexKantoOrder[wBreed]] = wBreed;
			break;

		case pm_order_houen:
			for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
				rgwOrderList[rom.m_pPokedexHouenOrder[wBreed]] = wBreed;
			break;
		}
	}

	///////////////////////////////////////////////////////////////////
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iSubItem = 0;

	for(wIndex = 0; wIndex < BREED_COUNT; ++wIndex)
	{
		lvi.iItem = wIndex;

		if(rgwOrderList)
			wBreed = rgwOrderList[wIndex];
		else if(m_dwCurOrder == pm_order_internal)
			wBreed = wIndex;
		else
			wBreed = (WORD)(m_ctrlPmList.GetItemData(wIndex));

		dwIcon = rom.GetSmallIconCount(wBreed);
		if(dwIcon > 0)
			dwIcon = rom.GetSmallIconIndex(wBreed, rand() % dwIcon);
		lvi.iImage = dwIcon;

		szIndex.Format(szFmt33[cfg.dwCount], wIndex);
		szText = szIndex + cfg.pBreedNameList[wBreed].rgszText[cfg.dwLang];
		lvi.pszText = szText.GetBuffer();
		if(wBreed < wCount)
			bResult = m_ctrlPmList.SetItem(&lvi);
		else
			m_ctrlPmList.InsertItem(&lvi);
		szText.ReleaseBuffer();

		if(bRomChanged)
			m_ctrlPmList.SetItemData(wIndex, wBreed);
	}

	if(rgwOrderList)
		delete []rgwOrderList;

	if(m_wCurBreed < BREED_COUNT)
	{
		SetCurPm(m_wCurBreed);
	}
}

void CBreedDlg::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	DWORD	dwIndex;

	if(!rom.m_bOpened)
		return;

	///////////////////////////////////////////////////////////////////
	// refresh image list for pm list
	if(bRomChanged)
		m_ctrlPmList.SetImageList(rom.m_pSmallIconList, LVSIL_NORMAL);

	///////////////////////////////////////////////////////////////////
	// refresh pm list
	RefreshPmList(bRomChanged);

	if(bRomChanged)
	{
		m_ctrlConfirm.EnableWindow(!rom.m_bIsFileReadOnly);

		///////////////////////////////////////////////////////////////////
		if(m_wCurBreed >= BREED_COUNT)
			m_ctrlPmList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);
	}

	///////////////////////////////////////////////////////////////////
	// refresh each page
	for(dwIndex = 0; dwIndex < BREED_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->OnConfigChanged(bRomChanged, bUILangChanged);
	}

	if(m_dwCurPage >= BREED_PAGE_COUNT)
		ShowTabPage(0);
}

void CBreedDlg::ShowTabPage(DWORD dwIndex)
{
	if(m_dwCurPage != dwIndex)
	{
		if(m_dwCurPage < BREED_PAGE_COUNT)
		{
			m_rgpPages[m_dwCurPage]->ShowWindow(SW_HIDE);
		}

		m_dwCurPage = dwIndex;

		if(m_dwCurPage < BREED_PAGE_COUNT)
		{
			m_rgpPages[m_dwCurPage]->ShowWindow(SW_SHOW);
		}
	}
}

BOOL CBreedDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DWORD	dwIndex;
	RECT	rect;

	for(dwIndex = 0; dwIndex < BREED_PAGE_COUNT; ++dwIndex)
	{
		m_ctrlTab.InsertItem(dwIndex, m_rgpPages[dwIndex]->m_szPageName);
	}

	m_ctrlTab.GetWindowRect(&rect);
	m_ctrlTab.AdjustRect(FALSE, &rect);
	m_ctrlTab.ScreenToClient(&rect);

	for(dwIndex = 0; dwIndex < BREED_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->Create(m_rgpPages[dwIndex]->m_uTemplateId, &m_ctrlTab);
		m_rgpPages[dwIndex]->ShowWindow(SW_HIDE);
		m_rgpPages[dwIndex]->MoveWindow(&rect);
	}

	m_ctrlPmList.SetItemCount(BREED_COUNT);
	m_ctrlPmList.SetIconSpacing(SMALL_ICON_BIT_WIDTH2 + 28, SMALL_ICON_BIT_HEIGHT2 + 24);
	m_ctrlPmList.SetExtendedStyle(LVS_EX_BORDERSELECT | LVS_EX_INFOTIP);

	CheckDlgButton(IDC_ORDER_INTERNAL + m_dwCurOrder, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBreedDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	ShowTabPage(m_ctrlTab.GetCurSel());
}

void CBreedDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD	dwIndex;

	if(	!rom.ReadBreedList() ||
		!rom.ReadLvlupLearnList() ||
		!rom.ReadDeriveLearnList() ||
		!rom.ReadMachineLearnList() ||
		!rom.ReadMachineSkillList())
	{
		AfxMessageBox(IDS_ERR_ROM_READ);
	}
	else
	{
		for(dwIndex = 0; dwIndex < BREED_PAGE_COUNT; ++dwIndex)
		{
			m_rgpPages[dwIndex]->TransData(TRUE);
		}
	}
}

void CBreedDlg::SetCurPm(WORD wBreed)
{
	if(rom.m_bOpened && wBreed < BREED_COUNT)
	{
		switch(m_dwCurOrder)
		{
		case pm_order_internal:
		default:
			m_wCurSel = wBreed;
			break;

		case pm_order_kanto:
			m_wCurSel = rom.m_pPokedexKantoOrder[wBreed];
			break;

		case pm_order_houen:
			m_wCurSel = rom.m_pPokedexHouenOrder[wBreed];
			break;
		}

		// m_wCurBreed = wBreed;
		m_ctrlPmList.SetItemState(m_wCurSel, -1, LVIS_SELECTED | LVIS_FOCUSED);
		m_ctrlPmList.EnsureVisible(m_wCurSel, FALSE);
	}
}

void CBreedDlg::OnBnClickedConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD	dwIndex;

	for(dwIndex = 0; dwIndex < BREED_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->TransData(FALSE);
	}

	if(	!rom.SaveBreedList() ||
		!rom.SaveLvlupLearnList() ||
		!rom.SaveDeriveLearnList() ||
		!rom.SaveMachineLearnList())
	{
		AfxMessageBox(IDS_ERR_ROM_WRITE);
	}
}

void CBreedDlg::OnLvnItemchangedPmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	DWORD	dwIndex;

	if(pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED))
	{
		if(m_wCurSel < BREED_COUNT)
		{
			for(dwIndex = 0; dwIndex < BREED_PAGE_COUNT; ++dwIndex)
			{
				m_rgpPages[dwIndex]->TransData(FALSE);
			}
		}

		m_wCurSel = pNMLV->iItem;

		if(m_wCurSel < BREED_COUNT)
		{
			m_wCurBreed = (WORD)(m_ctrlPmList.GetItemData(m_wCurSel));
			for(dwIndex = 0; dwIndex < BREED_PAGE_COUNT; ++dwIndex)
			{
				m_rgpPages[dwIndex]->TransData(TRUE);
			}
		}
		else
		{
			m_wCurBreed = -1;
		}
	}
}

void CBreedDlg::OnBnClickedFromEvo()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD	wBreed = theApp.m_dlgEvo.m_wCurBreed;
	SetCurPm(wBreed);
}

void CBreedDlg::OnLvnGetInfoTipPmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	WORD	wBreed;
	WORD	wBreed387, wIndexKanto, wIndexHouen;
	WORD	wPage, wRow, wColumn;
	WORD	wPageKanto, wRowKanto, wColumnKanto;
	WORD	wPageHouen, wRowHouen, wColumnHouen;

	if(rom.m_bOpened &&
		pGetInfoTip->cchTextMax >= 100 &&
		pGetInfoTip->dwFlags == LVGIT_UNFOLDED &&
		pGetInfoTip->iItem <= BREED_COUNT)
	{
		wBreed = (WORD)(m_ctrlPmList.GetItemData(pGetInfoTip->iItem));
		wBreed387 = ConvertBreed(wBreed, FALSE);
		GetPositionInBox(wBreed387, wPage, wRow, wColumn);

		wIndexKanto = rom.m_pPokedexKantoOrder[wBreed];
		GetPositionInBox(wIndexKanto - 1, wPageKanto, wRowKanto, wColumnKanto);

		wIndexHouen = rom.m_pPokedexHouenOrder[wBreed];
		GetPositionInBox(wIndexHouen - 1, wPageHouen, wRowHouen, wColumnHouen);

		wsprintf(
			pGetInfoTip->pszText,
			_T("%3hu|%2hu-%hu-%hu(K)\n")
			_T("%3hu|%2hu-%hu-%hu(H)\n")
			_T("%3hu|%03hXh\n")
			_T("%hu-%hu-%hu"),
			wIndexKanto, wPageKanto, wRowKanto, wColumnKanto,
			wIndexHouen, wPageHouen, wRowHouen, wColumnHouen,
			wBreed, wBreed, wPage, wRow, wColumn);
	}
}

INT CALLBACK CBreedDlg::SortPokemons(LPARAM lParam1, LPARAM lParam2, LPARAM lSortParam)
{
	INT		iResult = 0;

	if(rom.m_bOpened)
	{
		switch(lSortParam)
		{
		case pm_order_internal:
			iResult = INT(lParam1 - lParam2);
			break;

		case pm_order_kanto:
			iResult = INT(rom.m_pPokedexKantoOrder[lParam1] - rom.m_pPokedexKantoOrder[lParam2]);
			break;

		case pm_order_houen:
			iResult = INT(rom.m_pPokedexHouenOrder[lParam1] - rom.m_pPokedexHouenOrder[lParam2]);
			break;
		}
	}

	return iResult;
}

void CBreedDlg::OnChangeOrder(UINT uID)
{
	if(!rom.m_bOpened || uID < IDC_ORDER_INTERNAL || uID > IDC_ORDER_HOUEN)
		return;

	if(m_dwCurOrder != uID - IDC_ORDER_INTERNAL)
	{
		m_dwCurOrder = uID - IDC_ORDER_INTERNAL;
		//m_ctrlPmList.SortItems(SortPokemons, (LPARAM)(m_dwCurOrder));
		RefreshPmList(TRUE);
	}
}
