/*-------------------------------------------
  autoexechour.c
  dialog to set hours for autoexec
        Kazubon 1999  ÅüFDQ3TClock 2003
---------------------------------------------*/

#include "tclock.h"

BOOL CALLBACK AutoExecHourProc(HWND, UINT, WPARAM, LPARAM);
static void OnInit(HWND hDlg);
static void OnOK(HWND hDlg);
static void OnEveryHour(HWND hDlg);

static int retval;

/*------------------------------------------------
  show dialog box
--------------------------------------------------*/
int SetAutoExecHour(HWND hDlg, int n)
{
	retval = n;
	if(DialogBox(GetLangModule(), MAKEINTRESOURCE(IDD_AUTOEXECHOUR),
		hDlg, AutoExecHourProc) == IDOK)
		return retval;
	return n;
}

/*------------------------------------------------
  dialog procedure
--------------------------------------------------*/
BOOL CALLBACK AutoExecHourProc(HWND hDlg, UINT message,
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
				case IDC_AUTOEXECHOURALL:
					OnEveryHour(hDlg); break;
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
	HFONT hfont;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	f = 1;
	for(i = 0; i < 24; i++)
	{
		if(hfont)
			SendDlgItemMessage(hDlg, IDC_AUTOEXECHOUR0 + i,
				WM_SETFONT, (WPARAM)hfont, 0);

		if(retval & f)
			CheckDlgButton(hDlg, IDC_AUTOEXECHOUR0 + i, TRUE);
		f = f << 1;
	}

	if((retval & 0xffffff) == 0xffffff)
	{
		CheckDlgButton(hDlg, IDC_AUTOEXECHOURALL, TRUE);
		OnEveryHour(hDlg);
	}
}

/*------------------------------------------------
  retreive setting
--------------------------------------------------*/
void OnOK(HWND hDlg)
{
	int i, f;

	f = 1; retval = 0;
	for(i = 0; i < 24; i++)
	{
		if(IsDlgButtonChecked(hDlg, IDC_AUTOEXECHOUR0 + i))
			retval = retval | f;
		f = f << 1;
	}
}

/*------------------------------------------------
   Every day
--------------------------------------------------*/
void OnEveryHour(HWND hDlg)
{
	int i;

	if(IsDlgButtonChecked(hDlg, IDC_AUTOEXECHOURALL))
	{
		for(i = 0; i < 24; i++)
		{
			CheckDlgButton(hDlg, IDC_AUTOEXECHOUR0 + i, TRUE);
			EnableDlgItem(hDlg, IDC_AUTOEXECHOUR0+i, FALSE);
		}
	}
	else
	{
		for(i = 0; i < 24; i++)
		{
			CheckDlgButton(hDlg, IDC_AUTOEXECHOUR0 + i, FALSE);
			EnableDlgItem(hDlg, IDC_AUTOEXECHOUR0+i, TRUE);
		}
	}
}
