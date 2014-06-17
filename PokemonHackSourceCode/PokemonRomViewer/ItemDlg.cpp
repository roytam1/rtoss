// ItemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "ItemDlg.h"

// CItemDlg 对话框

IMPLEMENT_DYNAMIC(CItemDlg, CBaseDialog)
CItemDlg::CItemDlg()
{
	m_uTemplateId = CItemDlg::IDD;
	pItem = NULL;
	m_wCurItem = -1;
}

CItemDlg::~CItemDlg()
{
}

void CItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ITM_ITEM_LIST, m_ctrlItemList);
	DDX_Control(pDX, IDC_CONFIRM, m_ctrlConfirm);
}


BEGIN_MESSAGE_MAP(CItemDlg, CBaseDialog)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONFIRM, OnBnClickedConfirm)
	ON_LBN_SELCHANGE(IDC_ITM_ITEM_LIST, OnLbnSelchangeItmItemList)
END_MESSAGE_MAP()


// CItemDlg 消息处理程序
void CItemDlg::SetItemList()
{
	if(!rom.m_bOpened)
		return;

	WORD	wItemCount;
	WORD	wItem, wCurItem;
	CString	szIndex, szText;
	LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };

	m_ctrlConfirm.EnableWindow(!rom.m_bIsFileReadOnly);

	wItemCount = rom.GetItemCount();
	wCurItem = m_ctrlItemList.GetCurSel();
	if(wCurItem >= wItemCount)
		wCurItem = 0;
	m_ctrlItemList.ResetContent();
	for(wItem = 0; wItem < wItemCount; ++wItem)
	{
		szIndex.Format(szFmt33[cfg.dwCount], wItem);
		szText = szIndex + cfg.pItemNameList[wItem].rgszText[cfg.dwLang];
		m_ctrlItemList.AddString(szText);
	}
	m_ctrlItemList.SetCurSel(wCurItem);
}

void CItemDlg::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	SetItemList();
	SetDesc();

	if(bRomChanged)
	{
		m_ctrlConfirm.EnableWindow(!rom.m_bIsFileReadOnly);

		SetPrice();
	}
}

void CItemDlg::SetPrice()
{
	if(!rom.m_bOpened)
		return;

	WORD	wItemCount;
	CString	szText;

	wItemCount = rom.GetItemCount();
	m_wCurItem = m_ctrlItemList.GetCurSel();

	if(m_wCurItem < wItemCount)
	{
		pItem = rom.GetItemListEntry(m_wCurItem);
		if(pItem)
		{
			szText.Format(_T("%hu"), pItem->wPrice);
			SetDlgItemText(IDC_ITM_PRICE, szText);
		}
	}
}

void CItemDlg::GetPrice()
{
	if(!rom.m_bOpened)
		return;

	WORD	wItemCount;
	CString	szText;

	wItemCount = rom.GetItemCount();
	if(m_wCurItem < wItemCount)
	{
		pItem = rom.GetItemListEntry(m_wCurItem);
		if(pItem)
		{
			GetDlgItemText(IDC_ITM_PRICE, szText);
			pItem->wPrice = (WORD)(min(_tcstoul(szText, 0, 0), 0xFFFF));
		}
	}
}

void CItemDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!rom.ReadItemList())
	{
		AfxMessageBox(IDS_ERR_ROM_READ);
	}
	else
	{
		SetPrice();
	}
}

void CItemDlg::OnBnClickedConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	GetPrice();

	if(!rom.SaveItemList())
	{
		AfxMessageBox(IDS_ERR_ROM_WRITE);
	}
}

void CItemDlg::OnLbnSelchangeItmItemList()
{
	// TODO: 在此添加控件通知处理程序代码
	GetPrice();
	SetPrice();
	SetDesc();
}

void CItemDlg::SetDesc()
{
	if(!rom.m_bOpened)
		return;

	WORD	wItemCount;
	CString	szText;

	wItemCount = rom.GetItemCount();
	m_wCurItem = m_ctrlItemList.GetCurSel();

	if(m_wCurItem < wItemCount)
	{
		SetDlgItemText(IDC_ITM_DESC, cfg.pItemDescList[m_wCurItem].rgszText[cfg.dwLang]);
	}
}
