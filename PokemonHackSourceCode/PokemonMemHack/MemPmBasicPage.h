#pragma once
#include "afxwin.h"


// CMemPmBasicPage 对话框

class CMemPmBasicPage : public CMemPmTabPage
{
	DECLARE_DYNAMIC(CMemPmBasicPage)

public:
	CMemPmBasicPage();   // 标准构造函数
	virtual ~CMemPmBasicPage();

	void TransData(BOOL bToControls);
	void OnChangeUILang(VOID);

// 对话框数据
	enum { IDD = IDD_MEM_PM_BASIC };

protected:
	WORD		m_wPreBreed;

	CPokemonCodec *	m_ppc;
	CComboBoxEx	m_ctrlBreedList;
	CComboBox	m_ctrlPsList;
	CListBox	m_ctrlSpecList;
	CComboBox	m_ctrlItemList;
	CStatic		m_ctrlPsDesc;
	CEdit		m_ctrlNickName;
	CEdit		m_ctrlCatcherName;
	CComboBox	m_ctrlSexList;
	CComboBox	m_ctrlLevelList;

	afx_msg void SetBreed();
	afx_msg void GetBreed();

	afx_msg void GetChar();
	afx_msg void GetId();

	afx_msg void GetShiny();

	afx_msg void SetPs();
	afx_msg void SetPsDesc();
	afx_msg void GetPs();

	afx_msg void SetSpecList();
	afx_msg void SetSpecDesc();
	afx_msg void GetSpec();

	afx_msg void SetSexList();
	afx_msg void SetSex();
	afx_msg void GetSex();

	afx_msg void SetLevelList();
	afx_msg void SetLevel();
	afx_msg void GetLevel();

	afx_msg void SetExp();
	afx_msg void GetExp();

	afx_msg void GetItem();
	afx_msg void SetItemDesc();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedEgg();
	afx_msg void OnBnClickedEnableText();
	afx_msg void OnBnClickedObedience();
	afx_msg void OnCbnSelchangeSexList();
	afx_msg void OnEnChangeExp();
	afx_msg void OnStnDblclickCharTitle();
	afx_msg void OnStnDblclickIdTitle();
	DECLARE_MESSAGE_MAP()
};
