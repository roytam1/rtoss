#pragma once

// CEncWaterPage 对话框

class CEncWaterPage : public CEncTabPage
{
	DECLARE_DYNAMIC(CEncWaterPage)

public:
	CEncWaterPage();   // 标准构造函数
	virtual ~CEncWaterPage();

	void OnConfigChanged(BOOL bRomChanged, BOOL bUILangChanged);
	void TransData(BOOL bToControls);

	void SetFocusedEntry(DWORD dwEncEntryIndex);

// 对话框数据
	enum { IDD = IDD_ROM_ENC_WATER };

protected:
	CComboBox	m_ctrlBrdList[ENC_WATER_ENTRY_COUNT];
	CStatic		m_ctrlImage[ENC_WATER_ENTRY_COUNT];
	CButton		m_ctrlSetBrd[ENC_WATER_ENTRY_COUNT];

	void SetBrdImage(DWORD dwIndex);
	void GetBreed(DWORD dwIndex);
	void SetBreed(DWORD dwIndex);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnCbnSelchangeEncBreedList(UINT uID);
	afx_msg void OnBnClickedEncSetBreed(UINT uID);
	afx_msg void OnSetFocus(UINT uID);
	afx_msg void OnDblClickedIcon(UINT uID);
	DECLARE_MESSAGE_MAP()
};
