#include "GSPlayer2.h"
#include "MainWnd.h"
#include "mt19937ar.h"

////////////////////////////////////////////////////////////////////////////////
// publicメンバ
CMainWnd::CMainWnd()
{
	m_hWnd = NULL;
	m_hAccel = NULL;
	m_hMenu = NULL;
	m_hFnt = NULL;
	m_hFntTitle = NULL;

	m_pSkin = NULL;
	m_hBakDC = NULL;
	m_hBakBmp = NULL;
	m_hMemDC = NULL;
	m_hMemBmp = NULL;
	m_pPlayListDlg = NULL;

	m_nWidth = MAINWND_WIDTH;
	m_nHeight = MAINWND_HEIGHT;
	m_nPeek[0] = m_nPeek[1] = 0;

	m_nClientWidth = MAINWND_WIDTH;
	m_nClientHeight = MAINWND_HEIGHT;
	m_nDispWidth = DISP_WIDTH;
	m_nDispHeight = DISP_HEIGHT;
	m_nBtnTop = BTN_TOP;
	m_nBtnMax = BTN_MAX;

	m_nStatusTop = STATUS_TOP;
	m_nStatusHeight = STATUS_HEIGHT;
	m_nTitleTop = TITLE_TOP;
	m_nTitleWidth = TITLE_WIDTH;
	m_nTitleHeight = TITLE_HEIGHT;
	m_nSeekTop = SEEK_TOP;
	m_nSeekWidth = SEEK_WIDTH;
	m_nStreamTitleTop = STREAM_TITLE_TOP;
	m_nStreamTitleWidth = STREAM_TITLE_WIDTH;
	m_nStreamTitleHeight = STREAM_TITLE_HEIGHT;

	m_nVolumeSliderLeft = VOLSLIDER_LEFT;
	m_nVolumeSliderTop = VOLSLIDER_TOP;
	m_nVolumeSliderWidth = VOLSLIDER_WIDTH;
	m_nVolumeSliderHeight = VOLSLIDER_HEIGHT;
	m_nVolumeTrackWidth = VOLTRACK_WIDTH;

	m_fPressed = FALSE;
	m_nPressed = -1;
	m_ptPressed.x = -1;
	m_ptPressed.y = -1;
	m_fVolPressed = FALSE;
	m_fDrawVolume = FALSE;
	m_nLastDrawTime = -1;
	m_nLastDrawSeek = -1;
	m_fShowOpenDlg = FALSE;

	m_hMap = MAP_Initialize();
	m_pListFile = new CMultiBuff();
	m_nCurrent = OPEN_NONE;
	m_pOptions = NULL;
	m_hTrayIcon = NULL;
	m_fRegEvent = FALSE;

	m_nTitleLen = 0;
	m_nTitlePos = 0;
	m_nTitleWait = 0;
	m_nStreamTitleLen = 0;
	m_nStreamTitlePos = 0;
	m_nStreamTitleWait = 0;
	m_pszOpenFilter = NULL;

	m_nEnqueue = 0;
	m_fStopAfterCurr = FALSE;

	srand(GetTickCount());

	// User Agentの設定
	MAP_STREAMING_OPTIONS options;
	MAP_GetStreamingOptions(m_hMap, &options);
	_tcscpy(options.szUserAgent, STREAMING_USER_AGENT);
	MAP_SetStreamingOptions(m_hMap, &options);
}

CMainWnd::~CMainWnd()
{
	DeleteAllFile();
	if (m_pOptions) {
		delete m_pOptions;
		m_pOptions = NULL;
	}
	if (m_pSkin) {
		delete m_pSkin;
		m_pSkin = NULL;
}
	if (m_hMap) {
		MAP_Uninitialize(m_hMap);
		m_hMap = NULL;
	}
	if (m_pListFile) {
		delete m_pListFile;
		m_pListFile = NULL;
	}
	if (m_hAccel) {
		DestroyAcceleratorTable(m_hAccel);
		m_hAccel = NULL;
	}
	if (m_pszOpenFilter) {
		delete m_pszOpenFilter;
		m_pszOpenFilter = NULL;
	}
}

COptions* CMainWnd::GetOptionsClass()
{
	return new COptions();
}

CSkin* CMainWnd::GetSkinClass()
{
	return new CSkin();
}

BOOL CMainWnd::CanTransAccel(MSG* pMsg)
{
	return pMsg->hwnd == m_hWnd;
}

BOOL CMainWnd::Create(LPTSTR pszCmdLine, HWND hwndParent, DWORD dwStyle)
{
	CWaitCursor wc;

	// クラスの登録
	RegisterClass();

	// ウインドウ作成
#ifdef _WIN32_WCE
	CreateWindowEx(0, MAINWND_CLASS_NAME, MAINWND_TITLE, dwStyle, 
		0, 0, m_nClientWidth, m_nClientHeight, NULL, NULL, GetInst(), this);
#else
	CreateWindowEx(WS_EX_ACCEPTFILES, MAINWND_CLASS_NAME, MAINWND_TITLE, dwStyle, 
		0, 0, m_nClientWidth, m_nClientHeight, NULL, NULL, GetInst(), this);
#endif

	if (!m_hWnd)
		return FALSE;

	// ウインドウサイズを調整する
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_nWidth = m_nClientWidth + (m_nClientWidth - RECT_WIDTH(&rc));
	m_nHeight = m_nClientHeight + (m_nClientHeight - RECT_HEIGHT(&rc));
	MoveWindow(m_hWnd, 0, 0, m_nWidth, m_nHeight, TRUE);
	
	// 親ウインドウがない場合はウインドウを初期位置に移動
	if (!hwndParent) {
		MoveWindow(m_hWnd, m_pOptions->m_ptInitWnd.x, 
							m_pOptions->m_ptInitWnd.y,
							m_nWidth, m_nHeight, FALSE);
	}
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	SetWindowPos(m_hWnd, m_pOptions->m_fTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 
					0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	if (_tcslen(pszCmdLine)) {
		SendCmdLine(m_hWnd, pszCmdLine);
	}
	else {
		// デフォルトリストの読み込み
		if (m_pOptions->m_fSaveDefList) {
			m_nCurrent = 0;	// 開かないようにする

			// 読み込み
			TCHAR szPath[MAX_PATH];
			GetDefPlayListPath(szPath);
			LoadPlayList(szPath);

			m_nCurrent = OPEN_NONE;

			// レジューム情報を反映する
			if (m_pOptions->m_fResume && 
				m_pOptions->m_nResumeTrack != -1 &&
				m_pOptions->m_nResumeTrack < m_pListFile->GetCount()) {
				Open(m_pOptions->m_nResumeTrack);
				if (m_nCurrent == OPEN_NONE)
					OpenFirstFile();
				else {
					MAP_Seek(m_hMap, m_pOptions->m_nResumeTime);
					m_pOptions->m_nResumeTime = 0;
				}
			}
			else
				OpenFirstFile();
			if (m_pOptions->m_fPlayOnStart)
				Play();
		}
	}
	return TRUE;
}

HACCEL CMainWnd::GetAccelHandle()
{
	if (!m_hAccel) {
		ACCEL accel[] = {
			{FVIRTKEY, 'Z', IDM_PLAY_PLAYPAUSE},		// Play/Pause
			{FVIRTKEY, VK_RETURN, IDM_PLAY_PLAYPAUSE},	// Play/Pause
			{FVIRTKEY, 'X', IDM_PLAY_STOP},				// Stop
			{FVIRTKEY, VK_SPACE, IDM_PLAY_STOP},		// Stop
			{FVIRTKEY, 'C', IDM_PLAY_PREV},				// Prev
			{FVIRTKEY, VK_LEFT, IDM_PLAY_PREV},			// Prev
			{FVIRTKEY, 'V', IDM_PLAY_REW},				// Rew
			{FVIRTKEY, VK_PRIOR, IDM_PLAY_REW},			// Rew
			{FVIRTKEY, 'B', IDM_PLAY_FF},				// FF
			{FVIRTKEY, VK_NEXT, IDM_PLAY_FF},			// FF
			{FVIRTKEY, 'N', IDM_PLAY_NEXT},				// Next
			{FVIRTKEY, VK_RIGHT, IDM_PLAY_NEXT},		// Next
			{FVIRTKEY, 'R', IDM_PLAY_REPEAT},			// Repeat
			{FVIRTKEY, 'H', IDM_PLAY_SHUFFLE},			// Shuffle
			{FVIRTKEY, 'O', IDM_FILE_OPEN},				// Open
			{FVIRTKEY, 'U', IDM_FILE_OPEN_LOCATION},	// Open Location
			{FVIRTKEY, 'A', IDM_FILE_ADD},				// Add
			{FVIRTKEY, 'F', IDM_FILE_FIND},				// Find
			{FVIRTKEY, 'L', IDM_FILE_LIST},				// List
			{FVIRTKEY, 'S', IDM_FILE_SAVE_LIST},		// Save List
			{FVIRTKEY, VK_TAB, IDM_FILE_LIST},			// List
			{FVIRTKEY, 'E', IDM_TOOL_EFFECT},			// Effect
			{FVIRTKEY, VK_UP, IDM_TOOL_VOLUP},			// Volume Up
			{FVIRTKEY, VK_DOWN, IDM_TOOL_VOLDOWN},		// Volume Down
			{FVIRTKEY, 'M', IDM_SHOW_MENU},				// Show Menu
			{FVIRTKEY, VK_ESCAPE, IDM_APP_SHOWHIDE},	// Show / Hide
			{FVIRTKEY | FCONTROL, 'Q', IDM_APP_EXIT},	// Exit
		};
		m_hAccel = CreateAcceleratorTable(accel, sizeof(accel) / sizeof(ACCEL));
	}
	return m_hAccel;
}

BOOL CMainWnd::IsDialogMessage(LPMSG pMsg)
{
	if (m_pPlayListDlg) {
		return m_pPlayListDlg->IsDialogMessage(pMsg);
	}
	return FALSE;
}

void CMainWnd::CheckSystem()
{
	::CheckSystem();
}

////////////////////////////////////////////////////////////////////////////////
// ウインドウプロシージャ
LRESULT CALLBACK CMainWnd::MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CMainWnd* pMainWnd;
	switch (uMsg) {
		case WM_CREATE:
		{
			CREATESTRUCT* pcs;
			pcs = (CREATESTRUCT*)lParam;
			pMainWnd = (CMainWnd*)pcs->lpCreateParams;
			pMainWnd->OnCreate(hwnd);
			return 0;
		}
		case WM_COMMAND:
			return pMainWnd->OnCommand(wParam, lParam);
		case WM_DESTROY:
			pMainWnd->OnDestroy();
			return 0;
		case WM_LBUTTONDOWN:
			pMainWnd->OnLButtonDown((short)LOWORD(lParam), (short)HIWORD(lParam));
			return 0;
		case WM_LBUTTONUP:
			pMainWnd->OnLButtonUp((short)LOWORD(lParam), (short)HIWORD(lParam));
			return 0;
		case WM_MOUSEMOVE:
			pMainWnd->OnMouseMove(wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
			return 0;
		case WM_RBUTTONUP:
		{
			POINT pt = {LOWORD(lParam), HIWORD(lParam)};
			pMainWnd->OnShowMenu(&pt);
			return 0;
		}
		case WM_ERASEBKGND:
			pMainWnd->OnEraseBkgnd((HDC)wParam);
			return 0;
		case WM_TIMER:
			pMainWnd->OnTimer(wParam);
			return 0;
		case WM_MOVE:
			pMainWnd->OnMove(LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_SIZE:
			pMainWnd->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_ACTIVATE:
			pMainWnd->OnActivate(LOWORD(wParam), (BOOL)HIWORD(wParam), (HWND)lParam);
			return 0;
		case WM_HOTKEY:
			pMainWnd->OnHotKey((int)wParam, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
			return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hwnd, &ps);
			pMainWnd->OnPaint(hDC, &ps);
			EndPaint(hwnd, &ps);
			return 0;
		}
#ifdef _WIN32_WCE
		case WM_COPYDATA:
			pMainWnd->OnCopyData((COPYDATASTRUCT*)lParam);
			return 0;
#else
		case WM_DROPFILES:
			pMainWnd->OnDropFiles((HDROP)wParam);
			return 0;
#endif
		case MAP_MSG_STATUS:
			pMainWnd->OnMsgStatus((MAP_STATUS)wParam, lParam);
			return 0;
		case MAP_MSG_PEEK:
			pMainWnd->OnMsgPeek(wParam, lParam);
			return 0;
		case MAP_MSG_STREAM_TITLE:
			pMainWnd->OnMsgStreamTitle((LPTSTR)wParam);
			return 0;
		case WM_CLOSE:
			pMainWnd->OnClose();
			return 0;
		case WM_TRAYNOTIFY:
			pMainWnd->OnTrayNotify(lParam);
			return 0;
#ifdef REGISTER_WAKE_EVENT
		case WM_WAKEUP:
			pMainWnd->OnWakeUp();
			return 0;
#endif
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////////////
// メッセージハンドラ
void CMainWnd::OnCreate(HWND hWnd)
{
	m_hWnd = hWnd;

	// オプションクラスの作成
	m_pOptions = GetOptionsClass();

	// スキンクラスの作成
	m_pSkin = GetSkinClass();

	// 設定の読み込み
	Load();

	// イメージのロード
	if (!InitImages()) {
		DeleteImages();
	}

	// 初期化処理
	MAP_SetMessageWindow(m_hMap, m_hWnd);
	SetTimer(m_hWnd, ID_TIMER_TIME, TIMER_TIME_INTERVAL, NULL);
	SetTimer(m_hWnd, ID_TIMER_SUSPEND, TIMER_SUSPEND_INTERVAL, NULL);
	SetTimer(m_hWnd, ID_TIMER_VOLSLIDER, TIMER_VOLSLIDER_INTERVAL, NULL);

	// アイコンのセット
	HICON hIcon = LoadIcon(GetInst(), (LPCTSTR)IDI_MAIN);
	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	DestroyIcon(hIcon);

#ifdef _WIN32_WCE
	hIcon = (HICON)LoadImage(GetInst(), (LPCTSTR)IDI_MAIN, IMAGE_ICON, 16, 16, 0);
	SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	DestroyIcon(hIcon);
#endif

#ifdef REGISTER_WAKE_EVENT
	TCHAR sz[MAX_PATH];
	GetModuleFileName(GetInst(), sz, MAX_PATH);
	LPTSTR psz = _tcschr(sz + 1, _T('\\'));
	if (psz) {
		*psz = NULL;
		if (_tcscmp(sz, _T("\\")) != 0 &&
			!(GetFileAttributes(sz) & FILE_ATTRIBUTE_TEMPORARY))
			m_fRegEvent = TRUE;
		*psz = _T('\\');
	}
	else m_fRegEvent = TRUE;


	if (m_fRegEvent)
		CeRunAppAtEvent(sz, NOTIFICATION_EVENT_WAKEUP);
#endif
}

void CMainWnd::OnEraseBkgnd(HDC hDC)
{
}

BOOL NeedToRepaint(RECT* prc1, RECT* prc2)
{
	RECT rc;
	IntersectRect(&rc, prc1, prc2);
	return !IsRectEmpty(&rc);
}

void CMainWnd::OnPaint(HDC hDC, PAINTSTRUCT* pps)
{
	RECT rc;

	if (!m_hMemDC || !m_hMemBmp)
		return;

	// メモリDCを作成し背景を描画
	BitBlt(m_hMemDC, pps->rcPaint.left, pps->rcPaint.top,
		RECT_WIDTH(&pps->rcPaint), RECT_HEIGHT(&pps->rcPaint),
		m_hBakDC, pps->rcPaint.left, pps->rcPaint.top, SRCCOPY);

	// ボタン
	for (int i = 0; i < m_nBtnMax; i++) {
		SetRect(&rc, BTN_LEFT + (BTN_WIDTH * i), m_nBtnTop, 
			BTN_LEFT + (BTN_WIDTH * (i + 1)), m_nBtnTop + BTN_HEIGHT);
		if (NeedToRepaint(&pps->rcPaint, &rc))
			DrawButton(m_hMemDC, i);
	}

	// トラックNo.
	SetRect(&rc, TRACK_LEFT, TRACK_TOP, 
			TRACK_LEFT + TRACK_WIDTH, TRACK_TOP + TRACK_HEIGHT);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawTrack(m_hMemDC);

	// 時刻
	SetRect(&rc, TIME_LEFT, TIME_TOP, 
			TIME_LEFT + TIME_WIDTH, TIME_TOP + TIME_HEIGHT);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawTime(m_hMemDC);

	// シークバー
	SetRect(&rc, SEEK_LEFT, m_nSeekTop, 
			SEEK_LEFT + m_nSeekWidth, m_nSeekTop + SEEK_HEIGHT);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawSeek(m_hMemDC);

	// 情報
	SetRect(&rc, INFO_LEFT, INFO_TOP, 
			INFO_LEFT + INFO_WIDTH, INFO_TOP + INFO_HEIGHT);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawInfo(m_hMemDC);

	// オプション
	SetRect(&rc, PLAYOPT_LEFT, PLAYOPT_TOP, 
			PLAYOPT_LEFT + PLAYOPT_WIDTH, PLAYOPT_TOP + PLAYOPT_HEIGHT);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawPlayOption(m_hMemDC);

	// タイトル (ボリューム)
	SetRect(&rc, TITLE_LEFT, m_nTitleTop, 
		TITLE_LEFT + m_nTitleWidth, m_nTitleTop + m_nTitleHeight);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawTitle(m_hMemDC);

	// ステータス
	SetRect(&rc, STATUS_LEFT, m_nStatusTop, 
		STATUS_LEFT + STATUS_WIDTH, m_nStatusTop + m_nStatusHeight);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawStatus(m_hMemDC);

	// ピーク
	SetRect(&rc, PEEK_LEFT, PEEK_TOP, 
		PEEK_LEFT + PEEK_WIDTH, PEEK_TOP + PEEK_HEIGHT);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawPeek(m_hMemDC);

	// ストリームタイトル
	SetRect(&rc, STREAM_TITLE_LEFT, m_nStreamTitleTop, 
		STREAM_TITLE_LEFT + m_nStreamTitleWidth, m_nStreamTitleTop + m_nStreamTitleHeight);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawStreamTitle(m_hMemDC);

	// ボリュームスライダー
	SetRect(&rc, m_nVolumeSliderLeft, m_nVolumeSliderTop, 
		m_nVolumeSliderLeft + m_nVolumeSliderWidth, m_nVolumeSliderTop + m_nVolumeSliderHeight);
	if (NeedToRepaint(&pps->rcPaint, &rc))
		DrawVolumeSlider(m_hMemDC);

	// ウインドウDCに転送
	BitBlt(hDC, pps->rcPaint.left, pps->rcPaint.top,
		RECT_WIDTH(&pps->rcPaint), RECT_HEIGHT(&pps->rcPaint),
		m_hMemDC, pps->rcPaint.left, pps->rcPaint.top, SRCCOPY);
}

void CMainWnd::OnClose()
{
	// レジューム情報を保存しておく
	if (m_pOptions->m_fResume) {
		m_pOptions->m_nResumeTrack = m_nCurrent;
		m_pOptions->m_nResumeTime = MAP_GetCurrent(m_hMap);
	}

	// ファイルを閉じる
	Close();
	MAP_AudioDeviceClose(m_hMap);

	// リソースを解放する
	if (m_pPlayListDlg) {
		m_pPlayListDlg->Close();
		delete m_pPlayListDlg;
		m_pPlayListDlg = NULL;
	}

	// リストを保存する
	if (m_pOptions->m_fSaveDefList) {
		TCHAR szPath[MAX_PATH];
		GetDefPlayListPath(szPath);
		SavePlayList(szPath);
	}

	// タスクトレイアイコンの削除
	RegisterTrayIcon(FALSE);

	// ウインドウを元に戻す
	ShowWindow(m_hWnd, SW_HIDE);
	SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE);
	ShowWindow(m_hWnd, SW_RESTORE);

	// 状態を保存する
	Save();

#ifdef REGISTER_WAKE_EVENT
	if (m_fRegEvent) {
		TCHAR sz[MAX_PATH];
		GetModuleFileName(GetInst(), sz, MAX_PATH);
		CeRunAppAtEvent(sz, NULL);
	}
#endif

	// ウインドウの破棄
	DeleteImages();
	KillTimer(m_hWnd, ID_TIMER_TIME);
	KillTimer(m_hWnd, ID_TIMER_SUSPEND);
	KillTimer(m_hWnd, ID_TIMER_SCROLLTITLE);
	KillTimer(m_hWnd, ID_TIMER_SCROLLSTREAM);
	KillTimer(m_hWnd, ID_TIMER_STREAMINGWAIT);
	KillTimer(m_hWnd, ID_TIMER_SLEEPTIMER);
	DestroyWindow(m_hWnd);
}

void CMainWnd::OnDestroy()
{
	PostQuitMessage(0);
}

void CMainWnd::OnTimer(UINT uID)
{
	if (uID == ID_TIMER_TIME) {
		UpdateTime();
	}
	else if (uID == ID_TIMER_SUSPEND) {
#ifdef _WIN32_WCE
		// サスペンド防止
		if (MAP_GetStatus(m_hMap) == MAP_STATUS_PLAY) {
			//keybd_event(VK_F4, 0x1e, KEYEVENTF_KEYUP, 1);
			SystemIdleTimerReset();
		}
#endif
	}
	else if (uID == ID_TIMER_VOLUME) {
		KillTimer(m_hWnd, ID_TIMER_VOLUME);
		m_fDrawVolume = FALSE;
		UpdateTitle();
	}
	else if (uID == ID_TIMER_VOLSLIDER) {
		DWORD dwVolume;
		waveOutGetVolume(NULL, &dwVolume);
		if (m_dwCurrentVolume != dwVolume)
			UpdateVolumeSlider();
	}
	else if (uID == ID_TIMER_SCROLLTITLE) {
		if (++m_nTitleWait >=  SCROLL_TITLE_INITWAIT) {
			if (m_pOptions->m_fScrollTitle) {
				if (!m_fDrawVolume) {
					if (m_nTitleLen - SCROLL_TITLE_MARGIN > m_nTitleWidth) {
						m_nTitlePos += SCROLL_TITLE_INTERVAL;
						UpdateTitle();
					}
				}
			}
		}
	}
	else if (uID == ID_TIMER_SCROLLSTREAM) {
		if (++m_nStreamTitleWait >=  SCROLL_TITLE_INITWAIT) {
			FILEINFO* pInfo = GetInfo(GetCurrentFile());
			if (pInfo && IsURL(pInfo->szPath)) {
				if (m_nStreamTitleLen - SCROLL_TITLE_MARGIN > m_nStreamTitleWidth) {
					m_nStreamTitlePos += SCROLL_TITLE_INTERVAL;
					UpdateStreamTitle();
				}
			}
		}
	}
	else if (uID == ID_TIMER_STREAMINGWAIT) {
		UpdateStreamTitle();
		if (MAP_GetStatus(m_hMap) != MAP_STATUS_WAIT)
			KillTimer(m_hWnd, ID_TIMER_STREAMINGWAIT);
	}
	else if (uID == ID_TIMER_SLEEPTIMER) {
		if (m_pOptions->m_nSleepTime != -1) {
			DWORD dwTick = GetTickCount();
			m_pOptions->m_nSleepTime += dwTick - m_pOptions->m_dwSleepLast;
			m_pOptions->m_dwSleepLast = dwTick;

			if (m_pOptions->m_nSleepTime > m_pOptions->m_nSleepMinutes * 60 * 1000) {
				m_pOptions->m_nSleepTime = -1;
				KillTimer(m_hWnd, ID_TIMER_SLEEPTIMER);

				OnPlayStop();
#ifdef _WIN32_WCE
				if (m_pOptions->m_fSleepPowerOff) {
					MAP_AudioDeviceClose(m_hMap);
					keybd_event(VK_OFF, 0, KEYEVENTF_SILENT, 0);
					keybd_event(VK_OFF, 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 0);
				}
#endif
			}
		}
	}
}

void CMainWnd::OnLButtonDown(int x, int y)
{
#ifdef _WIN32_WCE
	if (GetAsyncKeyState(VK_MENU) & 0x8000) {
		return;
	}
	else if (x >= BTN_LEFT && y >= m_nBtnTop &&
#else
	if (x >= BTN_LEFT && y >= m_nBtnTop &&
#endif
		x < BTN_LEFT + BTN_WIDTH * m_nBtnMax &&
		y < m_nBtnTop + BTN_HEIGHT) {

		// ボタンの位置を検出
		m_fPressed = TRUE;
		m_nPressed = (x - BTN_LEFT) / BTN_WIDTH;

		RECT rc = {
			BTN_LEFT + BTN_WIDTH * m_nPressed,
			m_nBtnTop,
			BTN_LEFT + BTN_WIDTH * (m_nPressed + 1),
			m_nBtnTop + BTN_HEIGHT
		};
		InvalidateRect(m_hWnd, &rc, TRUE);
		UpdateWindow(m_hWnd);

		SetCapture(m_hWnd);
		return;
	}
	else if (x >= TIME_LEFT &&
		x < TIME_LEFT + TIME_WIDTH &&
		y >= TIME_TOP && 
		y < TIME_TOP + TIME_HEIGHT) {
		// 時刻表示リバース
		m_pOptions->m_fTimeReverse = !m_pOptions->m_fTimeReverse;
		UpdateTime();
	}
	else if (x >= PEEK_LEFT &&
		x < PEEK_LEFT + PEEK_WIDTH &&
		y >= PEEK_TOP && 
		y < PEEK_TOP + PEEK_HEIGHT) {
		// ピークバー表示切り替え
		m_nPeek[0] = m_nPeek[1] = 0;
		UpdatePeek();
		m_pOptions->m_fDrawPeek = !m_pOptions->m_fDrawPeek;
		MAP_SetScanPeek(m_hMap, m_pOptions->m_fDrawPeek);
	}
	else if (x >= SEEK_LEFT &&
		y >= m_nTitleTop &&
		x < SEEK_LEFT + m_nSeekWidth &&
		y < DISP_TOP + m_nDispHeight) {
		// シーク
		int nDur = MAP_GetDuration(m_hMap);
		int nSeek = int(((double)x - SEEK_LEFT) / m_nSeekWidth * nDur);
		Seek(nSeek);
		return;
	}
	else if (x >= m_nVolumeSliderLeft &&
		y >= m_nVolumeSliderTop &&
		x < m_nVolumeSliderLeft + m_nVolumeSliderWidth &&
		y < m_nVolumeSliderTop + m_nVolumeSliderHeight) {
		// ボリュームスライダー
		m_fVolPressed = TRUE;

		double dPos = x - m_nVolumeSliderLeft - m_nVolumeTrackWidth / 2;
		dPos = dPos * 100 / (m_nVolumeSliderWidth - m_nVolumeTrackWidth);
		dPos = dPos * 0xFFFF / 100;

		if (dPos < 0)
			dPos = 0;
		if (dPos > 0xFFFF)
			dPos = 0xFFFF;

		waveOutSetVolume(NULL, MAKELONG((int)dPos, (int)dPos));

		SetCapture(m_hWnd);
		UpdateVolumeSlider();

		// 表示を更新 (数秒後に元に戻す)
		KillTimer(m_hWnd, ID_TIMER_VOLUME);
		m_fDrawVolume = TRUE;
		UpdateTitle();
		SetTimer(m_hWnd, ID_TIMER_VOLUME, TIMER_VOLUME_INTERVAL, NULL);
	}
#ifdef _WIN32_WCE
	// 上記以外のものでディスプレイ以外をタップした場合は
	// メニューを表示
	else if (x < DISP_LEFT ||
			x > DISP_LEFT + DISP_WIDTH ||
			y < DISP_TOP || 
			y > DISP_TOP + DISP_HEIGHT) {
		POINT pt = {x, y};
		OnShowMenu(&pt);
	}
#endif

	// 位置を記憶しておく
	// ウインドウ移動用
	m_ptPressed.x = x;
	m_ptPressed.y = y;
}

void CMainWnd::OnMouseMove(int fKey, int x, int y)
{
	if (!(fKey & MK_LBUTTON))
		return;

	// ボタン
	if (m_nPressed != -1) {
		RECT rc = {
			BTN_LEFT + BTN_WIDTH * m_nPressed,
			m_nBtnTop,
			BTN_LEFT + BTN_WIDTH * (m_nPressed + 1),
			m_nBtnTop + BTN_HEIGHT
		};

		if (m_fPressed &&
			(x < rc.left || y < rc.top ||
			x >= rc.right || y >= rc.bottom)) {
			m_fPressed = FALSE;
			InvalidateRect(m_hWnd, &rc, TRUE);
			UpdateWindow(m_hWnd);
		}
		else if (!m_fPressed && 
			(x >= rc.left && y >= rc.top &&
			x < rc.right && y < rc.bottom)) {
			m_fPressed = TRUE;
			InvalidateRect(m_hWnd, &rc, TRUE);
			UpdateWindow(m_hWnd);
		}
	}
	else if (m_fVolPressed) {
		double dPos = x - m_nVolumeSliderLeft - m_nVolumeTrackWidth / 2;
		dPos = dPos * 100 / (m_nVolumeSliderWidth - m_nVolumeTrackWidth);
		dPos = dPos * 0xFFFF / 100;
		if (dPos < 0)
			dPos = 0;
		if (dPos > 0xFFFF)
			dPos = 0xFFFF;

		waveOutSetVolume(NULL, MAKELONG((int)dPos, (int)dPos));

		SetCapture(m_hWnd);
		UpdateVolumeSlider();

		// 表示を更新 (数秒後に元に戻す)
		KillTimer(m_hWnd, ID_TIMER_VOLUME);
		m_fDrawVolume = TRUE;
		UpdateTitle();
		SetTimer(m_hWnd, ID_TIMER_VOLUME, TIMER_VOLUME_INTERVAL, NULL);
	}
}

void CMainWnd::OnLButtonUp(int x, int y)
{
	if (m_nPressed != -1) {
		ReleaseCapture();

		if (m_fPressed) {
			// 表示を元に戻す
			m_fPressed = FALSE;
			RECT rc = {
				BTN_LEFT + BTN_WIDTH * m_nPressed,
				m_nBtnTop,
				BTN_LEFT + BTN_WIDTH * (m_nPressed + 1),
				m_nBtnTop + BTN_HEIGHT
			};
			InvalidateRect(m_hWnd, &rc, TRUE);
			UpdateWindow(m_hWnd);

			// コマンド送信
			int nCommand[] = {
				IDM_FILE_OPEN, 
				IDM_PLAY_PLAYPAUSE,
				IDM_PLAY_STOP,
				IDM_PLAY_PREV,
				IDM_PLAY_REW,
				IDM_PLAY_FF,
				IDM_PLAY_NEXT,
				IDM_SHOW_MENU
			};
			PostMessage(m_hWnd, WM_COMMAND, nCommand[m_nPressed], 0);
		}
	}
	else if (m_fVolPressed) {
		ReleaseCapture();
	}
#ifdef _WIN32_WCE
	else if (GetAsyncKeyState(VK_MENU) & 0x8000) {
		POINT pt = {x, y};
		OnShowMenu(&pt);
	}
#endif
	
	m_fPressed = FALSE;
	m_nPressed = -1;
	m_ptPressed.x = -1;
	m_ptPressed.y = -1;
	m_fVolPressed = FALSE;
}

void CMainWnd::OnMove(int x, int y)
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

void CMainWnd::OnSize(UINT nType, int cx, int cy)
{
	if (m_pOptions->m_fTrayIcon && nType == SIZE_MINIMIZED) {
		RegisterTrayIcon(TRUE);
		ShowWindow(m_hWnd, SW_HIDE);
	}
	else if (nType == SIZE_RESTORED) {
		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}
}

void CMainWnd::OnActivate(int nActive, BOOL fMinimized, HWND hwndPrevious)
{
}

void CMainWnd::OnHotKey(int nId, UINT fuModifiers, UINT uVirtKey)
{
}

#ifdef _WIN32_WCE
void CMainWnd::OnCopyData(COPYDATASTRUCT* pcds)
{
	if (pcds->dwData == WM_DROPFILES) {
		DeleteAllFile();

		LPTSTR pszPath = (LPTSTR)pcds->lpData;		
		while (TRUE) {
			DWORD dw = GetFileAttributes(pszPath);
			if (dw & FILE_ATTRIBUTE_DIRECTORY)
				AddDir(pszPath);
			else {
				if (IsPlayList(pszPath))
					LoadPlayList(pszPath);
				else
					AddFile(pszPath);
			}

			pszPath += _tcslen(pszPath);
			if (*pszPath != NULL || *(pszPath + 1) == NULL)
				break;

			pszPath++;
		}
		
		if (m_pPlayListDlg)
			m_pPlayListDlg->UpdatePlayList();

		Play();
	}
}
#else
void CMainWnd::OnDropFiles(HDROP hDrop)
{
	DeleteAllFile();
	AddDropFiles(hDrop);
	Play();
}

void CMainWnd::AddDropFiles(HDROP hDrop)
{
	TCHAR szFile[MAX_PATH];
	int nIndex = 0;

	// ファイルを追加していく
	while (DragQueryFile(hDrop, nIndex++, szFile, MAX_PATH)) {
		DWORD dwAttr = GetFileAttributes(szFile);
		if (dwAttr != 0xFFFFFFFF) {
			if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
				AddDir(szFile);
			else if(IsPlayList(szFile))
				LoadPlayList(szFile);
			else
				AddFile(szFile);
		}
	}
	DragFinish(hDrop);
	
	if (m_pPlayListDlg)
		m_pPlayListDlg->UpdatePlayList();
}
#endif

void CMainWnd::OnMsgStatus(MAP_STATUS status, BOOL fError)
{
	if (status == MAP_STATUS_STOP) {
		Stop();

		if(m_fStopAfterCurr) {
			m_fStopAfterCurr = FALSE;
			return;
		}

		// 1リピート時
		if (m_pOptions->m_fRepeat == COptions::RepeatOne) {
			if (!fError)
				Play();
			else {
				int n = m_nCurrent;
				Close();
				Open(m_nCurrent);
			}
		}
		else {
			// 次を再生
			int nNext = GetNextIndex(TRUE, fError);
			if (nNext != OPEN_NONE) {
				fError ? Open(nNext) : Play(nNext);
			}
			else {
				// リストの終端に達したとき
				UpdateTime();
				UpdateStatus();
				UpdateStreamTitle();

				// リストの先頭に戻す
				nNext = m_pOptions->m_fShuffle ? GetNextIndex(TRUE) : 0;
				Open(nNext);
			}
		}
	}
	else if (status == MAP_STATUS_PLAY) {
		MAP_GetFileInfo(m_hMap, &GetInfo(m_nCurrent)->info);
		UpdateInfo();
		UpdateTitle();
		UpdateStreamTitle();
	}
	else if (status == MAP_STATUS_WAIT) {
		if (MAP_GetStreamingStatus(m_hMap) > MAP_STREAMING_DISCONNECTED) {
			UpdateStreamTitle();
			SetTimer(m_hWnd, ID_TIMER_STREAMINGWAIT, TIMER_STREAMINGWAIT_INTERVAL, NULL);
		}
	}
	UpdateStatus();
}

void CMainWnd::OnMsgPeek(int nLeft, int nRight)
{
	if (!m_pOptions->m_fDrawPeek)
		return;

	m_nPeek[0] = nLeft;
	m_nPeek[1] = nRight;
	UpdatePeek();
}

void CMainWnd::OnMsgStreamTitle(LPTSTR pszTitle)
{
	UpdateStreamTitle();
}

LRESULT CMainWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam)) {
		case IDM_APP_EXIT:
			PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			return 0;
		case IDM_FILE_OPEN:
			OnFileOpen();
			return 0;
		case IDM_FILE_OPEN_LOCATION:
			OnFileOpenLocation();
			return 0;
		case IDM_FILE_ADD:
			OnFileAdd();
			return 0;
		case IDM_FILE_FIND:
			OnFileFind();
			return 0;
		case IDM_FILE_LIST:
			OnFileList();
			return 0;
		case IDM_FILE_SAVE_LIST:
			OnFileSaveList();
			return 0;
		case IDM_PLAY_PLAYPAUSE:
			OnPlayPlayPause();
			return 0;
		case IDM_PLAY_STOPAFTERCURR:
			OnPlayStopAfterCurrent();
			return 0;
		case IDM_PLAY_STOP:
			OnPlayStop();
			return 0;
		case IDM_PLAY_PREV:
			OnPlayPrev();
			return 0;
		case IDM_PLAY_NEXT:
			OnPlayNext();
			return 0;
		case IDM_PLAY_REW:
			OnPlayRew();
			return 0;
		case IDM_PLAY_FF:
			OnPlayFf();
			return 0;
		case IDM_PLAY_REPEAT:
			OnPlayRepeat();
			return 0;
		case IDM_PLAY_REPEAT_NONE:
			OnPlayRepeatNone();
			return 0;
		case IDM_PLAY_REPEAT_ALL:
			OnPlayRepeatAll();
			return 0;
		case IDM_PLAY_REPEAT_ONE:
			OnPlayRepeatOne();
			return 0;
		case IDM_PLAY_SHUFFLE:
			OnPlayShuffle();
			return 0;
		case IDM_SHOW_MENU:
			OnShowMenu();
			return 0;
		case IDM_TOOL_EFFECT:
			OnToolEffect();
			return 0;
		case IDM_TOOL_OPTION:
			OnToolOption();
			return 0;
		case IDM_TOOL_VOLUP:
			OnToolVolUp();
			return 0;
		case IDM_TOOL_VOLDOWN:
			OnToolVolDown();
			return 0;
		case IDM_TOOL_TOPMOST:
			OnToolTopMost();
			return 0;
		case IDM_TOOL_SLEEPTIMER:
			OnToolSleepTimer();
			return 0;
		case IDM_HELP_ABOUT:
			m_pOptions->ShowAboutDlg(m_hWnd, m_hMap);
			return 0;
		case IDM_APP_SHOWHIDE:
			if (GetWindowLong(m_hWnd, GWL_STYLE) & WS_VISIBLE) {
#ifndef _WIN32_WCE_PPC
				ShowWindow(m_hWnd, SW_MINIMIZE);
#endif
			}
			else {
				ShowWindow(m_hWnd, SW_RESTORE);
				RegisterTrayIcon(FALSE);
				SetForegroundWindow(m_hWnd);
			}
			return 0;
		default:
			return 0;
	}
}

void CMainWnd::OnTrayNotify(UINT uMsg)
{
	if (GetWindowLong(m_hWnd, GWL_STYLE) & WS_DISABLED)
		return;

	if (uMsg == WM_LBUTTONDBLCLK) {
		ShowWindow(m_hWnd, SW_RESTORE);
		RegisterTrayIcon(FALSE);
		SetForegroundWindow(m_hWnd);
	}
	else if (uMsg == WM_LBUTTONDOWN || 
			uMsg == WM_RBUTTONDOWN) {

		if (m_hMenu)
			return;

		m_hMenu = LoadMenu(GetInst(), (LPCTSTR)IDR_POPUP);
		HMENU hSub = GetSubMenu(m_hMenu, IDM_SUBMENU_TRAY);

		POINT pt;
		UINT uFlags;
#ifdef _WIN32_WCE
		pt.x = GetSystemMetrics(SM_CXSCREEN);
		pt.y = GetSystemMetrics(SM_CYSCREEN) - 25;
		uFlags = TPM_RIGHTALIGN | TPM_BOTTOMALIGN;
#else
		GetCursorPos(&pt);
		uFlags = TPM_LEFTALIGN | TPM_TOPALIGN;
		SetMenuDefaultItem(hSub, IDM_APP_SHOWHIDE, FALSE);
#endif

		switch (m_pOptions->m_fRepeat) {
		case COptions::RepeatAll:
			CheckMenuItem(hSub, IDM_PLAY_REPEAT_ALL, MF_BYCOMMAND | MF_CHECKED);
			break;
		case COptions::RepeatOne:
			CheckMenuItem(hSub, IDM_PLAY_REPEAT_ONE, MF_BYCOMMAND | MF_CHECKED);
			break;
		default:
			CheckMenuItem(hSub, IDM_PLAY_REPEAT_NONE, MF_BYCOMMAND | MF_CHECKED);
			break;
		}
		if (m_pOptions->m_fShuffle)
			CheckMenuItem(hSub, IDM_PLAY_SHUFFLE, MF_BYCOMMAND | MF_CHECKED);
		if (m_pOptions->m_fTopMost)
			CheckMenuItem(hSub, IDM_TOOL_TOPMOST, MF_BYCOMMAND | MF_CHECKED);

		SetForegroundWindow(m_hWnd);
		TrackPopupMenu(hSub, uFlags, pt.x, pt.y, 0, m_hWnd, NULL);
		DestroyMenu(m_hMenu);
		m_hMenu = NULL;
	}
}

#ifdef REGISTER_WAKE_EVENT
void CMainWnd::OnWakeUp()
{
	if (MAP_GetStatus(m_hMap) == MAP_STATUS_PLAY) {
		Pause();
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////
void CMainWnd::RegisterClass()
{
	WNDCLASS	wc;
	memset(&wc, 0, sizeof(WNDCLASS));
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)MainWndProc;
    wc.hInstance = GetInst();
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wc.lpszClassName = MAINWND_CLASS_NAME;
#ifndef _WIN32_WCE
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
#endif
	::RegisterClass(&wc);
}

BOOL CMainWnd::InitImages()
{
	m_pSkin->Load(m_pOptions->m_szSkinFile);

	// 描画用DCを作成
	HDC hDC = GetDC(m_hWnd);
	m_hMemDC = CreateCompatibleDC(hDC);
	m_hMemBmp = CreateCompatibleBitmap(hDC, m_nClientWidth, m_nClientHeight);
	SelectObject(m_hMemDC, m_hMemBmp);

	// 背景をメモリDCに描画しておく
	m_hBakDC = CreateCompatibleDC(hDC);
	m_hBakBmp = CreateCompatibleBitmap(hDC, m_nClientWidth, m_nClientHeight);
	SelectObject(m_hBakDC, m_hBakBmp);
	DrawBackground(m_hBakDC);
	ReleaseDC(m_hWnd, hDC);

	// フォント
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfCharSet = DEFAULT_CHARSET;
#ifdef _WIN32_WCE
	lf.lfHeight = 11;
	_tcscpy(lf.lfFaceName, _T("Tahoma"));
#else
	lf.lfHeight = 10;
#endif
	m_hFnt = CreateFontIndirect(&lf);
	m_hFntTitle = m_hFnt;

	// スキンの過去バージョンと互換性を保つため
	if (m_pSkin->GetImageList(IMAGELIST_BUTTON))
		m_nBtnMax = BTN_MAX;
	else
		m_nBtnMax = BTN_MAX - 1;

	m_nTitlePos = 0;
	m_nTitleLen = 0;
	m_nStreamTitlePos = 0;
	m_nStreamTitleLen = 0;

	// ボリュームスライダーが有効か無効か
	if (m_pSkin->GetImageList(IMAGELIST_VOLSLIDER) && m_pSkin->GetImageList(IMAGELIST_VOLTRACK)) {
		m_nVolumeSliderLeft = VOLSLIDER_LEFT;
		m_nVolumeSliderTop = VOLSLIDER_TOP;
		m_nVolumeSliderWidth = VOLSLIDER_WIDTH;
		m_nVolumeSliderHeight = VOLSLIDER_HEIGHT;
	}
	else {
		m_nVolumeSliderLeft = 0;
		m_nVolumeSliderTop = 0;
		m_nVolumeSliderWidth = 0;
		m_nVolumeSliderHeight = 0;
	}

	return TRUE;
}

void CMainWnd::DeleteImages()
{
	m_pSkin->Unload();
	if (m_hBakDC) {
		DeleteDC(m_hBakDC);
		m_hBakDC = NULL;
	}
	if (m_hBakBmp) {
		DeleteObject(m_hBakBmp);
		m_hBakBmp = NULL;
	}
	if (m_hMemDC) {
		DeleteDC(m_hMemDC);
		m_hMemDC = NULL;
	}
	if (m_hMemBmp) {
		DeleteObject(m_hMemBmp);
		m_hMemBmp = NULL;
	}
	if (m_hFnt) {
		DeleteObject(m_hFnt);
		m_hFnt = NULL;
	}
	m_hFntTitle = NULL;
}

void CMainWnd::DrawBackground(HDC hDC)
{
	// 背景を描画
	if (GetDeviceCaps(hDC, BITSPIXEL) < 16 ||
		!m_pSkin->GetBkImage()) {
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		FillRect(hDC, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
	}
	else {
		HDC hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, m_pSkin->GetBkImage());
		BitBlt(hDC, 0, 0, m_nClientWidth, m_nClientHeight, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
	}

	// ディスプレイを描画
	if (m_pSkin->IsDispVisible())
		DrawDispEdge(hDC);
}

void CMainWnd::DrawDispEdge(HDC hDC)
{
	RECT rc = {DISP_LEFT, DISP_TOP,
		DISP_LEFT + m_nDispWidth, DISP_TOP + m_nDispHeight};

	HBRUSH hbr = CreateSolidBrush(m_pSkin->GetColor(COLOR_DISP));
	FillRect(hDC, &rc, hbr);
	DrawEdge(hDC, &rc, BDR_SUNKENINNER, BF_BOTTOMRIGHT | BF_TOPLEFT);
	DeleteObject(hbr);
}

void CMainWnd::DrawButton(HDC hDC, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_nBtnMax)
		return;

	int x = BTN_LEFT + BTN_WIDTH * nIndex;
	int y = m_nBtnTop;

	if (m_pSkin->GetImageList(IMAGELIST_BUTTON)) {
		// 古いバージョンのボタン描画
		if (m_fPressed && m_nPressed == nIndex) {
			// ボタンが押されている場合
			RECT rc = {x, y, x + BTN_WIDTH, y + BTN_HEIGHT};
			DrawEdge(hDC, &rc, BDR_SUNKENINNER, BF_BOTTOMRIGHT | BF_TOPLEFT);

			// アイコンを描画
			ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_BUTTON), nIndex, hDC, 
				x + ((BTN_WIDTH - 16) / 2) + 1, y + ((BTN_HEIGHT - 16) / 2) + 1, ILD_TRANSPARENT);
		}
		else {
			ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_BUTTON), nIndex, hDC, 
				x + ((BTN_WIDTH - 16) / 2), y + ((BTN_HEIGHT - 16) / 2), ILD_TRANSPARENT);
		}
	}
	else {
		// 新しいバージョンのボタン描画
		if (m_fPressed && m_nPressed == nIndex) {
			// ボタンが押されている場合
			ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_BUTTON_PUSH), nIndex, hDC, 
				x, y , ILD_TRANSPARENT);
		}
		else {
			ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_BUTTON_NORMAL), nIndex, hDC, 
				x, y , ILD_TRANSPARENT);
		}
	}
}

void CMainWnd::DrawTrack(HDC hDC)
{
	int n = (m_nCurrent == -1) ? 0 : m_nCurrent + 1;
	n = n % 100;
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_NUMBER), n / 10, hDC, TRACK_LEFT, TRACK_TOP, ILD_TRANSPARENT);
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_NUMBER), n % 10, hDC, TRACK_LEFT + 10, TRACK_TOP, ILD_TRANSPARENT);
}

int CMainWnd::GetDrawTime()
{
	int nSec;
	
	FILEINFO* pInfo = (FILEINFO*)m_pListFile->GetAt(m_nCurrent);
	if (MAP_GetStatus(m_hMap) == MAP_STATUS_STOP)
		nSec = MAP_GetDuration(m_hMap) / 1000;
	else if (IsURL(pInfo->szPath))
		nSec = MAP_GetCurrent(m_hMap) / 1000;
	else if (m_pOptions->m_fTimeReverse)
		nSec = max((MAP_GetDuration(m_hMap) - MAP_GetCurrent(m_hMap)) / 1000, 0);
	else
		nSec = MAP_GetCurrent(m_hMap) / 1000;

	if (nSec >= 100 * 60)
		nSec %= (100 * 60);
	if (nSec < 0)
		nSec = 0;

	return nSec;
}

void CMainWnd::DrawTime(HDC hDC)
{
	int nSec = GetDrawTime();
	m_nLastDrawTime = nSec;

	// Minutes
	int n = nSec / 60;
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_NUMBER), n / 10, hDC, TIME_LEFT, TIME_TOP, ILD_TRANSPARENT);
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_NUMBER), n % 10, hDC, TIME_LEFT + 10, TIME_TOP, ILD_TRANSPARENT);

	// Separator
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_NUMBER), 10, hDC, TIME_LEFT + 20, TIME_TOP, ILD_TRANSPARENT);

	// Seconds
	n = nSec % 60;
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_NUMBER), n / 10, hDC, TIME_LEFT + 24, TIME_TOP, ILD_TRANSPARENT);
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_NUMBER), n % 10, hDC, TIME_LEFT + 34, TIME_TOP, ILD_TRANSPARENT);
}

void CMainWnd::DrawSeek(HDC hDC)
{
	int nDur = MAP_GetDuration(m_hMap);
	int nCur = MAP_GetCurrent(m_hMap);
	m_nLastDrawSeek = nCur;
	if (!nDur)
		return;

	if (nCur) {
		nCur = min((nCur * m_nSeekWidth) / nDur, m_nSeekWidth);
		RECT rc = {SEEK_LEFT, m_nSeekTop, SEEK_LEFT + nCur, m_nSeekTop + SEEK_HEIGHT};
		HBRUSH hbr = CreateSolidBrush(m_pSkin->GetColor(COLOR_SEEK));
		FillRect(hDC, &rc, hbr);
		DeleteObject(hbr);
	}
}

void CMainWnd::DrawInfo(HDC hDC)
{
	FILEINFO* pInfo = GetInfo(m_nCurrent);
	if (pInfo && pInfo->info.nSamplingRate && pInfo->info.nChannels) {
		TCHAR szInfo[128];
		RECT rc = {INFO_LEFT, INFO_TOP, INFO_LEFT + INFO_WIDTH,
					INFO_TOP + INFO_HEIGHT};

		wsprintf(szInfo, CTempStr(IDS_FMT_DISP_INFO), pInfo->info.nBitRate,
			pInfo->info.nSamplingRate / 1000, pInfo->info.nChannels == 1 ? _T("MONO") : _T("STEREO"));
		
		COLORREF crBk = SetTextColor(hDC, m_pSkin->GetColor(COLOR_INFO));
		int nOld = SetBkMode(hDC, TRANSPARENT);
		HFONT hBk = (HFONT)SelectObject(hDC, m_hFnt);
		DrawText(hDC, szInfo, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);	
		SetTextColor(hDC, crBk);
		SetBkMode(hDC, nOld);
		SelectObject(hDC, hBk);
	}
}

void CMainWnd::DrawPlayOption(HDC hDC)
{
	if (m_pOptions->m_fRepeat == COptions::RepeatAll) {
		ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_PLAYOPT), 0, hDC, PLAYOPT_LEFT, PLAYOPT_TOP, ILD_TRANSPARENT);
	}
	else if (m_pOptions->m_fRepeat == COptions::RepeatOne) {
		int nIndex = (ImageList_GetImageCount(m_pSkin->GetImageList(IMAGELIST_PLAYOPT)) > 2) ? 2 : 0;
		ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_PLAYOPT), nIndex, hDC, PLAYOPT_LEFT, PLAYOPT_TOP, ILD_TRANSPARENT);
	}
	if (m_pOptions->m_fShuffle) {
		ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_PLAYOPT), 1, hDC, PLAYOPT_LEFT, PLAYOPT_TOP + 8, ILD_TRANSPARENT);
	}
}

void CMainWnd::DrawStatus(HDC hDC)
{
	RECT rc = {STATUS_LEFT, m_nStatusTop,
				STATUS_LEFT + STATUS_WIDTH, m_nStatusTop + m_nStatusHeight};
	
	int nIndex;
	MAP_STATUS status = MAP_GetStatus(m_hMap);
	switch (status) {
	case MAP_STATUS_PLAY: nIndex = 1; break;
	case MAP_STATUS_PAUSE: nIndex = 2; break;	
	case MAP_STATUS_WAIT: nIndex = 3; break;	
	default: nIndex = 0;
	}
	
#ifdef _WIN32_WCE_PPC
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_STATUS), nIndex, hDC, STATUS_LEFT, 
									m_nStatusTop + (m_nStatusHeight - STATUS_WIDTH) - 2, ILD_TRANSPARENT);
#else
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_STATUS), nIndex, hDC, STATUS_LEFT, 
									m_nStatusTop, ILD_TRANSPARENT);
#endif
}

void CMainWnd::DrawTitle(HDC hDC)
{
	if (m_fDrawVolume) {
		DrawVolume(hDC);
		return;
	}

	TCHAR szTitle[MAX_PATH] = {0};
	GetTitle(m_nCurrent, szTitle);

	SIZE size;
	HFONT hBk = (HFONT)SelectObject(hDC, m_hFntTitle);
	GetTextExtentPoint32(hDC, szTitle, _tcslen(szTitle), &size);
	if (size.cx != m_nTitleLen - SCROLL_TITLE_MARGIN) {
		KillTimer(m_hWnd, ID_TIMER_SCROLLTITLE);

		m_nTitlePos = 0;
		m_nTitleWait = 0;
		m_nTitleLen = size.cx + SCROLL_TITLE_MARGIN;
		if (m_nTitleLen - SCROLL_TITLE_MARGIN > m_nTitleWidth)
			SetTimer(m_hWnd, ID_TIMER_SCROLLTITLE, TIMER_SCROLLTITLE_INTERVAL, NULL);
	}
	
	while (m_nTitlePos >= m_nTitleLen)
		m_nTitlePos = m_nTitlePos - m_nTitleLen;

	if (!m_pOptions->m_fScrollTitle || m_nTitlePos == 0 || m_nTitleLen - SCROLL_TITLE_MARGIN < m_nTitleWidth) {
		// スクロールなし
		RECT rc = {TITLE_LEFT, m_nTitleTop, 
			TITLE_LEFT + m_nTitleWidth, m_nTitleTop + m_nTitleHeight};

		COLORREF crBk = SetTextColor(hDC, m_pSkin->GetColor(COLOR_TITLE));
		int nOld = SetBkMode(hDC, TRANSPARENT);
		DrawText(hDC, szTitle, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);	
		SetTextColor(hDC, crBk);
		SetBkMode(hDC, nOld);
	}
	else {
		// スクロールあり
		RECT rc = {0, 0, m_nTitleLen, m_nTitleHeight};

		HDC hMemDC = CreateCompatibleDC(hDC);
		HBITMAP hBmp = CreateCompatibleBitmap(hDC, m_nTitleLen, m_nTitleHeight);
		SelectObject(hMemDC, hBmp);

		if (m_nTitleLen - m_nTitlePos < m_nTitleWidth) {
			BitBlt(hMemDC, m_nTitlePos, 0, m_nTitleLen - m_nTitlePos,
					m_nTitleHeight, hDC, TITLE_LEFT, m_nTitleTop, SRCCOPY);
			BitBlt(hMemDC, 0, 0, 
				m_nTitleWidth - (m_nTitleLen - m_nTitlePos),
				m_nTitleHeight, hDC, TITLE_LEFT + (m_nTitleLen - m_nTitlePos), m_nTitleTop, SRCCOPY);
		}
		else {
			BitBlt(hMemDC, m_nTitlePos, 0, m_nTitleWidth,
						m_nTitleHeight, hDC, TITLE_LEFT, m_nTitleTop, SRCCOPY);
		}

		COLORREF crBk = SetTextColor(hMemDC, m_pSkin->GetColor(COLOR_TITLE));
		int nOld = SetBkMode(hMemDC, TRANSPARENT);
		HFONT hFnt = (HFONT)SelectObject(hMemDC, m_hFntTitle);
		DrawText(hMemDC, szTitle, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);	
		SetTextColor(hMemDC, crBk);
		SetBkMode(hMemDC, nOld);
		SelectObject(hMemDC, hFnt);

		if (m_nTitleLen - m_nTitlePos < m_nTitleWidth) {
			BitBlt(hDC, TITLE_LEFT, m_nTitleTop, m_nTitleLen - m_nTitlePos, 
				m_nTitleHeight, hMemDC, m_nTitlePos, 0, SRCCOPY);
			BitBlt(hDC, TITLE_LEFT + (m_nTitleLen - m_nTitlePos), m_nTitleTop, 
				m_nTitleWidth - (m_nTitleLen - m_nTitlePos), m_nTitleHeight, hMemDC, 0, 0, SRCCOPY);
		}
		else {
			BitBlt(hDC, TITLE_LEFT, m_nTitleTop, m_nTitleWidth, 
				m_nTitleHeight, hMemDC, m_nTitlePos, 0, SRCCOPY);
		}

		DeleteDC(hMemDC);
		DeleteObject(hBmp);
	}
	SelectObject(hDC, hBk);
}

void CMainWnd::DrawVolume(HDC hDC)
{
	SIZE size;
	TCHAR szTitle[MAX_PATH] = {0};
	RECT rc = {TITLE_LEFT, m_nTitleTop, 
			TITLE_LEFT + m_nTitleWidth, m_nTitleTop + m_nTitleHeight};

	DWORD dwVolume;
	waveOutGetVolume(NULL, &dwVolume);
	dwVolume = max(LOWORD(dwVolume), HIWORD(dwVolume));
	dwVolume = dwVolume * 100 / 0xFFFF;
	wsprintf(szTitle, CTempStr(IDS_FMT_DISP_VOLUME), dwVolume);

	COLORREF crBk = SetTextColor(hDC, m_pSkin->GetColor(COLOR_TITLE));
	int nOld = SetBkMode(hDC, TRANSPARENT);
	HFONT hBk = (HFONT)SelectObject(hDC, m_hFntTitle);
	GetTextExtentPoint32(hDC, szTitle, _tcslen(szTitle), &size);
	DrawText(hDC, szTitle, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);	
	SetTextColor(hDC, crBk);
	SetBkMode(hDC, nOld);
	SelectObject(hDC, hBk);

#ifdef _WIN32_WCE_PPC
#define VOLBAR_LEFT		80
#define VOLBAR_WIDTH	125
#define VOLBAR_HEIGHT	8
#else
#define VOLBAR_LEFT		50
#define VOLBAR_WIDTH	145
#define VOLBAR_HEIGHT	8
#endif
	if (!dwVolume)
		return;

	int nWidth = VOLBAR_WIDTH * dwVolume / 100;
	RECT rcBar = {TITLE_LEFT + VOLBAR_LEFT, m_nTitleTop + m_nTitleHeight - size.cy + 1,
				TITLE_LEFT + VOLBAR_LEFT + nWidth, m_nTitleTop + m_nTitleHeight};
	HBRUSH hbr = CreateSolidBrush(m_pSkin->GetColor(COLOR_TITLE));
	FillRect(hDC, &rcBar, hbr);
	DeleteObject(hbr);
}

void CMainWnd::DrawPeek(HDC hDC)
{
	if (m_pOptions->m_fDrawPeek) {
		int n = min(PEEK_WIDTH * m_nPeek[0] / 100, PEEK_WIDTH);
		RECT rcLeft = {PEEK_LEFT, PEEK_TOP, PEEK_LEFT + n, PEEK_TOP + 2};
		HBRUSH hbr = CreateSolidBrush(m_pSkin->GetColor(COLOR_PEEKL));
		FillRect(hDC, &rcLeft, hbr);
		DeleteObject(hbr);

		n = min(PEEK_WIDTH * m_nPeek[1] / 100, PEEK_WIDTH);
		RECT rcRight = {PEEK_LEFT, PEEK_TOP + 3, PEEK_LEFT + n, PEEK_TOP + 5};
		hbr = CreateSolidBrush(m_pSkin->GetColor(COLOR_PEEKR));
		FillRect(hDC, &rcRight, hbr);
		DeleteObject(hbr);
	}
}

void CMainWnd::DrawStreamTitle(HDC hDC)
{
	TCHAR szTitle[MAX_URL] = {0};
	GetStreamTitle(szTitle);

	SIZE size;
	HFONT hBk = (HFONT)SelectObject(hDC, m_hFntTitle);
	GetTextExtentPoint32(hDC, szTitle, _tcslen(szTitle), &size);
	if (size.cx != m_nStreamTitleLen - SCROLL_TITLE_MARGIN) {
		KillTimer(m_hWnd, ID_TIMER_SCROLLSTREAM);

		m_nStreamTitlePos = 0;
		m_nStreamTitleWait = 0;
		m_nStreamTitleLen = size.cx + SCROLL_TITLE_MARGIN;
		if (m_nStreamTitleLen - SCROLL_TITLE_MARGIN > m_nStreamTitleWidth)
			SetTimer(m_hWnd, ID_TIMER_SCROLLSTREAM, TIMER_SCROLLSTREAM_INTERVAL, NULL);
	}

	while (m_nStreamTitlePos >= m_nStreamTitleLen)
		m_nStreamTitlePos = m_nStreamTitlePos - m_nStreamTitleLen;

	if (!m_pOptions->m_fScrollTitle || m_nStreamTitlePos == 0 ||
		m_nStreamTitleLen - SCROLL_TITLE_MARGIN < m_nStreamTitleWidth) {
		// スクロールなし
		RECT rc = {STREAM_TITLE_LEFT, m_nStreamTitleTop, 
			STREAM_TITLE_LEFT + m_nStreamTitleWidth, m_nStreamTitleTop + m_nStreamTitleHeight};

		COLORREF crBk = SetTextColor(hDC, m_pSkin->GetColor(COLOR_TITLE));
		int nOld = SetBkMode(hDC, TRANSPARENT);
		DrawText(hDC, szTitle, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);	
		SetTextColor(hDC, crBk);
		SetBkMode(hDC, nOld);
	}
	else {
		// スクロールあり
		RECT rc = {0, 0, m_nStreamTitleLen, m_nStreamTitleHeight};

		HDC hMemDC = CreateCompatibleDC(hDC);
		HBITMAP hBmp = CreateCompatibleBitmap(hDC, m_nStreamTitleLen, m_nStreamTitleHeight);
		SelectObject(hMemDC, hBmp);

		if (m_nStreamTitleLen - m_nStreamTitlePos < m_nStreamTitleWidth) {
			BitBlt(hMemDC, m_nStreamTitlePos, 0, m_nStreamTitleLen - m_nStreamTitlePos,
					m_nStreamTitleHeight, hDC, STREAM_TITLE_LEFT, m_nStreamTitleTop, SRCCOPY);
			BitBlt(hMemDC, 0, 0, 
				m_nStreamTitleWidth - (m_nStreamTitleLen - m_nStreamTitlePos),
				m_nStreamTitleHeight, hDC, STREAM_TITLE_LEFT + (m_nStreamTitleLen - m_nStreamTitlePos), m_nStreamTitleTop, SRCCOPY);
		}
		else {
			BitBlt(hMemDC, m_nStreamTitlePos, 0, m_nStreamTitleWidth,
						m_nStreamTitleHeight, hDC, STREAM_TITLE_LEFT, m_nStreamTitleTop, SRCCOPY);
		}

		COLORREF crBk = SetTextColor(hMemDC, m_pSkin->GetColor(COLOR_TITLE));
		int nOld = SetBkMode(hMemDC, TRANSPARENT);
		HFONT hFnt = (HFONT)SelectObject(hMemDC, m_hFntTitle);
		DrawText(hMemDC, szTitle, -1, &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX);	
		SetTextColor(hMemDC, crBk);
		SetBkMode(hMemDC, nOld);
		SelectObject(hMemDC, hFnt);

		if (m_nStreamTitleLen - m_nStreamTitlePos < m_nStreamTitleWidth) {
			BitBlt(hDC, STREAM_TITLE_LEFT, m_nStreamTitleTop, m_nStreamTitleLen - m_nStreamTitlePos, 
				m_nStreamTitleHeight, hMemDC, m_nStreamTitlePos, 0, SRCCOPY);
			BitBlt(hDC, STREAM_TITLE_LEFT + (m_nStreamTitleLen - m_nStreamTitlePos), m_nStreamTitleTop, 
				m_nStreamTitleWidth - (m_nStreamTitleLen - m_nStreamTitlePos), m_nStreamTitleHeight, hMemDC, 0, 0, SRCCOPY);
		}
		else {
			BitBlt(hDC, STREAM_TITLE_LEFT, m_nStreamTitleTop, m_nStreamTitleWidth, 
				m_nStreamTitleHeight, hMemDC, m_nStreamTitlePos, 0, SRCCOPY);
		}

		DeleteDC(hMemDC);
		DeleteObject(hBmp);
	}
	SelectObject(hDC, hBk);
}

void CMainWnd::DrawVolumeSlider(HDC hDC)
{
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_VOLSLIDER), 0, hDC, 
			m_nVolumeSliderLeft, m_nVolumeSliderTop, ILD_TRANSPARENT);

	DWORD dwVolume;
	waveOutGetVolume(NULL, &dwVolume);
	dwVolume = max(LOWORD(dwVolume), HIWORD(dwVolume));
	dwVolume = dwVolume * 100 / 0xFFFF;

	double dLeft = (double)(m_nVolumeSliderWidth - m_nVolumeTrackWidth) / 100 * dwVolume;
	ImageList_Draw(m_pSkin->GetImageList(IMAGELIST_VOLTRACK), 0, hDC, 
			m_nVolumeSliderLeft + (int)dLeft, m_nVolumeSliderTop, ILD_TRANSPARENT);

	waveOutGetVolume(NULL, &m_dwCurrentVolume);
}

void CMainWnd::UpdateDisp()
{
	RECT rc = {DISP_LEFT, DISP_TOP, 
		DISP_LEFT + m_nDispWidth, DISP_TOP + m_nDispHeight};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdateTrack()
{
	RECT rc = {TRACK_LEFT, TRACK_TOP, 
		TRACK_LEFT + TRACK_WIDTH, TRACK_TOP + TRACK_HEIGHT};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdateTime()
{
	RECT rc;
	BOOL fUpdate = FALSE;
	int nSec = GetDrawTime();
	if (m_nLastDrawTime != nSec) {
		SetRect(&rc, TIME_LEFT, TIME_TOP, 
			TIME_LEFT + TIME_WIDTH, TIME_TOP + TIME_HEIGHT);
		InvalidateRect(m_hWnd, &rc, TRUE);
		fUpdate = TRUE;
	}

	FILEINFO* pInfo = GetInfo(GetCurrentFile());
	if (!pInfo || !IsURL(pInfo->szPath)) {
		if (m_nLastDrawSeek != MAP_GetCurrent(m_hMap)) {
			SetRect(&rc, SEEK_LEFT, m_nSeekTop, 
				SEEK_LEFT + m_nSeekWidth, m_nSeekTop + SEEK_HEIGHT);
			InvalidateRect(m_hWnd, &rc, TRUE);
			fUpdate = TRUE;
		}
	}

	if (fUpdate)
		UpdateWindow(m_hWnd);
}

void CMainWnd::UpdateInfo()
{
	RECT rc = {INFO_LEFT, INFO_TOP, 
		INFO_LEFT + INFO_WIDTH, INFO_TOP + INFO_HEIGHT};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdatePlayOption()
{
	RECT rc = {PLAYOPT_LEFT, PLAYOPT_TOP, 
		PLAYOPT_LEFT + PLAYOPT_WIDTH, PLAYOPT_TOP + PLAYOPT_HEIGHT};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdateStatus()
{
	RECT rc = {STATUS_LEFT, m_nStatusTop, 
		STATUS_LEFT + STATUS_WIDTH, m_nStatusTop + m_nStatusHeight};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdateTitle()
{
	RECT rc = {TITLE_LEFT, m_nTitleTop, 
		TITLE_LEFT + m_nTitleWidth, m_nTitleTop + m_nTitleHeight};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdatePeek()
{
	RECT rc = {PEEK_LEFT, PEEK_TOP, 
		PEEK_LEFT + PEEK_WIDTH, PEEK_TOP + PEEK_HEIGHT};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdateStreamTitle()
{
	FILEINFO* pInfo = GetInfo(GetCurrentFile());
	if (!pInfo || !IsURL(pInfo->szPath))
		return;

	RECT rc = {STREAM_TITLE_LEFT, m_nStreamTitleTop, 
		STREAM_TITLE_LEFT + m_nStreamTitleWidth, m_nStreamTitleTop + m_nStreamTitleHeight};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

void CMainWnd::UpdateVolumeSlider()
{
	RECT rc = {m_nVolumeSliderLeft, m_nVolumeSliderTop, 
		m_nVolumeSliderLeft + m_nVolumeSliderWidth, m_nVolumeSliderTop + m_nVolumeSliderHeight};

	InvalidateRect(m_hWnd, &rc, TRUE);
	UpdateWindow(m_hWnd);
}

// 保存関係
void CMainWnd::Save()
{
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	m_pOptions->m_ptInitWnd.x = rc.left;
	m_pOptions->m_ptInitWnd.y = rc.top;
	m_pOptions->Save(m_hMap);
}

void CMainWnd::Load()
{
	m_pOptions->Load(m_hMap);
}

void CMainWnd::GetDefPlayListPath(LPTSTR pszFile)
{
	GetModuleFileName(GetInst(), pszFile, MAX_PATH);
	LPTSTR p = _tcsrchr(pszFile, _T('.'));
	if (p) {
		*(p + 1) = NULL;
		_tcscat(pszFile, M3U8_FILE_EXT);
	}
}

BOOL CMainWnd::IsExisting(LPTSTR pszFile)
{
	if (m_pOptions->m_fAddExisting)
		return FALSE;

	TCHAR szFile1[MAX_PATH];
	_tcscpy(szFile1, pszFile);
	_tcslwr(szFile1);
	for (int i = 0; i < m_pListFile->GetCount(); i++) {
		TCHAR szFile2[MAX_PATH];
		FILEINFO* p = (FILEINFO*)m_pListFile->GetAt(i);
		if (p) {
			_tcscpy(szFile2, p->szPath);
			_tcslwr(szFile2);
			if (_tcscmp(szFile1, szFile2) == 0)
				return TRUE;
		}
	}
	return FALSE;
}

void CMainWnd::RegisterTrayIcon(BOOL fRegist)
{
	NOTIFYICONDATA nid;
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = m_hWnd;
	nid.uID = 1;

	if (fRegist) {
		if (!m_hTrayIcon) {
			m_hTrayIcon = (HICON)LoadImage(GetInst(), (LPCTSTR)IDI_MAIN, IMAGE_ICON, 16, 16, 0);
			nid.hIcon = m_hTrayIcon;
			nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			nid.uCallbackMessage = WM_TRAYNOTIFY;
			_tcscpy(nid.szTip, MAINWND_TITLE);
			Shell_NotifyIcon(NIM_ADD, &nid);
		}
	}
	else {
		if (m_hTrayIcon) {
			Shell_NotifyIcon(NIM_DELETE, &nid);
			DestroyIcon(m_hTrayIcon);
			m_hTrayIcon = NULL;
		}
	}
}