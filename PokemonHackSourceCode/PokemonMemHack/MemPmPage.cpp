// MemPmPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"

// CMemPmPage 对话框

IMPLEMENT_DYNAMIC(CMemPmPage, CPropertyPage)
CMemPmPage::CMemPmPage()
	: CPropertyPage(CMemPmPage::IDD)
{
	m_dwCurPm = -1;
	m_dwCurTabPage = -1;

	m_pageBasic.m_szPageName = _T("基本");
	m_rgpPages[0] = &m_pageBasic;

	m_pageAbility.m_szPageName = _T("能力");
	m_rgpPages[1] = &m_pageAbility;

	m_pageSkill.m_szPageName = _T("技能");
	m_rgpPages[2] = &m_pageSkill;

	m_pageRibbon.m_szPageName = _T("缎带");
	m_rgpPages[3] = &m_pageRibbon;

	m_pageMisc.m_szPageName = _T("其它");
	m_rgpPages[4] = &m_pageMisc;
}

CMemPmPage::~CMemPmPage()
{
}

void CMemPmPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAB, m_ctrlTab);
	DDX_Control(pDX, IDC_PM_LIST, m_ctrlPmList);
	DDX_Control(pDX, IDC_CONFIRM, m_ctrlConfirm);
}


BEGIN_MESSAGE_MAP(CMemPmPage, CPropertyPage)
	ON_BN_CLICKED(IDC_LANG_CN, OnBnClickedLangCn)
	ON_BN_CLICKED(IDC_LANG_JP, OnBnClickedLangJp)
	ON_BN_CLICKED(IDC_COUNT_DEC, OnBnClickedCountDec)
	ON_BN_CLICKED(IDC_COUNT_HEX, OnBnClickedCountHex)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONFIRM, OnBnClickedConfirm)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PM_LIST, OnLvnItemchangedPmList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTcnSelchangeTab)
	ON_BN_CLICKED(IDC_LANG_EN, OnBnClickedLangEn)
	ON_BN_CLICKED(IDC_COUNT_NONE, OnBnClickedCountNone)
	ON_BN_CLICKED(IDC_MOVE_UP, OnBnClickedMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnBnClickedMoveDown)
END_MESSAGE_MAP()


// CMemPmPage 消息处理程序
void CMemPmPage::ShowTabPage(DWORD dwPageIndex)
{
	if(m_dwCurTabPage != dwPageIndex)
	{
		if(m_dwCurTabPage < MEM_PM_PAGE_COUNT)
			m_rgpPages[m_dwCurTabPage]->ShowWindow(SW_HIDE);
		m_dwCurTabPage = dwPageIndex;
		if(m_dwCurTabPage < MEM_PM_PAGE_COUNT)
			m_rgpPages[m_dwCurTabPage]->ShowWindow(SW_SHOW);
	}
}

BOOL CMemPmPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	DWORD	dwIndex;
	CRect	rect;
	CString	szText;

	for(dwIndex = 0; dwIndex < MEM_PM_PAGE_COUNT; ++dwIndex)
	{
		m_ctrlTab.InsertItem(dwIndex, m_rgpPages[dwIndex]->m_szPageName);
	}

	m_ctrlTab.GetWindowRect(&rect);
	m_ctrlTab.AdjustRect(FALSE, &rect);
	m_ctrlTab.ScreenToClient(&rect);

	for(dwIndex = 0; dwIndex < MEM_PM_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->Create(m_rgpPages[dwIndex]->m_uTemplateId, &m_ctrlTab);
		m_rgpPages[dwIndex]->ShowWindow(SW_HIDE);
		m_rgpPages[dwIndex]->MoveWindow(&rect);
	}

	m_ctrlPmList.SetIconSpacing(SMALL_ICON_BIT_WIDTH + 10, SMALL_ICON_BIT_HEIGHT + 22);
	m_ctrlPmList.SetExtendedStyle(LVS_EX_BORDERSELECT);
	m_ctrlPmList.SetItemCount(ACTIVE_POKEMON_COUNT);

	CheckDlgButton(IDC_LANG_CN + cfg.dwLang, BST_CHECKED);

	CheckDlgButton(IDC_COUNT_NONE + cfg.dwCount, BST_CHECKED);

	OnChangeUILang();

	ShowTabPage(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMemPmPage::SetPmList(BOOL bSelectionOnly)
{
	DWORD	dwIndex, dwCount;
	WORD	wBreed;
	CString	szText;
	LVITEM	lvi;
/*
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT;

	dwCount = m_ctrlPmList.GetItemCount();
	for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
	{
		if(g_MemHack.m_pActivePokeCodec == NULL)
		{
			wBreed = 0;
		}
		else
		{
			wBreed = g_MemHack.m_pActivePokeCodec[dwIndex].GetBreed();
		}

		lvi.iItem = dwIndex;
		szText.Format(_T("%lu:"), dwIndex + 1);
		if(wBreed < BREED_COUNT)
			szText += cfg.pBreedNameList[wBreed].rgszText[cfg.dwLang];
		else
			szText += _T("未知");
		lvi.pszText = szText.GetBuffer();
		if(dwIndex < dwCount)
			m_ctrlPmList.SetItem(&lvi);
		else
			m_ctrlPmList.InsertItem(&lvi);
		szText.ReleaseBuffer();
	}
*/
	if(!g_MemRom.m_bOpened)
		return;

	lvi.iSubItem = 0;
	if(!bSelectionOnly)
	{
		lvi.mask = LVIF_IMAGE | LVIF_TEXT;
		m_ctrlPmList.SetImageList(g_MemRom.m_pTinyIconList, LVSIL_NORMAL);
		dwCount = m_ctrlPmList.GetItemCount();
		for(dwIndex = 0; dwIndex < ACTIVE_POKEMON_COUNT; ++dwIndex)
		{
			if(g_MemHack.m_pActivePokeCodec == NULL)
			{
				wBreed = 0;
			}
			else
			{
				wBreed = g_MemHack.m_pActivePokeCodec[dwIndex].GetBreed();
			}
			if(wBreed == 0 || wBreed >= BREED_COUNT)
				wBreed = 0x114;

			lvi.iItem = dwIndex;
			lvi.iImage = g_MemRom.GetSmallIconIndex(wBreed, rand() % g_MemRom.GetSmallIconCount(wBreed));

			szText.Format(_T("%lu"), dwIndex + 1);
			lvi.pszText = szText.GetBuffer();
			if(dwIndex < dwCount)
				m_ctrlPmList.SetItem(&lvi);
			else
				m_ctrlPmList.InsertItem(&lvi);
			szText.ReleaseBuffer();
		}
	}
	else if(m_dwCurPm < ACTIVE_POKEMON_COUNT)
	{
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = m_dwCurPm;
		wBreed = g_MemHack.m_pActivePokeCodec[m_dwCurPm].GetBreed();
		if(wBreed == 0 || wBreed >= BREED_COUNT)
			wBreed = 0x114;
		lvi.iImage = g_MemRom.GetSmallIconIndex(wBreed, rand() % g_MemRom.GetSmallIconCount(wBreed));

		m_ctrlPmList.SetItem(&lvi);
	}

	if(m_dwCurPm >= ACTIVE_POKEMON_COUNT)
		m_ctrlPmList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);

	if(bSelectionOnly)
	{
		m_pageAbility.TransData(TRUE);
		m_pageSkill.TransData(TRUE);
	}
}

VOID CMemPmPage::OnChangeUILang(VOID)
{
	DWORD	dwIndex;

	SetPmList();

	for(dwIndex = 0; dwIndex < MEM_PM_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->OnChangeUILang();
	}

	CPropertySheet *pPropSht = reinterpret_cast<CPropertySheet *>(GetParent());
	if(pPropSht)
	{
		CMemMiscPage1 *pMiscPage1 =reinterpret_cast<CMemMiscPage1 *>(pPropSht->GetPage(2));
		if(pMiscPage1)
		{
			pMiscPage1->OnChangeUILang();
		}
	}
}

void CMemPmPage::OnBnClickedLangCn()
{
	if(cfg.dwLang == ui_lang_cn)
		return;

	cfg.dwLang = ui_lang_cn;
	OnChangeUILang();
}

void CMemPmPage::OnBnClickedLangJp()
{
	if(cfg.dwLang == ui_lang_jp)
		return;

	cfg.dwLang = ui_lang_jp;
	OnChangeUILang();
}

void CMemPmPage::OnBnClickedLangEn()
{
	if(cfg.dwLang == ui_lang_en)
		return;

	cfg.dwLang = ui_lang_en;
	OnChangeUILang();
}

void CMemPmPage::OnBnClickedCountNone()
{
	if(cfg.dwCount == ui_count_none)
		return;

	cfg.dwCount = ui_count_none;
	OnChangeUILang();
}

void CMemPmPage::OnBnClickedCountDec()
{
	if(cfg.dwCount == ui_count_dec)
		return;

	cfg.dwCount = ui_count_dec;
	OnChangeUILang();
}

void CMemPmPage::OnBnClickedCountHex()
{
	if(cfg.dwCount == ui_count_hex)
		return;

	cfg.dwCount = ui_count_hex;
	OnChangeUILang();
}

void CMemPmPage::OnBnClickedRefresh()
{
	DWORD	dwIndex;

	if(!g_MemHack.ReadActivePokemons())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
	else if(!g_MemRom.OpenRom(
				NULL,
				&g_MemHack.m_vbah,
				CPokemonRom::rom_breed_list |
				CPokemonRom::rom_skill_list |
				CPokemonRom::rom_experience_list |
				CPokemonRom::rom_tiny_image_list))
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
		m_ctrlConfirm.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlConfirm.EnableWindow(TRUE);
	}

	SetPmList();
	for(dwIndex = 0; dwIndex < MEM_PM_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->TransData(TRUE);
	}
}

void CMemPmPage::OnBnClickedConfirm()
{
	DWORD	dwIndex;

	for(dwIndex = 0; dwIndex < MEM_PM_PAGE_COUNT; ++dwIndex)
	{
		m_rgpPages[dwIndex]->TransData(FALSE);
	}

	if(!g_MemHack.SaveActivePokemons())
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemPmPage::OnLvnItemchangedPmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	DWORD	dwIndex;

	if((pNMLV->uNewState & LVIS_SELECTED) &&
		m_dwCurPm != pNMLV->iItem)
	{
		if(m_dwCurPm < ACTIVE_POKEMON_COUNT)
		{
			for(dwIndex = 0; dwIndex < MEM_PM_PAGE_COUNT; ++dwIndex)
			{
				m_rgpPages[dwIndex]->TransData(FALSE);
			}
		}

		m_dwCurPm = pNMLV->iItem;

		if(m_dwCurPm < ACTIVE_POKEMON_COUNT)
		{
			for(dwIndex = 0; dwIndex < MEM_PM_PAGE_COUNT; ++dwIndex)
			{
				m_rgpPages[dwIndex]->TransData(TRUE);
			}
		}
	}
}

void CMemPmPage::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	ShowTabPage(m_ctrlTab.GetCurSel());
}

void CMemPmPage::OnBnClickedMoveUp()
{
	if(m_dwCurPm > 0)
	{
		if(!g_MemHack.SwapActivePokemons(m_dwCurPm, m_dwCurPm - 1))
		{
			AfxMessageBox(_T("失败！"), MB_OK | MB_ICONERROR);
		}
		else
		{
			SetPmList();
			--m_dwCurPm;
			m_ctrlPmList.SetItemState(m_dwCurPm, -1, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}

void CMemPmPage::OnBnClickedMoveDown()
{
	if(m_dwCurPm < ACTIVE_POKEMON_COUNT - 1)
	{
		if(!g_MemHack.SwapActivePokemons(m_dwCurPm, m_dwCurPm + 1))
		{
			AfxMessageBox(_T("失败！"), MB_OK | MB_ICONERROR);
		}
		else
		{
			SetPmList();
			++m_dwCurPm;
			m_ctrlPmList.SetItemState(m_dwCurPm, -1, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}
