/*-------------------------------------------
  pagecolor.c
  "Color and Font" page
  KAZUBON 1997-1998
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnCheckColor2(HWND hDlg);
static void OnCheckShadow(HWND hDlg, int id);
static void InitComboFont(HWND hDlg);
static void SetComboFontSize(HWND hDlg, int bInit);
static HFONT hfontb;  // for IDC_BOLD
static HFONT hfonti;  // for IDC_ITALIC
static BOOL UseGrad = FALSE;
int confNo;

static COMBOCOLOR combocolor[4] = {
	{ IDC_COLBACK,      "BackColor",   0x80000000|COLOR_3DFACE },
	{ IDC_COLBACK2,     "BackColor2",  0xFFFFFFFF },
	{ IDC_COLFORE,      "ForeColor",   0x80000000|COLOR_BTNTEXT },
	{ IDC_COLCLKSHADOW, "ShadowColor", RGB(0, 0, 0) }
};

__inline void SendPSChanged(HWND hDlg)
{
	g_bApplyClock = TRUE;
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
}

/*------------------------------------------------
  Dialog procedure
--------------------------------------------------*/
BOOL CALLBACK PageColorProc(HWND hDlg, UINT message,
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
			OnDrawItemColorCombo(lParam, 20);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
			case IDC_FONT:
				if(code == CBN_SELCHANGE)
				{
					SetComboFontSize(hDlg, FALSE);
					SendPSChanged(hDlg);
				}
				break;
			case IDC_COLBACK:
			case IDC_COLBACK2:
			case IDC_COLFORE:
			case IDC_COLCLKSHADOW:
			case IDC_TEXTPOS:
				if(code == CBN_SELCHANGE)
				{
					SendPSChanged(hDlg);
				}
				break;
			case IDC_FONTSIZE:
				if(code == CBN_SELCHANGE || code == CBN_EDITCHANGE)
				{
					SendPSChanged(hDlg);
				}
				break;
			case IDC_CHKCOLOR:
			case IDC_CHKCOLOR2:
				OnCheckColor2(hDlg);
				break;
			case IDC_CHKCLKSHADOW:
			case IDC_CHKCLKBORDER:
				OnCheckShadow(hDlg, id);
				break;
			case IDC_CHOOSECOLBACK:
			case IDC_CHOOSECOLBACK2:
			case IDC_CHOOSECOLFORE:
			case IDC_CHOOSECOLCLKSHADOW:
				OnChooseColor(hDlg, id, 20);
				SendPSChanged(hDlg);
				break;
			case IDC_BOLD:
			case IDC_ITALIC:
			case IDC_CHKCOLORV:
				SendPSChanged(hDlg);
				break;
			case IDC_CLOCKHEIGHT:
			case IDC_CLOCKWIDTH:
			case IDC_VERTPOS:
			case IDC_LINEHEIGHT:
			case IDC_CLKSHADOWRANGE:
				if(code == EN_CHANGE)
				{
					SendPSChanged(hDlg);
				}
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 1); break;
			}
			return TRUE;
		case WM_DESTROY:
			DeleteObject(hfontb);
			DeleteObject(hfonti);
			break;
	}
	return FALSE;
}

/*------------------------------------------------
  Initialize
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	HDC hdc;
	LOGFONT logfont;
	HFONT hfont;
	DWORD dwVer;
	int index;

	//設定切り替え
	confNo = GetMyRegLong("", "ConfigNo", 1);

	// setting of "background" and "text"
	InitComboColor(hDlg, 4, combocolor, 20, TRUE);

	// if color depth is 256 or less
	hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	if(GetDeviceCaps(hdc, BITSPIXEL) <= 8)
	{
		EnableDlgItem(hDlg, IDC_CHOOSECOLBACK, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLBACK2, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLFORE, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLCLKSHADOW, FALSE);
	}
	DeleteDC(hdc);
	
	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
		SendDlgItemMessage(hDlg, IDC_FONT, WM_SETFONT, (WPARAM)hfont, 0);

	//「フォント」の設定
	InitComboFont(hDlg);
	//「フォントサイズ」の設定
	SetComboFontSize(hDlg, TRUE);
	//「テキストの位置」の設定
	index = CBAddString(hDlg, IDC_TEXTPOS, (LPARAM)MyString(IDS_TEXTCENTER));
//	CBSetItemData(hDlg, IDC_TEXTPOS, index, 0);
	index = CBAddString(hDlg, IDC_TEXTPOS, (LPARAM)MyString(IDS_TEXTLEFT));
//	CBSetItemData(hDlg, IDC_TEXTPOS, index, 1);
	index = CBAddString(hDlg, IDC_TEXTPOS, (LPARAM)MyString(IDS_TEXTRIGHT));
//	CBSetItemData(hDlg, IDC_TEXTPOS, index, 2);

	CBSetCurSel(hDlg, IDC_TEXTPOS,
		GetMyRegLongEx("", "TextPos", 0, confNo));
	{
		BOOL bWin2000;
		DWORD dw = GetVersion();
		bWin2000 = (!(dw & 0x80000000) && LOBYTE(LOWORD(dw)) == 5 && HIBYTE(LOWORD(dw)) == 0);
		CheckDlgButton(hDlg, IDC_CHKCOLOR,
			GetMyRegLongEx("", "UseBackColor", bWin2000, confNo));
	}
	CheckDlgButton(hDlg, IDC_CHKCOLOR2,
		GetMyRegLongEx("", "UseBackColor2", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_CHKCOLORV,
		GetMyRegLongEx("", "GradDir", FALSE, confNo));

	dwVer = GetVersion();
	if(((dwVer & 0x80000000) && // 98/Me/2000
		LOBYTE(LOWORD(dwVer)) >= 4 && HIBYTE(LOWORD(dwVer)) >= 10) ||
		(!(dwVer & 0x80000000) && LOBYTE(LOWORD(dwVer)) >= 5))
	{
		UseGrad = TRUE;
	}
	else // not (Win98 or 2000)
	{
		UseGrad = FALSE;
	}
	OnCheckColor2(hDlg);

	CheckDlgButton(hDlg, IDC_CHKCLKSHADOW,
		GetMyRegLongEx("", "UseClockShadow", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_CHKCLKBORDER,
		GetMyRegLongEx("", "UseClockBorder", FALSE, confNo));
	SendDlgItemMessage(hDlg, IDC_SPINCLKSHADOW, UDM_SETRANGE, 0,
		MAKELONG(10, 0));
	SendDlgItemMessage(hDlg, IDC_SPINCLKSHADOW, UDM_SETPOS, 0,
		(int)(short)GetMyRegLongEx("", "ClockShadowRange", 1, confNo));
	OnCheckShadow(hDlg, 0);

	//「Bold」「Italic」の設定
	CheckDlgButton(hDlg, IDC_BOLD,
		GetMyRegLongEx("", "Bold", FALSE, confNo));
	CheckDlgButton(hDlg, IDC_ITALIC,
		GetMyRegLongEx("", "Italic", FALSE, confNo));

	hfontb = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
	GetObject(hfontb, sizeof(LOGFONT), &logfont);
	logfont.lfWeight = FW_BOLD;
	hfontb = CreateFontIndirect(&logfont);
	SendDlgItemMessage(hDlg, IDC_BOLD, WM_SETFONT, (WPARAM)hfontb, 0);

	logfont.lfWeight = FW_NORMAL;
	logfont.lfItalic = 1;
	hfonti = CreateFontIndirect(&logfont);
	SendDlgItemMessage(hDlg, IDC_ITALIC, WM_SETFONT, (WPARAM)hfonti, 0);

	SendDlgItemMessage(hDlg, IDC_SPINCHEIGHT, UDM_SETRANGE, 0,
		MAKELONG(32, -32));
	SendDlgItemMessage(hDlg, IDC_SPINCHEIGHT, UDM_SETPOS, 0,
		(int)(short)GetMyRegLongEx("", "ClockHeight", 0, confNo));
	SendDlgItemMessage(hDlg, IDC_SPINCWIDTH, UDM_SETRANGE, 0,
		MAKELONG(32, -32));
	SendDlgItemMessage(hDlg, IDC_SPINCWIDTH, UDM_SETPOS, 0,
		(int)(short)GetMyRegLongEx("", "ClockWidth", 0, confNo));
	SendDlgItemMessage(hDlg, IDC_SPINVPOS, UDM_SETRANGE, 0,
		MAKELONG(32, -32));
	SendDlgItemMessage(hDlg, IDC_SPINVPOS, UDM_SETPOS, 0,
		(int)(short)GetMyRegLongEx("", "VertPos", 0, confNo));
	SendDlgItemMessage(hDlg, IDC_SPINLHEIGHT, UDM_SETRANGE, 0,
		MAKELONG(32, -32));
	SendDlgItemMessage(hDlg, IDC_SPINLHEIGHT, UDM_SETPOS, 0,
		(int)(short)GetMyRegLongEx("", "LineHeight", 0, confNo));
}

/*------------------------------------------------
  Apply
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	DWORD dw;
	char s[80];

	//背景色の保存
	SetMyRegLongEx("", "UseBackColor",
		IsDlgButtonChecked(hDlg, IDC_CHKCOLOR), confNo);
	dw = (DWORD)CBGetItemData(hDlg, IDC_COLBACK, CBGetCurSel(hDlg, IDC_COLBACK));
	SetMyRegLongEx("", "BackColor", dw, confNo);

	SetMyRegLongEx("", "UseBackColor2",
		IsDlgButtonChecked(hDlg, IDC_CHKCOLOR2), confNo);
	dw = (DWORD)CBGetItemData(hDlg, IDC_COLBACK2, CBGetCurSel(hDlg, IDC_COLBACK2));
	SetMyRegLongEx("", "BackColor2", dw, confNo);
	SetMyRegLongEx("", "GradDir", IsDlgButtonChecked(hDlg, IDC_CHKCOLORV), confNo);

	//文字色の保存
	dw = (DWORD)CBGetItemData(hDlg, IDC_COLFORE, CBGetCurSel(hDlg, IDC_COLFORE));
	SetMyRegLongEx("", "ForeColor", dw, confNo);

	//影の保存
	SetMyRegLongEx("", "UseClockShadow",
		IsDlgButtonChecked(hDlg, IDC_CHKCLKSHADOW), confNo);
	SetMyRegLongEx("", "UseClockBORDER",
		IsDlgButtonChecked(hDlg, IDC_CHKCLKBORDER), confNo);
	dw = (DWORD)CBGetItemData(hDlg, IDC_COLCLKSHADOW, CBGetCurSel(hDlg, IDC_COLCLKSHADOW));
	SetMyRegLongEx("", "ShadowColor", dw, confNo);
	SetMyRegLongEx("", "ClockShadowRange",
		SendDlgItemMessage(hDlg, IDC_SPINCLKSHADOW, UDM_GETPOS, 0, 0), confNo);

	//フォント名の保存
	CBGetLBText(hDlg, IDC_FONT, CBGetCurSel(hDlg, IDC_FONT), s);
	SetMyRegStrEx("", "Font", s, confNo);

	//フォントサイズの保存
	//5以下のサイズは許可しない(数値でないものも含まれる)
	dw = GetDlgItemInt(hDlg, IDC_FONTSIZE, NULL, FALSE);
	if(dw < 5)
	{
		dw = 9;
		SetDlgItemInt(hDlg, IDC_FONTSIZE, dw, FALSE);
	}
	SetMyRegLongEx("", "FontSize", dw, confNo);

	//テキスト位置の保存
	SetMyRegLongEx("", "TextPos", CBGetCurSel(hDlg, IDC_TEXTPOS), confNo);

	//「Bold」「Italic」の保存
	SetMyRegLongEx("", "Bold", IsDlgButtonChecked(hDlg, IDC_BOLD), confNo);
	SetMyRegLongEx("", "Italic", IsDlgButtonChecked(hDlg, IDC_ITALIC), confNo);

	SetMyRegLongEx("", "ClockHeight",
		SendDlgItemMessage(hDlg, IDC_SPINCHEIGHT, UDM_GETPOS, 0, 0), confNo);
	SetMyRegLongEx("", "ClockWidth",
		SendDlgItemMessage(hDlg, IDC_SPINCWIDTH, UDM_GETPOS, 0, 0), confNo);
	SetMyRegLongEx("", "VertPos",
		SendDlgItemMessage(hDlg, IDC_SPINVPOS, UDM_GETPOS, 0, 0), confNo);
	SetMyRegLongEx("", "LineHeight",
		SendDlgItemMessage(hDlg, IDC_SPINLHEIGHT, UDM_GETPOS, 0, 0), confNo);
}

/*------------------------------------------------
  enable/disable to use "background 2"
--------------------------------------------------*/
void OnCheckColor2(HWND hDlg)
{
	BOOL b;
	b = IsDlgButtonChecked(hDlg, IDC_CHKCOLOR);
	if (b == FALSE)
	{
		EnableDlgItem(hDlg, IDC_COLBACK, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLBACK, FALSE);
		EnableDlgItem(hDlg, IDC_CHKCOLOR2, FALSE);
		EnableDlgItem(hDlg, IDC_COLBACK2, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLBACK2, FALSE);
		EnableDlgItem(hDlg, IDC_CHKCOLORV, FALSE);
	}
	else
	{
		EnableDlgItem(hDlg, IDC_COLBACK, TRUE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLBACK, TRUE);

		if (UseGrad == TRUE)
		{
			EnableDlgItem(hDlg, IDC_CHKCOLOR2, TRUE);
			b = IsDlgButtonChecked(hDlg, IDC_CHKCOLOR2);
			EnableDlgItem(hDlg, IDC_COLBACK2, b);
			EnableDlgItem(hDlg, IDC_CHOOSECOLBACK2, b);
			EnableDlgItem(hDlg, IDC_CHKCOLORV, b);
		}
		else
		{
			EnableDlgItem(hDlg, IDC_CHKCOLOR2, FALSE);
			EnableDlgItem(hDlg, IDC_COLBACK2, FALSE);
			EnableDlgItem(hDlg, IDC_CHOOSECOLBACK2, FALSE);
			EnableDlgItem(hDlg, IDC_CHKCOLORV, FALSE);
		}
	}
	SendPSChanged(hDlg);
}

/*------------------------------------------------
  enable/disable to use "shadow"
--------------------------------------------------*/
void OnCheckShadow(HWND hDlg, int id)
{
	BOOL b;
	if (id == 0)
	{
		if (id == 0 && IsDlgButtonChecked(hDlg, IDC_CHKCLKSHADOW))
			id = IDC_CHKCLKSHADOW;
		else if (id == 0 && IsDlgButtonChecked(hDlg, IDC_CHKCLKBORDER))
			id = IDC_CHKCLKBORDER;
	}

	if (id == IDC_CHKCLKSHADOW)
	{
		b = IsDlgButtonChecked(hDlg, IDC_CHKCLKSHADOW);
		CheckDlgButton(hDlg, IDC_CHKCLKBORDER,FALSE);
		EnableDlgItem(hDlg, IDC_COLCLKSHADOW, b);
		EnableDlgItem(hDlg, IDC_CHOOSECOLCLKSHADOW, b);
		EnableDlgItem(hDlg, IDC_STATICSHADOW, b);
		EnableDlgItem(hDlg, IDC_CLKSHADOWRANGE, b);
		EnableDlgItem(hDlg, IDC_SPINCLKSHADOW, b);
	}
	else if (id == IDC_CHKCLKBORDER)
	{
		b = IsDlgButtonChecked(hDlg, IDC_CHKCLKBORDER);
		CheckDlgButton(hDlg, IDC_CHKCLKSHADOW,FALSE);
		EnableDlgItem(hDlg, IDC_COLCLKSHADOW, b);
		EnableDlgItem(hDlg, IDC_CHOOSECOLCLKSHADOW, b);
		EnableDlgItem(hDlg, IDC_STATICSHADOW, FALSE);
		EnableDlgItem(hDlg, IDC_CLKSHADOWRANGE, FALSE);
		EnableDlgItem(hDlg, IDC_SPINCLKSHADOW, FALSE);
	}
	else
	{
		EnableDlgItem(hDlg, IDC_COLCLKSHADOW, FALSE);
		EnableDlgItem(hDlg, IDC_CHOOSECOLCLKSHADOW, FALSE);
		EnableDlgItem(hDlg, IDC_STATICSHADOW, FALSE);
		EnableDlgItem(hDlg, IDC_CLKSHADOWRANGE, FALSE);
		EnableDlgItem(hDlg, IDC_SPINCLKSHADOW, FALSE);
	}
	SendPSChanged(hDlg);
}

// 参考：Visual C++ 4.x のサンプルWORDPADのFORMATBA.CPP

BOOL CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* lpntm, int FontType, LPARAM hCombo);
BOOL CALLBACK EnumSizeProcEx(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* lpntm, int FontType, LPARAM hCombo);
int nFontSizes[] =
	{8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
int logpixelsy;

/*------------------------------------------------
   Initialization of "Font" combo box
--------------------------------------------------*/
void InitComboFont(HWND hDlg)
{
	HDC hdc;
	LOGFONT lf;
	HWND hcombo;
	char s[80];
	int i;

	hdc = GetDC(NULL);

	// Enumerate fonts and set in the combo box
	memset(&lf, 0, sizeof(LOGFONT));
	hcombo = GetDlgItem(hDlg, IDC_FONT);
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

	GetMyRegStrEx("", "Font", s, 80, "", confNo);
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
	i = CBFindStringExact(hDlg, IDC_FONT, s);
	if(i == LB_ERR) i = 0;
	CBSetCurSel(hDlg, IDC_FONT, i);

	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_FONT, 12);
}

/*------------------------------------------------
　「フォントサイズ」コンボボックスの設定
--------------------------------------------------*/
void SetComboFontSize(HWND hDlg, BOOL bInit)
{
	HDC hdc;
	char s[160];
	DWORD size;
	LOGFONT lf;

	//以前のsizeを保存
	if(bInit) // WM_INITDIALOGのとき
	{
		size = GetMyRegLongEx("", "FontSize", 9, confNo);
		if(size == 0) size = 9;
	}
	else   // IDC_FONTが変更されたとき
	{
		size = GetDlgItemInt(hDlg, IDC_FONTSIZE, NULL, FALSE);
	}

	CBResetContent(hDlg, IDC_FONTSIZE);

	hdc = GetDC(NULL);
	logpixelsy = GetDeviceCaps(hdc, LOGPIXELSY);

	// s = フォント名
	CBGetLBText(hDlg, IDC_FONT, CBGetCurSel(hDlg, IDC_FONT), (LPARAM)s);

	//フォントのサイズを列挙してコンボボックスに入れる
	memset(&lf, 0, sizeof(LOGFONT));
	strcpy(lf.lfFaceName, s);
	lf.lfCharSet = (BYTE)CBGetItemData(hDlg, IDC_FONT, CBGetCurSel(hDlg, IDC_FONT));
	EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumSizeProcEx,
		(LPARAM)GetDlgItem(hDlg, IDC_FONTSIZE), 0);

	//EnumFontFamilies(hdc, s,
	//	(FONTENUMPROC)EnumSizeProc, (LPARAM)GetDlgItem(hDlg, IDC_FONTSIZE));

	ReleaseDC(NULL, hdc);

	// sizeをセット
	SetDlgItemInt(hDlg, IDC_FONTSIZE, size, FALSE);

	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_FONTSIZE, 5);
}

/*------------------------------------------------
  Callback function for enumerating fonts.
  To set a font name in the combo box.
--------------------------------------------------*/
BOOL CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* lpntm, int FontType, LPARAM hCombo)
{
	UNREFERENCED_PARAMETER(lpntm);
	UNREFERENCED_PARAMETER(FontType);

	// if(FontType & RASTER_FONTTYPE) return 1;
	if(pelf->elfLogFont.lfFaceName[0] != '@' &&
		SendMessage((HWND)hCombo, CB_FINDSTRINGEXACT, 0,
			(LPARAM)pelf->elfLogFont.lfFaceName) == LB_ERR)
	{
		int index;
		index = SendMessage((HWND)hCombo, CB_ADDSTRING, 0, (LPARAM)pelf->elfLogFont.lfFaceName);
		if(index >= 0)
			SendMessage((HWND)hCombo, CB_SETITEMDATA,
				index, (LPARAM)pelf->elfLogFont.lfCharSet);
	}
	return 1;
}

/*------------------------------------------------
　フォントの列挙コールバック
　コンボボックスにフォントサイズを入れる
--------------------------------------------------*/
BOOL CALLBACK EnumSizeProcEx(ENUMLOGFONTEX* pelf,
	NEWTEXTMETRICEX* lpntm, int FontType, LPARAM hCombo)
{
	char s[80];
	int num, i, count;

	UNREFERENCED_PARAMETER(pelf);
	//トゥルータイプフォントまたは、
	//トゥルータイプでもラスタフォントでもない場合
	if((FontType & TRUETYPE_FONTTYPE) ||
		!( (FontType & TRUETYPE_FONTTYPE) || (FontType & RASTER_FONTTYPE) ))
	{
		// nFontSizesの数字をそのまま入れる
		for (i = 0; i < 16; i++)
		{
			wsprintf(s, "%d", nFontSizes[i]);
			SendMessage((HWND)hCombo, CB_ADDSTRING, 0, (LPARAM)s);
		}
		return FALSE;
	}

	//それ以外の場合、１つ１つ数字を入れていく
	num = (lpntm->ntmTm.tmHeight - lpntm->ntmTm.tmInternalLeading) * 72 / logpixelsy;
	count = SendMessage((HWND)hCombo, CB_GETCOUNT, 0, 0);
	for(i = 0; i < count; i++)
	{
		SendMessage((HWND)hCombo, CB_GETLBTEXT, i, (LPARAM)s);
		if(num == atoi(s)) return TRUE;
		else if(num < atoi(s))
		{
			wsprintf(s, "%d", num);
			SendMessage((HWND)hCombo, CB_INSERTSTRING, i, (LPARAM)s);
			return TRUE;
		}
	}
	wsprintf(s, "%d", num);
	SendMessage((HWND)hCombo, CB_ADDSTRING, 0, (LPARAM)s);
	return TRUE;
}

