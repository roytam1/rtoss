#include "GSPlayer2.h"


static void FixupDialogItem(HINSTANCE hInst, HWND hDlg,LPDLGITEMTEMPLATE lpDlgItem, LPWORD lpClass, LPWORD lpData);
static LPBYTE WalkDialogData(LPBYTE lpData);

//////////////////////////////////////////////////////////////////////////////
// FUNCTION: RelayoutDialog
//
// PURPOSE: Re-lays out a dialog based on a dialog template.  This function
//      iterates through all the child window controls and does a SetWindowPos
//      for each.  It also does a SetWindowText for each static text control 
//      and updates the selected bitmap or icon in a static image control. 
//      This assumes that the current dialog and the new template have all the 
//      same controls, with the same IDCs.
//
// ON ENTRY:
//      HINSTANCE hInst: the hInstance of the current module.
//      HWND hDlg: the dialog to layout.
//      LPCWSTR iddTemplate: the new template for the dialog (can use 
//          the MAKEINTRESOURCE macro).
//
// ON EXIT: TRUE if success; FALSE if failure (either the iddTemplate is 
//      invalid, or there are two or more IDC_STATICs in the template).
//

BOOL RelayoutDialog(HINSTANCE hInst, HWND hDlg, LPCWSTR iddTemplate)
{
	HRSRC hRsrc = FindResource((HMODULE)hInst, iddTemplate, RT_DIALOG);
	if (hRsrc == NULL) {
		return FALSE;
	}
	HGLOBAL hGlobal = LoadResource((HMODULE)hInst, hRsrc);
	if (hGlobal == NULL) {
		return FALSE;
	}
	INT nStatics = 0;
    LPBYTE lpData = (LPBYTE)LockResource(hGlobal);
	LPDLGTEMPLATE lpTemplate = (LPDLGTEMPLATE)lpData;
	HDWP hDWP = BeginDeferWindowPos(lpTemplate->cdit);     
	//
	// See documentation on the DLGTEMPLATE and DLGITEMTEMPLATE structs    
	// for more information on the data structures we are walking.    
	//
	lpData += sizeof(DLGTEMPLATE);
	lpData = WalkDialogData(lpData); // menu
    lpData = WalkDialogData(lpData); // class
    lpData = WalkDialogData(lpData); // title
    if (lpTemplate->style & DS_SETFONT) {
    	lpData += sizeof(WORD); // font size.
    	lpData = WalkDialogData(lpData); // font face.
	}
	
	for (int i = 0; i < lpTemplate->cdit; i++) {
		lpData = (LPBYTE) (((INT)lpData + 3) & ~3);  // force to DWORD boundary.
		LPDLGITEMTEMPLATE lpDlgItem = (LPDLGITEMTEMPLATE)lpData;
		HWND hwndCtl = GetDlgItem(hDlg, lpDlgItem->id);
		if (lpDlgItem->id == 0xFFFF) {
			nStatics++;
		}
		         
		//
		// Move the item around.
		//
        {
        	RECT r;
        	r.left   = lpDlgItem->x;
        	r.top    = lpDlgItem->y;
        	r.right  = lpDlgItem->x + lpDlgItem->cx;
        	r.bottom = lpDlgItem->y + lpDlgItem->cy;
        	MapDialogRect(hDlg, &r);
        	DeferWindowPos(hDWP, hwndCtl, NULL, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOZORDER);
        }
        lpData += sizeof(DLGITEMTEMPLATE);
        LPWORD lpClass = (LPWORD)lpData;
        lpData = WalkDialogData(lpData);  // class
        
        //
        // Do some special handling for each dialog item (changing text,
        // bitmaps, ensuring visible, etc.
        //
        	
        FixupDialogItem(hInst, hDlg, lpDlgItem, lpClass, (LPWORD)lpData);
        lpData = WalkDialogData(lpData);  // title
        WORD cbExtra = *((LPWORD)lpData); // extra class data.
        lpData += (cbExtra ? cbExtra : sizeof(WORD));
	}
	
	EndDeferWindowPos(hDWP);
	return nStatics < 2 ? TRUE : FALSE;
}

//
// Called by RelayoutDialog to advance to the next item in the dialog template.
//

static LPBYTE WalkDialogData(LPBYTE lpData)
{
	LPWORD lpWord = (LPWORD)lpData;
	if (*lpWord == 0xFFFF) {
		return (LPBYTE)(lpWord + 2);
	}
	
	while (*lpWord != 0x0000) {
		lpWord++;
	}
	return (LPBYTE)(lpWord + 1);
}

//
// Post-processing step for each dialog item.
//    Static controls and buttons: change text and bitmaps.
//    Listboxes and combo boxes: ensures that the selected item is visible.
//

static void FixupDialogItem(HINSTANCE hInst, HWND hDlg, LPDLGITEMTEMPLATE lpDlgItem, LPWORD lpClass, LPWORD lpData)
{
	if (lpClass[0] == 0xFFFF) {
		switch (lpClass[1]) {
			case 0x0080: // button
			case 0x0082: // static
			{
				if (lpData[0] == 0xFFFF) {
					if (lpDlgItem->style & SS_ICON) {
						HICON hOld = (HICON)SendDlgItemMessage(hDlg, lpDlgItem->id, STM_GETIMAGE, IMAGE_ICON, 0);
						HICON hNew = LoadIcon(hInst, MAKEINTRESOURCE(lpData[1]));
						SendDlgItemMessage(hDlg, lpDlgItem->id, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hNew);
						DestroyIcon(hOld);
					}
					else if (lpDlgItem->style & SS_BITMAP) {
						HBITMAP hOld = (HBITMAP)SendDlgItemMessage(hDlg, lpDlgItem->id, STM_GETIMAGE, IMAGE_BITMAP, 0);
						HBITMAP hNew = LoadBitmap(hInst, MAKEINTRESOURCE(lpData[1]));
						SendDlgItemMessage(hDlg, lpDlgItem->id, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hNew);
						DeleteObject(hOld);
					}
				}
				else // lpData[0] is not 0xFFFF (it's text).
				{
					SetDlgItemText(hDlg, lpDlgItem->id, (LPCTSTR)lpData);
				}
				break;
			}
			case 0x0083: // list box
			{
				INT nSel = SendDlgItemMessage(hDlg, lpDlgItem->id, LB_GETCURSEL, 0, 0);
				if (nSel != LB_ERR) {
					SendDlgItemMessage(hDlg, lpDlgItem->id, LB_SETCURSEL, nSel, 0);
				}
				break;
			}
			case 0x0085: // combo box
			{
				INT nSel = SendDlgItemMessage(hDlg, lpDlgItem->id, CB_GETCURSEL, 0, 0);
				if (nSel != CB_ERR) {
					SendDlgItemMessage(hDlg, lpDlgItem->id, CB_SETCURSEL, nSel, 0);
				}
				break;
			}
		}
	}
}
