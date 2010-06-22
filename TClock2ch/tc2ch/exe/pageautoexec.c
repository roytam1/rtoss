/*-------------------------------------------
  pageautoexec.c
  "AutoExec" page of properties
  by FDQ3TClock
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnDestroy(HWND hDlg);
static void ReadAutoExecFromReg(PAUTOEXECSTRUCT pAS, int num);
static void SaveAutoExecToReg(PAUTOEXECSTRUCT pAS, int num);
static void GetAutoExecFromDlg(HWND hDlg, PAUTOEXECSTRUCT pAS);
static void SetAutoExecToDlg(HWND hDlg, PAUTOEXECSTRUCT pAS);
static void OnChangeAutoExec(HWND hDlg);
static void OnDropDownAutoExec(HWND hDlg);
static void OnHour(HWND hDlg);
static void OnMin(HWND hDlg);
static void OnDay(HWND hDlg);
static void OnAutoExec(HWND hDlg, WORD id);
static void OnSanshoAutoExec(HWND hDlg, WORD id);
static void OnDelAutoExec(HWND hDlg);
static void OnFileChange(HWND hDlg, WORD id);
static void OnTest(HWND hDlg, WORD id);

static int curAutoExec;
static BOOL bPlaying = FALSE;

#define SendPSChanged(hDlg) SendMessage(GetParent(hDlg),PSM_CHANGED,(WPARAM)(hDlg),0)

/*------------------------------------------------
  Dialog procedure
--------------------------------------------------*/
BOOL CALLBACK PageAutoExecProc(HWND hDlg, UINT message,
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
			// a combo-box to select alarm name
			case IDC_COMBOAUTOEXEC:
				if(code == CBN_SELCHANGE)
					OnChangeAutoExec(hDlg);
				else if(code == CBN_DROPDOWN)
					OnDropDownAutoExec(hDlg);
				break;
			// Day...
			case IDC_AUTOEXECHOUR:
				OnHour(hDlg);
				break;
			case IDC_AUTOEXECMIN:
				OnMin(hDlg);
				break;
			case IDC_AUTOEXECDAY:
				OnDay(hDlg);
				break;
			case IDC_AUTOEXEC:
				OnAutoExec(hDlg, id);
				break;
			// file name changed
			case IDC_FILEAUTOEXEC:
				if(code == EN_CHANGE)
				{
					OnFileChange(hDlg, id);
					SendPSChanged(hDlg);
				}
				break;
			// browse file
			case IDC_SANSHOAUTOEXEC:
				OnSanshoAutoExec(hDlg, id);
				OnFileChange(hDlg, (WORD)(id - 1));
				SendPSChanged(hDlg);
				break;
			// checked other checkboxes
			case IDC_ONBOOTEXEC:
				SendPSChanged(hDlg);
				break;
			// delete an alarm
			case IDC_DELAUTOEXEC:
				OnDelAutoExec(hDlg);
				break;
			// test sound
			case IDC_TESTAUTOEXEC:
				OnTest(hDlg, id);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 14); break;
			}
			return TRUE;
		case WM_DESTROY:
			OnDestroy(hDlg);
			if(bPlaying) StopFile(); bPlaying = FALSE;
			break;
		// playing sound ended
		case MM_MCINOTIFY:
		case MM_WOM_DONE:
			StopFile(); bPlaying = FALSE;
			SendDlgItemMessage(hDlg, IDC_TESTAUTOEXEC, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)g_hIconPlay);
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------
  initialize
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	int i, count, index;
	HFONT hfont;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
	{
		SendDlgItemMessage(hDlg, IDC_COMBOAUTOEXEC,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_FILEAUTOEXEC,
			WM_SETFONT, (WPARAM)hfont, 0);
	}

	index = CBAddString(hDlg, IDC_COMBOAUTOEXEC, (LPARAM)MyString(IDS_ADDALARM));
	CBSetItemData(hDlg, IDC_COMBOAUTOEXEC, index, 0);

	count = GetMyRegLong("", "AutoExecNum", 0);
	if(count < 1) count = 0;
	for(i = 0; i < count; i++)
	{
		PAUTOEXECSTRUCT pAS;
		pAS = malloc(sizeof(AUTOEXECSTRUCT));
		ReadAutoExecFromReg(pAS, i);
		index = CBAddString(hDlg, IDC_COMBOAUTOEXEC, (LPARAM)pAS->name);
		CBSetItemData(hDlg, IDC_COMBOAUTOEXEC, index, (LPARAM)pAS);
		if(i == 0)  SetAutoExecToDlg(hDlg, pAS);
	}
	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_COMBOAUTOEXEC, 7);
	if(count > 0)
	{
		CBSetCurSel(hDlg, IDC_COMBOAUTOEXEC, 1);
		curAutoExec = 1;
	}
	else
	{
		AUTOEXECSTRUCT as;
		CBSetCurSel(hDlg, IDC_COMBOAUTOEXEC, 0);
		curAutoExec = -1;
		memset(&as, 0, sizeof(as));
		as.hour = 12;
		as.days = 0x7f;
		SetAutoExecToDlg(hDlg, &as);
	}

	SendDlgItemMessage(hDlg, IDC_TESTAUTOEXEC, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconPlay);
	OnFileChange(hDlg, IDC_FILEAUTOEXEC);

	SendDlgItemMessage(hDlg, IDC_DELAUTOEXEC, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconDel);

	OnAutoExec(hDlg, IDC_AUTOEXEC);
	bPlaying = FALSE;
}

/*------------------------------------------------
   apply - save settings
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	int i, count, n_autoexec;
	PAUTOEXECSTRUCT pAS;

	n_autoexec = 0;

	if(curAutoExec < 0)
	{
		char name[40];
		GetDlgItemText(hDlg, IDC_COMBOAUTOEXEC, name, 40);
		if(name[0] && IsDlgButtonChecked(hDlg, IDC_AUTOEXEC))
		{
			pAS = malloc(sizeof(AUTOEXECSTRUCT));
			if(pAS)
			{
				int index;
				GetAutoExecFromDlg(hDlg, pAS);
				index = CBAddString(hDlg, IDC_COMBOAUTOEXEC, (LPARAM)pAS->name);
				CBSetItemData(hDlg, IDC_COMBOAUTOEXEC, index, (LPARAM)pAS);
				curAutoExec = index;
				CBSetCurSel(hDlg, IDC_COMBOAUTOEXEC, index);
				EnableDlgItem(hDlg, IDC_DELAUTOEXEC, TRUE);
			}
		}
	}
	else
	{
		pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
		if(pAS)
			GetAutoExecFromDlg(hDlg, pAS);
	}

	count = CBGetCount(hDlg, IDC_COMBOAUTOEXEC);
	for(i = 0; i < count; i++)
	{
		PAUTOEXECSTRUCT pAS;
		pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, i);
		if(pAS)
		{
			SaveAutoExecToReg(pAS, n_autoexec);
			n_autoexec++;
		}
	}
	for(i = n_autoexec; ; i++)
	{
		char subkey[20];
		wsprintf(subkey, "AutoExec%d", i + 1);
		if(GetMyRegLong(subkey, "Hour", -1) >= 0)
			DelMyRegKey(subkey);
		else break;
	}

	SetMyRegLong("", "AutoExecNum", n_autoexec);

	InitAlarm(); // alarm.c
}

/*------------------------------------------------
  free memories associated with combo box.
--------------------------------------------------*/
void OnDestroy(HWND hDlg)
{
	int i, count;

	count = CBGetCount(hDlg, IDC_COMBOAUTOEXEC);
	for(i = 0; i < count; i++)
	{
		PAUTOEXECSTRUCT pAS;
		pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, i);
		if(pAS) free(pAS);
	}
}

/*------------------------------------------------
  read settings of an alarm
--------------------------------------------------*/
void ReadAutoExecFromReg(PAUTOEXECSTRUCT pAS, int num)
{
	char subkey[20];

	wsprintf(subkey, "AutoExec%d", num + 1);

	GetMyRegStr(subkey, "Name", pAS->name, 40, "");
	pAS->bAutoExec = GetMyRegLong(subkey, "AutoExec", FALSE);
	pAS->hour = GetMyRegLong(subkey, "Hour", 0xffffff);
	pAS->minute1 = GetMyRegLong(subkey, "Minute1", 0);
	pAS->minute2 = GetMyRegLong(subkey, "Minute2", 0);
	GetMyRegStr(subkey, "File", pAS->fname, 1024, "");
	pAS->days = GetMyRegLong(subkey, "Days", 0x7f);
	pAS->bOnBoot = GetMyRegLong(subkey, "OnBoot", FALSE);

	if(pAS->name[0] == 0)
		wsprintf(pAS->name, "自動実行%d", num);
}

/*------------------------------------------------
  save settings of an alarm
--------------------------------------------------*/
void SaveAutoExecToReg(PAUTOEXECSTRUCT pAS, int num)
{
	char subkey[20];

	wsprintf(subkey, "AutoExec%d", num + 1);
	SetMyRegStr(subkey, "Name", pAS->name);
	SetMyRegLong(subkey, "AutoExec", pAS->bAutoExec);
	SetMyRegLong(subkey, "Hour", pAS->hour);
	SetMyRegLong(subkey, "Minute1", pAS->minute1);
	SetMyRegLong(subkey, "Minute2", pAS->minute2);
	SetMyRegStr(subkey, "File", pAS->fname);
	SetMyRegLong(subkey, "Days", pAS->days);
	SetMyRegLong(subkey, "OnBoot", pAS->bOnBoot);
}

/*------------------------------------------------
  get settings of an alarm from the page
--------------------------------------------------*/
void GetAutoExecFromDlg(HWND hDlg, PAUTOEXECSTRUCT pAS)
{
	GetDlgItemText(hDlg, IDC_COMBOAUTOEXEC, pAS->name, 40);
	pAS->bAutoExec = IsDlgButtonChecked(hDlg, IDC_AUTOEXEC);
	pAS->hour = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECHOUR, NULL, FALSE);
	pAS->minute1 = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN1, NULL, FALSE);
	pAS->minute2 = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN2, NULL, FALSE);
	GetDlgItemText(hDlg, IDC_FILEAUTOEXEC, pAS->fname, MAX_PATH);
	pAS->days = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECDAY, NULL, FALSE);
	pAS->bOnBoot = IsDlgButtonChecked(hDlg, IDC_ONBOOTEXEC);
}

/*------------------------------------------------
  set settings of an alarm to the page
--------------------------------------------------*/
void SetAutoExecToDlg(HWND hDlg, PAUTOEXECSTRUCT pAS)
{
	SetDlgItemText(hDlg, IDC_COMBOAUTOEXEC, pAS->name);
	CheckDlgButton(hDlg, IDC_AUTOEXEC, pAS->bAutoExec);
	SetDlgItemText(hDlg, IDC_FILEAUTOEXEC, pAS->fname);
	SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECDAY, pAS->days, FALSE);
	SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECHOUR, pAS->hour, FALSE);
	SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN1, pAS->minute1, FALSE);
	SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN2, pAS->minute2, FALSE);
	CheckDlgButton(hDlg, IDC_ONBOOTEXEC, pAS->bOnBoot);

	OnFileChange(hDlg, IDC_FILEAUTOEXEC);
}

/*------------------------------------------------
   selected an alarm name by combobox
--------------------------------------------------*/
void OnChangeAutoExec(HWND hDlg)
{
	PAUTOEXECSTRUCT pAS;
	int index;

	index = CBGetCurSel(hDlg, IDC_COMBOAUTOEXEC);
	if(curAutoExec >= 0 && index == curAutoExec) return;

	if(curAutoExec < 0)
	{
		char name[40];
		GetDlgItemText(hDlg, IDC_COMBOAUTOEXEC, name, 40);
		if(name[0] && IsDlgButtonChecked(hDlg, IDC_AUTOEXEC))
		{
			pAS = malloc(sizeof(AUTOEXECSTRUCT));
			if(pAS)
			{
				int index;
				GetAutoExecFromDlg(hDlg, pAS);
				index = CBAddString(hDlg, IDC_COMBOAUTOEXEC, (LPARAM)pAS->name);
				CBSetItemData(hDlg, IDC_COMBOAUTOEXEC, index, (LPARAM)pAS);
				curAutoExec = index;
				//リスト項目の表示数を指定
				AdjustDlgConboBoxDropDown(hDlg, IDC_COMBOAUTOEXEC, 7);
			}
		}
	}
	else
	{
		pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
		if(pAS) GetAutoExecFromDlg(hDlg, pAS);
	}

	pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, index);
	if(pAS)
	{
		SetAutoExecToDlg(hDlg, pAS);
		EnableDlgItem(hDlg, IDC_DELAUTOEXEC, TRUE);
		curAutoExec = index;
	}
	else
	{
		AUTOEXECSTRUCT as;
		memset(&as, 0, sizeof(as));
		as.hour = 0xffffff;
		as.days = 0x7f;
		SetAutoExecToDlg(hDlg, &as);
		EnableDlgItem(hDlg, IDC_DELAUTOEXEC, FALSE);
		curAutoExec = -1;
	}
}

/*------------------------------------------------
  combo box is about to be made visible
--------------------------------------------------*/
void OnDropDownAutoExec(HWND hDlg)
{
	PAUTOEXECSTRUCT pAS;
	char name[40];
	int index;

	if(curAutoExec < 0) return;
	pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
	if(pAS == 0) return;
	GetDlgItemText(hDlg, IDC_COMBOAUTOEXEC, name, 40);
	if(strcmp(name, pAS->name) != 0)
	{
		strcpy(pAS->name, name);
		CBDeleteString(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
		index = CBInsertString(hDlg, IDC_COMBOAUTOEXEC, curAutoExec, name);
		CBSetItemData(hDlg, IDC_COMBOAUTOEXEC, index, (LPARAM)pAS);
		CBSetCurSel(hDlg, IDC_COMBOAUTOEXEC, index);
		curAutoExec = index;
	}
}

/*------------------------------------------------
  "Hour..."
--------------------------------------------------*/
void OnHour(HWND hDlg)
{
	int nOld, nNew;

	nOld = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECHOUR, NULL, FALSE);
	nNew = SetAutoExecHour(hDlg, nOld);
	if(nNew != nOld)
	{
		SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECHOUR, nNew, FALSE);
		SendPSChanged(hDlg);
	}
}

/*------------------------------------------------
  "Min..."
--------------------------------------------------*/
void OnMin(HWND hDlg)
{
	int nOld1, nOld2, nNew1 = 0, nNew2 = 0;

	nOld1 = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN1, NULL, FALSE);
	nOld2 = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN2, NULL, FALSE);
	SetAutoExecMin(hDlg, nOld1, nOld2, &nNew1, &nNew2);

	if(nNew1 != nOld1 || nNew2 != nOld2)
	{
		SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN1, nNew1, FALSE);
		SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECMIN2, nNew2, FALSE);
		SendPSChanged(hDlg);
	}
}

/*------------------------------------------------
  "Day..."
--------------------------------------------------*/
void OnDay(HWND hDlg)
{
	int nOld, nNew;

	nOld = GetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECDAY, NULL, FALSE);
	nNew = SetAutoExecDay(hDlg, nOld);
	if(nNew != nOld)
	{
		SetDlgItemInt(hDlg, IDC_HIDDENAUTOEXECDAY, nNew, FALSE);
		SendPSChanged(hDlg);
	}
}

/*------------------------------------------------
  browse sound file
--------------------------------------------------*/
void OnSanshoAutoExec(HWND hDlg, WORD id)
{
	char deffile[MAX_PATH], fname[MAX_PATH];

	GetDlgItemText(hDlg, id - 1, deffile, MAX_PATH);

	if(!BrowseSoundFile(hDlg, deffile, fname, id)) // soundselect.c
		return;

	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
}

/*------------------------------------------------
  checked "acrive"
--------------------------------------------------*/
void OnAutoExec(HWND hDlg, WORD id)
{
	int s, e, i;
	BOOL b;

	s = IDC_AUTOEXECHOUR; e = IDC_HIDDENAUTOEXECMIN2;

	b = IsDlgButtonChecked(hDlg, id);
	for(i = s; i <= e; i++)	EnableDlgItem(hDlg, i, b);

	OnFileChange(hDlg, IDC_FILEAUTOEXEC);

	if(b)
	{
		char name[40];
		GetDlgItemText(hDlg, IDC_COMBOAUTOEXEC, name, 40);
		if(strcmp(name, MyString(IDS_ADDALARM)) == 0)
			SetDlgItemText(hDlg, IDC_COMBOAUTOEXEC, "");
	}

	SendPSChanged(hDlg);
}

/*------------------------------------------------
  delete an alarm
--------------------------------------------------*/
void OnDelAutoExec(HWND hDlg)
{
	PAUTOEXECSTRUCT pAS;

	if(curAutoExec < 0) return;

	pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
	if(pAS)
	{
		PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
		CBDeleteString(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
		free(pAS);
		if(curAutoExec > 0) curAutoExec--;
		CBSetCurSel(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
		pAS = (PAUTOEXECSTRUCT)CBGetItemData(hDlg, IDC_COMBOAUTOEXEC, curAutoExec);
		if(pAS) SetAutoExecToDlg(hDlg, pAS);
		else
		{
			AUTOEXECSTRUCT as;
			memset(&as, 0, sizeof(as));
			as.hour = 0xffffff;
			as.days = 0x7f;
			SetAutoExecToDlg(hDlg, &as);
			EnableDlgItem(hDlg, IDC_DELAUTOEXEC, FALSE);
			curAutoExec = -1;
		}
	}
}

/*------------------------------------------------
   file name changed - enable/disable controls
--------------------------------------------------*/
void OnFileChange(HWND hDlg, WORD id)
{
	char fname[MAX_PATH];
	BOOL b = FALSE;

	GetDlgItemText(hDlg, id, fname, MAX_PATH);
	if(IsWindowEnabled(GetDlgItem(hDlg, id)))
		b = TRUE;
	EnableDlgItem(hDlg, id + 3, b);
}

/*------------------------------------------------
  test sound
--------------------------------------------------*/
void OnTest(HWND hDlg, WORD id)
{
	char fname[MAX_PATH];

	GetDlgItemText(hDlg, id - 3, fname, MAX_PATH);
	if(fname[0] == 0) return;

	if((HICON)SendDlgItemMessage(hDlg, id, BM_GETIMAGE, IMAGE_ICON, 0)
		== g_hIconPlay)
	{
		if(PlayFile(hDlg, fname, 0))
		{
			SendDlgItemMessage(hDlg, id, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)g_hIconStop);
			InvalidateRect(GetDlgItem(hDlg, id), NULL, FALSE);
			bPlaying = TRUE;
		}
	}
	else
	{
		StopFile();
		bPlaying = FALSE;
	}
}

