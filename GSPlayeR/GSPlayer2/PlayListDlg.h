#ifndef __PLAYLISTDLG_H__
#define __PLAYLISTDLG_H__

class CMainWnd;
class CPlayListDlg
{
public:
	CPlayListDlg();
	virtual ~CPlayListDlg();
	virtual void Show(CMainWnd* pParent);
	virtual void Close();
	virtual void UpdatePlayList();
	virtual void SetCurrent(int nIndex);
	virtual BOOL IsDialogMessage(LPMSG pMsg);

protected:
	static BOOL CALLBACK PlayListDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnInitDialog(HWND hwndDlg);
	void OnMove(int x, int y);
	void OnAdd();
	void OnUp(BOOL fVisible = TRUE);
	void OnDown(BOOL fVisible = TRUE);
	void OnDelete();
	void OnSave();
	void OnOK();
	void OnSort(UINT uID);
	void OnListKeyDown(NMLVKEYDOWN* pnm);
	void OnListDblClk(NMHDR* pnm);
	void OnListClick(NMHDR* pnm);
	void OnListRClick(NMHDR* pnm);
	void AddList(HWND hwndLV, int nIndex);
#ifndef _WIN32_WCE
	void OnDropFiles(HDROP hDrop);
#endif

	void OnListBeginDrag(NMLISTVIEW* pnmlv);
	void OnMouseMove(int x, int y);
	void OnLButtonUp();
	void ShowMenu(int x, int y);

protected:
	CMainWnd*	m_pParent;
	HIMAGELIST	m_hImageList;
	HWND		m_hWnd;

	BOOL		m_fListDrag;
	int			m_nDragItem;
};

#endif // __PLAYLISTDLG_H__