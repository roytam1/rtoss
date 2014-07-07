#include <windows.h>
#include "resource.h"
#include "HexEditWnd.h"
#include "WideChar.h"
#include "Common.h"
#include "macros.H"

extern HexEditWnd HEdit;
extern BOOL		  bRightClickMenu;
extern int	   	  iFileCDMode;
extern WNDPROC    pOrgTBWndProc;

void RemoveChar(char *src, char c) {
	int i;

	for (i = 0; src[i]; i++) {
		if (src[i] == c) {
			char *p = src + i + 1;
			int len = lstrlen(p);
			MEMCPY((void *)(src + i), src + i + 1, len);
			src[i + len] = 0;
			i--;
		}
	}
}

void RestoreFromProfile(HWND hWnd, char *key) {
	char buff[256];
	GetPrivateProfileString(INI_SECTION, key, "", buff, sizeof(buff), HEdit.cIniPath);
	char *p = buff;
	char *last = buff;
	for (; *p != 0; p++) {
		if (*p == ',') {
			*p = 0;
			SendMessage(hWnd, CB_ADDSTRING, 0, (LONG)(VOID *)last);
			last = p+1;
		}
	}
}

void SaveToProfile(HWND hWnd, char *key) {
	char items[8][16];
	char buff[256];
	char item[16];
	buff[0] = 0;

	int total = 0;
	SendMessage(hWnd, WM_GETTEXT, sizeof(item), (LONG)(VOID *)item);
	strcat(buff, item);
	strcat(buff, ",");
	strcpy(items[total++], item);

	int count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);

	for (int i = 0; i < count; i++) {
		SendMessage(hWnd, CB_GETLBTEXT, i, (LONG)(VOID *)item);
		int same = 0;
		for (int j = 0; j < total; j++) {
			if (strcmp(items[j], item) == 0) {
				same = 1;
				break;
			}
		}
		if (same) {
			continue;
		} else {
			strcpy(items[total++], item);
			strcat(buff, item);
			strcat(buff, ",");
			if (total == 9) {
				break;
			}
		}
	}
	WritePrivateProfileString(INI_SECTION, key, buff, HEdit.cIniPath);
}

LRESULT __stdcall HEditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#ifdef DEBUG
	DebugPrint("MainWnd uMsg=%d wParam=%d lParam=%d", uMsg, wParam, lParam);
#endif
	switch (uMsg) {
	case WM_NOTIFY:
		if (HEdit.HEHandleWM_NOTIFY(hWnd, uMsg, wParam, lParam))
			return 0;
		else 
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

	case WM_SHOWWINDOW:
		HEdit.HEHandleWM_SHOWWINDOW(hWnd, uMsg, wParam, lParam);
		break;
	case WM_TIMER:
		HEdit.HEHandleWM_TIMER(hWnd, wParam);
		break;

	case WM_SIZE:
		HEdit.HEHandleWM_SIZE(hWnd, wParam, lParam);
		break;

	case WM_COMMAND:
		HEdit.HEHandleWM_COMMAND(hWnd, uMsg, wParam, lParam);
		break;

	case WM_TRAYMENU:
		HEdit.HEHandleWM_TRAYMENU(hWnd, uMsg, wParam, lParam);
		return 0;

	case WM_MOVE:
		HEdit.HEHandleWM_MOVE(hWnd, wParam, lParam);
		break;

	case WM_CLOSE:
		HEdit.HEHandleWM_CLOSE(hWnd, uMsg, wParam, lParam);
		break;

	case WM_PAINT:
		HEdit.PaintText(hWnd);
		break;

	case WM_KEYDOWN:
		HEdit.HEHandleWM_KEYDOWN(hWnd, uMsg, wParam, lParam);
		break;

	case WM_CHAR:
		return HEdit.HEHandleWM_CHAR(hWnd, uMsg, wParam, lParam);

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		HEdit.HEHandleLButton(hWnd, uMsg, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		HEdit.HEHandleWM_MOUSEMOVE(hWnd, uMsg, wParam, lParam);
		break;

	case WM_MOUSEWHEEL:
		HEdit.HEHandleWM_MOUSEWHEEL(hWnd, uMsg, wParam, lParam);
		break;

	case WM_VSCROLL:
		HEdit.HEHandleWM_VSCROLL(hWnd, uMsg, wParam, lParam);
		break;

	case WM_SETFOCUS:
		HEdit.HEHandleWM_SETFOCUS(hWnd);
		break;

	case WM_KILLFOCUS:
		HEdit.HEHandleWM_KILLFOCUS(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return TRUE;
}

LRESULT __stdcall TBHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_MOUSEMOVE:
		HEdit.HEHandleWM_MOUSEMOVE(hWnd, uMsg, wParam, lParam);
		break;

	case WM_LBUTTONUP:
		SendMessage(HEdit.hMainWnd, uMsg, 0, 0);
		break;
	}

	return CallWindowProc(pOrgTBWndProc, hWnd, uMsg, wParam, lParam);
}

void HexEditWnd::OptionDlgInit(HWND hDlg) {
	if (bRightClickMenu)
		CheckDlgButton(hDlg, IDC_SHELLI, TRUE);
	else
		CheckDlgButton(hDlg, IDC_SHELLR, TRUE);

	if (iFileCDMode == 0)
		CheckDlgButton(hDlg, IDC_FILECDC, TRUE);
	else if (iFileCDMode == 1)
		CheckDlgButton(hDlg, IDC_FILECDP, TRUE);
	else
		CheckDlgButton(hDlg, IDC_FILECDN, TRUE);
}

BOOL HexEditWnd::OptionDlgCommand(HWND hDlg, DWORD wParam) {
	switch (LOWORD(wParam)) {
	case IDOK:
		if (IsDlgButtonChecked(hDlg, IDC_SHELLI)) {
			bRightClickMenu = TRUE;
		} else {
			bRightClickMenu = FALSE;
		}

		if (IsDlgButtonChecked(hDlg, IDC_FILECDC)) {
			iFileCDMode = 0;
		} else if (IsDlgButtonChecked(hDlg, IDC_FILECDP)) {
			iFileCDMode = 1;
		} else {
			iFileCDMode = 2;
		}

		if (bRightClickMenu) {
			HKEY hkey, h16edit, hcommand;
			if (RegOpenKeyEx(HKEY_CLASSES_ROOT, "*\\Shell", 0, KEY_WRITE, &hkey) == ERROR_SUCCESS) {
				if (RegCreateKeyEx(hkey, "16Edit", 0, NULL, 0, KEY_WRITE, NULL, &h16edit, NULL) == ERROR_SUCCESS) {
					if (RegCreateKeyEx(h16edit, "command", 0, NULL, 0, KEY_WRITE, NULL, &hcommand, NULL) 
							== ERROR_SUCCESS) {
						char name[256];
						char command[256];

						GetModuleFileName(GetModuleHandle(NULL), name, sizeof(name));
						wsprintf(command, "%s \"%%1\"", name);
						RegSetValueEx(hcommand, NULL, 0, REG_SZ, (BYTE *)command, strlen(command) + 1);
						RegCloseKey(hcommand);
						RegCloseKey(h16edit);
						RegCloseKey(hkey);
					} else {
						RegCloseKey(h16edit);
						RegCloseKey(hkey);
					}
				} else {
					RegCloseKey(hkey);
				}
			}
		} else {
			RegDeleteKey(HKEY_CLASSES_ROOT, "*\\Shell\\16Edit\\command");
			RegDeleteKey(HKEY_CLASSES_ROOT, "*\\Shell\\16Edit");
		}

		WritePrivateProfileStruct(INI_SECTION, INI_SHELL, &bRightClickMenu, sizeof(BOOL), HEdit.cIniPath);
		WritePrivateProfileStruct(INI_SECTION, INI_FILECD, &iFileCDMode, sizeof(int), HEdit.cIniPath);
		EndDialog(hDlg, wParam);
		return TRUE;

	case IDCANCEL:
		EndDialog(hDlg, wParam);
		return TRUE;
	}
	return TRUE;
}

BOOL __stdcall OptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		HEdit.OptionDlgInit(hDlg);
		return TRUE;
	case WM_COMMAND:
		return HEdit.OptionDlgCommand(hDlg, wParam);

	case WM_CLOSE:
		EndDialog(hDlg, wParam);
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL __stdcall GotoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		HEdit.InitGotoDlg(hDlg);
		return TRUE;

	case WM_SHOWWINDOW:
		SetFocus( GetDlgItem(hDlg, GO_OFFSET) );
		return TRUE;

	case WM_COMMAND:
		return HEdit.GDHandleWM_COMMAND(hDlg, uMsg, wParam, lParam);

	case WM_CLOSE:
		EndDialog(hDlg, wParam);
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL __stdcall SelBlockDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		HEdit.InitSelBlockDlg(hDlg);
		return TRUE;

	case WM_SHOWWINDOW:
		CheckDlgButton(hDlg, SB_RADIOEND, TRUE);
		SetFocus( GetDlgItem(hDlg, SB_START) );
		return TRUE;

	case WM_COMMAND:
		return HEdit.SBHandleWM_COMMAND(hDlg, uMsg, wParam, lParam);

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL __stdcall ReplaceDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		HEdit.ReplaceInitDlg(hDlg, uMsg, wParam, lParam);
		return TRUE;

	case WM_SHOWWINDOW:
		SetFocus(GetDlgItem(hDlg, SS_STR) );
		return TRUE;

	case WM_COMMAND:
		return HEdit.ReplaceHandleWM_COMMAND(hDlg, uMsg, wParam, lParam);

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL __stdcall SearchDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		HEdit.SSInitDlg(hDlg, uMsg, wParam, lParam);
		return TRUE;

	case WM_SHOWWINDOW:
		SetFocus( GetDlgItem(hDlg, SS_STR) );
		return TRUE;

	case WM_COMMAND:
		return HEdit.SSHandleWM_COMMAND(hDlg, uMsg, wParam, lParam);

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE; // ERR
}

void HexEditWnd::InitSelBlockDlg(HWND hDlg) {
	// hook editbox'S
	HookHexEditbox( GetDlgItem(hDlg, SB_START) );
	HookHexEditbox( GetDlgItem(hDlg, SB_END) );
	HookHexEditbox( GetDlgItem(hDlg, SB_SIZE) );

	// limit text len
	SendDlgItemMessage(hDlg, SB_START, EM_SETLIMITTEXT, 8, 0);
	SendDlgItemMessage(hDlg, SB_END, EM_SETLIMITTEXT, 8, 0);
	SendDlgItemMessage(hDlg, SB_SIZE, EM_SETLIMITTEXT, 8, 0);

	return;
}

BOOL HexEditWnd::SBHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DWORD  dwStart, dwEnd;
	char   cBuff[9];
	BOOL   b;
	HWND   hESize, hEEnd;

	switch (LOWORD(wParam)) {
	case SB_START:
	case SB_END:
	case SB_SIZE:
		if (HIWORD(wParam) != NM_HEXEDITENTER)
			return FALSE;
	case SB_OK:
		// get numbers
		GetDlgItemText(hDlg, SB_START, cBuff, sizeof(cBuff));
		HexStrToInt(cBuff, &dwStart);

		if (IsDlgButtonChecked(hDlg, SB_RADIOEND)) {
			GetDlgItemText(hDlg, SB_END, cBuff, sizeof(cBuff));
			HexStrToInt(cBuff, &dwEnd);
		} else {
			GetDlgItemText(hDlg, SB_SIZE, cBuff, sizeof(cBuff));
			HexStrToInt(cBuff, &dwEnd);
			if (dwEnd != 0)
				--dwEnd;
			dwEnd += dwStart;
		}

		// valid?
		if (IsOutOfRange(dwEnd) ||
			IsOutOfRange(dwStart))
			ErrMsg(hDlg, "Out of range !");
		else {
			SetTopLine( (DWORD)__min(dwStart, dwEnd) );
			SetSelection(dwStart, dwEnd);
			SendMessage(hDlg, WM_CLOSE, 0, 0);
		}
		return TRUE;

	case SB_RADIOSIZE:
	case SB_RADIOEND:
		// GUI stuff
		hEEnd  = GetDlgItem(hDlg, SB_END);
		hESize = GetDlgItem(hDlg, SB_SIZE);
		b = (LOWORD(wParam) == SB_RADIOEND) ? TRUE : FALSE;
		EnableWindow( hEEnd, b);
		EnableWindow( hESize, b ^ 1);
		SetFocus( hEEnd );  
		SetWindowText( b ? hESize : hEEnd, "");
		SetFocus( b ? hEEnd : hESize);
		return TRUE;

	case SB_CANCEL:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		return TRUE;
	}

	return FALSE;
}

void HexEditWnd::SSInitDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND   hCur;
	char   cBuff[9];

	// setup str edit box
	HookEditboxEnter( GetDlgItem(hDlg, SS_STR) );

	RestoreFromProfile(GetDlgItem(hDlg, SS_STR), INI_SEARCH_STRING);
	SendDlgItemMessage(hDlg, SS_STR, CB_SETCURSEL, 0, 0);

	// setup offset edit box
	hCur = GetDlgItem(hDlg, SS_OFFSET);
	HookHexEditbox(hCur);
	SendMessage(hCur, EM_SETLIMITTEXT, 8, 0);

	if (search.bInited) {
		//
		// restore last search options
		//

		// str stuff
		if (search.bASCIIStr)
			CheckDlgButton(hDlg, SS_SEARCHASCII, TRUE);
		if (search.bWideCharStr)
			CheckDlgButton(hDlg, SS_SEARCHUNICODE, TRUE);
		if (search.bCaseSensitive)
			CheckDlgButton(hDlg, SS_CASESENSITIVE, TRUE);
		else
			CheckDlgButton(hDlg, SS_NOCASESENSITIVE, TRUE);

		if (!(search.bASCIIStr || search.bWideCharStr)) {
			HookHexEditbox(GetDlgItem(hDlg, SS_STR));
		} else {
			UnHookHexEditbox(GetDlgItem(hDlg, SS_STR));
		}

		//SetDlgItemText(hDlg, SS_STR, (PSTR)search.pDlgStr);

		// location stuff
		if (search.bDown)
			CheckDlgButton(hDlg, SS_DOWN, TRUE);
		else
			CheckDlgButton(hDlg, SS_UP, TRUE);
		switch (search.iDlgSearchFrom) {
		case 0:
			CheckDlgButton(hDlg, SS_SEARCHFROMTOP, TRUE);
			break;

		case 1:
			if (stat.bCaretPosValid) {
				SendMessage(hDlg, WM_COMMAND, SS_SEARCHFROMCURPOS, 0);
				break;
			}
			// ...

		default: // 2 and eventually 1
			wsprintf(cBuff, H8, search.dwStartOff);
			SetDlgItemText(hDlg, SS_OFFSET, cBuff);
			CheckDlgButton(hDlg, SS_SEARCHFROMOFF, TRUE);           
			break;
		}

	} else {
		//
		// setup default dlg
		//

		// case sensitivity
		CheckDlgButton(hDlg, SS_NOCASESENSITIVE, TRUE);

		// set default settings
		// CheckDlgButton(hDlg, SS_SEARCHASCII, TRUE);
		CheckDlgButton(hDlg, SS_SEARCHFROMCURPOS, TRUE);
		CheckDlgButton(hDlg, SS_DOWN, TRUE);
		if (!stat.bCaretPosValid) {
			// enable this radio
			EnableWindow( GetDlgItem(hDlg, SS_SEARCHFROMCURPOS), FALSE);
		}
	}

	// setup GUI
	SSEnableItems(hDlg);

	return;
}

void HexEditWnd::ReplaceInitDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HWND   hCur;
	char   cBuff[9];

	// setup offset edit box
	hCur = GetDlgItem(hDlg, SS_OFFSET);
	HookHexEditbox(hCur);
	SendMessage(hCur, EM_SETLIMITTEXT, 8, 0);

	RestoreFromProfile(GetDlgItem(hDlg, SS_STR), INI_SEARCH_STRING);
	SendDlgItemMessage(hDlg, SS_STR, CB_SETCURSEL, 0, 0);

	RestoreFromProfile(GetDlgItem(hDlg, SS_STR2), INI_REPLACE_STRING);
	SendDlgItemMessage(hDlg, SS_STR2, CB_SETCURSEL, 0, 0);

	if (search.bInited) {
		//
		// restore last search options
		//

		// str stuff
		if (search.bASCIIStr)
			CheckDlgButton(hDlg, SS_SEARCHASCII, TRUE);
		if (search.bWideCharStr)
			CheckDlgButton(hDlg, SS_SEARCHUNICODE, TRUE);
		if (search.bCaseSensitive)
			CheckDlgButton(hDlg, SS_CASESENSITIVE, TRUE);
		else
			CheckDlgButton(hDlg, SS_NOCASESENSITIVE, TRUE);

		if (!(search.bASCIIStr || search.bWideCharStr)) {
			HookHexEditbox(GetDlgItem(hDlg, SS_STR));
			HookHexEditbox(GetDlgItem(hDlg, SS_STR2));
		} else {
			UnHookHexEditbox(GetDlgItem(hDlg, SS_STR));
			UnHookHexEditbox(GetDlgItem(hDlg, SS_STR2));
		}

		//SetDlgItemText(hDlg, SS_STR, (PSTR)search.pDlgStr);
		//SetDlgItemText(hDlg, SS_STR2, (PSTR)search.pReplaceStr);

		// location stuff
		if (search.bDown)
			CheckDlgButton(hDlg, SS_DOWN, TRUE);
		else
			CheckDlgButton(hDlg, SS_UP, TRUE);
		switch (search.iDlgSearchFrom) {
		case 0:
			CheckDlgButton(hDlg, SS_SEARCHFROMTOP, TRUE);
			break;

		case 1:
			if (stat.bCaretPosValid) {
				SendMessage(hDlg, WM_COMMAND, SS_SEARCHFROMCURPOS, 0);
				break;
			}
			// ...

		default: // 2 and eventually 1
			wsprintf(cBuff, H8, search.dwStartOff);
			SetDlgItemText(hDlg, SS_OFFSET, cBuff);
			CheckDlgButton(hDlg, SS_SEARCHFROMOFF, TRUE);           
			break;
		}

	} else {
		// case sensitivity
		CheckDlgButton(hDlg, SS_NOCASESENSITIVE, TRUE);

		// set default settings
		// CheckDlgButton(hDlg, SS_SEARCHASCII, TRUE);
		CheckDlgButton(hDlg, SS_SEARCHFROMCURPOS, TRUE);
		CheckDlgButton(hDlg, SS_DOWN, TRUE);
		if (!stat.bCaretPosValid) {
			// enable this radio
			EnableWindow( GetDlgItem(hDlg, SS_SEARCHFROMCURPOS), FALSE);
		}
	}

	// setup GUI
	SSEnableItems(hDlg);

	return;
}

BOOL HexEditWnd::SSHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char  cBuff[20];

	switch (LOWORD(wParam)) {
	case SS_STR:
		if (HIWORD(wParam) != NM_HEXEDITENTER)
			return FALSE;
	case SS_OK:
		if (SSHandleSS_OK(hDlg)) {
			SaveToProfile(GetDlgItem(hDlg, SS_STR), INI_SEARCH_STRING);
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			PerformStrSearch(&search);
		}
		break;

	case SS_CANCEL:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		return TRUE;

	case SS_SEARCHASCII:
		CheckDlgButton(hDlg, SS_SEARCHUNICODE, BST_UNCHECKED);
		SSEnableItems(hDlg);
		SetFocus( GetDlgItem(hDlg, SS_STR) );
		return TRUE;

	case SS_SEARCHUNICODE:
		CheckDlgButton(hDlg, SS_SEARCHASCII, BST_UNCHECKED);
		SSEnableItems(hDlg);
		SetFocus( GetDlgItem(hDlg, SS_STR) );
		return TRUE;

	case SS_CASESENSITIVE:
	case SS_NOCASESENSITIVE:
		CheckRadioButton(hDlg, SS_CASESENSITIVE, SS_NOCASESENSITIVE, LOWORD(wParam) );
		SetFocus( GetDlgItem(hDlg, SS_STR) );
		return TRUE;

	case SS_SEARCHFROMCURPOS:
	case SS_SEARCHFROMTOP:
	case SS_SEARCHFROMOFF:
		CheckRadioButton(hDlg, SS_SEARCHFROMTOP, SS_SEARCHFROMOFF, LOWORD(wParam) );
		SSEnableItems(hDlg);

		// show current pos
		if (LOWORD(wParam) == SS_SEARCHFROMCURPOS) {
			// show current off
			wsprintf(cBuff, "("_H8")", stat.posCaret.dwOffset);
			SetDlgItemText(hDlg, SS_CUROFF, cBuff);
		} else
			SetDlgItemText(hDlg, SS_CUROFF, NULL);

		// focus off editbox
		if (LOWORD(wParam) == SS_SEARCHFROMOFF)
			SetFocus( GetDlgItem(hDlg, SS_OFFSET) );
		else
			SetFocus( GetDlgItem(hDlg, SS_STR) );
		return TRUE;

	case SS_UP:
	case SS_DOWN:
		CheckRadioButton(hDlg, SS_DOWN, SS_UP, LOWORD(wParam) );
		SetFocus( GetDlgItem(hDlg, SS_STR) );
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL HexEditWnd::ReplaceHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char  cBuff[20];

	switch (LOWORD(wParam)) {
	case SS_REPLACE:
		if (ReplaceHandleSS_OK(hDlg)) {
			SaveToProfile(GetDlgItem(hDlg, SS_STR), INI_SEARCH_STRING);
			SaveToProfile(GetDlgItem(hDlg, SS_STR2), INI_REPLACE_STRING);
			PerformStrReplace(&search);
		}
		break;
	case SS_REPLACEALL:
		if (ReplaceHandleSS_OK(hDlg)) {
			SaveToProfile(GetDlgItem(hDlg, SS_STR), INI_SEARCH_STRING);
			SaveToProfile(GetDlgItem(hDlg, SS_STR2), INI_REPLACE_STRING);
			PerformStrReplaceAll(&search);
		}
		break;
	case SS_FIND:
		if (ReplaceHandleSS_OK(hDlg)) {
			SaveToProfile(GetDlgItem(hDlg, SS_STR), INI_SEARCH_STRING);
			SaveToProfile(GetDlgItem(hDlg, SS_STR2), INI_REPLACE_STRING);
			PerformStrSearch(&search);
		}
		break;

	case SS_CANCEL:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		return TRUE;

	case SS_SEARCHASCII:
		CheckDlgButton(hDlg, SS_SEARCHUNICODE, BST_UNCHECKED);
		SSEnableItems(hDlg);
		return TRUE;

	case SS_SEARCHUNICODE:
		CheckDlgButton(hDlg, SS_SEARCHASCII, BST_UNCHECKED);
		SSEnableItems(hDlg);
		return TRUE;

	case SS_CASESENSITIVE:
	case SS_NOCASESENSITIVE:
		CheckRadioButton(hDlg, SS_CASESENSITIVE, SS_NOCASESENSITIVE, LOWORD(wParam) );
		return TRUE;

	case SS_SEARCHFROMCURPOS:
	case SS_SEARCHFROMTOP:
	case SS_SEARCHFROMOFF:
		CheckRadioButton(hDlg, SS_SEARCHFROMTOP, SS_SEARCHFROMOFF, LOWORD(wParam) );
		SSEnableItems(hDlg);

		// focus off editbox
		if (LOWORD(wParam) == SS_SEARCHFROMOFF)
			SetFocus( GetDlgItem(hDlg, SS_OFFSET) );

		// show current pos
		if (LOWORD(wParam) == SS_SEARCHFROMCURPOS) {
			// show current off
			wsprintf(cBuff, "("_H8")", stat.posCaret.dwOffset);
			SetDlgItemText(hDlg, SS_CUROFF, cBuff);
		} else
			SetDlgItemText(hDlg, SS_CUROFF, NULL);
		return TRUE;

	case SS_UP:
	case SS_DOWN:
		CheckRadioButton(hDlg, SS_DOWN, SS_UP, LOWORD(wParam) );
		return TRUE;
	}

	return FALSE; // ERR
}

void HexEditWnd::SSEnableItems(HWND hDlg)
{
	BOOL b;

	// case sensitive - radio
	b = IsDlgButtonChecked(hDlg, SS_SEARCHASCII) ||
		IsDlgButtonChecked(hDlg, SS_SEARCHUNICODE);
	EnableWindow( GetDlgItem(hDlg, SS_CASESENSITIVE), b);
	EnableWindow( GetDlgItem(hDlg, SS_NOCASESENSITIVE), b);
	if (!b) {
		HookHexEditbox(GetDlgItem(hDlg, SS_STR));
		HookHexEditbox(GetDlgItem(hDlg, SS_STR2));
	} else {
		UnHookHexEditbox(GetDlgItem(hDlg, SS_STR));
		UnHookHexEditbox(GetDlgItem(hDlg, SS_STR2));
	}


	// location
	b = IsDlgButtonChecked(hDlg, SS_SEARCHFROMOFF);
	EnableWindow( GetDlgItem(hDlg, SS_OFFSET), b);

	return;
}

/*
 * Create a new string according type
 * if the type == HEX_STRING and the string
 * cannot be converted, then return null
 */
BYTE* GetConvertString(char *src, int type, DWORD *retlen = NULL) {
	char *retstr = NULL;
	int len;

	if (type == HEX_STRING) {
		RemoveChar(src, ' ');
		len = strlen(src);
		if (len % 2 == 0) {
			len /= 2;

			retstr = (char*)malloc(len + 1);
			if (retlen != NULL) {
				*retlen = len;
			}
			for (int u = 0; u < len; u++) {
				char cBuff[9];
				DWORD dwByte;

				lstrcpyn(cBuff, (PSTR)(src + u * 2), 3);
				if (!HexStrToInt(cBuff, &dwByte)) {
					free(retstr);
					return NULL;
				}
				retstr[u] = (BYTE)dwByte;
			}
		} else {
			return NULL;
		}
	} else if (type == UNI_STRING) {
		len = strlen(src) + 1;
		retstr = (char*)malloc(len * 2);
		WideChar::SingleToWideCharStr(src, (PWSTR)retstr, len * 2);
	} else {
		len = strlen(src) + 1;
		retstr = (char*)malloc(len);
		strcpy(retstr, src);
	}
	return (BYTE*)retstr;
}

//
// get search options from gui
//
BOOL HexEditWnd::SSHandleSS_OK(HWND hDlg)
{
	HWND               hStrEdit;
	HE_SEARCHOPTIONS   data, tmp;
	DWORD              dwByte, dwOff;
	char               cBuff[9];
	UINT               u;
	BOOL               bRet = FALSE;

	ZERO(data);

	//
	// handle search data
	//

	// get info
	hStrEdit = GetDlgItem(hDlg, SS_STR);
	data.dwcStr           = GetWindowTextLength(hStrEdit);
	data.bASCIIStr        = IsDlgButtonChecked(hDlg, SS_SEARCHASCII);
	data.bWideCharStr     = IsDlgButtonChecked(hDlg, SS_SEARCHUNICODE);
	data.bCaseSensitive   = IsDlgButtonChecked(hDlg, SS_CASESENSITIVE);

	// valid?
	if (!data.dwcStr) {
		ErrMsg(hDlg, "Please enter a search string !");
		goto Exit; // ERR
	}

	// get search str data
	data.dwcBuff = data.dwcStr + 1;
	if (data.bWideCharStr) {
		data.dwcBuff *= 2;
		data.dwcStr  *= 2;
	}
	data.pData   = (BYTE *)malloc(data.dwcBuff);
	if (!data.pData) {
		ErrMsg(hDlg, STR_NO_MEM);
		goto Exit; // ERR
	}

	// save entered str
	data.pDlgStr = malloc(data.dwcStr + 1);
	if (!data.pDlgStr) {
		ErrMsg(hDlg, STR_NO_MEM);
		goto Exit; // ERR
	}
	GetDlgItemText(hDlg, SS_STR, (PSTR)data.pDlgStr, data.dwcStr + 1);

	// set search str data
	GetDlgItemText(hDlg, SS_STR, (PSTR)data.pData, data.dwcBuff);

	if ( !(data.bASCIIStr || data.bWideCharStr)) {
		RemoveChar((char *)data.pData, ' ');
		data.dwcStr = lstrlen((const char*)data.pData);
		if (data.dwcStr % 2) {
			ErrMsg(hDlg, "Hex byte pairs have to result in an even number of digits !");
			goto Exit; // ERR
		}
	}

	if ( !(data.bASCIIStr || data.bWideCharStr) ) {
		// ASCII -> bytes
		data.dwcStr /= 2;
		for (u = 0; u < data.dwcStr; u++) {
			lstrcpyn(cBuff, (PSTR)((DWORD)data.pData + u * 2), 3);
			if (!HexStrToInt(cBuff, &dwByte)) {
				ErrMsg(hDlg, "No hex string !");
				goto Exit; // ERR
			}
			*(BYTE*)((DWORD)data.pData + u) = (BYTE)dwByte;
		}

	}

	// finally convert ASCII -> UNICODE if wanted
	if (data.bWideCharStr)
		WideChar::SingleToWideCharStr(
									 (PSTR)data.pData,
									 (PWSTR)data.pData,
									 data.dwcBuff);

	//
	// handle location
	//
	if (IsDlgButtonChecked(hDlg, SS_SEARCHFROMCURPOS)) {
		data.dwStartOff       = stat.posCaret.dwOffset;
		data.iDlgSearchFrom   = 1;
	} else if (IsDlgButtonChecked(hDlg, SS_SEARCHFROMTOP)) {
		data.dwStartOff      = 0;
		data.iDlgSearchFrom  = 0;
	} else {
		data.iDlgSearchFrom = 2;

		GetDlgItemText(hDlg, SS_OFFSET, cBuff, sizeof(cBuff));
		HexStrToInt(cBuff, &dwOff);
		if (IsOutOfRange(dwOff)) {
			ErrMsg(hDlg, "Searching start offset is out of range !");
			goto Exit; // ERR
		}
		data.dwStartOff = dwOff;
	}
	data.bDown = IsDlgButtonChecked(hDlg, SS_DOWN);

	//
	// apply structure
	//

	// free old buffers
	if (search.bInited &&
		search.pData)
		free(search.pData);
	if (search.bInited &&
		search.pDlgStr)
		free(search.pDlgStr);

	tmp				= search;
	search          = data;

	search.pReplaceData	= tmp.pReplaceData;
	search.pReplaceStr	= tmp.pReplaceStr;
	search.dwcReplaceStr = tmp.dwcReplaceStr;

	search.bInited  = TRUE;

	ConfigureTB();

	bRet = TRUE;

	Exit:

	// free buffers
	if (!bRet && data.pData)
		free(data.pData);
	if (!bRet && data.pDlgStr)
		free(data.pDlgStr);

	return bRet; // OK
}

//
// get search options from gui
//
BOOL HexEditWnd::ReplaceHandleSS_OK(HWND hDlg)
{
	HWND               hStrEdit;
	HE_SEARCHOPTIONS   data;
	DWORD              dwOff;
	char               cBuff[256];
	BOOL               bRet = FALSE;
	int				   type = HEX_STRING;

	ZERO(data);

	// get info
	hStrEdit = GetDlgItem(hDlg, SS_STR);
	data.dwcStr           = GetWindowTextLength(hStrEdit);
	data.bASCIIStr        = IsDlgButtonChecked(hDlg, SS_SEARCHASCII);
	data.bWideCharStr     = IsDlgButtonChecked(hDlg, SS_SEARCHUNICODE);
	data.bCaseSensitive   = IsDlgButtonChecked(hDlg, SS_CASESENSITIVE);

	if (data.bWideCharStr) {
		type = UNI_STRING;
	} else if (data.bASCIIStr) {
		type = ASC_STRING;
	}

	if (!data.dwcStr) {
		ErrMsg(hDlg, "Please enter a search string!");
		goto Exit; // ERR
	}

	// get search str data
	data.dwcBuff = data.dwcStr + 1;
	if (data.bWideCharStr) {
		data.dwcBuff *= 2;
		data.dwcStr  *= 2;
	}

	data.pDlgStr = malloc(data.dwcStr + 1);
	GetDlgItemText(hDlg, SS_STR, (PSTR)data.pDlgStr, data.dwcStr + 1);

	GetDlgItemText(hDlg, SS_STR, (PSTR)cBuff, sizeof(cBuff));
	data.pData = GetConvertString(cBuff, type, &data.dwcStr);
	if (data.pData == NULL) {
		ErrMsg(hDlg, "Search string not hex string!");
		goto Exit; // ERR
	}

	hStrEdit = GetDlgItem(hDlg, SS_STR2);
	data.dwcReplaceStr    = GetWindowTextLength(hStrEdit);
	if (data.dwcReplaceStr > 0) {
		data.pReplaceStr = malloc(data.dwcReplaceStr + 1);
		GetDlgItemText(hDlg, SS_STR2, (PSTR)data.pReplaceStr, data.dwcReplaceStr + 1);

		if (data.bWideCharStr) {
			data.dwcReplaceStr  *= 2;
		}
		GetDlgItemText(hDlg, SS_STR2, (PSTR)cBuff, sizeof(cBuff));
		data.pReplaceData = GetConvertString(cBuff, type, &data.dwcReplaceStr);
		if (data.pReplaceData == NULL) {
			ErrMsg(hDlg, "Replace string not hex string!");
			goto Exit; // ERR
		}
	}

	//
	// handle location
	//
	data.bDown = IsDlgButtonChecked(hDlg, SS_DOWN);
	if (IsDlgButtonChecked(hDlg, SS_SEARCHFROMCURPOS)) {
		if (data.bDown) {
			if (stat.bSel)
				data.dwStartOff = stat.dwOffSelEnd + 1;
			else
				data.dwStartOff	= stat.posCaret.dwOffset;
		} else {
			if (stat.bSel)
				data.dwStartOff = stat.dwOffSelStart - 1;
			else
				data.dwStartOff	= stat.posCaret.dwOffset;
		}
		data.iDlgSearchFrom   = 1;
	} else if (IsDlgButtonChecked(hDlg, SS_SEARCHFROMTOP)) {
		data.dwStartOff      = 0;
		data.iDlgSearchFrom  = 0;
	} else {
		data.iDlgSearchFrom = 2;

		GetDlgItemText(hDlg, SS_OFFSET, cBuff, sizeof(cBuff));
		HexStrToInt(cBuff, &dwOff);
		if (IsOutOfRange(dwOff)) {
			ErrMsg(hDlg, "Searching start offset is out of range !");
			goto Exit; // ERR
		}
		data.dwStartOff = dwOff;
	}

	// free old buffers
	if (search.bInited && search.pData) {
		free(search.pData);
	}
	if (search.bInited && search.pDlgStr) {
		free(search.pDlgStr);
	}
	if (search.bInited && search.pReplaceStr) {
		free(search.pReplaceStr);
	}
	if (search.bInited && search.pReplaceData) {
		free(search.pReplaceData);
	}

	search          = data;
	search.bInited  = TRUE;
	ConfigureTB();
	bRet = TRUE;

	Exit:
	// free buffers
	if (!bRet && data.pData) {
		free(data.pData);
	}
	if (!bRet && data.pDlgStr) {
		free(data.pDlgStr);
	}
	if (!bRet && data.pReplaceStr) {
		free(data.pReplaceStr);
	}
	if (!bRet && data.pReplaceData) {
		free(data.pReplaceData);
	}

	return bRet; // OK
}

void HexEditWnd::InitGotoDlg(HWND hDlg) {
	HWND hWndOffset = GetWindow(GetDlgItem(hDlg, GO_OFFSET), GW_CHILD);
	HookHexEditbox(hWndOffset);
	SendMessage(hWndOffset, EM_SETLIMITTEXT, 8, 0);
	RestoreFromProfile(GetDlgItem(hDlg, GO_OFFSET), INI_GO_OFFSET);

	if (bFileOffset) {
		CheckDlgButton(hDlg, IDC_FO, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_VA, BST_UNCHECKED);
	} else {
		CheckDlgButton(hDlg, IDC_VA, BST_CHECKED);
		CheckDlgButton(hDlg, IDC_FO, BST_UNCHECKED);
	}

	if (!file_type((char *)diData.pDataBuff)) {
		EnableWindow(GetDlgItem(hDlg, IDC_VA), FALSE);
	}
	return;
}

BOOL HexEditWnd::GDHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DWORD dwNewOff;
	char  cBuff[9];

	switch (LOWORD(wParam)) {
	case GO_OFFSET:
		if (HIWORD(wParam) != NM_HEXEDITENTER)
			return FALSE;
	case GO_OK:
		SaveToProfile(GetDlgItem(hDlg, GO_OFFSET), INI_GO_OFFSET);

		// get off
		GetDlgItemText(hDlg, GO_OFFSET, cBuff, sizeof(cBuff));
		HexStrToInt(cBuff, &dwNewOff);

		if (IsDlgButtonChecked(hDlg, IDC_VA)) {
			dwNewOff = GetFileOffset(dwNewOff);
		}

		if (!IsOutOfRange(dwNewOff)) {
			KillSelection();
			SetCaretPosData(dwNewOff);
			SendMessage(hDlg, WM_CLOSE, 1, 0);
		} else
			ErrMsg(hDlg, "Out of range !");
		return TRUE;

	case GO_CANCEL:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		return TRUE;
	}

	return FALSE;
}
