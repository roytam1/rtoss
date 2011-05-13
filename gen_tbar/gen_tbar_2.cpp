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

// gen_tbar_2.cpp : Defines the entry point for the DLL application.
//

#include "gen_tbar_2.h"

gen_tbar* p_gen_tbar;

BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

winampGeneralPurposePlugin plugin =
{
	GPPHDR_VER,
	"Titlebar Info",
	init,
	config,
	quit,
};

int init()
{
	HINSTANCE hInstance = plugin.hDllInstance;
	HWND hWndParent     = plugin.hwndParent;

	// Get version information from DLL
	CStdString strHelpFile;
	CStdString strVersion;

	GetModuleFileName(hInstance, strHelpFile.GetBuffer(MAX_PATH), MAX_PATH);
	strHelpFile.ReleaseBuffer();

	strVersion = strHelpFile;
	PathRemoveFileSpec(strHelpFile.GetBuffer(MAX_PATH));
	strHelpFile.ReleaseBuffer();
	strHelpFile += _T("\\gen_tbar.chm");

	DWORD dwArg;
	DWORD dwInfoSize = GetFileVersionInfoSize(strVersion.GetBuffer(MAX_PATH), &dwArg);
	strVersion.ReleaseBuffer();

	BYTE* lpBuffer = new BYTE[dwInfoSize];
	GetFileVersionInfo(strVersion.GetBuffer(MAX_PATH), NULL, dwInfoSize, lpBuffer);
	strVersion.ReleaseBuffer();

	UINT uInfoSize;
	LPVOID lpValue;

	if (VerQueryValue(lpBuffer, TEXT("\\StringFileInfo\\040904b0\\FileVersion"), &lpValue, &uInfoSize))
		strVersion.Format(_T("Titlebar Info %s"), lpValue);
	else
		strVersion = _T("Titlebar Info x.xx");

	delete[] lpBuffer;
	lpBuffer = NULL;

#ifndef _MBCS
	strVersion += " [Unicode]";
#endif

	// Set plugin description
	static char c[512];

	CStdString strFilename;
	GetModuleFileName(hInstance, strFilename.GetBuffer(MAX_PATH), MAX_PATH);
	strFilename.ReleaseBuffer();
	std::string strVersionA, strFilenameA;
	ssasn(strVersionA, strVersion.GetBuffer(MAX_PATH));
	ssasn(strFilenameA, PathFindFileName(strFilename.GetBuffer(MAX_PATH)));

	wsprintfA((c), "%s (%s)", strVersionA.c_str(), strFilenameA.c_str()); // "%s" must be a pointer to char!
	plugin.description = c;

	strFilename.ReleaseBuffer();
	strVersion.ReleaseBuffer();

	p_gen_tbar = new gen_tbar(hInstance, hWndParent, strHelpFile, strVersion);

	return 0;
}

void quit()
{
	delete p_gen_tbar;
	p_gen_tbar = NULL;
}

void config()
{
	p_gen_tbar->config();
}

extern "C"
__declspec( dllexport ) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin()
{
	return &plugin;
}
