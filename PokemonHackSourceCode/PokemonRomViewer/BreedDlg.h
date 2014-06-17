#pragma once
#include "afxcmn.h"

#include "BreedTabPage.h"
#include "BreedTypePage.h"			// 1
#include "BreedAblPage.h"			// 2
#include "BreedSpecPage.h"			// 3
#include "BreedItemPage.h"			// 4
#include "BreedLvlupLearnPage.h"	// 5
#include "BreedDeriveLearnPage.h"	// 6
#include "BreedMachineLearnPage.h"	// 7

#define BREED_PAGE_COUNT	(7)

// CBreedDlg 对话框

class CBreedDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CBreedDlg)

public:
	CBreedDlg();   // 标准构造函数
	virtual ~CBreedDlg();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);

	DWORD	m_dwCurPage;
	WORD	m_wCurSel;
	WORD	m_wCurBreed;
	DWORD	m_dwCurOrder;

	void SetCurPm(WORD wBreed);

// 对话框数据
	enum { IDD = IDD_BREED };

protected:
	CListCtrl		m_ctrlPmList;
	CTabCtrl		m_ctrlTab;
	CButton			m_ctrlConfirm;

	CBreedTypePage			m_pageType;
	CBreedAblPage			m_pageAbl;
	CBreedSpecPage			m_pageSpec;
	CBreedItemPage			m_pageItem;
	CBreedLvlupLearnPage	m_pageLvlupLearn;
	CBreedDeriveLearnPage	m_pageDeriveLearn;
	CBreedMachineLearnPage	m_pageMachineLearn;
	CBreedTabPage *	m_rgpPages[BREED_PAGE_COUNT];

	void ShowTabPage(DWORD dwIndex);
	void RefreshPmList(BOOL bRomChanged);

	static INT CALLBACK SortPokemons(LPARAM lParam1, LPARAM lParam2, LPARAM lSortParam);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnLvnItemchangedPmList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedFromEvo();
	afx_msg void OnLvnGetInfoTipPmList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChangeOrder(UINT uID);
	DECLARE_MESSAGE_MAP()
};
