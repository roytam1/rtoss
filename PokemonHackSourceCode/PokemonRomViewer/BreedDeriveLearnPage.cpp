// BreedDeriveLearnPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedDeriveLearnPage.h"

// CBreedDeriveLearnPage 对话框

IMPLEMENT_DYNAMIC(CBreedDeriveLearnPage, CBreedTabPage)
CBreedDeriveLearnPage::CBreedDeriveLearnPage()
{
	m_uTemplateId = CBreedDeriveLearnPage::IDD;
	m_pDeriveLearnList = NULL;
	m_dwDeriveLearnCount = 0;
	m_dwCurSel = -1;
}

CBreedDeriveLearnPage::~CBreedDeriveLearnPage()
{
}

void CBreedDeriveLearnPage::DoDataExchange(CDataExchange* pDX)
{
	CBreedTabPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DL_LIST, m_ctrlLearnList);
	DDX_Control(pDX, IDC_DL_SKILL, m_ctrlSkillList);
}


BEGIN_MESSAGE_MAP(CBreedDeriveLearnPage, CBreedTabPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DL_LIST, OnLvnItemchangedDlList)
	ON_NOTIFY(NM_DBLCLK, IDC_DL_LIST, OnNMDblclkDlList)
	ON_BN_CLICKED(IDC_DL_FROM_SKILL_DLG, OnBnClickedDlFromSkillDlg)
	ON_BN_CLICKED(IDC_DL_SET_SKILL, OnBnClickedDlSetSkill)
END_MESSAGE_MAP()


// CBreedDeriveLearnPage 消息处理程序
BOOL CBreedDeriveLearnPage::OnInitDialog()
{
	CBreedTabPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlLearnList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_HEADERDRAGDROP);

	m_ctrlLearnList.InsertColumn(0, _T("技能"), LVCFMT_LEFT, 120, 0);
	m_ctrlLearnList.InsertColumn(1, _T("技能编号"), LVCFMT_LEFT, 40, 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBreedDeriveLearnPage::GetCurDeriveLearnList()
{
	WORD	wBreed = theApp.m_dlgBreed.m_wCurBreed;

	if(rom.m_bOpened && wBreed < BREED_COUNT)
	{
		m_pDeriveLearnList = rom.GetDeriveLearnList(wBreed);
		m_dwDeriveLearnCount = rom.GetDeriveLearnListEntryCount(wBreed);
	}
	else
	{
		m_pDeriveLearnList = NULL;
		m_dwDeriveLearnCount = 0;
	}
}

void CBreedDeriveLearnPage::RefreshLearnList()
{
	LVITEM	lvi;
	WORD	wSkill;
	DWORD	dwIndex, dwCount;
	CString	szText;
	LPCTSTR	szFmt33[3] = { _T("%-3hu"), _T("%-3hu"), _T("%03hX") };

	szText.Format(_T("%lu项"), m_dwDeriveLearnCount);
	SetDlgItemText(IDC_DL_COUNT, szText);

	if(!m_pDeriveLearnList)
	{
		m_ctrlLearnList.DeleteAllItems();
		m_dwCurSel = -1;
		return;
	}

	lvi.mask = LVIF_TEXT;
	dwCount = m_ctrlLearnList.GetItemCount();
	for(dwIndex = 0; dwIndex < m_dwDeriveLearnCount; ++dwIndex)
	{
		lvi.iItem = dwIndex;

		wSkill = m_pDeriveLearnList[dwIndex];

		// skill
		lvi.iSubItem = 0;
		szText = cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang];
		lvi.pszText = szText.GetBuffer();
		if(dwIndex < dwCount)
			m_ctrlLearnList.SetItem(&lvi);
		else
			m_ctrlLearnList.InsertItem(&lvi);
		szText.ReleaseBuffer();

		// skill id
		lvi.iSubItem = 1;
		szText.Format(szFmt33[cfg.dwCount], wSkill);
		lvi.pszText = szText.GetBuffer();
		m_ctrlLearnList.SetItem(&lvi);
		szText.ReleaseBuffer();

		m_ctrlLearnList.SetItemData(dwIndex, wSkill);
	}

	for(; dwIndex < dwCount; ++dwIndex)
	{
		m_ctrlLearnList.DeleteItem(m_dwDeriveLearnCount);
	}

	if(	m_dwDeriveLearnCount > 0 &&
		m_dwCurSel >= m_dwDeriveLearnCount)
	{
		m_ctrlLearnList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);
	}
	else
	{
		m_ctrlLearnList.SetItemState(m_dwCurSel, -1, LVIS_SELECTED | LVIS_FOCUSED);
		SetSkill();
	}
}

void CBreedDeriveLearnPage::RefreshSkillList()
{
	WORD	wSkill, wCurSel;
	CString	szIndex, szText;
	LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };

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

void CBreedDeriveLearnPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	if(bRomChanged)
		GetCurDeriveLearnList();

	RefreshSkillList();
	RefreshLearnList();
}

void CBreedDeriveLearnPage::TransData(BOOL bToControls)
{
	GetCurDeriveLearnList();

	if(bToControls)
	{
		RefreshLearnList();
	}
	else
	{
		GetSkills();
	}
}

void CBreedDeriveLearnPage::SetSkill()
{
	SetSkillDesc();
}

void CBreedDeriveLearnPage::SetSkillDesc()
{
	if(!m_pDeriveLearnList || m_dwCurSel >= m_dwDeriveLearnCount)
		return;

	DWORD_PTR	dwData;
	WORD		wSkill;
	CString		szText;

	dwData = m_ctrlLearnList.GetItemData(m_dwCurSel);
	wSkill = (WORD)(dwData & 0xFFFF);
	FormatSkillDesc(wSkill, szText);
	SetDlgItemText(IDC_DL_SKILL_DESC, szText);
}

void CBreedDeriveLearnPage::GetSkills()
{
	if(!m_pDeriveLearnList)
		return;

	DWORD		dwIndex;
	DWORD_PTR	dwData;
	WORD		wSkill;

	for(dwIndex = 0; dwIndex < m_dwDeriveLearnCount; ++dwIndex)
	{
		dwData = m_ctrlLearnList.GetItemData(dwIndex);
		wSkill = (WORD)(dwData & 0xFFFF);
		m_pDeriveLearnList[dwIndex] = wSkill;
	}
}

void CBreedDeriveLearnPage::OnLvnItemchangedDlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(	m_pDeriveLearnList &&
		(DWORD)(pNMLV->iItem) < m_dwDeriveLearnCount &&
		(DWORD)(pNMLV->iItem) != m_dwCurSel &&
		pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED))
	{
		m_dwCurSel = pNMLV->iItem;
		SetSkill();
	}
}

void CBreedDeriveLearnPage::OnNMDblclkDlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	DWORD_PTR	dwData;
	WORD		wSkill;

	if(	m_pDeriveLearnList &&
		m_dwCurSel < m_dwDeriveLearnCount)
	{
		dwData = m_ctrlLearnList.GetItemData(m_dwCurSel);
		wSkill = (WORD)(dwData & 0xFFFF);
		theApp.m_dlgSkill.SetCurSkill(wSkill);
		theApp.m_dlgSkill.ShowWindow(SW_SHOWDEFAULT);
		theApp.m_dlgSkill.SetForegroundWindow();
	}
}

void CBreedDeriveLearnPage::OnBnClickedDlFromSkillDlg()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD	wSkill = theApp.m_dlgSkill.m_wCurSkill;

	if(wSkill < SKILL_COUNT)
		m_ctrlSkillList.SetCurSel(wSkill);
}

void CBreedDeriveLearnPage::OnBnClickedDlSetSkill()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD	wSkill;
	CString	szText;
	LVITEM	lvi;
	LPCTSTR	szFmt33[3] = { _T("%-3hu"), _T("%-3hu"), _T("%03hX") };

	if(m_dwCurSel >= m_dwDeriveLearnCount)
		return;

	wSkill = m_ctrlSkillList.GetCurSel();
	if(wSkill == 0 || wSkill >= SKILL_COUNT)
		return;

	lvi.mask = LVIF_TEXT;
	lvi.iItem = m_dwCurSel;

	// skill
	lvi.iSubItem = 0;
	szText = cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang];
	lvi.pszText = szText.GetBuffer();
	m_ctrlLearnList.SetItem(&lvi);
	szText.ReleaseBuffer();

	// skill id
	lvi.iSubItem = 1;
	szText.Format(szFmt33[cfg.dwCount], wSkill);
	lvi.pszText = szText.GetBuffer();
	m_ctrlLearnList.SetItem(&lvi);
	szText.ReleaseBuffer();

	m_ctrlLearnList.SetItemData(m_dwCurSel, wSkill);

	SetSkill();
}
