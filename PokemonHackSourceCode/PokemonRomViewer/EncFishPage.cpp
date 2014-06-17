// EncFishPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "EncFishPage.h"


// CEncFishPage 对话框

IMPLEMENT_DYNAMIC(CEncFishPage, CEncTabPage)
CEncFishPage::CEncFishPage()
{
	m_uTemplateId = CEncFishPage::IDD;
}

CEncFishPage::~CEncFishPage()
{
}

void CEncFishPage::DoDataExchange(CDataExchange* pDX)
{
	DWORD	dwIndex;

	CEncTabPage::DoDataExchange(pDX);

	for(dwIndex = 0; dwIndex < ENC_FISH_ENTRY_COUNT; ++dwIndex)
	{
		DDX_Control(pDX, IDC_ENC_BREED_LIST0 + dwIndex, m_ctrlBrdList[dwIndex]);
		DDX_Control(pDX, IDC_LAND_IMAGE0 + dwIndex, m_ctrlImage[dwIndex]);
		DDX_Control(pDX, IDC_ENC_SET_BREED0 + dwIndex, m_ctrlSetBrd[dwIndex]);
	}
}

BEGIN_MESSAGE_MAP(CEncFishPage, CEncTabPage)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_ENC_BREED_LIST0, IDC_ENC_BREED_LIST11, OnCbnSelchangeEncBreedList)
	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_ENC_BREED_LIST0, IDC_ENC_BREED_LIST11, OnSetFocus)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_ENC_SET_BREED0, IDC_ENC_SET_BREED11, OnBnClickedEncSetBreed)
	ON_CONTROL_RANGE(BN_SETFOCUS, IDC_ENC_SET_BREED0, IDC_ENC_SET_BREED11, OnSetFocus)

	ON_CONTROL_RANGE(BN_SETFOCUS, IDC_MIN_LEVEL0, IDC_MIN_LEVEL11, OnSetFocus)
	ON_CONTROL_RANGE(BN_SETFOCUS, IDC_MAX_LEVEL0, IDC_MIN_LEVEL11, OnSetFocus)

	ON_CONTROL_RANGE(STN_DBLCLK, IDC_LAND_IMAGE0, IDC_LAND_IMAGE11, OnDblClickedIcon)
END_MESSAGE_MAP()

// CEncFishPage 消息处理程序
void CEncFishPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	static BOOL bInitialized = FALSE;

	if(!rom.m_bOpened)
		return;

	if(bUILangChanged || !bInitialized)
	{
		DWORD	dwIndex;
		WORD	wBreed, wCurBreed;
		CString	szIndex, szText;
		LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };

		///////////////////////////////////////////////////////////////////
		for(dwIndex = 0; dwIndex < ENC_FISH_ENTRY_COUNT; ++dwIndex)
		{
			wCurBreed = m_ctrlBrdList[dwIndex].GetCurSel();
			m_ctrlBrdList[dwIndex].ResetContent();
			for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
			{
				szIndex.Format(szFmt33[cfg.dwCount], wBreed);
				szText = szIndex + cfg.pBreedNameList[wBreed].rgszText[cfg.dwLang];
				m_ctrlBrdList[dwIndex].AddString(szText);
			}
			if(wCurBreed < BREED_COUNT)
				m_ctrlBrdList[dwIndex].SetCurSel(wCurBreed);
		}

		bInitialized = TRUE;
	}

	if(bRomChanged)
		TransData(TRUE);
}

void CEncFishPage::TransData(BOOL bToControls)
{
	m_dwFocusedEntry = -1;

	GetCurrentEncLists();
	if(m_pFishList == NULL)
		return;

	DWORD	dwIndex;
	EncEntry *pEntry = NULL;
	CString	szText;

	if(bToControls)
	{
		SetDlgItemInt(IDC_ENC_PARAM, m_pFishList->dwEncCode, FALSE);

		for(dwIndex = 0; dwIndex < ENC_FISH_ENTRY_COUNT; ++dwIndex)
		{
			pEntry = &(m_pFishList->rgEntry[dwIndex]);

			m_ctrlBrdList[dwIndex].SetCurSel(pEntry->wBreed);
			SetBrdImage(dwIndex);

			SetDlgItemInt(IDC_MIN_LEVEL0 + dwIndex, pEntry->bMinLevel, FALSE);
			SetDlgItemInt(IDC_MAX_LEVEL0 + dwIndex, pEntry->bMaxLevel, FALSE);
		}
	}
	else
	{
		GetDlgItemText(IDC_ENC_PARAM, szText);
		m_pFishList->dwEncCode = _tcstoul(szText, 0, 0);

		for(dwIndex = 0; dwIndex < ENC_FISH_ENTRY_COUNT; ++dwIndex)
		{
			pEntry = &(m_pFishList->rgEntry[dwIndex]);

			GetDlgItemText(IDC_MIN_LEVEL0 + dwIndex, szText);
			pEntry->bMinLevel = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));

			GetDlgItemText(IDC_MAX_LEVEL0 + dwIndex, szText);
			pEntry->bMaxLevel = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));

			if(pEntry->bMaxLevel < pEntry->bMinLevel)
				pEntry->bMaxLevel = pEntry->bMinLevel;
		}
	}
}

void CEncFishPage::SetBrdImage(DWORD dwIndex)
{
	WORD		wBreed;

	if(m_pFishList && dwIndex < ENC_FISH_ENTRY_COUNT)
	{
		wBreed = m_ctrlBrdList[dwIndex].GetCurSel();
		if(wBreed == 0 || wBreed >= BREED_COUNT)
		{
			m_ctrlImage[dwIndex].ShowWindow(SW_HIDE);
		}
		else
		{
			m_ctrlImage[dwIndex].SetBitmap(rom.GetSmallIcon(wBreed));
			m_ctrlImage[dwIndex].ShowWindow(SW_SHOW);
		}
	}
}

void CEncFishPage::GetBreed(DWORD dwIndex)
{
	WORD	wBreed;
	EncEntry *pEntry = NULL;

	if(m_pFishList && dwIndex < ENC_FISH_ENTRY_COUNT)
	{
		pEntry = &(m_pFishList->rgEntry[dwIndex]);
		wBreed = m_ctrlBrdList[dwIndex].GetCurSel();
		if(wBreed < BREED_COUNT && wBreed != pEntry->wBreed)
		{
			pEntry->wBreed = wBreed;
			SetBrdImage(dwIndex);
		}
	}
}

void CEncFishPage::SetBreed(DWORD dwIndex)
{
	if(	m_pFishList &&
		dwIndex < ENC_FISH_ENTRY_COUNT &&
		theApp.m_dlgBreed.m_wCurBreed < BREED_COUNT)
	{
		m_pFishList->rgEntry[dwIndex].wBreed = theApp.m_dlgBreed.m_wCurBreed;
		m_ctrlBrdList[dwIndex].SetCurSel(theApp.m_dlgBreed.m_wCurBreed);
	}
}

void CEncFishPage::OnCbnSelchangeEncBreedList(UINT uID)
{
	GetBreed(uID - IDC_ENC_BREED_LIST0);
}

void CEncFishPage::OnBnClickedEncSetBreed(UINT uID)
{
	SetBreed(uID - IDC_ENC_SET_BREED0);
	SetBrdImage(uID - IDC_ENC_SET_BREED0);
}

void CEncFishPage::OnSetFocus(UINT uID)
{
	DWORD	dwEntryIndex = -1;

	if(uID >= IDC_ENC_BREED_LIST0 && uID <= IDC_ENC_BREED_LIST11)
	{
		dwEntryIndex = uID - IDC_ENC_BREED_LIST0;
	}
	else if(uID >= IDC_ENC_SET_BREED0 && uID <= IDC_ENC_SET_BREED11)
	{
		dwEntryIndex = uID - IDC_ENC_SET_BREED0;
	}
	else if(uID >= IDC_MIN_LEVEL0 && uID <= IDC_MIN_LEVEL11)
	{
		dwEntryIndex = uID - IDC_MIN_LEVEL0;
	}
	else if(uID >= IDC_MAX_LEVEL0 && uID <= IDC_MAX_LEVEL11)
	{
		dwEntryIndex = uID - IDC_MAX_LEVEL0;
	}

	if(dwEntryIndex >= ENC_FISH_ENTRY_COUNT)
		m_dwFocusedEntry = -1;
	else
		m_dwFocusedEntry = dwEntryIndex;
}

void CEncFishPage::SetFocusedEntry(DWORD dwEncEntryIndex)
{
	if(dwEncEntryIndex < ENC_FISH_ENTRY_COUNT)
		m_ctrlBrdList[dwEncEntryIndex].SetFocus();
}

void CEncFishPage::OnDblClickedIcon(UINT uID)
{
	DWORD	dwEntryIndex = uID - IDC_LAND_IMAGE0;
	WORD	wBreed;

	if(dwEntryIndex < ENC_FISH_ENTRY_COUNT)
	{
		wBreed = m_pFishList->rgEntry[dwEntryIndex].wBreed;
		theApp.m_dlgBreed.SetCurPm(wBreed);
	}
}
