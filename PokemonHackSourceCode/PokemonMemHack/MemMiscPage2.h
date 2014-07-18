#pragma once


// CMemMiscPage2 对话框

class CMemMiscPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CMemMiscPage2)

public:
	CMemMiscPage2();
	virtual ~CMemMiscPage2();

	VOID OnChangeUILang(VOID);

// 对话框数据
	enum { IDD = IDD_MEM_MISC2 };

protected:
	CComboBox	m_ctrlGameVersionList;
	CFont		m_Font;

	UINT_PTR	m_uTimerId;
	DWORD		m_dwTimerFlags;
	BOOL		SetupTimer();
	VOID		UnsetupTimer();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedUpgradePokedex();
	afx_msg void OnBnClickedFullPokedex();
	afx_msg void OnBnClickedGiveBadges();
	afx_msg void OnBnClickedEnableMirageIsland();
	afx_msg void OnBnClickedGiveAllPokemon();
	afx_msg void OnBnClickedSetPokeball();
	afx_msg void OnBnClickedSetIndv();
	afx_msg void OnBnClickedSetObedience();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedLockStepCounterToFe();
	afx_msg void OnBnClickedLockDaycareCounter2ToFe();
	afx_msg void OnBnClickedAdjustClock();
	afx_msg void OnBnClickedLockExpGainTo7fff();
	DECLARE_MESSAGE_MAP()
};
