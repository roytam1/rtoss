/*
 *  GSPlayer - The audio player for WindowsCE
 *  Copyright (C) 2003  Y.Nagamidori
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef __GSPLAYER2_H__
#define __GSPLAYER2_H__

#include "resource.h"
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>

#ifdef _WIN32_WCE_PPC
#include <aygshell.h>
#define MENU_HEIGHT		26
#endif

#include "maplay.h"
#include "PlayerApp.h"

#ifdef _WIN32_WCE
#include <stdlib.h>
#include <notify.h>
#else
#include <stdio.h>
#endif

extern "C"
{
	CPlayerApp* GetPlayerAppClass();
	HINSTANCE GetInst();
	//CMainWnd* GetMainWnd();
	void SendCmdLine(HWND hwndTo, LPTSTR pszCmdLine);

	BOOL DefDlgPaintProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
	BOOL DefDlgCtlColorStaticProc(HWND hDlg, WPARAM wParam, LPARAM lParam);
	void ShellInitDialog(HWND hwndDlg);
	int CALLBACK PropSheetCallback(HWND hwndDlg, UINT message, LPARAM lParam);
	BOOL GetKeyString(FILE* fp, LPCTSTR pszSection, LPCTSTR pszKey, LPTSTR pszValue);
	BOOL IsDefSupportedExt(LPTSTR pszExt, BOOL fPlayList = TRUE);

#ifdef _WIN32_WCE_PPC
	BOOL IsLandscape();
#endif
}

class CTempStr
{
#define MAX_LOADSTR		512
public:
	CTempStr(UINT uID = 0)
	{
		if (uID)
			Load(uID);
	}
	void Load(UINT uID)
	{
		LoadString(GetInst(), uID, m_szStr, MAX_LOADSTR);

		LPTSTR psz = m_szStr;
		while (psz = _tcschr(psz, _T('|'))) {
			*psz++ = NULL;
		}
	}
	void LoadNormal(UINT uID)
	{
		LoadString(GetInst(), uID, m_szStr, MAX_LOADSTR);
	}
	operator LPTSTR() {return m_szStr;}
protected:
	TCHAR m_szStr[MAX_LOADSTR];
};

extern const TCHAR s_szDefSupportedExt[][5];
extern const TCHAR s_szDefSupportedExt2[][5];

#define RECT_WIDTH(prc)		((prc)->right - (prc)->left)
#define RECT_HEIGHT(prc)	((prc)->bottom - (prc)->top)

#endif // __GSPLAYER2_H__