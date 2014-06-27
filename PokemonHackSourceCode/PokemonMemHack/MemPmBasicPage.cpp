// MemPmBasicPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemPmBasicPage.h"

// CMemPmBasicPage 对话框

IMPLEMENT_DYNAMIC(CMemPmBasicPage, CMemPmTabPage)
CMemPmBasicPage::CMemPmBasicPage()
{
	m_uTemplateId = CMemPmBasicPage::IDD;
	m_ppc = NULL;
	m_wPreBreed = -1;
}

CMemPmBasicPage::~CMemPmBasicPage()
{
}

void CMemPmBasicPage::DoDataExchange(CDataExchange* pDX)
{
	CMemPmTabPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BREED_LIST, m_ctrlBreedList);
	DDX_Control(pDX, IDC_PS_LIST, m_ctrlPsList);
	DDX_Control(pDX, IDC_SPEC_LIST, m_ctrlSpecList);
	DDX_Control(pDX, IDC_ITEM_LIST, m_ctrlItemList);
	DDX_Control(pDX, IDC_PS_DESC, m_ctrlPsDesc);
	DDX_Control(pDX, IDC_NICK_NAME, m_ctrlNickName);
	DDX_Control(pDX, IDC_CATCHER_NAME, m_ctrlCatcherName);
	DDX_Control(pDX, IDC_SEX_LIST, m_ctrlSexList);
	DDX_Control(pDX, IDC_LEVEL_LIST, m_ctrlLevelList);
}

BEGIN_MESSAGE_MAP(CMemPmBasicPage, CMemPmTabPage)
	ON_CBN_SELCHANGE(IDC_PS_LIST, GetPs)
	ON_BN_CLICKED(IDC_SHINY, GetShiny)
	ON_EN_CHANGE(IDC_CHAR, GetChar)
	ON_EN_CHANGE(IDC_ID, GetId)
	ON_CBN_SELCHANGE(IDC_BREED_LIST, GetBreed)
	ON_LBN_SELCHANGE(IDC_SPEC_LIST, GetSpec)
	ON_CBN_SELCHANGE(IDC_SEX_LIST, GetSex)
	ON_EN_CHANGE(IDC_EXP, GetExp)
	ON_LBN_SELCHANGE(IDC_LEVEL_LIST, GetLevel)
	ON_CBN_SELCHANGE(IDC_ITEM_LIST, GetItem)
	ON_BN_CLICKED(IDC_EGG, OnBnClickedEgg)
	ON_BN_CLICKED(IDC_ENABLE_TEXT, OnBnClickedEnableText)
	ON_BN_CLICKED(IDC_OBEDIENCE, OnBnClickedObedience)
	ON_STN_DBLCLK(IDC_CHAR_TITLE, OnStnDblclickCharTitle)
	ON_STN_DBLCLK(IDC_ID_TITLE, OnStnDblclickIdTitle)
END_MESSAGE_MAP()


// CMemPmBasicPage 消息处理程序
BOOL CMemPmBasicPage::OnInitDialog()
{
	CMemPmTabPage::OnInitDialog();

	m_ctrlBreedList.SetExtendedStyle(0, CBES_EX_NOSIZELIMIT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CMemPmBasicPage::TransData(BOOL bToControls)
{
	DWORD	dwNum;
	CString	szText;
	WORD	wBreed;

	m_ppc = GetPokemonCodec();
	if(m_ppc == NULL)
		return;

	if(bToControls)
	{
		////////////////////////////////////////////////
		m_ctrlBreedList.SetImageList(g_MemRom.m_pTinyIconList);
		COMBOBOXEXITEM	cbei;
		cbei.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;

		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			cbei.iItem = wBreed;
			cbei.iImage = g_MemRom.GetSmallIconIndex(wBreed, rand() % g_MemRom.GetSmallIconCount(wBreed));
			cbei.iSelectedImage = cbei.iImage;
			m_ctrlBreedList.SetItem(&cbei);
		}

		OnBnClickedEnableText();

		SetBreed();

		szText.Format(_T("0x%08lX"), m_ppc->GetChar());
		SetDlgItemText(IDC_CHAR, szText);

		szText.Format(_T("0x%08lX"), m_ppc->GetID());
		SetDlgItemText(IDC_ID, szText);

		SetExp();

		SetDlgItemInt(IDC_INTIMATE, m_ppc->GetIntimate(), FALSE);

		SetPs();

		m_ctrlItemList.SetCurSel(m_ppc->GetItem());
		SetItemDesc();

		CheckDlgButton(IDC_EGG, m_ppc->GetIsEgg() ? BST_CHECKED : BST_UNCHECKED);

		CheckDlgButton(IDC_SHINY, m_ppc->GetIsShiny() ? BST_CHECKED : BST_UNCHECKED);

		CheckDlgButton(IDC_OBEDIENCE, m_ppc->GetObedience() ? BST_CHECKED : BST_UNCHECKED);
	}
	else
	{
		GetDlgItemText(IDC_INTIMATE, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0xFF);
		m_ppc->SetIntimate((BYTE)(dwNum));

		if(IsDlgButtonChecked(IDC_ENABLE_TEXT) == BST_CHECKED)
		{
			GetDlgItemText(IDC_NICK_NAME, szText);
			m_ppc->SetNickName(szText);

			GetDlgItemText(IDC_CATCHER_NAME, szText);
			m_ppc->SetCatcherName(szText);
		}
	}
}

void CMemPmBasicPage::OnChangeUILang(VOID)
{
	DWORD	dwIndex, dwCount, dwCurSel;
	CString	szText;
	COMBOBOXEXITEM	cbei;
	LPCTSTR	szFmt2[3] = { _T(""), _T("%-2lu: "), _T("%02lX: ") };
	LPCTSTR	szFmt3[3] = { _T(""), _T("%-3lu: "), _T("%03lX: ") };

	////////////////////////////////////////////////
	dwCurSel = m_ctrlBreedList.GetCurSel();
	SetSpecList();

	cbei.mask = CBEIF_TEXT;
	dwCount = m_ctrlBreedList.GetCount();
	for(dwIndex = 0; dwIndex < BREED_COUNT; ++dwIndex)
	{
		cbei.iItem = dwIndex;
		szText.Format(szFmt3[cfg.dwCount], dwIndex);
		szText += cfg.pBreedNameList[dwIndex].rgszText[cfg.dwLang];
		cbei.pszText = szText.GetBuffer();
		if(dwIndex < dwCount)
			m_ctrlBreedList.SetItem(&cbei);
		else if(dwIndex < BREED_COUNT)
			m_ctrlBreedList.InsertItem(&cbei);
		szText.ReleaseBuffer();
	}
	for(; dwIndex < dwCount; ++dwIndex)
	{
		m_ctrlBreedList.DeleteItem(BREED_COUNT);
	}
	m_ctrlBreedList.SetCurSel(dwCurSel);

	////////////////////////////////////////////////
	dwCurSel = m_ctrlPsList.GetCurSel();
	m_ctrlPsList.ResetContent();
	for(dwIndex = 0; dwIndex < PS_COUNT; ++dwIndex)
	{
		szText.Format(szFmt2[cfg.dwCount], dwIndex);
		szText += cfg.pPsNameList[dwIndex].rgszText[cfg.dwLang];
		m_ctrlPsList.AddString(szText);
	}
	m_ctrlPsList.SetCurSel(dwCurSel);

	////////////////////////////////////////////////
	dwCurSel = m_ctrlItemList.GetCurSel();
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

void CMemPmBasicPage::GetChar()
{
	if(!m_ppc)
		return;

	CString	szText;

	GetDlgItemText(IDC_CHAR, szText);
	m_ppc->SetChar(_tcstoul(szText, 0, 0));

	// ps
	SetPs();

	// sex
	SetSex();

	// shiny
	CheckDlgButton(IDC_SHINY, m_ppc->GetIsShiny() ? BST_CHECKED : BST_UNCHECKED);
}

void CMemPmBasicPage::GetId()
{
	if(!m_ppc)
		return;

	CString	szText;

	GetDlgItemText(IDC_ID, szText);
	m_ppc->SetID(_tcstoul(szText, 0, 0));

	// shiny
	CheckDlgButton(IDC_SHINY, m_ppc->GetIsShiny() ? BST_CHECKED : BST_UNCHECKED);
}

void CMemPmBasicPage::SetPs()
{
	if(!m_ppc)
		return;

	m_ctrlPsList.SetCurSel(m_ppc->GetPersonality());

	// ps desc
	SetPsDesc();
}

void CMemPmBasicPage::SetPsDesc()
{
	if(!m_ppc)
		return;

	BYTE	bPs = m_ppc->GetPersonality();
	CString	szDesc;

	CHAR	b[5] = { 1, 1, 1, 1, 1 };
	b[bPs / 5] += 1;
	b[bPs % 5] -= 1;
	static LPCTSTR sz[3] = { _T("－"), _T("　"), _T("＋") };

	szDesc.Format(_T("　　　　攻击：%s防御：%s\n敏捷：%s特攻：%s特防：%s"),
					sz[b[0]], sz[b[1]], sz[b[2]], sz[b[3]], sz[b[4]]);

	m_ctrlPsDesc.SetWindowText(szDesc);
}

void CMemPmBasicPage::SetBreed()
{
	if(!m_ppc)
		return;

	WORD	wBreed;

	wBreed = m_ppc->GetBreed();
	m_ctrlBreedList.SetCurSel(wBreed);

	// spec
	SetSpecList();

	// sex
	SetSexList();

	// exp
	SetLevelList();

	m_wPreBreed = wBreed;
}

void CMemPmBasicPage::GetBreed()
{
	if(!m_ppc)
		return;

	WORD	wBreed;

	wBreed = m_ctrlBreedList.GetCurSel();
	m_ppc->SetBreed(wBreed);

	// spec
	SetSpecList();

	// sex
	SetSexList();

	// exp
	SetLevelList();

	// parent's breed list
	(reinterpret_cast<CMemPmPage *>(GetParent()->GetParent()))->SetPmList(/*bSelectionOnly=*/ TRUE);

	m_wPreBreed = wBreed;
}

void CMemPmBasicPage::GetPs()
{
	if(!m_ppc)
		return;

	CString	szText;
	BYTE	bPs;
	
	bPs = m_ctrlPsList.GetCurSel();
	m_ppc->SetPersonality(bPs);

	// ps desc
	SetPsDesc();

	// char
	szText.Format(_T("0x%08lX"), m_ppc->GetChar());
	SetDlgItemText(IDC_CHAR, szText);

	// id
	szText.Format(_T("0x%08lX"), m_ppc->GetID());
	SetDlgItemText(IDC_ID, szText);
}

void CMemPmBasicPage::SetSpecList()
{
	if(!m_ppc)
		return;

	WORD	wBreed;
	BYTE	bSpec, bIndex;
	BreedListEntry *	pBreed = NULL;

	m_ctrlSpecList.ResetContent();
	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		for(bIndex = 0; bIndex < 2; ++bIndex)
		{
			bSpec = (&(pBreed->bSpecialty1))[bIndex];
			m_ctrlSpecList.AddString(cfg.pSpecNameList[bSpec].rgszText[cfg.dwLang]);
		}
		m_ctrlSpecList.SetCurSel(m_ppc->GetSpecialty());

		SetSpecDesc();
	}
}

void CMemPmBasicPage::SetSpecDesc()
{
	if(!m_ppc)
		return;

	WORD	wBreed;
	BYTE	bSpecSel;
	BYTE	bSpec;
	BreedListEntry *	pBreed = NULL;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		bSpecSel = m_ctrlSpecList.GetCurSel();
		if(bSpecSel < 2)
		{
			bSpec = (&(pBreed->bSpecialty1))[bSpecSel];
			SetDlgItemText(IDC_SPEC_DESC, cfg.pSpecDescList[bSpec].rgszText[cfg.dwLang]);
		}
	}
}

void CMemPmBasicPage::GetSpec()
{
	if(!m_ppc)
		return;

	BYTE	bSpec;
	WORD	wBreed;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		bSpec = m_ctrlSpecList.GetCurSel();
		if(bSpec < 2)
		{
			m_ppc->SetSpecialty(bSpec);
			SetSpecDesc();
		}
	}
}

void CMemPmBasicPage::SetSexList()
{
	if(!m_ppc)
		return;

	WORD	wBreed;
	BYTE	bPreFR, bFR;
	BOOL	bChange = TRUE;
	BreedListEntry *	pPreBreed = NULL;
	BreedListEntry *	pBreed = NULL;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		bFR = pBreed->bFemaleRatio;

		if(m_wPreBreed < BREED_COUNT)
		{
			pPreBreed = g_MemRom.GetBreedListEntry(m_wPreBreed);
			bPreFR = pPreBreed->bFemaleRatio;

			if((bPreFR == bFR) ||
			   (bPreFR > 0 && bPreFR < 0xFE &&
			    bFR > 0 && bFR < 0xFE))
			{
				// no need to change sex list
				bChange = FALSE;
			}
		}

		if(bChange)
		{
			m_ctrlSexList.ResetContent();
			switch(bFR)
			{
			case 0xFF:
				m_ctrlSexList.AddString(_T("不明"));
				break;

			case 0xFE:
				m_ctrlSexList.AddString(_T("全♀"));
				break;

			case 0x00:
				m_ctrlSexList.AddString(_T("全♂"));
				break;

			default:
				m_ctrlSexList.AddString(_T("♀"));
				m_ctrlSexList.AddString(_T("♂"));
				break;
			}
		}

		SetSex();
	}
}

void CMemPmBasicPage::SetSex()
{
	if(!m_ppc)
		return;

	WORD	wBreed;
	BreedListEntry *	pBreed = NULL;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		switch(pBreed->bFemaleRatio)
		{
		case 0xFF:
		case 0xFE:
		case 0x00:
			m_ctrlSexList.SetCurSel(0);
			break;

		default:
			m_ctrlSexList.SetCurSel(m_ppc->GetSex(pBreed->bFemaleRatio));
			break;
		}
	}
}

void CMemPmBasicPage::GetSex()
{
	if(!m_ppc)
		return;

	WORD	wBreed;
	BreedListEntry *	pBreed = NULL;
	BYTE	bSex;
	CString	szText;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		switch(pBreed->bFemaleRatio)
		{
		case 0xFF:
		case 0xFE:
		case 0x00:
			break;

		default:
			bSex = m_ctrlSexList.GetCurSel();
			m_ppc->SetSex(bSex, pBreed->bFemaleRatio);
			break;
		}

		// char
		szText.Format(_T("0x%08lX"), m_ppc->GetChar());
		SetDlgItemText(IDC_CHAR, szText);

		// id
		szText.Format(_T("0x%08lX"), m_ppc->GetID());
		SetDlgItemText(IDC_ID, szText);
	}
}

void CMemPmBasicPage::SetLevelList()
{
	if(!m_ppc)
		return;

	BYTE	bLevel, bExpType;
	WORD	wBreed;
	BOOL	bChange = TRUE;
	CString	szText;
	BreedListEntry *	pBreed = NULL;
	DWORD *				pdwExpList = NULL;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		bExpType = pBreed->bExpType;
		if(m_wPreBreed < BREED_COUNT)
		{
			if(g_MemRom.GetBreedListEntry(m_wPreBreed)->bExpType == bExpType)
			{
				bChange = FALSE;
			}
		}

		if(bChange)
		{
			m_ctrlLevelList.ResetContent();
			pdwExpList = g_MemRom.GetExpList(bExpType);
			if(pdwExpList)
			{
				for(bLevel = 0; bLevel < EXP_COUNT; ++bLevel)
				{
					szText.Format(_T("%-3lu:%lu"), (DWORD)(bLevel), pdwExpList[bLevel]);
					m_ctrlLevelList.AddString(szText);
				}
			}
		}

		SetLevel();
	}
}

void CMemPmBasicPage::SetLevel()
{
	if(!m_ppc)
		return;

	BYTE	bLevel, bExpType;
	WORD	wBreed;
	DWORD	dwExp;
	BreedListEntry *	pBreed = NULL;
	DWORD *				pdwExpList = NULL;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		bExpType = pBreed->bExpType;
		pdwExpList = g_MemRom.GetExpList(bExpType);
		dwExp = m_ppc->GetExp();

		if(pdwExpList)
		{
			for(bLevel = 1; bLevel < EXP_COUNT; ++bLevel)
			{
				if(dwExp < pdwExpList[bLevel])
					break;
			}
			m_ctrlLevelList.SetCurSel(bLevel - 1);
		}
	}
}

void CMemPmBasicPage::GetLevel()
{
	if(!m_ppc)
		return;

	BYTE	bLevel, bExpType;
	WORD	wBreed;
	DWORD	dwExp;
	BreedListEntry *	pBreed = NULL;
	DWORD *				pdwExpList = NULL;

	wBreed = m_ppc->GetBreed();
	if(wBreed < BREED_COUNT)
	{
		pBreed = g_MemRom.GetBreedListEntry(wBreed);
		bExpType = pBreed->bExpType;
		bLevel = m_ctrlLevelList.GetCurSel();
		if(bLevel < EXP_COUNT)
		{
			pdwExpList = g_MemRom.GetExpList(bExpType);
			dwExp = pdwExpList[bLevel];
			m_ppc->SetExp(dwExp);
			SetDlgItemInt(IDC_EXP, dwExp, FALSE);
		}
	}
}

void CMemPmBasicPage::SetExp()
{
	if(!m_ppc)
		return;

	SetDlgItemInt(IDC_EXP, m_ppc->GetExp(), FALSE);

	// level
	SetLevel();
}

void CMemPmBasicPage::GetExp()
{
	if(!m_ppc)
		return;

	DWORD	dwExp;
	CString	szText;

	GetDlgItemText(IDC_EXP, szText);
	dwExp = _tcstoul(szText, 0, 0);
	m_ppc->SetExp(dwExp);

	// level
	SetLevel();
}

void CMemPmBasicPage::GetItem()
{
	if(!m_ppc)
		return;

	WORD	wItem;
	wItem = m_ctrlItemList.GetCurSel();

	if(wItem < ITEM_COUNT)
	{
		m_ppc->SetItem(wItem);
		SetItemDesc();
	}
}

void CMemPmBasicPage::SetItemDesc()
{
	if(!m_ppc)
		return;

	WORD	wItem;

	wItem = m_ppc->GetItem();
	if(wItem < ITEM_COUNT)
		SetDlgItemText(IDC_ITEM_DESC, cfg.pItemDescList[wItem].rgszText[cfg.dwLang]);
}

void CMemPmBasicPage::GetShiny()
{
	DWORD	dwId;
	CString	szText;

	if(IsDlgButtonChecked(IDC_SHINY) == BST_CHECKED)
		dwId = m_ppc->GenShinyID();
	else
		dwId = m_ppc->GenNoShinyID();

	m_ppc->SetID(dwId);
	szText.Format(_T("0x%08lX"), dwId);
	SetDlgItemText(IDC_ID, szText);
}

void CMemPmBasicPage::OnBnClickedEgg()
{
	if(m_ppc)
	{
		if(IsDlgButtonChecked(IDC_EGG) == BST_CHECKED)
			m_ppc->SetIsEgg(1);
		else
			m_ppc->SetIsEgg(0);
	}
}

void CMemPmBasicPage::OnBnClickedEnableText()
{
	CString	szText;

	if(m_ppc)
	{
		if(g_MemHack.GetVersion())
			m_ppc->SetLang(g_MemHack.m_dwLang);

		if(IsDlgButtonChecked(IDC_ENABLE_TEXT) == BST_CHECKED)
		{
			m_ctrlNickName.EnableWindow(TRUE);
			m_ppc->GetNickName(szText);
			SetDlgItemText(IDC_NICK_NAME, szText);

			m_ctrlCatcherName.EnableWindow(TRUE);
			m_ppc->GetCatcherName(szText);
			SetDlgItemText(IDC_CATCHER_NAME, szText);
		}
		else
		{
			m_ctrlNickName.SetWindowText(_T(""));
			m_ctrlNickName.EnableWindow(FALSE);

			m_ctrlCatcherName.SetWindowText(_T(""));
			m_ctrlCatcherName.EnableWindow(FALSE);
		}
	}
	else
	{
		CheckDlgButton(IDC_ENABLE_TEXT, BST_UNCHECKED);
	}
}

void CMemPmBasicPage::OnBnClickedObedience()
{
	if(m_ppc)
	{
		if(IsDlgButtonChecked(IDC_OBEDIENCE) == BST_CHECKED)
			m_ppc->SetObedience(1);
		else
			m_ppc->SetObedience(0);
	}
}

void CMemPmBasicPage::OnStnDblclickCharTitle()
{
	DWORD	dwChar;
	CString	szText;

	if(m_ppc)
	{
		dwChar = GenLongRandom();
		szText.Format(_T("0x%08lX"), dwChar);
		SetDlgItemText(IDC_CHAR, szText);
	}
}

void CMemPmBasicPage::OnStnDblclickIdTitle()
{
	DWORD	dwId;
	CString	szText;

	if(m_ppc)
	{
		dwId = GenLongRandom();
		szText.Format(_T("0x%08lX"), dwId);
		SetDlgItemText(IDC_ID, szText);
	}
}
