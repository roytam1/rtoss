#pragma once
#include "afxwin.h"


// CMemMiscPage0 对话框

class CMemMiscPage0 : public CPropertyPage
{
	DECLARE_DYNAMIC(CMemMiscPage0)

public:
	CMemMiscPage0();
	virtual ~CMemMiscPage0();

	VOID OnChangeUILang(VOID);

// 对话框数据
	enum { IDD = IDD_MEM_MISC0 };

protected:
	BOOL		m_bEnableText;
	CEdit		m_ctrlTrainerName;

	BOOL SetPlayerName(VOID);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnBnClickedEnableText();
	DECLARE_MESSAGE_MAP()
};
