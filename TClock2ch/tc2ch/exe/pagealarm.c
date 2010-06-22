/*-------------------------------------------
  pagealarm.c
  "Alarm" page of properties
  KAZUBON 1997-2001
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnDestroy(HWND hDlg);
static void ReadAlarmFromReg(PALARMSTRUCT pAS, int num);
static void SaveAlarmToReg(PALARMSTRUCT pAS, int num);
static void GetAlarmFromDlg(HWND hDlg, PALARMSTRUCT pAS);
static void SetAlarmToDlg(HWND hDlg, PALARMSTRUCT pAS);
static void OnChangeAlarm(HWND hDlg);
static void OnDropDownAlarm(HWND hDlg);
static void OnDay(HWND hDlg);
static void OnAlermJihou(HWND hDlg, WORD id);
static void OnSanshoAlarm(HWND hDlg, WORD id);
static void On12Hour(HWND hDlg);
static void OnDelAlarm(HWND hDlg);
static void OnFileChange(HWND hDlg, WORD id);
static void OnTest(HWND hDlg, WORD id);

static int curAlarm;
static BOOL bPlaying = FALSE;

#define SendPSChanged(hDlg) SendMessage(GetParent(hDlg),PSM_CHANGED,(WPARAM)(hDlg),0)

/*------------------------------------------------
  Dialog procedure
--------------------------------------------------*/
BOOL CALLBACK PageAlarmProc(HWND hDlg, UINT message,
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
			case IDC_COMBOALARM:
				if(code == CBN_SELCHANGE)
					OnChangeAlarm(hDlg);
				else if(code == CBN_DROPDOWN)
					OnDropDownAlarm(hDlg);
				break;
			// checked "active" or "Play sound every hour"
			case IDC_ALARM:
			case IDC_JIHOU:
				OnAlermJihou(hDlg, id);
				break;
			// time setting changed
			case IDC_HOURALARM:
			case IDC_MINUTEALARM:
			case IDC_JIHOUPRE:
			case IDC_JIHOUTIME:
				if(code == EN_CHANGE)
					SendPSChanged(hDlg);
				break;
			// Day...
			case IDC_ALARMDAY:
				OnDay(hDlg);
				break;
			// file name changed
			case IDC_FILEALARM:
			case IDC_FILEJIHOU:
				if(code == EN_CHANGE)
				{
					OnFileChange(hDlg, id);
					SendPSChanged(hDlg);
				}
				break;
			// browse file
			case IDC_SANSHOALARM:
			case IDC_SANSHOJIHOU:
				OnSanshoAlarm(hDlg, id);
				OnFileChange(hDlg, (WORD)(id - 1));
				SendPSChanged(hDlg);
				break;
			// checked "12 hour"
			case IDC_12HOURALARM:
				On12Hour(hDlg);
				break;
			// checked other checkboxes
			case IDC_REPEATALARM:
			case IDC_REPEATJIHOU:
			case IDC_BLINKALARM:
			case IDC_BLINKJIHOU:
			case IDC_ONBOOTEXEC:
				SendPSChanged(hDlg);
				break;
			// delete an alarm
			case IDC_DELALARM:
				OnDelAlarm(hDlg);
				break;
			// test sound
			case IDC_TESTALARM:
			case IDC_TESTJIHOU:
				OnTest(hDlg, id);
				break;
			}
			return TRUE;
		}
		case WM_NOTIFY:
			switch(((NMHDR *)lParam)->code)
			{
				case PSN_APPLY: OnApply(hDlg); break;
				case PSN_HELP: My2chHelp(GetParent(hDlg), 3); break;
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
			SendDlgItemMessage(hDlg, IDC_TESTALARM, BM_SETIMAGE, IMAGE_ICON,
				(LPARAM)g_hIconPlay);
			SendDlgItemMessage(hDlg, IDC_TESTJIHOU, BM_SETIMAGE, IMAGE_ICON,
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
	char s[1024];
	int i, count, index;
	HFONT hfont;

	hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if(hfont)
	{
		SendDlgItemMessage(hDlg, IDC_COMBOALARM,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_FILEALARM,
			WM_SETFONT, (WPARAM)hfont, 0);
		SendDlgItemMessage(hDlg, IDC_FILEJIHOU,
			WM_SETFONT, (WPARAM)hfont, 0);
	}

	index = CBAddString(hDlg, IDC_COMBOALARM, (LPARAM)MyString(IDS_ADDALARM));
	CBSetItemData(hDlg, IDC_COMBOALARM, index, 0);

	count = GetMyRegLong("", "AlarmNum", 0);
	if(count < 1) count = 0;
	for(i = 0; i < count; i++)
	{
		PALARMSTRUCT pAS;
		pAS = malloc(sizeof(ALARMSTRUCT));
		ReadAlarmFromReg(pAS, i);
		index = CBAddString(hDlg, IDC_COMBOALARM, (LPARAM)pAS->name);
		CBSetItemData(hDlg, IDC_COMBOALARM, index, (LPARAM)pAS);
		if(i == 0)  SetAlarmToDlg(hDlg, pAS);
	}
	//リスト項目の表示数を指定
	AdjustDlgConboBoxDropDown(hDlg, IDC_COMBOALARM, 7);
	if(count > 0)
	{
		CBSetCurSel(hDlg, IDC_COMBOALARM, 1);
		curAlarm = 1;
	}
	else
	{
		ALARMSTRUCT as;
		CBSetCurSel(hDlg, IDC_COMBOALARM, 0);
		curAlarm = -1;
		memset(&as, 0, sizeof(as));
		as.hour = 12;
		as.days = 0x7f;
		SetAlarmToDlg(hDlg, &as);
	}

	CheckDlgButton(hDlg, IDC_JIHOU,
		GetMyRegLong("", "Jihou", FALSE));

	GetMyRegStr("", "JihouFile", s, 1024, "");
	SetDlgItemText(hDlg, IDC_FILEJIHOU, s);

	SendDlgItemMessage(hDlg, IDC_SPINJIHOUPRE, UDM_SETRANGE, 0, MAKELONG(30, 0));
	SendDlgItemMessage(hDlg, IDC_SPINJIHOUPRE, UDM_SETPOS, 0, GetMyRegLong("", "JihouPre", 0));
	SendDlgItemMessage(hDlg, IDC_SPINJIHOUTIME, UDM_SETRANGE, 0, MAKELONG(60, 0));
	SendDlgItemMessage(hDlg, IDC_SPINJIHOUTIME, UDM_SETPOS, 0, GetMyRegLong("", "JihouTime", 0));

	CheckDlgButton(hDlg, IDC_REPEATJIHOU,
		GetMyRegLong("", "JihouRepeat", FALSE));

	CheckDlgButton(hDlg, IDC_BLINKJIHOU,
		GetMyRegLong("", "JihouBlink", FALSE));

	OnAlermJihou(hDlg, IDC_JIHOU);

	SendDlgItemMessage(hDlg, IDC_TESTALARM, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconPlay);
	OnFileChange(hDlg, IDC_FILEALARM);
	SendDlgItemMessage(hDlg, IDC_TESTJIHOU, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconPlay);
	OnFileChange(hDlg, IDC_FILEJIHOU);

	SendDlgItemMessage(hDlg, IDC_DELALARM, BM_SETIMAGE, IMAGE_ICON,
		(LPARAM)g_hIconDel);


	bPlaying = FALSE;
}

/*------------------------------------------------
   apply - save settings
--------------------------------------------------*/
void OnApply(HWND hDlg)
{
	char s[1024];
	int i, count, n_alarm;
	PALARMSTRUCT pAS;

	n_alarm = 0;

	if(curAlarm < 0)
	{
		char name[40];
		GetDlgItemText(hDlg, IDC_COMBOALARM, name, 40);
		if(name[0] && IsDlgButtonChecked(hDlg, IDC_ALARM))
		{
			pAS = malloc(sizeof(ALARMSTRUCT));
			if(pAS)
			{
				int index;
				GetAlarmFromDlg(hDlg, pAS);
				index = CBAddString(hDlg, IDC_COMBOALARM, (LPARAM)pAS->name);
				CBSetItemData(hDlg, IDC_COMBOALARM, index, (LPARAM)pAS);
				curAlarm = index;
				CBSetCurSel(hDlg, IDC_COMBOALARM, index);
				EnableDlgItem(hDlg, IDC_DELALARM, TRUE);
			}
		}
	}
	else
	{
		pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, curAlarm);
		if(pAS)
			GetAlarmFromDlg(hDlg, pAS);
	}

	count = CBGetCount(hDlg, IDC_COMBOALARM);
	for(i = 0; i < count; i++)
	{
		PALARMSTRUCT pAS;
		pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, i);
		if(pAS)
		{
			SaveAlarmToReg(pAS, n_alarm);
			n_alarm++;
		}
	}
	for(i = n_alarm; ; i++)
	{
		char subkey[20];
		wsprintf(subkey, "Alarm%d", i + 1);
		if(GetMyRegLong(subkey, "Hour", -1) >= 0)
			DelMyRegKey(subkey);
		else break;
	}

	SetMyRegLong("", "AlarmNum", n_alarm);

	SetMyRegLong("", "Jihou",
		IsDlgButtonChecked(hDlg, IDC_JIHOU));

	GetDlgItemText(hDlg, IDC_FILEJIHOU, s, 1024);
	SetMyRegStr("", "JihouFile", s);

	SetMyRegLong("", "JihouPre",
		SendDlgItemMessage(hDlg, IDC_SPINJIHOUPRE, UDM_GETPOS, 0, 0));

	SetMyRegLong("", "JihouTime",
		SendDlgItemMessage(hDlg, IDC_SPINJIHOUTIME, UDM_GETPOS, 0, 0));

	SetMyRegLong("", "JihouRepeat",
		IsDlgButtonChecked(hDlg, IDC_REPEATJIHOU));
	SetMyRegLong("", "JihouBlink",
		IsDlgButtonChecked(hDlg, IDC_BLINKJIHOU));

	InitAlarm(); // alarm.c
}

/*------------------------------------------------
  free memories associated with combo box.
--------------------------------------------------*/
void OnDestroy(HWND hDlg)
{
	int i, count;

	count = CBGetCount(hDlg, IDC_COMBOALARM);
	for(i = 0; i < count; i++)
	{
		PALARMSTRUCT pAS;
		pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, i);
		if(pAS) free(pAS);
	}
}

/*------------------------------------------------
  read settings of an alarm
--------------------------------------------------*/
void ReadAlarmFromReg(PALARMSTRUCT pAS, int num)
{
	char subkey[20];

	wsprintf(subkey, "Alarm%d", num + 1);

	GetMyRegStr(subkey, "Name", pAS->name, 40, "");
	pAS->bAlarm = GetMyRegLong(subkey, "Alarm", FALSE);
	pAS->hour = GetMyRegLong(subkey, "Hour", 12);
	pAS->minute = GetMyRegLong(subkey, "Minute", 0);
	GetMyRegStr(subkey, "File", pAS->fname, 1024, "");
	pAS->bHour12 = GetMyRegLong(subkey, "Hour12", TRUE);
	pAS->bRepeat = GetMyRegLong(subkey, "Repeat", FALSE);
	pAS->bBlink = GetMyRegLong(subkey, "Blink", FALSE);
	pAS->days = GetMyRegLong(subkey, "Days", 0x7f);

	if(pAS->name[0] == 0)
		wsprintf(pAS->name, "%02d:%02d", pAS->hour, pAS->minute);
}

/*------------------------------------------------
  save settings of an alarm
--------------------------------------------------*/
void SaveAlarmToReg(PALARMSTRUCT pAS, int num)
{
	char subkey[20];

	wsprintf(subkey, "Alarm%d", num + 1);
	SetMyRegStr(subkey, "Name", pAS->name);
	SetMyRegLong(subkey, "Alarm", pAS->bAlarm);
	SetMyRegLong(subkey, "Hour", pAS->hour);
	SetMyRegLong(subkey, "Minute", pAS->minute);
	SetMyRegStr(subkey, "File", pAS->fname);
	SetMyRegLong(subkey, "Hour12", pAS->bHour12);
	SetMyRegLong(subkey, "Repeat", pAS->bRepeat);
	SetMyRegLong(subkey, "Blink", pAS->bBlink);
	SetMyRegLong(subkey, "Days", pAS->days);
}

/*------------------------------------------------
  get settings of an alarm from the page
--------------------------------------------------*/
void GetAlarmFromDlg(HWND hDlg, PALARMSTRUCT pAS)
{
	GetDlgItemText(hDlg, IDC_COMBOALARM, pAS->name, 40);
	pAS->bAlarm = IsDlgButtonChecked(hDlg, IDC_ALARM);
	pAS->hour =
		SendDlgItemMessage(hDlg, IDC_SPINHOUR, UDM_GETPOS, 0, 0);
	pAS->minute =
		SendDlgItemMessage(hDlg, IDC_SPINMINUTE, UDM_GETPOS, 0, 0);
	GetDlgItemText(hDlg, IDC_FILEALARM, pAS->fname, MAX_PATH);
	pAS->bHour12 = IsDlgButtonChecked(hDlg, IDC_12HOURALARM);
	pAS->bRepeat = IsDlgButtonChecked(hDlg, IDC_REPEATALARM);
	pAS->bBlink = IsDlgButtonChecked(hDlg, IDC_BLINKALARM);
	pAS->days = GetDlgItemInt(hDlg, IDC_HIDDENALARMDAY, NULL, FALSE);
}

/*------------------------------------------------
  set settings of an alarm to the page
--------------------------------------------------*/
void SetAlarmToDlg(HWND hDlg, PALARMSTRUCT pAS)
{
	SetDlgItemText(hDlg, IDC_COMBOALARM, pAS->name);
	CheckDlgButton(hDlg, IDC_ALARM, pAS->bAlarm);
	SendDlgItemMessage(hDlg, IDC_SPINHOUR, UDM_SETRANGE, 0,
		MAKELONG(23, 0));
	SendDlgItemMessage(hDlg, IDC_SPINHOUR, UDM_SETPOS, 0,
		pAS->hour);
	SendDlgItemMessage(hDlg, IDC_SPINMINUTE, UDM_SETRANGE, 0,
		MAKELONG(59, 0));
	SendDlgItemMessage(hDlg, IDC_SPINMINUTE, UDM_SETPOS, 0,
		pAS->minute);
	SetDlgItemText(hDlg, IDC_FILEALARM, pAS->fname);
	CheckDlgButton(hDlg, IDC_12HOURALARM, pAS->bHour12);
	CheckDlgButton(hDlg, IDC_REPEATALARM, pAS->bRepeat);
	CheckDlgButton(hDlg, IDC_BLINKALARM, pAS->bBlink);
	SetDlgItemInt(hDlg, IDC_HIDDENALARMDAY, pAS->days, FALSE);

	On12Hour(hDlg);
	OnFileChange(hDlg, IDC_FILEALARM);
	OnAlermJihou(hDlg, IDC_ALARM);
}

/*------------------------------------------------
   selected an alarm name by combobox
--------------------------------------------------*/
void OnChangeAlarm(HWND hDlg)
{
	PALARMSTRUCT pAS;
	int index;

	index = CBGetCurSel(hDlg, IDC_COMBOALARM);
	if(curAlarm >= 0 && index == curAlarm) return;

	if(curAlarm < 0)
	{
		char name[40];
		GetDlgItemText(hDlg, IDC_COMBOALARM, name, 40);
		if(name[0] && IsDlgButtonChecked(hDlg, IDC_ALARM))
		{
			pAS = malloc(sizeof(ALARMSTRUCT));
			if(pAS)
			{
				int index;
				GetAlarmFromDlg(hDlg, pAS);
				index = CBAddString(hDlg, IDC_COMBOALARM, (LPARAM)pAS->name);
				CBSetItemData(hDlg, IDC_COMBOALARM, index, (LPARAM)pAS);
				curAlarm = index;
				//リスト項目の表示数を指定
				AdjustDlgConboBoxDropDown(hDlg, IDC_COMBOALARM, 7);
			}
		}
	}
	else
	{
		pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, curAlarm);
		if(pAS) GetAlarmFromDlg(hDlg, pAS);
	}

	pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, index);
	if(pAS)
	{
		SetAlarmToDlg(hDlg, pAS);
		EnableDlgItem(hDlg, IDC_DELALARM, TRUE);
		curAlarm = index;
	}
	else
	{
		ALARMSTRUCT as;
		memset(&as, 0, sizeof(as));
		as.hour = 12;
		as.days = 0x7f;
		SetAlarmToDlg(hDlg, &as);
		EnableDlgItem(hDlg, IDC_DELALARM, FALSE);
		curAlarm = -1;
	}
}

/*------------------------------------------------
  combo box is about to be made visible
--------------------------------------------------*/
void OnDropDownAlarm(HWND hDlg)
{
	PALARMSTRUCT pAS;
	char name[40];
	int index;

	if(curAlarm < 0) return;
	pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, curAlarm);
	if(pAS == 0) return;
	GetDlgItemText(hDlg, IDC_COMBOALARM, name, 40);
	if(strcmp(name, pAS->name) != 0)
	{
		strcpy(pAS->name, name);
		CBDeleteString(hDlg, IDC_COMBOALARM, curAlarm);
		index = CBInsertString(hDlg, IDC_COMBOALARM, curAlarm, name);
		CBSetItemData(hDlg, IDC_COMBOALARM, index, (LPARAM)pAS);
		CBSetCurSel(hDlg, IDC_COMBOALARM, index);
		curAlarm = index;
	}
}

/*------------------------------------------------
  "Day..."
--------------------------------------------------*/
void OnDay(HWND hDlg)
{
	int nOld, nNew;

	nOld = GetDlgItemInt(hDlg, IDC_HIDDENALARMDAY, NULL, FALSE);
	nNew = SetAlarmDay(hDlg, nOld);
	if(nNew != nOld)
	{
		SetDlgItemInt(hDlg, IDC_HIDDENALARMDAY, nNew, FALSE);
		SendPSChanged(hDlg);
	}
}

/*------------------------------------------------
  checked "acrive" or "Play sound every hour"
--------------------------------------------------*/
void OnAlermJihou(HWND hDlg, WORD id)
{
	int s = 0, e = 0, i;
	BOOL b;

	switch(id){
		case IDC_ALARM:
			s = IDC_LABTIMEALARM; e = IDC_BLINKALARM;
			break;
		case IDC_JIHOU:
			s = IDC_LABSOUNDJIHOU; e = IDC_LABJIHOUTIME;
			break;
	}


	b = IsDlgButtonChecked(hDlg, id);
	for(i = s; i <= e; i++)	EnableDlgItem(hDlg, i, b);

	switch(id){
		case IDC_ALARM:
			OnFileChange(hDlg, IDC_FILEALARM);
		case IDC_JIHOU:
			OnFileChange(hDlg, IDC_FILEJIHOU);
	}

	if(b && id == IDC_ALARM)
	{
		char name[40];
		GetDlgItemText(hDlg, IDC_COMBOALARM, name, 40);
		if(strcmp(name, MyString(IDS_ADDALARM)) == 0)
			SetDlgItemText(hDlg, IDC_COMBOALARM, "");
	}

	SendPSChanged(hDlg);
}

/*------------------------------------------------
  browse sound file
--------------------------------------------------*/
void OnSanshoAlarm(HWND hDlg, WORD id)
{
	char deffile[MAX_PATH], fname[MAX_PATH];

	GetDlgItemText(hDlg, id - 1, deffile, MAX_PATH);

	if(!BrowseSoundFile(hDlg, deffile, fname, id)) // soundselect.c
		return;

	SetDlgItemText(hDlg, id - 1, fname);
	PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
}

/*------------------------------------------------
  checked "12 hour"
--------------------------------------------------*/
void On12Hour(HWND hDlg)
{
	WORD h, u, l;

	h = (WORD)SendDlgItemMessage(hDlg, IDC_SPINHOUR, UDM_GETPOS, 0, 0);
	if(h > 23) h = 23;
	if(h <  0) h = 0;

	// set limits to spin controls
	u = 23; l = 0;
	if(IsDlgButtonChecked(hDlg, IDC_12HOURALARM))
	{
		if(h > 12) h -= 12;
		if(h == 0) h = 12;
		u = 12; l = 1;
	}
	SendDlgItemMessage(hDlg, IDC_SPINHOUR, UDM_SETPOS, 0, h);
	SendDlgItemMessage(hDlg, IDC_SPINHOUR, UDM_SETRANGE, 0,
		MAKELONG(u, l));
	SendPSChanged(hDlg);
}

/*------------------------------------------------
  delete an alarm
--------------------------------------------------*/
void OnDelAlarm(HWND hDlg)
{
	PALARMSTRUCT pAS;

	if(curAlarm < 0) return;

	pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, curAlarm);
	if(pAS)
	{
		PostMessage(hDlg, WM_NEXTDLGCTL, 1, FALSE);
		CBDeleteString(hDlg, IDC_COMBOALARM, curAlarm);
		free(pAS);
		if(curAlarm > 0) curAlarm--;
		CBSetCurSel(hDlg, IDC_COMBOALARM, curAlarm);
		pAS = (PALARMSTRUCT)CBGetItemData(hDlg, IDC_COMBOALARM, curAlarm);
		if(pAS) SetAlarmToDlg(hDlg, pAS);
		else
		{
			ALARMSTRUCT as;
			memset(&as, 0, sizeof(as));
			as.hour = 14;
			as.days = 0x7f;
			SetAlarmToDlg(hDlg, &as);
			EnableDlgItem(hDlg, IDC_DELALARM, FALSE);
			curAlarm = -1;
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

	EnableDlgItem(hDlg,
		(id==IDC_FILEALARM)?IDC_REPEATALARM:IDC_REPEATJIHOU,
		b?IsMMFile(fname):FALSE);
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

