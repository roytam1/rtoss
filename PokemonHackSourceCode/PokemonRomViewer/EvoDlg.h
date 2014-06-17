#pragma once


// CEvoDlg 对话框

class CEvoDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CEvoDlg)

public:
	CEvoDlg();   // 标准构造函数
	virtual ~CEvoDlg();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);

	WORD	m_wCurSel;
	WORD	m_wCurBreed;
	DWORD	m_dwCurOrder;

	void SetCurPm(WORD wBreed);

// 对话框数据
	enum { IDD = IDD_EVO };

protected:
	EvoListEntry *	m_pEvo;

	CListCtrl	m_ctrlPmList;
	CComboBox	m_ctrlEvoCondList[EVO_LIST_COUNT];
	CStatic		m_ctrlEvoParamDesc[EVO_LIST_COUNT];
	CEdit		m_ctrlEvoParam[EVO_LIST_COUNT];
	CComboBox	m_ctrlEvoItemList[EVO_LIST_COUNT];
	CComboBox	m_ctrlEvoBreedList[EVO_LIST_COUNT];
	CStatic		m_ctrlEvoBreedIcon[EVO_LIST_COUNT];
	CButton		m_ctrlConfirm;

	void RefreshPmList(BOOL bRomChanged);
	void SetItemList();
	void GetCurrentEvo();
	void GetAllParams();
	void GetParam(DWORD dwIndex);
	void SetAllParams();
	void SetParam(DWORD dwIndex, BOOL bForceUpdate = FALSE);
	void SetBreedIcon(DWORD dwIndex);
	void SetBreed(DWORD dwIndex);
	BOOL GetBreed(DWORD dwIndex);
	void OnCondChanged(DWORD dwIndex);
	void OnSetBreed(DWORD dwIndex);

	static INT CALLBACK SortPokemons(LPARAM lParam1, LPARAM lParam2, LPARAM lSortParam);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedConfirm();
	afx_msg void OnLvnItemchangedPmList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeEvoBrdList(UINT uID);
	afx_msg void OnCbnSelchangeEvoCondList(UINT uID);
	afx_msg void OnBnClickedEvoSetBrd(UINT uID);
	afx_msg void OnLvnGetInfoTipPmList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedFromBrd();
	afx_msg void OnBnClickedReverseFind();
	afx_msg void OnStnDblclickIcon(UINT uID);
	afx_msg void OnChangeOrder(UINT uID);
	DECLARE_MESSAGE_MAP()
};
