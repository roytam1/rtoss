/*-------------------------------------------
  pagedesktop.c
  "desktop" page of properties
  Kazubon 1997-1999
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnCheckDesktopIconColor(HWND hDlg);

extern BOOL g_bApplyTaskbar; // propsheet.c

extern void SetDesktopIconTextBk(BOOL bNoTrans); // main.c

static COMBOCOLOR combocolor[1] = {
	{ IDC_COLDESKTOPICON, "DesktopTextColor", RGB(255, 255, 255) }
};

__inline void SendPSChanged(HWND hDlg)
{
	g_bApplyTaskbar = TRUE;
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
}

/*------------------------------------------------
   dialog procedure of this page
--------------------------------------------------*/
BOOL CALLBACK PageDesktopProc(HWND hDlg, UINT message,
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
			case IDC_TRANSDESKTOPICONBK:
			case IDC_DESKTOPICONLIST:
			case IDC_RADDESKTOPICONNORMAL:
			case IDC_RADDESKTOPICONLIST:
			case IDC_RADDESKTOPICONREPORT:
				SendPSChanged(hDlg);
				break;
			case IDC_DESKTOPICONCOLOR:
				OnCheckDesktopIconColor(hDlg);
				break;
			case IDC_COLDESKTOPICON:
				if(code == CBN_SELCHANGE)
				{
					SendPSChanged(hDlg);
				}
				break;
			case IDC_CHOOSECOLDESKTOPICON:
				OnChooseColor(hDlg, id, 16);
				SendPSChanged(hDlg);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 9); break;
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
	HDC hdc;

	InitComboColor(hDlg, 1, combocolor, 16, FALSE);
	hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	if(GetDeviceCaps(hdc, BITSPIXEL) <= 8)
	{
		EnableDlgItem(hDlg, IDC_CHOOSECOLDESKTOPICON, FALSE);
	}
	DeleteDC(hdc);

	CheckDlgButton(hDlg, IDC_TRANSDESKTOPICONBK,
		GetMyRegLong(NULL, "DesktopTextBk", FALSE));

	CheckDlgButton(hDlg, IDC_DESKTOPICONLIST,
		1 == GetMyRegLong(NULL, "DesktopIconList", FALSE));

	CheckDlgButton(hDlg, IDC_DESKTOPICONCOLOR,
		GetMyRegLong(NULL, "UseDesktopTextColor", FALSE));

	switch (GetMyRegLong(NULL, "DesktopIconList", FALSE))
	{
	case 0:
		CheckDlgButton(hDlg, IDC_RADDESKTOPICONNORMAL, BST_CHECKED);    break;
	case 1:
		CheckDlgButton(hDlg, IDC_RADDESKTOPICONLIST, BST_CHECKED);      break;
	case 2:
		CheckDlgButton(hDlg, IDC_RADDESKTOPICONREPORT, BST_CHECKED);    break;
	}

	OnCheckDesktopIconColor(hDlg);
}

/*------------------------------------------------
  apply - save settings
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	DWORD dw;

	SetMyRegLong(NULL, "DesktopTextBk",
		IsDlgButtonChecked(hDlg, IDC_TRANSDESKTOPICONBK));

	// edit 2008/02/17
	//SetMyRegLong(NULL, "DesktopIconList",
	//	IsDlgButtonChecked(hDlg, IDC_DESKTOPICONLIST));
	dw = (IsDlgButtonChecked(hDlg, IDC_RADDESKTOPICONLIST))   ? 1 :
			 (IsDlgButtonChecked(hDlg, IDC_RADDESKTOPICONREPORT)) ? 2 : 0;
	SetMyRegLong(NULL, "DesktopIconList", dw);

	SetMyRegLong("", "UseDesktopTextColor",
		IsDlgButtonChecked(hDlg, IDC_DESKTOPICONCOLOR));
	dw = CBGetItemData(hDlg, IDC_COLDESKTOPICON, CBGetCurSel(hDlg, IDC_COLDESKTOPICON));
	SetMyRegLong("", "DesktopTextColor", dw);

	SetDesktopIconTextBk(TRUE);
}

/*------------------------------------------------
  enable/disable to use "background 2"
--------------------------------------------------*/
void OnCheckDesktopIconColor(HWND hDlg)
{
	BOOL b;
	b = IsDlgButtonChecked(hDlg, IDC_DESKTOPICONCOLOR);
	EnableDlgItem(hDlg, IDC_COLDESKTOPICON, b);
	EnableDlgItem(hDlg, IDC_CHOOSECOLDESKTOPICON, b);

	SendPSChanged(hDlg);
};

