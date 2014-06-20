// BreedItemPage.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BreedItemPage.h"


// CBreedItemPage 对话框

IMPLEMENT_DYNAMIC(CBreedItemPage, CBreedTabPage)
CBreedItemPage::CBreedItemPage()
{
	m_uTemplateId = CBreedItemPage::IDD;
}

CBreedItemPage::~CBreedItemPage()
{
}

BOOL CBreedItemPage::OnInitDialog()
{
	CBreedTabPage::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_Font.CreateFont(
	   GUI_FONT_HEIGHT,           // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_NORMAL,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   ANSI_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   _T(GUI_FONT_NAME));        // lpszFacename 

	m_Static = (CStatic *) GetDlgItem(IDC_BRD_ITEM_DESC0);
	m_Static->SetFont(&m_Font);
	m_Static = (CStatic *) GetDlgItem(IDC_BRD_ITEM_DESC1);
	m_Static->SetFont(&m_Font);
	m_ListBox = (CListBox *) GetDlgItem(IDC_BRD_ITEM_LIST0);
	m_ListBox->SetFont(&m_Font);
	m_ListBox = (CListBox *) GetDlgItem(IDC_BRD_ITEM_LIST1);
	m_ListBox->SetFont(&m_Font);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBreedItemPage::DoDataExchange(CDataExchange* pDX)
{
	CBreedTabPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BRD_ITEM_LIST0, m_ctrlItemList[0]);
	DDX_Control(pDX, IDC_BRD_ITEM_LIST1, m_ctrlItemList[1]);
}


BEGIN_MESSAGE_MAP(CBreedItemPage, CBreedTabPage)
	ON_CONTROL_RANGE(LBN_SELCHANGE, IDC_BRD_ITEM_LIST0, IDC_BRD_ITEM_LIST1, SetItemDesc)
END_MESSAGE_MAP()


// CBreedItemPage 消息处理程序
void CBreedItemPage::OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged)
{
	DWORD	dwIndex;
	WORD	wItem, wCurItem, wItemCount;
	CString	szIndex, szText;
	LPCTSTR	szFmt33[3] = { _T(""), _T("%-3hu:"), _T("%03hX:") };

	if(!rom.m_bOpened)
		return;

	///////////////////////////////////////////////////////////////////
	// refresh item lists
	wItemCount = rom.GetItemCount();
	for(dwIndex = 0; dwIndex < 2; ++dwIndex)
	{
		wCurItem = m_ctrlItemList[dwIndex].GetCurSel();
		if(wCurItem >= wItemCount)
			wCurItem = 0;
		m_ctrlItemList[dwIndex].ResetContent();
		for(wItem = 0; wItem < wItemCount; ++wItem)
		{
			szIndex.Format(szFmt33[cfg.dwCount], wItem);
			szText = szIndex + cfg.pItemNameList[wItem].rgszText[cfg.dwLang];
			m_ctrlItemList[dwIndex].AddString(szText);
		}
		wCurItem = m_ctrlItemList[dwIndex].SetCurSel(wCurItem);

		SetItemDesc(IDC_BRD_ITEM_LIST0 + dwIndex);
	}

	if(bRomChanged)
		TransData(TRUE);
}

void CBreedItemPage::TransData(BOOL bToControls)
{
	GetCurrentBreed();
	if(m_pBreed == NULL)
		return;

	WORD	wItemCount = rom.GetItemCount();
	WORD	wItem;

	if(bToControls)
	{
		m_ctrlItemList[0].SetCurSel(m_pBreed->wDrop1);
		m_ctrlItemList[1].SetCurSel(m_pBreed->wDrop2);
		SetItemDesc(IDC_BRD_ITEM_LIST0);
		SetItemDesc(IDC_BRD_ITEM_LIST1);
	}
	else
	{
		wItem = m_ctrlItemList[0].GetCurSel();
		if(wItem < wItemCount)
			m_pBreed->wDrop1 = wItem;

		wItem = m_ctrlItemList[1].GetCurSel();
		if(wItem < wItemCount)
			m_pBreed->wDrop2 = wItem;
	}
}

void CBreedItemPage::SetItemDesc(UINT uID)
{
	if(!rom.m_bOpened)
		return;

	WORD	wItem, wItemCount = rom.GetItemCount();
	DWORD	dwIndex = uID - IDC_BRD_ITEM_LIST0;

	if(dwIndex < 2)
	{
		wItem = m_ctrlItemList[dwIndex].GetCurSel();
		if(wItem < wItemCount)
		{
			SetDlgItemText(IDC_BRD_ITEM_DESC0 + dwIndex, cfg.pItemDescList[wItem].rgszText[cfg.dwLang]);
		}
	}
}
