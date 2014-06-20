// SkillComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "PokemonMemHack.h"
#include "SkillComboBox.h"


// CSkillComboBox

IMPLEMENT_DYNAMIC(CSkillComboBox, CComboBox)

CSkillComboBox::CSkillComboBox()
{

}

CSkillComboBox::~CSkillComboBox()
{
}


BEGIN_MESSAGE_MAP(CSkillComboBox, CComboBox)
END_MESSAGE_MAP()


void CSkillComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_COMBOBOX);
	LPCTSTR	lpszText = _T("¤¢j");

	CSize	Size;
	CDC *	pDC = GetDC();

	Size = pDC->GetTextExtent(lpszText, (int)(_tcslen(lpszText)));
	ReleaseDC(pDC);

	lpMeasureItemStruct->itemHeight = Size.cy << 1;
}

void CSkillComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_COMBOBOX);
	ASSERT(g_MemRom.m_bOpened);

	COLORREF	rgDefClrTable[] =
	{
		RGB(255,255,255),	// Bg
		RGB(0,0,0),		// Fg
	};

	WORD	wSkill = (WORD)(lpDrawItemStruct->itemData);
	if(wSkill >= SKILL_COUNT)
		return;

	SkillListEntry * pSkill = g_MemRom.GetSkillListEntry(wSkill);
	if(!pSkill)
		return;

	CString	szText;
	LPCTSTR	szFmt3[3] = { _T(""), _T("%-3lu: "), _T("%03lX: ") };

	CRect	rect = lpDrawItemStruct->rcItem;
	CDC		dc;
	COLORREF	clrForeOld, clrBackOld;

	dc.Attach(lpDrawItemStruct->hDC);

	// set text color
	clrForeOld = dc.SetTextColor(pSkill->bType < g_dwForeClrCount ? g_rgForeClrTable[pSkill->bType] : rgDefClrTable[1]);
	clrBackOld = dc.SetBkColor(pSkill->bType < g_dwBackClrCount ? g_rgBackClrTable[pSkill->bType] : rgDefClrTable[0]);

	// draw rectangle
	dc.FillSolidRect(&rect, pSkill->bType < g_dwBackClrCount ? g_rgBackClrTable[pSkill->bType] : rgDefClrTable[0]);
	if(lpDrawItemStruct->itemState & ODS_SELECTED)
		dc.DrawEdge(&rect, EDGE_SUNKEN, BF_ADJUST | BF_RECT);
	else
		dc.DrawEdge(&rect, BDR_RAISEDINNER, BF_ADJUST | BF_RECT);

	// draw text
	szText.Format(szFmt3[cfg.dwCount], wSkill);
	szText += cfg.pSkillNameList[wSkill].rgszText[cfg.dwLang];
	dc.DrawText(szText,
				&(lpDrawItemStruct->rcItem),
				DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	// clean up
	dc.SetTextColor(clrForeOld);
	dc.SetBkColor(clrBackOld);
	dc.Detach();
}

int CSkillComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	ASSERT(lpCompareItemStruct->CtlType == ODT_COMBOBOX);
	ASSERT(g_MemRom.m_bOpened);

	WORD	wSkill1 = (WORD)(lpCompareItemStruct->itemData1);
	WORD	wSkill2 = (WORD)(lpCompareItemStruct->itemData2);

	if(wSkill1 == wSkill2)
		return 0;

	SkillListEntry * pSkill1 = g_MemRom.GetSkillListEntry(wSkill1);
	SkillListEntry * pSkill2 = g_MemRom.GetSkillListEntry(wSkill2);
	if(!pSkill1 || !pSkill2)
		return (wSkill1 - wSkill2);

	if(pSkill1->bType != pSkill2->bType)
		return (pSkill1->bType - pSkill2->bType);
	else if(pSkill1->bPower != pSkill2->bPower)
		return (pSkill2->bPower - pSkill1->bPower);
	else
		return (wSkill1 - wSkill2);
}

// CSkillComboBox message handlers
