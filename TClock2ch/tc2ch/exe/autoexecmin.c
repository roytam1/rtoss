/*-------------------------------------------
  autoexecmin.c
  dialog to set hours for autoexec
        Kazubon 1999  ÅüFDQ3TClock 2003
---------------------------------------------*/

#include "tclock.h"

INT_PTR CALLBACK AutoExecMinProc(HWND, UINT, WPARAM, LPARAM);
static void OnInit(HWND hDlg);
static void OnOK(HWND hDlg);
static void OnEveryMin(HWND hDlg);

static int retval1;
static int retval2;

/*------------------------------------------------
  show dialog box
--------------------------------------------------*/
void SetAutoExecMin(HWND hDlg, int nOld1, int nOld2, int* nNew1, int* nNew2)
{
	retval1 = nOld1; retval2 = nOld2;
	if(DialogBox(GetLangModule(), MAKEINTRESOURCE(IDD_AUTOEXECMIN),
		hDlg, AutoExecMinProc) == IDOK)
	{
		*nNew1 = retval1; *nNew2 = retval2;
	}
	else
	{
		*nNew1 = nOld1; *nNew2 = nOld2;
	}
	return;
}

/*------------------------------------------------
  dialog procedure
--------------------------------------------------*/
INT_PTR CALLBACK AutoExecMinProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
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
				case IDC_AUTOEXECMINALL:
					OnEveryMin(hDlg); break;
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
	for(i = 0; i < 30; i++)
	{
		if(hfont)
			SendDlgItemMessage(hDlg, IDC_AUTOEXECMIN0 + i,
				WM_SETFONT, (WPARAM)hfont, 0);

		if(retval1 & f)
			CheckDlgButton(hDlg, IDC_AUTOEXECMIN0 + i, TRUE);
		f = f << 1;
	}
	f = 1;
	for(i = 30; i < 60; i++)
	{
		if(hfont)
			SendDlgItemMessage(hDlg, IDC_AUTOEXECMIN0 + i,
				WM_SETFONT, (WPARAM)hfont, 0);

		if(retval2 & f)
			CheckDlgButton(hDlg, IDC_AUTOEXECMIN0 + i, TRUE);
		f = f << 1;
	}

	if((retval1 & 0x3fffffff) == 0x3fffffff && (retval2 & 0x3fffffff) == 0x3fffffff )
	{
		CheckDlgButton(hDlg, IDC_AUTOEXECMINALL, TRUE);
		OnEveryMin(hDlg);
	}
}

/*------------------------------------------------
  retreive setting
--------------------------------------------------*/
void OnOK(HWND hDlg)
{
	int i, f;

	f = 1; retval1 = 0;
	for(i = 0; i < 30; i++)
	{
		if(IsDlgButtonChecked(hDlg, IDC_AUTOEXECMIN0 + i))
			retval1 = retval1 | f;
		f = f << 1;
	}
	f = 1; retval2 = 0;
	for(i = 30; i < 60; i++)
	{
		if(IsDlgButtonChecked(hDlg, IDC_AUTOEXECMIN0 + i))
			retval2 = retval2 | f;
		f = f << 1;
	}
}

/*------------------------------------------------
   Every day
--------------------------------------------------*/
void OnEveryMin(HWND hDlg)
{
	int i;

	if(IsDlgButtonChecked(hDlg, IDC_AUTOEXECMINALL))
	{
		for(i = 0; i < 60; i++)
		{
			CheckDlgButton(hDlg, IDC_AUTOEXECMIN0 + i, TRUE);
			EnableDlgItem(hDlg, IDC_AUTOEXECMIN0+i, FALSE);
		}
	}
	else
	{
		for(i = 0; i < 60; i++)
		{
			CheckDlgButton(hDlg, IDC_AUTOEXECMIN0 + i, FALSE);
			EnableDlgItem(hDlg, IDC_AUTOEXECMIN0+i, TRUE);
		}
	}
}
