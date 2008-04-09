//PpcFolderDlg.cpp
#include "GSPlayer2.h"
#include "PpcKeyCtrlDlg.h"

CPpcKeyCtrlDlg::CPpcKeyCtrlDlg()
{
	m_hwndParent = NULL;
	memset(&m_rcDlg, 0, sizeof(m_rcDlg));
	m_nColorBack = 0;
	m_nColorText = 0;
	m_hBrushBack = NULL;
	m_dwStart = 0;
}

CPpcKeyCtrlDlg::~CPpcKeyCtrlDlg()
{
}

void CPpcKeyCtrlDlg::ShowKeyCtrlDlg(HWND hwndParent, RECT* prc, int nColorBack, int nColorText)
{
	m_hwndParent = hwndParent;
	m_rcDlg = *prc;
	m_nColorBack = nColorBack;
	m_nColorText = nColorText;
	DialogBoxParam(GetInst(), MAKEINTRESOURCE(IDD_KEYCTRL_DLG), hwndParent, PpcKeyCtrlDlgProc, (LPARAM)this);
}

void CPpcKeyCtrlDlg::OnInitDialog(HWND hDlg)
{
	m_dwStart = GetTickCount();
	SetTimer(hDlg, ID_TIMER_KEYCTRLCLOSE, INT_TIMER_KEYCTRLCLOSE, NULL);

	m_hBrushBack = CreateSolidBrush(m_nColorBack); 

	RECT rc;
	int nXDiff, nYDiff;
	GetWindowRect(hDlg, &rc);
	nXDiff = (RECT_WIDTH(&m_rcDlg) - RECT_WIDTH(&rc)) / 2;
	nYDiff = (RECT_HEIGHT(&m_rcDlg) - RECT_HEIGHT(&rc)) / 2;
	MoveWindow(hDlg, m_rcDlg.left, m_rcDlg.top, RECT_WIDTH(&m_rcDlg), RECT_HEIGHT(&m_rcDlg), TRUE);

	GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_UP), &rc);
	ScreenToClient(hDlg, (LPPOINT)&rc);
	SetWindowPos(GetDlgItem(hDlg, IDC_STATIC_UP), 0, rc.left + nXDiff, rc.top + nYDiff, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_DOWN), &rc);
	ScreenToClient(hDlg, (LPPOINT)&rc);
	SetWindowPos(GetDlgItem(hDlg, IDC_STATIC_DOWN), 0, rc.left + nXDiff, rc.top + nYDiff, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_LEFT), &rc);
	ScreenToClient(hDlg, (LPPOINT)&rc);
	SetWindowPos(GetDlgItem(hDlg, IDC_STATIC_LEFT), 0, rc.left + nXDiff, rc.top + nYDiff, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_RIGHT), &rc);
	ScreenToClient(hDlg, (LPPOINT)&rc);
	SetWindowPos(GetDlgItem(hDlg, IDC_STATIC_RIGHT), 0, rc.left + nXDiff, rc.top + nYDiff, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_CENTER), &rc);
	ScreenToClient(hDlg, (LPPOINT)&rc);
	SetWindowPos(GetDlgItem(hDlg, IDC_STATIC_CENTER), 0, rc.left + nXDiff, rc.top + nYDiff, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CPpcKeyCtrlDlg::OnClose(HWND hDlg)
{
	if (m_hBrushBack) {
		DeleteObject(m_hBrushBack);
		m_hBrushBack = NULL;
	}
	KillTimer(hDlg, ID_TIMER_KEYCTRLCLOSE);
	EndDialog(hDlg, IDOK);
}

void CPpcKeyCtrlDlg::OnKeyDown(HWND hDlg, UINT uKeyCode)
{
	switch (uKeyCode) {
	case VK_RETURN:
		OnClose(hDlg);
		break;
	case VK_UP:
		PostMessage(m_hwndParent, WM_COMMAND, IDM_TOOL_VOLUP, 0);
		break;
	case VK_RIGHT:
		PostMessage(m_hwndParent, WM_COMMAND, IDM_PLAY_NEXT, 0);
		break;
	case VK_DOWN:
		PostMessage(m_hwndParent, WM_COMMAND, IDM_TOOL_VOLDOWN, 0);
		break;
	case VK_LEFT:
		PostMessage(m_hwndParent, WM_COMMAND, IDM_PLAY_PREV, 0);
		break;
	}
	m_dwStart = GetTickCount();
}

long CPpcKeyCtrlDlg::OnCtlColorStatic(HDC hDC)
{
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, m_nColorText);
	return (long)m_hBrushBack;
}

void CPpcKeyCtrlDlg::OnPaint(HWND hDlg)
{
	PAINTSTRUCT ps = {0};
	HDC hDC = BeginPaint(hDlg, &ps);
	FillRect(ps.hdc, &ps.rcPaint, m_hBrushBack);
	EndPaint(hDlg, &ps);
}

void CPpcKeyCtrlDlg::OnTimer(HWND hDlg, UINT nId)
{
	if (nId == ID_TIMER_KEYCTRLCLOSE) {
		if (GetTickCount() > m_dwStart + KEYCTRLDLG_CLOSE_INT)
			OnClose(hDlg);
	}
}

BOOL CALLBACK CPpcKeyCtrlDlg::PpcKeyCtrlDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CPpcKeyCtrlDlg* pDlg;
	switch (uMsg) {
	case WM_INITDIALOG:
		pDlg = (CPpcKeyCtrlDlg*)lParam;
		pDlg->OnInitDialog(hDlg);
		return TRUE;
	case WM_KEYDOWN:
		pDlg->OnKeyDown(hDlg, (UINT)wParam);
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			pDlg->OnClose(hDlg);
			return TRUE;
		}
		return FALSE;
	case WM_LBUTTONUP:
		pDlg->OnClose(hDlg);
		return TRUE;
	case WM_PAINT:
		pDlg->OnPaint(hDlg);
		return TRUE;
	case WM_CTLCOLORSTATIC:
		return pDlg->OnCtlColorStatic((HDC)wParam);
	case WM_TIMER:
		pDlg->OnTimer(hDlg, wParam);
		return TRUE;
	}
	return FALSE;
}