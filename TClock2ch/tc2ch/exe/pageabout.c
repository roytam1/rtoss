/*-------------------------------------------
  pageabout.c
     「バージョン情報」
     KAZUBON 1997-1998
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnBrowseFile(HWND hDlg, WORD id);
static void OnLinkClicked(HWND hDlg, UINT id);

static HFONT hfontLink;  //リンクラベルフォント
// リンクラベル用サブクラスプロシージャ
static WNDPROC oldLabProc = NULL;
LRESULT CALLBACK LabLinkProc(HWND, UINT, WPARAM, LPARAM);
static HCURSOR hCurHand = NULL;

#define SendPSChanged(hDlg) SendMessage(GetParent(hDlg),PSM_CHANGED,(WPARAM)(hDlg),0)

/*------------------------------------------------
　「バージョン情報」ページ用ダイアログプロシージャ
--------------------------------------------------*/

BOOL CALLBACK PageAboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			OnInit(hDlg);
			return TRUE;
		case WM_CTLCOLORSTATIC:
		{
			int id; HDC hdc;
			hdc = (HDC)wParam;
			id = GetDlgCtrlID((HWND)lParam);
			if(id == IDC_MAILTO || id == IDC_HOMEPAGE)
			{
				SetTextColor(hdc, RGB(0,0,255));
				SetBkMode(hdc, TRANSPARENT);
				return (int)GetStockObject(NULL_BRUSH);
			}
			break;
		}
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
			case IDC_MAILTO:
			case IDC_HOMEPAGE:
				if(code == STN_CLICKED)
				{
					OnLinkClicked(hDlg, id);
				}
				break;
			case IDC_HELPURL:
			case IDC_HELP2CH:
			case IDC_LANGDLL:
				if(code == EN_CHANGE)
				{
					SendPSChanged(hDlg);
				}
				break;
			case IDC_BROWSEHELP:
			case IDC_BROWSELANGDLL:
			case IDC_BROWSEHELP2CH:
				OnBrowseFile(hDlg, id);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 12); break;
			}
			return TRUE;
		case WM_DESTROY:
			DeleteObject(hfontLink);
			break;
	}
	return FALSE;
}

/*------------------------------------------------
  initialize
--------------------------------------------------*/
static void OnInit(HWND hDlg)
{
	LOGFONT logfont;
	char s[MAX_PATH];
	HFONT hfont;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
	{
		SendDlgItemMessage(hDlg, IDC_HELPURL,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_LANGDLL,
			WM_SETFONT, (WPARAM)hfont, 0);
	}

	SendDlgItemMessage(hDlg, IDC_ABOUTICON, STM_SETIMAGE,
		IMAGE_ICON, (LPARAM)g_hIconTClock);

	hfontLink = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
	GetObject(hfontLink, sizeof(LOGFONT), &logfont);
	logfont.lfUnderline = 1;
	hfontLink = CreateFontIndirect(&logfont);
	SendDlgItemMessage(hDlg, IDC_MAILTO, WM_SETFONT, (WPARAM)hfontLink, 0);
	SendDlgItemMessage(hDlg, IDC_HOMEPAGE, WM_SETFONT, (WPARAM)hfontLink, 0);
	if(hCurHand == NULL)
		hCurHand = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_HAND));
	oldLabProc = (WNDPROC)GetWindowLong(GetDlgItem(hDlg, IDC_MAILTO),
		GWL_WNDPROC);
	SetWindowLong(GetDlgItem(hDlg, IDC_MAILTO),
		GWL_WNDPROC, (LONG)LabLinkProc);
	SetWindowLong(GetDlgItem(hDlg, IDC_HOMEPAGE),
		GWL_WNDPROC, (LONG)LabLinkProc);

	//hbrback = CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	GetMyRegStr("", "HelpURL", s, MAX_PATH, MyString(IDS_HELPURL));
	if (!s[0])
		lstrcpyn(s, MyString(IDS_HELPURL), MAX_PATH);
	SetDlgItemText(hDlg, IDC_HELPURL, s);

	GetMyRegStr("", "2chHelpURL", s, MAX_PATH, MyString(IDS_HELP2CH));
	if (!s[0])
		lstrcpyn(s, MyString(IDS_HELP2CH), MAX_PATH);
	SetDlgItemText(hDlg, IDC_HELP2CH, s);

	GetMyRegStr("", "LangDLL", s, MAX_PATH, "");
	SetDlgItemText(hDlg, IDC_LANGDLL, s);
}

/*------------------------------------------------
  "Apply" button
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	char s[MAX_PATH], s2[MAX_PATH];

	GetDlgItemText(hDlg, IDC_HELPURL, s, MAX_PATH);
	SetMyRegStr("", "HelpURL", s);

	GetDlgItemText(hDlg, IDC_HELP2CH, s, MAX_PATH);
	SetMyRegStr("", "2chHelpURL", s);

	GetMyRegStr("", "LangDLL", s2, MAX_PATH, "");
	GetDlgItemText(hDlg, IDC_LANGDLL, s, MAX_PATH);
	if(strcmp(s, s2) != 0)
		g_bApplyLangDLL = TRUE; // reload language dll
	SetMyRegStr("", "LangDLL", s);
}

/*------------------------------------------------
  clicked "..." button
--------------------------------------------------*/
void OnBrowseFile(HWND hDlg, WORD id)
{
	char filter[80], deffile[MAX_PATH], fname[MAX_PATH];

	filter[0] = filter[1] = 0;

	if(id == IDC_BROWSEHELP)
	{
		str0cat(filter, "HTML");
		str0cat(filter, "*.html;*.htm");
	}
	else if(id == IDC_BROWSEHELP2CH)
	{
		str0cat(filter, "HTML");
		str0cat(filter, "*.html;*.htm");
	}
	else if(id == IDC_BROWSELANGDLL)
	{
		str0cat(filter, MyString(IDS_LANGDLLFILE));
		str0cat(filter, "lang*.dll");
	}

	GetDlgItemText(hDlg, id - 1, deffile, MAX_PATH);

	if(!SelectMyFile(hDlg, filter, 0, deffile, fname)) // propsheet.c
		return;

	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
　リンク用ラベルがクリックされた
--------------------------------------------------*/
void OnLinkClicked(HWND hDlg, UINT id)
{
	char str[1024], *p;
	BOOL bOutlook = FALSE;

	if(id == IDC_MAILTO)
	{
		GetRegStr(HKEY_CLASSES_ROOT, "mailto\\shell\\open\\command", "",
			str, 1024, "");
		p = str;
		while(*p)
		{
			if(_strnicmp(p, "MSIMN.EXE", 9) == 0)
			{
				bOutlook = TRUE; break;
			}
			p++;
		}

		strcpy(str, "mailto:");
		if(bOutlook)
		{
			strcat(str, "Kazubon <");
			GetDlgItemText(hDlg, id, str + strlen(str), 80);
			strcat(str, ">?subject=About%20TClock");
		}
		else
			GetDlgItemText(hDlg, id, str + strlen(str), 80);
	}
	else GetDlgItemText(hDlg, id, str, 80);

	ShellExecute(hDlg, NULL, str, NULL, "", SW_SHOW);
}

/*------------------------------------------------
　リンク用ラベルのサブクラスプロシージャ
--------------------------------------------------*/
LRESULT CALLBACK LabLinkProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_SETCURSOR:
			SetCursor(hCurHand);
			return 1;
	}
	return CallWindowProc(oldLabProc, hwnd, message, wParam, lParam);
}

