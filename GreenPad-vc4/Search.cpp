
#include "stdafx.h"
#include "rsrc/resource.h"
#include "Search.h"
#include "NSearch.h"
#include "RSearch.h"
using namespace ki;
using namespace editwing;
using view::VPos;
using editwing::doc::MacroCommand;

//-------------------------------------------------------------------------
// Replacement escape decoding (regexp mode only) and multiline end calc
//-------------------------------------------------------------------------

static ulong DecodeReplacement( const wchar_t* src, wchar_t* dst )
{
	ulong di = 0;
	for( ulong si=0; src[si]!=L'\0'; ++si )
	{
		if( src[si]==L'\\' && src[si+1]!=L'\0' )
		{
			wchar_t n = src[++si];
			switch( n )
			{
			case L't': dst[di++]=L'\t'; break;
			case L'n': dst[di++]=L'\n'; break;
			case L'r': dst[di++]=L'\r'; break;
			case L'f': dst[di++]=L'\f'; break;
			case L'v': dst[di++]=L'\v'; break;
			case L'a': dst[di++]=L'\a'; break;
			default:   dst[di++]=n; break;
			}
		}
		else
		{
			dst[di++] = src[si];
		}
	}
	dst[di] = L'\0';
	return di;
}

static DPos ReplaceEndPos( const DPos& b, const wchar_t* ustr, ulong ulen )
{
	ulong breaks = 0;
	ulong lastBreakEnd = 0;
	for( ulong i=0; i<ulen; )
	{
		if( ustr[i]==L'\r' )
		{
			++breaks;
			++i;
			if( i<ulen && ustr[i]==L'\n' )
				++i;
			lastBreakEnd = i;
		}
		else if( ustr[i]==L'\n' )
		{
			++breaks;
			++i;
			lastBreakEnd = i;
		}
		else
		{
			++i;
		}
	}
	DPos e;
	if( breaks == 0 )
	{
		e.tl = b.tl;
		e.ad = b.ad + ulen;
	}
	else
	{
		e.tl = b.tl + breaks;
		e.ad = ulen - lastBreakEnd;
	}
	return e;
}



//-------------------------------------------------------------------------

SearchManager::SearchManager( ki::Window& w, editwing::EwEdit& e )
	: searcher_( NULL )
	, edit_( e )
	, DlgImpl( IDD_FINDREPLACE )
	, bIgnoreCase_( true ) // 1.08 default true
	, bRegExp_( false )
	, bDownSearch_( true )
	, mainWnd_( w )
{
}

SearchManager::~SearchManager()
{
}

void SearchManager::SaveToINI( ki::IniFile& ini )
{
	ini.SetSectionAsUserName();
	ini.PutBool( TEXT("SearchIgnoreCase"), bIgnoreCase_ );
	ini.PutBool( TEXT("SearchRegExp"), bRegExp_ );
}

void SearchManager::LoadFromINI( ki::IniFile& ini )
{
	ini.SetSectionAsUserName();
	bIgnoreCase_ = ini.GetBool( TEXT("SearchIgnoreCase"), bIgnoreCase_ );
	bRegExp_     = ini.GetBool( TEXT("SearchRegExp"), bRegExp_ );
}

//-------------------------------------------------------------------------
// ダイアログ関係
//-------------------------------------------------------------------------

void SearchManager::ShowDlg()
{
//	GoModal( ::GetParent(edit_.hwnd()) );
	if( isAlive() )
	{
		SetFront();
	}
	else
	{
		GoModeless( ::GetParent(edit_.hwnd()) );
		ShowUp();
	}
}

bool SearchManager::TrapMsg(MSG* msg)
{
	if( ! isAlive() || type()==MODAL )
		return false;
	return DlgImpl::PreTranslateMessage(msg);
}

void SearchManager::on_init()
{
	if( bIgnoreCase_ )
		SendMsgToItem( IDC_IGNORECASE, BM_SETCHECK, BST_CHECKED );
	if( bRegExp_ )
		SendMsgToItem( IDC_REGEXP, BM_SETCHECK, BST_CHECKED );

	if( edit_.getCursor().isSelected() )
	{
		// 選択されている状態では、基本的にそれをボックスに表示
		ulong dmy;
		aarr<unicode> str = edit_.getCursor().getSelectedStr();

		ulong len=0;
		for( ; str[len]!=L'\0' && str[len]!=L'\n'; ++len );
		str[len] = L'\0';

		if( searcher_.isValid() &&
		    searcher_->Search( str.get(), len, 0, &dmy, &dmy ) )
		{
			SendMsgToItem( IDC_FINDBOX, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(findStr_.c_str()) );
		}
		else
		{
		#ifdef _UNICODE
			SendMsgToItem( IDC_FINDBOX, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(str.get()) );
		#else
			ki::aarr<char> ab( new TCHAR[(len+1)*3] );
			::WideCharToMultiByte( CP_ACP, 0, str.get(), -1,
				ab.get(), (len+1)*3, NULL, NULL );
			SendMsgToItem( IDC_FINDBOX, WM_SETTEXT, 0,
				reinterpret_cast<LPARAM>(ab.get()) );
		#endif
		}
	}
	else
	{
		SendMsgToItem( IDC_FINDBOX, WM_SETTEXT, 0,
			reinterpret_cast<LPARAM>(findStr_.c_str()) );
	}

	SendMsgToItem( IDC_REPLACEBOX, WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>(replStr_.c_str()) );

	::SetFocus( item(IDC_FINDBOX) );
	SendMsgToItem( IDC_FINDBOX, EM_SETSEL, 0,
		::GetWindowTextLength(item(IDC_FINDBOX)) );
}

void SearchManager::on_destroy()
{
	bChanged_ = false;
}

bool SearchManager::on_command( UINT cmd, UINT id, HWND ctrl )
{
	if( cmd==EN_CHANGE )
	{
		// 文字列変更があったことを記憶
		bChanged_ = true;
	}
	else if( cmd==BN_CLICKED )
	{
		switch( id )
		{
		// チェックボックスの変更があったことを記憶
		case IDC_IGNORECASE:
		case IDC_REGEXP:
			bChanged_ = true;
			break;
		// ボタンが押された場合
		case ID_FINDNEXT:
			on_findnext();
			break;
		case ID_FINDPREV:
			on_findprev();
			break;
		case ID_REPLACENEXT:
			on_replacenext();
			break;
		case ID_REPLACEALL:
			on_replaceall();
			break;
		}
	}
	else
	{
		return false;
	}
	return true;
}

void SearchManager::on_findnext()
{
	if( edit_.getDoc().isBusy() )
		return;
	UpdateData();
	ConstructSearcher();
	if( isReady() )
	{
		FindNextImpl();
//		End( IDOK );
	}
}

void SearchManager::on_findprev()
{
	if( edit_.getDoc().isBusy() )
		return;
	UpdateData();
	ConstructSearcher( false );
	if( isReady() )
		FindPrevImpl();
}

void SearchManager::on_replacenext()
{
	if( edit_.getDoc().isBusy() )
		return;
	UpdateData();
	ConstructSearcher();
	if( isReady() )
		ReplaceImpl();
}

void SearchManager::on_replaceall()
{
	if( edit_.getDoc().isBusy() )
		return;
	UpdateData();
	ConstructSearcher();
	if( isReady() )
		ReplaceAllImpl();
}

void SearchManager::UpdateData()
{
	// ダイアログから変更点を取り込み
	bIgnoreCase_ =
		(BST_CHECKED==SendMsgToItem( IDC_IGNORECASE, BM_GETCHECK ));
	bRegExp_ =
		(BST_CHECKED==SendMsgToItem( IDC_REGEXP, BM_GETCHECK ));

	TCHAR* str;
	LRESULT n = SendMsgToItem( IDC_FINDBOX, WM_GETTEXTLENGTH );
	str = new TCHAR[n+1];
	SendMsgToItem( IDC_FINDBOX, WM_GETTEXT,
		n+1, reinterpret_cast<LPARAM>(str) );
	findStr_ = str;
	delete [] str;

	n = SendMsgToItem( IDC_REPLACEBOX, WM_GETTEXTLENGTH );
	str = new TCHAR[n+1];
	SendMsgToItem( IDC_REPLACEBOX, WM_GETTEXT,
		n+1, reinterpret_cast<LPARAM>(str) );
	replStr_ = str;
	delete [] str;
}

void SearchManager::ConstructSearcher( bool down )
{
	bChanged_ = (bChanged_ || (bDownSearch_ != down));
	if( (bChanged_ || !isReady()) && findStr_.len()!=0 )
	{
		// 検索者作成
		bDownSearch_ = down;
		const unicode *u = findStr_.ConvToWChar();

		if( bRegExp_ )
			searcher_ = new RSearch( u, !bIgnoreCase_, bDownSearch_ );
		else
			if( bDownSearch_ )
				if( bIgnoreCase_ )
					searcher_ = new NSearch<IgnoreCase>(u);
				else
					searcher_ = new NSearch<CaseSensitive>(u);
			else
				if( bIgnoreCase_ )
					searcher_ = new NSearchRev<IgnoreCase>(u);
				else
					searcher_ = new NSearchRev<CaseSensitive>(u);

		findStr_.FreeWCMem(u);

		// 変更終了フラグ
		bChanged_ = false;
	}
}



//-------------------------------------------------------------------------

void SearchManager::FindNext()
{
	if( !isReady() )
	{
		ShowDlg();
	}
	else
	{
		ConstructSearcher();
		if( isReady() )
			FindNextImpl();
	}
}

void SearchManager::FindPrev()
{
	if( !isReady() )
	{
		ShowDlg();
	}
	else
	{
		ConstructSearcher( false );
		if( isReady() )
			FindPrevImpl();
	}
}



//-------------------------------------------------------------------------
// 実際の処理の実装
//-------------------------------------------------------------------------

void SearchManager::FindNextImpl()
{
	// カーソル位置取得
	const VPos *stt, *end;
	edit_.getCursor().getCurPos( &stt, &end );

	// 選択範囲ありなら、選択範囲先頭の１文字先から検索
	// そうでなければカーソル位置から検索
	DPos s = *stt;
	if( *stt != *end )
		if( stt->ad == edit_.getDoc().len(stt->tl) )
			s = DPos( stt->tl+1, 0 );
		else
			s = DPos( stt->tl, stt->ad+1 );

	// 検索
	DPos b, e;
	if( FindNextFromImpl( s, &b, &e ) )
	{
		// 見つかったら選択
		edit_.getCursor().MoveCur( b, false );
		edit_.getCursor().MoveCur( e, true );
		return;
	}

	// 見つからなかった場合
	NotFound();
}

void SearchManager::NotFound()
{
	//MsgBox( String(IDS_NOTFOUND).c_str() );
	::MessageBox( NULL, String(IDS_NOTFOUND).c_str(), NULL, MB_OK|MB_TASKMODAL );
}

void SearchManager::FindPrevImpl()
{
	// カーソル位置取得
	const VPos *stt, *end;
	edit_.getCursor().getCurPos( &stt, &end );

	if( stt->ad!=0 || stt->tl!=0 )
	{
		// 選択範囲先頭の１文字前から検索
		DPos s;
		if( stt->ad == 0 )
			s = DPos( stt->tl-1, edit_.getDoc().len(stt->tl-1) );
		else
			s = DPos( stt->tl, stt->ad-1 );

		// 検索
		DPos b, e;
		if( FindPrevFromImpl( s, &b, &e ) )
		{
			// 見つかったら選択
			edit_.getCursor().MoveCur( b, false );
			edit_.getCursor().MoveCur( e, true );
			return;
		}
	}

	// 見つからなかった場合
	NotFound();
}

bool SearchManager::FindNextFromImpl( DPos s, DPos* beg, DPos* end )
{
	// １行ずつサーチ
	doc::Document& d = edit_.getDoc();
	for( ulong mbg,med,e=d.tln(); s.tl<e; ++s.tl, s.ad=0 )
		if( searcher_->Search(
			d.tl(s.tl), d.len(s.tl), s.ad, &mbg, &med ) )
		{
			beg->tl = end->tl = s.tl;
			beg->ad = mbg;
			end->ad = med;
			return true; // 発見
		}
	return false;
}

bool SearchManager::FindPrevFromImpl( DPos s, DPos* beg, DPos* end )
{
	// １行ずつサーチ
	doc::Document& d = edit_.getDoc();
	for( ulong mbg,med; ; s.ad=d.len(--s.tl) )
	{
		if( searcher_->Search(
			d.tl(s.tl), d.len(s.tl), s.ad, &mbg, &med ) )
		{
			beg->tl = end->tl = s.tl;
			beg->ad = mbg;
			end->ad = med;
			return true; // 発見
		}
		if( s.tl==0 )
			break;
	}
	return false;
}

void SearchManager::ReplaceImpl()
{
	// カーソル位置取得
	const VPos *stt, *end;
	edit_.getCursor().getCurPos( &stt, &end );

	// 選択範囲先頭から検索
	DPos b, e;
	if( FindNextFromImpl( *stt, &b, &e ) )
		if( e == *end )
		{
			const wchar_t* raw = replStr_.ConvToWChar();
			const ulong rawLen = my_lstrlenW( raw );
			unicode* dec = NULL;
			const unicode* ustr = raw;
			ulong ulen = rawLen;
			if( bRegExp_ )
			{
				dec = new unicode[rawLen+1];
				ulen = DecodeReplacement( raw, dec );
				ustr = dec;
			}

			// 置換
			edit_.getDoc().Execute( doc::Replace(
				b, e, ustr, ulen
			) );

			DPos nxt = ReplaceEndPos( b, ustr, ulen );
			replStr_.FreeWCMem( raw );
			delete [] dec;

			if( FindNextFromImpl( nxt, &b, &e ) )
			{
				// 次を選択
				edit_.getCursor().MoveCur( b, false );
				edit_.getCursor().MoveCur( e, true );
				return;
			}
		}
		else
		{
			// そうでなければとりあえず選択
			edit_.getCursor().MoveCur( b, false );
			edit_.getCursor().MoveCur( e, true );
			return;
		}

	// 見つからなかった場合
	NotFound();
}

void SearchManager::ReplaceAllImpl()
{
	// まず、実行する置換を全てここに登録する
	MacroCommand mcr;

	// 置換後文字列
	const wchar_t* raw = replStr_.ConvToWChar();
	const ulong rawLen = my_lstrlenW( raw );
	unicode* dec = NULL;
	const unicode* ustr = raw;
	ulong ulen = rawLen;
	if( bRegExp_ )
	{
		dec = new unicode[rawLen+1];
		ulen = DecodeReplacement( raw, dec );
		ustr = dec;
	}

	// 文書の頭から検索
	ulong tlAdd = 0;
	long adDif = 0;
	ulong prevTl = 0;
	bool firstHit = true;
	DPos s(0,0), b, e;
	while( FindNextFromImpl( s, &b, &e ) )
	{
		if( b.tl == e.tl && b.ad == e.ad )
		{
			if( s.ad < edit_.getDoc().len( s.tl ) )
				s.ad = s.ad + 1;
			else
				s = DPos( s.tl+1, 0 );
			if( s.tl >= edit_.getDoc().tln() )
				break;
			continue;
		}
		if( firstHit )
		{
			prevTl = b.tl;
			firstHit = false;
		}
		else if( b.tl != prevTl )
		{
			adDif = 0;
			prevTl = b.tl;
		}
		s = e;
		ulong obAd = b.ad;
		ulong oeAd = e.ad;
		b.tl += tlAdd; b.ad += adDif;
		e.tl += tlAdd; e.ad += adDif;

		// 置換コマンドを登録
		mcr.Add( new doc::Replace(b,e,ustr,ulen) );
		ulong newBreaks = 0;
		ulong lastSeg = ulen;
		for( ulong ri=0; ri<ulen; )
		{
			if( ustr[ri]==L'\r' )
			{
				++newBreaks;
				++ri;
				if( ri<ulen && ustr[ri]==L'\n' )
					++ri;
				lastSeg = ulen - ri;
			}
			else if( ustr[ri]==L'\n' )
			{
				++newBreaks;
				++ri;
				lastSeg = ulen - ri;
			}
			else
			{
				++ri;
			}
		}
		if( newBreaks == 0 )
			adDif += (long)ulen - (long)(oeAd - obAd);
		else
			adDif = (long)lastSeg - (long)oeAd;
		tlAdd += newBreaks;
	}

	if( mcr.size() > 0 )
	{
		// ここで連続置換
		edit_.getDoc().Execute( mcr );
		// カーソル移動
		DPos lastEnd = ReplaceEndPos( b, ustr, ulen );
		edit_.getCursor().MoveCur( lastEnd, false );
		// 閉じる？
		End( IDOK );
	}

	TCHAR str[255];
	::wsprintf( str, String(IDS_REPLACEALLDONE).c_str(), mcr.size() );
	MsgBox( str, String(IDS_APPNAME).c_str(), MB_ICONINFORMATION );

	replStr_.FreeWCMem( raw );
	delete [] dec;
}
