#pragma once


// CBreedTabPage 对话框

class CBreedTabPage : public CBaseDialog
{
	DECLARE_DYNAMIC(CBreedTabPage)

public:
	CBreedTabPage();   // 标准构造函数
	virtual ~CBreedTabPage();

	LPCTSTR	m_szPageName;

	virtual void TransData(BOOL bToControls) = 0;

// 对话框数据

protected:
	BreedListEntry *m_pBreed;
	WORD *			m_pBreedAbilities;

	void GetCurrentBreed();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};
