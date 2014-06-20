#pragma once
#include "afxwin.h"


// CBreedSpecPage 对话框

class CBreedSpecPage : public CBreedTabPage
{
	DECLARE_DYNAMIC(CBreedSpecPage)

public:
	CBreedSpecPage();   // 标准构造函数
	virtual ~CBreedSpecPage();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);
	void TransData(BOOL bToControls);

// 对话框数据
	enum { IDD = IDD_BREED_SPEC };

protected:
	CComboBox	m_ctrlExpTypeList;
	CComboBox	m_ctrlSpecList[2];

	CFont		m_Font;
	CStatic *		m_Static;
	CComboBox *		m_ComboBox;

	void SetSpecDesc(DWORD dwIndex);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeBrdSpecList0();
	afx_msg void OnCbnSelchangeBrdSpecList1();
	DECLARE_MESSAGE_MAP()
};
