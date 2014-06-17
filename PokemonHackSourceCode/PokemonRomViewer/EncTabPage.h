#pragma once


// CEncTabPage 对话框

class CEncTabPage : public CBaseDialog
{
	DECLARE_DYNAMIC(CEncTabPage)

public:
	CEncTabPage();   // 标准构造函数
	virtual ~CEncTabPage();

	LPCTSTR	m_szPageName;
	DWORD	m_dwFocusedEntry;

	virtual void TransData(BOOL bToControls) = 0;
	virtual void SetFocusedEntry(DWORD dwEncEntryIndex) = 0;

// 对话框数据

protected:
	EncLandList *	m_pLandList;
	EncWaterList *	m_pWaterList;
	EncStoneList *	m_pStoneList;
	EncFishList *	m_pFishList;
	BYTE *			m_pAnnoonList;

	void GetCurrentEncLists();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};
