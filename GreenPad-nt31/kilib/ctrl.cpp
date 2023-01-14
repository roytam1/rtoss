#include "stdafx.h"
#include "app.h"
#include "ctrl.h"
using namespace ki;



//=========================================================================

StatusBar::StatusBar()
{
	//app().InitModule( App::CTL );
}

bool StatusBar::Create(  )
{
	HWND h = NULL;
	WNDCLASS wc;
	app().InitModule( App::CTL );
/*#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
	h = ::CreateStatusWindow(
		WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,
		TEXT(""), parent, 1787 );
#else*/
	// Avoid using CreateStatusWindow that is not present on NT3.1.
	h = ::CreateWindowEx(
		0,  // extended window style
		GetClassInfo(NULL, STATUSCLASSNAME, &wc) ? STATUSCLASSNAME : TEXT("msctls_statusbar"),  // pointer to registered class name
		NULL, // pointer to window name
		WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP , // window style
		0, 0, 0, 0, //x, y, w, h
		parent_, // handle to parent or owner window
		(struct HMENU__ *)1787,          // handle to menu or child-window identifier
		app().hinst(), // handle to application instance
		NULL // pointer to window-creation data
	);
//#endif
	if( h == NULL )
		return false;

	SetHwnd( h );
	AutoResize( false );
	return true;
}

void StatusBar::SetText( const TCHAR* str, int part ) {
//#if defined(TARGET_VER) && TARGET_VER==310
#if defined(UNICODE)
	if(app().getOSVer() == MKVER(3,10) || app().hasOldCommCtrl()) {
		// early builds of common controls status bar is ANSI only
		char buf[256];
		long len = ::WideCharToMultiByte(CP_ACP, 0, str, -1 , buf, countof(buf), NULL, NULL);
		buf[len] = '\0';
		::SendMessageA( hwnd(), SB_SETTEXTA, part, reinterpret_cast<LPARAM>(buf) );
	} else {
		SendMsg( SB_SETTEXT, part, reinterpret_cast<LPARAM>(str) );
	}
#else
	SendMsg( SB_SETTEXT, part, reinterpret_cast<LPARAM>(str) );
#endif
}

int StatusBar::AutoResize( bool maximized )
{
	// サイズ自動変更
	SendMsg( WM_SIZE );

	// 変更後のサイズを取得
	RECT rc;
	getPos( &rc );
	width_ = rc.right - rc.left;
	if( !maximized )
		width_ -= 15;
	return (isStatusBarVisible() ? rc.bottom - rc.top : 0);
}

bool StatusBar::PreTranslateMessage( MSG* )
{
	// 何もしない
	return false;
}



//=========================================================================

void ComboBox::Select( const TCHAR* str )
{
	// SELECTSTRING は先頭が合ってる物に全てにマッチするので使えない。
	// おそらくインクリメンタルサーチとかに使うべきものなのだろう。
	size_t i =
		SendMsg( CB_FINDSTRING, ~0, reinterpret_cast<LPARAM>(str) );
	if( i != CB_ERR )
		SendMsg( CB_SETCURSEL, i );
}

bool ComboBox::PreTranslateMessage( MSG* )
{
	return false;
}
