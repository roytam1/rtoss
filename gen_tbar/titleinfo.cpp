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

#include "titleinfo.h"

TitleInfo::TitleInfo()
{
	strTitleText.Empty();

	bShowPlayStatus = false;
	bODInited = false;

	pSearchCaption = new CStdString[10];

	for (int i=0; i < 10; i++)
		pSearchCaption[i].Empty();
}

TitleInfo::~TitleInfo()
{
	delete[] pSearchCaption;
	pSearchCaption = NULL;
}

void TitleInfo::updateCaption(HWND hWnd)
{
	CStdString strWindowText;
	GetWindowText(hWnd, strWindowText.GetBuffer(512), 512);
	strWindowText.ReleaseBuffer();

	if (!isStringInCaption(strWindowText))
	{
		#define PREVENT_XP_DISPLAY_BUG true

		if (PREVENT_XP_DISPLAY_BUG)
		{
			// Prevents "XP display bug", but behaves strange with browsers based on newer versions of Mozilla (don't ask me why...)
			SetWindowPos(hWnd, NULL, NULL, NULL, NULL, NULL, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
		}
		else
		{
			DefWindowProc(hWnd, WM_NCACTIVATE, (WPARAM) true, 0);
		}

	}
}

void TitleInfo::drawOnCaption()
{
	HWND hForegroundWnd = GetForegroundWindow();
	CStdString strName;
	unsigned int nStyle;
	unsigned int nExStyle;

	if (!hForegroundWnd || strTitleText.IsEmpty())
		return;

	// Find out the actual parent of this foreground window
	// in case it is a popup window with a caption
	if (GetParent(hForegroundWnd))
	{
		HWND hWnd;

		hWnd = GetParent(hForegroundWnd);
		while(hWnd)
		{
			hForegroundWnd = hWnd;
			hWnd = GetParent(hForegroundWnd);
		}
	}

	// Check if we are supposed to draw this info
	// the window must have a titlebar and must not be a toolwindow
	// it must not be setup.exe or winver.exe (#32770) and not
	// the Windows Taskbar (Shell_TrayWnd)
	// it also mustn't be a Winamp window (Winamp v1.x).
	GetClassName(hForegroundWnd, strName.GetBuffer(80), 80);
	strName.ReleaseBuffer();

	// patch from Darren Owen (problems with command line option "/class")
	if((strName != _T("Winamp PE")) && (strName != _T("Winamp v1.x")))
		if(FindWindowEx(hForegroundWnd,0,_T("WinampVis"),0))
			return;
	// patch end

	nStyle = GetWindowLong(hForegroundWnd, GWL_STYLE);
	nExStyle = GetWindowLong(hForegroundWnd, GWL_EXSTYLE);

	if ((nStyle & WS_CAPTION) == WS_CAPTION && !(nExStyle & WS_EX_TOOLWINDOW) && (strName != _T("#32770")) &&
		(strName != _T("Shell_TrayWnd")) && (strName != _T("Winamp v1.x")) && (strName != _T("Winamp EQ")) &&
		(strName != _T("Winamp PE")) && (strName != _T("Winamp MB"))
		 /*&& (strName != _T("Chrome_WidgetWin_0"))*/ ) // chrome hack
	{
		drawCaption(hForegroundWnd, nStyle);
	}
}

void TitleInfo::drawCaption(HWND hForegroundWnd, unsigned int nStyle)
{
	CStdString strWindowText;
	GetWindowText(hForegroundWnd, strWindowText.GetBuffer(512), 512);
	strWindowText.ReleaseBuffer();

	if (!isStringInCaption(strWindowText))
	{
		CStdString strTrackNumber;

		int nLeft;
		int nLeftSpace = 0;
		int nRightSpace = 10;
		int nDrawCaptionPos = 0;

		RECT sDrawRectTitle;
		RECT sRect;

		SIZE sTempSize;
		SIZE sTitleSize;

		NONCLIENTMETRICS sMetrics;
		HFONT hDrawFont;

		sMetrics.cbSize = sizeof(sMetrics);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &sMetrics, FALSE);

		// First we need to calculate the actual length of the active window text
		HDC hDC   = GetWindowDC(hForegroundWnd);
		hDrawFont = CreateFontIndirect(&sMetrics.lfCaptionFont);
		hDrawFont = (HFONT)SelectObject(hDC, hDrawFont);
		GetTextExtentPoint32(hDC, strWindowText, strWindowText.GetLength(), &sTempSize);
		hDrawFont = (HFONT)SelectObject(hDC, hDrawFont);
		DeleteObject(hDrawFont);

		// Now use the user defined font
		hDrawFont = CreateFontIndirect(&sCaptionFont);
		hDrawFont = (HFONT)SelectObject(hDC, hDrawFont);
		GetWindowRect(hForegroundWnd, &sRect);
		GetTextExtentPoint32(hDC, strTitleText, strTitleText.GetLength(), &sTitleSize);

		// Close button
		if ((nStyle & WS_SYSMENU) == WS_SYSMENU)
		{
			int nTemp        = GetSystemMetrics(SM_CXSIZE);
			nRightSpace     += nTemp;
			nLeftSpace      += nTemp;
		}

		if ((nStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX || (nStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX)
		{
			int nTemp        = GetSystemMetrics(SM_CXSIZE);
			nRightSpace     += 2 * nTemp;
		}

		if ((nStyle & WS_BORDER) == WS_BORDER && !((nStyle & WS_MAXIMIZE) == WS_MAXIMIZE))
		{
			int nTemp        = GetSystemMetrics(SM_CXBORDER);
			nRightSpace     += nTemp;
			nLeftSpace      += nTemp;
			nDrawCaptionPos += nTemp;
		}

		if ((nStyle & WS_THICKFRAME) == WS_THICKFRAME && !((nStyle & WS_MAXIMIZE) == WS_MAXIMIZE))
		{
			int nTemp        = GetSystemMetrics(SM_CXFRAME);
			nRightSpace     += nTemp;
			nLeftSpace      += nTemp;
			nDrawCaptionPos += nTemp;
		}

		if ((nStyle & WS_DLGFRAME) == WS_DLGFRAME && !((nStyle & WS_MAXIMIZE) == WS_MAXIMIZE))
		{
			int nTemp        = GetSystemMetrics(SM_CXDLGFRAME);
			nRightSpace     += nTemp;
			nLeftSpace      += nTemp;
			nDrawCaptionPos += nTemp;
		}

		nLeft = sRect.right - sRect.left - nRightSpace - sTitleSize.cx + nHorizontalShift;
		nLeftSpace -= nHorizontalShift;

		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, nCaptionFontColor);
		sDrawRectTitle.top = GetSystemMetrics(SM_CYFRAME) + (GetSystemMetrics(SM_CYCAPTION) - sTitleSize.cy) / 2 - 1;
		sDrawRectTitle.bottom = sDrawRectTitle.top + sTitleSize.cy;

		// Designates space between window title and winamp data written on caption
		int spacer = 25;

		if (nLeft > (sTempSize.cx + nLeftSpace + spacer))
		{
			sDrawRectTitle.left  = nLeft;
			sDrawRectTitle.right = sDrawRectTitle.left + sTitleSize.cx;
		}
		else
		{
			sDrawRectTitle.left  = sTempSize.cx + spacer + nLeftSpace;
			sDrawRectTitle.right = nLeft + sTitleSize.cx;
		}

		sDrawRectTitle.left   += nHorizontalShift;
		sDrawRectTitle.right  += nHorizontalShift;
		sDrawRectTitle.top    += nVerticalShift;
		sDrawRectTitle.bottom += nVerticalShift;

		int nTitleWidth = (sDrawRectTitle.right - sDrawRectTitle.left);

		if (nTitleWidth < sTitleSize.cx)
		{
			SIZE sTextSize;
			strTitleText += "...";
			int nStringLength = strTitleText.GetLength();
			GetTextExtentPoint32(hDC, strTitleText, nStringLength, &sTextSize);

			while (nTitleWidth < sTextSize.cx)
			{
				if (nStringLength > 8)
				{
					strTitleText.Delete(nStringLength - 4, 1);
					nStringLength--;
					GetTextExtentPoint32(hDC, strTitleText, nStringLength, &sTextSize);
				}
				else
				{
					strTitleText.Empty();
					break;
				}
			}
		}

		hLastForegroundWindow = hForegroundWnd;
		updateCaption(hForegroundWnd);

		/* titlebar area cleanup hack */
#if 0
		sDrawRectTitle.top    -= 5;
		sDrawRectTitle.left   -= 10;
		sDrawRectTitle.right  += 10;
		sDrawRectTitle.bottom += 5;
		RedrawWindow(hForegroundWnd, &sDrawRectTitle, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
		sDrawRectTitle.top    += 5;
		sDrawRectTitle.left   += 10;
		sDrawRectTitle.right  -= 10;
		sDrawRectTitle.bottom -= 5;
#endif
		if(bODInited) {
			rectOldDimension.top    -= 5;
			rectOldDimension.left   -= 10;
			rectOldDimension.right  += 10;
			rectOldDimension.bottom += 5;
			RedrawWindow(hForegroundWnd, &rectOldDimension, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
		} else {
			bODInited = true;
		}
		CopyRect(&rectOldDimension,&sDrawRectTitle);
		/* titlebar area cleanup hack end */

		DrawText(hDC, strTitleText, strTitleText.GetLength(), &sDrawRectTitle, DT_LEFT | DT_VCENTER | DT_NOPREFIX);

		hDrawFont = (HFONT)SelectObject(hDC, hDrawFont);
		DeleteObject(hDrawFont);
		ReleaseDC(hForegroundWnd, hDC);
	}
}

void TitleInfo::setText(CStdString newTitleText)
{
	strTitleText = newTitleText;
}

void TitleInfo::setSearchCaption(CStdString newSearchCaption)
{
	CStdString strSearchCaption;
	strSearchCaption = newSearchCaption;

	TCHAR* token = _tcstok(strSearchCaption.GetBuffer(512), _T(";"));
	strSearchCaption.ReleaseBuffer();

	for (int i=0; i < 10; i++)
	{
		if (token != NULL)
			pSearchCaption[i] = token;
		else
			pSearchCaption[i].Empty();

		token = _tcstok(NULL, _T(";"));
	}
}

bool TitleInfo::isStringInCaption(CStdString WindowText)
{
	for (int i=0; i < 10; i++)
	{
		if (!pSearchCaption[i].IsEmpty())					// pSearchCaption[i] is not empty
			if (WindowText.Find(pSearchCaption[i]) != -1)	// pSearchCaption[i] appears in pWindowText
				return true;								// the search is over
	}

	return false;											// nothing found
}
