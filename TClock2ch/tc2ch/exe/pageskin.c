/*-------------------------------------------
  pageskin.c
  "skin" page of properties
  Kazubon 1997-2001
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnStartBtn(HWND hDlg);
static void OnSansho(HWND hDlg, WORD id);
static void OnSanshoMenu(HWND hDlg, WORD id);
static void OnStartMenu(HWND hDlg);
static void OnSkinTray(HWND hDlg);
static void SetColorFromBmp(HWND hDlg, int idCombo, char* fname);
static BOOL SelectIconInDLL(HWND hDlg, char* fname);
static int nFilterIndex = 1;

static COMBOCOLOR combocolor[1] = {
	{ IDC_COLMENU, "StartMenuCol", RGB(128, 128, 128) }
};

__inline void SendPSChanged(HWND hDlg)
{
	g_bApplyTaskbar = TRUE;
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
}

/*------------------------------------------------
   dialog procedure of this page
--------------------------------------------------*/
BOOL CALLBACK PageSkinProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			OnInit(hDlg);
			return TRUE;
		case WM_MEASUREITEM:
			OnMeasureItemColorCombo(lParam);
			return TRUE;
		case WM_DRAWITEM:
			OnDrawItemColorCombo(lParam, 16);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
			// 「スタートボタンを改造する」
			case IDC_STARTBTN:
				OnStartBtn(hDlg);
				break;
			// 「ビットマップ」「キャプション」テキストボックス
			case IDC_FILESTART:
			case IDC_CAPTIONSTART:
			case IDC_FILEMENU:
			case IDC_FILETBG:
				if(code == EN_CHANGE)
					SendPSChanged(hDlg);
				break;
			// スタートボタンのテキストを書式解析
			case IDC_CHK_ISFORMAT:
				SendPSChanged(hDlg);
				break;
			// 「...」 スタートメニューのアイコン参照
			case IDC_SANSHOSTART:
				OnSansho(hDlg, id);
				break;
			// 「...」 スタートメニューのビットマップ参照
			case IDC_SANSHOMENU:
			case IDC_SANSHOTBG:
				OnSanshoMenu(hDlg, id);
				break;
			// 「スタートメニューを改造する」
			case IDC_STARTMENU:
				OnStartMenu(hDlg);
				break;
			// コンボボックス
			case IDC_COLMENU:
				if(code == CBN_SELCHANGE)
					SendPSChanged(hDlg);
				break;
			//「...」色の選択
			case IDC_CHOOSECOLMENU:
				OnChooseColor(hDlg, id, 16);
				SendPSChanged(hDlg);
				break;
			// 「並べる」
			case IDC_TILEMENU:
				SendPSChanged(hDlg);
				break;
			case IDC_TBGUP:
			case IDC_TBGDOWN:
			case IDC_TBGLEFT:
			case IDC_TBGRIGHT:
				SendPSChanged(hDlg);
				break;
			case IDC_TRAYBG:
				OnSkinTray(hDlg);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 6); break;
			}
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
  initialize
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	char s[1024];
	HFONT hfont;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
	{
		SendDlgItemMessage(hDlg, IDC_FILESTART,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_CAPTIONSTART,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_FILEMENU,
			WM_SETFONT, (WPARAM)hfont, 0);
	}

	CheckDlgButton(hDlg, IDC_STARTBTN,
		GetMyRegLong("", "StartButton", FALSE));

	GetMyRegStr("", "StartButtonIcon", s, 1024, "");
	SetDlgItemText(hDlg, IDC_FILESTART, s);

	GetMyRegStr("", "StartButtonCaption", s, 80, MyString(IDS_START));
	SetDlgItemText(hDlg, IDC_CAPTIONSTART, s);

	CheckDlgButton(hDlg, IDC_CHK_ISFORMAT,
		GetMyRegLong("", "StartButtonIsFormat", FALSE));

	OnStartBtn(hDlg);

	CheckDlgButton(hDlg, IDC_STARTMENU,
		GetMyRegLong("", "StartMenu", FALSE));

	GetMyRegStr("", "StartMenuBmp", s, 1024, "");
	SetDlgItemText(hDlg, IDC_FILEMENU, s);

	CheckDlgButton(hDlg, IDC_TRAYBG,
		GetMyRegLong("", "SkinTray", FALSE));

	GetMyRegStr("Clock", "ClockSkin", s, 1024, "");
	SetDlgItemText(hDlg, IDC_FILETBG, s);

	CheckDlgButton(hDlg, IDC_TILEMENU,
		GetMyRegLong("", "StartMenuTile", FALSE));

	SendDlgItemMessage(hDlg, IDC_SPINTBGU, UDM_SETRANGE, 0,
		MAKELONG(1, 999));
	SendDlgItemMessage(hDlg, IDC_SPINTBGU, UDM_SETPOS, 0,
		(int)(short)GetMyRegLong("Clock", "ClockSkinEdgeTop", 0));
	SendDlgItemMessage(hDlg, IDC_SPINTBGD, UDM_SETRANGE, 0,
		MAKELONG(1, 999));
	SendDlgItemMessage(hDlg, IDC_SPINTBGD, UDM_SETPOS, 0,
		(int)(short)GetMyRegLong("Clock", "ClockSkinEdgeBottom", 0));
	SendDlgItemMessage(hDlg, IDC_SPINTBGL, UDM_SETRANGE, 0,
		MAKELONG(1, 999));
	SendDlgItemMessage(hDlg, IDC_SPINTBGL, UDM_SETPOS, 0,
		(int)(short)GetMyRegLong("Clock", "ClockSkinEdgeLeft", 0));
	SendDlgItemMessage(hDlg, IDC_SPINTBGR, UDM_SETRANGE, 0,
		MAKELONG(1, 999));
	SendDlgItemMessage(hDlg, IDC_SPINTBGR, UDM_SETPOS, 0,
		(int)(short)GetMyRegLong("Clock", "ClockSkinEdgeRight", 0));
//	InitColor(hDlg);
	InitComboColor(hDlg, 1, combocolor, 16, FALSE);
	OnStartMenu(hDlg);
	OnSkinTray(hDlg);
}

/*------------------------------------------------
  apply
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	char s[1024];
	DWORD dw;

	SetMyRegLong("", "StartButton", IsDlgButtonChecked(hDlg, IDC_STARTBTN));

	GetDlgItemText(hDlg, IDC_FILESTART, s, 1024);
	SetMyRegStr("", "StartButtonIcon", s);

	GetDlgItemText(hDlg, IDC_CAPTIONSTART, s, 80);
	SetMyRegStr("", "StartButtonCaption", s);

	SetMyRegLong("", "StartButtonIsFormat",
		IsDlgButtonChecked(hDlg, IDC_CHK_ISFORMAT));

	SetMyRegLong("", "StartMenu", IsDlgButtonChecked(hDlg, IDC_STARTMENU));

	GetDlgItemText(hDlg, IDC_FILEMENU, s, 1024);
	SetMyRegStr("", "StartMenuBmp", s);

	dw = (DWORD)CBGetItemData(hDlg, IDC_COLMENU, CBGetCurSel(hDlg, IDC_COLMENU));
	SetMyRegLong("", "StartMenuCol", dw);

	SetMyRegLong("", "StartMenuTile",
		IsDlgButtonChecked(hDlg, IDC_TILEMENU));

	SetMyRegLong("Clock", "ClockSkinEdgeTop",
		SendDlgItemMessage(hDlg, IDC_SPINTBGU, UDM_GETPOS, 0, 0));
	SetMyRegLong("Clock", "ClockSkinEdgeBottom",
		SendDlgItemMessage(hDlg, IDC_SPINTBGD, UDM_GETPOS, 0, 0));
	SetMyRegLong("Clock", "ClockSkinEdgeLeft",
		SendDlgItemMessage(hDlg, IDC_SPINTBGL, UDM_GETPOS, 0, 0));
	SetMyRegLong("Clock", "ClockSkinEdgeRight",
		SendDlgItemMessage(hDlg, IDC_SPINTBGR, UDM_GETPOS, 0, 0));
	GetDlgItemText(hDlg, IDC_FILETBG, s, 1024);
	SetMyRegStr("Clock", "ClockSkin", s);
	SetMyRegLong("", "SkinTray", IsDlgButtonChecked(hDlg, IDC_TRAYBG));
}

/*------------------------------------------------
　「スタートボタンを改造する」
--------------------------------------------------*/
void OnStartBtn(HWND hDlg)
{
	BOOL b;
	int i;

	b = IsDlgButtonChecked(hDlg, IDC_STARTBTN);
#if 1
	for (i = IDC_LABFILESTART; i <= IDC_CHK_ISFORMAT; i++) {
		EnableDlgItem(hDlg, i, b);
	}
#else
	for(i = IDC_LABFILESTART; i <= IDC_CAPTIONSTART; i++)
		EnableDlgItem(hDlg, i, b);
#endif
	SendPSChanged(hDlg);
}

/*------------------------------------------------
　「...」　スタートボタンのアイコン参照
--------------------------------------------------*/
void OnSansho(HWND hDlg, WORD id)
{
	char filter[160], deffile[MAX_PATH], fname[MAX_PATH+10];
	char s[MAX_PATH+10], num[10];
	HFILE hf = HFILE_ERROR; char head[2];

	filter[0] = filter[1] = 0;
	str0cat(filter, MyString(IDS_BMPICONFILE)); str0cat(filter, "*.bmp;*.ico");
	str0cat(filter, MyString(IDS_EXEDLLFILE)); str0cat(filter, "*.exe;*.dll");
	str0cat(filter, MyString(IDS_ALLFILE)); str0cat(filter, "*.*");

	deffile[0] = 0;
	GetDlgItemText(hDlg, id - 1, s, MAX_PATH);
	if(s[0])
	{
		parse(deffile, s, 0);
		parse(num, s, 1);
		hf = _lopen(deffile, OF_READ);
	}
	if(hf != HFILE_ERROR)
	{
		_lread(hf, head, 2);
		_lclose(hf);

		if(head[0] == 'M' && head[1] == 'Z') //実行ファイル
		{
			// 「アイコンの選択」ダイアログ
			if(SelectIconInDLL(hDlg, deffile))
			{
				SetDlgItemText(hDlg, id - 1, deffile);
				PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
				SendPSChanged(hDlg);
			}
			return;
		}
	}

	if(!SelectMyFile(hDlg, filter, 0, deffile, fname)) // propsheet.c
		return;

	hf = _lopen(fname, OF_READ);
	if(hf == HFILE_ERROR) return;
	_lread(hf, head, 2);
	_lclose(hf);
	if(head[0] == 'M' && head[1] == 'Z') //実行ファイル
	{
		// 「アイコンの選択」ダイアログ
		if(!SelectIconInDLL(hDlg, fname)) return;
	}

	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
　「...」　スタートメニューのビットマップ参照
--------------------------------------------------*/
void OnSanshoMenu(HWND hDlg, WORD id)
{
	char filter[80], deffile[MAX_PATH], fname[MAX_PATH];

	filter[0] = filter[1] = 0;
	str0cat(filter, MyString(IDS_BMPFILE)); str0cat(filter, "*.bmp");
	str0cat(filter, MyString(IDS_ALLFILE)); str0cat(filter, "*.*");

	GetDlgItemText(hDlg, id - 1, deffile, MAX_PATH);

	if(!SelectMyFile(hDlg, filter, 0, deffile, fname)) // propsheet.c
		return;

	SetColorFromBmp(hDlg, IDC_COLMENU, fname);

	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
　「スタートメニューを改造する」
--------------------------------------------------*/
void OnStartMenu(HWND hDlg)
{
	BOOL b;
	int i;
	HDC hdc;

	b = IsDlgButtonChecked(hDlg, IDC_STARTMENU);
	for(i = IDC_LABFILEMENU; i <= IDC_TILEMENU; i++)
		EnableDlgItem(hDlg, i, b);
	SendPSChanged(hDlg);

	//環境が256色以下のときは、色の選択を無効に
	hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	if(GetDeviceCaps(hdc, BITSPIXEL) <= 8)
		EnableDlgItem(hDlg, IDC_CHOOSECOLMENU, FALSE);
	DeleteDC(hdc);
}

/*------------------------------------------------
   Select "Color" combo box automatically.
--------------------------------------------------*/
#define WIDTHBYTES(i) ((i+31)/32*4)

void SetColorFromBmp(HWND hDlg, int idCombo, char* fname)
{
	HFILE hf;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	int numColors;
	BYTE pixel[3];
	COLORREF col;
	int i, index = 0;
	HDC hdc;

	hf = _lopen(fname, OF_READ);
	if(hf == HFILE_ERROR) return;

	if(_lread(hf, &bmfh, sizeof(bmfh)) != sizeof(bmfh) ||
		bmfh.bfType != *(WORD*)"BM" ||
		_lread(hf, &bmih, sizeof(bmih)) != sizeof(bmih) ||
		bmih.biSize != sizeof(bmih) ||
		bmih.biCompression != BI_RGB ||
		!(bmih.biBitCount <= 8 || bmih.biBitCount == 24))
	{
		_lclose(hf); return;
	}
	numColors = bmih.biClrUsed;
	if(numColors == 0)
	{
		if(bmih.biBitCount <= 8) numColors = 1 << bmih.biBitCount;
		else numColors = 0;
	}
	if(numColors > 0 &&
		_llseek(hf, sizeof(RGBQUAD)*numColors, FILE_CURRENT) == HFILE_ERROR)
	{
		_lclose(hf); return;
	}
	if(_llseek(hf,
			WIDTHBYTES(bmih.biWidth*bmih.biBitCount)*(bmih.biHeight-1),
			FILE_CURRENT) == HFILE_ERROR ||
		_lread(hf, pixel, sizeof(pixel)) != sizeof(pixel))
	{
		_lclose(hf); return;
	}
	if(bmih.biBitCount < 24)
	{
		index = -1;
		if(bmih.biBitCount == 8) index = pixel[0];
		else if(bmih.biBitCount == 4)
			index = (pixel[0] & 0xF0) >> 4;
		else if(bmih.biBitCount == 1)
			index = (pixel[0] & 0x80) >> 7;
		if(_llseek(hf, sizeof(bmfh)+sizeof(bmih)+sizeof(RGBQUAD)*index,
			FILE_BEGIN) == HFILE_ERROR ||
			_lread(hf, pixel, sizeof(pixel)) != sizeof(pixel))
		{
			index = -1;
		}
	}
	_lclose(hf);
	if(index == -1) return;
	col = RGB(pixel[2], pixel[1], pixel[0]);

	for(i = 0; i < 16; i++)
	{
		if(col == (COLORREF)CBGetItemData(hDlg, idCombo, i)) break;
	}
	if(i == 16)
	{
		int screencolor;
		hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
		screencolor = GetDeviceCaps(hdc, BITSPIXEL);
		DeleteDC(hdc);
		if(screencolor <= 8) return;

		if(CBGetCount(hDlg, idCombo) == 16)
			CBAddString(hDlg, idCombo, col);
		else CBSetItemData(hDlg, idCombo, 16, col);
	}
	CBSetCurSel(hDlg, idCombo, i);
}

/*------------------------------------------------
　「時計の背景に画像を使用する」
--------------------------------------------------*/
void OnSkinTray(HWND hDlg)
{
	BOOL b;
	int i;

	b = IsDlgButtonChecked(hDlg, IDC_TRAYBG);
	for(i = IDC_FILETBG; i <= IDC_LABTB5; i++)
		EnableDlgItem(hDlg, i, b);
	SendPSChanged(hDlg);
}


//------------------------------------------------------------------
// 以下、アイコン選択ダイアログ

INT_PTR CALLBACK DlgProcSelectIcon(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam);
char* fname_SelectIcon;

/*-----------------------------------------------------------------
　アイコンの選択
　fnameは、
　　実行ファイルのとき、"A:\WINDOWS\SYSTEM\SHELL32.DLL,8"
	それ以外のとき、　　"C:\MY PROGRAM\TCLOCK\NIKO.BMP" などとなる
-------------------------------------------------------------------*/
BOOL SelectIconInDLL(HWND hDlg, char* fname)
{
	fname_SelectIcon = fname;
	if(DialogBox(GetLangModule(), MAKEINTRESOURCE(IDD_SELECTICON),
		hDlg, DlgProcSelectIcon) != IDOK) return FALSE;
	return TRUE;
}

static BOOL InitSelectIcon(HWND hDlg);
static void EndSelectIcon(HWND hDlg);
static void OnOKSelectIcon(HWND hDlg);
static void OnMeasureItemListSelectIcon(HWND hDlg, LPARAM lParam);
static void OnDrawItemListSelectIcon(LPARAM lParam);
static void OnSanshoSelectIcon(HWND hDlg);

/*------------------------------------------------
　アイコン選択ダイアログプロシージャ
--------------------------------------------------*/
INT_PTR CALLBACK DlgProcSelectIcon(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			if(!InitSelectIcon(hDlg))
				EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case WM_MEASUREITEM:
			OnMeasureItemListSelectIcon(hDlg, lParam);
			return TRUE;
		case WM_DRAWITEM:
			OnDrawItemListSelectIcon(lParam);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			if(id == IDC_SANSHOICON) OnSanshoSelectIcon(hDlg);
			else if(id == IDOK || id == IDCANCEL)
			{
				if(id == IDOK) OnOKSelectIcon(hDlg);
				EndSelectIcon(hDlg);
				EndDialog(hDlg, id);
			}
			return TRUE;
		}
	}
	return FALSE;
}

/*------------------------------------------------
　アイコン選択ダイアログの初期化
--------------------------------------------------*/
BOOL InitSelectIcon(HWND hDlg)
{
	int i, count, index;
	HICON hicon, hiconl;
	char msg[MAX_PATH];
	char fname[MAX_PATH], num[10];

	parse(fname, fname_SelectIcon, 0);
	parse(num, fname_SelectIcon, 1);
	if(num[0] == 0) index = 0;
	else index = atoi(num);

#pragma warning(push)
#pragma warning(disable: 4311)	// '型キャスト' : ポインタを 'HICON' から 'int' へ切り詰めます。
	count = (int)ExtractIcon(g_hInst, fname, (UINT)-1);
#pragma warning(pop)

	if(count == 0)
	{
		strcpy(msg, MyString(IDS_NOICON));
		strcat(msg, "\n");
		strcat(msg, fname);
		MyMessageBox(hDlg, msg, "TClock", MB_OK, MB_ICONEXCLAMATION);
		return FALSE;
	}

	EndSelectIcon(hDlg);
	SendDlgItemMessage(hDlg, IDC_LISTICON, LB_RESETCONTENT, 0, 0);

	for(i = 0; i < count; i++)
	{
		hiconl = NULL; hicon = NULL;
		ExtractIconEx(fname, i, &hiconl, &hicon, 1);
		if(hiconl) DestroyIcon(hiconl);
		SendDlgItemMessage(hDlg, IDC_LISTICON, LB_ADDSTRING, 0,
			(LPARAM)hicon);
	}
	SetDlgItemText(hDlg, IDC_FNAMEICON, fname);
	SendDlgItemMessage(hDlg, IDC_LISTICON, LB_SETCURSEL,
		index, 0);
	strcpy(fname_SelectIcon, fname);
	return TRUE;
}

/*------------------------------------------------
　アイコン選択ダイアログの後始末
--------------------------------------------------*/
void EndSelectIcon(HWND hDlg)
{
	int i, count;
	HICON hicon;
	count = SendDlgItemMessage(hDlg, IDC_LISTICON, LB_GETCOUNT, 0, 0);
	for(i = 0; i < count; i++)
	{
		hicon = (HICON)SendDlgItemMessage(hDlg, IDC_LISTICON,
			LB_GETITEMDATA, i, 0);
		if(hicon) DestroyIcon(hicon);
	}
}

/*------------------------------------------------
　アイコン選択ダイアログの「OK」
--------------------------------------------------*/
void OnOKSelectIcon(HWND hDlg)
{
	char num[10];
	int index;

	GetDlgItemText(hDlg, IDC_FNAMEICON, fname_SelectIcon, MAX_PATH);
	index = SendDlgItemMessage(hDlg, IDC_LISTICON, LB_GETCURSEL, 0, 0);
	wsprintf(num, ",%d", index);
	strcat(fname_SelectIcon, num);
}

/*------------------------------------------------
　アイコンリストのサイズを決める
--------------------------------------------------*/
void OnMeasureItemListSelectIcon(HWND hDlg, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT pMis;
	RECT rc;

	pMis = (LPMEASUREITEMSTRUCT)lParam;
	GetClientRect(GetDlgItem(hDlg, pMis->CtlID), &rc);
	pMis->itemHeight = rc.bottom;
	pMis->itemWidth = 32;
}

/*------------------------------------------------
　アイコンリストの描画
--------------------------------------------------*/
void OnDrawItemListSelectIcon(LPARAM lParam)
{
	LPDRAWITEMSTRUCT pDis;
	HBRUSH hbr;
	COLORREF col;
	RECT rc;
	int cxicon, cyicon;

	pDis = (LPDRAWITEMSTRUCT)lParam;

	switch(pDis->itemAction)
	{
		case ODA_DRAWENTIRE:
		case ODA_SELECT:
		{
			if(pDis->itemState & ODS_SELECTED)
				col = GetSysColor(COLOR_HIGHLIGHT);
			else col = GetSysColor(COLOR_WINDOW);
			hbr = CreateSolidBrush(col);
			FillRect(pDis->hDC, &pDis->rcItem, hbr);
			DeleteObject(hbr);
			if(!(pDis->itemState & ODS_FOCUS)) break;
		}
		case ODA_FOCUS:
		{
			if(pDis->itemState & ODS_FOCUS)
				col = GetSysColor(COLOR_WINDOWTEXT);
			else
				col = GetSysColor(COLOR_WINDOW);
			hbr = CreateSolidBrush(col);
			FrameRect(pDis->hDC, &pDis->rcItem, hbr);
			DeleteObject(hbr);
			break;
		}
	}

	if(pDis->itemData == 0) return;

	cxicon = GetSystemMetrics(SM_CXSMICON);
	cyicon = GetSystemMetrics(SM_CYSMICON);

	CopyRect(&rc, &(pDis->rcItem));
	DrawIconEx(pDis->hDC,
		rc.left + (rc.right - rc.left - cxicon)/2,
		rc.top + (rc.bottom - rc.top - cyicon)/2,
		(HICON)pDis->itemData,
		cxicon, cyicon, 0, NULL, DI_NORMAL);
}

/*------------------------------------------------
　アイコンの選択の中のファイルの参照
--------------------------------------------------*/
void OnSanshoSelectIcon(HWND hDlg)
{
	char filter[160], deffile[MAX_PATH], fname[MAX_PATH];
	HFILE hf = HFILE_ERROR;
	char head[2];

	filter[0] = filter[1] = 0;
	str0cat(filter, MyString(IDS_BMPICONFILE)); str0cat(filter, "*.bmp;*.ico");
	str0cat(filter, MyString(IDS_EXEDLLFILE)); str0cat(filter, "*.exe;*.dll");
	str0cat(filter, MyString(IDS_ALLFILE)); str0cat(filter, "*.*");

	GetDlgItemText(hDlg, IDC_FNAMEICON, deffile, MAX_PATH);

	if(!SelectMyFile(hDlg, filter, 2, deffile, fname))
		return;

	hf = _lopen(fname, OF_READ);
	if(hf == HFILE_ERROR) return;
	_lread(hf, head, 2);
	_lclose(hf);
	strcpy(fname_SelectIcon, fname);

	if(head[0] == 'M' && head[1] == 'Z') //実行ファイル
	{
		if(InitSelectIcon(hDlg))
			PostMessage(hDlg, WM_NEXTDLGCTL,
				(WPARAM)GetDlgItem(hDlg, IDC_LISTICON), TRUE);
	}
	else
	{
		EndSelectIcon(hDlg);
		EndDialog(hDlg, IDOK);
	}
}

