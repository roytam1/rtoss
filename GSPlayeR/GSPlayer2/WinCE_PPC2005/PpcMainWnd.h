#ifndef __PPCMAINWND_H__
#define __PPCMAINWND_H__

#include "MainWnd.h"

#define		PPC_MAINWND_WIDTH		(GetSystemMetrics(SM_CXSCREEN))
#define		PPC_MAINWND_HEIGHT		(GetSystemMetrics(SM_CYSCREEN) - MENU_HEIGHT * 2)
#define		PPC_DISP_WIDTH			(PPC_MAINWND_WIDTH - 9)
#define		PPC_DISP_HEIGHT			(PPC_MAINWND_HEIGHT - 32)
#define		PPC_BTN_TOP				(PPC_MAINWND_HEIGHT - 23)
#define		PPC_BTN_MAX				7	// 最大値
#define		PPC_VOLSLIDER_TOP		PPC_BTN_TOP

#define		PPC_STATUS_TOP			(DISP_TOP + 20 + 3)
#define		PPC_STATUS_HEIGHT		15
#define		PPC_TITLE_TOP			(DISP_TOP + 20 + 3)
#define		PPC_TITLE_WIDTH			(PPC_DISP_WIDTH - TITLE_LEFT - 5)
#define		PPC_TITLE_HEIGHT		15
#define		PPC_SEEK_WIDTH			(PPC_DISP_WIDTH - 2)
#define		PPC_SEEK_TOP			(PPC_TITLE_TOP + PPC_TITLE_HEIGHT + 5)
#define		PPC_STREAM_TITLE_TOP		(PPC_TITLE_TOP + PPC_TITLE_HEIGHT + 1)
#define		PPC_STREAM_TITLE_WIDTH		PPC_TITLE_WIDTH
#define		PPC_STREAM_TITLE_HEIGHT		15

#define		PPC_PLAYLIST_LEFT		(DISP_LEFT + 2)
#define		PPC_PLAYLIST_TOP		68
#define		PPC_PLAYLIST_WIDTH		(PPC_DISP_WIDTH - 4)
#define		PPC_PLAYLIST_HEIGHT		(PPC_MAINWND_HEIGHT - 97)
#define		ID_TIMER_DISPCHECK			10
#define		TIMER_DISPCHECK_INTERVAL	2500
#define		ID_TIMER_DISPSTATE			11
#define		TIMER_DISPSTATE_INTERVAL	500
#define		ID_TIMER_HOLD				12
#define		TIMER_HOLD_INTERVAL			1500

#define		IDM_SUBMENU_SEL0	2
#define		IDM_SUBMENU_SEL1	3

class CPpcMainWnd : public CMainWnd
{
public:
	CPpcMainWnd();
	virtual ~CPpcMainWnd();
	virtual HACCEL GetAccelHandle();
	virtual BOOL Create(LPTSTR pszCmdLine = NULL, HWND hwndParent = NULL, DWORD dwStyle = 0);
	virtual BOOL CanTransAccel(MSG* pMsg);
	virtual BOOL OpenMediaFile(BOOL fAdd, HWND hwndParent = NULL);
	virtual void OpenFirstFile();
	
protected:
	virtual void OnCreate(HWND hwnd);
	virtual void OnClose();
	virtual void OnActivate(int nActive, BOOL fMinimized, HWND hwndPrevious);
	virtual void OnHotKey(int nId, UINT fuModifiers, UINT uVirtKey);
	virtual void OnTimer(UINT uID);
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnCopyData(COPYDATASTRUCT* pcds);

	virtual void OnLButtonDown(int x, int y);
	virtual void OnMouseMove(int fKey, int x, int y);
	virtual void OnLButtonUp(int x, int y);
	virtual void OnInitMenuPopup(HMENU hMenu, UINT uPos, BOOL fSystemMenu);

	virtual void OnFileFind();
	virtual void OnFileList();
	virtual void OnFileSort(UINT uID);
	virtual void OnToolEffect();
	virtual void OnToolOption();
	virtual void OnShowMenu(POINT* ppt = NULL);
	virtual void OnToolHold();
	virtual void OnToolHoldDisp();
	virtual void OnPlayPlayStop();
	virtual void OnToolKeyCtrl();

	virtual void OnFileUp();
	virtual void OnFileDown();
	virtual void OnFileDelete();
	virtual void OnFileDeleteAll();
	virtual void OnPlaySelected();
	virtual void OnListViewReturn(NMHDR* pnmh);
	virtual void OnListViewKeyDown(NMHDR* pnmh);
	virtual void OnListViewDblClk(NMHDR* pnmh);
	virtual void OnListViewBeginDrag(NMHDR* pnmh);
	virtual void OnListViewConextMenu(NMHDR* pnmh);

	virtual void OnMsgStatus(MAP_STATUS status, BOOL fError);
	
	// ウインドウプロシージャ
	static LRESULT CALLBACK PpcMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void RegisterClass();
	virtual COptions* GetOptionsClass();
	virtual CSkin* GetSkinClass();
	virtual void CreateMenuBar();
	virtual void CreateListView();
	virtual void DrawTitle(HDC hDC);
	
	virtual BOOL InitImages();
	virtual void InitSize();

	virtual void AddDir(LPTSTR pszDir);
	virtual BOOL AddFile(LPTSTR pszFile, LPTSTR pszTitle = NULL);
	virtual BOOL InsertFile(LPTSTR pszFile, int nIndex);
	virtual BOOL UpFile(int nIndex);
	virtual BOOL DownFile(int nIndex);
	virtual BOOL DeleteFile(int nIndex);
	virtual void DeleteAllFile();
	virtual void SortFile(int nSort);
	virtual BOOL Open(int nIndex);

	virtual void RegisterHotKeys();
	virtual void UnregisterHotKeys();
	virtual void RegisterTrayIcon(BOOL fRegist) {}

protected:
	SHACTIVATEINFO	m_sai;

	BOOL	m_fMenuLoop;
	BOOL	m_fActive;
	BOOL	m_fHotKey;
	DWORD	m_dwLastOperation;
	BOOL	m_fDisplay;

	HWND	m_hwndCB;
	HWND	m_hwndLV;

	BOOL	m_fListDrag;
	int		m_nListDragItem;
	BOOL	m_fHold;
	BOOL	m_fDrawHold;
	BOOL	m_fNoListRedraw;
};

#endif // __PPCMAINWND_H__
