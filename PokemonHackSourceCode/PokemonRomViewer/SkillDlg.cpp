// SkillDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "SkillDlg.h"

struct SkillSortParam
{
	INT		iSubItem;
	BOOL	bAscending;
};

// CSkillDlg 对话框

IMPLEMENT_DYNAMIC(CSkillDlg, CBaseDialog)
CSkillDlg::CSkillDlg()
{
	m_uTemplateId = CSkillDlg::IDD;
	m_wCurSel = -1;
	m_wCurSkill = -1;
}

CSkillDlg::~CSkillDlg()
{
}

void CSkillDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SKL_SKILL_LIST, m_ctrlSkillList);
	DDX_Control(pDX, IDC_SKL_TARGET_LIST, m_ctrlTargetList);
	DDX_Control(pDX, IDC_SKL_TYPE_LIST, m_ctrlTypeList);
	DDX_Control(pDX, IDC_CONFIRM, m_ctrlConfirm);
}


BEGIN_MESSAGE_MAP(CSkillDlg, CBaseDialog)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickSklSkillList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SKL_SKILL_LIST, OnLvnItemchangedSklSkillList)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONFIRM, OnBnClickedConfirm)
END_MESSAGE_MAP()


// CSkillDlg 消息处理程序

BOOL CSkillDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DWORD	dwIndex, dwItem;
	CString	szText;

	for(dwIndex = 0; dwIndex < dwSkillTargetTypesCount; ++dwIndex)
	{
		szText.Format(_T("%02lX:%s"),
						(DWORD)(rgSkillTargetTypes[dwIndex].bCode),
						rgSkillTargetTypes[dwIndex].szName);
		dwItem = m_ctrlTargetList.AddString(szText);
		m_ctrlTargetList.SetItemData(dwItem, rgSkillTargetTypes[dwIndex].bCode);
	}

	for(dwIndex = 0; dwIndex < dwTypesCount; ++dwIndex)
	{
		szText.Format(_T("%02lX:%s"), dwIndex, szTypes[dwIndex]);
		m_ctrlTypeList.AddString(szText);
	}

	m_ctrlSkillList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	m_ctrlSkillList.InsertColumn(0,		_T("名称"),			LVCFMT_LEFT, 110,	0);
	m_ctrlSkillList.InsertColumn(1,		_T("编号"),			LVCFMT_LEFT, 40,	1);
	m_ctrlSkillList.InsertColumn(2,		_T("属性"),			LVCFMT_LEFT, 50,	2);
	m_ctrlSkillList.InsertColumn(3,		_T("威力"),			LVCFMT_LEFT, 40,	3);
	m_ctrlSkillList.InsertColumn(4,		_T("命中"),			LVCFMT_LEFT, 40,	4);
	m_ctrlSkillList.InsertColumn(5,		_T("ＰＰ"),			LVCFMT_LEFT, 40,	5);
	m_ctrlSkillList.InsertColumn(6,		_T("效果"),			LVCFMT_LEFT, 40,	6);
	m_ctrlSkillList.InsertColumn(7,		_T("几率"),			LVCFMT_LEFT, 40,	7);
	m_ctrlSkillList.InsertColumn(8,		_T("对象"),			LVCFMT_LEFT, 65,	8);
	m_ctrlSkillList.InsertColumn(9,		_T("优先"),			LVCFMT_LEFT, 40,	9);
	m_ctrlSkillList.InsertColumn(10,	_T("近"),			LVCFMT_LEFT, 25,	10);
	m_ctrlSkillList.InsertColumn(11,	_T("守"),			LVCFMT_LEFT, 25,	10);
	m_ctrlSkillList.InsertColumn(12,	_T("替"),			LVCFMT_LEFT, 25,	10);
	m_ctrlSkillList.InsertColumn(13,	_T("?0"),			LVCFMT_LEFT, 25,	10);
	m_ctrlSkillList.InsertColumn(14,	_T("?1"),			LVCFMT_LEFT, 25,	10);
	m_ctrlSkillList.InsertColumn(15,	_T("王"),			LVCFMT_LEFT, 25,	10);
	m_ctrlSkillList.InsertColumn(16,	_T("效果说明"),		LVCFMT_LEFT, 250,	11);
	m_ctrlSkillList.InsertColumn(17,	_T("特殊"),			LVCFMT_LEFT, 100,	12);

	for(dwIndex = 0; dwIndex < SKILL_COUNT; ++dwIndex)
	{
		m_ctrlSkillList.InsertItem(dwIndex, _T(""));
		m_ctrlSkillList.SetItemData(dwIndex, dwIndex);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSkillDlg::SetSkillListItem(WORD wIndex)
{
	WORD	wSkill;
	DWORD	dwTargetTypeIndex;
	CString	szText;
	SkillListEntry *pSkill = NULL;

	if(!rom.m_bOpened || wIndex >= SKILL_COUNT)
		return;

	wSkill = (WORD)(m_ctrlSkillList.GetItemData(wIndex));
	pSkill = rom.GetSkillListEntry(wSkill);

	// 属性
	m_ctrlSkillList.SetItemText(wIndex, 2, szTypes[pSkill->bType]);

	// 威力
	szText.Format(_T("%lu"), (DWORD)(pSkill->bPower));
	m_ctrlSkillList.SetItemText(wIndex, 3, szText);

	// 命中
	szText.Format(_T("%lu"), (DWORD)(pSkill->bAccuracy));
	m_ctrlSkillList.SetItemText(wIndex, 4, szText);

	// ＰＰ
	szText.Format(_T("%lu"), (DWORD)(pSkill->bPP));
	m_ctrlSkillList.SetItemText(wIndex, 5, szText);

	// 效果
	szText.Format(_T("%02lXh"), (DWORD)(pSkill->bEffect));
	m_ctrlSkillList.SetItemText(wIndex, 6, szText);

	// 几率
	szText.Format(_T("%lu"), (DWORD)(pSkill->bEffectRatio));
	m_ctrlSkillList.SetItemText(wIndex, 7, szText);

	// 对象
	dwTargetTypeIndex = GetTargetTypeIndex(pSkill->bTargetType);
	m_ctrlSkillList.SetItemText(wIndex, 8, rgSkillTargetTypes[dwTargetTypeIndex].szName);

	// 优先度
	szText.Format(_T("%li"), (LONG)(pSkill->nPriority));
	m_ctrlSkillList.SetItemText(wIndex, 9, szText);

	// 近
	szText.Format(_T("%lu"), (pSkill->dwFlags & skl_melee) ? 1 : 0);
	m_ctrlSkillList.SetItemText(wIndex, 10, szText);

	// 守
	szText.Format(_T("%lu"), (pSkill->dwFlags & skl_protect) ? 1 : 0);
	m_ctrlSkillList.SetItemText(wIndex, 11, szText);

	// 替
	szText.Format(_T("%lu"), (pSkill->dwFlags & skl_substitute) ? 1 : 0);
	m_ctrlSkillList.SetItemText(wIndex, 12, szText);

	// ?0
	szText.Format(_T("%lu"), (pSkill->dwFlags & skl_unk0) ? 1 : 0);
	m_ctrlSkillList.SetItemText(wIndex, 13, szText);

	// ?1
	szText.Format(_T("%lu"), (pSkill->dwFlags & skl_unk1) ? 1 : 0);
	m_ctrlSkillList.SetItemText(wIndex, 14, szText);

	// 王
	szText.Format(_T("%lu"), (pSkill->dwFlags & skl_kingsrock) ? 1 : 0);
	m_ctrlSkillList.SetItemText(wIndex, 15, szText);

	// 效果说明
	m_ctrlSkillList.SetItemText(wIndex, 16, cfg.pSkillEffList[pSkill->bEffect].rgszText[cfg.dwLang]);

	// 特殊说明
	m_ctrlSkillList.SetItemText(wIndex, 17, cfg.pSkillSpecList[wSkill].rgszText[cfg.dwLang]);
}

void CSkillDlg::SetSkillDesc()
{
	SkillListEntry *pSkill = NULL;

	if(!rom.m_bOpened || m_wCurSkill >= SKILL_COUNT)
		return;

	pSkill = rom.GetSkillListEntry(m_wCurSkill);
	SetDlgItemText(IDC_SKL_DESC, cfg.pSkillEffList[pSkill->bEffect].rgszText[cfg.dwLang]);
}

void CSkillDlg::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	if(!rom.m_bOpened)
		return;

	if(bUILangChanged)
	{
		WORD	wIndex, wSkill;
		CString	szText;
		LPCTSTR	szFmt33[3] = { _T("%-3hu"), _T("%-3hu"), _T("%03hX") };

		for(wIndex = 0; wIndex < SKILL_COUNT; ++wIndex)
		{
			wSkill = (WORD)(m_ctrlSkillList.GetItemData(wIndex));

			// 名称
			m_ctrlSkillList.SetItemText(wIndex, 0, cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang]);

			// 编号
			szText.Format(szFmt33[cfg.dwCount], wSkill);
			m_ctrlSkillList.SetItemText(wIndex, 1, szText);
		}
	}

	if(bRomChanged)
	{
		WORD	wIndex;

		for(wIndex = 0; wIndex < SKILL_COUNT; ++wIndex)
		{
			SetSkillListItem(wIndex);
		}

		if(m_wCurSel > SKILL_COUNT)
			m_ctrlSkillList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);
	}

	// 说明
	if(bUILangChanged)
		SetSkillDesc();
}

INT CALLBACK CSkillDlg::SortSkills(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	INT		iResult = 0;
	SkillSortParam * pSSP = reinterpret_cast<SkillSortParam *>(lParamSort);
	SkillListEntry * pSkill1 = rom.GetSkillListEntry((WORD)(lParam1));
	SkillListEntry * pSkill2 = rom.GetSkillListEntry((WORD)(lParam2));

	switch(pSSP->iSubItem)
	{
	//case 0:		// 名称
	//	iResult = 0;
	//	break;

	case 1:	// 编号
		iResult = (INT)(lParam1 - lParam2);
		break;

	case 2:		// 属性
		iResult = pSkill1->bType - pSkill2->bType;
		break;

	case 3:		// 威力
		iResult = pSkill2->bPower - pSkill1->bPower;
		break;

	case 4:		// 命中
		iResult = pSkill2->bAccuracy - pSkill1->bAccuracy;
		break;

	case 5:		// ＰＰ
		iResult = pSkill2->bPP - pSkill1->bPP;
		break;

	case 6:		// 效果
		iResult = pSkill1->bEffect - pSkill2->bEffect;
		break;

	case 7:		// 几率
		iResult = pSkill2->bEffectRatio - pSkill1->bEffectRatio;
		break;

	case 8:		// 对象
		iResult = pSkill1->bTargetType - pSkill2->bTargetType;
		break;

	case 9:		// 优先度
		iResult = pSkill2->nPriority - pSkill1->nPriority;
		break;

	case 10:	// 近
		iResult = (INT)((pSkill1->dwFlags & skl_melee) - (pSkill2->dwFlags & skl_melee));
		break;

	case 11:	// 守
		iResult = (INT)((pSkill1->dwFlags & skl_protect) - (pSkill2->dwFlags & skl_protect));
		break;

	case 12:	// 替
		iResult = (INT)((pSkill1->dwFlags & skl_substitute) - (pSkill2->dwFlags & skl_substitute));
		break;

	case 13:	// ?0
		iResult = (INT)((pSkill1->dwFlags & skl_unk0) - (pSkill2->dwFlags & skl_unk0));
		break;

	case 14:	// ?1
		iResult = (INT)((pSkill1->dwFlags & skl_unk1) - (pSkill2->dwFlags & skl_unk1));
		break;

	case 15:	// 王
		iResult = (INT)((pSkill1->dwFlags & skl_kingsrock) - (pSkill2->dwFlags & skl_kingsrock));
		break;

	//case 16:	// 效果说明
	//	iResult = 0;
	//	break;

	//case 17:	// 特殊说明
	//	iResult = 0;
	//	break;
	}

	if(!pSSP->bAscending)
		iResult = -iResult;

	return iResult;
}

void CSkillDlg::SetCurSkill(WORD wSkill)
{
	WORD	wItem, wItemCount = m_ctrlSkillList.GetItemCount();
	WORD	wItemData;

	for(wItem = 0; wItem < wItemCount; ++wItem)
	{
		wItemData = (WORD)(m_ctrlSkillList.GetItemData(wItem));
		if(wSkill == wItemData)
		{
			m_ctrlSkillList.SetItemState(wItem, -1, LVIS_SELECTED | LVIS_FOCUSED);
			m_ctrlSkillList.EnsureVisible(wItem, FALSE);
			break;
		}
	}
}

void CSkillDlg::OnHdnItemclickSklSkillList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	static SkillSortParam ssp = { 0, TRUE };
	POSITION	pos;

	if(!rom.m_bOpened ||
		phdr->iItem == 0 ||		// 名称
		phdr->iItem == 16 ||	// 效果说明
		phdr->iItem == 17)		// 特殊说明
	{
		return;
	}

	if(ssp.iSubItem == phdr->iItem)
	{
		ssp.bAscending = ssp.bAscending ? FALSE : TRUE;
	}
	else
	{
		ssp.iSubItem = phdr->iItem;
		ssp.bAscending = TRUE;
	}

	m_ctrlSkillList.SortItems(SortSkills, reinterpret_cast<LPARAM>(&ssp));

	pos = m_ctrlSkillList.GetFirstSelectedItemPosition();
	if(pos)
	{
		m_wCurSel = m_ctrlSkillList.GetNextSelectedItem(pos);
		m_wCurSkill = (WORD)(m_ctrlSkillList.GetItemData(m_wCurSel));
		m_ctrlSkillList.EnsureVisible(m_wCurSel, FALSE);
	}
	else
	{
		m_wCurSel = -1;
		m_wCurSkill = -1;
	}
}

void CSkillDlg::SetSkill()
{
	SkillListEntry * pSkill = NULL;
	CString	szText;

	if(!rom.m_bOpened || m_wCurSkill >= SKILL_COUNT)
		return;

	pSkill = rom.GetSkillListEntry(m_wCurSkill);

	// 属性
	m_ctrlTypeList.SetCurSel(pSkill->bType);

	// 威力
	szText.Format(_T("%lu"), (DWORD)(pSkill->bPower));
	SetDlgItemText(IDC_SKL_POWER, szText);

	// 命中
	szText.Format(_T("%lu"), (DWORD)(pSkill->bAccuracy));
	SetDlgItemText(IDC_SKL_ACC, szText);

	// ＰＰ
	szText.Format(_T("%lu"), (DWORD)(pSkill->bPP));
	SetDlgItemText(IDC_SKL_PP, szText);

	// 效果
	szText.Format(_T("0x%02lX"), (DWORD)(pSkill->bEffect));
	SetDlgItemText(IDC_SKL_EFFECT, szText);

	// 几率
	szText.Format(_T("%lu"), (DWORD)(pSkill->bEffectRatio));
	SetDlgItemText(IDC_SKL_EFFECT_RATIO, szText);

	// 对象
	m_ctrlTargetList.SetCurSel(GetTargetTypeIndex(pSkill->bTargetType));

	// 优先度
	szText.Format(_T("%li"), (LONG)(pSkill->nPriority));
	SetDlgItemText(IDC_SKL_PRIORITY, szText);

	// 近
	CheckDlgButton(IDC_SKL_MELEE, (pSkill->dwFlags & skl_melee) ? BST_CHECKED : BST_UNCHECKED);

	// 守
	CheckDlgButton(IDC_SKL_PROTECT, (pSkill->dwFlags & skl_protect) ? BST_CHECKED : BST_UNCHECKED);

	// 替
	CheckDlgButton(IDC_SKL_SUBSTITUTE, (pSkill->dwFlags & skl_substitute) ? BST_CHECKED : BST_UNCHECKED);

	// ?0
	CheckDlgButton(IDC_SKL_UNK0, (pSkill->dwFlags & skl_unk0) ? BST_CHECKED : BST_UNCHECKED);

	// ?1
	CheckDlgButton(IDC_SKL_UNK1, (pSkill->dwFlags & skl_unk1) ? BST_CHECKED : BST_UNCHECKED);

	// 王
	CheckDlgButton(IDC_SKL_KINGSROCK, (pSkill->dwFlags & skl_kingsrock) ? BST_CHECKED : BST_UNCHECKED);

	// 说明
	SetSkillDesc();
}

void CSkillDlg::GetSkill()
{
	SkillListEntry * pSkill = NULL;
	CString	szText;
	BYTE	bNum;

	if(!rom.m_bOpened || m_wCurSkill >= SKILL_COUNT)
		return;

	pSkill = rom.GetSkillListEntry(m_wCurSkill);

	// 属性
	bNum = m_ctrlTypeList.GetCurSel();
	if(bNum < dwTypesCount)
	{
		pSkill->bType = bNum;
	}

	// 威力
	GetDlgItemText(IDC_SKL_POWER, szText);
	pSkill->bPower = (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

	// 命中
	GetDlgItemText(IDC_SKL_ACC, szText);
	pSkill->bAccuracy = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));

	// ＰＰ
	GetDlgItemText(IDC_SKL_PP, szText);
	pSkill->bPP = (BYTE)(min(_tcstoul(szText, 0, 0), 159));

	// 效果
	GetDlgItemText(IDC_SKL_EFFECT, szText);
	pSkill->bEffect = (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

	// 几率
	GetDlgItemText(IDC_SKL_EFFECT_RATIO, szText);
	pSkill->bEffectRatio = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));

	// 对象
	bNum = m_ctrlTargetList.GetCurSel();
	if(bNum < dwSkillTargetTypesCount)
	{
		bNum = (BYTE)(m_ctrlTargetList.GetItemData(bNum));
		pSkill->bTargetType = bNum;
	}

	// 优先度
	GetDlgItemText(IDC_SKL_PRIORITY, szText);
	pSkill->nPriority = (CHAR)(max(min(_tcstol(szText, 0, 0), CHAR_MAX), CHAR_MIN));

	// 近
	if(IsDlgButtonChecked(IDC_SKL_MELEE) == BST_CHECKED)
		pSkill->dwFlags |= skl_melee;
	else
		pSkill->dwFlags &= ~skl_melee;

	// 守
	if(IsDlgButtonChecked(IDC_SKL_PROTECT) == BST_CHECKED)
		pSkill->dwFlags |= skl_protect;
	else
		pSkill->dwFlags &= ~skl_protect;

	// 替
	if(IsDlgButtonChecked(IDC_SKL_SUBSTITUTE) == BST_CHECKED)
		pSkill->dwFlags |= skl_substitute;
	else
		pSkill->dwFlags &= ~skl_substitute;

	// ?0
	if(IsDlgButtonChecked(IDC_SKL_UNK0) == BST_CHECKED)
		pSkill->dwFlags |= skl_unk0;
	else
		pSkill->dwFlags &= ~skl_unk0;

	// ?1
	if(IsDlgButtonChecked(IDC_SKL_UNK1) == BST_CHECKED)
		pSkill->dwFlags |= skl_unk1;
	else
		pSkill->dwFlags &= ~skl_unk1;

	// 王
	if(IsDlgButtonChecked(IDC_SKL_KINGSROCK) == BST_CHECKED)
		pSkill->dwFlags |= skl_kingsrock;
	else
		pSkill->dwFlags &= ~skl_kingsrock;

	SetSkillListItem(m_wCurSel);
}

void CSkillDlg::OnLvnItemchangedSklSkillList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if((pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED)) &&
		pNMLV->iItem != m_wCurSel)
	{
		if(m_wCurSel < SKILL_COUNT)
			GetSkill();

		m_wCurSel = pNMLV->iItem;

		if(m_wCurSel < SKILL_COUNT)
		{
			m_wCurSkill = (WORD)(m_ctrlSkillList.GetItemData(m_wCurSel));
			SetSkill();
		}
		else
		{
			m_wCurSkill = -1;
		}
	}
}

void CSkillDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!rom.ReadSkillList())
	{
		AfxMessageBox(IDS_ERR_ROM_READ);
	}
	else
	{
		OnConfigChanged(TRUE, FALSE);
		SetSkill();
	}
}

void CSkillDlg::OnBnClickedConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	GetSkill();

	if(!rom.SaveSkillList())
	{
		AfxMessageBox(IDS_ERR_ROM_WRITE);
	}
}
