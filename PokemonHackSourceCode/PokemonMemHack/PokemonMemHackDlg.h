// PokemonMemHackDlg.h : 头文件
//

#pragma once
#include "MemPmPage.h"
#include "MemMiscPage0.h"
#include "MemMiscPage1.h"
#include "MemMiscPage2.h"

// CPokemonMemHackDlg 对话框
class CPokemonMemHackDlg : public CDialog
{
// 构造
public:
	CPokemonMemHackDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPokemonMemHackDlg();

// 对话框数据
	enum { IDD = IDD_POKEMONMEMHACK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON	m_hIcon;

	CMemPmPage		m_pageMemPm;
	CMemMiscPage0	m_pageMisc0;
	CMemMiscPage1	m_pageMisc1;
	CMemMiscPage2	m_pageMisc2;
	CPropertySheet *m_pPropSht;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};
