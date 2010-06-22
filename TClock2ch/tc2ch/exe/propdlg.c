/*-------------------------------------------
  propdlg.c
  show "properties for TClock"
---------------------------------------------*/

//#define NONAMELESSUNION
#include "tclock.h"

#define MAX_PAGE  14

INT_PTR CALLBACK PropertyDialog(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK SubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void SetMyDialgPos(HWND hwnd);

// dialog procedures of each page
INT_PTR CALLBACK PageColorProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageFormatProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageAlarmProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageMouseProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageTooltipProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageSkinProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageGraphProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageTaskbarProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageDesktopProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageSNTPProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageAutoExecProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageMiscProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageAboutProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PageAnalogClockProc(HWND, UINT, WPARAM, LPARAM);

void SetPropSheetPos(HWND hwnd);

// TV_INSERTSTRUCT‚Å‚Í‚È‚º‚©ƒGƒ‰[‚ª‚Å‚½
typedef struct{
	HTREEITEM hParent;
	HTREEITEM hInsertAfter;
	TV_ITEM item;
} _TV_INSERTSTRUCT;


static HHOOK hookMsgfilter = 0;

static WNDPROC oldWndProc; // to save default window procedure
static int startpage = 0;  // page to open first

BOOL g_bApplyClock = FALSE;
BOOL g_bApplyTaskbar = FALSE;
BOOL g_bApplyLangDLL = FALSE;

// menu.c
extern HMENU g_hMenu;

/*-------------------------------------------
  show property sheet
---------------------------------------------*/
void MyPropertyDialog(void)
{
	g_bApplyClock = FALSE;
	g_bApplyTaskbar = FALSE;
	g_bApplyLangDLL = FALSE;

	if(g_hwndPropDlg && IsWindow(g_hwndPropDlg))
		;
	else
		g_hwndPropDlg = CreateDialog(GetLangModule(), MAKEINTRESOURCE(IDD_PROPERTY), g_hwndMain, PropertyDialog);
	ShowWindow(g_hwndPropDlg, SW_SHOW);
	UpdateWindow(g_hwndPropDlg);

	SetForegroundWindow98(g_hwndPropDlg);
}

static VOID SetPageDlgPos(HWND hParent, HWND hDlg)
{
	LONG DlgBase;
	WORD DlgBaseH;
	HWND hTree;
	RECT rect;
	POINT pos;

	hTree = GetDlgItem(hParent, IDC_TREE);
	GetWindowRect(hTree, &rect);
	pos.x = rect.right;
	pos.y = rect.top;
	ScreenToClient(hParent, &pos);

	DlgBase = GetDialogBaseUnits();
	DlgBaseH = LOWORD(DlgBase);

	pos.x = pos.x + (DlgBaseH / 4);
	pos.y = pos.y;
	SetWindowPos(hDlg, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE);
}

static VOID CreatePageDialog(HWND hParent, HWND hDlg[], BOOL bDlgFlg[], int index, WORD wID, DLGPROC dlgprc)
{
	HINSTANCE hInst;

	if (bDlgFlg[index]) {
		return;
	}
	hInst   = GetLangModule();
	hDlg[index] = CreateDialog(hInst, MAKEINTRESOURCE(wID), hParent, dlgprc);
	SetPageDlgPos(hParent, hDlg[index]);
	//SetWindowPos(hDlg[index], NULL, 130, 5, 0, 0, SWP_NOSIZE);
	bDlgFlg[index] = TRUE;
}

/*-------------------------------------------
  Property dialog
---------------------------------------------*/
INT_PTR CALLBACK PropertyDialog(HWND hDwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hTree;
	static HWND hDlg[MAX_PAGE];
	static BOOL bDlgFlg[MAX_PAGE];
	static HWND *hNowDlg;
	_TV_INSERTSTRUCT tv;
	HTREEITEM hParent[5], hChild[MAX_PAGE];
	NM_TREEVIEW *pNMTV;
//	HINSTANCE hInst;
	static int nowDlg;
	int i;

	switch(message){
		case WM_INITDIALOG:
			InitCommonControls();

			hTree = GetDlgItem(hDwnd, IDC_TREE);
			memset(&tv, 0, sizeof(_TV_INSERTSTRUCT));

			tv.hInsertAfter = TVI_LAST;
			tv.hParent = TVI_ROOT;
			tv.item.mask = TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
			tv.item.state = TVIS_EXPANDED;
			tv.item.stateMask = TVIS_EXPANDED;
			tv.item.pszText = MyString(IDS_CLOCK);
			tv.item.lParam = 0;
			hParent[0] = TreeView_InsertItem(hTree, &tv);
			tv.item.lParam = 1;
			tv.item.pszText = MyString(IDS_ALARM);
			hParent[1] = TreeView_InsertItem(hTree, &tv);
			tv.item.lParam = 2;
			tv.item.pszText = MyString(IDS_TOOLTIP);
			hParent[2] = TreeView_InsertItem(hTree, &tv);
			tv.item.lParam = 3;
			tv.item.pszText = MyString(IDS_TASKBAR);
			hParent[3] = TreeView_InsertItem(hTree, &tv);
			tv.item.lParam = 4;
			tv.item.pszText = MyString(IDS_MISC);
			hParent[4] = TreeView_InsertItem(hTree, &tv);

			tv.hParent = hParent[0];
			tv.item.mask = TVIF_TEXT | TVIF_PARAM;
			tv.item.pszText = MyString(IDS_PROP_COLOR);
			tv.item.lParam = 10;
			hChild[0] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_FORMAT);
			tv.item.lParam = 11;
			hChild[1] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_SYNC);
			tv.item.lParam = 12;
			hChild[2] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_MOUSE);
			tv.item.lParam = 13;
			hChild[3] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_GRAPH);
			tv.item.lParam = 14;
			hChild[4] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_ANALOG);
			tv.item.lParam = 15;
			hChild[5] = TreeView_InsertItem(hTree, &tv);

			tv.hParent = hParent[1];
			tv.item.pszText = MyString(IDS_PROP_ALARM);
			tv.item.lParam = 16;
			hChild[6] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_AUTO);
			tv.item.lParam = 17;
			hChild[7] = TreeView_InsertItem(hTree, &tv);

			tv.hParent = hParent[2];
			tv.item.pszText = MyString(IDS_PROP_TOOLTIP);
			tv.item.lParam = 18;
			hChild[8] = TreeView_InsertItem(hTree, &tv);

			tv.hParent = hParent[3];
			tv.item.pszText = MyString(IDS_PROP_TASKBAR);
			tv.item.lParam = 19;
			hChild[9] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_DESKTOP);
			tv.item.lParam = 20;
			hChild[10] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_SKIN);
			tv.item.lParam = 21;
			hChild[11] = TreeView_InsertItem(hTree, &tv);

			tv.hParent = hParent[4];
			tv.item.pszText = MyString(IDS_PROP_MISC);
			tv.item.lParam = 22;
			hChild[12] = TreeView_InsertItem(hTree, &tv);
			tv.item.pszText = MyString(IDS_PROP_ABOUT);
			tv.item.lParam = 23;
			hChild[13] = TreeView_InsertItem(hTree, &tv);

#if 1
			CreatePageDialog(hDwnd, hDlg, bDlgFlg, 0, IDD_PAGECOLOR, PageColorProc);
			nowDlg = startpage;
			hNowDlg = &hDlg[nowDlg];
			ShowWindow(*hNowDlg, SW_SHOW);
			UpdateWindow(*hNowDlg);
			TreeView_SelectItem(hTree, hChild[nowDlg]);
#else
			hInst = GetLangModule();
			hDlg[0]  = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PAGECOLOR), hDwnd, PageColorProc);
			SetPageDlgPos(hDwnd, hDlg[0]);
			//SetWindowPos(hDlg[0], NULL, 130, 5, 0, 0, SWP_NOSIZE);
			bDlgFlg[0] = TRUE;
			nowDlg = startpage;
			hNowDlg = &hDlg[nowDlg];
			ShowWindow(*hNowDlg, SW_SHOW);
			UpdateWindow(*hNowDlg);
			TreeView_SelectItem(hTree, hChild[nowDlg]);
#endif
			break;

		case WM_SHOWWINDOW: // adjust the window position
			SetMyDialgPos(hDwnd);
			break;

		case WM_NOTIFY:
			pNMTV = (NM_TREEVIEW *)lParam;
			switch (pNMTV->hdr.code){
				case TVN_SELCHANGED:
					ShowWindow(*hNowDlg, SW_HIDE);
					UpdateWindow(*hNowDlg);
					nowDlg = (int)pNMTV->itemNew.lParam;
					switch (nowDlg)
					{
						case 0:
							nowDlg = 0;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGECOLOR, PageColorProc);
							break;
						case 1:
							nowDlg = 6;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEALARM, PageAlarmProc);
							break;
						case 2:
							nowDlg = 8;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGETOOLTIP, PageTooltipProc);
							break;
						case 3:
							nowDlg = 9;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGETASKBAR, PageTaskbarProc);
							break;
						case 4:
							nowDlg = 12;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEMISC, PageMiscProc);
							break;
						case 10:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGECOLOR, PageColorProc);
							break;
						case 11:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEFORMAT, PageFormatProc);
							break;
						case 12:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGESNTP, PageSNTPProc);
							break;
						case 13:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEMOUSE, PageMouseProc);
							break;
						case 14:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEGRAPH, PageGraphProc);
							break;
						case 15:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEANALOG, PageAnalogClockProc);
							break;
						case 16:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEALARM, PageAlarmProc);
							break;
						case 17:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEAUTOEXEC, PageAutoExecProc);
							break;
						case 18:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGETOOLTIP, PageTooltipProc);
							break;
						case 19:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGETASKBAR, PageTaskbarProc);
							break;
						case 20:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEDESKTOP, PageDesktopProc);
							break;
						case 21:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGESKIN, PageSkinProc);
							break;
						case 22:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEMISC, PageMiscProc);
							break;
						case 23:
							nowDlg -= 10;
							CreatePageDialog(hDwnd, hDlg, bDlgFlg, nowDlg, IDD_PAGEABOUT, PageAboutProc);
							break;
						default:
							nowDlg -= 10;
							break;
					}
					hNowDlg = &hDlg[nowDlg];
					ShowWindow(*hNowDlg, SW_SHOW);
					UpdateWindow(*hNowDlg);
					break;
			}
			break;

		case WM_COMMAND:
			// apply settings
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == ID_APPLY)
			{
				NMHDR lp;
				lp.code = PSN_APPLY;
				SendMessage(hDlg[nowDlg], WM_NOTIFY, 0, (LPARAM)&lp);
				if(g_bApplyClock)
				{
					PostMessage(g_hwndClock, CLOCKM_REFRESHCLOCK, 0, 0);
					g_bApplyClock = FALSE;
				}
				if(g_bApplyTaskbar)
				{
					PostMessage(g_hwndClock, CLOCKM_REFRESHTASKBAR, 0, 0);
					g_bApplyTaskbar = FALSE;
				}
			}
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				startpage = nowDlg;

				// reload language dll
				if(g_bApplyLangDLL)
				{
					char fname[MAX_PATH];
					HINSTANCE hInst;
					hInst = LoadLanguageDLL(fname);
					if(hInst != NULL)
					{
						if(g_hDlgTimer && IsWindow(g_hDlgTimer))
							PostMessage(g_hDlgTimer, WM_CLOSE, 0, 0);
						if(g_hMenu) DestroyMenu(g_hMenu);
						g_hMenu = NULL;
						if(g_hInstResource) FreeLibrary(g_hInstResource);
						g_hInstResource = hInst;
						strcpy(g_langdllname, fname);
					}
				}
				for (i = 0; i < MAX_PAGE; i++) {
					bDlgFlg[i] = FALSE;
				}
				DestroyWindow(hDwnd);
				g_hwndPropDlg = NULL;
			}
			// HELP
			if(LOWORD(wParam) == ID_HELP){
				NMHDR lp;
				lp.code = PSN_HELP;
				SendMessage(hDlg[nowDlg], WM_NOTIFY, 0, (LPARAM)&lp);
			}
			break;

		// close by "x" button
		case WM_SYSCOMMAND:
			if((wParam & 0xfff0) == SC_CLOSE)
				PostMessage(hDwnd, WM_COMMAND, IDCANCEL, 0);
			break;
	}

	return 0;
}



/*------------------------------------------------
   adjust the window position
--------------------------------------------------*/
void SetMyDialgPos(HWND hwnd)
{
	HWND hwndTray;
	RECT rc, rcTray;
	int wscreen, hscreen, wProp, hProp;
	int x, y;

	GetWindowRect(hwnd, &rc);
	wProp = rc.right - rc.left;
	hProp = rc.bottom - rc.top;

	wscreen = GetSystemMetrics(SM_CXSCREEN);
	hscreen = GetSystemMetrics(SM_CYSCREEN);

	hwndTray = FindWindow("Shell_TrayWnd", NULL);
	if(hwndTray == NULL) return;
	GetWindowRect(hwndTray, &rcTray);
	if(rcTray.right - rcTray.left >
		rcTray.bottom - rcTray.top)
	{
		x = wscreen - wProp - 32;
		if(rcTray.top < hscreen / 2)
			y = rcTray.bottom + 2;
		else
			y = rcTray.top - hProp - 32;
		if(y < 0) y = 0;
	}
	else
	{
		y = hscreen - hProp - 2;
		if(rcTray.left < wscreen / 2)
			x = rcTray.right + 2;
		else
			x = rcTray.left - wProp - 2;
		if(x < 0) x = 0;
	}

	MoveWindow(hwnd, x, y, wProp, hProp, FALSE);
}

/*------------------------------------------------
   select file
--------------------------------------------------*/
BOOL SelectMyFile(HWND hDlg, const char *filter, DWORD nFilterIndex,
	const char *deffile, char *retfile)
{
	OPENFILENAME ofn;
	char fname[MAX_PATH], ftitle[MAX_PATH], initdir[MAX_PATH];
	BOOL r;

	memset(&ofn, '\0', sizeof(OPENFILENAME));

	strcpy(initdir, g_mydir);
	if(deffile[0])
	{
		WIN32_FIND_DATA fd;
		HANDLE hfind;
		hfind = FindFirstFile(deffile, &fd);
		if(hfind != INVALID_HANDLE_VALUE)
		{
			FindClose(hfind);
			strcpy(initdir, deffile);
			del_title(initdir);
		}
	}

	fname[0] = 0;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hDlg;
	ofn.hInstance = g_hInst;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = nFilterIndex;
	ofn.lpstrFile= fname;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = ftitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = initdir;
	ofn.Flags = OFN_HIDEREADONLY|OFN_EXPLORER|OFN_FILEMUSTEXIST;

	r = GetOpenFileName(&ofn);
	if(!r) return r;

	strcpy(retfile, ofn.lpstrFile);

	return r;
}

