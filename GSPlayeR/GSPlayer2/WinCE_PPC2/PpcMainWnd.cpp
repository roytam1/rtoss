#include "GSPlayer2.h"
#include "PpcMainWnd.h"
#include "PpcOptions.h"
#include "PpcSkin.h"
#include "PpcDisplay.h"

extern const TCHAR s_szSupportedExt[][5];

static const UINT s_uHoldKeys[][2] = {
	{VK_RETURN, 0}, {VK_LEFT, 0},
	{VK_UP, 0}, {VK_RIGHT, 0}, {VK_DOWN, 0},
	{VK_BTN1, MOD_WIN}, {VK_BTN2, MOD_WIN},
	{VK_BTN3, MOD_WIN}, {VK_BTN4, MOD_WIN},
	{VK_BTN5, MOD_WIN}, {VK_BTN6, MOD_WIN}, 
};

CPpcMainWnd::CPpcMainWnd()
{
	m_fMenuLoop = FALSE;
	m_fActive = FALSE;
	m_fHotKey = FALSE;
	m_dwLastOperation = 0;
	m_fDisplay = GetDisplayStatus();

	m_hwndCB = NULL;
	m_hwndLV = NULL;
	m_fListDrag = FALSE;
	m_fHold = FALSE;
	m_fNoListRedraw = FALSE;

	InitSize();
}

CPpcMainWnd::~CPpcMainWnd()
{
}

COptions* CPpcMainWnd::GetOptionsClass()
{
	return new CPpcOptions();
}

CSkin* CPpcMainWnd::GetSkinClass()
{
	return new CPpcSkin();
}

HACCEL CPpcMainWnd::GetAccelHandle()
{
	if (!m_hAccel) {
		ACCEL accel[] = {
			{FVIRTKEY | FCONTROL, 'Z', IDM_PLAY_PLAYPAUSE},		// Play/Pause
			{FVIRTKEY | FCONTROL, 'X', IDM_PLAY_STOP},				// Stop
			{FVIRTKEY | FCONTROL, 'C', IDM_PLAY_PREV},				// Prev
			{FVIRTKEY | FCONTROL, 'V', IDM_PLAY_REW},				// Rew
			{FVIRTKEY | FCONTROL, 'B', IDM_PLAY_FF},				// FF
			{FVIRTKEY | FCONTROL, 'N', IDM_PLAY_NEXT},				// Next
			{FVIRTKEY | FCONTROL, 'R', IDM_PLAY_REPEAT},			// Repeat
			{FVIRTKEY | FCONTROL, 'H', IDM_PLAY_SHUFFLE},			// Shuffle
			{FVIRTKEY | FCONTROL, 'O', IDM_FILE_OPEN},				// Open
			{FVIRTKEY | FCONTROL, 'U', IDM_FILE_OPEN_LOCATION},	// Open Location
			{FVIRTKEY | FCONTROL, 'A', IDM_FILE_ADD},				// Add
			{FVIRTKEY | FCONTROL, 'F', IDM_FILE_FIND},				// Find
			{FVIRTKEY | FCONTROL, 'L', IDM_FILE_LIST},				// List
			{FVIRTKEY | FCONTROL, 'S', IDM_FILE_SAVE_LIST},		// Save List
			{FVIRTKEY | FCONTROL, 'E', IDM_TOOL_EFFECT},			// Effect
			{FVIRTKEY | FCONTROL, 'Y', IDM_TOOL_VOLUP},			// Volume Up
			{FVIRTKEY | FCONTROL, 'T', IDM_TOOL_VOLDOWN},		// Volume Down
			{FVIRTKEY | FCONTROL, 'Q', IDM_APP_EXIT},	// Exit
		};
		m_hAccel = CreateAcceleratorTable(accel, sizeof(accel) / sizeof(ACCEL));
	}
	return m_hAccel;
}

BOOL CPpcMainWnd::CanTransAccel(MSG* pMsg)
{
	return pMsg->hwnd == m_hWnd || pMsg->hwnd == m_hwndLV;
}

void CPpcMainWnd::RegisterClass()
{
	WNDCLASS	wc;
	memset(&wc, 0, sizeof(WNDCLASS));
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)PpcMainWndProc;
    wc.hInstance = GetInst();
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wc.lpszClassName = MAINWND_CLASS_NAME;
	::RegisterClass(&wc);
}

LRESULT CALLBACK CPpcMainWnd::PpcMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CPpcMainWnd* pMainWnd = NULL;

	if (pMainWnd &&
		uMsg != WM_TIMER && uMsg != WM_PAINT && uMsg != WM_GETTEXT &&
		uMsg != WM_ERASEBKGND && uMsg < MAP_MSG_BASE) {
		pMainWnd->m_dwLastOperation = GetTickCount();
	}

	switch (uMsg) {
	case WM_CREATE:
		CREATESTRUCT* pcs;
		pcs = (CREATESTRUCT*)lParam;
		pMainWnd = (CPpcMainWnd*)pcs->lpCreateParams;
		return CMainWnd::MainWndProc(hwnd, uMsg, wParam, lParam);
	case WM_INITMENUPOPUP:
		pMainWnd->OnInitMenuPopup((HMENU)wParam, LOWORD(lParam), HIWORD(lParam));
		return CMainWnd::MainWndProc(hwnd, uMsg, wParam, lParam);
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam;
			switch (pnmh->code) {
			case NM_RETURN:
				pMainWnd->OnListViewReturn(pnmh);
				return 0;
			case LVN_KEYDOWN:
				pMainWnd->OnListViewKeyDown(pnmh);
				return 0;
			case NM_DBLCLK:
				pMainWnd->OnListViewDblClk(pnmh);
				return 0;
			case LVN_BEGINDRAG:
				pMainWnd->OnListViewBeginDrag(pnmh);
				return 0;
			case GN_CONTEXTMENU:
				pMainWnd->OnListViewConextMenu(pnmh);
				return 0;
			default:
				return CMainWnd::MainWndProc(hwnd, uMsg, wParam, lParam);
			}
		}
	case WM_SETTINGCHANGE:
		SHHandleWMSettingChange(hwnd, wParam, lParam, &pMainWnd->m_sai);
		return CMainWnd::MainWndProc(hwnd, uMsg, wParam, lParam);
	case WM_ENTERMENULOOP:
		pMainWnd->m_fMenuLoop = TRUE;
		return 0;
	case WM_EXITMENULOOP:
		pMainWnd->m_fMenuLoop = FALSE;
		return 0;
	}
	return CMainWnd::MainWndProc(hwnd, uMsg, wParam, lParam);
}

BOOL CPpcMainWnd::Create(LPTSTR pszCmdLine, HWND hwndParent, DWORD dwStyle)
{
	CWaitCursor wc;

	// クラスの登録
	RegisterClass();

	// ウインドウ作成
	CreateWindow(MAINWND_CLASS_NAME, MAINWND_TITLE, WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 
		PPC_MAINWND_WIDTH, PPC_MAINWND_HEIGHT, NULL, NULL, GetInst(), this);


	if (!m_hWnd)
		return FALSE;

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	if (_tcslen(pszCmdLine)) {
		SendCmdLine(m_hWnd, pszCmdLine);
	}
	else {
		// デフォルトリストの読み込み
		if (((CPpcOptions*)m_pOptions)->m_fSaveDefList) {
			m_nCurrent = 0;	// 開かないようにする

			// 読み込み
			m_fNoListRedraw = TRUE;
			SendMessage(m_hwndLV, WM_SETREDRAW, 0, 0);
			TCHAR szPath[MAX_PATH];
			GetDefPlayListPath(szPath);
			LoadPlayList(szPath);
			SendMessage(m_hwndLV, WM_SETREDRAW, 1, 0);
			m_fNoListRedraw = FALSE;
			InvalidateRect(m_hwndLV, 0, FALSE);
			UpdateWindow(m_hwndLV);

			m_nCurrent = OPEN_NONE;

			// レジューム情報を反映する
			if (((CPpcOptions*)m_pOptions)->m_fResume && 
				((CPpcOptions*)m_pOptions)->m_nResumeTrack != -1 &&
				((CPpcOptions*)m_pOptions)->m_nResumeTrack < m_pListFile->GetCount()) {
				Open(((CPpcOptions*)m_pOptions)->m_nResumeTrack);
				if (m_nCurrent == OPEN_NONE)
					OpenFirstFile();
				else {
					MAP_Seek(m_hMap, ((CPpcOptions*)m_pOptions)->m_nResumeTime);
					((CPpcOptions*)m_pOptions)->m_nResumeTime = 0;
				}
			}
			else
				OpenFirstFile();
			if (((CPpcOptions*)m_pOptions)->m_fPlayOnStart)
				Play();
		}
	}

	return TRUE;
}

BOOL CPpcMainWnd::InitImages()
{
	InitSize();
	BOOL fRet = CMainWnd::InitImages();
	if (fRet) {
		ShowWindow(m_hwndLV, ((CPpcSkin*)m_pSkin)->IsPlaylistVisible() ? SW_SHOW : SW_HIDE);
		ListView_SetBkColor(m_hwndLV, ((CPpcSkin*)m_pSkin)->GetColor2(COLOR2_PLAYLISTBACK));
		ListView_SetTextBkColor(m_hwndLV, ((CPpcSkin*)m_pSkin)->GetColor2(COLOR2_PLAYLISTBACK));
		ListView_SetTextColor(m_hwndLV, ((CPpcSkin*)m_pSkin)->GetColor2(COLOR2_PLAYLISTTEXT));

		m_hFntTitle = (HFONT)SendMessage(m_hWnd, WM_GETFONT, 0, 0);

		InvalidateRect(m_hwndLV, NULL, TRUE);
		UpdateWindow(m_hwndLV);

		// ボリュームスライダーが有効か無効か
		if (m_pSkin->GetImageList(IMAGELIST_VOLSLIDER) && m_pSkin->GetImageList(IMAGELIST_VOLTRACK)) {
			m_nVolumeSliderLeft = VOLSLIDER_LEFT;
			m_nVolumeSliderTop = PPC_VOLSLIDER_TOP;
			m_nVolumeSliderWidth = VOLSLIDER_WIDTH;
			m_nVolumeSliderHeight = VOLSLIDER_HEIGHT;
		}
		else {
			m_nVolumeSliderLeft = 0;
			m_nVolumeSliderTop = 0;
			m_nVolumeSliderWidth = 0;
			m_nVolumeSliderHeight = 0;
		}

		m_nBtnMax = PPC_BTN_MAX;
	}

	return fRet;
}

void CPpcMainWnd::InitSize()
{
	m_nWidth = PPC_MAINWND_WIDTH;
	m_nHeight = PPC_MAINWND_HEIGHT;
	m_nClientWidth = PPC_MAINWND_WIDTH;
	m_nClientHeight = PPC_MAINWND_HEIGHT;
	m_nDispWidth = PPC_DISP_WIDTH;
	m_nDispHeight = PPC_DISP_HEIGHT;
	m_nBtnTop = PPC_BTN_TOP;
	m_nBtnMax = PPC_BTN_MAX;
	m_nStatusTop = PPC_STATUS_TOP;
	m_nStatusHeight = PPC_STATUS_HEIGHT;
	m_nTitleTop = PPC_TITLE_TOP;
	m_nTitleWidth = PPC_TITLE_WIDTH;
	m_nTitleHeight = PPC_TITLE_HEIGHT;
	m_nSeekTop = PPC_SEEK_TOP;
	m_nSeekWidth = PPC_SEEK_WIDTH;
	m_nStreamTitleTop = PPC_STREAM_TITLE_TOP;
	m_nStreamTitleWidth = PPC_STREAM_TITLE_WIDTH;
	m_nStreamTitleHeight = PPC_STREAM_TITLE_HEIGHT;
	m_nVolumeSliderLeft = VOLSLIDER_LEFT;
	m_nVolumeSliderTop = PPC_VOLSLIDER_TOP;
	m_nVolumeSliderWidth = VOLSLIDER_WIDTH;
	m_nVolumeSliderHeight = VOLSLIDER_HEIGHT;
	m_nVolumeTrackWidth = VOLTRACK_WIDTH;
}

void CPpcMainWnd::OnCreate(HWND hwnd)
{
	m_hWnd = hwnd;
	CreateMenuBar();
	CreateListView();

	CMainWnd::OnCreate(hwnd);

	RegisterHotKeys();
	SetTimer(m_hWnd, ID_TIMER_DISPCHECK, TIMER_DISPCHECK_INTERVAL, NULL);
	SetTimer(m_hWnd, ID_TIMER_DISPSTATE, TIMER_DISPSTATE_INTERVAL, NULL);

	memset(&m_sai, 0, sizeof(m_sai));
	m_sai.cbSize = sizeof(m_sai);

	SetFocus(m_hwndLV);
	ShowWindow(m_hwndLV, SW_HIDE);
}

void CPpcMainWnd::OnClose()
{
	KillTimer(m_hWnd, ID_TIMER_DISPCHECK);
	UnregisterHotKeys();
	CMainWnd::OnClose();
}

LRESULT CPpcMainWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam)) {
	case IDM_TOOL_TOGGLEDISPLAY:
		ToggleDisplay();
		OnTimer(ID_TIMER_DISPSTATE);
		return 0;
	case IDM_FILE_UP:
		OnFileUp();
		return 0;
	case IDM_FILE_DOWN:
		OnFileDown();
		return 0;
	case IDM_FILE_DELETE:
		OnFileDelete();
		return 0;
	case IDM_FILE_DELETEALL:
		OnFileDeleteAll();
		return 0;
	case IDM_PLAY_SELECTED:
		OnPlaySelected();
		return 0;
	case IDM_FILE_SORT_BYNAME:
	case IDM_FILE_SORT_BYPATH:
		OnFileSort(LOWORD(wParam));
		return 0;
	case IDOK:
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		return 0;
	case IDM_TOOL_HOLD:
		OnToolHold();
		return 0;
	case IDM_TOOL_HOLDDISP:
		OnToolHoldDisp();
		return 0;
	case IDM_PLAY_PLAYSTOP:
		OnPlayPlayStop();
		return 0;
	case IDM_ENQUEUE:
		OnEnqueue();
		return 0;
	case IDM_DEQUEUE:
		OnDequeue();
		return 0;
	case IDM_CLEARQUEUE:
		OnClearQueue();
		return 0;
	}
	return CMainWnd::OnCommand(wParam, lParam);
}

void CPpcMainWnd::OnActivate(int nActive, BOOL fMinimized, HWND hwndPrevious)
{
	m_fActive = nActive == WA_INACTIVE ? FALSE : TRUE;

	if (!m_pOptions)
		return;

	if (((CPpcOptions*)m_pOptions)->m_fReleaseKeyMap && !m_fHold) {
		if (nActive == WA_INACTIVE)
			UnregisterHotKeys();
		else
			RegisterHotKeys();
	}

	SHHandleWMActivate(m_hWnd, MAKELONG(nActive, fMinimized), (LONG)hwndPrevious, &m_sai, FALSE);
}

void CPpcMainWnd::OnHotKey(int nId, UINT fuModifiers, UINT uVirtKey)
{
	// 未処理のWM_HOTKEYを削除する
	MSG msg;
	while (PeekMessage(&msg, m_hWnd, WM_HOTKEY, WM_HOTKEY, PM_REMOVE));

	CPpcOptions* pOptions = (CPpcOptions*)m_pOptions;
	if (!pOptions) return;

	// 登録情報を参照する
	for (int i = 0; i < pOptions->m_listKeyMap.GetCount(); i++) {
		ACCEL* p = (ACCEL*)pOptions->m_listKeyMap.GetAt(i);
		if (p->key == uVirtKey) {
			// モーダルダイアログが表示されている場合は
			// ダイアログをだすもの、メニューを出すものを処理しない
			if (m_fMenuLoop || m_hMenu || (GetWindowLong(m_hWnd, GWL_STYLE) & WS_DISABLED)) {
				switch (p->cmd) {
				case IDM_PLAY_PLAYPAUSE:
				case IDM_PLAY_STOP:
				case IDM_PLAY_PREV:
				case IDM_PLAY_REW:
				case IDM_PLAY_FF:
				case IDM_PLAY_NEXT:
				case IDM_PLAY_REPEAT:
				case IDM_TOOL_VOLUP:
				case IDM_TOOL_VOLDOWN:
				case IDM_TOOL_TOGGLEDISPLAY:
				case IDM_TOOL_HOLD:
				case IDM_TOOL_HOLDDISP:
				case IDM_PLAY_PLAYSTOP:
				case IDM_ENQUEUE:
				case IDM_DEQUEUE:
					break;
				default: return;
				}
			}

			if (m_fHold) {
				BOOL fHold = FALSE;
				ACCEL* p = (ACCEL*)pOptions->m_listKeyMap.GetAt(i);
				for (int j = 0; j < sizeof(s_uHoldKeys) / sizeof(UINT) / 2; j++) {
					if (s_uHoldKeys[j][0] == p->key) {
						fHold = TRUE;
						break;
					}
				}

				if (p->cmd == IDM_TOOL_TOGGLEDISPLAY ||
					p->cmd == IDM_TOOL_HOLD ||
					p->cmd == IDM_TOOL_HOLDDISP || !fHold)
					PostMessage(m_hWnd, WM_COMMAND, p->cmd, 0);
				else {
					KillTimer(m_hWnd, ID_TIMER_HOLD);
					m_fDrawHold = TRUE;
					SetTimer(m_hWnd, ID_TIMER_HOLD, TIMER_HOLD_INTERVAL, NULL);
					UpdateTitle();
				}
			}
			else PostMessage(m_hWnd, WM_COMMAND, p->cmd, 0);

			// バックライト制御以外で
			// 自動バックライトON制御が有効のとき
			if (pOptions->m_fDispAutoOn &&
				(p->cmd != IDM_TOOL_TOGGLEDISPLAY && p->cmd != IDM_TOOL_HOLDDISP)) {
				TurnOnDisplay();
				m_fDisplay = TRUE;

				InvalidateRect(m_hWnd, NULL, TRUE);
				UpdateWindow(m_hWnd);
			}
			return;
		}
	}

	if (m_fHold) {
		KillTimer(m_hWnd, ID_TIMER_HOLD);
		m_fDrawHold = TRUE;
		SetTimer(m_hWnd, ID_TIMER_HOLD, TIMER_HOLD_INTERVAL, NULL);
		UpdateTitle();
	}
}

void CPpcMainWnd::OnTimer(UINT uID)
{
	CMainWnd::OnTimer(uID);
	if (uID == ID_TIMER_DISPCHECK) {
		// 自動画面OFFのチェック
		if (!m_pOptions)
			return;

		// フラグのチェック
		if (!((CPpcOptions*)m_pOptions)->m_nDispAutoOff) {
			m_dwLastOperation = GetTickCount();
			return;
		}

		// フォーカスがない場合
		if (!m_fActive) {
			m_dwLastOperation = GetTickCount();
			return;
		}

		// バッテリ状態チェック
		if (((CPpcOptions*)m_pOptions)->m_fDispEnableBattery) {
			SYSTEM_POWER_STATUS_EX spse;
			GetSystemPowerStatusEx(&spse, FALSE);
			if (spse.ACLineStatus == 1) {
				m_dwLastOperation = GetTickCount();
				return;
			}
		}

		// 既にオフの場合
		if (!GetDisplayStatus()) {
			m_dwLastOperation = GetTickCount();
			return;
		}

		// 再生中のチェック
		if (MAP_GetStatus(m_hMap) != MAP_STATUS_PLAY) {
			m_dwLastOperation = GetTickCount();
			return;
		}

		// カウントUp & オフ
		if (((int)(GetTickCount() - m_dwLastOperation) / 1000) >= ((CPpcOptions*)m_pOptions)->m_nDispAutoOff) {
			TurnOffDisplay();
			m_fDisplay = FALSE;
			m_dwLastOperation = GetTickCount();
		}
	}
	else if (uID == ID_TIMER_DISPSTATE) {
		// ステータス更新のために呼んでおく
		BOOL fRet = GetDisplayStatus();

		if (fRet && m_fDisplay != fRet) {
			InvalidateRect(m_hWnd, NULL, TRUE);
			UpdateWindow(m_hWnd);
		}
		m_fDisplay = fRet;
	}
	else if (uID == ID_TIMER_HOLD) {
		KillTimer(m_hWnd, ID_TIMER_HOLD);
		m_fDrawHold = FALSE;
		UpdateTitle();
	}
}

void CPpcMainWnd::OnSize(UINT nType, int cx, int cy)
{
	DeleteImages();
	InitImages();

	RECT rc;
	MoveWindow(m_hwndLV, PPC_PLAYLIST_LEFT, PPC_PLAYLIST_TOP, PPC_PLAYLIST_WIDTH, PPC_PLAYLIST_HEIGHT, TRUE);
	GetWindowRect(m_hwndLV, &rc);
	SendMessage(m_hwndLV, LVM_SETCOLUMNWIDTH, 0, 
		MAKELPARAM(RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXBORDER) * 2, 0));

	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

void CPpcMainWnd::OnLButtonDown(int x, int y)
{
	CMainWnd::OnLButtonDown(x, y);
}

void CPpcMainWnd::OnMouseMove(int fKey, int x, int y)
{
	if (m_fListDrag) {
		RECT rcItem;
		if (!ListView_GetItemRect(m_hwndLV, 0, &rcItem, LVIR_BOUNDS))
			return;

		POINT pt = {x, y};
		ClientToScreen(m_hWnd, &pt);
		ScreenToClient(m_hwndLV, &pt);

		RECT rcLV;
		GetClientRect(m_hwndLV, &rcLV);
		if (pt.y < RECT_HEIGHT(&rcItem)) {
			ListView_Scroll(m_hwndLV, 0, -RECT_HEIGHT(&rcItem));
			OnFileUp();
			m_nListDragItem = max(m_nListDragItem - 1, 0);
		}
		else if (pt.y > rcLV.bottom - RECT_HEIGHT(&rcItem)) {
			ListView_Scroll(m_hwndLV, 0, RECT_HEIGHT(&rcItem));
			OnFileDown();
			m_nListDragItem = min(m_nListDragItem + 1, ListView_GetItemCount(m_hwndLV) - 1);
		}
		else {
			LVHITTESTINFO lvhti;
			lvhti.pt.x = pt.x; 
			lvhti.pt.y = pt.y;
			int nItem = ListView_HitTest(m_hwndLV, &lvhti);
			if (nItem == -1)
				return;

			if (nItem - m_nListDragItem > 0) {
				for (int i = 0; i < nItem - m_nListDragItem; i++)
					OnFileDown();
				m_nListDragItem = nItem;
			}
			else if (nItem - m_nListDragItem < 0) {
				for (int i = 0; i < m_nListDragItem - nItem; i++)
					OnFileUp();
				m_nListDragItem = nItem;
			}
		}
	}
	else
		CMainWnd::OnMouseMove(fKey, x, y);
}

void CPpcMainWnd::OnLButtonUp(int x, int y)
{
	if (m_fListDrag) {
		ReleaseCapture();
		m_fListDrag = FALSE;
	}
	else
		CMainWnd::OnLButtonUp(x, y);
}

void CPpcMainWnd::OnInitMenuPopup(HMENU hMenu, UINT uPos, BOOL fSystemMenu)
{
	if (uPos == IDM_PLAY) {
		switch (((CPpcOptions*)m_pOptions)->m_fRepeat) {
		case COptions::RepeatAll:
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_NONE, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_ALL, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_ONE, MF_BYCOMMAND | MF_UNCHECKED);
			break;
		case COptions::RepeatOne:
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_NONE, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_ALL, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_ONE, MF_BYCOMMAND | MF_CHECKED);
			break;
		default:
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_NONE, MF_BYCOMMAND | MF_CHECKED);
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_ALL, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(hMenu, IDM_PLAY_REPEAT_ONE, MF_BYCOMMAND | MF_UNCHECKED);
			break;
		}
		CheckMenuItem(hMenu, IDM_PLAY_SHUFFLE,
			((CPpcOptions*)m_pOptions)->m_fShuffle ? MF_BYCOMMAND | MF_CHECKED : MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(hMenu, IDM_PLAY_STOPAFTERCURR,
			m_fStopAfterCurr ? MF_BYCOMMAND | MF_CHECKED : MF_BYCOMMAND | MF_UNCHECKED);
	}
}

void CPpcMainWnd::OnFileList()
{
	SetForegroundWindow(m_hWnd);
	CMainWnd::OnFileList();
}

void CPpcMainWnd::OnFileSort(UINT uID)
{
	int nSort = SORTFILE_BYNAME;
	switch (uID) {
		case IDM_FILE_SORT_BYNAME:
			nSort = SORTFILE_BYNAME; break;
		case IDM_FILE_SORT_BYPATH:
			nSort = SORTFILE_BYPATH; break;
	}
	SortFile(nSort);
}

void CPpcMainWnd::OnToolEffect()
{
	SetForegroundWindow(m_hWnd);
	CMainWnd::OnToolEffect();
}

void CPpcMainWnd::OnToolOption()
{
	if (m_fHold)
		return;

	SetForegroundWindow(m_hWnd);
	UnregisterHotKeys();
	CMainWnd::OnToolOption();
	RegisterHotKeys();
}

void CPpcMainWnd::OnShowMenu(POINT* ppt)
{
	//SetForegroundWindow(m_hWnd);
	//CMainWnd::OnShowMenu(ppt);
}

void CPpcMainWnd::OnToolHold()
{
	UnregisterHotKeys();
	m_fHold = !m_fHold;
	EnableWindow(m_hWnd, !m_fHold);
	RegisterHotKeys();

	if (!m_fActive)
		SetForegroundWindow(m_hWnd);

	KillTimer(m_hWnd, ID_TIMER_HOLD);
	m_fDrawHold = TRUE;
	SetTimer(m_hWnd, ID_TIMER_HOLD, TIMER_HOLD_INTERVAL, NULL);
	UpdateTitle();
}

void CPpcMainWnd::OnToolHoldDisp()
{
	UnregisterHotKeys();
	m_fHold = !m_fHold;
	EnableWindow(m_hWnd, !m_fHold);
	RegisterHotKeys();

	if (!m_fActive)
		SetForegroundWindow(m_hWnd);

	KillTimer(m_hWnd, ID_TIMER_HOLD);
	m_fDrawHold = TRUE;
	SetTimer(m_hWnd, ID_TIMER_HOLD, TIMER_HOLD_INTERVAL, NULL);
	UpdateTitle();

	if (m_fHold) {
		TurnOffDisplay();
		m_fDisplay = FALSE;
	}
	else {
		TurnOnDisplay();
		m_fDisplay = TRUE;
		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}
}

void CPpcMainWnd::RegisterHotKeys()
{
	if (m_fHotKey)
		return;

	int i, j;
	if (m_fHold) {
		for (i = 0; i < sizeof(s_uHoldKeys) / sizeof(UINT) / 2; i++)
			RegisterHotKey(m_hWnd, s_uHoldKeys[i][0], s_uHoldKeys[i][1], s_uHoldKeys[i][0]);

		CPpcOptions* pOptions = (CPpcOptions*)m_pOptions;
		for (i = 0; i < pOptions->m_listKeyMap.GetCount(); i++) {
			BOOL fHold = FALSE;
			ACCEL* p = (ACCEL*)pOptions->m_listKeyMap.GetAt(i);
			for (j = 0; j < sizeof(s_uHoldKeys) / sizeof(UINT) / 2; j++) {
				if (s_uHoldKeys[j][0] == p->key) {
					fHold = TRUE;
					break;
				}
			}
			
			if (!fHold) {
				UINT fsModifiers = 0;
				if (p->fVirt & FALT)
					fsModifiers |= MOD_ALT;
				if (p->fVirt & FCONTROL)
					fsModifiers |= MOD_CONTROL;
				if (p->fVirt & FSHIFT)
					fsModifiers |= MOD_SHIFT;
				if (p->fVirt & FWIN)
					fsModifiers |= MOD_WIN;
				RegisterHotKey(m_hWnd, p->key, fsModifiers, p->key);
			}
		}
	}
	else {
		CPpcOptions* pOptions = (CPpcOptions*)m_pOptions;
		for (i = 0; i < pOptions->m_listKeyMap.GetCount(); i++) {
			ACCEL* p = (ACCEL*)pOptions->m_listKeyMap.GetAt(i);

			UINT fsModifiers = 0;
			if (p->fVirt & FALT)
				fsModifiers |= MOD_ALT;
			if (p->fVirt & FCONTROL)
				fsModifiers |= MOD_CONTROL;
			if (p->fVirt & FSHIFT)
				fsModifiers |= MOD_SHIFT;
			if (p->fVirt & FWIN)
				fsModifiers |= MOD_WIN;
			RegisterHotKey(m_hWnd, p->key, fsModifiers, p->key);
		}
	}
	m_fHotKey = TRUE;
}

void CPpcMainWnd::UnregisterHotKeys()
{
	if (!m_fHotKey)
		return;

	int i, j;
	if (m_fHold) {
		for (i = 0; i < sizeof(s_uHoldKeys) / sizeof(UINT) / 2; i++)
			UnregisterHotKey(m_hWnd, s_uHoldKeys[i][0]);

		CPpcOptions* pOptions = (CPpcOptions*)m_pOptions;
		for (i = 0; i < pOptions->m_listKeyMap.GetCount(); i++) {
			BOOL fHold = FALSE;
			ACCEL* p = (ACCEL*)pOptions->m_listKeyMap.GetAt(i);
			for (j = 0; j < sizeof(s_uHoldKeys) / sizeof(UINT) / 2; j++) {
				if (s_uHoldKeys[j][0] == p->key) {
					fHold = TRUE;
					break;
				}
			}

			if (!fHold) UnregisterHotKey(m_hWnd, p->key);
		}
	}
	else {
		CPpcOptions* pOptions = (CPpcOptions*)m_pOptions;
		for (i = 0; i < pOptions->m_listKeyMap.GetCount(); i++) {
			ACCEL* p = (ACCEL*)pOptions->m_listKeyMap.GetAt(i);
			UnregisterHotKey(m_hWnd, p->key);
		}
	}
	m_fHotKey = FALSE;
}

void CPpcMainWnd::CreateMenuBar()
{
	SHMENUBARINFO mbi = {0};
	mbi.cbSize = sizeof(SHMENUBARINFO);
	mbi.hwndParent = m_hWnd;
	mbi.nToolBarId = IDR_MAIN;
	mbi.hInstRes = GetInst();
	mbi.nBmpId = IDR_MAIN;
	mbi.cBmpImages = 5;
	//mbi.dwFlags = SHCMBF_HIDESIPBUTTON;

	if (!SHCreateMenuBar(&mbi)) 
		return;

	m_hwndCB = mbi.hwndMB;

	TBBUTTON tbbMain[] = {
		{1, IDM_FILE_ADD, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
		{2, IDM_FILE_UP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 1},
		{3, IDM_FILE_DOWN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 2},
		{4, IDM_FILE_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 3},
		//{5, IDM_FILE_SAVE_LIST, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 4}
	};
	CommandBar_AddButtons(m_hwndCB, sizeof(tbbMain) / sizeof(TBBUTTON), tbbMain);

#ifdef LANG_JPN
	TBBUTTONINFO tbi = {sizeof(TBBUTTONINFO), TBIF_TEXT | TBIF_IMAGE, 0, 0, TBSTYLE_BUTTON, 0, 0, 0, 0, 0};
	SendMessage(m_hwndCB, TB_SETBUTTONINFO, IDM_TOOL, (LPARAM)&tbi);
#endif
}

void CPpcMainWnd::CreateListView()
{
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_NOCOLUMNHEADER;
	m_hwndLV = CreateWindow(WC_LISTVIEW, NULL, dwStyle, 
		PPC_PLAYLIST_LEFT, PPC_PLAYLIST_TOP, PPC_PLAYLIST_WIDTH, PPC_PLAYLIST_HEIGHT, m_hWnd, 0, GetInst(), NULL);

	ListView_SetExtendedListViewStyle(m_hwndLV, 
			ListView_GetExtendedListViewStyle(m_hwndLV) | LVS_EX_FULLROWSELECT);

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = 0;
	lvc.pszText = _T("Name");
	lvc.iSubItem = 0;
	ListView_InsertColumn(m_hwndLV, 0, &lvc);

	RECT rc;
	GetWindowRect(m_hwndLV, &rc);
	SendMessage(m_hwndLV, LVM_SETCOLUMNWIDTH, 0, 
		MAKELPARAM(RECT_WIDTH(&rc) - GetSystemMetrics(SM_CXVSCROLL) - GetSystemMetrics(SM_CXBORDER) * 2 - 1, 0));
}

// リスト
BOOL CPpcMainWnd::AddFile(LPTSTR pszFile, LPTSTR pszTitle, long nDur)
{
	if (!m_hMap) return FALSE;

	// 存在チェック
	if (IsExisting(pszFile))
		return FALSE;

	// 有効性チェック
	if (!((CPpcOptions*)m_pOptions)->m_fFastLoad && !IsValidStream(pszFile))
		return FALSE;

	// リストに追加
	FILEINFO* pInfo = new FILEINFO;

	if (nDur)
		pInfo->info.nDuration = nDur;

	if (pszTitle)
		_tcscpy(pInfo->szDisplayName, pszTitle);
	else 
		MAP_GetId3TagFile(m_hMap, pszFile, &pInfo->tag);
	_tcscpy(pInfo->szPath, pszFile);
	m_pListFile->Add((DWORD)pInfo);

	// リストビューに追加
	TCHAR szTitle[MAX_PATH];
	GetTitle(m_pListFile->GetCount() - 1, szTitle);
	LVITEM lvi = {0};
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ListView_GetItemCount(m_hwndLV);
	lvi.pszText = szTitle;
	ListView_InsertItem(m_hwndLV, &lvi);

	// 開いていない場合は開く
	OpenFirstFile();

	return TRUE;
}

void CPpcMainWnd::AddDir(LPTSTR pszDir)
{
	CMainWnd::AddDir(pszDir);
}

BOOL CPpcMainWnd::InsertFile(LPTSTR pszFile, int nIndex)
{
	if (!m_hMap) return FALSE;

	// 有効性チェック
	if (!IsValidStream(pszFile))
		return FALSE;

	// リストに挿入
	FILEINFO* pInfo = new FILEINFO;
	MAP_GetId3TagFile(m_hMap, pszFile, &pInfo->tag);
	_tcscpy(pInfo->szPath, pszFile);
	m_pListFile->Insert((DWORD)pInfo, nIndex);

	// リストビューに追加
	TCHAR szTitle[MAX_PATH];
	GetTitle(m_pListFile->GetCount() - 1, szTitle);
	LVITEM lvi = {0};
	lvi.mask = LVIF_TEXT;
	lvi.iItem = nIndex;
	lvi.pszText = szTitle;
	ListView_InsertItem(m_hwndLV, &lvi);

	// 開いていない場合は開く
	OpenFirstFile();

	return TRUE;
}

void ExchangeItem(HWND hwndLV, int nItem1, int nItem2)
{
	UINT uItem1, uItem2;
	TCHAR szTitle1[MAX_PATH];
	TCHAR szTitle2[MAX_PATH];
	ListView_GetItemText(hwndLV, nItem1, 0, szTitle1, MAX_PATH);
	ListView_GetItemText(hwndLV, nItem2, 0, szTitle2, MAX_PATH);
	uItem1 = ListView_GetItemState(hwndLV, nItem1, LVIS_FOCUSED | LVIS_SELECTED);
	uItem2 = ListView_GetItemState(hwndLV, nItem2, LVIS_FOCUSED | LVIS_SELECTED);

	ListView_SetItemText(hwndLV, nItem1, 0, szTitle2);
	ListView_SetItemText(hwndLV, nItem2, 0, szTitle1);
	ListView_SetItemState(hwndLV, nItem1, uItem2, LVIS_FOCUSED | LVIS_SELECTED);
	ListView_SetItemState(hwndLV, nItem2, uItem1, LVIS_FOCUSED | LVIS_SELECTED);
}

BOOL CPpcMainWnd::UpFile(int nIndex)
{
	if (nIndex <= 0)
		return FALSE;

	FILEINFO* p1 = (FILEINFO*)m_pListFile->GetAt(nIndex);
	FILEINFO* p2 = (FILEINFO*)m_pListFile->GetAt(nIndex - 1);
	if (!p1 || !p2)
		return FALSE;

	m_pListFile->SetAt(nIndex - 1, (DWORD)p1);
	m_pListFile->SetAt(nIndex, (DWORD)p2);

	ExchangeItem(m_hwndLV, nIndex - 1, nIndex);

	if (m_nCurrent == nIndex)
		m_nCurrent--;
	else if (m_nCurrent == nIndex - 1)
		m_nCurrent++;

	UpdateTrack();
	return TRUE;
}

BOOL CPpcMainWnd::DownFile(int nIndex)
{
	if (nIndex >= m_pListFile->GetCount() - 1)
		return FALSE;

	FILEINFO* p1 = (FILEINFO*)m_pListFile->GetAt(nIndex);
	FILEINFO* p2 = (FILEINFO*)m_pListFile->GetAt(nIndex + 1);
	if (!p1 || !p2)
		return FALSE;

	m_pListFile->SetAt(nIndex + 1, (DWORD)p1);
	m_pListFile->SetAt(nIndex, (DWORD)p2);

	ExchangeItem(m_hwndLV, nIndex + 1, nIndex);

	if (m_nCurrent == nIndex)
		m_nCurrent++;
	else if (m_nCurrent == nIndex + 1)
		m_nCurrent--;

	UpdateTrack();
	return TRUE;
}
BOOL CPpcMainWnd::DeleteFile(int nIndex)
{
	if (!m_pListFile->RemoveAt(nIndex))
		return FALSE;

	ListView_DeleteItem(m_hwndLV, nIndex);
	
	if (!m_pListFile->GetCount()) {
		Close();
		m_nCurrent = OPEN_NONE;
		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}
	else if (m_nCurrent == nIndex) {
		Stop();
		
		UpdateTime();
		UpdateStatus();
		m_nCurrent = OPEN_NONE;

		Open(m_pListFile->GetCount() - 1 < nIndex ? 0 : nIndex);
	}
	else if (m_nCurrent > nIndex) {
		m_nCurrent--;
		UpdateTrack();
	}
	return TRUE;
}

void CPpcMainWnd::DeleteAllFile()
{
	Stop();
	Close();

	m_nCurrent = OPEN_NONE;
	for (int i = 0; i < m_pListFile->GetCount(); i++) {
		FILEINFO* pInfo = (FILEINFO*)m_pListFile->GetAt(i);
		delete pInfo;
	}
	m_pListFile->RemoveAll();
	ListView_DeleteAllItems(m_hwndLV);

	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

void CPpcMainWnd::SortFile(int nSort)
{
#define CURRENT_MASK	0x8000
	int i, nFocus = -1;
	FILEINFO* p;
	TCHAR szTitle[MAX_PATH];

	for (i = 0; i < m_pListFile->GetCount(); i++) {
		p = (FILEINFO*)m_pListFile->GetAt(i);
		p->dwUser = (i == m_nCurrent) ? CURRENT_MASK : 0;
		p->dwUser |= ListView_GetItemState(m_hwndLV, i, LVIS_FOCUSED | LVIS_SELECTED);
	}

	m_pListFile->Sort(SortCompareProc, nSort);
	for (i = 0; i < m_pListFile->GetCount(); i++) {
		p = (FILEINFO*)m_pListFile->GetAt(i);
		if (p->dwUser & CURRENT_MASK)
			m_nCurrent = i;
		GetTitle(i, szTitle);
		ListView_SetItemText(m_hwndLV, i, 0, szTitle);
		ListView_SetItemState(m_hwndLV, i, p->dwUser, LVIS_FOCUSED | LVIS_SELECTED);
		if (p->dwUser &LVIS_FOCUSED)
			nFocus = i;
	}
	if (nFocus != -1)
		ListView_EnsureVisible(m_hwndLV, nFocus, FALSE);

	UpdateTrack();
}

void CPpcMainWnd::OnFileUp()
{
	if (!ListView_GetSelectedCount(m_hwndLV))
		return;

	int nCount = ListView_GetItemCount(m_hwndLV);
	if (ListView_GetItemState(m_hwndLV, 0, LVIS_SELECTED))
		return;

	for (int i = 1; i < nCount; i++) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_SELECTED))
			UpFile(i);
	}
}

void CPpcMainWnd::OnFileDown()
{
	if (!ListView_GetSelectedCount(m_hwndLV))
		return;

	int nCount = ListView_GetItemCount(m_hwndLV);
	if (ListView_GetItemState(m_hwndLV, nCount - 1, LVIS_SELECTED))
		return;

	for (int i = nCount - 2; i >= 0; i--) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_SELECTED))
			DownFile(i);
	}
}

void CPpcMainWnd::OnFileDelete()
{
	int i;

	if (!ListView_GetSelectedCount(m_hwndLV))
		return;

	int nCount = ListView_GetItemCount(m_hwndLV);
	for (i = nCount - 1; i >= 0; i--) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_SELECTED))
			DeleteFile(i);
	}

	nCount = ListView_GetItemCount(m_hwndLV);
	for (i = 0; i < nCount; i++) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_FOCUSED)) {
			ListView_SetItemState(m_hwndLV, i, LVIS_SELECTED, LVIS_SELECTED);
			break;
		}
	}
}

void CPpcMainWnd::OnFileDeleteAll()
{
	m_fNoListRedraw = TRUE;
	SendMessage(m_hwndLV, WM_SETREDRAW, 0, 0);
	int nCount = ListView_GetItemCount(m_hwndLV);
	for (int i = nCount - 1; i >= 0; i--)
		DeleteFile(i);
	SendMessage(m_hwndLV, WM_SETREDRAW, 1, 0);
	m_fNoListRedraw = FALSE;
}

void CPpcMainWnd::OnPlaySelected()
{
	OnListViewDblClk(NULL);
}

void CPpcMainWnd::OnListViewReturn(NMHDR* pnmh)
{
	// バックライト制御以外で
	// 自動バックライトON制御が有効のとき
	if (((CPpcOptions*)m_pOptions)->m_fDispAutoOn) {
		TurnOnDisplay();
		m_fDisplay = TRUE;

		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}

	if (!ListView_GetSelectedCount(m_hwndLV))
		return;

	int nCount = ListView_GetItemCount(m_hwndLV);
	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_FOCUSED)) {
			Play(i);
			break;
		}
	}
}

void CPpcMainWnd::OnListViewKeyDown(NMHDR* pnmh)
{
	// バックライト制御以外で
	// 自動バックライトON制御が有効のとき
	if (((CPpcOptions*)m_pOptions)->m_fDispAutoOn)  {
		TurnOnDisplay();
		m_fDisplay = TRUE;

		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}

	if (((LPNMLVKEYDOWN)pnmh)->wVKey == 'Q' && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}

void CPpcMainWnd::OnListViewDblClk(NMHDR* pnmh)
{
	if (!ListView_GetSelectedCount(m_hwndLV))
		return;

	int nCount = ListView_GetItemCount(m_hwndLV);
	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_FOCUSED)) {
			Play(i);
			break;
		}
	}
}

void CPpcMainWnd::OnListViewBeginDrag(NMHDR* pnmh)
{
	NMLISTVIEW* pnmlv = (NMLISTVIEW*)pnmh;

	SetCapture(m_hWnd);
	m_fListDrag = TRUE;
	m_nListDragItem = pnmlv->iItem;
}

void CPpcMainWnd::OnListViewConextMenu(NMHDR* pnmh)
{
	NMRGINFO* pnmri = (NMRGINFO*)pnmh;

	int nCount = ListView_GetSelectedCount(m_hwndLV);
	if (nCount) {
		HMENU hMenu, hSub;
		hMenu = LoadMenu(GetInst(), (LPCTSTR)IDR_POPUP);
		if (nCount == 1)
			hSub = GetSubMenu(hMenu, IDM_SUBMENU_SEL0);
		else
			hSub = GetSubMenu(hMenu, IDM_SUBMENU_SEL1);
		TrackPopupMenu(hSub, TPM_LEFTALIGN | TPM_TOPALIGN, 
						pnmri->ptAction.x, pnmri->ptAction.y, 0, m_hWnd, NULL);
		DestroyMenu(hMenu);
	}
}

void CPpcMainWnd::OnMsgStatus(MAP_STATUS status, BOOL fError)
{
	CMainWnd::OnMsgStatus(status, fError);

	if (status == MAP_STATUS_PLAY) {
		TCHAR szText[MAX_PATH];
		TCHAR szTitle[MAX_PATH];
		
		int nCount = ListView_GetItemCount(m_hwndLV);
		for (int i = 0; i < nCount; i++) {
			ListView_GetItemText(m_hwndLV, i, 0, szText, MAX_PATH);
			GetTitle(i, szTitle);

			if (_tcscmp(szTitle, szText) != 0)
				ListView_SetItemText(m_hwndLV, i, 0, szTitle);
		}
	}
}

void CPpcMainWnd::DrawTitle(HDC hDC)
{
	if (m_fDrawHold) {
		RECT rc = {TITLE_LEFT, m_nTitleTop, 
			TITLE_LEFT + m_nTitleWidth, m_nTitleTop + m_nTitleHeight};

		CTempStr str(m_fHold ? IDS_MSG_HOLD_ON : IDS_MSG_HOLD_OFF);
		COLORREF crBk = SetTextColor(hDC, m_pSkin->GetColor(COLOR_TITLE));
		int nOld = SetBkMode(hDC, TRANSPARENT);
		HFONT hBk = (HFONT)SelectObject(hDC, m_hFntTitle);
		DrawText(hDC, str, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);	
		SetTextColor(hDC, crBk);
		SetBkMode(hDC, nOld);
		SelectObject(hDC, hBk);
	}
	else CMainWnd::DrawTitle(hDC);
}

void CPpcMainWnd::OnPlayPlayStop()
{
	if (MAP_GetStatus(m_hMap) == MAP_STATUS_STOP)
		OnPlayPlayPause();
	else 
		OnPlayStop();
}

void CPpcMainWnd::OnEnqueue()
{
	TCHAR szTitle[MAX_PATH];

	if (!ListView_GetSelectedCount(m_hwndLV))
		return;

	int nCount = ListView_GetItemCount(m_hwndLV);
	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_SELECTED)) {
			Enqueue(i);
			GetTitle(i, szTitle);
			ListView_SetItemText(m_hwndLV, i, 0, szTitle);
		}
	}
}

void CPpcMainWnd::OnDequeue()
{

	if (!ListView_GetSelectedCount(m_hwndLV))
		return;

	int nCount = ListView_GetItemCount(m_hwndLV);
	for (int i = 0; i < nCount; i++) {
		if (ListView_GetItemState(m_hwndLV, i, LVIS_SELECTED)) {
			Dequeue(i);
		}
	}
	// Hack - Refresh Playlist
	OnMsgStatus(MAP_STATUS_PLAY, FALSE);
}

void CPpcMainWnd::OnClearQueue()
{
	ClearQueue();
	// Hack - Refresh Playlist
	OnMsgStatus(MAP_STATUS_PLAY, FALSE);
}

BOOL CPpcMainWnd::Open(int nIndex)
{
	m_fDrawHold = FALSE;
	KillTimer(m_hWnd, ID_TIMER_HOLD);

	return CMainWnd::Open(nIndex);
}

void CPpcMainWnd::OnCopyData(COPYDATASTRUCT* pcds)
{
	m_fNoListRedraw = TRUE;
	SendMessage(m_hwndLV, WM_SETREDRAW, 0, 0);
	CMainWnd::OnCopyData(pcds);
	SendMessage(m_hwndLV, WM_SETREDRAW, 1, 0);
	m_fNoListRedraw = FALSE;

	InvalidateRect(m_hwndLV, NULL, FALSE);
	UpdateWindow(m_hwndLV);
}

void CPpcMainWnd::OnFileFind()
{
	m_fNoListRedraw = TRUE;
	SendMessage(m_hwndLV, WM_SETREDRAW, 0, 0);
	CMainWnd::OnFileFind();
	SendMessage(m_hwndLV, WM_SETREDRAW, 1, 0);
	m_fNoListRedraw = FALSE;

	InvalidateRect(m_hwndLV, NULL, FALSE);
	UpdateWindow(m_hwndLV);
}

#define FILE_BUFF_LEN 2048
BOOL CPpcMainWnd::OpenMediaFile(BOOL fAdd, HWND hwndParent)
{
	if (m_fShowOpenDlg)
		return FALSE;

	BOOL fRet;
	m_fShowOpenDlg = TRUE;
	CTempStr strTitle(fAdd ? IDS_TITLE_ADD_FILE : IDS_TITLE_OPEN_FILE);
	TCHAR szFile[FILE_BUFF_LEN] = {0};

	OPENFILENAME ofn;
	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner   = hwndParent ? hwndParent : m_hWnd;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = FILE_BUFF_LEN;	
	ofn.lpstrTitle  = strTitle;

	BOOL (*gsGetOpenFileName)(OPENFILENAME*) = NULL;
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		(FARPROC&)gsGetOpenFileName = GetProcAddress(hInst, _T("gsGetOpenFileName"));
	}

	if (gsGetOpenFileName) {
		if (!m_pszOpenFilter) MakeOpenFilter(&m_pszOpenFilter, TRUE);
		ofn.lpstrFilter = m_pszOpenFilter;
		ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY;
		ofn.lpstrInitialDir = ((CPpcOptions*)m_pOptions)->m_szLastDir;
		fRet = gsGetOpenFileName(&ofn);
	}
	else {
		if (!m_pszOpenFilter) MakeOpenFilter(&m_pszOpenFilter, TRUE);
		ofn.lpstrFilter = m_pszOpenFilter;
		ofn.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrInitialDir = ((CPpcOptions*)m_pOptions)->m_szLastDir;
		fRet = GetOpenFileName(&ofn);
	}

	if (hInst) FreeLibrary(hInst);

	m_fShowOpenDlg = FALSE;
	if (!fRet)
		return FALSE;

	m_fNoListRedraw = TRUE;
	SendMessage(m_hwndLV, WM_SETREDRAW, 0, 0);

	// 既存のリストを削除
	if (!fAdd) DeleteAllFile();

	// 追加
	DWORD dw = GetFileAttributes(szFile);
	if (dw != 0xFFFFFFFF) {
		if (!(dw & FILE_ATTRIBUTE_DIRECTORY)) {
			// single
			if (IsPlayList(szFile))
				LoadPlayList(szFile);
			else 
				AddFile(szFile);
			LPTSTR psz = _tcsrchr(szFile, _T('\\'));
			if (psz) *psz = _T('\0');
				_tcscpy(((CPpcOptions*)m_pOptions)->m_szLastDir, szFile);
		}
		else {
			// multi
			TCHAR szPath[MAX_PATH];

			LPTSTR p = szFile;
			_tcscpy(((CPpcOptions*)m_pOptions)->m_szLastDir, szFile);
			while (TRUE) {
				p += _tcslen(p);
				if (*p != NULL || *(p + 1) == NULL)
					break;

				wsprintf(szPath, _T("%s\\%s"), ((CPpcOptions*)m_pOptions)->m_szLastDir, ++p);
				if (IsPlayList(szPath))
					LoadPlayList(szPath);
				else
					AddFile(szPath);
			}
		}
	}

	SendMessage(m_hwndLV, WM_SETREDRAW, 1, 0);
	m_fNoListRedraw = FALSE;

	InvalidateRect(m_hwndLV, NULL, FALSE);
	UpdateWindow(m_hwndLV);
	return TRUE;
}

void CPpcMainWnd::OpenFirstFile()
{
	CMainWnd::OpenFirstFile();
	if (m_fNoListRedraw)
		SendMessage(m_hwndLV, WM_SETREDRAW, 0, 0);
}