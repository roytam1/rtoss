// MemMiscPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "MemMiscPage0.h"

// CMemMiscPage0 对话框

IMPLEMENT_DYNAMIC(CMemMiscPage0, CPropertyPage)
CMemMiscPage0::CMemMiscPage0()
	: CPropertyPage(CMemMiscPage0::IDD)
{
	m_bEnableText = FALSE;
}

CMemMiscPage0::~CMemMiscPage0()
{
}

void CMemMiscPage0::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYER_NAME, m_ctrlTrainerName);
}


BEGIN_MESSAGE_MAP(CMemMiscPage0, CPropertyPage)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONFIRM, OnBnClickedConfirm)
	ON_BN_CLICKED(IDC_ENABLE_TEXT, OnBnClickedEnableText)
END_MESSAGE_MAP()


// CMemMiscPage0 消息处理程序
void CMemMiscPage0::OnChangeUILang(VOID)
{
}

void CMemMiscPage0::OnBnClickedRefresh()
{
	BOOL	bResult;
	DWORD	dwNum;
	WORD	wNum;
	CString	szText;

	bResult = SetPlayerName();

	if(bResult)
	{
		bResult = g_MemHack.GetPlayerId(dwNum);
		if(bResult)
		{
			szText.Format(_T("0x%08lX"), dwNum);
			SetDlgItemText(IDC_PLAYER_ID, szText);
		}
		else
		{
			SetDlgItemText(IDC_PLAYER_ID, _T(""));
		}
	}

	if(bResult)
	{
		bResult = g_MemHack.GetMoney(dwNum);
		if(bResult)
			SetDlgItemInt(IDC_MONEY, dwNum, FALSE);
		else
			SetDlgItemText(IDC_MONEY, _T(""));
	}

	if(bResult)
	{
		bResult = g_MemHack.GetCoin(wNum);
		if(bResult)
			SetDlgItemInt(IDC_COIN, wNum, FALSE);
		else
			SetDlgItemText(IDC_COIN, _T(""));
	}

	if(bResult)
	{
		wNum = 0;
		bResult = g_MemHack.GetDust(wNum);
		if(bResult)
			SetDlgItemInt(IDC_DUST, wNum, FALSE);
		else
			SetDlgItemText(IDC_DUST, _T(""));
	}

	if(bResult)
	{
		bResult = g_MemHack.GetSprayTime(wNum);
		if(bResult)
			SetDlgItemInt(IDC_SPRAY_TIME, wNum, FALSE);
		else
			SetDlgItemText(IDC_SPRAY_TIME, _T(""));
	}

	if(bResult)
	{
		bResult = g_MemHack.GetSafariTime(wNum);
		if(bResult)
			SetDlgItemInt(IDC_SAFARI_TIME, wNum, FALSE);
		else
			SetDlgItemText(IDC_SAFARI_TIME, _T(""));
	}

	if(bResult)
	{
		wNum = 0;
		bResult = g_MemHack.GetBattlePointsCurrent(wNum);
		if(bResult)
			SetDlgItemInt(IDC_BP_CURRENT, wNum, FALSE);
	}

	if(bResult)
	{
		wNum = 0;
		bResult = g_MemHack.GetBattlePointsTrainerCard(wNum);
		if(bResult)
			SetDlgItemInt(IDC_BP_TRAINERCARD, wNum, FALSE);
	}

	if(!bResult)
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

void CMemMiscPage0::OnBnClickedConfirm()
{
	BOOL	bResult = TRUE;
	DWORD	dwNum;
	CString	szText;

	if(IsDlgButtonChecked(IDC_ENABLE_TEXT) == BST_CHECKED)
	{
		GetDlgItemText(IDC_PLAYER_NAME, szText);
		bResult = g_MemHack.SetPlayerName(szText);
	}

	if(bResult)
	{
		GetDlgItemText(IDC_PLAYER_ID, szText);
		dwNum = _tcstoul(szText, 0, 0);
		bResult = g_MemHack.SetPlayerId(dwNum);
	}

	if(bResult)
	{
		GetDlgItemText(IDC_MONEY, szText);
		dwNum = _tcstoul(szText, 0, 0);
		bResult = g_MemHack.SetMoney(dwNum);
	}

	if(bResult)
	{
		GetDlgItemText(IDC_COIN, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0xFFFF);
		bResult = g_MemHack.SetCoin((WORD)(dwNum));
	}

	if(bResult)
	{
		GetDlgItemText(IDC_DUST, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0xFFFF);
		bResult = g_MemHack.SetDust((WORD)(dwNum));
	}

	if(bResult)
	{
		GetDlgItemText(IDC_SPRAY_TIME, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0xFFFF);
		bResult = g_MemHack.SetSprayTime((WORD)(dwNum));
	}

	if(bResult)
	{
		GetDlgItemText(IDC_SAFARI_TIME, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0xFFFF);
		bResult = g_MemHack.SetSafariTime((WORD)(dwNum));
	}

	if(bResult)
	{
		GetDlgItemText(IDC_BP_CURRENT, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0xFFFF);
		bResult = g_MemHack.SetBattlePointsCurrent((WORD)(dwNum));
	}

	if(bResult)
	{
		GetDlgItemText(IDC_BP_TRAINERCARD, szText);
		dwNum = min(_tcstoul(szText, 0, 0), 0xFFFF);
		bResult = g_MemHack.SetBattlePointsTrainerCard((WORD)(dwNum));
	}

	if(!bResult)
	{
		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}

BOOL CMemMiscPage0::SetPlayerName(VOID)
{
	BOOL	bResult = TRUE;
	CString	szText;

	if(IsDlgButtonChecked(IDC_ENABLE_TEXT) == BST_CHECKED)
	{
		bResult = g_MemHack.GetPlayerName(szText);
		if(bResult)
		{
			m_ctrlTrainerName.EnableWindow(TRUE);
			m_ctrlTrainerName.SetWindowText(szText);
		}
	}
	else
	{
		m_ctrlTrainerName.EnableWindow(FALSE);
		m_ctrlTrainerName.SetWindowText(_T(""));
	}

	return bResult;
}

void CMemMiscPage0::OnBnClickedEnableText()
{
	if(!SetPlayerName())
	{
		CheckDlgButton(IDC_ENABLE_TEXT, BST_UNCHECKED);
//		AfxMessageBox(IDS_ERR_MEM_ACCESS, MB_OK | MB_ICONERROR);
	}
}
