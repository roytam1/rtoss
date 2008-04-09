#ifndef __MAINWND_H__
#define __MAINWND_H__

#include "multibuff.h"
#include "options.h"
#include "skin.h"
#include "checksys.h"

#ifdef _WIN32_WCE_PPC
#include "PlayListDlgPPC.h"
#else
#include "PlayListDlg.h"
#endif

#define HTTP_PREFIX				_T("http://")
#define MAINWND_CLASS_NAME		_T("GSPLAYER")
#define MAINWND_TITLE			_T("GSPlayer")
#ifdef _WIN32_WCE
#define STREAMING_USER_AGENT	_T("GSPlayer (WindowsCE)")
#else
#define STREAMING_USER_AGENT	_T("GSPlayer (Windows)")
#endif

// 座標関係
#define		MAINWND_WIDTH		238
#define		MAINWND_HEIGHT		80
#define		DISP_LEFT			5
#define		DISP_TOP			5
#define		DISP_WIDTH			228
#define		DISP_HEIGHT			48
#define		DISP_COLOR			RGB(0, 0, 64)
#define		BTN_LEFT			5
#define		BTN_TOP				57
#define		BTN_WIDTH			25
#define		BTN_HEIGHT			20

#define		BTN_OPEN			0
#define		BTN_PLAY			1
#define		BTN_STOP			2
#define		BTN_PREV			3
#define		BTN_REW				4
#define		BTN_FF				5
#define		BTN_NEXT			6
#define		BTN_MENU			7
#define		BTN_MAX				8	// 最大値

#define		TRACK_LEFT			(DISP_LEFT + 5)
#define		TRACK_TOP			(DISP_TOP + 5)
#define		TRACK_WIDTH			20
#define		TRACK_HEIGHT		15
#define		TIME_LEFT			(DISP_LEFT + 30)
#define		TIME_TOP			(DISP_TOP + 5)
#define		TIME_WIDTH			45
#define		TIME_HEIGHT			15
#define		PLAYOPT_LEFT		TIME_LEFT + 48
#define		PLAYOPT_TOP			TRACK_TOP
#define		PLAYOPT_WIDTH		10
#define		PLAYOPT_HEIGHT		15
#define		INFO_LEFT			(DISP_LEFT + 96)
#define		INFO_TOP			(DISP_TOP + 12)
#define		INFO_WIDTH			120
#define		INFO_HEIGHT			9
#define		INFO_COLOR			RGB(166, 202, 244)
#define		STATUS_LEFT			(DISP_TOP + 5)
#define		STATUS_TOP			(DISP_TOP + 20 + 5)
#define		STATUS_WIDTH		9
#define		STATUS_HEIGHT		14
#define		TITLE_LEFT			(DISP_LEFT + STATUS_WIDTH + 10)
#define		TITLE_TOP			(DISP_TOP + 20)
#define		TITLE_WIDTH			(DISP_WIDTH - TITLE_LEFT - 5)
#define		TITLE_HEIGHT		STATUS_HEIGHT
#define		TITLE_COLOR			RGB(166, 202, 244)
#define		SEEK_LEFT			DISP_LEFT + 1
#define		SEEK_TOP			(TITLE_TOP + TITLE_HEIGHT + 5)
#define		SEEK_WIDTH			(DISP_WIDTH - 2)
#define		SEEK_HEIGHT			4
#define		SEEK_COLOR			TITLE_COLOR
#define		PEEK_LEFT			INFO_LEFT
#define		PEEK_TOP			TRACK_TOP
#define		PEEK_WIDTH			INFO_WIDTH
#define		PEEK_HEIGHT			5
#define		PEEK_COLOR_L		RGB(0, 128, 0)
#define		PEEK_COLOR_R		RGB(128, 0, 0)
#define		MASK_COLOR			RGB(255, 0, 255)

#define		STREAM_TITLE_LEFT		TITLE_LEFT
#define		STREAM_TITLE_TOP		(TITLE_TOP + TITLE_HEIGHT)
#define		STREAM_TITLE_WIDTH		TITLE_WIDTH
#define		STREAM_TITLE_HEIGHT		10
#define		STREAM_TITLE_COLOR		TITLE_COLOR

#define		VOLSLIDER_LEFT		(BTN_LEFT + BTN_WIDTH * 7 + 2)
#define		VOLSLIDER_TOP		BTN_TOP
#define		VOLSLIDER_WIDTH		51
#define		VOLSLIDER_HEIGHT	BTN_HEIGHT
#define		VOLTRACK_WIDTH		10

#define		WINDOW_FIT_MARGIN			8
#define		SKIP_INTERVAL				10000
#define		SCROLL_TITLE_INTERVAL		8
#define		SCROLL_TITLE_MARGIN			25
#define		SCROLL_TITLE_INITWAIT		2 // (* interval sec)

#define		ID_TIMER_TIME				1
#define		TIMER_TIME_INTERVAL			333
#define		ID_TIMER_SUSPEND			2
#define		TIMER_SUSPEND_INTERVAL		10000
#define		ID_TIMER_VOLUME				3
#define		TIMER_VOLUME_INTERVAL		3000
#define		ID_TIMER_VOLSLIDER			4
#define		TIMER_VOLSLIDER_INTERVAL	2000
#define		ID_TIMER_SCROLLTITLE			5
#define		TIMER_SCROLLTITLE_INTERVAL		500
#define		ID_TIMER_SCROLLSTREAM			6
#define		TIMER_SCROLLSTREAM_INTERVAL		500
#define		ID_TIMER_STREAMINGWAIT			7
#define		TIMER_STREAMINGWAIT_INTERVAL	250
#define		ID_TIMER_SLEEPTIMER				8
#define		TIMER_SLEEPTIMER_INTERVAL		5000

#define		M3U_FILE_EXT	_T("m3u")
#define		PLS_FILE_EXT	_T("pls")
#define		M3U8_FILE_EXT	_T("m3u8")
#define		WM_TRAYNOTIFY	(WM_USER + 1000)
#define		WM_WAKEUP		(WM_USER + 1001)

#define		IDM_SUBMENU_MAIN		0
#define		IDM_SUBMENU_TRAY		1
#define		IDM_SUBMENU_PLAYLIST1	2
#define		IDM_SUBMENU_PLAYLIST2	3

// 再生関係
#define OPEN_NONE -1
typedef struct _tagFileInfo
{
	TCHAR szPath[MAX_PATH];
	TCHAR szDisplayName[MAX_PATH];
	MAP_INFORMATION info;
	ID3TAGV1 tag;

	BOOL	fShuffle;
	DWORD	dwUser;
	DWORD	dwQueue;

	_tagFileInfo() 
	{
		szPath[0] = NULL;
		szDisplayName[0] = NULL;
		memset(&info, 0, sizeof(info));
		memset(&tag, 0, sizeof(tag));
		fShuffle = FALSE;
		dwUser = 0;
		dwQueue = 0;
	}
}FILEINFO;

class CTempDC
{
public:
	CTempDC(HWND hwnd) {m_hWnd = hwnd; m_hDC = GetDC(m_hWnd);}
	~CTempDC() {ReleaseDC(m_hWnd, m_hDC);}
	operator HDC() {return m_hDC;}

protected:
	HDC m_hDC;
	HWND m_hWnd;
};

class CWaitCursor
{
public:
	CWaitCursor()
	{
		m_hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	}
	~CWaitCursor()
	{
		Restore();
	}
	void Restore()
	{
		SetCursor(m_hCursor);
	}
protected:
	HCURSOR	m_hCursor;
};

class CPlayListDlg;
class CMainWnd
{
// ウインドウの表示/基本動作関係
public:
	CMainWnd();
	virtual ~CMainWnd();
	virtual HWND GetHandle() {return m_hWnd;}
	virtual BOOL Create(LPTSTR pszCmdLine = NULL, HWND hwndParent = NULL, 
		DWORD dwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
	virtual HACCEL GetAccelHandle();
	virtual BOOL IsDialogMessage(LPMSG pMsg);
	virtual BOOL OpenMediaFile(BOOL fAdd, HWND hwndParent = NULL);
	virtual void SaveMediaFile(HWND hwndParent = NULL);
	virtual BOOL CanTransAccel(MSG* pMsg);
	static void CheckSystem();
	void MakeOpenFilter(LPTSTR* ppszFilter, BOOL fGsGetFile);

#ifndef _WIN32_WCE
	virtual void AddDropFiles(HDROP hDrop);
#endif
	
protected:
	// メッセージハンドラ
	virtual void OnCreate(HWND hWnd);
	virtual void OnEraseBkgnd(HDC hDC);
	virtual void OnPaint(HDC hDC, PAINTSTRUCT* pps);
	virtual void OnDestroy();
	virtual void OnClose();
	virtual void OnTimer(UINT uID);
	virtual void OnMove(int x, int y);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnLButtonDown(int x, int y);
	virtual void OnMouseMove(int fKey, int x, int y);
	virtual void OnLButtonUp(int x, int y);
	virtual void OnActivate(int nActive, BOOL fMinimized, HWND hwndPrevious);
#ifdef _WIN32_WCE
#define WM_DROPFILES	0x0233
	virtual void OnCopyData(COPYDATASTRUCT* pcds);
#else
	virtual void OnDropFiles(HDROP hDrop);
#endif
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnHotKey(int nId, UINT fuModifiers, UINT uVirtKey);
	virtual void OnTrayNotify(UINT uMsg);
#ifdef REGISTER_WAKE_EVENT
	virtual void OnWakeUp();
#endif

	// コマンド
	virtual void OnFileOpen();
	virtual void OnFileOpenLocation();
	virtual void OnFileAdd();
	virtual void OnFileFind();
	virtual void OnFileList();
	virtual void OnFileSaveList();
	virtual void OnPlayPlayPause();
	virtual void OnPlayStopAfterCurrent();
	virtual void OnPlayStop();
	virtual void OnPlayPrev();
	virtual void OnPlayRew();
	virtual void OnPlayFf();
	virtual void OnPlayNext();
	virtual void OnPlayRepeat();
	virtual void OnPlayRepeatNone();
	virtual void OnPlayRepeatAll();
	virtual void OnPlayRepeatOne();
	virtual void OnPlayShuffle();
	virtual void OnShowMenu(POINT* ppt = NULL);
	virtual void OnToolVolUp();
	virtual void OnToolVolDown();
	virtual void OnToolEffect();
	virtual void OnToolTopMost();
	virtual void OnToolOption();
	virtual void OnToolSleepTimer();

	// コールバック
	virtual void OnMsgStatus(MAP_STATUS status, BOOL fError);
	virtual void OnMsgPeek(int nLeft = 0, int nRight = 0);
	virtual void OnMsgStreamTitle(LPTSTR pszTitle);

	// ウインドウプロシージャ
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void RegisterClass();
	virtual void DrawBackground(HDC hDC);
	virtual void DrawDispEdge(HDC hDC);
	virtual void DrawButton(HDC hDC, int nIndex);
	virtual void DrawTrack(HDC hDC);
	virtual void DrawTime(HDC hDC);
	virtual void DrawSeek(HDC hDC);
	virtual void DrawInfo(HDC hDC);
	virtual void DrawPlayOption(HDC hDC);
	virtual void DrawStatus(HDC hDC);
	virtual void DrawTitle(HDC hDC);
	virtual void DrawPeek(HDC hDC);
	virtual void DrawStreamTitle(HDC hDC);
	virtual void DrawVolume(HDC hDC);
	virtual void DrawVolumeSlider(HDC hDC);
	virtual int GetDrawTime();

	virtual void UpdateDisp();
	virtual void UpdateTrack();
	virtual void UpdateTime();
	virtual void UpdateInfo();
	virtual void UpdatePlayOption();
	virtual void UpdateStatus();
	virtual void UpdateTitle();
	virtual void UpdatePeek();
	virtual void UpdateStreamTitle();
	virtual void UpdateVolumeSlider();

	virtual BOOL InitImages();
	virtual void DeleteImages();

protected:
	HWND		m_hWnd;
	HACCEL		m_hAccel;
	HMENU		m_hMenu;

	CSkin*		m_pSkin;
	HDC			m_hBakDC;
	HBITMAP		m_hBakBmp;
	HDC			m_hMemDC;
	HBITMAP		m_hMemBmp;
	HFONT		m_hFnt;
	HFONT		m_hFntTitle;

	int			m_nWidth;
	int			m_nHeight;
	int			m_nPeek[2];

	int			m_nClientWidth;
	int			m_nClientHeight;
	int			m_nDispWidth;
	int			m_nDispHeight;
	int			m_nBtnTop;
	int			m_nBtnMax;

	int			m_nStatusTop;
	int			m_nStatusHeight;
	int			m_nTitleTop;
	int			m_nTitleWidth;
	int			m_nTitleHeight;
	int			m_nSeekTop;
	int			m_nSeekWidth;
	int			m_nStreamTitleTop;
	int			m_nStreamTitleWidth;
	int			m_nStreamTitleHeight;

	int			m_nVolumeSliderLeft;
	int			m_nVolumeSliderTop;
	int			m_nVolumeSliderWidth;
	int			m_nVolumeSliderHeight;
	int			m_nVolumeTrackWidth;

	BOOL		m_fPressed;
	int			m_nPressed;
	POINT		m_ptPressed;
	BOOL		m_fVolPressed;

	BOOL		m_fDrawVolume;
	int			m_nLastDrawTime;
	int			m_nLastDrawSeek;
	BOOL		m_fShowOpenDlg;

	int			m_nTitleLen;
	int			m_nTitlePos;
	int			m_nTitleWait;
	int			m_nStreamTitleLen;
	int			m_nStreamTitlePos;
	int			m_nStreamTitleWait;
	LPTSTR		m_pszOpenFilter;
	
// レジストリ保存関係
protected:
	virtual void Save();
	virtual void Load();
	virtual COptions* GetOptionsClass();
	virtual CSkin* GetSkinClass();

// 再生/リスト管理関係
public:
	virtual void AddDir(LPTSTR pszDir);
	virtual BOOL AddFile(LPTSTR pszFile, LPTSTR pszTitle = NULL, long nDur = 0);
	virtual BOOL InsertFile(LPTSTR pszFile, int nIndex);
	virtual BOOL UpFile(int nIndex);
	virtual BOOL DownFile(int nIndex);
	virtual BOOL DeleteFile(int nIndex);
	virtual void DeleteAllFile();
#define SORTFILE_BYNAME			0
#define SORTFILE_BYPATH			1
#define SORTFILE_BYTRACKNUM		2
#define SORTFILE_BYARTIST		3
#define SORTFILE_BYTRACK		4
	virtual void SortFile(int nSort);
	virtual int GetFileCount();
	
	virtual int GetNextIndex(BOOL fEos = FALSE, BOOL fError = FALSE);
	virtual int GetPrevIndex();
	virtual void ResetShuffle();

	// 操作
	virtual void OpenFirstFile();
	virtual BOOL Play(int nIndex = OPEN_NONE);
	virtual void Pause();
	virtual void Stop();
	
	// RT Hack - Queue
	virtual BOOL Enqueue(int nIndex = OPEN_NONE);
	virtual BOOL Dequeue(int nIndex = OPEN_NONE);
	virtual BOOL ClearQueue();
	
	// 情報
	virtual FILEINFO* GetInfo(int nIndex);
	virtual BOOL GetTitle(int nIndex, LPTSTR pszTitle, BOOL fForSaving = FALSE);
	virtual int GetCurrentFile() {return m_nCurrent;}
	virtual void GetStreamTitle(LPTSTR pszTitle);

protected:
	virtual BOOL IsValidStream(LPTSTR pszFile);
	virtual BOOL Open(int nIndex);
	virtual void Close();
	virtual void Seek(int nMilisec);
	virtual void SavePlayList(LPTSTR pszFile);
	virtual void LoadPlayList(LPTSTR pszFile);
	virtual BOOL IsPlayList(LPTSTR pszFile);
	virtual void GetDefPlayListPath(LPTSTR pszFile);
	virtual BOOL IsExisting(LPTSTR pszFile);
	virtual BOOL IsURL(LPTSTR pszFile);

	virtual void AddFile2(LPTSTR pszPath, LPTSTR pszFile, LPTSTR pszTitle = NULL, long nDur = 0);
	virtual void SaveM3uPlayList(LPTSTR pszFile);
	virtual void LoadM3uPlayList(LPTSTR pszFile);
	virtual void LoadPlsPlayList(LPTSTR pszFile);
	static int CALLBACK SortCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam);

protected:
	HANDLE			m_hMap;
	int				m_nCurrent;
	CMultiBuff*		m_pListFile;
	CPlayListDlg*	m_pPlayListDlg;
	COptions*		m_pOptions;

	DWORD			m_dwCurrentVolume;
	BOOL			m_fRegEvent;

	// RT Hack
	int				m_nEnqueue;
	BOOL			m_fStopAfterCurr;
// タスクトレイ関係
protected:
	HICON			m_hTrayIcon;
	virtual void RegisterTrayIcon(BOOL fRegist);

	friend CPlayListDlg;
};

BOOL NeedToRepaint(RECT* prc1, RECT* prc2);

#endif // __MAINWND_H__