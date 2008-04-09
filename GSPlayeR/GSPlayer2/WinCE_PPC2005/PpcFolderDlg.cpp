//PpcFolderDlg.cpp
#include "GSPlayer2.h"
#include "PpcFolderDlg.h"

CPpcFolderDlg::CPpcFolderDlg()
{
	m_hwndMB = NULL;
}

CPpcFolderDlg::~CPpcFolderDlg()
{
}

void CPpcFolderDlg::OnInitDialog(HWND hDlg)
{
	CFolderDlg::OnInitDialog(hDlg);

	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hDlg;
	mbi.nToolBarId = IDR_DIALOG;
	mbi.hInstRes = GetInst();
	SHCreateMenuBar(&mbi);
	m_hwndMB = mbi.hwndMB;
}

void CPpcFolderDlg::OnOK(HWND hDlg)
{
	CFolderDlg::OnOK(hDlg);
	if (m_hwndMB) {
		CommandBar_Destroy(m_hwndMB);
		m_hwndMB = NULL;
	}
}

void CPpcFolderDlg::OnCancel(HWND hDlg)
{
	CFolderDlg::OnCancel(hDlg);
	if (m_hwndMB) {
		CommandBar_Destroy(m_hwndMB);
		m_hwndMB = NULL;
	}
}