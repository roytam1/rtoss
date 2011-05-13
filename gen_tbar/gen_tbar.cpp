/* ----------------------------------------------------------------------------

    gen_tbar
    ========
    Winamp plugin to display customizable data on window captions

    Copyright (c) 1999-2000 Jos van den Oever (http://www.vandenoever.info/)
    Copyright (c) 2005 Martin Zuther (http://www.mzuther.de/)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

-------------------------------------------------------------------------------

    Notify CD Player
    ================
    Copyright (c) 1996-1997 Mats Ljungqvist (mlt@cyberdude.com)

    StdString
    =========
    Copyright (c) 2002 Joseph M. O'Leary.  This code is 100% free.  Use it
    anywhere you want. Rewrite it, restructure it, whatever.  If you can write
    software that makes money off of it, good for you.  I kinda like capitalism.
    Please don't blame me if it causes your $30 billion dollar satellite explode
    in orbit. If you redistribute it in any form, I'd appreciate it if you would
    leave this notice here.

---------------------------------------------------------------------------- */

#include "gen_tbar.h"

gen_tbar* pGenTbar = NULL;

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	pGenTbar->draw_titlebar();
}

BOOL CALLBACK ConfigProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return pGenTbar->config_proc(hwndDlg, uMsg, wParam, lParam);
}

gen_tbar::gen_tbar(HINSTANCE h_dll_instance, HWND hwnd_parent, CStdString help_file, CStdString plugin_version)
{
	hDllInstance = h_dll_instance;
	hWndParent   = hwnd_parent;

	strHelpFile = help_file;
	strVersion  = plugin_version;

	winamp_version = SendMessage(hWndParent, WM_WA_IPC, 0, IPC_GETVERSION);

	// Get data from registry
	data = new TCHAR[200];

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("title_mask_2"), &data))
		strTitleMask = data;
	else
	{
//		if ((winamp_version >= 0x2090) || (winamp_version >= 0x2900))
//			strTitleMask = "%number%. %artist% - %title% - %time%"; // does not work with tagless files
//		else
			strTitleMask = _T("%number%. %track% - %time%"); // works with and without tags
	}

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("search_caption"), &data))
		strSearchCaption = data;
	else
		strSearchCaption.Empty();

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("remaining_time"), &data))
		show_remaining_time = (_tcstol(data, NULL, 10) != 0);
	else
		show_remaining_time = true;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("song_length"), &data))
		show_song_length = (_tcstol(data, NULL, 10) != 0);
	else
		show_song_length = false;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_name"), &data))
	{
		_tcsncpy(lfont.lfFaceName, data, LF_FACESIZE);
		lfont.lfFaceName[LF_FACESIZE - 1] = '\0';
	}
	else
		_tcsncpy(lfont.lfFaceName, _T("Arial"), LF_FACESIZE);

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_height"), &data))
		lfont.lfHeight = _tcstol(data, NULL, 10);
	else
		lfont.lfHeight = -11;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_weight"), &data))
		lfont.lfWeight = _tcstol(data, NULL, 10);
	else
		lfont.lfWeight = FW_BOLD;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_italic"), &data))
		lfont.lfItalic = (unsigned char)_tcstoul(data, NULL, 10);
	else
		lfont.lfItalic = false;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_underline"), &data))
		lfont.lfUnderline = (unsigned char)_tcstoul(data, NULL, 10);
	else
		lfont.lfUnderline = false;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_strikeout"), &data))
		lfont.lfStrikeOut = (unsigned char)_tcstoul(data, NULL, 10);
	else
		lfont.lfStrikeOut = false;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_color"), &data))
		color = _tcstoul(data, NULL, 10);
	else
		color = GetSysColor(COLOR_CAPTIONTEXT);

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("show_playstatus"), &data))
		show_playstatus = (_tcstol(data, NULL, 10) != 0);
	else
		show_playstatus = true;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("plugin_enabled"), &data))
		plugin_enabled = (_tcstol(data, NULL, 10) != 0);
	else
		plugin_enabled = true;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("shift_horizontal"), &data))
		horizontalshift = _tcstol(data, NULL, 10);
	else
		horizontalshift = 0;

	if (get_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("shift_vertical"), &data))
		verticalshift = _tcstol(data, NULL, 10);
	else
		verticalshift = 0;

	// Set font attributes that cannot be changed
	lfont.lfWidth			= 0;
	lfont.lfEscapement		= 0;
	lfont.lfOrientation		= 0;
	lfont.lfCharSet			= DEFAULT_CHARSET;
	lfont.lfOutPrecision	= OUT_STRING_PRECIS;
	lfont.lfClipPrecision	= CLIP_STROKE_PRECIS;
	lfont.lfQuality			= DRAFT_QUALITY;
	lfont.lfPitchAndFamily	= VARIABLE_PITCH | FF_DONTCARE;

	pInfo = new TitleInfo;
	pInfo->setSearchCaption(strSearchCaption);

	pGenTbar = this;

	uTimerID = SetTimer(NULL, 0, 1000, TimerProc);
}

gen_tbar::~gen_tbar()
{
	KillTimer(NULL, uTimerID);

	pGenTbar = NULL;

	delete pInfo;
	pInfo = NULL;

	// Save data in registry
	_stprintf(data, _T("%s"), strTitleMask.GetBuffer(512));		// "%s" must be a pointer to char!
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("title_mask_2"), data);

	_stprintf(data, _T("%s"), strSearchCaption.GetBuffer(512));	// "%s" must be a pointer to char!
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("search_caption"), data);

	_stprintf(data, _T("%d"), show_remaining_time);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("remaining_time"), data);

	_stprintf(data, _T("%d"), show_song_length);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("song_length"), data);

	_stprintf(data, _T("%s"), lfont.lfFaceName);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_name"), data);

	_stprintf(data, _T("%d"), lfont.lfHeight);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_height"), data);

	_stprintf(data, _T("%d"), lfont.lfWeight);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_weight"), data);

	_stprintf(data, _T("%d"), lfont.lfItalic);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_italic"), data);

	_stprintf(data, _T("%d"), lfont.lfUnderline);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_underline"), data);

	_stprintf(data, _T("%d"), lfont.lfStrikeOut);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_strikeout"), data);

	_stprintf(data, _T("%d"), color);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("font_color"), data);

	_stprintf(data, _T("%d"), show_playstatus);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("show_playstatus"), data);

	_stprintf(data, _T("%d"), plugin_enabled);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("plugin_enabled"), data);

	_stprintf(data, _T("%d"), horizontalshift);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("shift_horizontal"), data);

	_stprintf(data, _T("%d"), verticalshift);
	set_registry_string(HKEY_CURRENT_USER, _T("Software\\radix\\gen_tbar"), _T("shift_vertical"), data);

	delete[] data;
	data = NULL;
}

void gen_tbar::config()
{
	// Open dialog box for configuration
	DialogBox(hDllInstance, MAKEINTRESOURCE(IDD_DIALOG), hWndParent, ConfigProc);
}

bool gen_tbar::config_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)							// Initialisation of dialog box
	{
		SetWindowText(hwndDlg, strVersion);

		if ((winamp_version >= 0x2090) || (winamp_version >= 0x2900))
		{
			EnableWindow(GetDlgItem(hwndDlg, IDC_STATIC_WINAMP_5_1), true);
			EnableWindow(GetDlgItem(hwndDlg, IDC_STATIC_WINAMP_5_2), true);
		}

		show_remaining_time_old = show_remaining_time;
		show_song_length_old	= show_song_length;
		show_playstatus_old		= show_playstatus;
		plugin_enabled_old		= plugin_enabled;

		color_old = color;
		lfont_old = lfont;

		horizontalshift_old	= horizontalshift;
		verticalshift_old	= verticalshift;

		CheckDlgButton(hwndDlg, IDC_SHOW_REMAINING_TIME, show_remaining_time_old);
		CheckDlgButton(hwndDlg, IDC_SHOW_SONG_LENGTH, show_song_length_old);
		CheckDlgButton(hwndDlg, IDC_SHOW_PLAYSTATUS, show_playstatus_old);

		CheckDlgButton(hwndDlg, IDC_PLUGIN_ENABLED, plugin_enabled_old);
		SetWindowText(GetDlgItem(hwndDlg, IDC_PLUGIN_ENABLED), plugin_enabled?_T("Plugin en&abled"):_T("Plugin dis&abled"));

		_stprintf(data, _T("%s"), lfont.lfFaceName);
		HWND hStaticDialog = GetDlgItem(hwndDlg, IDC_CHOOSE_FONT);
		SetWindowText(hStaticDialog, data);

		HWND hHorSpin = GetDlgItem(hwndDlg, IDC_HSPIN);
		SendMessage(hHorSpin, UDM_SETRANGE, 0L, MAKELONG(250, -250));
		SendMessage(hHorSpin, UDM_SETPOS,   0L, MAKELONG(horizontalshift_old, 0));

		HWND hVerSpin = GetDlgItem(hwndDlg, IDC_VSPIN);
		SendMessage(hVerSpin, UDM_SETRANGE, 0L, MAKELONG(20, -20));
		SendMessage(hVerSpin, UDM_SETPOS,   0L, MAKELONG(verticalshift_old, 0));

		HWND hSearchCaption = GetDlgItem(hwndDlg, IDC_SEARCH_CAPTION);
		SendMessage(hSearchCaption, EM_SETLIMITTEXT, (WPARAM) SEARCH_CAPTION_TEXT, 0L);
		SetWindowText(hSearchCaption, strSearchCaption);

		strTitleMaskOld = strTitleMask;

		HWND hTitleMask = GetDlgItem(hwndDlg, IDC_TITLE_MASK);
		SendMessage(hTitleMask, EM_SETLIMITTEXT, (WPARAM) TITLE_MASK_TEXT, 0L);
		SetWindowText(hTitleMask, strTitleMask);
	}
	else if ((uMsg == WM_VSCROLL) && (LOWORD(wParam) == SB_THUMBPOSITION))
	{
		HWND hwndScrollBar = (HWND) lParam; // handle to scroll bar

		if (hwndScrollBar == GetDlgItem(hwndDlg, IDC_HSPIN))
			horizontalshift = (short int) HIWORD(wParam);
		else if (hwndScrollBar == GetDlgItem(hwndDlg, IDC_VSPIN))
			verticalshift = (short int) HIWORD(wParam);
	}
	else if (uMsg == WM_HELP)
	{
		CStdString strContextHelpFile = strHelpFile + "::/popup.txt";

		LPHELPINFO lphi = (LPHELPINFO) lParam;
		if (lphi->iContextType == HELPINFO_WINDOW)
			HtmlHelp((HWND) lphi->hItemHandle, strContextHelpFile, HH_TP_HELP_WM_HELP, (DWORD) id_array) ;
	}
	else if (uMsg == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
			case IDC_SHOW_REMAINING_TIME:	// Check box "Show remaining time" has been (un)checked
			{
				show_remaining_time = (IsDlgButtonChecked(hwndDlg,IDC_SHOW_REMAINING_TIME) == BST_CHECKED?1:0);
				break;
			}
			case IDC_SHOW_SONG_LENGTH:	// Check box "Show song length" has been (un)checked
			{
				show_song_length = (IsDlgButtonChecked(hwndDlg,IDC_SHOW_SONG_LENGTH) == BST_CHECKED?1:0);
				break;
			}
			case IDC_SHOW_PLAYSTATUS:		// Check box "Show playback status" has been (un)checked
			{
				show_playstatus = (IsDlgButtonChecked(hwndDlg,IDC_SHOW_PLAYSTATUS) == BST_CHECKED?1:0);
				break;
			}
			case IDC_PLUGIN_ENABLED:		// Check box "Plugin ***abled" has been (un)checked
			{
				plugin_enabled = (IsDlgButtonChecked(hwndDlg,IDC_PLUGIN_ENABLED) == BST_CHECKED?1:0);
				SetWindowText(GetDlgItem(hwndDlg, IDC_PLUGIN_ENABLED), plugin_enabled?_T("Plugin en&abled"):_T("Plugin dis&abled"));
				break;
			}
			case IDC_TITLE_MASK_RESET:		// Button "Reset" has been pressed
			{
				HWND hTitleMask = GetDlgItem(hwndDlg, IDC_TITLE_MASK);
				strTitleMask = _T("%number%. %track% - %time%");
//				if ((winamp_version >= 0x2090) || (winamp_version >= 0x2900))
//					SetWindowText(hTitleMask, "%number%. %artist% - %title% - %time%"); // does not work with tagless files
//				else
					SetWindowText(hTitleMask, _T("%number%. %track% - %time%")); // works with and without tags

				break;
			}
			case IDC_TITLE_MASK:			// Content of IDC_TITLE_MASK has changed
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					HWND hTitleMask = GetDlgItem(hwndDlg, IDC_TITLE_MASK);
					GetWindowText(hTitleMask, strTitleMask.GetBuffer(TITLE_MASK_TEXT + 1), TITLE_MASK_TEXT);
					strTitleMask.ReleaseBuffer();
				}
				break;
			}
			case IDC_GET_HELP:
			{
				HtmlHelp(NULL, strHelpFile, HH_DISPLAY_TOPIC, 0);
				break;
			}
			case IDC_ABOUT:					// Button "About" has been pressed
			{
				MessageBox(hWndParent, _T("Copyright (c) 1996-1997 Mats Ljungqvist (mlt@cyberdude.com)\n")
					_T("Copyright (c) 1999-2000 Jos van den Oever (http://www.vandenoever.info/)\n")
					_T("Copyright (c) 2001-2005 Martin Zuther (http://www.mzuther.de/)\n\n")

					_T("This program is free software; you can redistribute it and/or modify\n")
					_T("it under the terms of the GNU General Public License as published by\n")
					_T("the Free Software Foundation; either version 2 of the License, or\n")
					_T("(at your option) any later version.\n\n")

					_T("This program is distributed in the hope that it will be useful,\n")
					_T("but WITHOUT ANY WARRANTY; without even the implied warranty of\n")
					_T("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n")
					_T("GNU General Public License for more details.\n\n")

					_T("You should have received a copy of the GNU General Public License\n")
					_T("along with this program; if not, write to the Free Software\n")
					_T("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\n")

					_T("Thanks to anyone who sent ideas or bug reports."),
					strVersion, MB_OK|MB_APPLMODAL);
				break;
			}
			case IDC_CHOOSE_FONT:			// Button "Font" has been pressed
			{
				CHOOSEFONT cf;

				cf.lStructSize	= sizeof(CHOOSEFONT);
				cf.hwndOwner	= hwndDlg;
				cf.Flags		= CF_FORCEFONTEXIST | CF_SCREENFONTS | CF_LIMITSIZE | CF_INITTOLOGFONTSTRUCT | CF_EFFECTS;
				cf.lpLogFont	= &lfont;
				cf.nSizeMin		= 6;
				cf.nSizeMax		= 14;
				cf.rgbColors	= color;
				if (ChooseFont(&cf))		// "Ok" button was pressed in font dialog
				{
					color = cf.rgbColors;
					_stprintf(data, _T("%s"), lfont.lfFaceName);
					HWND hStaticDialog = GetDlgItem(hwndDlg, IDC_CHOOSE_FONT);
					SetWindowText(hStaticDialog, data);
				}
				break;
			}
			case IDOK:						// "Ok" button was pressed in dialog box
			{
				HWND hSearchCaption = GetDlgItem(hwndDlg, IDC_SEARCH_CAPTION);
				GetWindowText(hSearchCaption, strSearchCaption.GetBuffer(SEARCH_CAPTION_TEXT + 1), SEARCH_CAPTION_TEXT);
				strSearchCaption.ReleaseBuffer();

				HWND hTitleMask = GetDlgItem(hwndDlg, IDC_TITLE_MASK);
				GetWindowText(hTitleMask, strTitleMask.GetBuffer(TITLE_MASK_TEXT + 1), TITLE_MASK_TEXT);
				strTitleMask.ReleaseBuffer();

				pInfo->setSearchCaption(strSearchCaption);

				EndDialog(hwndDlg,0);
				break;
			}
			case IDCANCEL:					// "Cancel" button was pressed in dialog box
			{
				show_remaining_time	= show_remaining_time_old;
				show_song_length	= show_song_length_old;
				show_playstatus		= show_playstatus_old;
				plugin_enabled		= plugin_enabled_old;

				horizontalshift	= horizontalshift_old;
				verticalshift	= verticalshift_old;

				lfont = lfont_old;
				color = color_old;

				strTitleMask = strTitleMaskOld;

				EndDialog(hwndDlg,0);
				break;
			}
		}
	}

	return false;
}

bool gen_tbar::get_song_data()
{
	strTitle = strTitleMask;

	// IPC_GETLISTPOS returns the playlist position
	int track_number = SendMessage(hWndParent, WM_WA_IPC, 0, IPC_GETLISTPOS);

	// IPC_GETPLAYLISTTITLE gets the title of the playlist entry [index]
#ifdef _MBCS
	CStdString strTrackName = (char*) SendMessage(hWndParent, WM_WA_IPC, track_number, IPC_GETPLAYLISTTITLE);
#else
	CStdString strTrackName = (wchar_t*) SendMessage(hWndParent, WM_WA_IPC, track_number, IPC_GETPLAYLISTTITLEW);
#endif

	if ((winamp_version >= 0x2090) || (winamp_version >= 0x2900)) // buggy because of lame unicode support !!!
	{
#ifdef _MBCS
		extendedFileInfoStruct ExtFileInfoStruct;

		ExtFileInfoStruct.filename = (char*) SendMessage(hWndParent, WM_WA_IPC, track_number, IPC_GETPLAYLISTFILE);
#else
		extendedFileInfoStructW ExtFileInfoStruct;

		ExtFileInfoStruct.filename = (wchar_t*) SendMessage(hWndParent, WM_WA_IPC, track_number, IPC_GETPLAYLISTFILEW);
#endif


		if (strTitle.Find(_T("%artist%")) != -1)
		{
			CStdString strArtist;

			ExtFileInfoStruct.metadata	= _T("artist");
			ExtFileInfoStruct.ret		= strArtist.GetBuffer(TEXTLENGTH);
			ExtFileInfoStruct.retlen	= TEXTLENGTH;

#ifdef _MBCS
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFO);
#else
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFOW);
#endif

			strArtist.ReleaseBuffer();
			strTitle.Replace(_T("%artist%"), strArtist);
		}

		if (strTitle.Find(_T("%album%")) != -1)
		{
			CStdString strAlbum;

			ExtFileInfoStruct.metadata	= _T("album");
			ExtFileInfoStruct.ret		= strAlbum.GetBuffer(TEXTLENGTH);
			ExtFileInfoStruct.retlen	= TEXTLENGTH;

#ifdef _MBCS
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFO);
#else
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFOW);
#endif

			strAlbum.ReleaseBuffer();
			strTitle.Replace(_T("%album%"), strAlbum);
		}

		if (strTitle.Find(_T("%title%")) != -1)
		{
			CStdString strSongTitle;

			ExtFileInfoStruct.metadata	= _T("title");
			ExtFileInfoStruct.ret		= strSongTitle.GetBuffer(TEXTLENGTH);
			ExtFileInfoStruct.retlen	= TEXTLENGTH;

#ifdef _MBCS
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFO);
#else
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFOW);
#endif

			strSongTitle.ReleaseBuffer();
			strTitle.Replace(_T("%title%"), strSongTitle);
		}

		if (strTitle.Find(_T("%genre%")) != -1)
		{
			CStdString strGenre;

			ExtFileInfoStruct.metadata	= _T("genre");
			ExtFileInfoStruct.ret		= strGenre.GetBuffer(TEXTLENGTH);
			ExtFileInfoStruct.retlen	= TEXTLENGTH;

#ifdef _MBCS
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFO);
#else
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFOW);
#endif

			strGenre.ReleaseBuffer();
			strTitle.Replace(_T("%genre%"), strGenre);
		}

		if (strTitle.Find(_T("%year%")) != -1)
		{
			CStdString strYear;

			ExtFileInfoStruct.metadata	= _T("year");
			ExtFileInfoStruct.ret		= strYear.GetBuffer(TEXTLENGTH);
			ExtFileInfoStruct.retlen	= TEXTLENGTH;

#ifdef _MBCS
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFO);
#else
			SendMessage(hWndParent, WM_WA_IPC, (WPARAM) &ExtFileInfoStruct, IPC_GET_EXTENDED_FILE_INFOW);
#endif

			strYear.ReleaseBuffer();
			strTitle.Replace(_T("%year%"), strYear);
		}
	}

	// Prevent further mischief in case of error
	if ((strTrackName == NULL) || strTrackName.IsEmpty())
	{
		strTitle.Empty();
		strTime.Empty();

		return false;
	}
	else
	{
		int play_status;

		if (show_playstatus)
		{
			// IPC_ISPLAYING returns the status of playback
			play_status = SendMessage(hWndParent, WM_WA_IPC, 0, IPC_ISPLAYING);	// Paused: 3
		}
		else
			play_status = 1; // "[Pause]" or [Stop] are not displayed

		switch (play_status)
		{
			case 1:		// Winamp is playing: get current play time and song length from Winamp
			{
				// IPC_GETOUTPUTTIME returns the position of the current song in milliseconds (mode 0)
				int song_position = int ((SendMessage(hWndParent, WM_WA_IPC, 0, IPC_GETOUTPUTTIME) / 1000.0) + 0.5); // convert to seconds
				// IPC_GETOUTPUTTIME returns the length of the current song in seconds (mode 1)
				int song_length   = SendMessage(hWndParent, WM_WA_IPC, 1, IPC_GETOUTPUTTIME); // seconds

				if (show_remaining_time) // Show remaining time or play time
					song_position = song_length - song_position;

				if (song_position < 0) // Track has been written (i.e. encoded) while Winamp already played it
				{
					song_position = abs(song_position);
					strTime.Format(_T("[%02d:%02d]"), song_position/60, song_position%60);
				}
				else
				{
					if (show_song_length) // Show song length after song position
						strTime.Format(_T("%02d:%02d (%02d:%02d)"), song_position/60, song_position%60, song_length/60, song_length%60);
					else
						strTime.Format(_T("%02d:%02d"), song_position/60, song_position%60);
				}
				break;
			}
			case 3:		// Playback is currently paused
			{
				strTime = "Pause";
				break;
			}
			default:	// Playback is currently stopped
			{
				strTime = "Stop";
				break;
			}
		}
	}

	CStdString strTrackNumber;
	strTrackNumber.Format(_T("%02d"), track_number + 1);

	strTitle.Replace(_T("%number%"), strTrackNumber);

	if (strTitle.Find(_T("%total%")) != -1)
	{
		// IPC_GETLISTLENGTH returns the length of the current playlist in tracks
		int total_track_number = SendMessage(hWndParent, WM_WA_IPC, 0, IPC_GETLISTLENGTH);

		CStdString strTotalTrackNumber;
		strTotalTrackNumber.Format(_T("%02d"), total_track_number);

		strTitle.Replace(_T("%total%"), strTotalTrackNumber);
	}

	strTitle.Replace(_T("%track%"), strTrackName);
	strTitle.Replace(_T("%time%"), strTime);

	return true;
}

void gen_tbar::draw_titlebar()
{
	if (pInfo && plugin_enabled && get_song_data())
	{
		pInfo->setText(strTitle);
		pInfo->setFont(lfont);
		pInfo->setFontColor(color);
		pInfo->setFontShift(horizontalshift, verticalshift);
		pInfo->setShowPlayStatus(show_playstatus);
		pInfo->drawOnCaption();
	}
}

bool gen_tbar::get_registry_string(HKEY root_key, TCHAR* sub_key, TCHAR* key_name, TCHAR** data)
{
	HKEY hRegistry;
	DWORD dwType = REG_SZ;
	DWORD dwSize = 199;

	RegOpenKeyEx(root_key, sub_key, NULL, KEY_EXECUTE, &hRegistry);
	LONG rueck = RegQueryValueEx(hRegistry, key_name, NULL, &dwType, (BYTE*)*data, &dwSize);
	RegCloseKey(hRegistry);
	hRegistry = NULL;

	return (rueck == ERROR_SUCCESS);
}

bool gen_tbar::set_registry_string(HKEY root_key, TCHAR* sub_key, TCHAR* key_name, TCHAR* data)
{
	TCHAR* registry_data = new TCHAR[200];
	_tcsncpy(registry_data, data, 200);
	registry_data[199] = '\0';

	HKEY hRegistry;
	DWORD dwSize = 199;
	DWORD dwType = REG_SZ;
	RegCreateKeyEx(root_key, sub_key, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegistry, NULL);

	dwSize = 199;
	dwType = REG_SZ;
	LONG rueck = RegSetValueEx(hRegistry, key_name, NULL, dwType, (BYTE*)registry_data, (_tcslen(registry_data) + 1)* sizeof(TCHAR));
	RegCloseKey(hRegistry);
	hRegistry = NULL;

	delete[] registry_data;
	registry_data = NULL;

	return (rueck == ERROR_SUCCESS);
}
