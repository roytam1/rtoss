#include "GSPlayer2.h"
#include "MainWnd.h"
#include "PlayListDlgPPC.h"

CPlayListDlg::CPlayListDlg()
{
	m_hWnd = NULL;
	m_hwndMB = NULL;
	m_fCtrlKey = FALSE;
	
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

void  CPlayListDlg::Show(CMainWnd* pParent)
{
	m_pParent = pParent;
	DialogBoxParam(GetInst(), (LPCTSTR)IDD_PLAYLIST_DLG, pParent->GetHandle(), PlayListDlgProc, (LPARAM)this);
}

void CPlayListDlg::Close()
{
	if (m_hWnd) {
		EndDialog(m_hWnd, IDOK);
		m_hWnd = NULL;
	}
	if (m_hwndMB) {
		CommandBar_Destroy(m_hwndMB);
		m_hwndMB = NULL;
	}
	if (m_fCtrlKey) {
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 1);
		m_fCtrlKey = FALSE;
	}
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
				pDlg->Close();
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
			case IDC_CTRLKEY:
				pDlg->OnCtrlKey();
				return TRUE;
			case IDM_FILE_SORT_BYNAME:
			case IDM_FILE_SORT_BYPATH:
				pDlg->OnSort(LOWORD(wParam));
				return TRUE;
		}
		return FALSE;
	case WM_CLOSE:
		pDlg->Close();
		return TRUE;
	case WM_DESTROY:
		pDlg->m_hWnd = NULL;
		return TRUE;
	case WM_MOVE:
		pDlg->OnMove(LOWORD(lParam), HIWORD(lParam));
		return TRUE;
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
		case NM_RETURN:
			pDlg->OnOK();
			return TRUE;
		case GN_CONTEXTMENU:
			pDlg->OnContextMenu((NMRGINFO*)pnmh);
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
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	case WM_SIZE:
	{
		RECT rc;
		HWND hwnd;
		POINT pt;

		hwnd = GetDlgItem(hwndDlg, IDCANCEL);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd, LOWORD(lParam) - RECT_WIDTH(&rc) - 2, pt.y, RECT_WIDTH(&rc), RECT_HEIGHT(&rc), TRUE);

		hwnd = GetDlgItem(hwndDlg, IDC_PLAY_LIST);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd, pt.x, pt.y, LOWORD(lParam) - pt.x * 2, HIWORD(lParam) - pt.y - pt.x, TRUE);

		GetWindowRect(hwnd, &rc);
		SendMessage(hwnd, LVM_SETCOLUMNWIDTH, 0, 
			MAKELPARAM(RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXBORDER) * 2, 0));
	}
	default:
		return FALSE;
	}
}

void CPlayListDlg::OnInitDialog(HWND hwndDlg)
{
	m_hWnd = hwndDlg;
	ShellInitDialog(m_hWnd);

	// ツールバーの準備
	SHMENUBARINFO mbi;
	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize = sizeof(SHMENUBARINFO);
	mbi.hwndParent = m_hWnd;
	mbi.nToolBarId = IDR_DUMMY;
	mbi.hInstRes = GetInst();
	SHCreateMenuBar(&mbi);
	m_hwndMB = mbi.hwndMB;

	SendMessage(m_hwndMB, TB_DELETEBUTTON, 0, 0);
	SendMessage(m_hwndMB, TB_ADDSTRING, (WPARAM)GetInst(), IDC_UP);
	SendMessage(m_hwndMB, TB_ADDSTRING, (WPARAM)GetInst(), IDC_DOWN);

	CommandBar_AddBitmap(m_hwndMB, GetInst(), IDR_PLAYLIST, 6, 16, 16);
	TBBUTTON tbbMain[] = {
		{0, IDC_ADD, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{1, IDC_UP, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE, 0, 0, 0, 0},
		{2, IDC_DOWN, TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE, 0, 0, 0, 1},
		{3, IDC_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{4, IDC_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
		{5, IDC_CTRLKEY, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, 0, 0, -1}
	};

	for (int i = 0; i < sizeof(tbbMain) / sizeof(TBBUTTON); i++) {
		CommandBar_InsertButton(m_hwndMB, i, &tbbMain[i]);
	}

	// リストの準備
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	ListView_SetExtendedListViewStyle(hwndLV, 
		ListView_GetExtendedListViewStyle(hwndLV) | LVS_EX_FULLROWSELECT);

	// カラムの挿入
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = 0;
	lvc.pszText = _T("Name");
	lvc.iSubItem = 0;
	ListView_InsertColumn(hwndLV, 0, &lvc);
	
	// イメージリストを設定
	ListView_SetImageList(hwndLV, m_hImageList, LVSIL_SMALL);

	// リストを更新
	UpdatePlayList();
}

void CPlayListDlg::OnOK()
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	int nCount = ListView_GetItemCount(hwndLV);
	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED)) {
			m_pParent->Play(i);
			Close();
			break;
		}
	}
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
	case VK_ESCAPE:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		return;
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
			Close();
			break;
		}
	}
}

void CPlayListDlg::OnListBeginDrag(NMLISTVIEW* pnmlv)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);

	SetCapture(m_hWnd);
	m_fListDrag = TRUE;
	m_nDragItem = pnmlv->iItem;
}

void CPlayListDlg::OnContextMenu(NMRGINFO* pnmri)
{
	HWND hwndLV = GetDlgItem(m_hWnd, IDC_PLAY_LIST);
	
	HMENU hMenu = LoadMenu(GetInst(), (LPCTSTR)IDR_POPUP);
	HMENU hSub = GetSubMenu(hMenu, 
		ListView_GetSelectedCount(hwndLV) == 1 ? IDM_SUBMENU_PLAYLIST1 : IDM_SUBMENU_PLAYLIST2);

	TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN, 
				pnmri->ptAction.x, pnmri->ptAction.y, 0, m_hWnd, NULL);
	DestroyMenu(hMenu);
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

void CPlayListDlg::OnCtrlKey()
{
	keybd_event(VK_CONTROL, 0, m_fCtrlKey ? KEYEVENTF_KEYUP : 0, 1);
	m_fCtrlKey = !m_fCtrlKey;

	SendMessage(m_hwndMB, TB_SETSTATE, IDC_CTRLKEY, m_fCtrlKey ?
				TBSTATE_CHECKED | TBSTATE_ENABLED : TBSTATE_ENABLED);
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
