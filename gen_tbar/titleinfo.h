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

// class TitleInfo

#ifndef TITLEINFO_H
#define TITLEINFO_H

#include "stdafx.h"
#include "resource.h"

#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include "StdString.h"

class TitleInfo
{
public:
	TitleInfo();
	~TitleInfo();
	void drawOnCaption();
	void setText(CStdString newTitleText);
	void setSearchCaption(CStdString newSearchCaption);
	bool isStringInCaption(CStdString WindowText);
	void setShowPlayStatus(bool newstatus)							{ bShowPlayStatus = newstatus; }
	void setFont(LOGFONT lfont)										{ sCaptionFont = lfont; }
	void setFontColor(const DWORD newcolor)							{ nCaptionFontColor = newcolor; }
	void setFontShift(int newHorizontalShift, int newVerticalShift)	{ nHorizontalShift = newHorizontalShift; nVerticalShift   = newVerticalShift; }

private:
	DWORD   nCaptionFontColor;
	LOGFONT sCaptionFont;
	HWND    hLastForegroundWindow;

	int nHorizontalShift;
	int nVerticalShift;

	CStdString* pSearchCaption;

	CStdString strTitleText;

	bool bShowPlayStatus;

	void drawCaption(HWND hForegroundWnd, unsigned int nStyle);
	void updateCaption(HWND hWnd);
};

#endif
