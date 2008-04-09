#ifndef __PLAYLISTDLG_H__
#define __PLAYLISTDLG_H__

class CMainWnd;
class CPlayListDlg
{
public:
	CPlayListDlg();
	~CPlayListDlg();
	void Show(CMainWnd* pParent);
	void Close();
	void UpdatePlayList();
	void SetCurrent(int nIndex);
	virtual BOOL IsDialogMessage(LPMSG pMsg) {return FALSE;}

protected:
	static BOOL CALLBACK PlayListDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnInitDialog(HWND hwndDlg);
	void OnOK();
	void OnMove(int x, int y);
	void OnAdd();
	void OnUp(BOOL fVisible = TRUE);
	void OnDown(BOOL fVisible = TRUE);
	void OnDelete();
	void OnSave();
	void OnCtrlKey();
	void OnSort(UINT uID);
	void OnListKeyDown(NMLVKEYDOWN* pnm);
	void OnListDblClk(NMHDR* pnm);
	void AddList(HWND hwndLV, int nIndex);

	void OnListBeginDrag(NMLISTVIEW* pnmlv);
	void OnContextMenu(NMRGINFO* pnmri);
	void OnMouseMove(int x, int y);
	void OnLButtonUp();

protected:
	CMainWnd*	m_pParent;
	HIMAGELIST	m_hImageList;
	HWND		m_hWnd;

	BOOL		m_fListDrag;
	int			m_nDragItem;

	HWND		m_hwndMB;
	BOOL		m_fCtrlKey;
};

#endif // __PLAYLISTDLG_H__