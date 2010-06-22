/*-------------------------------------------
  autoexecday.c
  dialog to set days for autoexec
        Kazubon 1999  ÅüFDQ3TClock 2003
---------------------------------------------*/

#include "tclock.h"

BOOL CALLBACK AutoExecDayProc(HWND, UINT, WPARAM, LPARAM);
static void OnInit(HWND hDlg);
static void OnOK(HWND hDlg);
static void OnEveryDay(HWND hDlg);

static int retval;

/*------------------------------------------------
  show dialog box
--------------------------------------------------*/
int SetAutoExecDay(HWND hDlg, int n)
{
	retval = n;
	if(DialogBox(GetLangModule(), MAKEINTRESOURCE(IDD_AUTOEXECDAY),
		hDlg, AutoExecDayProc) == IDOK)
		return retval;
	return n;
}

/*------------------------------------------------
  dialog procedure
--------------------------------------------------*/
BOOL CALLBACK AutoExecDayProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			OnInit(hDlg);
			return TRUE;
		case WM_COMMAND:
		{
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			switch(id)
			{
				case IDC_AUTOEXECDAY0:
					OnEveryDay(hDlg); break;
				case IDOK: OnOK(hDlg);
				case IDCANCEL: EndDialog(hDlg, id); break;
			}
			return TRUE;
		}
	}
	return FALSE;
}

/*------------------------------------------------
  initialize
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	int i, f;
	char s[80];
	HFONT hfont;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	f = 1;
	for(i = 0; i < 7; i++)
	{
		if(hfont)
			SendDlgItemMessage(hDlg, IDC_AUTOEXECDAY1 + i,
				WM_SETFONT, (WPARAM)hfont, 0);

		GetLocaleInfoWA(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			LOCALE_SDAYNAME1+i, s, 80);
		SetDlgItemText(hDlg, IDC_AUTOEXECDAY1 + i, s);
		if(retval & f)
			CheckDlgButton(hDlg, IDC_AUTOEXECDAY1 + i, TRUE);
		f = f << 1;
	}

	if((retval & 0x7f) == 0x7f)
	{
		CheckDlgButton(hDlg, IDC_AUTOEXECDAY0, TRUE);
		OnEveryDay(hDlg);
	}
}

/*------------------------------------------------
  retreive setting
--------------------------------------------------*/
void OnOK(HWND hDlg)
{
	int i, f;

	f = 1; retval = 0;
	for(i = 0; i < 7; i++)
	{
		if(IsDlgButtonChecked(hDlg, IDC_AUTOEXECDAY1 + i))
			retval = retval | f;
		f = f << 1;
	}
}

/*------------------------------------------------
   Every day
--------------------------------------------------*/
void OnEveryDay(HWND hDlg)
{
	int i;

	if(IsDlgButtonChecked(hDlg, IDC_AUTOEXECDAY0))
	{
		for(i = 0; i < 7; i++)
		{
			CheckDlgButton(hDlg, IDC_AUTOEXECDAY1 + i, TRUE);
			EnableDlgItem(hDlg, IDC_AUTOEXECDAY1+i, FALSE);
		}
	}
	else
	{
		for(i = 0; i < 7; i++)
		{
			CheckDlgButton(hDlg, IDC_AUTOEXECDAY1 + i, FALSE);
			EnableDlgItem(hDlg, IDC_AUTOEXECDAY1+i, TRUE);
		}
	}
}
