// WaitDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PokemonRomViewer.h"
#include "WaitDlg.h"

// CWaitDlg 对话框

IMPLEMENT_DYNAMIC(CWaitDlg, CDialog)
CWaitDlg::CWaitDlg()
{
	m_uTimer = 0;
}

CWaitDlg::~CWaitDlg()
{
	if(m_uTimer)
		KillTimer(m_uTimer);
}

void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProg);
}

BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaitDlg 消息处理程序

void CWaitDlg::OnOK()
{
}

void CWaitDlg::OnCancel()
{
}

void CWaitDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent == m_uTimer)
	{
		KillTimer(nIDEvent);
		m_uTimer = 0;
		ShowWindow(SW_HIDE);
	}

	CDialog::OnTimer(nIDEvent);
}

void CWaitDlg::BeginWait(DWORD dwRange)
{
	if(m_uTimer)
	{
		KillTimer(m_uTimer);
		m_uTimer = 0;
	}

	CRect	rect;
	GetWindowRect(&rect);
	rect.MoveToXY(0, 0);
	GetParent()->ClientToScreen(&rect);
	MoveWindow(&rect);

	m_dwRange = dwRange;
	m_ctrlProg.SetRange32(0, m_dwRange);
	m_ctrlProg.SetStep(1);
	m_ctrlProg.SetPos(0);

	ShowWindow(SW_SHOW);
}

void CWaitDlg::IncStep()
{
	m_ctrlProg.StepIt();
	--m_dwRange;
	if(m_dwRange == 0)
	{
		m_uTimer = SetTimer(1, 500, NULL);
		if(m_uTimer == 0)
		{
			ShowWindow(SW_HIDE);
		}
	}
}
