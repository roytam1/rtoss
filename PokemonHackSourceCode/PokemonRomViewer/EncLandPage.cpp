// EncLandPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "EncLandPage.h"

// CEncLandPage 对话框

IMPLEMENT_DYNAMIC(CEncLandPage, CEncTabPage)
CEncLandPage::CEncLandPage()
{
	m_uTemplateId = CEncLandPage::IDD;
}

CEncLandPage::~CEncLandPage()
{
}

void CEncLandPage::DoDataExchange(CDataExchange* pDX)
{
	DWORD	dwIndex;

	CEncTabPage::DoDataExchange(pDX);

	for(dwIndex = 0; dwIndex < ENC_LAND_ENTRY_COUNT; ++dwIndex)
	{
		DDX_Control(pDX, IDC_ENC_BREED_LIST0 + dwIndex, m_ctrlBrdList[dwIndex]);
		DDX_Control(pDX, IDC_LAND_IMAGE0 + dwIndex, m_ctrlImage[dwIndex]);
		DDX_Control(pDX, IDC_ENC_SET_BREED0 + dwIndex, m_ctrlSetBrd[dwIndex]);
	}
}

BEGIN_MESSAGE_MAP(CEncLandPage, CEncTabPage)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_ENC_BREED_LIST0, IDC_ENC_BREED_LIST11, OnCbnSelchangeEncBreedList)
	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_ENC_BREED_LIST0, IDC_ENC_BREED_LIST11, OnSetFocus)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_ENC_SET_BREED0, IDC_ENC_SET_BREED11, OnBnClickedEncSetBreed)
	ON_CONTROL_RANGE(BN_SETFOCUS, IDC_ENC_SET_BREED0, IDC_ENC_SET_BREED11, OnSetFocus)

	ON_CONTROL_RANGE(BN_SETFOCUS, IDC_MIN_LEVEL0, IDC_MIN_LEVEL11, OnSetFocus)
	ON_CONTROL_RANGE(BN_SETFOCUS, IDC_MAX_LEVEL0, IDC_MIN_LEVEL11, OnSetFocus)

	ON_CONTROL_RANGE(STN_DBLCLK, IDC_LAND_IMAGE0, IDC_LAND_IMAGE11, OnDblClickedIcon)
END_MESSAGE_MAP()

// CEncLandPage 消息处理程序
void CEncLandPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
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
		// refresh breed lists
		for(dwIndex = 0; dwIndex < ENC_LAND_ENTRY_COUNT; ++dwIndex)
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

void CEncLandPage::TransData(BOOL bToControls)
{
	m_dwFocusedEntry = -1;

	GetCurrentEncLists();
	if(m_pLandList == NULL)
		return;

	DWORD	dwIndex;
	EncEntry *pEntry = NULL;
	CString	szText;

	if(bToControls)
	{
		SetDlgItemInt(IDC_ENC_PARAM, m_pLandList->dwEncCode, FALSE);

		for(dwIndex = 0; dwIndex < ENC_LAND_ENTRY_COUNT; ++dwIndex)
		{
			pEntry = &(m_pLandList->rgEntry[dwIndex]);

			if(m_pAnnoonList)
				if(pEntry->wBreed != 201)
					pEntry->wBreed = 201;

			m_ctrlBrdList[dwIndex].SetCurSel(pEntry->wBreed);
			SetBrdImage(dwIndex);

			SetDlgItemInt(IDC_MIN_LEVEL0 + dwIndex, pEntry->bMinLevel, FALSE);
			SetDlgItemInt(IDC_MAX_LEVEL0 + dwIndex, pEntry->bMaxLevel, FALSE);
		}
	}
	else
	{
		GetDlgItemText(IDC_ENC_PARAM, szText);
		m_pLandList->dwEncCode = _tcstoul(szText, 0, 0);

		for(dwIndex = 0; dwIndex < ENC_LAND_ENTRY_COUNT; ++dwIndex)
		{
			pEntry = &(m_pLandList->rgEntry[dwIndex]);

			GetDlgItemText(IDC_MIN_LEVEL0 + dwIndex, szText);
			pEntry->bMinLevel = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));

			GetDlgItemText(IDC_MAX_LEVEL0 + dwIndex, szText);
			pEntry->bMaxLevel = (BYTE)(min(_tcstoul(szText, 0, 0), 0x64));

			if(pEntry->bMaxLevel < pEntry->bMinLevel)
				pEntry->bMaxLevel = pEntry->bMinLevel;
		}
	}
}

void CEncLandPage::SetBrdImage(DWORD dwIndex)
{
	WORD		wBreed;

	if(m_pLandList && dwIndex < ENC_LAND_ENTRY_COUNT)
	{
		wBreed = m_ctrlBrdList[dwIndex].GetCurSel();
		if(wBreed == 0 || wBreed >= BREED_COUNT)
		{
			m_ctrlImage[dwIndex].ShowWindow(SW_HIDE);
		}
		else
		{
			if(m_pAnnoonList)
			{
				m_ctrlImage[dwIndex].SetBitmap(
					rom.GetSmallIcon(wBreed, m_pAnnoonList[dwIndex] * SMALL_ICON_SUB_LIST_ENTRY_COUNT));
			}
			else
			{
				m_ctrlImage[dwIndex].SetBitmap(rom.GetSmallIcon(wBreed));
			}
			m_ctrlImage[dwIndex].ShowWindow(SW_SHOW);
		}
	}
}

void CEncLandPage::GetBreed(DWORD dwIndex)
{
	WORD	wBreed;
	EncEntry *pEntry = NULL;

	if(m_pLandList && dwIndex < ENC_LAND_ENTRY_COUNT)
	{
		pEntry = &(m_pLandList->rgEntry[dwIndex]);
		if(m_pAnnoonList)
		{
			if(pEntry->wBreed != 201)	// アンノーン
				pEntry->wBreed = 201;

			wBreed = m_ctrlBrdList[dwIndex].GetCurSel();
			if(wBreed < pEntry->wBreed)
				m_pAnnoonList[dwIndex] = (m_pAnnoonList[dwIndex] + ANNOON_COUNT - 1) % ANNOON_COUNT;
			else
				m_pAnnoonList[dwIndex] = (m_pAnnoonList[dwIndex] + 1) % ANNOON_COUNT;

			m_ctrlBrdList[dwIndex].SetCurSel(pEntry->wBreed);
			SetBrdImage(dwIndex);
		}
		else
		{
			wBreed = m_ctrlBrdList[dwIndex].GetCurSel();
			if(wBreed < BREED_COUNT && wBreed != pEntry->wBreed)
			{
				pEntry->wBreed = wBreed;
				SetBrdImage(dwIndex);
			}
		}
	}
}

void CEncLandPage::SetBreed(DWORD dwIndex)
{
	if(	m_pLandList &&
		dwIndex < ENC_LAND_ENTRY_COUNT &&
		theApp.m_dlgBreed.m_wCurBreed < BREED_COUNT)
	{
		m_pLandList->rgEntry[dwIndex].wBreed = theApp.m_dlgBreed.m_wCurBreed;
		m_ctrlBrdList[dwIndex].SetCurSel(theApp.m_dlgBreed.m_wCurBreed);
	}
}

void CEncLandPage::OnCbnSelchangeEncBreedList(UINT uID)
{
	GetBreed(uID - IDC_ENC_BREED_LIST0);
}

void CEncLandPage::OnBnClickedEncSetBreed(UINT uID)
{
	if(!m_pAnnoonList)
	{
		SetBreed(uID - IDC_ENC_SET_BREED0);
		SetBrdImage(uID - IDC_ENC_SET_BREED0);
	}
}

void CEncLandPage::OnSetFocus(UINT uID)
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

	if(dwEntryIndex >= ENC_LAND_ENTRY_COUNT)
		m_dwFocusedEntry = -1;
	else
		m_dwFocusedEntry = dwEntryIndex;
}

void CEncLandPage::SetFocusedEntry(DWORD dwEncEntryIndex)
{
	if(dwEncEntryIndex < ENC_LAND_ENTRY_COUNT)
		m_ctrlBrdList[dwEncEntryIndex].SetFocus();
}

void CEncLandPage::OnDblClickedIcon(UINT uID)
{
	DWORD	dwEntryIndex = uID - IDC_LAND_IMAGE0;
	WORD	wBreed;

	if(dwEntryIndex < ENC_LAND_ENTRY_COUNT)
	{
		wBreed = m_pLandList->rgEntry[dwEntryIndex].wBreed;
		theApp.m_dlgBreed.SetCurPm(wBreed);
	}
}
