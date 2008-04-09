#include "GSPlayer2.h"
#include "MainWnd.h"
#include "PlayListDlg.h"

CPlayListDlg::CPlayListDlg()
{
	m_fListDrag = FALSE;
	m_nDragItem = -1;

	m_hWnd = NULL;
	m_hImageList = ImageList_Create(16, 16, ILC_COLOR | ILC_MASK, 2, 2);
	HICON hIcon = LoadIcon(GetInst(), LPCTSTR(IDI_FILE_S));
	ImageList_AddIcon(m_hImageList, hIcon);
	DestroyIcon(hIcon);
	hIcon = LoadIcon(GetInst(), LPCTSTR(IDI_PLAY_S));
	ImageList_AddIcon(m_hImageList, hIcon);
	DestroyIcon(hIcon);
}

CPlayListDlg::~CPlayListDlg()
{
	ImageList_Destroy(m_hImageList);
}

BOOL CPlayListDlg::IsDialogMessage(LPMSG pMsg)
{
	if (!m_hWnd)
		return FALSE;
	return ::IsDialogMessage(m_hWnd, pMsg);
}

void CPlayListDlg::Show(CMainWnd* pParent)
{
	if (m_hWnd) {
		ShowWindow(m_hWnd, SW_SHOW);
		SetForegroundWindow(m_hWnd);
	}
	else {
		m_pParent = pParent;
		CreateDialogParam(GetInst(), (LPCTSTR)IDD_PLAYLIST_DLG, 
						pParent->GetHandle(), PlayListDlgProc, (LPARAM)this);
#ifndef _WIN32_WCE
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_ACCEPTFILES);
#endif
		ShowWindow(m_hWnd, SW_SHOW);
	}
}

void CPlayListDlg::Close()
{
	DestroyWindow(m_hWnd);
}

void CPlayListDlg::UpdatePlayList()
{
	if (!m_hWnd)
		return;

	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);

	ListView_DeleteAllItems(hwndLV);
	int nCount = m_pParent->GetFileCount();
	for (int i = 0; i < nCount; i++)
		AddList(hwndLV, i);

	int nCurrent = m_pParent->GetCurrentFile();
	ListView_EnsureVisible(hwndLV, nCurrent, FALSE);
	ListView_SetItemState(hwndLV, nCurrent, 
		LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

void CPlayListDlg::SetCurrent(int nIndex)
{
	if (!m_hWnd)
		return;

	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	int nCount = ListView_GetItemCount(hwndLV);
	for (int i = 0; i < nCount; i++) {
		LVITEM li;
		memset(&li, 0, sizeof(LVITEM));
		li.mask = LVIF_IMAGE;
		li.iItem = i;
		ListView_GetItem(hwndLV, &li);
		if (li.iImage && i != nIndex) {
			li.iImage = 0;
			ListView_SetItem(hwndLV, &li);
		}
		else if (i == nIndex) {
			li.iImage = 1;
			ListView_SetItem(hwndLV, &li);
		}
	}
}

void CPlayListDlg::AddList(HWND hwndLV, int nIndex)
{
	TCHAR szTitle[MAX_PATH] = {0};

	FILEINFO* p = m_pParent->GetInfo(nIndex);
	m_pParent->GetTitle(nIndex, szTitle);

	LVITEM li;
	memset(&li, 0, sizeof(LVITEM));
	li.mask = LVIF_TEXT | LVIF_IMAGE;
	li.iItem = ListView_GetItemCount(hwndLV);
	li.pszText = szTitle;
	li.iImage = m_pParent->GetCurrentFile() == nIndex ? 1 : 0;
	ListView_InsertItem(hwndLV, &li);
}

BOOL CALLBACK CPlayListDlg::PlayListDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CPlayListDlg* pDlg;
	switch (uMsg) {
	case WM_INITDIALOG:
		pDlg = (CPlayListDlg*)lParam;
		pDlg->OnInitDialog(hwndDlg);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			case IDCANCEL:
				//DestroyWindow(hwndDlg);
				ShowWindow(hwndDlg, SW_HIDE);
				return TRUE;
			case IDOK:
			case IDM_PLAY_SELECTED:
				pDlg->OnOK();
				return TRUE;
			case IDC_DELETE:
			case IDM_FILE_DELETE:
				pDlg->OnDelete();
				return TRUE;
			case IDC_UP:
			case IDM_FILE_UP:
				pDlg->OnUp();
				return TRUE;
			case IDC_DOWN:
			case IDM_FILE_DOWN:
				pDlg->OnDown();
				return TRUE;
			case IDC_ADD:
				pDlg->OnAdd();
				return TRUE;
			case IDC_SAVE:
				pDlg->OnSave();
				return TRUE;
			case IDM_FILE_SORT_BYNAME:
			case IDM_FILE_SORT_BYPATH:
				pDlg->OnSort(LOWORD(wParam));
				return TRUE;
		}
		return FALSE;
	case WM_CLOSE:
		//DestroyWindow(hwndDlg);
		ShowWindow(hwndDlg, SW_HIDE);
		return TRUE;
	case WM_DESTROY:
		pDlg->m_hWnd = NULL;
		return TRUE;
	case WM_MOVE:
		pDlg->OnMove(LOWORD(lParam), HIWORD(lParam));
		return TRUE;
#ifndef _WIN32_WCE
	case WM_DROPFILES:
		pDlg->OnDropFiles((HDROP)wParam);
		return TRUE;
#endif
	case WM_NOTIFY:
	{
		NMHDR* pnmh = (NMHDR*)lParam;
		switch (pnmh->code) {
		case LVN_KEYDOWN:
			pDlg->OnListKeyDown((NMLVKEYDOWN*)pnmh);
			return TRUE;
		case NM_DBLCLK:
			pDlg->OnListDblClk(pnmh);
			return TRUE;
		case LVN_BEGINDRAG:
			pDlg->OnListBeginDrag((NMLISTVIEW*)pnmh);
			return TRUE;
		case NM_RCLICK:
			pDlg->OnListRClick(pnmh);
			return TRUE;
		case NM_CLICK:
			pDlg->OnListClick(pnmh);
			return TRUE;
		}
		return FALSE;
	}
	case WM_MOUSEMOVE:
		pDlg->OnMouseMove((short)LOWORD(lParam), (short)HIWORD(lParam));
		return TRUE;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		pDlg->OnLButtonUp();
		return TRUE;
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	default:
		return FALSE;
	}
}

void CPlayListDlg::OnInitDialog(HWND hwndDlg)
{
	m_hWnd = hwndDlg;
	ShellInitDialog(m_hWnd);
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);

	RECT rc;
	GetClientRect(hwndLV, &rc);

	ListView_SetExtendedListViewStyle(hwndLV, 
		ListView_GetExtendedListViewStyle(hwndLV) | LVS_EX_FULLROWSELECT);

	// カラムの挿入
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - 2;
	lvc.pszText = _T("Name");
	lvc.iSubItem = 0;
	ListView_InsertColumn(hwndLV, 0, &lvc);
	
	// イメージリストを設定
	ListView_SetImageList(hwndLV, m_hImageList, LVSIL_SMALL);
	
	// リストを更新
	UpdatePlayList();
}

void CPlayListDlg::OnMove(int x, int y)
{
	RECT rc;
	GetWindowRect(m_hWnd, &rc);

	RECT rcWork;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, FALSE);

	int nLeft = rc.left;
	int nTop = rc.top;
	if (nLeft > rcWork.left - WINDOW_FIT_MARGIN && nLeft < rcWork.left + WINDOW_FIT_MARGIN)
		nLeft = rcWork.left;
	else if (nLeft + RECT_WIDTH(&rc) > rcWork.left + RECT_WIDTH(&rcWork) - WINDOW_FIT_MARGIN &&
			nLeft + RECT_WIDTH(&rc) < rcWork.left + RECT_WIDTH(&rcWork) + WINDOW_FIT_MARGIN)
		nLeft = rcWork.left + RECT_WIDTH(&rcWork) - RECT_WIDTH(&rc);

	if (nTop > rcWork.top - WINDOW_FIT_MARGIN && nTop < rcWork.top + WINDOW_FIT_MARGIN)
		nTop = rcWork.top;
	else if (nTop + RECT_HEIGHT(&rc) > rcWork.top + RECT_HEIGHT(&rcWork) - WINDOW_FIT_MARGIN &&
			nTop + RECT_HEIGHT(&rc) < rcWork.top + RECT_HEIGHT(&rcWork) + WINDOW_FIT_MARGIN)
		nTop = rcWork.top + RECT_HEIGHT(&rcWork) - RECT_HEIGHT(&rc);

	if (nLeft != rc.left || nTop != rc.top)
		MoveWindow(m_hWnd, nLeft, nTop, RECT_WIDTH(&rc), RECT_HEIGHT(&rc), TRUE);
}

void CPlayListDlg::OnAdd()
{
	m_pParent->OpenMediaFile(TRUE, m_hWnd);
	UpdatePlayList();
}

void CPlayListDlg::OnUp(BOOL fVisible)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	SetFocus(hwndLV);

	int nCount = ListView_GetItemCount(hwndLV);
	if (ListView_GetItemState(hwndLV, 0, LVIS_SELECTED))
		return;

	for (int i = 1; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED)) {
			// ListViewの更新
			TCHAR szTitle[MAX_PATH] = {0};

			LVITEM li;
			memset(&li, 0, sizeof(LVITEM));
			li.mask = LVIF_TEXT | LVIF_IMAGE;
			li.iItem = i;
			li.pszText = szTitle;
			li.cchTextMax = MAX_PATH;
			ListView_GetItem(hwndLV, &li);
			ListView_DeleteItem(hwndLV, i);
			li.iItem = i - 1;
			ListView_InsertItem(hwndLV, &li);

			// プレイリストの更新
			m_pParent->UpFile(i);

			ListView_SetItemState(hwndLV, i - 1, 
				LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

			if (fVisible)
				ListView_EnsureVisible(hwndLV, i - 1, FALSE);
		}
	}
}

void CPlayListDlg::OnDown(BOOL fVisible)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	SetFocus(hwndLV);

	int nCount = ListView_GetItemCount(hwndLV);
	if (ListView_GetItemState(hwndLV, nCount - 1, LVIS_SELECTED))
		return;

	for (int i = nCount - 2; i >= 0; i--) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED)) {
			// ListViewの更新
			TCHAR szTitle[MAX_PATH] = {0};

			LVITEM li;
			memset(&li, 0, sizeof(LVITEM));
			li.mask = LVIF_TEXT | LVIF_IMAGE;
			li.iItem = i;
			li.pszText = szTitle;
			li.cchTextMax = MAX_PATH;
			ListView_GetItem(hwndLV, &li);
			ListView_DeleteItem(hwndLV, i);
			li.iItem = i + 1;
			ListView_InsertItem(hwndLV, &li);

			// プレイリストの更新
			m_pParent->DownFile(i);
			ListView_SetItemState(hwndLV, i + 1, 
				LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

			if (fVisible)
				ListView_EnsureVisible(hwndLV, i + 1, FALSE);
		}
	}
}

void CPlayListDlg::OnDelete()
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	SetFocus(hwndLV);

	int i;
	int nCount = ListView_GetItemCount(hwndLV);
	for (i = nCount - 1; i >= 0; i--) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED)) {
			ListView_DeleteItem(hwndLV, i);
			m_pParent->DeleteFile(i);
		}
	}

	nCount = ListView_GetItemCount(hwndLV);
	for (i = 0; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_FOCUSED)) {
			ListView_SetItemState(hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
			break;
		}
	}
}

void CPlayListDlg::OnSave()
{
	m_pParent->SaveMediaFile(m_hWnd);
}

void CPlayListDlg::OnListKeyDown(NMLVKEYDOWN* pnm)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);

	switch (pnm->wVKey) {
	case VK_DELETE:
		OnDelete();
		return;
	case VK_TAB:
		SetFocus(m_pParent->GetHandle());
		return;
	case 'A':
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000) 
			ListView_SetItemState(hwndLV, -1, LVIS_SELECTED, LVIS_SELECTED);
		return;
	}
}

void CPlayListDlg::OnListDblClk(NMHDR* pnm)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	int nCount = ListView_GetItemCount(hwndLV);
	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED)) {
			m_pParent->Play(i);
			return;
		}
	}
}

void CPlayListDlg::OnListClick(NMHDR* pnm)
{
#ifdef _WIN32_WCE
	if (GetAsyncKeyState(VK_MENU) & 0x8000) {
		NMLISTVIEW* pnmlv = (NMLISTVIEW*)pnm;
		ShowMenu(pnmlv->ptAction.x, pnmlv->ptAction.y);
	}
#endif
}

void CPlayListDlg::OnListRClick(NMHDR* pnm)
{
	NMLISTVIEW* pnmlv = (NMLISTVIEW*)pnm;
	ShowMenu(pnmlv->ptAction.x, pnmlv->ptAction.y);
}

#ifndef _WIN32_WCE
void CPlayListDlg::OnDropFiles(HDROP hDrop)
{
	m_pParent->AddDropFiles(hDrop);
}
#endif

void CPlayListDlg::OnListBeginDrag(NMLISTVIEW* pnmlv)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);

	SetCapture(m_hWnd);
	m_fListDrag = TRUE;
	m_nDragItem = pnmlv->iItem;
}

void CPlayListDlg::OnMouseMove(int x, int y)
{
	if (m_fListDrag) {
		RECT rcItem;
		HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
		if (!ListView_GetItemRect(hwndLV, 0, &rcItem, LVIR_BOUNDS))
			return;

		POINT pt = {x, y};
		ClientToScreen(m_hWnd, &pt);
		ScreenToClient(hwndLV, &pt);

		RECT rcLV;
		GetClientRect(hwndLV, &rcLV);
		if (pt.y < RECT_HEIGHT(&rcItem)) {
			ListView_Scroll(hwndLV, 0, -RECT_HEIGHT(&rcItem));
			OnUp(FALSE);
			m_nDragItem = max(m_nDragItem - 1, 0);
		}
		else if (pt.y > rcLV.bottom - RECT_HEIGHT(&rcItem)) {
			ListView_Scroll(hwndLV, 0, RECT_HEIGHT(&rcItem));
			OnDown(FALSE);
			m_nDragItem = min(m_nDragItem + 1, ListView_GetItemCount(hwndLV) - 1);
		}
		else {
			LVHITTESTINFO lvhti;
			lvhti.pt.x = pt.x; 
			lvhti.pt.y = pt.y;
			int nItem = ListView_HitTest(hwndLV, &lvhti);
			if (nItem == -1)
				return;

			if (nItem - m_nDragItem > 0) {
				for (int i = 0; i < nItem - m_nDragItem; i++)
					OnDown(FALSE);
				m_nDragItem = nItem;
			}
			else if (nItem - m_nDragItem < 0) {
				for (int i = 0; i < m_nDragItem - nItem; i++)
					OnUp(FALSE);
				m_nDragItem = nItem;
			}
		}
	}
}

void CPlayListDlg::OnLButtonUp()
{
	if (!m_fListDrag)
		return;

	m_fListDrag = FALSE;
	ReleaseCapture();
}

void CPlayListDlg::OnOK()
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	int nCount = ListView_GetItemCount(hwndLV);
	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED)) {
			m_pParent->Play(i);
			break;
		}
	}
}

void CPlayListDlg::ShowMenu(int x, int y)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	
	HMENU hMenu = LoadMenu(GetInst(), (LPCTSTR)IDR_POPUP);
	HMENU hSub = GetSubMenu(hMenu, 
		ListView_GetSelectedCount(hwndLV) == 1 ? IDM_SUBMENU_PLAYLIST1 : IDM_SUBMENU_PLAYLIST2);

	POINT pt = {x, y};
	ClientToScreen(hwndLV, &pt);
	TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
	DestroyMenu(hMenu);
}

void CPlayListDlg::OnSort(UINT uID)
{
	int nSort = SORTFILE_BYNAME;
	switch (uID) {
		case IDM_FILE_SORT_BYNAME:
			nSort = SORTFILE_BYNAME; break;
		case IDM_FILE_SORT_BYPATH:
			nSort = SORTFILE_BYPATH; break;
	}
	m_pParent->SortFile(nSort);
	UpdatePlayList();
}
