#include "stdafx.h"
#include "rsrc/resource.h"
#include "GpMain.h"
using namespace ki;
using namespace editwing;



//-------------------------------------------------------------------------
// 新規プロセス起動
//-------------------------------------------------------------------------

void BootNewProcess( const TCHAR* cmd = TEXT("") )
{
	STARTUPINFO         sti;
	PROCESS_INFORMATION psi;
	::GetStartupInfo( &sti );

	String fcmd = Path(Path::ExeName).BeShortStyle();
	fcmd += ' ';
	fcmd += cmd;

	if( ::CreateProcess( NULL, const_cast<TCHAR*>(fcmd.c_str()),
			NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL,
			&sti, &psi ) )
	{
		::CloseHandle( psi.hThread );
		::CloseHandle( psi.hProcess );
	}
}

typedef HBRUSH (WINAPI *GSCB)(int);
HBRUSH WINAPI MyGetSysColorBrush_fallback(int nIndex)
{
	return ::CreateSolidBrush( ::GetSysColor(nIndex) );
}
HBRUSH WINAPI MyGetSysColorBrush_init(int nIndex);
static GSCB MyGetSysColorBrush = MyGetSysColorBrush_init;
HBRUSH WINAPI MyGetSysColorBrush_init(int nIndex)
{
	if( MyGetSysColorBrush == MyGetSysColorBrush_init ) {
		MyGetSysColorBrush = (GSCB)GetProcAddress(GetModuleHandleA("USER32.DLL"), "GetSysColorBrush");

		// Should be supported since Windows NT 3.51...
		if( !(MyGetSysColorBrush) ) {
			MyGetSysColorBrush = MyGetSysColorBrush_fallback;
		}
	}

	return MyGetSysColorBrush( nIndex );
}


//-------------------------------------------------------------------------
// ステータスバー制御
//-------------------------------------------------------------------------

inline GpStBar::GpStBar()
	: str_(NULL)
	, lb_(2)
{
}

inline void GpStBar::SetCsText( const TCHAR* str )
{
	// 文字コード表示領域にSetTextする
	SetText( str_=str, 1 );
}

inline void GpStBar::SetLbText( int lb )
{
	// 改行コード表示領域にSetTextする
	static const TCHAR* const lbstr[] = {TEXT("CR"),TEXT("LF"),TEXT("CRLF")};
	SetText( lbstr[lb_=lb], 2 );
}

int GpStBar::AutoResize( bool maximized )
{
	// 文字コード表示領域を確保しつつリサイズ
	int h = StatusBar::AutoResize( maximized );
	int w[] = { width()-5, width()-5, width()-5 };

	HDC dc = ::GetDC( hwnd() );
	SIZE s;

	if(app().isWin32s() || (app().isWin3later() && !app().getOSBuild())) { // old win95 betas may not have GVEx and leaving build number unsetted
		if( ::GetTextExtentPoint( dc, TEXT("BBBBM"), 5, &s ) ) // Line Ending
			w[1] = w[2] - s.cx;
		if( ::GetTextExtentPoint( dc, TEXT("BBBWWWW"), 7, &s ) ) // Charset
			w[0] = w[1] - s.cx;
	}else {
		if( ::GetTextExtentPoint32( dc, TEXT("BBBBM"), 5, &s ) ) // Line Ending
			w[1] = w[2] - s.cx;
		if( ::GetTextExtentPoint32( dc, TEXT("BBBWWWW"), 7, &s ) ) // Charset
			w[0] = w[1] - s.cx;
	}
	::ReleaseDC( hwnd(), dc );

	SetParts( countof(w), w );
	SetCsText( str_ );
	SetLbText( lb_ );
	return h;
}



//-------------------------------------------------------------------------
// ディスパッチャ
//-------------------------------------------------------------------------

LRESULT GreenPadWnd::on_message( UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg )
	{
	// アクティブ化。EditCtrlにフォーカスを。
	case WM_ACTIVATE:
		if( LOWORD(wp) != WA_INACTIVE )
			edit_.SetFocus();
		break;

	// サイズ変更。子窓を適当に移動。
	case WM_SIZE:
		if( wp==SIZE_MAXIMIZED || wp==SIZE_RESTORED )
		{
			int ht = stb_.AutoResize( wp==SIZE_MAXIMIZED );
			edit_.MoveTo( 0, 0, LOWORD(lp), HIWORD(lp)-ht );
			cfg_.RememberWnd(this);
		}
		break;

	// ウインドウ移動
	case WM_MOVE:
		{
			RECT rc;
			getPos(&rc);
			cfg_.RememberWnd(this);
		}
		break;

	// システムコマンド。終了ボタンとか。
	case WM_SYSCOMMAND:
		if( wp==SC_CLOSE || wp==SC_DEFAULT )
			on_exit();
		else
			return WndImpl::on_message( msg, wp, lp );
		break;

	// 右クリックメニュー
	case WM_CONTEXTMENU:
		if( reinterpret_cast<HWND>(wp) == edit_.hwnd() )
			::TrackPopupMenu(
				::GetSubMenu( ::GetMenu(hwnd()), 1 ), // 編集メニュー表示
				TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
				static_cast<SHORT>(LOWORD(lp)), static_cast<SHORT>(HIWORD(lp)), 0, hwnd(), NULL );
		else
			return WndImpl::on_message( msg, wp, lp );
		break;

	// メニューのグレーアウト処理
	case WM_INITMENU:
	case WM_INITMENUPOPUP:
		on_initmenu( reinterpret_cast<HMENU>(wp), msg==WM_INITMENUPOPUP );
		break;

	// Ｄ＆Ｄ
	case WM_DROPFILES:
		{
			HGLOBAL hDrop = reinterpret_cast<HGLOBAL>(wp);
			DROPFILES *df = (DROPFILES *)::GlobalLock( hDrop );
			size_t hdropSize = ::GlobalSize( hDrop );
			HWND *hDummy = (HWND*)( ((BYTE*)df) + hdropSize - 2*sizeof(HWND) );
			HWND *hCustomHwnd = (HWND*)( ((BYTE*)hDummy) + sizeof(HWND) );
			BOOL bProcessDrops = *hDummy || (*hDummy == 0 && *hCustomHwnd != hwnd());
			::GlobalUnlock(hDrop);
			if(bProcessDrops)
			{
				on_drop( reinterpret_cast<HDROP>(wp) );
			}
		}
		break;

#ifndef NO_OLEDNDSRC
	case WM_NCLBUTTONDOWN: {
		if( wp == HTSYSMENU
		&& !isUntitled()
		&& coolDragDetect( hwnd(), /*pt=*/lp, WM_NCLBUTTONUP, PM_NOREMOVE )  )
		{
			// Allow dragging file out of system icon with Left button.
			const unicode *fnu = filename_.ConvToWChar();
			if( fnu )
			{
				OleDnDSourceFile doDrag( hwnd(), fnu, my_lstrlenW(fnu), DROPEFFECT_COPY );
				filename_.FreeWCMem(fnu);
			}
			break;
		}
	} return WndImpl::on_message( msg, wp, lp );
#endif // NO_OLEDNDSRC

	// MRU
	case GPM_MRUCHANGED:
		SetupMRUMenu();
		break;

	// NOTIFY
	case WM_NOTIFY:
		if( ((NMHDR*)lp)->code == NM_DBLCLK )
			if( wp == 1787 ) // Status Bar ID 
				on_reopenfile();
		break;

	// その他
	default:
		return WndImpl::on_message( msg, wp, lp );
	}

	return 0;
}

bool GreenPadWnd::on_command( UINT id, HWND ctrl )
{
	switch( id )
	{
	// Window
	case ID_CMD_NEXTWINDOW: on_nextwnd(); break;
	case ID_CMD_PREVWINDOW: on_prevwnd(); break;

	// File
	case ID_CMD_NEWFILE:    on_newfile();   break;
	case ID_CMD_OPENFILE:   on_openfile();  break;
	case ID_CMD_REOPENFILE: on_reopenfile();break;
	case ID_CMD_SAVEFILE:   on_savefile();  break;
	case ID_CMD_SAVEFILEAS: on_savefileas();break;
	case ID_CMD_PRINT:      on_print();     break;
	case ID_CMD_SAVEEXIT:   if(Save_showDlgIfNeeded()) on_exit();  break;
	case ID_CMD_DISCARDEXIT: Destroy();     break;
	case ID_CMD_EXIT:       on_exit();      break;

	// Edit
	case ID_CMD_UNDO:       edit_.getDoc().Undo();              break;
	case ID_CMD_REDO:       edit_.getDoc().Redo();              break;
	case ID_CMD_CUT:        edit_.getCursor().Cut();            break;
	case ID_CMD_COPY:       edit_.getCursor().Copy();           break;
	case ID_CMD_PASTE:      edit_.getCursor().Paste();          break;
	case ID_CMD_DELETE: if( edit_.getCursor().isSelected() )
	                        edit_.getCursor().Del();            break;
	case ID_CMD_SELECTALL:  edit_.getCursor().Home(true,false);
	                        edit_.getCursor().End(true,true);   break;
	case ID_CMD_DATETIME:   on_datetime();                      break;
	case ID_CMD_RECONV:     on_reconv();                        break;
	case ID_CMD_TOGGLEIME:  on_toggleime();                     break;

	// Search
	case ID_CMD_FIND:       search_.ShowDlg();  break;
	case ID_CMD_FINDNEXT:   search_.FindNext(); break;
	case ID_CMD_FINDPREV:   search_.FindPrev(); break;
	case ID_CMD_JUMP:       on_jump(); break;
	case ID_CMD_GREP:       on_grep();break;

	// View
	case ID_CMD_NOWRAP:     edit_.getView().SetWrapType( wrap_=-1 ); break;
	case ID_CMD_WRAPWIDTH:  edit_.getView().SetWrapType( wrap_=cfg_.wrapWidth() ); break;
	case ID_CMD_WRAPWINDOW: edit_.getView().SetWrapType( wrap_=0 ); break;
	case ID_CMD_CONFIG:     on_config();    break;
	case ID_CMD_STATUSBAR:  on_statusBar(); break;
	case ID_CMD_HELPABOUT:  on_helpabout(); break;

	// DocType
	default: if( ID_CMD_DOCTYPE <= id ) {
			on_doctype( id - ID_CMD_DOCTYPE );
			break;
		} else if( ID_CMD_MRU <= id ) {
			on_mru( id - ID_CMD_MRU );
			break;
		}

	// Default
		return false;
	}
	return true;
}

bool GreenPadWnd::PreTranslateMessage( MSG* msg )
{
	// 苦肉の策^^;
	if( search_.TrapMsg(msg) )
		return true;
	// キーボードショートカット処理
	return 0 != ::TranslateAccelerator( hwnd(), accel_, msg );
}



//-------------------------------------------------------------------------
// コマンド処理
//-------------------------------------------------------------------------

void GreenPadWnd::on_dirtyflag_change( bool )
{
	UpdateWindowName();
}

void GreenPadWnd::on_newfile()
{
	BootNewProcess();
}

void GreenPadWnd::on_openfile()
{
	Path fn;
	int  cs;
	if( ShowOpenDlg( &fn, &cs ) )
		Open( fn, cs );
}

void GreenPadWnd::on_reopenfile()
{
	if( !isUntitled() )
	{
		ReopenDlg dlg( charSets_, csi_ );
		dlg.GoModal( hwnd() );
		if( dlg.endcode()==IDOK && AskToSave() )
			OpenByMyself( filename_, charSets_[dlg.csi()].ID, false );
	}
}

void GreenPadWnd::on_savefile()
{
	Save_showDlgIfNeeded();
}

void GreenPadWnd::on_savefileas()
{
	if( ShowSaveDlg() )
	{
		Save();
		ReloadConfig(); // 文書タイプに応じて表示を更新
	}
}

void GreenPadWnd::on_print()
{
	TCHAR tmp[128];

	doc::Document& d = edit_.getDoc();
	const unicode* buf;
	ulong dpStart = 0, len = 0;
	short procCopies = 0, totalCopies = 0;

	PRINTDLG thePrintDlg = { sizeof(thePrintDlg) };
	thePrintDlg.Flags = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION | PD_HIDEPRINTTOFILE;
	thePrintDlg.nCopies = 1;

	if (PrintDlg(&thePrintDlg) == 0) {
		// cancelled
		return;
	}

	totalCopies = thePrintDlg.nCopies;

	// タイトルに表示される文字列の調整
	// FileName * - GreenPad
	String name;
	name += isUntitled() ? TEXT("untitled") : filename_.name();
	if( edit_.getDoc().isModified() ) name += TEXT(" *");
	name += TEXT(" - ");
	name += String(IDS_APPNAME).c_str();

	// Set DOCINFO structure
	DOCINFO di = { sizeof(DOCINFO) };
	di.lpszDocName = name.c_str();
	di.lpszOutput = (LPTSTR) NULL;
	di.lpszDatatype = (LPTSTR) NULL;
	di.fwType = 0;
	RECT rctmp = { 0, 0, 0, 0 };

	int nError = ::StartDoc(thePrintDlg.hDC, &di);
	if (nError == SP_ERROR)
	{
		::wsprintf(tmp,TEXT("StartDoc Error #%d - please check printer."),::GetLastError());
		::MessageBox( NULL, tmp, String(IDS_APPNAME).c_str(), MB_OK|MB_TASKMODAL );
		return;
		// Handle the error intelligently
	}
	::StartPage(thePrintDlg.hDC);

	// Get Printer Caps
	int cWidthPels, cHeightPels, cLineHeight;
	cWidthPels = ::GetDeviceCaps(thePrintDlg.hDC, HORZRES);
	cHeightPels = ::GetDeviceCaps(thePrintDlg.hDC, VERTRES);

	// Get Line height
	rctmp.right = cWidthPels;
	rctmp.bottom = cHeightPels;
	::DrawTextW(thePrintDlg.hDC, L"#", 1, &rctmp, DT_CALCRECT|DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS|DT_EDITCONTROL);
	cLineHeight = rctmp.bottom-rctmp.top;

	RECT rcPrinter = { 5, 5, cWidthPels - 10, cHeightPels - 10 };

	int nThisLineHeight, nChars = 0, nHi = 0, nLo = 0;
	const unicode* uStart;

	// Process with multiple copies
	do {
		if(procCopies) 
		{
			::StartPage(thePrintDlg.hDC);
			rcPrinter.top = 5;
			rcPrinter.left = 5;
			rcPrinter.right = cWidthPels - 10;
			rcPrinter.bottom = cHeightPels - 10;
		}
		// Print
		for( ulong e=d.tln(), dpStart=0; dpStart<e; )
		{
			len = d.len(dpStart);
			buf = d.tl(dpStart);
			if(!len)
			{	// Empty Line
				rcPrinter.top += cLineHeight;
				++dpStart;
			}
			else
			{
				rctmp = rcPrinter;
				nHi = len; 
				nLo = 0;
				if(!nChars)
				{
					uStart = buf;
					nChars = len;
				}
				else
				{
					uStart += nChars;
					nHi = nChars = len-nChars;
				}

				while (nLo < nHi) { // Find maximum number of chars can be printed
					rctmp.top = rcPrinter.top;
					nThisLineHeight = ::DrawTextW(thePrintDlg.hDC, uStart, nChars, &rctmp, DT_CALCRECT|DT_WORDBREAK|DT_NOCLIP|DT_EXPANDTABS|DT_NOPREFIX|DT_EDITCONTROL);
					if (rcPrinter.top+nThisLineHeight < rcPrinter.bottom)
						nLo = nChars;
					if (rcPrinter.top+nThisLineHeight > rcPrinter.bottom)
						nHi = nChars;
					if (nLo == nHi - 1)
						nChars = nHi = nLo;
					if (nLo < nHi)
						nChars = nLo + (nHi - nLo)/2;
				}
				rcPrinter.top += ::DrawTextW(thePrintDlg.hDC, uStart, nChars, &rcPrinter, DT_WORDBREAK|DT_NOCLIP|DT_EXPANDTABS|DT_NOPREFIX|DT_EDITCONTROL);
				if(uStart+nChars == buf+len) // Line end
				{
					nChars = 0;
					++dpStart;
				}
			}

			// turn to new page
			if( (dpStart<e) && (rcPrinter.top + cLineHeight + 5 > rcPrinter.bottom) )
			{
				::EndPage(thePrintDlg.hDC);
				::StartPage(thePrintDlg.hDC);
				rcPrinter.top = 5;
				rcPrinter.left = 5;
				rcPrinter.right = cWidthPels - 10;
				rcPrinter.bottom = cHeightPels - 10;
			}
		}

		::EndPage(thePrintDlg.hDC);
	} while(++procCopies < totalCopies);

	// Close Printer
	::EndDoc(thePrintDlg.hDC);
	::DeleteDC(thePrintDlg.hDC);

	/*
	::GlobalUnlock(thePrintDlg.hDevNames);
	::GlobalUnlock(thePrintDlg.hDevMode);

	// 解放する。
	::GlobalFree(thePrintDlg.hDevNames);
	::GlobalFree(thePrintDlg.hDevMode);
	*/
}

void GreenPadWnd::on_exit()
{
	search_.SaveToINI( cfg_.getImpl() );
	if( AskToSave() )
		Destroy();
}

void GreenPadWnd::on_initmenu( HMENU menu, bool editmenu_only )
{
#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
	LOGGER("GreenPadWnd::ReloadConfig on_initmenu begin");
	if(app().isWin3later())
	{
		MENUITEMINFO mi = { sizeof(MENUITEMINFO), MIIM_STATE };

		mi.fState =
			(edit_.getCursor().isSelected() ? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_CUT, FALSE, &mi );
		::SetMenuItemInfo( menu, ID_CMD_COPY, FALSE, &mi );
		::SetMenuItemInfo( menu, ID_CMD_DELETE, FALSE, &mi );

		mi.fState =
			(edit_.getDoc().isUndoAble() ? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_UNDO, FALSE, &mi );

		mi.fState =
			(edit_.getCursor().isSelected() && ime().IsIME() && ime().CanReconv() ? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_RECONV, FALSE, &mi );
		mi.fState =
			(ime().IsIME() ? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_TOGGLEIME, FALSE, &mi );

		mi.fState =
			(edit_.getDoc().isRedoAble() ? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_REDO, FALSE, &mi );

		if( editmenu_only )
		{
			LOGGER("GreenPadWnd::ReloadConfig on_initmenu end");
			return;
		}

		mi.fState = (isUntitled() || edit_.getDoc().isModified()
			? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_SAVEFILE, FALSE, &mi );

		mi.fState =
			(!isUntitled() ? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_REOPENFILE, FALSE, &mi );

		mi.fState =
			(cfg_.grepExe().len()>0 ? MFS_ENABLED : MFS_DISABLED);
		::SetMenuItemInfo( menu, ID_CMD_GREP, FALSE, &mi );

		UINT id = (wrap_==-1 ? ID_CMD_NOWRAP
			: (wrap_>0 ? ID_CMD_WRAPWIDTH : ID_CMD_WRAPWINDOW));
		::CheckMenuRadioItem(
			menu, ID_CMD_NOWRAP, ID_CMD_WRAPWINDOW, id, MF_BYCOMMAND );
	}
	else
#endif
	{
		::EnableMenuItem( menu, ID_CMD_CUT, MF_BYCOMMAND|(edit_.getCursor().isSelected() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_COPY, MF_BYCOMMAND|(edit_.getCursor().isSelected() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_DELETE, MF_BYCOMMAND|(edit_.getCursor().isSelected() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_UNDO, MF_BYCOMMAND|(edit_.getDoc().isUndoAble() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_REDO, MF_BYCOMMAND|(edit_.getDoc().isRedoAble() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_RECONV, MF_BYCOMMAND|(edit_.getCursor().isSelected() && ime().IsIME() && ime().CanReconv() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_TOGGLEIME, MF_BYCOMMAND|(ime().IsIME() ? MF_ENABLED : MF_GRAYED) );

		if( editmenu_only )
		{
			LOGGER("GreenPadWnd::ReloadConfig on_initmenu end");
			return;
		}

		::EnableMenuItem( menu, ID_CMD_SAVEFILE, MF_BYCOMMAND|(isUntitled() || edit_.getDoc().isModified() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_REOPENFILE, MF_BYCOMMAND|(!isUntitled() ? MF_ENABLED : MF_GRAYED) );
		::EnableMenuItem( menu, ID_CMD_GREP, MF_BYCOMMAND|(cfg_.grepExe().len()>0 ? MF_ENABLED : MF_GRAYED) );

		::CheckMenuItem( menu, ID_CMD_NOWRAP, MF_BYCOMMAND|(wrap_==-1?MF_CHECKED:MF_UNCHECKED));
		::CheckMenuItem( menu, ID_CMD_WRAPWIDTH, MF_BYCOMMAND|(wrap_>0?MF_CHECKED:MF_UNCHECKED));
		::CheckMenuItem( menu, ID_CMD_WRAPWINDOW, MF_BYCOMMAND|(wrap_==0?MF_CHECKED:MF_UNCHECKED));
	}

	if (app().isCommCtrlAvailable()) {
		::CheckMenuItem( menu, ID_CMD_STATUSBAR,
			cfg_.showStatusBar()?MF_CHECKED:MF_UNCHECKED );
	} else {
		::EnableMenuItem( menu, ID_CMD_STATUSBAR, MF_BYCOMMAND|MF_GRAYED );
	}

	LOGGER("GreenPadWnd::ReloadConfig on_initmenu end");
}

void GreenPadWnd::on_drop( HDROP hd )
{
	UINT iMax = ::DragQueryFile( hd, 0xffffffff, NULL, 0 );

	for( UINT i=0; i<iMax; ++i )
	{
		TCHAR str[MAX_PATH];
		::DragQueryFile( hd, i, str, countof(str) );
		Open( str, AutoDetect );
	}
	::DragFinish( hd );
}

void GreenPadWnd::on_jump()
{
	struct JumpDlg : public DlgImpl {
		JumpDlg(HWND w) : DlgImpl(IDD_JUMP), w_(w) { GoModal(w); }
		void on_init() {
			SetCenter(hwnd(),w_); ::SetFocus(item(IDC_LINEBOX)); }
		bool on_ok() {
			TCHAR str[100];
			::GetWindowText( item(IDC_LINEBOX), str, countof(str) );
			LineNo = String(str).GetInt();
			return true;
		}
		int LineNo; HWND w_;
	} dlg(hwnd());

	if( IDOK == dlg.endcode() )
		JumpToLine( dlg.LineNo );
}

void GreenPadWnd::on_grep()
{
	Path g = cfg_.grepExe();
	if( g.len() != 0 )
	{
		Path d;
		if( filename_.len() )
			(d = filename_).BeDirOnly().BeBackSlash(false);
		else
			d = Path(Path::Cur);

		String fcmd;
		for( int i=0, e=g.len(); i<e; ++i )
			if( g[i]==TEXT('%') )
			{
				if( g[i+1]==TEXT('1') || g[i+1]==TEXT('D') ) // '1' for bkwd compat
					++i, fcmd += d;
				else if( g[i+1]==TEXT('F') )
					++i, fcmd += filename_;
				else if( g[i+1]==TEXT('N') )
					++i, fcmd += filename_.name();
			}
			else
				fcmd += g[i];

		PROCESS_INFORMATION psi;
		STARTUPINFO         sti = {sizeof(STARTUPINFO)};
		//sti.dwFlags = STARTF_USESHOWWINDOW;
		//sti.wShowWindow = SW_SHOWNORMAL;
		if( ::CreateProcess( NULL, const_cast<TCHAR*>(fcmd.c_str()),
				NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL,
				&sti, &psi ) )
		{
			::CloseHandle( psi.hThread );
			::CloseHandle( psi.hProcess );
		}
	}
}

void GreenPadWnd::on_datetime()
{
#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310) || (defined(TARGET_VER) && TARGET_VER==310 && defined(UNICODE))
	String g = cfg_.dateFormat();
	TCHAR buf[255], tmp[255];
	::GetTimeFormat
		( LOCALE_USER_DEFAULT, 0, NULL, g.len()?const_cast<TCHAR*>(g.c_str()):TEXT("HH:mm yyyy/MM/dd"), buf, countof(buf));
	::GetDateFormat
		( LOCALE_USER_DEFAULT, 0, NULL, buf, tmp,countof(tmp));
	if( tmp[0] ) edit_.getCursor().Input( tmp, ::lstrlen(tmp) );
#endif
}

void GreenPadWnd::on_doctype( int no )
{
	if( HMENU m = ::GetSubMenu( ::GetSubMenu(::GetMenu(hwnd()),3),4 ) )
	{
		cfg_.SetDocTypeByMenu( no, m );
		ReloadConfig( true );
	}
}

void GreenPadWnd::on_config()
{
	if( cfg_.DoDialog(*this) )
	{
		SetupSubMenu();
		SetupMRUMenu();
		ReloadConfig(false);
	}
}

static inline void MyShowWnd( HWND wnd )
{
	if( ::IsIconic(wnd) )
		::ShowWindow( wnd, SW_RESTORE );
	::BringWindowToTop( wnd );
}

void GreenPadWnd::on_nextwnd()
{
#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
	if( HWND next = ::FindWindowEx( NULL, hwnd(), className_, NULL ) )
	{
		HWND last=next, pos;
		while( last != NULL )
			last = ::FindWindowEx( NULL, pos=last, className_, NULL );
		if( pos != next )
			::SetWindowPos( hwnd(), pos,
				0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW );
		MyShowWnd( next );
	}
#endif
}

void GreenPadWnd::on_prevwnd()
{
#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
	HWND pos=NULL, next=::FindWindowEx( NULL,NULL,className_,NULL );
	if( next==hwnd() )
	{
		while( next != NULL )
			next = ::FindWindowEx( NULL,pos=next,className_,NULL );
		if( pos!=hwnd())
			MyShowWnd( pos );
	}
	else
	{
		while( next!=hwnd() && next!=NULL )
			next = ::FindWindowEx( NULL,pos=next,className_,NULL );
		if( next!=NULL )
			MyShowWnd( pos );
	}
#endif
}

void GreenPadWnd::on_statusBar()
{
	stb_.SetStatusBarVisible( !stb_.isStatusBarVisible() );
	cfg_.ShowStatusBarSwitch();

	WINDOWPLACEMENT wp = {sizeof(wp)};
	::GetWindowPlacement( hwnd(), &wp );
	if( wp.showCmd != SW_MINIMIZE )
	{
		const int ht = stb_.AutoResize( wp.showCmd == SW_MAXIMIZE );
		RECT rc;
		getClientRect(&rc);
		edit_.MoveTo( 0, 0, rc.right, rc.bottom-ht );
	}
}

void GreenPadWnd::on_move( const DPos& c, const DPos& s )
{
	static int busy_cnt = 0;
	if( edit_.getDoc().isBusy() && ((++busy_cnt)&0xff) )
		return;

	ulong cad = c.ad;
	if( ! cfg_.countByUnicode() )
	{
		// ShiftJIS風のByte数カウント
		const unicode* cu = edit_.getDoc().tl(c.tl);
		const ulong tab = NZero(cfg_.vConfig().tabstep);
		cad = 0;
		for( ulong i=0; i<c.ad; ++i )
			if( cu[i] == L'\t' )
				cad = (cad/tab+1)*tab;
			else if( cu[i]<0x80 || (cu[i]>0xff60 && cu[i]<=0xff9f) )
				cad = cad + 1;
			else
				cad = cad + 2;
	}

	String str;
	str += TEXT('(');
	str += String().SetInt(c.tl+1);
	str += TEXT(',');
	str += String().SetInt(cad+1);
	str += TEXT(')');
	if( c != s )
	{
		ulong sad = s.ad;
		if( ! cfg_.countByUnicode() )
		{
			// ShiftJIS風のByte数カウント
			const unicode* su = edit_.getDoc().tl(s.tl);
			sad = 0;
			for( ulong i=0; i<s.ad; ++i )
				sad += (su[i]<0x80 || (su[i]>0xff60 && su[i]<=0xff9f) ? 1 : 2);
		}
		str += TEXT(" - (");
		str += String().SetInt(s.tl+1);
		str += TEXT(',');
		str += String().SetInt(sad+1);
		str += TEXT(')');
	}
	stb_.SetText( str.c_str() );
}

void GreenPadWnd::on_reconv()
{
	edit_.getCursor().Reconv();
}
void GreenPadWnd::on_toggleime()
{
	edit_.getCursor().ToggleIME();
}

void GreenPadWnd::on_helpabout()
{
	// Crazy double macro so that an int define
	// Can be seen as a string
	#define SHARP(x) #x
	#define STR(x) SHARP(x)

	#if defined(UNICOWS)
		#define UNIANSI TEXT(" (Unicows)")
	#elif defined(UNICODE)
		#define UNIANSI TEXT(" (Unicode)")
	#elif defined(_MBCS)
		#define UNIANSI TEXT(" (MBCS)")
	#else
		#define UNIANSI TEXT(" (ANSI)")
	#endif

	#if defined(__GNUC__)
		#define COMPILER TEXT( "GNU C Compiler - " __VERSION__ "\n" )
	#elif defined(_MSC_VER)
		#define COMPILER TEXT("Visual C++ - ")  +String().SetInt((_MSC_VER-600)/100)+TEXT(".")+String().SetInt(_MSC_VER%100)+
	#elif defined(__WATCOMC__)
		#define COMPILER TEXT("Watcom C++ - ") TEXT(STR(__WATCOMC__))
	#elif defined(__BORLANDC__)
		#define COMPILER TEXT("Borland C++ - ") TEXT(STR(__BORLANDC__))
	#elif defined(__DMC__)
		#define COMPILER TEXT("Digital Mars C++ - ") TEXT(STR(__DMC__))
	#elif defined(__INTEL_COMPILER)
		#define COMPILER TEXT("Intel C++ - ") TEXT(STR(__INTEL_COMPILER))
	#elif defined(__clang__)
		#define COMPILER TEXT("LLVM Clang - ")  TEXT(STR(__clang_major__)) TEXT(".") TEXT(STR(__clang_minor__))
	#else
		//#error Unknown compiler, consider adding it to the list.
		#define COMPILER TEXT( "(unknown compiler)\n" )
	#endif

	#if defined(WIN32S)
		#define TARGETOS TEXT("Win32s")
	#elif defined(UNICODE)
		#define TARGETOS TEXT("Windows NT")
	#else
		#define TARGETOS TEXT("Windows 9x")
	#endif

	#if defined(TARGET_VER)
		#if TARGET_VER == 310 && ( defined(WIN32S) || ( defined(NO_OLEDNDSRC) && defined(NO_OLEDNDTAR) ) )
			#define TGVER TEXT(" 3.10+")
		#elif TARGET_VER == 350 || TARGET_VER == 310 // 3.10 non-Win32s + OLE = 3.50
			#define TGVER TEXT(" 3.50+")
		#else //if TARGET_VER == 351
			#if defined(_M_IA64)
				// 2000/NT5.0 is the first IA64 version of Windows.
				#define TGVER TEXT(" 5.0+")
			#elif defined(_M_ARM64) || defined(_M_ARM)
				// 8/NT6.2 is the first ARM version of Windows.
				#define TGVER TEXT(" 6.2+")
			#elif defined(_M_AMD64) || defined(_M_X64) || defined(WIN64)
				// XP/NT5.1 is the first x64 version of Windows.
				#define TGVER TEXT(" 5.1+")
			#else
				// Default to NT3.51/95 (I guess...)
				#define TGVER TEXT(" 3.51+")
			#endif
		#endif
	#else
		#if defined(_M_IA64)
			// 2000/NT5.0 is the first IA64 version of Windows.
			#define TGVER TEXT(" 5.0+")
		#elif defined(_M_ARM64) || defined(_M_ARM)
			// 8/NT6.2 is the first ARM version of Windows.
			#define TGVER TEXT(" 6.2+")
		#elif defined(_M_AMD64) || defined(_M_X64) || defined(WIN64)
			// XP/NT5.1 is the first x64 version of Windows.
			#define TGVER TEXT(" 5.1+")
		#else
			// Default to NT3.51/95 (I guess...)
			#define TGVER TEXT(" 3.51+")
		#endif
	#endif //TARGET_VER

	#if defined(NO_OLEDNDSRC) && defined(NO_OLEDNDTAR)
		#define USEOLE TEXT(" ")
	#else
		#define USEOLE TEXT(" OLE ")
	#endif //TARGET_VER


	#if defined(_M_AMD64) || defined(_M_X64)
		#define PALT TEXT( "- x86_64" )
	#elif defined(_M_IA64)
		#define PALT TEXT( "- IA64" )
	#elif defined(_M_IX86)
		#define PALT TEXT( "- i386" )
	#elif defined(_M_MRX000) || defined(_MIPS_)
		#define PALT TEXT( "- MIPS" )
	#elif defined(_M_ARM64)
		#define PALT TEXT( "- ARM64" )
	#elif defined(_M_ARM)
		#define PALT TEXT( "- ARM" )
	#elif defined(_M_ALPHA) && defined(WIN64)
		#define PALT TEXT( "- Alpha64" )
	#elif defined(_M_ALPHA)
		#define PALT TEXT( "- Alpha" )
	#elif defined(_M_PPC)
		#define PALT TEXT( "- PPC" )
	#else
		#define PALT TEXT("- (unknown)")
	#endif
	// Show Help->About dialog box.
	struct AboutDlg : public DlgImpl {
		AboutDlg(HWND parent) : DlgImpl(IDD_ABOUTDLG), parent_( parent ) { GoModal(parent_); }
		void on_init()
		{
			String s = String(IDS_APPNAME);
			s += TEXT(" - ") TEXT( VER_FILEVERSIONSTR ) UNIANSI TEXT("\r\n")
			     COMPILER TEXT(" on ") TEXT( __DATE__ ) TEXT("\r\n")
			     TEXT("for ") TARGETOS TGVER USEOLE PALT TEXT("\r\n")
			     TEXT("Running on ");

			if( app().isNT() )
				s += TEXT("Windows NT ");
			else if( app().isWin32s() )
				s += TEXT("Win32s ");
			else
				s+= TEXT("Windows ");

			WORD sver = app().getOSVer();
			s += String().SetInt( HIBYTE(sver) ) + TEXT(".")
			   + String().SetInt( LOBYTE(sver) ) + TEXT(".")
			   + String().SetInt( app().getOSBuild() );

			WORD wDetType = app().getDetectType();
			if( wDetType & MVI_KERNELEX )
				s+= TEXT(" (KEx)");

			SendMsgToItem(IDC_ABOUTSTR, WM_SETTEXT, s.c_str());
			SendMsgToItem(IDC_ABOUTURL, WM_SETTEXT, TEXT("https://github.com/roytam1/rtoss/tree/master/GreenPad"));
			SetCenter(hwnd(), parent_);
		}
		DWORD on_ctlcolor(HDC ctrldc, HWND ctrl)
		{
			LONG dlgStyle;
			DWORD ctrlID = GetDlgCtrlID(ctrl);
			switch(ctrlID) {
				case IDC_ABOUTSTR:
				case IDC_ABOUTURL:
					dlgStyle = GetWindowLong(hwnd(), GWL_STYLE);
					if(dlgStyle & DS_3DLOOK) {
						SetBkColor(ctrldc, GetSysColor(COLOR_BTNFACE));
						SelectObject(ctrldc, MyGetSysColorBrush(COLOR_BTNFACE));
						return (DWORD)MyGetSysColorBrush(COLOR_BTNFACE);
					}
				default:
					return NULL;
			}
		}
		HWND parent_;
	} ahdlg (hwnd());

	#undef UNIANSI
	#undef COMPILER
	#undef TGVER
	#undef USEOLE
	#undef PALT
}

//-------------------------------------------------------------------------
// ユーティリティー
//-------------------------------------------------------------------------

void GreenPadWnd::JumpToLine( ulong ln )
{
	edit_.getCursor().MoveCur( DPos(ln-1,0), false );
}

void GreenPadWnd::SetupSubMenu()
{
	if( HMENU m = ::GetSubMenu( ::GetSubMenu(::GetMenu(hwnd()),3),4 ) )
	{
		cfg_.SetDocTypeMenu( m, ID_CMD_DOCTYPE );
		::DrawMenuBar( hwnd() );
	}
}

void GreenPadWnd::UpdateWindowName()
{
	// タイトルバーに表示される文字列の調整
	// [FileName *] - GreenPad
	static TCHAR cpname[10];

	String name;
	name += TEXT('[');
	name += isUntitled() ? TEXT("untitled") : filename_.name();
	if( edit_.getDoc().isModified() ) name += TEXT(" *");
	name += TEXT("] - ");
	name += String(IDS_APPNAME).c_str();

	SetText( name.c_str() );

	if(csi_ >= 0xf0f00000 && csi_ < 0xf1000000) {
		::wsprintf(cpname,TEXT("CP%d%c"),csi_ & 0xfffff, ::IsValidCodePage(csi_ & 0xfffff) ? ' ' : '*');
		stb_.SetCsText( cpname );
	} else 
		stb_.SetCsText( csi_==0xffffffff?TEXT("UNKN"):charSets_[csi_].shortName );
	stb_.SetLbText( lb_ );
}

void GreenPadWnd::SetupMRUMenu()
{
	if( HMENU m = ::GetSubMenu( ::GetSubMenu(::GetMenu(hwnd()),0),8 ) )
	{
		cfg_.SetUpMRUMenu( m, ID_CMD_MRU );
		::DrawMenuBar( hwnd() );
	}
}

void GreenPadWnd::on_mru( int no )
{
	Path fn = cfg_.GetMRU(no);
	if( fn.len() != 0 )
		Open( fn, AutoDetect );
}



//-------------------------------------------------------------------------
// 設定更新処理
//-------------------------------------------------------------------------

void GreenPadWnd::ReloadConfig( bool noSetDocType )
{
	// 文書タイプロード
	if( !noSetDocType )
	{
		int t = cfg_.SetDocType( filename_ );
		if( HMENU m = ::GetSubMenu( ::GetSubMenu(::GetMenu(hwnd()),3),4 ) )
			cfg_.CheckMenu( m, t );
	}
	LOGGER("GreenPadWnd::ReloadConfig DocTypeLoaded");

	// Undo回数制限
	edit_.getDoc().SetUndoLimit( cfg_.undoLimit() );

	// 行番号
	bool ln = cfg_.showLN();
	edit_.getView().ShowLineNo( ln );

	// 折り返し方式
	wrap_ = cfg_.wrapType();
	edit_.getView().SetWrapType( wrap_ );

	// 色・フォント
	VConfig vc = cfg_.vConfig();
	edit_.getView().SetFont( vc );
	LOGGER("GreenPadWnd::ReloadConfig ViewConfigLoaded");

	// キーワードファイル
	Path kwd = cfg_.kwdFile();
	FileR fp;
	if( kwd.len()!=0 && kwd.isFile() && fp.Open(kwd.c_str()) )
		edit_.getDoc().SetKeyword((const unicode*)fp.base(),fp.size()/2);
	else
		edit_.getDoc().SetKeyword(NULL,0);
	LOGGER("GreenPadWnd::ReloadConfig KeywordLoaded");
}



//-------------------------------------------------------------------------
// 開く処理
//-------------------------------------------------------------------------

bool GreenPadWnd::ShowOpenDlg( Path* fn, int* cs )
{
	// [Open][Cancel] 開くファイル名指定ダイアログを表示
	String flst[] = {
		String(IDS_TXTFILES),
		String(cfg_.txtFileFilter()),
		String(IDS_ALLFILES),
		String(TEXT("*.*"))
	};
	aarr<TCHAR> filt = OpenFileDlg::ConnectWithNull(flst,countof(flst));

	OpenFileDlg ofd( charSets_ );
	bool ok = ofd.DoModal( hwnd(), filt.get(), filename_.c_str() );
	if( ok )
	{
		*fn = ofd.filename();
		*cs = charSets_[ofd.csi()].ID;
	}

	return ok;
}

bool GreenPadWnd::Open( const ki::Path& fn, int cs )
{
	if( isUntitled() && !edit_.getDoc().isModified() )
	{
		// 無題で無変更だったら自分で開く
		return OpenByMyself( fn, cs );
	}
	else
	{
		// 同じ窓で開くモードならそうする
		if( cfg_.openSame() )
			return ( AskToSave() ? OpenByMyself( fn, cs ) : true );

		// そうでなければ他へ回す
		String
			cmd  = TEXT("-c");
			cmd += String().SetInt( cs );
			cmd += TEXT(" \"");
			cmd += fn;
			cmd += TEXT('\"');
		BootNewProcess( cmd.c_str() );
		return true;
	}
}
BOOL CALLBACK GreenPadWnd::PostMsgToFriendsProc(HWND hwnd, LPARAM lPmsg)
{
	TCHAR classn[256];
	if(IsWindow(hwnd)) 
	{
		GetClassName(hwnd, classn, countof(classn));
		if (!lstrcmp(classn, className_))
			PostMessage(hwnd, (UINT)lPmsg, 0, 0);
	}
	return TRUE; // Next hwnd
}
bool GreenPadWnd::PostMsgToAllFriends(UINT msg)
{
	return !!EnumWindows(PostMsgToFriendsProc, (LPARAM)msg);
}
bool GreenPadWnd::OpenByMyself( const ki::Path& fn, int cs, bool needReConf )
{
	// ファイルを開けなかったらそこでおしまい。
	aptr<TextFileR> tf( new TextFileR(cs) );
	if( !tf->Open( fn.c_str() ) )
	{
		// ERROR!
		MsgBox( String(IDS_OPENERROR).c_str() );
		return false;
	}

	// 自分内部の管理情報を更新
	if( fn[0]==TEXT('\\') || fn[1]==TEXT(':') )
		filename_ = fn;
	else
		filename_ = Path( Path::Cur ) + fn;
	if( tf->size() )
	{
		csi_      = charSets_.findCsi( tf->codepage() );
		if( csi_ == 0xffffffff )
			csi_       = 0xf0f00000 | tf->codepage();

		if( tf->nolb_found() )
			lb_       = cfg_.GetNewfileLB();
		else
			lb_       = tf->linebreak();
	}
	else
	{ // 空ファイルの場合は新規作成と同じ扱い
		csi_      = cfg_.GetNewfileCsi();
		lb_       = cfg_.GetNewfileLB();
	}
	filename_.BeShortLongStyle();

	// カレントディレクトリを、ファイルのある位置以外にしておく
	// （こうしないと、開いているファイルのあるディレクトリが削除できない）
	::SetCurrentDirectory( Path(filename_).BeDriveOnly().c_str() );

	// 文書タイプに応じて表示を更新
	if( needReConf )
		ReloadConfig();

	// 開く
	edit_.getDoc().ClearAll();
	stb_.SetText(String(IDS_LOADING).c_str());
	edit_.getDoc().OpenFile( tf );

	// タイトルバー更新
	UpdateWindowName();

	// [最近使ったファイル]へ追加
	cfg_.AddMRU( filename_ );
	PostMsgToAllFriends(GPM_MRUCHANGED);

	return true;
}



//-------------------------------------------------------------------------
// 保存処理
//-------------------------------------------------------------------------

bool GreenPadWnd::ShowSaveDlg()
{
	// [Save][Cancel] 保存先ファイル名指定ダイアログを表示

	String flst[] = {
		String(IDS_ALLFILES),
		String(TEXT("*.*"))
	};
	aarr<TCHAR> filt = SaveFileDlg::ConnectWithNull( flst, countof(flst) );

	SaveFileDlg sfd( charSets_, csi_, lb_ );
	if( !sfd.DoModal( hwnd(), filt.get(), filename_.c_str() ) )
		return false;

	filename_ = sfd.filename();
	csi_      = sfd.csi();
	lb_       = sfd.lb();

	return true;
}

bool GreenPadWnd::Save_showDlgIfNeeded()
{
	bool wasUntitled = isUntitled();

	// [Save][Cancel] ファイル名未定ならダイアログ表示
	if( isUntitled() )
		if( !ShowSaveDlg() )
			return false;
	if( Save() )
	{
		if( wasUntitled )
			ReloadConfig(); // 文書タイプに応じて表示を更新
		return true;
	}
	return false;
}

bool GreenPadWnd::AskToSave()
{
	// 変更されていたら、
	// [Yes][No][Cancel] 保存するかどうか尋ねる。
	// 保存するなら
	// [Save][Cancel]    ファイル名未定ならダイアログ表示

	if( edit_.getDoc().isModified() )
	{
		int answer = MsgBox(
			String(IDS_ASKTOSAVE).c_str(),
			String(IDS_APPNAME).c_str(),
			MB_YESNOCANCEL|MB_ICONQUESTION
		);
		if( answer == IDYES )    return Save_showDlgIfNeeded();
		if( answer == IDCANCEL ) return false;
	}
	return true;
}

bool GreenPadWnd::Save()
{
	int save_Csi;
	
	if(!csi_ || csi_ == 0xffffffff)
		save_Csi = ::GetACP();
	else if(csi_ >= 0xf0f00000 && csi_ < 0xf1000000)
		save_Csi = csi_ & 0xfffff;
	else
		save_Csi = charSets_[csi_].ID;

	TextFileW tf( save_Csi, lb_ );
	if( tf.Open( filename_.c_str() ) )
	{
		// 無事ファイルに保存できた場合
		edit_.getDoc().SaveFile( tf );
		UpdateWindowName();
		// [最近使ったファイル]更新
		cfg_.AddMRU( filename_ );
		PostMsgToAllFriends(GPM_MRUCHANGED);
		return true;
	}

	// Error!
	MsgBox( String(IDS_SAVEERROR).c_str() );
	return false;
}



//-------------------------------------------------------------------------
// メインウインドウの初期化
//-------------------------------------------------------------------------

GreenPadWnd::ClsName GreenPadWnd::className_ = TEXT("GreenPad MainWnd");

GreenPadWnd::GreenPadWnd()
	: WndImpl  ( className_, WS_OVERLAPPEDWINDOW, WS_EX_ACCEPTFILES )
	, charSets_( cfg_.GetCharSetList() )
	, csi_     ( cfg_.GetNewfileCsi() )
	, lb_      ( cfg_.GetNewfileLB() )
	, search_  ( *this, edit_ )
{
	LOGGER( "GreenPadWnd::Construct begin" );

#if !defined(TARGET_VER) || TARGET_VER>350
	static WNDCLASSEX wc;
#else
	static WNDCLASS wc;
#endif
	wc.hIcon         = app().LoadIcon( IDR_MAIN );
	wc.hCursor       = app().LoadOemCursor( IDC_ARROW );
	wc.lpszMenuName  = MAKEINTRESOURCE( IDR_MAIN );
	wc.lpszClassName = className_;
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	WndImpl::Register( &wc );

	ime().EnableGlobalIME( true );

	LOGGER( "GreenPadWnd::Construct end" );
}

void GreenPadWnd::on_create( CREATESTRUCT* cs )
{
	LOGGER("GreenPadWnd::on_create begin");

	accel_ = app().LoadAccel( IDR_MAIN );
	edit_.Create( NULL, hwnd(), 0, 0, 100, 100 );
	LOGGER("GreenPadWnd::on_create edit created");
	edit_.getDoc().AddHandler( this );
	edit_.getCursor().AddHandler( this );
//#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
	stb_.SetParent( hwnd() );
	stb_.SetStatusBarVisible( cfg_.showStatusBar() );
/*#elif defined(TARGET_VER) && TARGET_VER==310
	stb_.SetStatusBarVisible( false );
#endif*/

	LOGGER("GreenPadWnd::on_create halfway");

	search_.LoadFromINI( cfg_.getImpl() );
	SetupSubMenu();
	SetupMRUMenu();

	LOGGER("GreenPadWnd::on_create menu");
}

bool GreenPadWnd::StartUp( const Path& fn, int cs, int ln )
{
	LOGGER( "GreenPadWnd::StartUp begin" );
	Create( 0, 0, cfg_.GetWndX(), cfg_.GetWndY(), cfg_.GetWndW(), cfg_.GetWndH(), 0 );
	ShowUp2(); LOGGER( "showup!" );
	LOGGER( "GreenPadWnd::Created" );
	if( fn.len()==0 || !OpenByMyself( fn, cs ) )
	{
		LOGGER( "for new file..." );

		// ファイルを開か(け)なかった場合
		ReloadConfig( fn.len()==0 );
		LOGGER( "GreenPadWnd::StartUp reloadconfig end" );
		UpdateWindowName();
		LOGGER( "GreenPadWnd::StartUp updatewindowname end" );
	}

	// 指定の行へジャンプ
	if( ln != -1 )
		JumpToLine( ln );

	LOGGER( "GreenPadWnd::StartUp end" );
	return true;
}

void GreenPadWnd::ShowUp2()
{
	Window::ShowUp( cfg_.GetWndM() ? SW_MAXIMIZE : SW_SHOW );
}


//-------------------------------------------------------------------------
// スタートアップルーチン
//	コマンドラインの解析を行う
//-------------------------------------------------------------------------

int kmain()
{
	LOGGER( "kmain() begin" );

	Argv  arg;
	ulong   i;

	LOGGER( "argv processed" );

  //-- まずオプションスイッチを処理

	int optL = -1;
	int optC = 0;

	for( i=1; i<arg.size() && arg[i][0]==TEXT('-'); ++i )
		switch( arg[i][1] )
		{
		case TEXT('c'):
			optC = String::GetInt( arg[i]+2 );
			break;
		case TEXT('l'):
			optL = String::GetInt( arg[i]+2 );
			break;
		}

	LOGGER( "option processed" );

  //-- 次にファイル名

	Path file;

	if( i < arg.size() )
	{
		file = arg[i];
		if( !file.isFile() )
		{
			ulong j; // ""無しで半スペ入りでもそれなりに対処
			for( j=i+1; j<arg.size(); ++j )
			{
				file += ' ';
				file += arg[j];
				if( file.isFile() )
					break;
			}

			if( j==arg.size() )
				file = arg[i];
			else
				i=j;
		}
	}

	LOGGER( "filename processed" );

  //-- 余ってる引数があれば、それで新規プロセス起動

	if( ++i < arg.size() )
	{
		String cmd;
		for( ; i<arg.size(); ++i )
		{
			cmd += TEXT('\"');
			cmd += arg[i];
			cmd += TEXT("\" ");
		}
		::BootNewProcess( cmd.c_str() );
	}

	LOGGER( "newprocess booted" );

  //-- メインウインドウ発進

	GreenPadWnd wnd;
	if( !wnd.StartUp(file,optC,optL) )
		return -1;

	LOGGER( "kmain() startup ok" );

  //-- メインループ

	wnd.ShowUp2();
	LOGGER( "showup!" );
	wnd.MsgLoop();

	LOGGER( "fin" );
	return 0;
}
