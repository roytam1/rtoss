
#include "Common.h"
#include "HexEditWnd.h"

//
// constants
//
#define          CUT_CHAR             0x18
#define          COPY_CHAR            0x03
#define          PASTE_CHAR           0x16
#define          STRZ_CHAR            0x1a

//
// global variables
//

BOOL UnHookHexEditbox(HWND hEdit) {
	char clsName[256];
	GetClassName(hEdit, clsName, sizeof(clsName));

	if (stricmp("ComboBox", clsName) == 0) {
		hEdit = GetWindow(hEdit, GW_CHILD);
	}

	GetClassName(hEdit, clsName, sizeof(clsName));
	WNDCLASS  wc;
	GetClassInfo(NULL, clsName, &wc);

	if (SetWindowLong(
		hEdit,
		GWL_WNDPROC,
		(DWORD)wc.lpfnWndProc))
		return TRUE;
	else
		return FALSE;
}

//
// window proc stub for hex editbox's
//
BOOL HookHexEditbox(HWND hEdit)
{
	char clsName[256];
	GetClassName(hEdit, clsName, sizeof(clsName));

	if (stricmp("ComboBox", clsName) == 0) {
		hEdit = GetWindow(hEdit, GW_CHILD);
	}

	if (SetWindowLong(
		hEdit,
		GWL_WNDPROC,
		(DWORD)HexOnlyEditProc))
		return TRUE;
	else
		return FALSE;
}

//
// call this with an edit box as argument to monitor the occurrence of the ENTER key
//
BOOL HookEditboxEnter(HWND hEdit)
{
	char clsName[256];
	GetClassName(hEdit, clsName, sizeof(clsName));

	if (stricmp("ComboBox", clsName) == 0) {
		hEdit = GetWindow(hEdit, GW_CHILD);
	}

	if (SetWindowLong(
		hEdit,
		GWL_WNDPROC,
		(DWORD)EditHookEnterProc))
		return TRUE;
	else
		return FALSE;
}

LRESULT __stdcall HexOnlyEditProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	CHAR      c = 0;
	/*
	char      cBuff[9], *pCH;
	DWORD     dw;
	*/
	WNDCLASS  wc;

	switch(Msg)
	{
	case WM_PASTE:
		// check whether the contents is a valid hex number string
		if (OpenClipboard(hWnd))
		{
			int		len;
			char	*pData;

			if (IsClipboardFormatAvailable(cf16Edit)) {
				PHE_CLIPBOARD_DATA	pcbd;

				pcbd = (PHE_CLIPBOARD_DATA)::GetClipboardData(cf16Edit);
				len = pcbd->dwDataSize;
				pData = (char *)&pcbd->byDataStart;
			} else if (IsClipboardFormatAvailable(CF_TEXT)) {
				pData = (char *)::GetClipboardData(CF_TEXT);
				len = strlen(pData);
			} else {
				return 0;
			}

			for (int i = 0; i < len; i++) {
				int ch1 = (pData[i] & 0xF0) >> 4;
				int ch2 = (pData[i] & 0x0F);

				if (ch1 >= 0 && ch1 <= 9) {
					ch1 += 0x30;
				} else {
					ch1 += 'A' - 0xA;
				}
				PostMessage(hWnd, WM_CHAR, ch1, 0);

				if (ch2 >= 0 && ch2 <= 9) {
					ch2 += 0x30;
				} else {
					ch2 += 'A' - 0xA;
				}
				PostMessage(hWnd, WM_CHAR, ch2, 0);
			}
			CloseClipboard();
			/*
			pCH = (char*)GetClipboardData(EnumClipboardFormats(CF_LOCALE)); // for win2k
			if (!pCH)
				pCH = (char*)GetClipboardData(EnumClipboardFormats(0));
			CloseClipboard();
			if (pCH)
				if (lstrcpyn(cBuff, pCH, sizeof(cBuff)))
					if (HexStrToInt(cBuff, &dw))
						break;			
			*/
		}
		return 0;

	case WM_KEYDOWN:
		// inform parent window via WM_COMMAND msg about ENTER key occurrence
		switch(wParam)
		{
		case VK_RETURN:
			SendMessage(
				GetParent(hWnd),
				WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(hWnd), NM_HEXEDITENTER),
				(LPARAM)hWnd);
			return 0;

		//case VK_ESCAPE:
			// if the control handles this key it'll close the parent dlg - dunno why :(
		//	return 0;
		}
		break;

	case WM_CHAR:
		// force hex characters
		if (wParam != VK_BACK    &&
		   wParam  != CUT_CHAR   && 
		   wParam  != PASTE_CHAR &&
		   wParam  != COPY_CHAR  &&
		   wParam  != STRZ_CHAR)         
		{
			c = toupper(wParam);
			if ( (c < '0' || c > '9') &&
				 (c < 'A' || c > 'F'))
				 return 0;
			else
				wParam = c;
		}
		break;
	}

	char clsName[256];
	GetClassName(hWnd, clsName, sizeof(clsName));

	// get default edit wnd proc
	GetClassInfo(NULL, clsName, &wc);

	return CallWindowProc(wc.lpfnWndProc, hWnd, Msg, wParam, lParam);
}

LRESULT __stdcall EditHookEnterProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	WNDCLASS  wc;

	switch(Msg)
	{
	case WM_KEYDOWN:
		// inform parent window via WM_COMMAND msg about ENTER key occurrence
		switch(wParam)
		{
		case VK_RETURN:
			SendMessage(
				GetParent(hWnd),
				WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(hWnd), NM_HEXEDITENTER),
				(LPARAM)hWnd);
			return 0;

		//case VK_ESCAPE:
			// if the control handles this key it'll close the parent dlg - dunno why :(
		//	return 0;
		}
		break;
	}

	char clsName[256];
	GetClassName(hWnd, clsName, sizeof(clsName));
	GetClassInfo(NULL, clsName, &wc);

	return CallWindowProc(wc.lpfnWndProc, hWnd, Msg, wParam, lParam);
}

//
// convert a hex number string to a DW
//
BOOL HexStrToInt(char *szHexStr, DWORD *pdwHexVal)
{
	char   *pCH, c;
	DWORD  dwVal = 0, dw;

	pCH = szHexStr;
	while (*pCH)
	{
		c = toupper(*pCH++);
		if (c >= 'A' && c <= 'F')
			dw = (DWORD)c - ((DWORD)'A' - 10);
		else if (c >= '0' && c <= '9')
			dw = (DWORD)c - (DWORD)'0';
		else
			return FALSE; // invalid hex char
		dwVal = (dwVal << 4) + dw;
	}

	*pdwHexVal = dwVal;
	return TRUE;
}

/*
 * Return DWORD
 * 		2 : EXE
 * 		1 : DLL
 * 		0 : NOT PE
 */
DWORD file_type(char *base) {
	IMAGE_DOS_HEADER *dos_head = (IMAGE_DOS_HEADER *)base;
	IMAGE_NT_HEADERS *header;
	DWORD type;

	if (dos_head->e_magic != IMAGE_DOS_SIGNATURE) {
		return 0;
	}

	header = (IMAGE_NT_HEADERS *)((char *)dos_head + dos_head->e_lfanew);
	if (IsBadReadPtr(header, sizeof(*header))) {
		return 0;
	}
	if (header->Signature != IMAGE_NT_SIGNATURE) {
		return 0;
	}

	if (header->FileHeader.Characteristics & IMAGE_FILE_DLL) {
		type = 1;
	} else {
		type = 2;
	}

	return type;
}

#define isin(address,start,length) ((address)>=(start) && (address)<(start)+(length))
/*
 * Get the vitual offset from file offset
 */
int get_va(char *base, DWORD file_offset) {
	IMAGE_DOS_HEADER *dos_head = (IMAGE_DOS_HEADER *)base;
	IMAGE_NT_HEADERS *header;
	IMAGE_SECTION_HEADER *section_header;
	int   sect;

	header = (IMAGE_NT_HEADERS *)((char *)dos_head + dos_head->e_lfanew);
	for (sect = 0, section_header = (IMAGE_SECTION_HEADER *)
		((char *)header + header->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + 4); 
		sect < header->FileHeader.NumberOfSections; sect++, section_header++) {
		if (isin(file_offset, section_header->PointerToRawData, section_header->SizeOfRawData)) {
			return section_header->VirtualAddress + 
				file_offset - section_header->PointerToRawData + header->OptionalHeader.ImageBase;
		}
	}
	return file_offset;
}

/*
 * Get the file offset from vitual offset
 */
int get_fo(char *base, DWORD va_offset) {
	IMAGE_DOS_HEADER *dos_head = (IMAGE_DOS_HEADER *)base;
	IMAGE_NT_HEADERS *header;
	IMAGE_SECTION_HEADER *section_header;
	int   sect;
	DWORD	va;

	header = (IMAGE_NT_HEADERS *)((char *)dos_head + dos_head->e_lfanew);
	va = va_offset - header->OptionalHeader.ImageBase;

	for (sect = 0, section_header = (IMAGE_SECTION_HEADER *)
		((char *)header + header->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + 4); 
		sect < header->FileHeader.NumberOfSections; sect++, section_header++) {
		if (isin(va, section_header->VirtualAddress, section_header->SizeOfRawData)) {
			return section_header->PointerToRawData + va - section_header->VirtualAddress;
		}
	}
	return va_offset;
}

