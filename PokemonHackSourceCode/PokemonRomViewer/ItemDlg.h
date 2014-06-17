#pragma once
#include "afxwin.h"


// CItemDlg 对话框

class CItemDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CItemDlg)

public:
	CItemDlg();   // 标准构造函数
	virtual ~CItemDlg();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);

	WORD	m_wCurItem;

// 对话框数据
	enum { IDD = IDD_ITEM };

protected:
	ItemListEntry *	pItem;

	CListBox	m_ctrlItemList;
	CButton		m_ctrlConfirm;

	void SetItemList();
	void SetPrice();
	void GetPrice();
	void SetDesc();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnLbnSelchangeItmItemList();
	DECLARE_MESSAGE_MAP()
};
