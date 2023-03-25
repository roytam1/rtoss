#include "stdafx.h"
#include "app.h"
#include "winutil.h"
using namespace ki;

#ifndef NO_OLEDNDSRC
// Alternative version of DragDetect that beter fits my needs:
// 1) it does not removes the button up from message queue.
// 2) Any button can be used to do the drag and a drag can occur
//    in the non-client area.
// 3) Should run on All windows versions, even Win32s beta/Chicago.
bool coolDragDetect( HWND hwnd, LPARAM pt, WORD btup, WORD removebutton )
{
	int cxd = GetSystemMetrics(SM_CXDRAG);
	int cyd = GetSystemMetrics(SM_CYDRAG);
	short x = LOWORD(pt);
	short y = HIWORD(pt);

	MSG msg;
	BOOL mm;
	do
	{
		mm = PeekMessage(&msg, hwnd, btup, btup, removebutton );
		if( mm )
			return false;

		mm = PeekMessage(&msg, hwnd, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE);
		if( mm && msg.message == VK_ESCAPE )
			return false;

		mm = PeekMessage(&msg, hwnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
		if( !mm )
			mm = PeekMessage(&msg, hwnd, WM_NCMOUSEMOVE, WM_NCMOUSEMOVE, PM_REMOVE);
		if( mm && (Abs(x-LOWORD(msg.lParam)) > cxd || Abs(y-HIWORD(msg.lParam)) > cyd) )
		{
			return true;
		}
	}
	while( WaitMessage() );

	return false;
}
#endif // NO_OLEDNDSRC

//=========================================================================

Clipboard::Clipboard( HWND owner, bool read )
	: opened_( false )
{
	if( ::OpenClipboard(owner) )
		if( read || ::EmptyClipboard() )
			opened_ = true;
		else
			::CloseClipboard();
}

Clipboard::~Clipboard()
{
	if( opened_ )
		::CloseClipboard();
}

Clipboard::Text Clipboard::GetUnicodeText() const
{
	if( app().isNT() )
	{
		// NTÇ»ÇÁíºê⁄UnicodeÇ≈Ç∆ÇÍÇÈ
		HANDLE h = GetData( CF_UNICODETEXT );
		if( h != NULL )
		{
			unicode* ustr = static_cast<unicode*>( ::GlobalLock( h ) );
			return Text( ustr, Text::GALLOC );
		}
	}
	else
	{
		// 9xÇ»ÇÁïœä∑Ç™ïKóv
		HANDLE h = GetData( CF_TEXT );
		if( h != NULL )
		{
			char* cstr = static_cast<char*>( ::GlobalLock( h ) );
			int Lu = ::lstrlenA( cstr ) * 3;
			unicode* ustr = new unicode[Lu];
			::MultiByteToWideChar( CP_ACP, 0, cstr, -1, ustr, Lu );
			::GlobalUnlock( h );
			return Text( ustr, Text::NEW );
		}
	}

	return Text( NULL, Text::NEW );
}
