#pragma once


// CBreedAblPage 对话框

class CBreedAblPage : public CBreedTabPage
{
	DECLARE_DYNAMIC(CBreedAblPage)

public:
	CBreedAblPage();   // 标准构造函数
	virtual ~CBreedAblPage();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);
	void TransData(BOOL bToControls);

// 对话框数据
	enum { IDD = IDD_BREED_ABL };

protected:
	//CButton		m_ctrlExportBreed;
	//CButton		m_ctrlExportEvo;
	//CButton		m_ctrlExportLearn;
	//CButton		m_ctrlExportMachine;
	//CButton		m_ctrlExportSkill;
	//CButton		m_ctrlExportIndex;
	CButton		m_ctrlExportBtn[6];

	void EnableExportButtons(BOOL bEnable);

	void GetIndvAbl(DWORD dwIndex);
	void CalcIndvSum();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnEnChangeBrdAbl(UINT uID);
	afx_msg void OnBnClickedExportBrd();
	afx_msg void OnBnClickedExportEvo();
	afx_msg void OnBnClickedExportLearn();
	afx_msg void OnBnClickedExportMachine();
	afx_msg void OnBnClickedExportSkill();
	afx_msg void OnBnClickedExportIndex();
	afx_msg void OnStnDblclickShowExportButtons();
	DECLARE_MESSAGE_MAP()
};
