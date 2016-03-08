/**
 * @file	d_wab.cpp
 * @brief	Window Accelerator Board configure dialog procedure
 *
 * @author	$Author: SimK $
 * @date	$Date: 2016/03/08 $
 */

#include "compiler.h"
#include <commctrl.h>
#include <prsht.h>
#include "strres.h"
#include "resource.h"
#include "np2.h"
#include "dosio.h"
#include "misc\tstring.h"
#include "joymng.h"
#include "sysmng.h"
#include "np2class.h"
#include "dialog.h"
#include "dialogs.h"
#include "pccore.h"
#include "iocore.h"
#include "video/video.h"

#if !defined(__GNUC__)
#pragma comment(lib, "comctl32.lib")
#endif	// !defined(__GNUC__)

static LRESULT CALLBACK CLGD5430optDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	//HWND	sub;
	HWND	hItem;
	//int		cur;
#if defined(SUPPORT_CL_GD5430)

	switch(msg) {
		case WM_INITDIALOG:
			//SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
			//				(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			
			hItem = GetDlgItem(hWnd, IDC_GD5430ENABLED);
			if(np2cfg.usegd5430)
				SendMessage(hItem , BM_SETCHECK , BST_CHECKED , 0);
			else
				SendMessage(hItem , BM_SETCHECK , BST_UNCHECKED , 0);
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_GD5430ENABLED:
					hItem = GetDlgItem(hWnd, IDC_GD5430ENABLED);
					np2cfg.usegd5430 = (SendMessage(hItem , BM_GETCHECK , 0 , 0) ? 1 : 0);
					break;
			}
			break;

		case WM_NOTIFY:
			break;
	}
#endif
	return(FALSE);
}

void dialog_wabopt(HWND hWnd)
{
	HINSTANCE		hInstance;
	PROPSHEETPAGE	psp;
	PROPSHEETHEADER	psh;
	HPROPSHEETPAGE	hpsp[1];
	
#if defined(SUPPORT_LGY98)
	hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

	ZeroMemory(&psp, sizeof(psp));
	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.dwFlags = PSP_DEFAULT;
	psp.hInstance = hInstance;

	psp.pszTemplate = MAKEINTRESOURCE(IDD_GD5430);
	psp.pfnDlgProc = (DLGPROC)CLGD5430optDlgProc;
	hpsp[0] = CreatePropertySheetPage(&psp);

	std::tstring rTitle(LoadTString(IDS_WABOPTION));

	ZeroMemory(&psh, sizeof(psh));
	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	psh.hwndParent = hWnd;
	psh.hInstance = hInstance;
	psh.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	psh.nPages = 1;
	psh.phpage = hpsp;
	psh.pszCaption = rTitle.c_str();
	psh.pfnCallback = np2class_propetysheet;
	PropertySheet(&psh);
	InvalidateRect(hWnd, NULL, TRUE);
#endif
}
