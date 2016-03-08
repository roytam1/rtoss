/**
 * @file	d_network.cpp
 * @brief	Network configure dialog procedure
 *
 * @author	$Author: SimK $
 * @date	$Date: 2016/02/29 21:58:11 $
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
#include "network/net.h"
#include "network/lgy98.h"

#if !defined(__GNUC__)
#pragma comment(lib, "comctl32.lib")
#endif	// !defined(__GNUC__)

static const CBPARAM cpIOLGY98[] =
{
	{MAKEINTRESOURCE(IDS_00D0),		0x00},
	{MAKEINTRESOURCE(IDS_10D0),		0x01},
	{MAKEINTRESOURCE(IDS_20D0),		0x02},
	{MAKEINTRESOURCE(IDS_30D0),		0x03},
	{MAKEINTRESOURCE(IDS_40D0),		0x04},
	{MAKEINTRESOURCE(IDS_50D0),		0x05},
	{MAKEINTRESOURCE(IDS_60D0),		0x06},
	{MAKEINTRESOURCE(IDS_70D0),		0x07},
};

static const CBPARAM cpIrqLGY98[] =
{
	{MAKEINTRESOURCE(IDS_INT0),		 3},
	{MAKEINTRESOURCE(IDS_INT1),		 5},
	{MAKEINTRESOURCE(IDS_INT2),		 6},
	{MAKEINTRESOURCE(IDS_INT5),		12},
};

static UINT8 IRQ2Index(UINT8 irq){
	switch(irq){
	case 3:
		return 0;
	case 5:
		return 1;
	case 6:
		return 2;
	case 12:
		return 3;
	}
	return 0;
}

//UINT16 lgy98io = 0;
//UINT8  lgy98irq = 0;

//
//static UINT8 irq2int(UINT8 irq){
//	switch(irq){
//	case 3:
//	case 5:
//	case 6:
//	case 9:
//	case 10:
//	case 11:
//	case 12:
//	case 13:
//
//	}
//}
//static UINT8 int2irq(UINT8 intv){
//	switch(intv){
//	case 0:
//	case 1:
//	case 2:
//	case 3:
//	case 41:
//	case 42:
//	case 5:
//	case 6:
//
//	}
//}
//
//static void setlgy98io(HWND hWnd, UINT uID, UINT16 cValue)
//{
//	dlgs_setcbcur(hWnd, uID, cValue & 0x10);
//}
//static UINT16 getlgy98io(HWND hWnd, UINT uID)
//{
//	return dlgs_getcbcur(hWnd, uID, 0x10);
//}
//
//static void setlgy98irq(HWND hWnd, UINT uID, UINT8 cValue)
//{
//	dlgs_setcbcur(hWnd, uID, cValue & 0xc0);
//}
//static UINT8 getlgy98irq(HWND hWnd, UINT uID)
//{
//	return dlgs_getcbcur(hWnd, uID, 0xc0);
//}
//
//static void lgy98set(HWND hWnd, UINT16 cIO, UINT8 cIRQ)
//{
//	setlgy98io(hWnd, IDC_LGY98IO, cIO);
//	setlgy98irq(hWnd, IDC_LGY98INT, cIRQ);
//}

static LRESULT CALLBACK LGY98optDlgProc(HWND hWnd, UINT msg,
													WPARAM wp, LPARAM lp) {

	//HWND	sub;
	HWND	hItem;
	int		cur;
#if defined(SUPPORT_LGY98)

	switch(msg) {
		case WM_INITDIALOG:
			//dlgs_setcbnumber(hWnd, IDC_SND26IO, cpIOLGY98, NELEMENTS(cpIOLGY98));
			//dlgs_setcbnumber(hWnd, IDC_SND26INT, cpIrqLGY98, NELEMENTS(cpIrqLGY98));
			//lgy98io = np2cfg.lgy98io;
			//lgy98irq = np2cfg.lgy98irq;
			dlgs_setcbitem(hWnd, IDC_LGY98IO, cpIOLGY98, NELEMENTS(cpIOLGY98));
			dlgs_setcbitem(hWnd, IDC_LGY98INT, cpIrqLGY98, NELEMENTS(cpIrqLGY98));
			//lgy98set(hWnd, lgy98io, lgy98irq);
			//sub = GetDlgItem(hWnd, IDC_SND26JMP);
			//SetWindowLong(sub, GWL_STYLE, SS_OWNERDRAW +
			//				(GetWindowLong(sub, GWL_STYLE) & (~SS_TYPEMASK)));
			
			hItem = GetDlgItem(hWnd, IDC_LGY98ENABLED);
			if(np2cfg.uselgy98)
				SendMessage(hItem , BM_SETCHECK , BST_CHECKED , 0);
			else
				SendMessage(hItem , BM_SETCHECK , BST_UNCHECKED , 0);
			hItem = GetDlgItem(hWnd, IDC_LGY98IO);
			if((np2cfg.lgy98io >> 12) <  0) np2cfg.lgy98io = 0x00D0;
			if((np2cfg.lgy98io >> 12) >= NELEMENTS(cpIOLGY98)) np2cfg.lgy98io = (NELEMENTS(cpIOLGY98)<<12) | 0x00D0 ;
			SendMessage(hItem , CB_SETCURSEL , np2cfg.lgy98io>>12, 0);
			hItem = GetDlgItem(hWnd, IDC_LGY98INT);
			cur = IRQ2Index(np2cfg.lgy98irq);
			if(cur <  0) np2cfg.lgy98irq = cpIrqLGY98[0].nItemData;
			if(cur >= NELEMENTS(cpIrqLGY98)) np2cfg.lgy98irq = cpIrqLGY98[NELEMENTS(cpIrqLGY98)-1].nItemData;
			SendMessage(hItem , CB_SETCURSEL , cur , 0);
			return(TRUE);

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_LGY98ENABLED:
					hItem = GetDlgItem(hWnd, IDC_LGY98ENABLED);
					np2cfg.uselgy98 = (SendMessage(hItem , BM_GETCHECK , 0 , 0) ? 1 : 0);
					break;

				case IDC_LGY98IO:
					hItem = GetDlgItem(hWnd, IDC_LGY98IO);
					cur = SendMessage(hItem , CB_GETCURSEL , 0 , 0);
					if(0 <= cur && cur < NELEMENTS(cpIOLGY98)){
						np2cfg.lgy98io = (cur<<12) | 0x00D0;
					}
					//set26jmp(hWnd, getlgy98io(hWnd, IDC_LGY98IO), 0x10D0);
					break;

				case IDC_LGY98INT:
					hItem = GetDlgItem(hWnd, IDC_LGY98INT);
					cur = SendMessage(hItem , CB_GETCURSEL , 0 , 0);
					if(0 <= cur && cur < NELEMENTS(cpIrqLGY98)){
						np2cfg.lgy98irq = cpIrqLGY98[cur].nItemData;
					}
					//set26jmp(hWnd, getlgy98int(hWnd, IDC_LGY98INT), 0x05);
					break;

				case IDC_LGY98DEF:
					//snd26 = 0xd1;
					//snd26set(hWnd, snd26);
					//InvalidateRect(GetDlgItem(hWnd, IDC_SND26JMP), NULL, TRUE);
					hItem = GetDlgItem(hWnd, IDC_LGY98IO);
					np2cfg.lgy98io = 0x10D0 ;
					SendMessage(hItem , CB_SETCURSEL , np2cfg.lgy98io>>12, 0);
					hItem = GetDlgItem(hWnd, IDC_LGY98INT);
					np2cfg.lgy98irq = cpIrqLGY98[1].nItemData;
					SendMessage(hItem , CB_SETCURSEL , 1 , 0);
					break;
			}
			break;

		case WM_NOTIFY:
			//if ((((NMHDR *)lp)->code) == (UINT)PSN_APPLY) {
			//	if (np2cfg.snd26opt != snd26) {
			//		np2cfg.snd26opt = snd26;
			//		sysmng_update(SYS_UPDATECFG);
			//	}
			//	return(TRUE);
			//}
			break;
	}
#endif
	return(FALSE);
}

void dialog_netopt(HWND hWnd)
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

	psp.pszTemplate = MAKEINTRESOURCE(IDD_LGY98);
	psp.pfnDlgProc = (DLGPROC)LGY98optDlgProc;
	hpsp[0] = CreatePropertySheetPage(&psp);

	std::tstring rTitle(LoadTString(IDS_NETWORKOPTION));

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
