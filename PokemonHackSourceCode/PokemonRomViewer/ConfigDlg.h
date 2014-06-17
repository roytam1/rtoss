#pragma once


// CConfigDlg 对话框

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigDlg();

	DWORD		m_dwLang;
	DWORD		m_dwCount;
	CString		m_szRomPath;

// 对话框数据
	enum { IDD = IDD_CONFIG };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedBrowse();
	DECLARE_MESSAGE_MAP()
};
