//FolderDlg.cpp
#include "GSPlayer2.h"
#include "FolderDlg.h"

CFolderDlg::CFolderDlg()
{
}

CFolderDlg::~CFolderDlg()
{
}

///////////////////////////////////////////////////////////////
void CFolderDlg::OnInitDialog(HWND hDlg)
{
	CTempStr str(IDS_ROOT_FOLDER_NAME);
	ShellInitDialog(hDlg);
	HWND hTreeView = GetDlgItem(hDlg, IDC_TREE_FOLDER);

	m_bRefresh = FALSE;
	m_hImageList = NULL;
	HICON hIcon;

	//イメージリスト作成
	m_hImageList = ImageList_Create(16, 16, ILC_COLOR|ILC_MASK, 1, 2);

	hIcon = LoadIcon(GetInst(), (LPCTSTR)IDI_ROOT);
	ImageList_AddIcon(m_hImageList, hIcon);
	DeleteObject(hIcon);

	SHFILEINFO shfi;
	SHGetFileInfo(_T("\\Windows"), NULL, &shfi, sizeof(shfi), SHGFI_SMALLICON | SHGFI_ICON);
	ImageList_AddIcon(m_hImageList, shfi.hIcon);

	TreeView_SetImageList(hTreeView, m_hImageList, TVSIL_NORMAL);
	SetWindowLong(hTreeView, GWL_STYLE, GetWindowLong(hTreeView, GWL_STYLE)|TVS_HASBUTTONS|TVS_HASLINES|TVS_SHOWSELALWAYS);
	
	//フォルダ読み込み
	HCURSOR hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

	TVINSERTSTRUCT tvis;
	tvis.hParent = NULL;
	tvis.hInsertAfter = TVI_SORT;
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
	tvis.item.pszText = str;
	tvis.item.iImage = 0;
	tvis.item.iSelectedImage = 0;
	tvis.item.cChildren = 1;
	HTREEITEM hRoot = TreeView_InsertItem(hTreeView, &tvis);
	TreeView_Expand(hTreeView, hRoot, TVE_EXPAND);

	//カレントフォルダをセット
	SetCurrentFolder(hTreeView, m_pszPath);

	if (m_bSubFolder)
		SendMessage(GetDlgItem(hDlg, IDC_CHECK_SUBFOLDER), BM_SETCHECK, 1, 0);

	SetCursor(hCursor);
}
///////////////////////////////////////////////////////////////
void CFolderDlg::OnOK(HWND hDlg)
{
	HWND hTV = GetDlgItem(hDlg, IDC_TREE_FOLDER);

	TCHAR szPath[MAX_PATH] = _T("");
	TCHAR szTemp[MAX_PATH] = _T("");
	TCHAR szName[MAX_PATH] = _T("");
	
	HTREEITEM hTreeItem = TreeView_GetSelection(hTV);
	GetTree(hTV, hTreeItem, m_pszPath);

	m_bSubFolder = SendMessage(GetDlgItem(hDlg, IDC_CHECK_SUBFOLDER), BM_GETCHECK, 0, 0);
	EndDialog(hDlg, IDOK);
}
///////////////////////////////////////////////////////////////
void CFolderDlg::OnCancel(HWND hDlg)
{
	EndDialog(hDlg, IDCANCEL);
}
///////////////////////////////////////////////////////////////
LRESULT CALLBACK CFolderDlg::FolderDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static CFolderDlg* pDlg;
	switch (message)
	{
		case WM_INITDIALOG:
		{
			pDlg = (CFolderDlg*)lParam;
			pDlg->OnInitDialog(hDlg);
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) {
				case IDOK :
				{
					pDlg->OnOK(hDlg);
					return TRUE;
				}
				case IDCANCEL:
				{
					pDlg->OnCancel(hDlg);
					return TRUE;
				}
			}
			break;
		}
		case WM_NOTIFY:
		{
			NMHDR* lpnmh = (NMHDR*)lParam;
			if (lpnmh->code == TVN_ITEMEXPANDING) {
				pDlg->OnItemExpanding((NMTREEVIEW*)lpnmh);
				return TRUE;
			}
			else if (lpnmh->code == TVN_ITEMEXPANDED) {
				pDlg->OnItemExpanded((NMTREEVIEW*)lpnmh);
				return TRUE;
			}
			break;
		}
		//case WM_PAINT:
		//	return DefDlgPaintProc(hDlg, wParam, lParam);
		case WM_CTLCOLORSTATIC:
			return DefDlgCtlColorStaticProc(hDlg, wParam, lParam);
#ifdef _WIN32_WCE_PPC
		case WM_SIZE:
		{
			RECT rc;
			HWND hwnd;
			POINT pt;

#define CHECK_HEIGHT	22
#define CHECK_MARGIN	2

			hwnd = GetDlgItem(hDlg, IDCANCEL);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hDlg, &pt);
			MoveWindow(hwnd, LOWORD(lParam) - RECT_WIDTH(&rc) - 2, pt.y, RECT_WIDTH(&rc), RECT_HEIGHT(&rc), TRUE);

			hwnd = GetDlgItem(hDlg, IDC_TREE_FOLDER);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hDlg, &pt);
			MoveWindow(hwnd, pt.x, pt.y, LOWORD(lParam) - pt.x * 2, 
				HIWORD(lParam) - pt.y - (CHECK_HEIGHT + CHECK_MARGIN * 2), TRUE);

			hwnd = GetDlgItem(hDlg, IDC_CHECK_SUBFOLDER);
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left; pt.y = rc.top;
			ScreenToClient(hDlg, &pt);
			MoveWindow(hwnd, pt.x, HIWORD(lParam) - (CHECK_HEIGHT + CHECK_MARGIN), RECT_WIDTH(&rc), CHECK_HEIGHT, TRUE);

			return TRUE;
		}
#endif
	}
    return FALSE;
}
///////////////////////////////////////////////////////////////
int CFolderDlg::ShowFolderDlg(HWND hwndParent, LPTSTR szPath, int nSize)
{
	m_pszPath = szPath;
	m_nSize = nSize;
	return DialogBoxParam(GetInst(), (LPCTSTR)IDD_FOLDER_DLG, hwndParent, (DLGPROC)FolderDlgProc, (DWORD)this);
}

///////////////////////////////////////////////////////////////
// GSFinder + TQのコードを引っ張ってきた。
// Qtaさんありがとー！！
// -- Ex.Q --
// ツリー ビューの項目の絶対パスを求める
void CFolderDlg::GetTree(HWND hwndTV, HTREEITEM hItem, LPTSTR pszKey)
{
	CTempStr str(IDS_ROOT_FOLDER_NAME);
	TCHAR szName[MAX_PATH];

    TV_ITEM tvi;
    memset (&tvi, 0, sizeof (tvi));

	// 親を取得
    HTREEITEM hParent;
	hParent = TreeView_GetParent (hwndTV, hItem);
    if (hParent) { 
        // 項目の親の親・・・を取得する
        GetTree (hwndTV, hParent, pszKey);

        // 項目の名前を取得する
		tvi.mask       = TVIF_TEXT;
		tvi.hItem      = hItem;
		tvi.pszText    = szName;
		tvi.cchTextMax = MAX_PATH;
		TreeView_GetItem(hwndTV, &tvi);

		// ルートでなければ￥をつける
		if (wcscmp(pszKey, str) != 0)
			lstrcat (pszKey, TEXT("\\"));
		else
			lstrcpy (pszKey, TEXT("\\"));

		// 親のパスに今のフォルダ名を追加
		lstrcat (pszKey, szName);
	} else {
        // 項目の名前を取得する
		lstrcpy(pszKey, _T(""));
        szName[0]      = _T('\0');
        tvi.mask       = TVIF_TEXT | TVIF_PARAM;
        tvi.hItem      = hItem;
        tvi.pszText    = szName;
        tvi.cchTextMax = MAX_PATH;
		TreeView_GetItem(hwndTV, &tvi);
    }
}

///////////////////////////////////////////////////////////////
// CountChildren - キーのサブキーの数を調べる
//
int CFolderDlg::CountChildren(LPTSTR pszPath, LPTSTR pszKey)
{
	int rc;
	TCHAR szName[MAX_PATH];
	HANDLE ff;
	WIN32_FIND_DATA wfd;
    DWORD dwCnt = 0;
	wsprintf (szName, _T("%s\\%s\\*.*"), pszPath, pszKey);
	
	ff = FindFirstFile( szName, &wfd );
	if (ff != INVALID_HANDLE_VALUE) {
		while (1) {
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				dwCnt++;
				break;
			}
			rc = FindNextFile (ff, &wfd);
			if (rc == 0)
				break;
		}
		FindClose(ff);
    }
	return dwCnt;
}

///////////////////////////////////////////////////////////////
// EnumChildren - キーのサブキーを列挙する
int CFolderDlg::EnumChildren(HWND hwndTV, HTREEITEM hParent, LPTSTR pszPath)
{
	int rc;
    DWORD nChild;
	HANDLE ff;
	WIN32_FIND_DATA wfd;
	TCHAR szName[MAX_PATH];
	TVINSERTSTRUCT tvis;
	DWORD dwCnt = 0;

	wsprintf (szName, _T("%s\\*.*"), pszPath);

	ff = FindFirstFile (szName, &wfd);
	if (ff != INVALID_HANDLE_VALUE) {
		while (1) {
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ||
				wfd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) {
				// 下位のディレクトリがあるか検査
				nChild = CountChildren (pszPath, wfd.cFileName);

				// メモリカード？
				int nIndex = 1; // default icon index
				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) {
					wsprintf(szName, _T("%s\\%s"), pszPath, wfd.cFileName);
					SHFILEINFO shfi;
					SHGetFileInfo(szName, NULL, &shfi, sizeof(shfi), SHGFI_SMALLICON | SHGFI_ICON);
					nIndex = ImageList_AddIcon(m_hImageList, shfi.hIcon);
				}

				// ツリー ビュー コントロールにキーを挿入する
				tvis.hParent      = hParent;
				tvis.hInsertAfter = TVI_SORT;
				tvis.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
				tvis.item.pszText = wfd.cFileName;
				tvis.item.iImage  = nIndex;
				tvis.item.iSelectedImage = nIndex;
				tvis.item.cChildren = (nChild ? 1 : 0);
				TreeView_InsertItem(hwndTV, &tvis);

				dwCnt++;
			}
			rc = FindNextFile (ff, &wfd);
			if (rc == 0)
				break;
		}
		FindClose(ff);
	}

    // サブキーがない場合は展開ボタンを削除する
    TVITEM tvi;
	tvi.hItem = hParent;
	tvi.mask  = TVIF_CHILDREN;
    if (dwCnt == 0)
        tvi.cChildren = 0;
    else
        tvi.cChildren = 1;
	TreeView_SetItem (hwndTV, &tvi);

	// ソート
	TreeView_SortChildren(hwndTV, hParent, 0);

	return dwCnt;
}

///////////////////////////////////////////////////////////////
void CFolderDlg::OnItemExpanded(NMTREEVIEW* lpnmtv)
{
	// 次にブランチを展開するときに改めて列挙するのでサブキーを削除する
	if (lpnmtv->action == TVE_COLLAPSE) {
		HTREEITEM hChild;
		HTREEITEM hNext;

		hChild = TreeView_GetChild (lpnmtv->hdr.hwndFrom, lpnmtv->itemNew.hItem);
		while (hChild) {
			hNext = TreeView_GetNextItem (lpnmtv->hdr.hwndFrom, hChild, TVGN_NEXT);
			TreeView_DeleteItem (lpnmtv->hdr.hwndFrom, hChild);
			hChild = hNext;
		}
	}
}

///////////////////////////////////////////////////////////////
void CFolderDlg::OnItemExpanding(NMTREEVIEW* lpnmtv)
{
	if (lpnmtv->action == TVE_EXPAND) {
		TCHAR szKey[MAX_PATH];
		//フォルダ名を取得
		GetTree (lpnmtv->hdr.hwndFrom, lpnmtv->itemNew.hItem, szKey);
		//子ツリーを列挙する
		EnumChildren (lpnmtv->hdr.hwndFrom, lpnmtv->itemNew.hItem, szKey);
	}
}

void CFolderDlg::SetCurrentFolder(HWND hwndTV, LPCTSTR lpszPath)
{
	LPCTSTR pszStart = wcschr(lpszPath, _T('\\'));
	HTREEITEM hSelectItem = TreeView_GetRoot(hwndTV);

	HTREEITEM hParent = hSelectItem;
	TreeView_Expand(hwndTV, hParent, TVE_EXPAND);

	while (pszStart) {
		TCHAR szName[MAX_PATH] = {0};
		LPCTSTR pszEnd = wcschr(pszStart + 1, _T('\\'));
		if (pszEnd) {
			int len = pszEnd - (pszStart + 1);
			wcsncpy(szName, ++pszStart, len);
			szName[len] = 0;
		}
		else
			wcscpy(szName, ++pszStart);

		HTREEITEM hChild = TreeView_GetChild(hwndTV, hParent);
		while (hChild) {
			TCHAR szChildName[MAX_PATH] = {0};
			TVITEM tvi;
			tvi.mask = TVIF_TEXT | TVIF_HANDLE;
			tvi.pszText = szChildName;
			tvi.cchTextMax = MAX_PATH;
			tvi.hItem = hChild;
			TreeView_GetItem(hwndTV, &tvi);
			if (wcscmp(szName, szChildName) == 0) {
				hSelectItem = hParent = hChild;
				TreeView_Expand(hwndTV, hParent, TVE_EXPAND);
				break;
			}

			hChild = TreeView_GetNextItem(hwndTV, hChild, TVGN_NEXT);
		}
		if (!hChild) break;

		pszStart = wcschr(pszStart, _T('\\'));
	}

	TreeView_SelectItem(hwndTV, hSelectItem);
}

void CFolderDlg::CreateNewFolder(HWND)
{
}

void CFolderDlg::OnRenameFolder(HWND)
{
}

	