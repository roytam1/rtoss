#pragma once


// CBaseDialog 对话框

class CBaseDialog : public CDialog
{
	DECLARE_DYNAMIC(CBaseDialog)

public:
	CBaseDialog();   // 标准构造函数
	virtual ~CBaseDialog();

	UINT	m_uTemplateId;

	virtual void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged) = 0;

// 对话框数据

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
