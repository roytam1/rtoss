#pragma once


// CWaitDlg 对话框

class CWaitDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitDlg)

public:
	CWaitDlg();   // 标准构造函数
	virtual ~CWaitDlg();

	void BeginWait(DWORD dwRange);
	void IncStep();

// 对话框数据
	enum { IDD = IDD_WAIT };

protected:
	CProgressCtrl	m_ctrlProg;
	UINT_PTR		m_uTimer;
	DWORD			m_dwRange;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};
