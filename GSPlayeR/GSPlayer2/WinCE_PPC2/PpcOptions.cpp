#include "GSPlayer2.h"
#include "PpcOptions.h"

#define MAX_KEYMAP			99
#define NAME_KEY_FVIRT		_T("KeyVirt%d")
#define NAME_KEY_CODE		_T("KeyCode%d")
#define NAME_KEY_COMMAND	_T("KeyCmd%d")
#define NAME_RELEASEKEYMAP	_T("ReleaseKeyMap")
#define NAME_DISPAUTOOFF	_T("DispAutoOff")
#define NAME_DISPAUTOON		_T("DispAutoOn")
#define NAME_DISPBATTERY	_T("DispEnableBattery")
#define KEY_KEYMAP			_T("Software\\GreenSoftware\\GSPlayer\\MP3\\Key")

static const int s_nCommands[] = {
	IDM_PLAY_PLAYPAUSE, IDM_PLAY_PLAYSTOP, IDM_PLAY_STOP, IDM_PLAY_PREV,
	IDM_PLAY_REW, IDM_PLAY_FF, IDM_PLAY_NEXT, IDM_PLAY_REPEAT, IDM_ENQUEUE, IDM_DEQUEUE, IDM_FILE_OPEN, 
	IDM_FILE_OPEN_LOCATION, IDM_FILE_ADD, IDM_FILE_FIND, IDM_FILE_LIST,
	IDM_FILE_SAVE_LIST, IDM_TOOL_EFFECT, IDM_TOOL_VOLUP, IDM_TOOL_VOLDOWN,
	IDM_TOOL_TOGGLEDISPLAY, IDM_TOOL_HOLD, IDM_TOOL_HOLDDISP, IDM_APP_EXIT, IDM_DELETE_KEYMAP
};

static const UINT s_uKeys[][3] = {
	{ID_KEY_RETURN, 0, VK_RETURN}, {ID_KEY_LEFT, 0, VK_LEFT},
	{ID_KEY_UP, 0, VK_UP}, {ID_KEY_RIGHT, 0, VK_RIGHT}, {ID_KEY_DOWN, 0, VK_DOWN},
	{ID_KEY_BTN1, MOD_WIN, VK_BTN1}, {ID_KEY_BTN2, MOD_WIN, VK_BTN2},
	{ID_KEY_BTN3, MOD_WIN, VK_BTN3}, {ID_KEY_BTN4, MOD_WIN, VK_BTN4},
	{ID_KEY_BTN5, MOD_WIN, VK_BTN5}, {ID_KEY_BTN6, MOD_WIN, VK_BTN6}, 
	{ID_KEY_REMBTN1, MOD_WIN, VK_CREMBTN1}, {ID_KEY_REMBTN2, MOD_WIN, VK_CREMBTN2},
	{ID_KEY_REMBTN3, MOD_WIN, VK_CREMBTN3}, {ID_KEY_REMBTN4, MOD_WIN, VK_CREMBTN4},
	{ID_KEY_REMBTN1, 0, VK_TREMBTN1}, {ID_KEY_REMBTN2, 0, VK_TREMBTN2},
	{ID_KEY_REMBTN3, 0, VK_TREMBTN3}, {ID_KEY_REMBTN4, 0, VK_TREMBTN4},
	{ID_KEY_REMBTN5, 0, VK_TREMBTN5}, {ID_KEY_REMBTN6, 0, VK_TREMBTN6},
	{ID_KEY_REMBTN7, 0, VK_TREMBTN7}
};

static const int s_nDispSecs[] = {0, 5, 10, 15, 20, 30, 60, 90, 120, 180, 300}; // 5の倍数

CPpcOptions::CPpcOptions()
{
	m_fReleaseKeyMap = TRUE;
	m_nDispAutoOff = 0;
	m_fDispAutoOn = FALSE;
	m_fDispEnableBattery = FALSE;
}

CPpcOptions::~CPpcOptions()
{
	DeleteKeyMap();
}

void CPpcOptions::Save(HANDLE hMap)
{
	COptions::Save(hMap);

	// レジストリキーを消す
	RegDeleteKey(HKEY_CURRENT_USER, KEY_KEYMAP);

	// キー割り当て
	HKEY hKey = 0;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, KEY_KEYMAP, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {

		DWORD dwBuf;
		TCHAR szName[MAX_PATH];
		for (int i = 0; i < m_listKeyMap.GetCount(); i++) {
			ACCEL* p = (ACCEL*)m_listKeyMap.GetAt(i);
			dwBuf = p->fVirt;
			wsprintf(szName, NAME_KEY_FVIRT, i);
			RegSetValueEx(hKey, szName, 0, REG_DWORD, (LPBYTE)&dwBuf, sizeof(DWORD));
			dwBuf = p->key;
			wsprintf(szName, NAME_KEY_CODE, i);
			RegSetValueEx(hKey, szName, 0, REG_DWORD, (LPBYTE)&dwBuf, sizeof(DWORD));
			dwBuf = p->cmd;
			wsprintf(szName, NAME_KEY_COMMAND, i);
			RegSetValueEx(hKey, szName, 0, REG_DWORD, (LPBYTE)&dwBuf, sizeof(DWORD));
		}
		RegSetValueEx(hKey, NAME_RELEASEKEYMAP, 0, REG_DWORD, (LPBYTE)&m_fReleaseKeyMap, sizeof(DWORD));
		RegCloseKey(hKey);
	}

	// 画面制御
	if (RegCreateKeyEx(HKEY_CURRENT_USER, KEY_SETTINGS, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {
		RegSetValueEx(hKey, NAME_DISPAUTOOFF, 0, REG_DWORD, (LPBYTE)&m_nDispAutoOff, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_DISPAUTOON, 0, REG_DWORD, (LPBYTE)&m_fDispAutoOn, sizeof(DWORD));
		RegSetValueEx(hKey, NAME_DISPBATTERY, 0, REG_DWORD, (LPBYTE)&m_fDispEnableBattery, sizeof(DWORD));
	}
}

void CPpcOptions::Load(HANDLE hMap)
{
	COptions::Load(hMap);

	// キー割り当て
	DeleteKeyMap();
	HKEY hKey = 0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, KEY_KEYMAP, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		TCHAR szName[MAX_PATH];
		DWORD dwType, dwSize;
		for (int i = 0; i < MAX_KEYMAP; i++) {
			int fVirt = -1, key = -1, cmd = -1;
			dwSize = sizeof(DWORD);

			wsprintf(szName, NAME_KEY_FVIRT, i);
			RegQueryValueEx(hKey, szName, 0, &dwType, (LPBYTE)&fVirt, &dwSize);
			wsprintf(szName, NAME_KEY_CODE, i);
			RegQueryValueEx(hKey, szName, 0, &dwType, (LPBYTE)&key, &dwSize);
			wsprintf(szName, NAME_KEY_COMMAND, i);
			RegQueryValueEx(hKey, szName, 0, &dwType, (LPBYTE)&cmd, &dwSize);

			if (fVirt != -1 && key != -1 && cmd != -1) {
				ACCEL* p = new ACCEL;
				p->fVirt = fVirt;
				p->key = key;
				p->cmd = cmd;
				m_listKeyMap.Add((DWORD)p);
			}
		}
		if (RegQueryValueEx(hKey, NAME_RELEASEKEYMAP, 0, &dwType, (LPBYTE)&m_fReleaseKeyMap, &dwSize) != ERROR_SUCCESS)
			m_fReleaseKeyMap = TRUE;
		RegCloseKey(hKey);
	}

	// 画面制御
	if (RegOpenKeyEx(HKEY_CURRENT_USER, KEY_SETTINGS, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD dwType, dwSize;
		dwSize = sizeof(DWORD);
		if (RegQueryValueEx(hKey, NAME_DISPAUTOOFF, 0, &dwType, (LPBYTE)&m_nDispAutoOff, &dwSize) != ERROR_SUCCESS)
			m_nDispAutoOff = 0;
		if (RegQueryValueEx(hKey, NAME_DISPAUTOON, 0, &dwType, (LPBYTE)&m_fDispAutoOn, &dwSize) != ERROR_SUCCESS)
			m_fDispAutoOn = FALSE;
		if (RegQueryValueEx(hKey, NAME_DISPBATTERY, 0, &dwType, (LPBYTE)&m_fDispEnableBattery, &dwSize) != ERROR_SUCCESS)
			m_fDispEnableBattery = FALSE;
		RegCloseKey(hKey);
	}

	/*if (m_listKeyMap.GetCount() == 0) {
		ACCEL accel[] = {
			{FVIRTKEY, VK_RETURN, IDM_PLAY_PLAYPAUSE},	// Play/Pause
			{FVIRTKEY, VK_LEFT, IDM_PLAY_PREV},			// Prev
			{FVIRTKEY, VK_RIGHT, IDM_PLAY_NEXT},		// Next
			{FVIRTKEY, VK_UP, IDM_TOOL_VOLUP},			// Volume Up
			{FVIRTKEY, VK_DOWN, IDM_TOOL_VOLDOWN},		// Volume Down
		};
		for (int i = 0; i < sizeof(accel) / sizeof(ACCEL); i++) {
			ACCEL* p = new ACCEL;
			*p = accel[i];
			m_listKeyMap.Add((DWORD)p);
		}
	}*/

	m_fTrayIcon = FALSE; // 最小化時のトレイアイコンは常にオン
}

int CPpcOptions::GetPropPages(PROPSHEETPAGE** ppPage)
{
	int nPages = COptions::GetPropPages(ppPage);

	*ppPage = (PROPSHEETPAGE*)realloc(*ppPage, sizeof(PROPSHEETPAGE) * (nPages + 2));
	(*ppPage)[nPages].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[nPages].dwFlags = PSP_DEFAULT;
	(*ppPage)[nPages].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_KEYMAP);
	(*ppPage)[nPages].pfnDlgProc = (DLGPROC)KeyMapPageProc;
	(*ppPage)[nPages].hInstance = GetInst();
	(*ppPage)[nPages].pszIcon = NULL;
	(*ppPage)[nPages].lParam = (LPARAM)this;
	nPages++;
	(*ppPage)[nPages].dwSize = sizeof(PROPSHEETPAGE);
	(*ppPage)[nPages].dwFlags = PSP_DEFAULT;
	(*ppPage)[nPages].pszTemplate = MAKEINTRESOURCE(IDD_PAGE_DISPLAY);
	(*ppPage)[nPages].pfnDlgProc = (DLGPROC)DisplayPageProc;
	(*ppPage)[nPages].hInstance = GetInst();
	(*ppPage)[nPages].pszIcon = NULL;
	(*ppPage)[nPages].lParam = (LPARAM)this;
	nPages++;
	
	return nPages;
}

void CPpcOptions::DeleteKeyMap()
{
	while (!m_listKeyMap.IsEmpty()) {
		ACCEL* p = (ACCEL*)m_listKeyMap.RemoveAt(0);
		delete p;
	}
}

BOOL CALLBACK CPpcOptions::KeyMapPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CPpcOptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (CPpcOptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->KeyMapPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDM_SHOW_MENU) {
			pOptions->KeyMapPageOnShowMenu(hwndDlg, LOWORD(lParam), HIWORD(lParam));
			return TRUE;
		}
		return FALSE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->KeyMapPageOnOK(hwndDlg);
				pOptions->KeyMapPageDeleteAllMap(hwndDlg);
				return TRUE;
			}
			if (pnmh->code == PSN_QUERYCANCEL) {
				pOptions->KeyMapPageDeleteAllMap(hwndDlg);
				pOptions->KeyMapPageUnregisterHotKeys(hwndDlg);
				return FALSE;
			}
			if (pnmh->code == PSN_SETACTIVE) {
				pOptions->KeyMapPageRegisterHotKeys(hwndDlg);
				return TRUE;
			}
			if (pnmh->code == PSN_KILLACTIVE) {
				pOptions->KeyMapPageUnregisterHotKeys(hwndDlg);
				return TRUE;
			}
			if (pnmh->code == GN_CONTEXTMENU) {
				NMRGINFO* p = (NMRGINFO*)lParam;
				PostMessage(hwndDlg, WM_COMMAND, IDM_SHOW_MENU, MAKELONG(p->ptAction.x, p->ptAction.y));
				return TRUE;
			}
			return FALSE;
		}
	case WM_HOTKEY:
		pOptions->KeyMapPageOnHotKey(hwndDlg, (int)wParam, 
			(UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
		return TRUE;
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	case WM_SIZE:
	{
		RECT rc, rcCheck;
		POINT pt;
		HWND hwnd;

		hwnd = GetDlgItem(hwndDlg, IDC_CMB_FUNC);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd, pt.x, pt.y, LOWORD(lParam) - pt.x - 8, RECT_HEIGHT(&rc), TRUE);

#define CHECK_MARGIN	2
		hwnd = GetDlgItem(hwndDlg, IDC_CHECK_RELEASE);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd, pt.x, 
			HIWORD(lParam) - (RECT_HEIGHT(&rc) + CHECK_MARGIN), RECT_WIDTH(&rc), RECT_HEIGHT(&rc), TRUE);

		rcCheck = rc;
		hwnd = GetDlgItem(hwndDlg, IDC_LIST_KEYMAP);
		GetWindowRect(hwnd, &rc);
		pt.x = rc.left; pt.y = rc.top;
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd, pt.x, pt.y, LOWORD(lParam) - pt.x * 2, 
			HIWORD(lParam) - pt.y - (RECT_HEIGHT(&rcCheck) + CHECK_MARGIN * 2), TRUE);

		GetWindowRect(hwnd, &rc);
		SendMessage(hwnd, LVM_SETCOLUMNWIDTH, 0, 
			MAKELPARAM((RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXBORDER) * 2) / 2, 0));
		SendMessage(hwnd, LVM_SETCOLUMNWIDTH, 1, 
			MAKELPARAM((RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXBORDER) * 2) / 2, 0));

		return TRUE;
	}
	default:
		return FALSE;
	}
}

#define COLUMN_KEY		0
#define COLUMN_COMMAND	1
void CPpcOptions::KeyMapPageOnInitDialog(HWND hwndDlg)
{
	int i;
	CTempStr str;
	// コンボボックスを初期化する
	HWND hwndCmb = GetDlgItem(hwndDlg, IDC_CMB_FUNC); 
	str.Load(IDS_TITLE_PREVIEW);
	SendMessage(hwndCmb, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)str);
	for (i = 0; i < sizeof(s_nCommands) / sizeof(int); i++) {
		str.Load(s_nCommands[i]);
		SendMessage(hwndCmb, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)str);
	}
	SendMessage(hwndCmb, CB_SETCURSEL, 0, 0);

	// ListViewを初期化する
	RECT rc;
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_KEYMAP);
	GetClientRect(hwndLV, &rc);
	int nWidth = (RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - 1) / 2;
	
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = nWidth;
	str.Load(IDS_KEY_COLUMN_KEY);
	lvc.pszText = str;
	lvc.iSubItem = COLUMN_KEY;
	ListView_InsertColumn(hwndLV, COLUMN_KEY, &lvc);
	str.Load(IDS_KEY_COLUMN_COMMAND);
	lvc.pszText = str;
	lvc.iSubItem = COLUMN_COMMAND;
	ListView_InsertColumn(hwndLV, COLUMN_COMMAND, &lvc);
	
	DWORD dwExStyle = ListView_GetExtendedListViewStyle(hwndLV);
	ListView_SetExtendedListViewStyle(hwndLV, dwExStyle | LVS_EX_FULLROWSELECT);

	// アイテムを追加する
	for (i = 0; i < m_listKeyMap.GetCount(); i++) {
		ACCEL* p = (ACCEL*)m_listKeyMap.GetAt(i);
		KeyMapPageAddMap(hwndDlg, p);
	}

	if (m_fReleaseKeyMap)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_RELEASE), BM_SETCHECK, 1, 0);
}

void CPpcOptions::KeyMapPageOnHotKey(HWND hwndDlg, int nId, UINT fuModifiers, UINT uVirtKey)
{
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_KEYMAP);
	HWND hwndCmb = GetDlgItem(hwndDlg, IDC_CMB_FUNC);

	int nSel = SendMessage(hwndCmb, CB_GETCURSEL, 0, 0);
	if (nSel != 0 && nSel != CB_ERR) {
		if (s_nCommands[nSel - 1] == IDM_DELETE_KEYMAP) {
			int nCount = ListView_GetItemCount(hwndLV);
			for (int i = 0; i < nCount; i++) {
				LVITEM li;
				memset(&li, 0, sizeof(LVITEM));
				li.mask = LVIF_PARAM;
				li.iItem = i;
				ListView_GetItem(hwndLV, &li);
				ACCEL* pac = (ACCEL*)li.lParam;
				if (pac->key == uVirtKey) {
					delete pac;
					ListView_DeleteItem(hwndLV, i);
					break;
				}
			}
		}
		else {
			ACCEL accel;
			accel.cmd = s_nCommands[nSel - 1];
			accel.key = uVirtKey;
			accel.fVirt = FVIRTKEY;
			if (fuModifiers & MOD_ALT)
				accel.fVirt |= FALT;
			if (fuModifiers & MOD_CONTROL)
				accel.fVirt |= FCONTROL;
			if (fuModifiers & MOD_SHIFT)
				accel.fVirt |= FSHIFT;
			if (fuModifiers & MOD_WIN)
				accel.fVirt |= FWIN;
			int nIndex = KeyMapPageAddMap(hwndDlg, &accel);
			ListView_SetItemState(hwndLV, nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED);
			ListView_EnsureVisible(hwndLV, nIndex, FALSE);
		}
	}
	SendMessage(hwndCmb, CB_SETCURSEL, 0, 0);
}

void CPpcOptions::KeyMapPageOnShowMenu(HWND hwndDlg, int x, int y)
{
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_KEYMAP);
	if (!ListView_GetSelectedCount(hwndLV))
		return;

	HMENU hMenu = LoadMenu(GetInst(), (LPCTSTR)IDR_KEYMAP);
	HMENU hSub = GetSubMenu(hMenu, 0);
	if (TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD, 
							x, y, 0, hwndDlg, NULL) == IDM_DELETE_KEYMAP) {
		
		int nCount = ListView_GetItemCount(hwndLV);
		for (int i = 0; i < nCount; i++) {
			if (ListView_GetItemState(hwndLV, i, LVIS_SELECTED)) {
				LVITEM li;
				memset(&li, 0, sizeof(LVITEM));
				li.mask = LVIF_PARAM;
				li.iItem = i;
				ListView_GetItem(hwndLV, &li);
				delete (ACCEL*)li.lParam;
				ListView_DeleteItem(hwndLV, i);
				break;
			}
		}
	}
	DestroyMenu(hMenu);
}

void CPpcOptions::KeyMapPageOnOK(HWND hwndDlg)
{
	DeleteKeyMap();

	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_KEYMAP);
	int nCount = ListView_GetItemCount(hwndLV);
	for (int i = 0; i < nCount; i++) {
		LVITEM li;
		memset(&li, 0, sizeof(LVITEM));
		li.mask = LVIF_PARAM;
		li.iItem = i;
		ListView_GetItem(hwndLV, &li);

		ACCEL* p = new ACCEL;
		*p = *((ACCEL*)li.lParam);
		m_listKeyMap.Add((DWORD)p);
	}
	m_fReleaseKeyMap = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_RELEASE), BM_GETCHECK, 0, 0);
}

void CPpcOptions::KeyMapPageDeleteAllMap(HWND hwndDlg)
{
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_KEYMAP);
	int nCount = ListView_GetItemCount(hwndLV);
	for (int i = 0; i < nCount; i++) {
		LVITEM li;
		memset(&li, 0, sizeof(LVITEM));
		li.mask = LVIF_PARAM;
		li.iItem = i;
		ListView_GetItem(hwndLV, &li);
		
		ACCEL* p = (ACCEL*)li.lParam;
		delete p;
	}
	ListView_DeleteAllItems(hwndLV);
}

int CPpcOptions::KeyMapPageAddMap(HWND hwndDlg, ACCEL* pAccel)
{
	TCHAR szBuf[MAX_PATH];
	HWND hwndLV = GetDlgItem(hwndDlg, IDC_LIST_KEYMAP);

	// 重複チェック
	int nCount = ListView_GetItemCount(hwndLV);
	for (int i = 0; i < nCount; i++) {
		LVITEM li;
		memset(&li, 0, sizeof(LVITEM));
		li.mask = LVIF_PARAM;
		li.iItem = i;
		ListView_GetItem(hwndLV, &li);

		ACCEL* p = (ACCEL*)li.lParam;
		if (p->fVirt == pAccel->fVirt && p->key == pAccel->key) {
			p->cmd = pAccel->cmd;
			// command文字列置き換え
			KeyMapPageCreateCommandString(p->cmd, szBuf);
			ListView_SetItemText(hwndLV, i, COLUMN_COMMAND, szBuf);
			return i;
		}
	}

	// 新規追加
	ACCEL* p = new ACCEL;
	*p = *pAccel;

	LVITEM li;
	memset(&li, 0, sizeof(LVITEM));
	li.mask = LVIF_PARAM;
	li.iItem = nCount;
	li.lParam = (long)p;
	ListView_InsertItem(hwndLV, &li);

	// key文字列作成
	KeyMapPageCreateKeyString(p->key, szBuf);
	ListView_SetItemText(hwndLV, nCount, COLUMN_KEY, szBuf);

	// command文字列作成
	KeyMapPageCreateCommandString(p->cmd, szBuf);
	ListView_SetItemText(hwndLV, nCount, COLUMN_COMMAND, szBuf);

	return nCount;
}

void CPpcOptions::KeyMapPageCreateKeyString(UINT uKey, LPTSTR psz)
{
	for (int i = 0; i < sizeof(s_uKeys) / sizeof(UINT) / 3; i++) {
		if (s_uKeys[i][2] == uKey) {
			CTempStr str(s_uKeys[i][0]);
			_tcscpy(psz, str);
			return;
		}
	}
	wsprintf(psz, _T("0x%dX"), uKey);
}

void CPpcOptions::KeyMapPageCreateCommandString(int nCommand, LPTSTR psz)
{
	CTempStr str(nCommand);
	_tcscpy(psz, str);
}

void CPpcOptions::KeyMapPageRegisterHotKeys(HWND hwndDlg)
{
	for (int i = 0; i < sizeof(s_uKeys) / sizeof(UINT) / 3; i++) {
		RegisterHotKey(hwndDlg, s_uKeys[i][2], s_uKeys[i][1], s_uKeys[i][2]);
	}
}

void CPpcOptions::KeyMapPageUnregisterHotKeys(HWND hwndDlg)
{
	for (int i = 0; i < sizeof(s_uKeys) / sizeof(UINT) / 3; i++) {
		UnregisterHotKey(hwndDlg, s_uKeys[i][2]);
	}
}

BOOL CALLBACK CPpcOptions::DisplayPageProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CPpcOptions* pOptions;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
		pOptions = (CPpcOptions*)((LPPROPSHEETPAGE)lParam)->lParam;
		pOptions->DisplayPageOnInitDialog(hwndDlg);
		return TRUE;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam; 
			if (pnmh->code == PSN_APPLY) {
				pOptions->DisplayPageOnOK(hwndDlg);
				return TRUE;
			}
			return FALSE;
		}
	case WM_PAINT:
		return DefDlgPaintProc(hwndDlg, wParam, lParam);
	case WM_CTLCOLORSTATIC:
		return DefDlgCtlColorStaticProc(hwndDlg, wParam, lParam);
	default:
		return FALSE;
	}
}

void CPpcOptions::DisplayPageOnInitDialog(HWND hwndDlg)
{
	HWND hwndCmb = GetDlgItem(hwndDlg, IDC_CMB_AUTOOFF);
	for (int i = 0; i < sizeof(s_nDispSecs) / sizeof(int); i++) {
		if (i == 0) {
			CTempStr str(IDS_FMT_DISPLAY_AUTOOFF_NONE);
			SendMessage(hwndCmb, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)str);
		}
		else {
			TCHAR sz[32];
			CTempStr str(IDS_FMT_DISPLAY_AUTOOFF);
			wsprintf(sz, str, s_nDispSecs[i]);
			SendMessage(hwndCmb, CB_ADDSTRING, 0, (LPARAM)sz);
		}
		if (m_nDispAutoOff == s_nDispSecs[i])
			SendMessage(hwndCmb, CB_SETCURSEL, i, 0);
	}

	if (m_fDispAutoOn)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_AUTOON), BM_SETCHECK, 1, 0);
	if (m_fDispEnableBattery)
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ENABLE_BATTERY), BM_SETCHECK, 1, 0);
}

void CPpcOptions::DisplayPageOnOK(HWND hwndDlg)
{
	HWND hwndCmb = GetDlgItem(hwndDlg, IDC_CMB_AUTOOFF);
	int nSel = SendMessage(hwndCmb, CB_GETCURSEL, 0, 0);
	if (nSel == CB_ERR)
		return;

	m_nDispAutoOff = s_nDispSecs[nSel];
	m_fDispAutoOn = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_AUTOON), BM_GETCHECK, 0, 0);
	m_fDispEnableBattery = SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ENABLE_BATTERY), BM_GETCHECK, 0, 0);
}