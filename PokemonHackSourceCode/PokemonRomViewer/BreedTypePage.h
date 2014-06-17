#pragma once
#include "afxwin.h"


// CBreedTypePage 对话框

class CBreedTypePage : public CBreedTabPage
{
	DECLARE_DYNAMIC(CBreedTypePage)

public:
	CBreedTypePage();   // 标准构造函数
	virtual ~CBreedTypePage();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);
	void TransData(BOOL bToControls);

// 对话框数据
	enum { IDD = IDD_BREED_TYPE };

protected:
	CListBox	m_ctrlTypeList[2];
	CListBox	m_ctrlEGList[2];

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
