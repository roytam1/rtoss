/*-------------------------------------------
  pagetooltip.c
　　「ツールチップ」プロパティページ
　　KAZUBON 1997-1998 / 635@p5
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnDestroy(HWND hDlg);
static void OnSansho(HWND hDlg, WORD id);
static void OnTipSwitch(HWND hDlg, WORD id);
static void InitComboFontTip(HWND hDlg); //635@p5
static void SetComboFontSizeTip(HWND hDlg, int bInit);
static HFONT hfontb;  // for IDC_BOLD
static HFONT hfonti;  // for IDC_ITALIC
static COMBOCOLOR combocolor[2] = {
	{ IDC_TFONCOL,      "TipFonColor",  0x80000000|COLOR_BTNTEXT },
	{ IDC_TBAKCOL,      "TipBakColor",  0x80000000|COLOR_INFOBK }
};

extern int confNo;

//static char reg_section[] = "";

typedef struct {
	BOOL disable;
	int func[4];
	char format[4][256];
	char fname[4][256];
} CLICKDATA;
static CLICKDATA *pData = NULL;

//635@p5
//#define SendPSChanged(hDlg) SendMessage(GetParent(hDlg),PSM_CHANGED,(WPARAM)(hDlg),0)
__inline void SendPSChanged(HWND hDlg)
{
	g_bApplyTaskbar = TRUE;
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
}

/*------------------------------------------------
　「ツールチップ」ページ用ダイアログプロシージャ
--------------------------------------------------*/
BOOL CALLBACK PageTooltipProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			OnDrawItemColorCombo(lParam, 20);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
			case IDC_TFONT:
				if(code == CBN_SELCHANGE)
				{
					SetComboFontSizeTip(hDlg, FALSE);
					SendPSChanged(hDlg);
					g_bApplyClock = TRUE;
				}
				break;
			case IDC_TFONTSIZE:
				if(code == CBN_SELCHANGE || code == CBN_EDITCHANGE)
				{
					SendPSChanged(hDlg);
					g_bApplyClock = TRUE;
				}
				break;
			case IDC_TINTERVAL:
			case IDC_TINTERVALSPIN:
			case IDC_TBOLD:
			case IDC_TITALIC:
			case IDC_ALPHATIP:
			case IDC_ALTIPSPIN:
			case IDC_DISPT:
			case IDC_DISPTSPIN:
			case IDC_TIPTATE:
			case IDC_UPDATETIP1:
			case IDC_UPDATETIP2:
			case IDC_UPDATETIP3:
			case IDC_DISABLETIPCUSTOMDRAW:
			case IDC_ENABLEDOUBLEBUFFERING:
			case IDC_HTMLSIZEX:
			case IDC_HTMLSIZEY:
				SendPSChanged(hDlg);
				g_bApplyClock = TRUE;
				break;
			case IDC_TIP2:
				OnTipSwitch(hDlg, id);
				break;
			case IDC_TIP3:
				SendPSChanged(hDlg);
				break;
			case IDC_TFONCOL:
			case IDC_TBAKCOL:
			case IDC_TICON:
			case IDC_BALLOONFLG:
				if(code == CBN_SELCHANGE)
				{
					g_bApplyClock = TRUE;
					SendPSChanged(hDlg);
				}
				break;
			case IDC_TTITLE:
				if(code == EN_CHANGE)
				{
					g_bApplyClock = TRUE;
					SendPSChanged(hDlg);
				}
				break;
			case IDC_CHOOSETFONCOL:
			case IDC_CHOOSETBAKCOL:
				OnChooseColor(hDlg, id, 20);
				SendPSChanged(hDlg);
				break;
			case IDC_SANSHOTIP:
			case IDC_SANSHOTIP2:
			case IDC_SANSHOTIP3:
				OnSansho(hDlg, id);
				break;
			case IDC_TOOLTIP:
			case IDC_TOOLTIP2:
			case IDC_TOOLTIP3:
				if(code == EN_CHANGE)
				{
					g_bApplyClock = TRUE;
					SendPSChanged(hDlg);
				}
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 5); break;
			}
			return TRUE;
		case WM_DESTROY:
			DeleteObject(hfontb);
			DeleteObject(hfonti);
			OnDestroy(hDlg);
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
　ページの初期化
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	char s[1024];
	int i;
	HFONT hfont;
	LOGFONT logfont;
	DWORD dw;
	HDC hdc;
	BOOL bTip2;

	//設定切り替え
	confNo = GetMyRegLong("", "ConfigNo", 1);

	// setting of "background" and "text"
	InitComboColor(hDlg, 2, combocolor, 20, TRUE);

	// if color depth is 256 or less
	hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	if(GetDeviceCaps(hdc, BITSPIXEL) <= 8)
	{
		EnableDlgItem(hDlg, IDC_CHOOSETFONCOL, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSETBAKCOL, FALSE);
	}
	DeleteDC(hdc);

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
	{
		SendDlgItemMessage(hDlg, IDC_TOOLTIP,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_TOOLTIP2,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_TOOLTIP3,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_TFONT,
			WM_SETFONT, (WPARAM)hfont, 0);
	}

	//「フォント」の設定
	InitComboFontTip(hDlg);
	//「フォントサイズ」の設定
	SetComboFontSizeTip(hDlg, TRUE);

	for(i = IDS_TICONNO; i <= IDS_TICONERR; i++)
		CBAddString(hDlg, IDC_TICON, (LPARAM)MyString(i));
	CBSetCurSel(hDlg, IDC_TICON,
		GetMyRegLongEx("", "TipIcon", 0, confNo));

	GetMyRegStrEx("", "TipTitle", s, 300, "", confNo);
	SetDlgItemText(hDlg, IDC_TTITLE, s);
	//end

	GetMyRegStrEx("Tooltip", "Tooltip", s, 1024, "", confNo);
	if(s[0] == 0) strcpy(s, "TClock <%LDATE%>");
	SetDlgItemText(hDlg, IDC_TOOLTIP, s);
	GetMyRegStrEx("Tooltip", "Tooltip2", s, 1024, "", confNo);
	if(s[0] == 0) strcpy(s, "TClock <%LDATE%>");
	SetDlgItemText(hDlg, IDC_TOOLTIP2, s);
	GetMyRegStrEx("Tooltip", "Tooltip3", s, 1024, "", confNo);
	if(s[0] == 0) strcpy(s, "TClock <%LDATE%>");
	SetDlgItemText(hDlg, IDC_TOOLTIP3, s);

	bTip2 = GetMyRegLongEx("Tooltip", "Tip2Use", FALSE, confNo);
	CheckDlgButton(hDlg, IDC_TIP2,bTip2);
	CheckDlgButton(hDlg, IDC_TIP3,
		GetMyRegLongEx("Tooltip", "Tip3Use", FALSE, confNo));

	dw = GetMyRegLongEx("Tooltip", "Tip2Use", FALSE, confNo);

	CheckDlgButton(hDlg, IDC_UPDATETIP1, GetMyRegLongEx("Tooltip", "Tip1Update", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_UPDATETIP2, GetMyRegLongEx("Tooltip", "Tip2Update", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_UPDATETIP3, GetMyRegLongEx("Tooltip", "Tip3Update", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_DISABLETIPCUSTOMDRAW, GetMyRegLongEx("Tooltip", "TipDisableCustomDraw", TRUE, confNo));
	CheckDlgButton(hDlg, IDC_ENABLEDOUBLEBUFFERING, GetMyRegLongEx("Tooltip", "TipEnableDoubleBuffering", FALSE, confNo));


	SetDlgItemText(hDlg, IDC_TOOLTIP3, s);
	if (!bTip2){
		CheckDlgButton(hDlg, IDC_TIP3,FALSE);
		EnableDlgItem(hDlg,IDC_TIP3,FALSE);
	}

	CheckDlgButton(hDlg, IDC_TIPTATE,
		GetMyRegLongEx("Tooltip", "TipTateFlg", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_TBOLD,
		GetMyRegLongEx("", "TipBold", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_TITALIC,
		GetMyRegLongEx("", "TipItalic", FALSE, confNo));
	for(i = IDS_TIPTYPENORMAL; i <= IDS_TIPTYPEIECOMP; i++)
		CBAddString(hDlg, IDC_BALLOONFLG, (LPARAM)MyString(i));
	CBSetCurSel(hDlg, IDC_BALLOONFLG,
		GetMyRegLongEx("", "BalloonFlg", 0, confNo));
	AdjustDlgConboBoxDropDown(hDlg, IDC_BALLOONFLG, 3);


	hfontb = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
	GetObject(hfontb, sizeof(LOGFONT), &logfont);
	logfont.lfWeight = FW_BOLD;
	hfontb = CreateFontIndirect(&logfont);
	SendDlgItemMessage(hDlg, IDC_TBOLD, WM_SETFONT, (WPARAM)hfontb, 0);

	logfont.lfWeight = FW_NORMAL;
	logfont.lfItalic = 1;
	hfonti = CreateFontIndirect(&logfont);
	SendDlgItemMessage(hDlg, IDC_TITALIC, WM_SETFONT, (WPARAM)hfonti, 0);

	dw = GetVersion();
	if(!(dw & 0x80000000) && LOBYTE(LOWORD(dw)) >= 5) ; // Win2000
	else
	{
		EnableDlgItem(hDlg, IDC_ALPHATIP, FALSE);
		EnableDlgItem(hDlg, IDC_ALTIPSPIN, FALSE);
	}

	dw = GetMyRegLongEx(NULL, "AlphaTip", 0, confNo);
	if(dw > 100) dw = 100;
	if(dw < 0  ) dw = 0;
	SendDlgItemMessage(hDlg,IDC_ALTIPSPIN,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)100, (short)0));
	SendDlgItemMessage(hDlg, IDC_ALTIPSPIN, UDM_SETPOS, 0,
		(int)(short)dw);

	dw = GetMyRegLong(NULL, "TipDispTime", 5);
	if(dw > 30) dw = 30;
	if(dw < 1  ) dw = 1;
	SendDlgItemMessage(hDlg,IDC_DISPTSPIN,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)30, (short)1));
	SendDlgItemMessage(hDlg, IDC_DISPTSPIN, UDM_SETPOS, 0,
		(int)(short)dw);

	dw = GetMyRegLong("Tooltip", "TipDispInterval", 0);
	if(dw > 30) dw = 30;
	if(dw < 1) dw = 1;
	SendDlgItemMessage(hDlg,IDC_TINTERVALSPIN,UDM_SETRANGE,0,
		(LPARAM) MAKELONG((short)30, (short)1));
	SendDlgItemMessage(hDlg, IDC_TINTERVALSPIN, UDM_SETPOS, 0,
		(int)(short)dw);

	SetDlgItemInt(hDlg, IDC_HTMLSIZEX, GetMyRegLong("Tooltip", "HTMLTipDispSizeX", 200), FALSE);
	SetDlgItemInt(hDlg, IDC_HTMLSIZEY, GetMyRegLong("Tooltip", "HTMLTipDispSizeY", 200), FALSE);
}

/*------------------------------------------------
　更新
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	char s[1024];
	int n;
	DWORD dw;

	GetDlgItemText(hDlg, IDC_TOOLTIP, s, 1024);
	SetMyRegStrEx("Tooltip", "Tooltip", s, confNo);
	GetDlgItemText(hDlg, IDC_TOOLTIP2, s, 1024);
	SetMyRegStrEx("Tooltip", "Tooltip2", s, confNo);
	GetDlgItemText(hDlg, IDC_TOOLTIP3, s, 1024);
	SetMyRegStrEx("Tooltip", "Tooltip3", s, confNo);

	SetMyRegLongEx("Tooltip", "Tip2Use", IsDlgButtonChecked(hDlg, IDC_TIP2), confNo);
	SetMyRegLongEx("Tooltip", "Tip3Use", IsDlgButtonChecked(hDlg, IDC_TIP3), confNo);
	SetMyRegLongEx("Tooltip", "TipTateFlg", IsDlgButtonChecked(hDlg, IDC_TIPTATE), confNo);

	SetMyRegLongEx("Tooltip", "Tip1Update", IsDlgButtonChecked(hDlg, IDC_UPDATETIP1), confNo);
	SetMyRegLongEx("Tooltip", "Tip2Update", IsDlgButtonChecked(hDlg, IDC_UPDATETIP2), confNo);
	SetMyRegLongEx("Tooltip", "Tip3Update", IsDlgButtonChecked(hDlg, IDC_UPDATETIP3), confNo);
	SetMyRegLongEx("Tooltip", "TipDisableCustomDraw", IsDlgButtonChecked(hDlg, IDC_DISABLETIPCUSTOMDRAW), confNo);
	SetMyRegLongEx("Tooltip", "TipEnableDoubleBuffering", IsDlgButtonChecked(hDlg, IDC_ENABLEDOUBLEBUFFERING), confNo);

	//フォント名の保存
	CBGetLBText(hDlg, IDC_TFONT, CBGetCurSel(hDlg, IDC_TFONT), s);
	SetMyRegStrEx("", "TipFont", s, confNo);

	//フォントサイズの保存
	//5以下のサイズは許可しない(数値でないものも含まれる)
	dw = GetDlgItemInt(hDlg, IDC_TFONTSIZE, NULL, FALSE);
	if(dw < 5)
	{
		dw = 9;
		SetDlgItemInt(hDlg, IDC_TFONTSIZE, dw, FALSE);
	}
	SetMyRegLongEx("", "TipFontSize", dw, confNo);

	//「Bold」「Italic」の保存
	SetMyRegLongEx("", "TipBold", IsDlgButtonChecked(hDlg, IDC_TBOLD), confNo);
	SetMyRegLongEx("", "TipItalic", IsDlgButtonChecked(hDlg, IDC_TITALIC), confNo);
	SetMyRegLong("", "BalloonFlg", CBGetCurSel(hDlg, IDC_BALLOONFLG));

	dw = GetDlgItemInt(hDlg, IDC_ALPHATIP, NULL, FALSE);
	if(dw > 100) dw = 100;
	if(dw < 0  ) dw = 0;
	SetDlgItemInt(hDlg, IDC_ALPHATIP, dw, FALSE);

	SetMyRegLongEx("", "AlphaTip", dw, confNo);

	dw = GetDlgItemInt(hDlg, IDC_DISPT, NULL, FALSE);
	if(dw > 30) dw = 30;
	if(dw < 1  ) dw = 1;
	SetDlgItemInt(hDlg, IDC_DISPT, dw, FALSE);

	SetMyRegLong("", "TipDispTime", dw);

	dw = GetDlgItemInt(hDlg, IDC_TINTERVAL, NULL, FALSE);
	if(dw > 30) dw = 30;
	if(dw < 1) dw = 1;
	SetDlgItemInt(hDlg, IDC_TINTERVAL, dw, FALSE);

	SetMyRegLong("Tooltip", "TipDispInterval", dw);


	dw = (DWORD)CBGetItemData(hDlg, IDC_TFONCOL, CBGetCurSel(hDlg, IDC_TFONCOL));
	SetMyRegLongEx("", "TipFonColor", dw, confNo);
	dw = (DWORD)CBGetItemData(hDlg, IDC_TBAKCOL, CBGetCurSel(hDlg, IDC_TBAKCOL));
	SetMyRegLongEx("", "TipBakColor", dw, confNo);

	n = CBGetCurSel(hDlg, IDC_TICON);
	SetMyRegLongEx("", "TipIcon", n, confNo);
	GetDlgItemText(hDlg, IDC_TTITLE, s, 300);
	SetMyRegStrEx("", "TipTitle", s, confNo);

	GetDlgItemText(hDlg, IDC_HTMLSIZEX, s, 20);
	SetMyRegLong("Tooltip", "HTMLTipDispSizeX", atoi(s));
	GetDlgItemText(hDlg, IDC_HTMLSIZEY, s, 20);
	SetMyRegLong("Tooltip", "HTMLTipDispSizeY", atoi(s));
}

/*------------------------------------------------

--------------------------------------------------*/
void OnDestroy(HWND hDlg)
{
	UNREFERENCED_PARAMETER(hDlg);
	if(pData) free(pData);
}

/*------------------------------------------------
　「...」　ファイルの参照
--------------------------------------------------*/
void OnSansho(HWND hDlg, WORD id)
{
	char filter[80], deffile[MAX_PATH], fname[MAX_PATH], tipfname[MAX_PATH];

	filter[0] = 0;
	str0cat(filter, MyString(IDS_TIPFILEEXT));
	str0cat(filter, "*.txt");
	str0cat(filter, MyString(IDS_ALLFILE));
	str0cat(filter, "*.*");

	GetDlgItemText(hDlg, id - 1, deffile, MAX_PATH);

	if(!SelectMyFile(hDlg, filter, 0, deffile, fname)) // propsheet.c
		return;

	strcpy(tipfname,"file:");
	strcat(tipfname,fname);
	strcpy(fname,tipfname);

	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
	SendPSChanged(hDlg);
}

void OnTipSwitch(HWND hDlg, WORD id)
{
	UNREFERENCED_PARAMETER(id);
	if(IsDlgButtonChecked(hDlg, IDC_TIP2))
	{
		EnableDlgItem(hDlg,IDC_TIP3,TRUE);
	}
	else
	{
		CheckDlgButton(hDlg, IDC_TIP3,FALSE);
		EnableDlgItem(hDlg,IDC_TIP3,FALSE);
	}
	SendPSChanged(hDlg);
}

BOOL CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* lpntm, int FontType, LPARAM hCombo);
BOOL CALLBACK EnumSizeProcEx(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* lpntm, int FontType, LPARAM hCombo);
int logpixelsyTip;

/*------------------------------------------------
   Initialization of "Font" combo box
--------------------------------------------------*/
void InitComboFontTip(HWND hDlg)
{
	HDC hdc;
	LOGFONT lf;
	HWND hcombo;
	char s[80];
	int i;

	hdc = GetDC(NULL);

	// Enumerate fonts and set in the combo box
	memset(&lf, 0, sizeof(LOGFONT));
	hcombo = GetDlgItem(hDlg, IDC_TFONT);
	lf.lfCharSet = (BYTE)GetTextCharset(hdc);  // MS UI Gothic, ...
	EnumFontFamiliesEx(hdc, &lf,
		(FONTENUMPROC)EnumFontFamExProc, (LPARAM)hcombo, 0);
	lf.lfCharSet = OEM_CHARSET;   // Small Fonts, Terminal...
	EnumFontFamiliesEx(hdc, &lf,
		(FONTENUMPROC)EnumFontFamExProc, (LPARAM)hcombo, 0);
	lf.lfCharSet = DEFAULT_CHARSET;  // Arial, Courier, Times New Roman, ...
	EnumFontFamiliesEx(hdc, &lf,
		(FONTENUMPROC)EnumFontFamExProc, (LPARAM)hcombo, 0);
	ReleaseDC(NULL, hdc);

	GetMyRegStrEx("", "TipFont", s, 80, "", confNo);
	if(s[0] == 0)
	{
		HFONT hfont;
		hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if(hfont)
		{
			GetObject(hfont, sizeof(lf),(LPVOID)&lf);
			strcpy(s, lf.lfFaceName);
		}
	}
	i = CBFindStringExact(hDlg, IDC_TFONT, s);
	if(i == LB_ERR) i = 0;
	CBSetCurSel(hDlg, IDC_TFONT, i);

	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_TFONT, 12);
}

/*------------------------------------------------
　「フォントサイズ」コンボボックスの設定
--------------------------------------------------*/
void SetComboFontSizeTip(HWND hDlg, BOOL bInit)
{
	HDC hdc;
	char s[160];
	DWORD size;
	LOGFONT lf;

	//以前のsizeを保存
	if(bInit) // WM_INITDIALOGのとき
	{
		size = GetMyRegLongEx("", "TipFontSize", 9, confNo);
		if(size == 0) size = 9;

	}
	else   // IDC_TFONTが変更されたとき
	{
		size = GetDlgItemInt(hDlg, IDC_TFONTSIZE, NULL, FALSE);
	}

	CBResetContent(hDlg, IDC_TFONTSIZE);

	hdc = GetDC(NULL);
	logpixelsyTip = GetDeviceCaps(hdc, LOGPIXELSY);

	// s = フォント名
	CBGetLBText(hDlg, IDC_TFONT, CBGetCurSel(hDlg, IDC_TFONT), (LPARAM)s);

	//フォントのサイズを列挙してコンボボックスに入れる
	memset(&lf, 0, sizeof(LOGFONT));
	strcpy(lf.lfFaceName, s);
	lf.lfCharSet = (BYTE)CBGetItemData(hDlg, IDC_TFONT, CBGetCurSel(hDlg, IDC_TFONT));
	EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumSizeProcEx,
		(LPARAM)GetDlgItem(hDlg, IDC_TFONTSIZE), 0);

	//EnumFontFamilies(hdc, s,
	//	(FONTENUMPROC)EnumSizeProc, (LPARAM)GetDlgItem(hDlg, IDC_TFONTSIZE));

	ReleaseDC(NULL, hdc);

	// sizeをセット
	SetDlgItemInt(hDlg, IDC_TFONTSIZE, size, FALSE);

	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_TFONTSIZE, 10);
}

