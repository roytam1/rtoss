#pragma once


// CMemPmTabPage 对话框

class CMemPmTabPage : public CDialog
{
	DECLARE_DYNAMIC(CMemPmTabPage)

public:
	CMemPmTabPage();   // 标准构造函数
	virtual ~CMemPmTabPage();

	UINT	m_uTemplateId;
	LPCTSTR	m_szPageName;

	virtual void TransData(BOOL bToControls) = 0;
	virtual void OnChangeUILang(VOID) = 0;

// 对话框数据

protected:
	virtual CPokemonCodec * GetPokemonCodec(VOID);
	virtual PokemonStructActive * GetPokemonStructActive(VOID);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
};
