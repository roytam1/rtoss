// BreedSpecPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedSpecPage.h"

// CBreedSpecPage 对话框

IMPLEMENT_DYNAMIC(CBreedSpecPage, CBreedTabPage)
CBreedSpecPage::CBreedSpecPage()
{
	m_uTemplateId = CBreedSpecPage::IDD;
}

CBreedSpecPage::~CBreedSpecPage()
{
}

void CBreedSpecPage::DoDataExchange(CDataExchange* pDX)
{
	CBreedTabPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BRD_EXP_TYPE_LIST, m_ctrlExpTypeList);
	DDX_Control(pDX, IDC_BRD_SPEC_LIST0, m_ctrlSpecList[0]);
	DDX_Control(pDX, IDC_BRD_SPEC_LIST1, m_ctrlSpecList[1]);
}


BEGIN_MESSAGE_MAP(CBreedSpecPage, CBreedTabPage)
	ON_CBN_SELCHANGE(IDC_BRD_SPEC_LIST0, OnCbnSelchangeBrdSpecList0)
	ON_CBN_SELCHANGE(IDC_BRD_SPEC_LIST1, OnCbnSelchangeBrdSpecList1)
END_MESSAGE_MAP()


// CBreedSpecPage 消息处理程序
void CBreedSpecPage::SetSpecDesc(DWORD dwIndex)
{
	if(dwIndex >= 2)
		return;

	BYTE	bSpec = m_ctrlSpecList[dwIndex].GetCurSel();
	if(bSpec >= SPEC_COUNT)
		return;

	SetDlgItemText(IDC_BRD_SPEC_DESC0 + dwIndex, cfg.pSpecDescList[bSpec].rgszText[cfg.dwLang]);
}

void CBreedSpecPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	static BOOL bInitialized = FALSE;

	if(!rom.m_bOpened)
		return;

	if(bUILangChanged || !bInitialized)
	{
		DWORD	dwIndex;
		BYTE	bSpec, bCurSpec;
		CString	szIndex, szText;
		LPCTSTR	szFmt22[3] = { _T(""), _T("%2hu:"), _T("%02hX:") };

		///////////////////////////////////////////////////////////////////
		for(dwIndex = 0; dwIndex < 2; ++dwIndex)
		{
			////////////////////////////////////////
			bCurSpec = m_ctrlSpecList[dwIndex].GetCurSel();
			if(bCurSpec >= SPEC_COUNT)
				bCurSpec = 0;
			m_ctrlSpecList[dwIndex].ResetContent();
			for(bSpec = 0; bSpec < SPEC_COUNT; ++bSpec)
			{
				szIndex.Format(szFmt22[cfg.dwCount], (WORD)(bSpec));
				szText = szIndex + cfg.pSpecNameList[bSpec].rgszText[cfg.dwLang];
				m_ctrlSpecList[dwIndex].AddString(szText);
			}
			m_ctrlSpecList[dwIndex].SetCurSel(bCurSpec);

			SetSpecDesc(dwIndex);
		}

		bInitialized = TRUE;
	}

	if(bRomChanged)
	{
		BYTE	bExpType;
		DWORD *	pdwExp = NULL;
		CString	szText;

		///////////////////////////////////////////////////////////////////
		// refresh exp type lists
		m_ctrlExpTypeList.ResetContent();
		for(bExpType = 0; bExpType < EXP_LIST_COUNT; ++bExpType)
		{
			pdwExp = rom.GetExpList(bExpType);
			if(pdwExp == NULL)
				break;

			szText.Format(_T("%lu: %lu"), (DWORD)(bExpType), pdwExp[EXP_COUNT - 1]);
			m_ctrlExpTypeList.AddString(szText);
		}

		///////////////////////////////////////////////////////////////////
		TransData(TRUE);
	}
}

void CBreedSpecPage::TransData(BOOL bToControls)
{
	GetCurrentBreed();
	if(m_pBreed == NULL)
		return;

	BYTE	bExpType;
	BYTE	bSpec;
	CString	szText;

	if(bToControls)
	{
		SetDlgItemInt(IDC_BRD_CATCH_RATIO,   m_pBreed->bCatchRatio,   FALSE);
		SetDlgItemInt(IDC_BRD_ESCAPE_RATIO,  m_pBreed->bEscapeRatio,  FALSE);
		SetDlgItemInt(IDC_BRD_FEMALE_RATIO,  m_pBreed->bFemaleRatio,  FALSE);
		SetDlgItemInt(IDC_BRD_HATCH_TIME,    m_pBreed->bHatchTime,    FALSE);
		SetDlgItemInt(IDC_BRD_INIT_INTIMATE, m_pBreed->bInitIntimate, FALSE);
		SetDlgItemInt(IDC_BRD_BASE_EXP,      m_pBreed->bBaseExp,      FALSE);
		SetDlgItemInt(IDC_BRD_UNK,           m_pBreed->unk0,          FALSE);

		m_ctrlExpTypeList.SetCurSel(m_pBreed->bExpType);

		m_ctrlSpecList[0].SetCurSel(m_pBreed->bSpecialty1);
		SetSpecDesc(0);
		m_ctrlSpecList[1].SetCurSel(m_pBreed->bSpecialty2);
		SetSpecDesc(1);
	}
	else
	{
		GetDlgItemText(IDC_BRD_CATCH_RATIO,   szText);
		m_pBreed->bCatchRatio =   (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

		GetDlgItemText(IDC_BRD_ESCAPE_RATIO,  szText);
		m_pBreed->bEscapeRatio =  (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

		GetDlgItemText(IDC_BRD_FEMALE_RATIO,  szText);
		m_pBreed->bFemaleRatio =  (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

		GetDlgItemText(IDC_BRD_HATCH_TIME,    szText);
		m_pBreed->bHatchTime =    (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

		GetDlgItemText(IDC_BRD_INIT_INTIMATE, szText);
		m_pBreed->bInitIntimate = (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

		GetDlgItemText(IDC_BRD_BASE_EXP,      szText);
		m_pBreed->bBaseExp =      (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

		GetDlgItemText(IDC_BRD_UNK,           szText);
		m_pBreed->unk0 =          (BYTE)(min(_tcstoul(szText, 0, 0), 0xFF));

		bExpType = m_ctrlExpTypeList.GetCurSel();
		if(bExpType < EXP_LIST_COUNT)
			m_pBreed->bExpType = bExpType;

		bSpec = m_ctrlSpecList[0].GetCurSel();
		if(bSpec < SPEC_COUNT)
			m_pBreed->bSpecialty1 = bSpec;

		bSpec = m_ctrlSpecList[1].GetCurSel();
		if(bSpec < SPEC_COUNT)
			m_pBreed->bSpecialty2 = bSpec;
	}
}

void CBreedSpecPage::OnCbnSelchangeBrdSpecList0()
{
	// TODO: 在此添加控件通知处理程序代码
	SetSpecDesc(0);
}

void CBreedSpecPage::OnCbnSelchangeBrdSpecList1()
{
	// TODO: 在此添加控件通知处理程序代码
	SetSpecDesc(1);
}
