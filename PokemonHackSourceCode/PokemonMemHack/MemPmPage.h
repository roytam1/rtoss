#pragma once


// CMemPmPage 对话框
#include "MemPmTabPage.h"
#include "MemPmBasicPage.h"		// 1
#include "MemPmAbilityPage.h"	// 2
#include "MemPmSkillPage.h"		// 3
#include "MemPmRibbonPage.h"	// 4
#include "MemPmMiscPage.h"		// 5
#define MEM_PM_PAGE_COUNT	(5)

class CMemPmPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CMemPmPage)

public:
	CMemPmPage();
	virtual ~CMemPmPage();

	DWORD	m_dwCurTabPage;
	DWORD	m_dwCurPm;

// 对话框数据
	enum { IDD = IDD_MEM_PM };

	VOID OnChangeUILang(VOID);
	void SetPmList(BOOL bSelectionOnly = FALSE);

protected:
	CTabCtrl	m_ctrlTab;
	CListCtrl	m_ctrlPmList;
	CButton		m_ctrlConfirm;

	CMemPmBasicPage		m_pageBasic;
	CMemPmAbilityPage	m_pageAbility;
	CMemPmSkillPage		m_pageSkill;
	CMemPmRibbonPage	m_pageRibbon;
	CMemPmMiscPage		m_pageMisc;
	CMemPmTabPage *	m_rgpPages[MEM_PM_PAGE_COUNT];

	void ShowTabPage(DWORD dwPageIndex);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLangCn();
	afx_msg void OnBnClickedLangJp();
	afx_msg void OnBnClickedCountDec();
	afx_msg void OnBnClickedCountHex();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnLvnItemchangedPmList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLangEn();
	afx_msg void OnBnClickedCountNone();
	afx_msg void OnBnClickedMoveUp();
	afx_msg void OnBnClickedMoveDown();
	DECLARE_MESSAGE_MAP()
};
