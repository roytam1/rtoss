// EvoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "EvoDlg.h"
#include ".\evodlg.h"

// CEvoDlg 对话框

IMPLEMENT_DYNAMIC(CEvoDlg, CBaseDialog)
CEvoDlg::CEvoDlg()
{
	m_uTemplateId = CEvoDlg::IDD;
	m_pEvo = NULL;
	m_wCurSel = -1;
	m_wCurBreed = -1;
	m_dwCurOrder = pm_order_internal;
}

CEvoDlg::~CEvoDlg()
{
}

void CEvoDlg::DoDataExchange(CDataExchange* pDX)
{
	DWORD	dwIndex;

	CBaseDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PM_LIST, m_ctrlPmList);
	DDX_Control(pDX, IDC_CONFIRM, m_ctrlConfirm);

	for(dwIndex = 0; dwIndex < EVO_LIST_COUNT; ++dwIndex)
	{
		DDX_Control(pDX, IDC_EVO_COND_LIST0  + dwIndex, m_ctrlEvoCondList[dwIndex]);
		DDX_Control(pDX, IDC_EVO_PARAM_DESC0 + dwIndex, m_ctrlEvoParamDesc[dwIndex]);
		DDX_Control(pDX, IDC_EVO_PARAM0      + dwIndex, m_ctrlEvoParam[dwIndex]);
		DDX_Control(pDX, IDC_EVO_ITEM_LIST0  + dwIndex, m_ctrlEvoItemList[dwIndex]);
		DDX_Control(pDX, IDC_EVO_BRD_LIST0   + dwIndex, m_ctrlEvoBreedList[dwIndex]);
		DDX_Control(pDX, IDC_EVO_ICON0       + dwIndex, m_ctrlEvoBreedIcon[dwIndex]);
	}
}

BEGIN_MESSAGE_MAP(CEvoDlg, CBaseDialog)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_CONFIRM, OnBnClickedConfirm)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PM_LIST, OnLvnItemchangedPmList)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_EVO_BRD_LIST0, IDC_EVO_BRD_LIST4, OnCbnSelchangeEvoBrdList)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_EVO_COND_LIST0, IDC_EVO_COND_LIST4, OnCbnSelchangeEvoCondList)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EVO_SET_BRD0, IDC_EVO_SET_BRD4, OnBnClickedEvoSetBrd)
	ON_NOTIFY(LVN_GETINFOTIP, IDC_PM_LIST, OnLvnGetInfoTipPmList)
	ON_BN_CLICKED(IDC_FROM_BRD, OnBnClickedFromBrd)
	ON_BN_CLICKED(IDC_REVERSE_FIND, OnBnClickedReverseFind)
	ON_CONTROL_RANGE(STN_DBLCLK, IDC_EVO_ICON0, IDC_EVO_ICON4, OnStnDblclickIcon)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_ORDER_INTERNAL, IDC_ORDER_HOUEN, OnChangeOrder)
END_MESSAGE_MAP()


// CEvoDlg 消息处理程序
void CEvoDlg::GetCurrentEvo()
{
	if(!rom.m_bOpened || m_wCurBreed >= BREED_COUNT)
		m_pEvo = NULL;
	else
		m_pEvo = rom.GetEvoListEntry(m_wCurBreed, 0);
}

void CEvoDlg::RefreshPmList(BOOL bRomChanged)
{
	BOOL	bResult;
	WORD	wIndex, wBreed;
	DWORD	dwIcon;
	WORD	wCount = m_ctrlPmList.GetItemCount();
	LVITEM	lvi;
	CString	szIndex, szText;
	LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };
	WORD *	rgwOrderList = NULL;	// i-th element: the breed id of the i-th item of pm list

	if(!rom.m_bOpened)
		return;

	///////////////////////////////////////////////////////////////////
	// build order list if rom is changed
	if(bRomChanged)
	{
		if(m_dwCurOrder != pm_order_internal)
		{
			rgwOrderList = new WORD[BREED_COUNT];
			if(!rgwOrderList)
				return;
		}

		switch(m_dwCurOrder)
		{
		case pm_order_kanto:
			for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
				rgwOrderList[rom.m_pPokedexKantoOrder[wBreed]] = wBreed;
			break;

		case pm_order_houen:
			for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
				rgwOrderList[rom.m_pPokedexHouenOrder[wBreed]] = wBreed;
			break;
		}
	}

	///////////////////////////////////////////////////////////////////
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iSubItem = 0;

	for(wIndex = 0; wIndex < BREED_COUNT; ++wIndex)
	{
		lvi.iItem = wIndex;

		if(rgwOrderList)
			wBreed = rgwOrderList[wIndex];
		else if(m_dwCurOrder == pm_order_internal)
			wBreed = wIndex;
		else
			wBreed = (WORD)(m_ctrlPmList.GetItemData(wIndex));

		dwIcon = rom.GetSmallIconCount(wBreed);
		if(dwIcon > 0)
			dwIcon = rom.GetSmallIconIndex(wBreed, rand() % dwIcon);
		lvi.iImage = dwIcon;

		szIndex.Format(szFmt33[cfg.dwCount], wIndex);
		szText = szIndex + cfg.pBreedNameList[wBreed].rgszText[cfg.dwLang];
		lvi.pszText = szText.GetBuffer();
		if(wBreed < wCount)
			bResult = m_ctrlPmList.SetItem(&lvi);
		else
			m_ctrlPmList.InsertItem(&lvi);
		szText.ReleaseBuffer();

		if(bRomChanged)
			m_ctrlPmList.SetItemData(wIndex, wBreed);
	}

	if(rgwOrderList)
		delete []rgwOrderList;

	if(m_wCurBreed < BREED_COUNT)
	{
		SetCurPm(m_wCurBreed);
	}
}

void CEvoDlg::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	static BOOL bInitialized = FALSE;

	if(!rom.m_bOpened)
		return;

	///////////////////////////////////////////////////////////////////
	// refresh image list for pm list
	if(bRomChanged)
		m_ctrlPmList.SetImageList(rom.m_pSmallIconList, LVSIL_NORMAL);

	///////////////////////////////////////////////////////////////////
	// refresh pm list & item lists
	RefreshPmList(bRomChanged);
	SetItemList();

	if(bUILangChanged || !bInitialized)
	{
		DWORD	dwIndex;
		DWORD	dwEvoIndex, dwCurCond;
		WORD	wBreed, wCurBreed;
		WORD	wCount = m_ctrlPmList.GetItemCount();
		CString	szIndex, szText;
		LPCTSTR	szFmt22[3] = { _T(""), _T("%-2lu:"), _T("%02lX:") };
		LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };

		///////////////////////////////////////////////////////////////////
		// setup evo-condition lists
		if(!bInitialized)
		{
			for(dwIndex = 0; dwIndex < EVO_LIST_COUNT; ++dwIndex)
			{
				dwCurCond = m_ctrlEvoCondList[dwIndex].GetCurSel();
				m_ctrlEvoCondList[dwIndex].ResetContent();
				for(dwEvoIndex = 0; dwEvoIndex < dwEvoConditionsCount; ++dwEvoIndex)
				{
					szIndex.Format(szFmt22[cfg.dwCount], dwEvoIndex);
					szText = szIndex + rgEvoConditions[dwEvoIndex].szCondition;
					m_ctrlEvoCondList[dwIndex].AddString(szText);
				}
				if(dwCurCond < dwEvoConditionsCount)
					m_ctrlEvoCondList[dwIndex].SetCurSel(dwCurCond);
			}
		}

		///////////////////////////////////////////////////////////////////
		// refresh breed lists
		for(dwIndex = 0; dwIndex < EVO_LIST_COUNT; ++dwIndex)
		{
			////////////////////////////////////////
			wCurBreed = m_ctrlEvoBreedList[dwIndex].GetCurSel();
			m_ctrlEvoBreedList[dwIndex].ResetContent();
			for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
			{
				szIndex.Format(szFmt33[cfg.dwCount], wBreed);
				szText = szIndex + cfg.pBreedNameList[wBreed].rgszText[cfg.dwLang];
				m_ctrlEvoBreedList[dwIndex].AddString(szText);
			}
			if(wCurBreed < BREED_COUNT)
				m_ctrlEvoBreedList[dwIndex].SetCurSel(wCurBreed);
		}

		bInitialized = TRUE;
	}

	if(bRomChanged)
	{
		m_ctrlConfirm.EnableWindow(!rom.m_bIsFileReadOnly);

		GetCurrentEvo();

		if(m_wCurBreed >= BREED_COUNT)
			m_ctrlPmList.SetItemState(0, -1, LVIS_SELECTED | LVIS_FOCUSED);
		else
			SetAllParams();
	}
}

void CEvoDlg::SetItemList()
{
	DWORD	dwIndex;
	WORD	wItem, wCurItem, wItemCount;
	CString	szIndex, szText;
	LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };

	if(!rom.m_bOpened)
		return;

	for(dwIndex = 0; dwIndex < EVO_LIST_COUNT; ++dwIndex)
	{
		wItemCount = rom.GetItemCount();
		wCurItem = m_ctrlEvoItemList[dwIndex].GetCurSel();
		m_ctrlEvoItemList[dwIndex].ResetContent();
		for(wItem = 0; wItem < wItemCount; ++wItem)
		{
			szIndex.Format(szFmt33[cfg.dwCount], wItem);
			szText = szIndex + cfg.pItemNameList[wItem].rgszText[cfg.dwLang];
			m_ctrlEvoItemList[dwIndex].AddString(szText);
		}
		if(wCurItem < wItemCount)
			m_ctrlEvoItemList[dwIndex].SetCurSel(wCurItem);
	}
}

void CEvoDlg::OnBnClickedRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!rom.ReadEvoList())
	{
		AfxMessageBox(IDS_ERR_ROM_READ);
		return;
	}

	SetAllParams();
}

void CEvoDlg::OnBnClickedConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	GetAllParams();

	if(!rom.SaveEvoList())
	{
		AfxMessageBox(IDS_ERR_ROM_WRITE);
		return;
	}
}

void CEvoDlg::OnLvnItemchangedPmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if(pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED))
	{
		GetAllParams();
		m_wCurSel = pNMLV->iItem;
		if(m_wCurSel < BREED_COUNT)
		{
			m_wCurBreed = (WORD)(m_ctrlPmList.GetItemData(m_wCurSel));
			GetCurrentEvo();
			SetAllParams();
		}
		else
		{
			m_wCurBreed = -1;
		}
	}
}

BOOL CEvoDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlPmList.SetIconSpacing(SMALL_ICON_BIT_WIDTH2 + 28, SMALL_ICON_BIT_HEIGHT2 + 24);
	m_ctrlPmList.SetExtendedStyle(LVS_EX_BORDERSELECT | LVS_EX_INFOTIP);

	CheckDlgButton(IDC_ORDER_INTERNAL + m_dwCurOrder, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CEvoDlg::GetParam(DWORD dwIndex)
{
	EvoConditions *	pEvoCond = NULL;
	CString	szText;
	WORD	wBreed;
	WORD	wItem, wItemCount;

	if(m_pEvo == NULL || dwIndex >= EVO_LIST_COUNT)
		return;

	wBreed = m_ctrlEvoBreedList[dwIndex].GetCurSel();
	if(wBreed < BREED_COUNT)
		m_pEvo[dwIndex].wBreed = wBreed;

	if(wBreed == 0 || wBreed >= BREED_COUNT)
		m_pEvo[dwIndex].wCondition = 0;

	pEvoCond = &(rgEvoConditions[m_pEvo[dwIndex].wCondition]);
	switch(pEvoCond->dwParamType)
	{
	case evo_none:
		m_pEvo[dwIndex].wValue = 0;
		break;

	case evo_level:
		GetDlgItemText(IDC_EVO_PARAM0 + dwIndex, szText);
		m_pEvo[dwIndex].wValue = (WORD)(min(_tcstoul(szText, 0, 0), 0x64));
		break;

	case evo_beauty:
		GetDlgItemText(IDC_EVO_PARAM0 + dwIndex, szText);
		m_pEvo[dwIndex].wValue = (WORD)(min(_tcstoul(szText, 0, 0), 0xFF));
		break;

	case evo_item:
		wItemCount = rom.GetItemCount();
		wItem = m_ctrlEvoItemList[dwIndex].GetCurSel();
		if(wItem < wItemCount)
			m_pEvo[dwIndex].wValue = wItem;
		break;
	}
}

void CEvoDlg::GetAllParams()
{
	DWORD	dwIndex;

	for(dwIndex = 0; dwIndex < EVO_LIST_COUNT; ++dwIndex)
	{
		GetParam(dwIndex);
	}
}

void CEvoDlg::SetAllParams()
{
	DWORD	dwIndex;

	if(m_pEvo == NULL)
		return;

	for(dwIndex = 0; dwIndex < EVO_LIST_COUNT; ++dwIndex)
	{
		SetParam(dwIndex);
	}
}

void CEvoDlg::SetParam(DWORD dwIndex, BOOL bForceUpdate)
{
	if(m_pEvo == NULL)
		return;

	if(dwIndex >= EVO_LIST_COUNT)
		return;

	WORD	wEvoCond;
	WORD	wItem, wItemCount;
	EvoConditions *	pEvoCond = NULL;
	INT		iItemListShow, iParamShow;
	BOOL	bBreedListEnable;

	wEvoCond = m_pEvo[dwIndex].wCondition < dwEvoConditionsCount ? m_pEvo[dwIndex].wCondition : 0;
	if(wEvoCond == 0)
		bBreedListEnable = FALSE;
	else
		bBreedListEnable = TRUE;

	pEvoCond = &(rgEvoConditions[wEvoCond]);
	switch(pEvoCond->dwParamType)
	{
	case evo_level:
		iParamShow = SW_SHOW;
		iItemListShow = SW_HIDE;
		SetDlgItemInt(IDC_EVO_PARAM0 + dwIndex, min(m_pEvo[dwIndex].wValue, 0x64), FALSE);
		break;

	case evo_beauty:
		iParamShow = SW_SHOW;
		iItemListShow = SW_HIDE;
		SetDlgItemInt(IDC_EVO_PARAM0 + dwIndex, min(m_pEvo[dwIndex].wValue, 0xFF), FALSE);
		break;

	case evo_item:
		iParamShow = SW_HIDE;
		iItemListShow = SW_SHOW;
		wItemCount = rom.GetItemCount();
		wItem = min(m_pEvo[dwIndex].wValue, wItemCount - 1);
		m_ctrlEvoItemList[dwIndex].SetCurSel(wItem);
		break;

	case evo_none:
	default:
		iParamShow = SW_HIDE;
		iItemListShow = SW_HIDE;
		break;
	}

	if(bForceUpdate || m_ctrlEvoCondList[dwIndex].GetCurSel() != wEvoCond)
	{
		m_ctrlEvoCondList[dwIndex].SetCurSel(wEvoCond);
		m_ctrlEvoParamDesc[dwIndex].SetWindowText(pEvoCond->szParamDesc);
		m_ctrlEvoParam[dwIndex].ShowWindow(iParamShow);
		m_ctrlEvoItemList[dwIndex].ShowWindow(iItemListShow);
		m_ctrlEvoBreedList[dwIndex].EnableWindow(bBreedListEnable);
	}

	if(m_ctrlEvoBreedList[dwIndex].GetCurSel() != m_pEvo[dwIndex].wBreed)
	{
		m_ctrlEvoBreedList[dwIndex].SetCurSel(bBreedListEnable ? m_pEvo[dwIndex].wBreed : 0);
		SetBreedIcon(dwIndex);
	}
}

void CEvoDlg::SetBreedIcon(DWORD dwIndex)
{
	WORD	wBreed;
	DWORD	dwIcon;

	if(dwIndex >= EVO_LIST_COUNT)
		return;

	wBreed = m_ctrlEvoBreedList[dwIndex].GetCurSel();
	if(wBreed >= BREED_COUNT)
		wBreed = 0;

	if(wBreed > 0)
	{
		dwIcon = rom.GetSmallIconCount(wBreed);
		if(dwIcon > 0)
			dwIcon = rom.GetSmallIconIndex(wBreed, rand() % dwIcon);

		m_ctrlEvoBreedIcon[dwIndex].ShowWindow(SW_SHOW);
		m_ctrlEvoBreedIcon[dwIndex].SetBitmap(rom.m_rghbmSmallIcons[dwIcon]);
	}
	else
	{
		m_ctrlEvoBreedIcon[dwIndex].ShowWindow(SW_HIDE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CEvoDlg::GetBreed(DWORD dwIndex)
{
	WORD	wBreed;

	if(m_pEvo == NULL || dwIndex >= EVO_LIST_COUNT)
		return FALSE;

	wBreed = m_ctrlEvoBreedList[dwIndex].GetCurSel();
	if(wBreed >= BREED_COUNT)
		return FALSE;

	m_pEvo[dwIndex].wBreed = wBreed;
	return TRUE;
}

void CEvoDlg::OnCbnSelchangeEvoBrdList(UINT uID)
{
	// TODO: 在此添加控件通知处理程序代码
	if(GetBreed(uID - IDC_EVO_BRD_LIST0))
		SetBreedIcon(uID - IDC_EVO_BRD_LIST0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CEvoDlg::OnCondChanged(DWORD dwIndex)
{
	if(m_pEvo && dwIndex < EVO_LIST_COUNT)
	{
		GetParam(dwIndex);
		m_pEvo[dwIndex].wCondition = m_ctrlEvoCondList[dwIndex].GetCurSel();
		SetParam(dwIndex, TRUE);
	}
}

void CEvoDlg::OnCbnSelchangeEvoCondList(UINT uID)
{
	// TODO: 在此添加控件通知处理程序代码
	OnCondChanged(uID - IDC_EVO_COND_LIST0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CEvoDlg::OnSetBreed(DWORD dwIndex)
{
	if(theApp.m_dlgBreed.m_wCurBreed < BREED_COUNT &&
		m_pEvo && m_pEvo[dwIndex].wCondition != 0)
	{
		m_ctrlEvoBreedList[dwIndex].SetCurSel(theApp.m_dlgBreed.m_wCurBreed);
		GetBreed(dwIndex);
		SetBreedIcon(dwIndex);
	}
}

void CEvoDlg::OnBnClickedEvoSetBrd(UINT uID)
{
	// TODO: 在此添加控件通知处理程序代码
	OnSetBreed(uID - IDC_EVO_SET_BRD0);
}

void CEvoDlg::OnLvnGetInfoTipPmList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	WORD	wBreed;
	WORD	wBreed387, wIndexKanto, wIndexHouen;
	WORD	wPage, wRow, wColumn;
	WORD	wPageKanto, wRowKanto, wColumnKanto;
	WORD	wPageHouen, wRowHouen, wColumnHouen;

	if(rom.m_bOpened &&
		pGetInfoTip->cchTextMax >= 100 &&
		pGetInfoTip->dwFlags == LVGIT_UNFOLDED &&
		pGetInfoTip->iItem <= BREED_COUNT)
	{
		wBreed = (WORD)(m_ctrlPmList.GetItemData(pGetInfoTip->iItem));
		wBreed387 = ConvertBreed(wBreed, FALSE);
		GetPositionInBox(wBreed387, wPage, wRow, wColumn);

		wIndexKanto = rom.m_pPokedexKantoOrder[wBreed];
		GetPositionInBox(wIndexKanto - 1, wPageKanto, wRowKanto, wColumnKanto);

		wIndexHouen = rom.m_pPokedexHouenOrder[wBreed];
		GetPositionInBox(wIndexHouen - 1, wPageHouen, wRowHouen, wColumnHouen);

		wsprintf(
			pGetInfoTip->pszText,
			_T("%3hu|%2hu-%hu-%hu(K)\n")
			_T("%3hu|%2hu-%hu-%hu(H)\n")
			_T("%3hu|%03hXh\n")
			_T("%hu-%hu-%hu"),
			wIndexKanto, wPageKanto, wRowKanto, wColumnKanto,
			wIndexHouen, wPageHouen, wRowHouen, wColumnHouen,
			wBreed, wBreed, wPage, wRow, wColumn);
	}
}

void CEvoDlg::OnBnClickedFromBrd()
{
	// TODO: 在此添加控件通知处理程序代码
	WORD	wBreed = theApp.m_dlgBreed.m_wCurBreed;
	if(wBreed < BREED_COUNT)
	{
		SetCurPm(wBreed);
	}
}

void CEvoDlg::OnBnClickedReverseFind()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE	bEvoIndex;
	WORD	wBreed;
	EvoListEntry *pEvo = NULL;

	if(rom.m_bOpened && m_wCurBreed > 0 && m_wCurBreed < BREED_COUNT)
	{
		for(wBreed = 0; wBreed < BREED_COUNT; ++wBreed)
		{
			for(bEvoIndex = 0; bEvoIndex < EVO_LIST_COUNT; ++bEvoIndex)
			{
				pEvo = rom.GetEvoListEntry(wBreed, bEvoIndex);
				if(pEvo->wBreed == m_wCurBreed)
				{
					SetCurPm(wBreed);
					break;
				}
			}
		}
	}
}

void CEvoDlg::OnStnDblclickIcon(UINT uID)
{
	DWORD	dwIndex = uID - IDC_EVO_ICON0;
	WORD	wBreed;

	if(m_pEvo && dwIndex < EVO_LIST_COUNT)
	{
		wBreed = m_pEvo[dwIndex].wBreed;
		SetCurPm(wBreed);
	}
}

void CEvoDlg::SetCurPm(WORD wBreed)
{
	if(rom.m_bOpened && wBreed < BREED_COUNT)
	{
		switch(m_dwCurOrder)
		{
		case pm_order_internal:
		default:
			m_wCurSel = wBreed;
			break;

		case pm_order_kanto:
			m_wCurSel = rom.m_pPokedexKantoOrder[wBreed];
			break;

		case pm_order_houen:
			m_wCurSel = rom.m_pPokedexHouenOrder[wBreed];
			break;
		}

		// m_wCurBreed = wBreed;
		m_ctrlPmList.SetItemState(m_wCurSel, -1, LVIS_SELECTED | LVIS_FOCUSED);
		m_ctrlPmList.EnsureVisible(m_wCurSel, FALSE);
	}
}

INT CALLBACK CEvoDlg::SortPokemons(LPARAM lParam1, LPARAM lParam2, LPARAM lSortParam)
{
	INT		iResult = 0;

	if(rom.m_bOpened)
	{
		switch(lSortParam)
		{
		case pm_order_internal:
			iResult = INT(lParam1 - lParam2);
			break;

		case pm_order_kanto:
			iResult = INT(rom.m_pPokedexKantoOrder[lParam1] - rom.m_pPokedexKantoOrder[lParam2]);
			break;

		case pm_order_houen:
			iResult = INT(rom.m_pPokedexHouenOrder[lParam1] - rom.m_pPokedexHouenOrder[lParam2]);
			break;
		}
	}

	return iResult;
}

void CEvoDlg::OnChangeOrder(UINT uID)
{
	if(!rom.m_bOpened || uID < IDC_ORDER_INTERNAL || uID > IDC_ORDER_HOUEN)
		return;

	POSITION	pos;

	if(m_dwCurOrder != uID - IDC_ORDER_INTERNAL)
	{
		m_dwCurOrder = uID - IDC_ORDER_INTERNAL;
		//m_ctrlPmList.SortItems(SortPokemons, (LPARAM)(m_dwCurOrder));
		RefreshPmList(TRUE);

		pos = m_ctrlPmList.GetFirstSelectedItemPosition();
		if(pos)
		{
			m_wCurSel = m_ctrlPmList.GetNextSelectedItem(pos);
			m_wCurBreed = (WORD)(m_ctrlPmList.GetItemData(m_wCurSel));
			m_ctrlPmList.EnsureVisible(m_wCurSel, FALSE);
		}
		else
		{
			m_wCurSel = -1;
			m_wCurBreed = -1;
		}
	}
}
