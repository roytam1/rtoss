#include "stdafx.h"
#include "app.h"
#include "ctrl.h"
using namespace ki;



//=========================================================================

StatusBar::StatusBar()
{
	app().InitModule( App::CTL );
}

bool StatusBar::Create( HWND parent )
{
	HWND h = NULL;
/*#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
	h = ::CreateStatusWindow(
		WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,
		TEXT(""), parent, 1787 );
#else*/
	if(App::getOSVer() == 310 || (App::getOSVer() == 350 && App::getOSBuild() < 711)) {
	// Avoid using CreateStatusWindow that is not present on NT3.1.
		h = ::CreateWindow(
			TEXT("msctls_statusbar"),  // pointer to registered class name, N.B. no "32" suffix in NT 3.1's comctl32.dll
			NULL, // pointer to window name
			WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP , // window style
			0, 0, 0, 0, //x, y, w, h
			parent, // handle to parent or owner window
			(struct HMENU__ *)1787,          // handle to menu or child-window identifier
			app().hinst(), // handle to application instance
			NULL // pointer to window-creation data
		);
	} else {
	// Avoid using CreateStatusWindow that is not present on NT3.1.
		h = ::CreateWindow(
			TEXT("msctls_statusbar32"),  // pointer to registered class name
			NULL, // pointer to window name
			WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP , // window style
			0, 0, 0, 0, //x, y, w, h
			parent, // handle to parent or owner window
			(struct HMENU__ *)1787,          // handle to menu or child-window identifier
			app().hinst(), // handle to application instance
			NULL // pointer to window-creation data
		);
	}
//#endif
	if( h == NULL )
		return false;

	SetStatusBarVisible();
	SetHwnd( h );
	AutoResize( false );
	return true;
}

void StatusBar::SetText( const TCHAR* str, int part ) {
//#if defined(TARGET_VER) && TARGET_VER==310
#if defined(UNICODE)
	if(App::getOSVer() == 310 || app().isOldCommCtrl()) {
		// early builds of common controls status bar is ANSI only
		int strlength = ::lstrlen(str)+1;
		char *asciistr = new char[strlength];
		for(int i=0;i<strlength; ++i) asciistr[i]=*(str+i); // cheap conversion to ANSI
		::SendMessageA( hwnd(), SB_SETTEXTA, part, reinterpret_cast<LPARAM>(asciistr) );
		delete []asciistr;
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
		SendMsg( CB_FINDSTRINGEXACT, ~0, reinterpret_cast<LPARAM>(str) );
	if( i != CB_ERR )
		SendMsg( CB_SETCURSEL, i );
}

bool ComboBox::PreTranslateMessage( MSG* )
{
	return false;
}
