// EncDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "EncDlg.h"

// CEncDlg 对话框

IMPLEMENT_DYNAMIC(CEncDlg, CBaseDialog)
CEncDlg::CEncDlg()
{
	m_uTemplateId = CEncDlg::IDD;
	m_dwSortOrder = 0;

	m_dwCurSel = -1;
	m_dwCurEnc = -1;
	m_dwCurPage = -1;
	m_bFindEditChanged = FALSE;

	m_pageLand.m_szPageName = _T("陆上");
	m_rgpPages[0] = &m_pageLand;

	m_pageWater.m_szPageName = _T("水上");
	m_rgpPages[1] = &m_pageWater;

	m_pageStone.m_szPageName = _T("碎岩");
	m_rgpPages[2] = &m_pageStone;

	m_pageFish.m_szPageName = _T("垂钓");
	m_rgpPages[3] = &m_pageFish;
}

CEncDlg::~CEncDlg()
{
}

void CEncDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENC_LIST, m_ctrlEncList);
	DDX_Control(pDX, IDC_TAB, m_ctrlTab);
	DDX_Control(pDX, IDC_CONFIRM, m_ctrlConfirm);
}


BEGIN_MESSAGE_MAP(CEncDlg, CBaseDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ENC_LIST, OnLvnItemchangedEncList)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONFIRM, OnBnClickedConfirm)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTcnSelchangeTab)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickEncList)
	ON_BN_CLICKED(IDC_ENC_FIND, OnBnClickedEncFind)
	ON_EN_CHANGE(IDC_ENC_FIND_EDIT, OnEnChangeEncFindEdit)
END_MESSAGE_MAP()


// CEncDlg 消息处理程序
INT CALLBACK CEncDlg::EncListSortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	INT		iResult = 0;
	WORD	wAreaId1, wAreaId2;

	if(rom.m_bOpened)
	{
		wAreaId1 = rom.m_pEncAddressList[lParam1].wAreaId;
		wAreaId2 = rom.m_pEncAddressList[lParam2].wAreaId;

		switch(lParamSort)
		{
		case 0:
			iResult = (wAreaId1 & 0xFF) - (wAreaId2 & 0xFF);
			if(iResult == 0)
			{
				iResult = (wAreaId1 & 0xFF00) - (wAreaId2 & 0xFF00);
			}
			break;

		case 1:
			iResult = (INT)(lParam1 - lParam2);
			break;
		}
	}

	return iResult;
}

void CEncDlg::RefreshEncList()
{
	DWORD	dwIndex, dwEncCount, dwListCount;
	LVITEM	lvi;
	WORD	wAreaId;
	StringEntry *pStringEntry = NULL;
	CString	szText;
	POSITION	pos;

	if(!rom.m_bOpened)
		return;

	dwEncCount = rom.GetEncCount();
	dwListCount = m_ctrlEncList.GetItemCount();

	lvi.mask = LVIF_TEXT;

	for(dwIndex = 0; dwIndex < dwEncCount; ++dwIndex)
	{
		lvi.iItem = dwIndex;

		lvi.iSubItem = 0;
		wAreaId = rom.m_pEncAddressList[dwIndex].wAreaId;
		szText.Format(_T("%04hX"), wAreaId);

		lvi.pszText = szText.GetBuffer();
		if(dwIndex < dwListCount)
			m_ctrlEncList.SetItem(&lvi);
		else
			m_ctrlEncList.InsertItem(&lvi);
		szText.ReleaseBuffer();

		lvi.iSubItem = 1;
		pStringEntry = GetEncNameEntry(wAreaId);
		szText = pStringEntry ? pStringEntry->rgszText[cfg.dwLang] : _T("???");
		lvi.pszText = szText.GetBuffer();
		m_ctrlEncList.SetItem(&lvi);
		szText.ReleaseBuffer();

		m_ctrlEncList.SetItemData(dwIndex, dwIndex);
	}

	for(dwIndex = dwEncCount; dwIndex < dwListCount; ++dwIndex)
	{
		m_ctrlEncList.DeleteItem(dwEncCount);
	}

	m_ctrlEncList.SortItems(EncListSortFunc, m_dwSortOrder);

	pos = m_ctrlEncList.GetFirstSelectedItemPosition();
	if(pos)
	{
		m_dwCurSel = m_ctrlEncList.GetNextSelectedItem(pos);
		m_dwCurEnc = (DWORD)(m_ctrlEncList.GetItemData(m_dwCurSel));
		m_ctrlEncList.EnsureVisible(m_dwCurSel, FALSE);
	}
	else
	{
		m_dwCurSel = -1;
		m_dwCurEnc = -1;
	}
}

void CEncDlg::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	static DWORD dwPreRomVersion = -1;
	BOOL	bChangeSortOrder = TRUE;
	DWORD	dwRomVersion;
	DWORD	dwIndex;
	DWORD	dwListCount;

	if(!rom.m_bOpened)
		return;

	/////////////////////////////////////////////
	// change sort order of enc list if necessary
	if(bRomChanged)
	{
		m_ctrlConfirm.EnableWindow(!rom.m_bIsFileReadOnly);

		dwRomVersion = rom.GetRomVersion();

		// if the new rom is the same as the previous rom, then there's no need to sort enc list
		if(dwRomVersion >= CPokemonRom::rom_ruby_jp && dwRomVersion <= CPokemonRom::rom_sapphire_en &&
			dwPreRomVersion >= CPokemonRom::rom_ruby_jp && dwPreRomVersion <= CPokemonRom::rom_sapphire_en)
		{
			bChangeSortOrder = FALSE;
		}
		else if(dwRomVersion >= CPokemonRom::rom_fire_jp && dwRomVersion <= CPokemonRom::rom_leaf_en &&
			dwPreRomVersion >= CPokemonRom::rom_fire_jp && dwPreRomVersion <= CPokemonRom::rom_leaf_en)
		{
			bChangeSortOrder = FALSE;
		}
		else if(dwRomVersion == CPokemonRom::rom_emerald_jp &&
				dwPreRomVersion == CPokemonRom::rom_emerald_jp)
		{
			bChangeSortOrder = FALSE;
		}

		dwPreRomVersion = dwRomVersion;

		/////////////////////////////////////////
		// check for sanity
		dwListCount = m_ctrlEncList.GetItemCount();
		if(!bChangeSortOrder)
		{
			if(m_dwCurEnc >= dwListCount)
			{
				// this is no necessary, but good for preventing errors
				bChangeSortOrder = TRUE;
			}
		}

		/////////////////////////////////////////
		// change sort order: use the default order
		if(bChangeSortOrder)
			m_dwSortOrder = 0;
	}

	/////////////////////////////////////////////
	// refresh enc list
	RefreshEncList();

	if(bRomChanged)
	{
		if(!bChangeSortOrder)
		{
			// this is no necessary, but good for preventing errors
			m_dwCurEnc = (DWORD)(m_ctrlEncList.GetItemData(m_dwCurSel));
		}
		else
		{
			// unselect previous selection (this prevents getting data from controls)
			if(m_dwCurSel < dwListCount)
				m_ctrlEncList.SetItemState(m_dwCurSel, 0, LVIS_SELECTED | LVIS_FOCUSED);

			// select the first item in enc list
			m_dwCurSel = -1;
			m_dwCurEnc = -1;
			m_ctrlEncList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);

			m_ctrlEncList.EnsureVisible(m_dwCurSel, FALSE);
		}
	}

	/////////////////////////////////////////////
	// refresh each page
	for(dwIndex = 0; dwIndex < ENC_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->OnConfigChanged(bRomChanged, bUILangChanged);
	}

	/////////////////////////////////////////////
	// show the 1st page if no page is showing
	if(m_dwCurPage >= ENC_PAGE_COUNT)
		ShowTabPage(0);
}

BOOL CEncDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DWORD	dwIndex;
	RECT	rect;

	for(dwIndex = 0; dwIndex < ENC_PAGE_COUNT; ++dwIndex)
	{
		m_ctrlTab.InsertItem(dwIndex, m_rgpPages[dwIndex]->m_szPageName);
	}

	m_ctrlTab.GetWindowRect(&rect);
	m_ctrlTab.AdjustRect(FALSE, &rect);
	m_ctrlTab.ScreenToClient(&rect);

	for(dwIndex = 0; dwIndex < ENC_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->Create(m_rgpPages[dwIndex]->m_uTemplateId, &m_ctrlTab);
		m_rgpPages[dwIndex]->ShowWindow(SW_HIDE);
		m_rgpPages[dwIndex]->MoveWindow(&rect);
	}

	m_ctrlEncList.InsertColumn(0, _T("编号"), LVCFMT_LEFT, 50, 0);
	m_ctrlEncList.InsertColumn(1, _T("地区名称"), LVCFMT_LEFT, 200, 1);
	m_ctrlEncList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | LVS_EX_LABELTIP);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEncDlg::ShowTabPage(DWORD dwIndex)
{
	DWORD *	pdwAddress = NULL;

	if(!rom.m_bOpened)
		return;

	pdwAddress = &(rom.m_pEncAddressList[m_dwCurEnc].dwLandEncAddress);

	if(m_dwCurPage < ENC_PAGE_COUNT &&
		(m_dwCurPage != dwIndex || pdwAddress[m_dwCurPage] == 0))
	{
		m_rgpPages[m_dwCurPage]->ShowWindow(SW_HIDE);
	}

	m_dwCurPage = dwIndex;

	if(m_dwCurPage < ENC_PAGE_COUNT)
	{
		if(pdwAddress[m_dwCurPage] == 0)
			m_rgpPages[m_dwCurPage]->ShowWindow(SW_HIDE);
		else
			m_rgpPages[m_dwCurPage]->ShowWindow(SW_SHOW);
	}
}

void CEncDlg::TransData(BOOL bToControls)
{
	DWORD	dwIndex;

	if(!rom.m_bOpened)
		return;

	if(bToControls)
	{
		SetTabPageNames();
	}

	for(dwIndex = 0; dwIndex < ENC_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->TransData(bToControls);
	}

	if(bToControls)
	{
		ShowTabPage(m_dwCurPage);
	}
}

void CEncDlg::SetTabPageNames()
{
	DWORD	dwIndex;
	DWORD *	pdwAddress = NULL;
	TCITEM	tci;

	if(!rom.m_bOpened)
		return;

	tci.mask = TCIF_TEXT;

	pdwAddress = &(rom.m_pEncAddressList[m_dwCurEnc].dwLandEncAddress);
	for(dwIndex = 0; dwIndex < ENC_PAGE_COUNT; ++dwIndex)
	{
		if(pdwAddress[dwIndex])
		{
			tci.pszText = const_cast<LPTSTR>(m_rgpPages[dwIndex]->m_szPageName);
		}
		else
		{
			tci.pszText = _T("没有");
		}

		m_ctrlTab.SetItem(dwIndex, &tci);
	}

	if(m_dwCurPage < ENC_PAGE_COUNT)
	{
		m_rgpPages[m_dwCurPage]->RedrawWindow();
	}
}

void CEncDlg::OnLvnItemchangedEncList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(!rom.m_bOpened)
		return;

	DWORD	dwEncCount = rom.GetEncCount();

	if((pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED)) &&
		pNMLV->iItem != m_dwCurSel)
	{
		if(m_dwCurSel < dwEncCount)
		{
			TransData(FALSE);
		}

		m_dwCurSel = pNMLV->iItem;

		if(m_dwCurSel < dwEncCount)
		{
			m_dwCurEnc = (DWORD)(m_ctrlEncList.GetItemData(m_dwCurSel));
			TransData(TRUE);
		}
		else
		{
			m_dwCurEnc = -1;
		}
	}
}

void CEncDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!rom.ReadEncList())
	{
		AfxMessageBox(IDS_ERR_ROM_READ);
	}
	else
	{
		TransData(TRUE);
	}
}

void CEncDlg::OnBnClickedConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	TransData(FALSE);

	if(!rom.SaveEncList())
	{
		AfxMessageBox(IDS_ERR_ROM_WRITE);
	}
}

void CEncDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	ShowTabPage(m_ctrlTab.GetCurSel());
}

void CEncDlg::OnHdnItemclickEncList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(!rom.m_bOpened ||
		phdr->iItem == 1)	// 地区名称
		return;

	m_dwSortOrder = (m_dwSortOrder + 1) & 1;
	m_ctrlEncList.SortItems(EncListSortFunc, m_dwSortOrder);

	POSITION	pos;
	DWORD		dwCurSel;

	pos = m_ctrlEncList.GetFirstSelectedItemPosition();
	if(pos)
	{
		dwCurSel = m_ctrlEncList.GetNextSelectedItem(pos);
		m_ctrlEncList.EnsureVisible(dwCurSel, FALSE);
		m_dwCurSel = dwCurSel;
		m_dwCurEnc = (DWORD)(m_ctrlEncList.GetItemData(m_dwCurSel));
	}
	else
	{
		m_dwCurSel = -1;
		m_dwCurEnc = -1;
	}
}

BOOL CEncDlg::FindBreedInEnc(WORD wBreed, DWORD dwEncIndex, DWORD &dwEncType, DWORD &dwEncEntryIndex)
{
	BOOL	bFound = FALSE;
	DWORD	dwEncEntryCount;
	EncEntry *	pEncEntryList = NULL;

	if(rom.m_bOpened && wBreed > 0 && wBreed < BREED_COUNT)
	{
		for(; dwEncType < ENC_PAGE_COUNT; ++dwEncType)
		{
			pEncEntryList = rom.GetEncList(dwEncIndex, dwEncType);
			if(pEncEntryList)
			{
				dwEncEntryCount = rom.GetEncListEntryCount(dwEncType);
				for(; dwEncEntryIndex < dwEncEntryCount; ++dwEncEntryIndex)
				{
					if(pEncEntryList[dwEncEntryIndex].wBreed == wBreed)
					{
						bFound = TRUE;
						break;
					}
				}

			}

			if(bFound)
				break;
			else
				dwEncEntryIndex = 0;
		}
	}

	return bFound;
}

void CEncDlg::OnEnChangeEncFindEdit()
{
	// TODO:  在此添加控件通知处理程序代码
	m_bFindEditChanged = TRUE;
}

void CEncDlg::OnBnClickedEncFind()
{
	// TODO: 在此添加控件通知处理程序代码
	static WORD wBreed = 0;
	DWORD	dwItemCount, dwItemIndex;
	DWORD	dwEncIndex, dwPageIndex, dwEncEntryCount, dwEncEntryIndex;

	if(m_bFindEditChanged)
	{
		CString	szText;

		GetDlgItemText(IDC_ENC_FIND_EDIT, szText);
		szText.Trim();
		wBreed = TextToBreed(szText);

		m_bFindEditChanged = FALSE;
	}

	dwItemCount = rom.GetEncCount();
	dwPageIndex = m_dwCurPage;

	dwEncEntryCount = rom.GetEncListEntryCount(dwPageIndex);
	dwEncEntryIndex = m_rgpPages[dwPageIndex]->m_dwFocusedEntry + 1;
	if(dwEncEntryIndex >= dwEncEntryCount)
	{
		++dwPageIndex;
		dwEncEntryIndex = 0;
	}

	for(dwItemIndex = m_dwCurSel; dwItemIndex < dwItemCount; ++dwItemIndex)
	{
		dwEncIndex = (DWORD)(m_ctrlEncList.GetItemData(dwItemIndex));
		if(FindBreedInEnc(wBreed, dwEncIndex, dwPageIndex, dwEncEntryIndex))
		{
			m_ctrlEncList.SetItemState(dwItemIndex, -1, LVIS_SELECTED | LVIS_FOCUSED);
			m_ctrlEncList.EnsureVisible(dwItemIndex, FALSE);
			m_ctrlTab.SetCurSel(dwPageIndex);
			ShowTabPage(dwPageIndex);
			m_rgpPages[dwPageIndex]->SetFocusedEntry(dwEncEntryIndex);
			break;
		}
		dwPageIndex = 0;
		dwEncEntryIndex = 0;
	}
	if(dwItemIndex >= dwItemCount)
		MessageBox(_T("搜索完毕！"), _T("查找"), MB_OK | MB_ICONINFORMATION);
}
