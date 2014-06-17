#pragma once
#include "afxwin.h"


// CBreedItemPage 对话框

class CBreedItemPage : public CBreedTabPage
{
	DECLARE_DYNAMIC(CBreedItemPage)

public:
	CBreedItemPage();   // 标准构造函数
	virtual ~CBreedItemPage();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);
	void TransData(BOOL bToControls);

// 对话框数据
	enum { IDD = IDD_BREED_ITEM };

protected:
	CListBox	m_ctrlItemList[2];

	afx_msg void SetItemDesc(UINT uID);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
};
