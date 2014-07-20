// BreedMachineLearnPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedMachineLearnPage.h"

// CBreedMachineLearnPage 对话框

IMPLEMENT_DYNAMIC(CBreedMachineLearnPage, CBreedTabPage)
CBreedMachineLearnPage::CBreedMachineLearnPage()
{
	m_uTemplateId = CBreedMachineLearnPage::IDD;
	m_pMachineLearnList = NULL;
	m_pMachineSkillList = NULL;
	m_dwCurSel = -1;
	m_dwCheckedCount = 0;
	m_bIngoreCheckNotify = TRUE;
	m_dwSortOrder = order_checked;
}

CBreedMachineLearnPage::~CBreedMachineLearnPage()
{
}

void CBreedMachineLearnPage::DoDataExchange(CDataExchange* pDX)
{
	CBreedTabPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ML_LIST, m_ctrlLearnList);
}


BEGIN_MESSAGE_MAP(CBreedMachineLearnPage, CBreedTabPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ML_LIST, OnLvnItemchangedMlList)
	ON_NOTIFY(NM_DBLCLK, IDC_ML_LIST, OnNMDblclkMlList)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickMlList)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_ML_ORDER_MACHINE, IDC_ML_ORDER_CHECKED, OnChangeSortOrder)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_ML_CHECK_ALL, IDC_ML_UNCHECK_SEL, OnButtonClicked)
END_MESSAGE_MAP()


// CBreedMachineLearnPage 消息处理程序
BOOL CBreedMachineLearnPage::OnInitDialog()
{
	CBreedTabPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlLearnList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_HEADERDRAGDROP | LVS_EX_CHECKBOXES);

	m_ctrlLearnList.InsertColumn(0, _T("机器"), LVCFMT_LEFT, 50, 0);
	m_ctrlLearnList.InsertColumn(1, _T("技能"), LVCFMT_LEFT, 120, 1);
	m_ctrlLearnList.InsertColumn(2, _T("技能编号"), LVCFMT_LEFT, 40, 2);

	CheckDlgButton(IDC_ML_ORDER_CHECKED, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBreedMachineLearnPage::GetMachineSkillList()
{
	if(rom.m_bOpened)
		m_pMachineSkillList = rom.m_pMachineSkillList;
	else
		m_pMachineSkillList = NULL;
}

void CBreedMachineLearnPage::GetCurMachineLearnList()
{
	WORD	wBreed = theApp.m_dlgBreed.m_wCurBreed;

	if(rom.m_bOpened && wBreed < BREED_COUNT)
		m_pMachineLearnList = rom.GetMachineLearnList(wBreed);
	else
		m_pMachineLearnList = NULL;
}

void CBreedMachineLearnPage::RefreshLearnList()
{
	WORD	wSkill;
	DWORD	dwIndex, dwCount;
	LVITEM	lvi;
	CString	szText;
	LPCTSTR	szFmt33[3] = { _T("%-3hu"), _T("%-3hu"), _T("%03hX") };

	if(!m_pMachineSkillList)
	{
		m_ctrlLearnList.DeleteAllItems();
		m_dwCurSel = -1;
		return;
	}

	lvi.mask = LVIF_TEXT;
	dwCount = m_ctrlLearnList.GetItemCount();
	for(dwIndex = 0; dwIndex < MACHINE_COUNT; ++dwIndex)
	{
		lvi.iItem = dwIndex;

		wSkill = m_pMachineSkillList[dwIndex];

		// machine
		lvi.iSubItem = 0;
		if(dwIndex < 50)
			szText.Format(_T("技%02lu"), dwIndex + 1);
		else
			szText.Format(_T("秘%lu"), dwIndex - 49);
		lvi.pszText = szText.GetBuffer();
		if(dwIndex < dwCount)
			m_ctrlLearnList.SetItem(&lvi);
		else
			m_ctrlLearnList.InsertItem(&lvi);
		szText.ReleaseBuffer();

		// skill
		lvi.iSubItem = 1;
		if(wSkill < SKILL_COUNT)
			szText = cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang];
		else
			szText.Format(_T("0x%x"),wSkill);
		lvi.pszText = szText.GetBuffer();
		m_ctrlLearnList.SetItem(&lvi);
		szText.ReleaseBuffer();

		// skill id
		lvi.iSubItem = 2;
		szText.Format(szFmt33[cfg.dwCount], wSkill);
		lvi.pszText = szText.GetBuffer();
		m_ctrlLearnList.SetItem(&lvi);
		szText.ReleaseBuffer();

		m_ctrlLearnList.SetItemData(dwIndex, ((DWORD_PTR)(wSkill) << 8) + dwIndex);
	}
}

void CBreedMachineLearnPage::SetMachineSkills()
{
	DWORD		dwIndex;
	DWORD_PTR	dwData;
	POSITION	pos;

	/*
		dwData <=>
		bit0-7: machine id
		bit8-23: skill id
		bit24: check state
	*/
	if(!m_pMachineLearnList || !m_pMachineSkillList)
		return;

	// prevent OnLvnItemchangedMlList() modifing m_dwCheckedCount
	m_bIngoreCheckNotify = TRUE;

	m_dwCheckedCount = 0;

	for(dwIndex = 0; dwIndex < MACHINE_COUNT; ++dwIndex)
	{
		dwData = m_ctrlLearnList.GetItemData(dwIndex);

		if(GetBit(m_pMachineLearnList, dwData & 0xFF))
		{
			++m_dwCheckedCount;
			m_ctrlLearnList.SetCheck(dwIndex, TRUE);
			if((dwData & 0xFF000000) == 0)
			{
				dwData |= 0x01000000;
				m_ctrlLearnList.SetItemData(dwIndex, dwData);
			}
		}
		else
		{
			m_ctrlLearnList.SetCheck(dwIndex, FALSE);
			if(dwData & 0xFF000000)
			{
				dwData &= 0x00FFFFFF;
				m_ctrlLearnList.SetItemData(dwIndex, dwData);
			}
		}
	}

	SetCheckedCount();

	if(m_dwSortOrder != order_machine)
	{
		// clear selection
		pos = m_ctrlLearnList.GetFirstSelectedItemPosition();
		while(pos)
		{
			dwIndex = m_ctrlLearnList.GetNextSelectedItem(pos);
			m_ctrlLearnList.SetItemState(dwIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
		}

		// sort
		m_ctrlLearnList.SortItems(SortSkillList, m_dwSortOrder);

		// set selection
		m_ctrlLearnList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);
		//m_ctrlLearnList.EnsureVisible(0, FALSE);
		m_dwCurSel = 0;
		SetSkillDesc();
	}

	m_bIngoreCheckNotify = FALSE;
}

void CBreedMachineLearnPage::SetCheckedCount()
{
	CString	szText;

	szText.Format(_T("%lu项"), m_dwCheckedCount);
	SetDlgItemText(IDC_ML_COUNT, szText);
}

void CBreedMachineLearnPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	GetMachineSkillList();
	RefreshLearnList();

	if(bRomChanged)
		TransData(TRUE);
}

void CBreedMachineLearnPage::TransData(BOOL bToControls)
{
	GetCurMachineLearnList();
	if(!m_pMachineLearnList)
		return;

	if(bToControls)
	{
		SetMachineSkills();
	}
}

void CBreedMachineLearnPage::SetSkillDesc()
{
	if(!m_pMachineLearnList || !m_pMachineSkillList || m_dwCurSel >= MACHINE_COUNT)
		return;

	DWORD_PTR	dwData;
	CString		szText;

	dwData = m_ctrlLearnList.GetItemData(m_dwCurSel);
	FormatSkillDesc(m_pMachineSkillList[dwData & 0xFF], szText);
	SetDlgItemText(IDC_ML_SKILL_DESC, szText);
}

void CBreedMachineLearnPage::OnLvnItemchangedMlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	DWORD_PTR	dwData;

	if(	m_pMachineLearnList &&
		m_pMachineSkillList &&
		pNMLV->iItem < MACHINE_COUNT)
	{
		// an item is selected & focused
		if(pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED))
		{
			m_dwCurSel = pNMLV->iItem;
			SetSkillDesc();
		}

		// an item is checked or unchecked
		if(	!m_bIngoreCheckNotify &&
			(pNMLV->uNewState & LVIS_STATEIMAGEMASK) != (pNMLV->uOldState & LVIS_STATEIMAGEMASK))
		{
			dwData = m_ctrlLearnList.GetItemData(pNMLV->iItem);

			// the item was checked (so it's unchecked now)
			if(dwData & 0xFF000000)
			{
				--m_dwCheckedCount;
				ClrBit(m_pMachineLearnList, dwData & 0xFF);
				dwData &= 0x00FFFFFF;
			}
			// the item was unchecked (so it's checked now)
			else
			{
				SetBit(m_pMachineLearnList, dwData & 0xFF);
				++m_dwCheckedCount;
				dwData |= 0x01000000;
			}

			m_ctrlLearnList.SetItemData(pNMLV->iItem, dwData);
			SetCheckedCount();
		}
	}
}

void CBreedMachineLearnPage::OnNMDblclkMlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(	m_pMachineLearnList &&
		m_pMachineSkillList &&
		m_dwCurSel < MACHINE_COUNT)
	{
		theApp.m_dlgSkill.SetCurSkill(m_pMachineSkillList[m_dwCurSel]);
		theApp.m_dlgSkill.ShowWindow(SW_SHOWDEFAULT);
		theApp.m_dlgSkill.SetForegroundWindow();
	}
}

INT CALLBACK CBreedMachineLearnPage::SortSkillList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	INT	iResult = 0;

	switch(lParamSort)
	{
	case order_machine:
		iResult = (INT)((lParam1 & 0xFF) - (lParam2 & 0xFF));
		break;

	case order_checked:
		iResult = (INT)((lParam2 & 0xFF000000) - (lParam1 & 0xFF000000));
		if(iResult == 0)
			iResult = (INT)((lParam1 & 0xFF) - (lParam2 & 0xFF));
		break;
	}

	return iResult;
}

void CBreedMachineLearnPage::OnHdnItemclickMlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	POSITION	pos;

	if(m_dwSortOrder != order_machine)
		m_ctrlLearnList.SortItems(SortSkillList, m_dwSortOrder);

	pos = m_ctrlLearnList.GetFirstSelectedItemPosition();
	if(pos)
	{
		m_dwCurSel = m_ctrlLearnList.GetNextSelectedItem(pos);
		m_ctrlLearnList.EnsureVisible(m_dwCurSel, FALSE);
	}
	else
	{
		m_dwCurSel = -1;
	}
}

void CBreedMachineLearnPage::SetCheck(DWORD dwItem, BOOL bCheck)
{
	DWORD_PTR	dwData;

	if(	!m_pMachineLearnList ||
		!m_pMachineSkillList ||
		dwItem >= MACHINE_COUNT)
		return;

	m_bIngoreCheckNotify = TRUE;

	dwData = m_ctrlLearnList.GetItemData(dwItem);
	if(bCheck)
	{
		// check the item if it was unchecked
		if((dwData & 0xFF000000) == 0)
		{
			m_ctrlLearnList.SetCheck(dwItem, bCheck);
			SetBit(m_pMachineLearnList, dwData & 0xFF);
			++m_dwCheckedCount;

			dwData |= 0x01000000;
			m_ctrlLearnList.SetItemData(dwItem, dwData);
		}
	}
	else
	{
		// uncheck the item if it was checked
		if(dwData & 0xFF000000)
		{
			m_ctrlLearnList.SetCheck(dwItem, bCheck);
			ClrBit(m_pMachineLearnList, dwData & 0xFF);
			--m_dwCheckedCount;

			dwData &= 0x00FFFFFF;
			m_ctrlLearnList.SetItemData(dwItem, dwData);
		}
	}

	m_bIngoreCheckNotify = FALSE;
}

void CBreedMachineLearnPage::OnButtonClicked(UINT uID)
{
	BOOL		bCheck = TRUE;
	POSITION	pos;
	DWORD		dwIndex;

	if(!m_pMachineLearnList || !m_pMachineSkillList)
		return;

	switch(uID)
	{
	case IDC_ML_UNCHECK_ALL:
		bCheck = FALSE;
	case IDC_ML_CHECK_ALL:
		// check/uncheck all items
		for(dwIndex = 0; dwIndex < MACHINE_COUNT; ++dwIndex)
		{
			SetCheck(dwIndex, bCheck);
		}
		break;

	case IDC_ML_UNCHECK_SEL:
		bCheck = FALSE;
	case IDC_ML_CHECK_SEL:
		pos = m_ctrlLearnList.GetFirstSelectedItemPosition();
		while(pos)
		{
			dwIndex = m_ctrlLearnList.GetNextSelectedItem(pos);
			SetCheck(dwIndex, bCheck);
		}
		break;
	}

	SetCheckedCount();
}

void CBreedMachineLearnPage::OnChangeSortOrder(UINT uID)
{
	POSITION	pos;
	uID -= IDC_ML_ORDER_MACHINE;
	if(uID < 2 && uID != m_dwSortOrder)
	{
		m_dwSortOrder = uID;
		m_ctrlLearnList.SortItems(SortSkillList, m_dwSortOrder);

		pos = m_ctrlLearnList.GetFirstSelectedItemPosition();
		if(pos)
		{
			m_dwCurSel = m_ctrlLearnList.GetNextSelectedItem(pos);
			m_ctrlLearnList.EnsureVisible(m_dwCurSel, FALSE);
		}
		else
		{
			m_dwCurSel = -1;
		}
	}
}
