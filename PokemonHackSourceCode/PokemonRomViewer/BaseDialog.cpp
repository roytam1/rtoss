// BaseDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "BaseDialog.h"

// CBaseDialog 对话框

IMPLEMENT_DYNAMIC(CBaseDialog, CDialog)
CBaseDialog::CBaseDialog()
{
}

CBaseDialog::~CBaseDialog()
{
}

void CBaseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDialog, CDialog)
END_MESSAGE_MAP()


// CBaseDialog 消息处理程序
BOOL CBaseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	HICON	hIcon;

	if(GetWindowLongPtr(m_hWnd, GWL_STYLE) & WS_DLGFRAME)
	{
		hIcon = AfxGetApp()->LoadIcon(IDI_DIALOG);
		SetIcon(hIcon, TRUE);
		SetIcon(hIcon, FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CBaseDialog::OnOK()
{
}
