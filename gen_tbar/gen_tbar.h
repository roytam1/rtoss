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

#ifndef GEN_TBAR_H
#define GEN_TBAR_H

#include <windows.h>
#include "Winamp SDK/wa_ipc.h"

#include "titleinfo.h"
#include "Help\popup.h"
#include <htmlhelp.h>
#include <Shlwapi.h>

#define SEARCH_CAPTION_TEXT  64
#define TITLE_MASK_TEXT     128
#define TEXTLENGTH          512

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

class gen_tbar
{
public:
	gen_tbar(HINSTANCE h_dll_instance, HWND hwnd_parent, CStdString help_file, CStdString plugin_version);
	~gen_tbar();

	void config();
	void draw_titlebar();
	bool enabled() { return plugin_enabled; }

	bool gen_tbar::config_proc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	bool get_song_data();

	bool get_registry_string(HKEY root_key, char* sub_key, char* key_name, char** data);
	bool set_registry_string(HKEY root_key, char* sub_key, char* key_name, char* data);

	UINT uTimerID;

	HINSTANCE hDllInstance;	// plugin data
	HWND hWndParent;		// plugin data

	TitleInfo* pInfo;

	LOGFONT lfont;
	LOGFONT lfont_old;

	int winamp_version;
	int verticalshift;
	int verticalshift_old;
	int horizontalshift;
	int horizontalshift_old;

	DWORD color;
	DWORD color_old;

	char* data;

	CStdString strHelpFile;
	CStdString strSearchCaption;
	CStdString strVersion;

    CStdString strTime;
    CStdString strTitle;
    CStdString strTitleMask;
    CStdString strTitleMaskOld;

	bool plugin_enabled;
	bool plugin_enabled_old;
	bool show_remaining_time;
	bool show_remaining_time_old;
	bool show_song_length;
	bool show_song_length_old;
	bool show_playstatus;
	bool show_playstatus_old;
};

BOOL CALLBACK ConfigProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

static DWORD id_array[] =
{
	IDOK,                    IDH_OK,
	IDCANCEL,                IDH_CANCEL,
	IDC_SHOW_REMAINING_TIME, IDH_SHOW_REMAINING_TIME,
	IDC_SHOW_SONG_LENGTH,	 IDH_SHOW_SONG_LENGTH,
	IDC_CHOOSE_FONT,         IDH_CHOOSE_FONT,
	IDC_SHOW_PLAYSTATUS,     IDH_SHOW_PLAYSTATUS,
	IDC_GET_HELP,            IDH_GET_HELP,
	IDC_ABOUT,               IDH_ABOUT,
	IDC_HSPIN,               IDH_HSPIN,
	IDC_HEDIT,               IDH_HEDIT,
	IDC_VEDIT,               IDH_VEDIT,
	IDC_VSPIN,               IDH_VSPIN,
	IDC_SEARCH_CAPTION,      IDH_SEARCH_CAPTION,
	IDC_PLUGIN_ENABLED,      IDH_PLUGIN_ENABLED,
	IDC_TITLE_MASK,          IDH_TITLE_MASK,
	IDC_TITLE_MASK_RESET,    IDH_TITLE_MASK_RESET,
	0, 0
};

#endif // GEN_TBAR_H
