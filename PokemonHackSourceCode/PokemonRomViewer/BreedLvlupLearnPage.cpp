// BreedLvlupLearnPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedLvlupLearnPage.h"

// CBreedLvlupLearnPage 对话框

IMPLEMENT_DYNAMIC(CBreedLvlupLearnPage, CBreedTabPage)
CBreedLvlupLearnPage::CBreedLvlupLearnPage()
{
	m_uTemplateId = CBreedLvlupLearnPage::IDD;
	m_pLvlupLearnList = NULL;
	m_dwLvlupLearnCount = 0;
	m_dwCurSel = -1;
}

CBreedLvlupLearnPage::~CBreedLvlupLearnPage()
{
}

void CBreedLvlupLearnPage::DoDataExchange(CDataExchange* pDX)
{
	CBreedTabPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LUL_LIST, m_ctrlLearnList);
	DDX_Control(pDX, IDC_LUL_SKILL, m_ctrlSkillList);
}


BEGIN_MESSAGE_MAP(CBreedLvlupLearnPage, CBreedTabPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LUL_LIST, OnLvnItemchangedLulList)
	ON_NOTIFY(NM_DBLCLK, IDC_LUL_LIST, OnNMDblclkLulList)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickLulList)
	ON_BN_CLICKED(IDC_LUL_FROM_SKILL_DLG, OnBnClickedLulFromSkillDlg)
	ON_BN_CLICKED(IDC_LUL_SET_SKILL, OnBnClickedLulSetSkill)
END_MESSAGE_MAP()


// CBreedLvlupLearnPage 消息处理程序
void CBreedLvlupLearnPage::GetCurLvlLearnList()
{
	WORD	wBreed = theApp.m_dlgBreed.m_wCurBreed;

	if(rom.m_bOpened && wBreed < BREED_COUNT)
	{
		m_pLvlupLearnList = rom.GetLvlupLearnList(wBreed);
		m_dwLvlupLearnCount = rom.GetLvlupLearnListEntryCount(wBreed);
	}
	else
	{
		m_pLvlupLearnList = NULL;
		m_dwLvlupLearnCount = 0;
	}
}

void CBreedLvlupLearnPage::RefreshLearnList()
{
	LVITEM	lvi;
	BYTE	bLevel;
	WORD	wSkill;
	DWORD	dwIndex, dwCount;
	CString	szText;
	LPCTSTR	szFmt33[3] = { _T("%-3hu"), _T("%-3hu"), _T("%03hX") };

	szText.Format(_T("%lu项"), m_dwLvlupLearnCount);
	SetDlgItemText(IDC_LUL_COUNT, szText);

	if(!m_pLvlupLearnList)
	{
		m_ctrlLearnList.DeleteAllItems();
		m_dwCurSel = -1;
		return;
	}

	lvi.mask = LVIF_TEXT;
	dwCount = m_ctrlLearnList.GetItemCount();
	for(dwIndex = 0; dwIndex < m_dwLvlupLearnCount; ++dwIndex)
	{
		lvi.iItem = dwIndex;

		bLevel = (BYTE)(m_pLvlupLearnList[dwIndex].wLevel);
		wSkill = m_pLvlupLearnList[dwIndex].wSkill;

		// level
		lvi.iSubItem = 0;
		szText.Format(_T("%hu"), bLevel);
		lvi.pszText = szText.GetBuffer();
		if(dwIndex < dwCount)
			m_ctrlLearnList.SetItem(&lvi);
		else
			m_ctrlLearnList.InsertItem(&lvi);
		szText.ReleaseBuffer();

		// skill
		lvi.iSubItem = 1;
		szText = cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang];
		lvi.pszText = szText.GetBuffer();
		m_ctrlLearnList.SetItem(&lvi);
		szText.ReleaseBuffer();

		// skill id
		lvi.iSubItem = 2;
		szText.Format(szFmt33[cfg.dwCount], wSkill);
		lvi.pszText = szText.GetBuffer();
		m_ctrlLearnList.SetItem(&lvi);
		szText.ReleaseBuffer();

		m_ctrlLearnList.SetItemData(dwIndex, ((DWORD_PTR)(wSkill) << 8) + bLevel);
	}

	for(; dwIndex < dwCount; ++dwIndex)
	{
		m_ctrlLearnList.DeleteItem(m_dwLvlupLearnCount);
	}

	if(	m_dwLvlupLearnCount > 0 &&
		m_dwCurSel >= m_dwLvlupLearnCount)
	{
		m_ctrlLearnList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);
	}
	else
	{
		m_ctrlLearnList.SetItemState(m_dwCurSel, -1, LVIS_SELECTED);
		SetSkill();
	}
}

void CBreedLvlupLearnPage::RefreshSkillList()
{
	WORD	wSkill, wCurSel;
	CString	szIndex, szText;
	LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };

	if(!m_pLvlupLearnList)
		return;

	wCurSel = m_ctrlSkillList.GetCurSel();
	m_ctrlSkillList.ResetContent();
	for(wSkill = 0; wSkill < SKILL_COUNT; ++wSkill)
	{
		szIndex.Format(szFmt33[cfg.dwCount], wSkill);
		szText = szIndex + cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang];
		m_ctrlSkillList.AddString(szText);
	}

	if(wCurSel < SKILL_COUNT)
		m_ctrlSkillList.SetCurSel(wCurSel);
}

void CBreedLvlupLearnPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	if(bRomChanged)
		GetCurLvlLearnList();

	RefreshSkillList();
	RefreshLearnList();
}

void CBreedLvlupLearnPage::TransData(BOOL bToControls)
{
	GetCurLvlLearnList();
	if(!m_pLvlupLearnList)
		return;

	if(bToControls)
	{
		RefreshLearnList();
	}
	else
	{
		GetSkills();
	}
}

BOOL CBreedLvlupLearnPage::OnInitDialog()
{
	CBreedTabPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlLearnList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_HEADERDRAGDROP);

	m_ctrlLearnList.InsertColumn(0, _T("等级"), LVCFMT_LEFT, 40, 0);
	m_ctrlLearnList.InsertColumn(1, _T("技能"), LVCFMT_LEFT, 120, 1);
	m_ctrlLearnList.InsertColumn(2, _T("技能编号"), LVCFMT_LEFT, 40, 2);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBreedLvlupLearnPage::SetSkill()
{
	if(!m_pLvlupLearnList || m_dwCurSel >= m_dwLvlupLearnCount)
		return;

	SetSkillDesc();
}

void CBreedLvlupLearnPage::SetSkillDesc()
{
	if(!m_pLvlupLearnList || m_dwCurSel >= m_dwLvlupLearnCount)
		return;

	DWORD_PTR	dwData;
	WORD		wSkill;
	CString		szText;

	dwData = m_ctrlLearnList.GetItemData(m_dwCurSel);
	wSkill = (WORD)((dwData >> 8) & 0xFFFF);
	FormatSkillDesc(wSkill, szText);
	SetDlgItemText(IDC_LUL_SKILL_DESC, szText);
}

void CBreedLvlupLearnPage::GetSkills()
{
	DWORD		dwIndex, dwIndex2;
	DWORD_PTR	dwData;
	LvlupLearnListEntry	Entry;

	if(!m_pLvlupLearnList)
		return;

	for(dwIndex = 0; dwIndex < m_dwLvlupLearnCount; ++dwIndex)
	{
		dwData = m_ctrlLearnList.GetItemData(dwIndex);
		m_pLvlupLearnList[dwIndex].wLevel = (BYTE)(dwData & 0xFF);
		m_pLvlupLearnList[dwIndex].wSkill = (WORD)((dwData >> 8) & 0xFFFF);

		// simple sort (it doesn't matter, as there're just a few entries)
		for(dwIndex2 = dwIndex; dwIndex2 > 0; --dwIndex2)
		{
			if(m_pLvlupLearnList[dwIndex2].wLevel >= m_pLvlupLearnList[dwIndex2 - 1].wLevel)
				break;

			Entry = m_pLvlupLearnList[dwIndex2];
			m_pLvlupLearnList[dwIndex2] = m_pLvlupLearnList[dwIndex2 - 1];
			m_pLvlupLearnList[dwIndex2 - 1] = Entry;
		}
	}
}

void CBreedLvlupLearnPage::OnLvnItemchangedLulList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(	m_pLvlupLearnList &&
		(DWORD)(pNMLV->iItem) < m_dwLvlupLearnCount &&
		(DWORD)(pNMLV->iItem) != m_dwCurSel &&
		pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED))
	{
		m_dwCurSel = pNMLV->iItem;
		SetSkill();
	}
}

void CBreedLvlupLearnPage::OnNMDblclkLulList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	DWORD_PTR	dwData;
	WORD		wSkill;

	if(	m_pLvlupLearnList &&
		m_dwCurSel < m_dwLvlupLearnCount)
	{
		dwData = m_ctrlLearnList.GetItemData(m_dwCurSel);
		wSkill = (WORD)((dwData >> 8) & 0xFFFF);
		theApp.m_dlgSkill.SetCurSkill(wSkill);
		theApp.m_dlgSkill.ShowWindow(SW_SHOWDEFAULT);
		theApp.m_dlgSkill.SetForegroundWindow();
	}
}

INT CALLBACK CBreedLvlupLearnPage::SortSkillList(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	INT	iResult;

	iResult = (INT)((lParam1 & 0xFF) - (lParam2 & 0xFF));

	return iResult;
}

void CBreedLvlupLearnPage::OnHdnItemclickLulList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	POSITION	pos;

	m_ctrlLearnList.SortItems(SortSkillList, 0);

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

void CBreedLvlupLearnPage::OnBnClickedLulFromSkillDlg()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD	wSkill = theApp.m_dlgSkill.m_wCurSkill;

	if(wSkill < SKILL_COUNT)
		m_ctrlSkillList.SetCurSel(wSkill);
}

void CBreedLvlupLearnPage::OnBnClickedLulSetSkill()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD	wSkill;
	BYTE	bLevel;
	CString	szText;
	LVITEM	lvi;
	LPCTSTR	szFmt33[3] = { _T("%-3hu"), _T("%-3hu"), _T("%03hX") };

	if(m_dwCurSel >= m_dwLvlupLearnCount)
		return;

	wSkill = m_ctrlSkillList.GetCurSel();
	if(wSkill == 0 || wSkill >= SKILL_COUNT)
		return;

	GetDlgItemText(IDC_LUL_LEVEL, szText);
	bLevel = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));
	if(bLevel == 0)
		return;

	lvi.mask = LVIF_TEXT;
	lvi.iItem = m_dwCurSel;

	// level
	lvi.iSubItem = 0;
	szText.Format(_T("%hu"), bLevel);
	lvi.pszText = szText.GetBuffer();
	m_ctrlLearnList.SetItem(&lvi);
	szText.ReleaseBuffer();

	// skill
	lvi.iSubItem = 1;
	szText = cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang];
	lvi.pszText = szText.GetBuffer();
	m_ctrlLearnList.SetItem(&lvi);
	szText.ReleaseBuffer();

	// skill id
	lvi.iSubItem = 2;
	szText.Format(szFmt33[cfg.dwCount], wSkill);
	lvi.pszText = szText.GetBuffer();
	m_ctrlLearnList.SetItem(&lvi);
	szText.ReleaseBuffer();

	m_ctrlLearnList.SetItemData(m_dwCurSel, ((DWORD_PTR)(wSkill) << 8) + bLevel);

	SetSkill();
}
