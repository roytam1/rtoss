
#include "stdafx.h"
#include "rsrc/resource.h"
#include "Search.h"
#include "NSearch.h"
#include "RSearch.h"
using namespace ki;
using namespace editwing;
using view::VPos;

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
			case L'u': {
				// \uXXXX[X[X]] : 4-6 hex digits, value <= 0x10FFFF.
				// Astral values emit a surrogate pair. Anything
				// else stays literal.
				unsigned long v = 0;
				ulong k = 0;
				for( ; k<6 && src[si+1+k]!=L'\0'; ++k )
				{
					wchar_t d = src[si+1+k];
					if( L'0'<=d && d<=L'9' ) v = v*16 + (unsigned long)(d-L'0');
					else if( L'a'<=d && d<=L'f' ) v = v*16 + (unsigned long)(d-L'a'+10);
					else if( L'A'<=d && d<=L'F' ) v = v*16 + (unsigned long)(d-L'A'+10);
					else break;
				}
				if( k >= 4 && v <= 0x10FFFFUL )
				{
					si += k;
					if( v <= 0xFFFFUL )
						dst[di++]= (wchar_t)v;
					else
					{
						v -= 0x10000UL;
						dst[di++]= (wchar_t)(0xD800UL + (v>>10));
						dst[di++]= (wchar_t)(0xDC00UL + (v&0x3FFUL));
					}
				}
				else
					dst[di++]=n;
				break;
			}
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
	, bChanged_( true )
	, bEscapes_( false )
	, bMultiline_( false )
	, bMultiSearch_( false )
	, winCap_( 0 )
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
	ini.PutBool( TEXT("SearchEscapeCodes"), bEscapes_ );
	ini.PutBool( TEXT("SearchMultiline"), bMultiline_ );
}

void SearchManager::LoadFromINI( ki::IniFile& ini )
{
	ini.SetSectionAsUserName();
	bIgnoreCase_ = ini.GetBool( TEXT("SearchIgnoreCase"), bIgnoreCase_ );
	bRegExp_     = ini.GetBool( TEXT("SearchRegExp"), bRegExp_ );
	bEscapes_    = ini.GetBool( TEXT("SearchEscapeCodes"), bEscapes_ );
	bMultiline_  = ini.GetBool( TEXT("SearchMultiline"), bMultiline_ );
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
	if( bEscapes_ )
		SendMsgToItem( IDC_ESCAPES, BM_SETCHECK, BST_CHECKED );
	if( bMultiline_ )
		SendMsgToItem( IDC_MULTILINE, BM_SETCHECK, BST_CHECKED );
	::EnableWindow( item( IDC_ESCAPES ), !bRegExp_ );
	::EnableWindow( item( IDC_MULTILINE ), bRegExp_ );

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
		case IDC_ESCAPES:
		case IDC_MULTILINE:
			bChanged_ = true;
			break;
		case IDC_REGEXP:
			bChanged_ = true;
			{
				bool rx = (BST_CHECKED==SendMsgToItem( IDC_REGEXP, BM_GETCHECK ));
				::EnableWindow( item( IDC_ESCAPES ), !rx );
				::EnableWindow( item( IDC_MULTILINE ), rx );
			}
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
	bEscapes_ =
		(BST_CHECKED==SendMsgToItem( IDC_ESCAPES, BM_GETCHECK ));
	bMultiline_ =
		(BST_CHECKED==SendMsgToItem( IDC_MULTILINE, BM_GETCHECK ));

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
		const unicode *key = u;
		unicode *decKey = NULL;
		if( !bRegExp_ && bEscapes_ )
		{
			ulong rawLen = my_lstrlenW( u );
			decKey = new unicode[rawLen+1];
			ulong dlen = DecodeReplacement( u, decKey );
			decKey[dlen] = L'\0';
			key = decKey;
		}

		if( bRegExp_ )
		{
			searcher_ = new RSearch( key, !bIgnoreCase_, bDownSearch_, bMultiline_ );
			bMultiSearch_ = (bMultiline_ && searcher_->canSpanLines());
		}
		else
		{
			bool plainMulti = false;
			if( bEscapes_ && decKey!=NULL )
			{
				ulong nd = 0;
				for( ulong si=0; key[si]!=L'\0'; )
				{
					if( key[si]==L'\r' )
					{
						if( key[si+1]==L'\n' )
							++si;
						decKey[nd++]=L'\n';
						++si;
					}
					else
						decKey[nd++]=key[si++];
				}
				decKey[nd]=L'\0';
				key = decKey;
			}
			for( const unicode* pp=key; *pp!=L'\0'; ++pp )
				if( *pp==L'\n' || *pp==L'\r' )
					{ plainMulti = true; break; }
			bMultiSearch_ = plainMulti;
			if( key[0]==L'\0' )
			{
				searcher_ = NULL;
			}
			else if( bDownSearch_ )
				if( bIgnoreCase_ )
					searcher_ = new NSearch<IgnoreCase>(key);
				else
					searcher_ = new NSearch<CaseSensitive>(key);
			else
				if( bIgnoreCase_ )
					searcher_ = new NSearchRev<IgnoreCase>(key);
				else
					searcher_ = new NSearchRev<CaseSensitive>(key);
		}

		findStr_.FreeWCMem(u);
		delete [] decKey;

		// 変更終了フラグ
		bChanged_ = false;
	}
	else if( !bChanged_ && isReady() )
	{
		if( findStr_.len()==0 )
			bMultiSearch_ = false;
		else if( bRegExp_ )
		{
			if( !bMultiline_ )
				bMultiSearch_ = false;
			else
				bMultiSearch_ = searcher_->canSpanLines();
		}
		else if( bEscapes_ )
		{
			const unicode *ru = findStr_.ConvToWChar();
			ulong rlen = my_lstrlenW( ru );
			unicode *rd = new unicode[rlen+1];
			ulong rdlen = DecodeReplacement( ru, rd );
			rd[rdlen]=L'\0';
			ulong w = 0;
			for( ulong r=0; rd[r]!=L'\0'; )
			{
				if( rd[r]==L'\r' )
				{
					if( rd[r+1]==L'\n' )
						++r;
					rd[w++]=L'\n';
					++r;
				}
				else
					rd[w++]=rd[r++];
			}
			rd[w]=L'\0';
			bool pm = false;
			for( ulong r=0; rd[r]!=L'\0'; ++r )
				if( rd[r]==L'\n' )
					{ pm = true; break; }
			bMultiSearch_ = pm;
			findStr_.FreeWCMem( ru );
			delete [] rd;
		}
		else
			bMultiSearch_ = false;
	}
	else
	{
		bMultiSearch_ = false;
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
	if( bMultiSearch_ )
		return FindNextMultiFromImpl( s, beg, end );
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
	if( bMultiSearch_ )
		return FindPrevMultiFromImpl( s, beg, end );
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

ulong SearchManager::BuildWindow( ulong tl0, ulong ad0 )
{
	doc::Document& d = edit_.getDoc();
	ulong total = d.tln();
	winOffs_.ForceSize( 0 );
	if( tl0 >= total )
		return 0;
	if( !winBuf_.isValid() )
	{
		aarr<unicode> nb( new unicode[WinMax] );
		winBuf_ = nb;
		winCap_ = WinMax;
	}
	unicode* buf = winBuf_.get();
	ulong pos = 0;
	for( ulong tl=tl0; tl<total && pos<WinMax; ++tl )
	{
		const unicode* line = d.tl( tl );
		ulong llen = d.len( tl );
		ulong startAd = (tl==tl0 ? ad0 : 0);
		if( startAd > llen )
			startAd = llen;
		winOffs_.Add( pos );
		ulong copy = llen - startAd;
		if( copy > WinMax - pos )
			copy = WinMax - pos;
		for( ulong i=0; i<copy; ++i )
			buf[pos+i] = line[startAd+i];
		pos += copy;
		if( copy != llen - startAd )
			break;
		if( tl+1 < total )
		{
			if( pos < WinMax )
				buf[pos++] = L'\n';
			else
				break;
		}
	}
	return pos;
}

DPos SearchManager::MapWindowOffset( ulong tl0, const ulong* offs, ulong nlines, ulong pos )
{
	DPos p( tl0, 0 );
	if( nlines == 0 )
		return p;
	if( pos < offs[0] )
		return p;
	ulong l = 0, r = nlines;
	while( l+1 < r )
	{
		ulong m = (l+r)>>1;
		if( offs[m] <= pos )
			l = m;
		else
			r = m;
	}
	p.tl = tl0 + l;
	p.ad = pos - offs[l];
	return p;
}

bool SearchManager::FindNextMultiFromImpl( DPos s, DPos* beg, DPos* end )
{
	doc::Document& d = edit_.getDoc();
	ulong total = d.tln();
	if( total==0 || s.tl>=total )
		return false;
	ulong llen0 = d.len( s.tl );
	if( s.ad > llen0 )
		s.ad = llen0;
	if( !winBuf_.isValid() )
	{
		aarr<unicode> nb( new unicode[WinMax] );
		winBuf_ = nb;
		winCap_ = WinMax;
	}
	unicode* buf = winBuf_.get();
	bool isHead = searcher_->isHeadType();
	bool isTail = searcher_->isTailType();
	for( ulong tl0=s.tl; tl0<total; )
	{
		ulong winLen = BuildWindow( tl0, 0 );
		ulong nlines = winOffs_.size();
		if( winLen==0 || nlines==0 )
			break;
		const ulong* offs = winOffs_.head();
		ulong startOff = 0;
		if( tl0 == s.tl )
		{
			startOff = offs[0] + s.ad;
			if( startOff > winLen )
				startOff = winLen;
		}
		ulong cur = startOff;
		while( cur < winLen )
		{
			ulong mbg = 0, med = 0;
			if( !searcher_->Search( buf, winLen, cur, &mbg, &med ) )
				break;
			bool ok = true;
			if( isHead )
			{
				if( mbg!=0 && buf[mbg-1]!=L'\n' )
					ok = false;
			}
			if( ok && isTail )
			{
				if( med < winLen )
				{
					if( buf[med]!=L'\n' )
						ok = false;
				}
				else
				{
					DPos epos = MapWindowOffset( tl0, offs, nlines, med );
					if( epos.tl >= total )
						ok = false;
					else
					{
						ulong llen = d.len( epos.tl );
						if( !(epos.ad==llen || epos.ad==0) )
							ok = false;
					}
				}
			}
			if( ok )
			{
				*beg = MapWindowOffset( tl0, offs, nlines, mbg );
				*end = MapWindowOffset( tl0, offs, nlines, med );
				if( *end < *beg )
					ok = false;
				else
					return true;
			}
			if( mbg+1 <= cur )
				cur++;
			else
				cur = mbg+1;
		}
		if( tl0 + nlines >= total )
			break;
		ulong step = nlines;
		if( winLen >= WinMax && nlines > 65 )
			step = nlines - 64;
		if( step == 0 )
			step = 1;
		tl0 += step;
	}
	return false;
}

bool SearchManager::FindPrevMultiFromImpl( DPos s, DPos* beg, DPos* end )
{
	doc::Document& d = edit_.getDoc();
	ulong total = d.tln();
	if( total==0 )
		return false;
	if( s.tl >= total )
	{
		s.tl = total-1;
		s.ad = d.len( s.tl );
	}
	else
	{
		ulong llen = d.len( s.tl );
		if( s.ad > llen )
			s.ad = llen;
	}
	if( !winBuf_.isValid() )
	{
		aarr<unicode> nb( new unicode[WinMax] );
		winBuf_ = nb;
		winCap_ = WinMax;
	}
	unicode* buf = winBuf_.get();
	bool isHead = searcher_->isHeadType();
	bool isTail = searcher_->isTailType();
	ulong tl0 = s.tl;
	for( ;; )
	{
		ulong winLen = BuildWindow( tl0, 0 );
		ulong nlines = winOffs_.size();
		if( winLen==0 || nlines==0 )
		{
			if( tl0 == 0 )
				break;
			tl0 = (tl0 > 1 ? tl0-1 : 0);
			if( tl0 >= s.tl )
				break;
			continue;
		}
		const ulong* offs = winOffs_.head();
		ulong maxStart = winLen;
		bool containsS = (s.tl>=tl0 && s.tl<tl0+nlines);
		if( containsS )
		{
			ulong idx = s.tl - tl0;
			maxStart = offs[idx] + s.ad;
			if( maxStart >= winLen && winLen>0 )
				maxStart = winLen-1;
		}
		else if( tl0+nlines-1 < s.tl )
		{
			maxStart = (winLen>0 ? winLen-1 : 0);
		}
		else
		{
			if( tl0 == 0 )
				break;
			tl0 = (tl0 > 1 ? tl0-1 : 0);
			continue;
		}
		ulong curMax = maxStart;
		for( ;; )
		{
			ulong mbg = 0, med = 0;
			if( !searcher_->Search( buf, winLen, curMax, &mbg, &med ) )
				break;
			if( mbg > curMax )
				break;
			bool ok = true;
			if( isHead )
			{
				if( mbg!=0 && buf[mbg-1]!=L'\n' )
					ok = false;
			}
			if( ok && isTail )
			{
				if( med < winLen )
				{
					if( buf[med]!=L'\n' )
						ok = false;
				}
				else
				{
					DPos epos = MapWindowOffset( tl0, offs, nlines, med );
					if( epos.tl >= total )
						ok = false;
					else
					{
						ulong llen = d.len( epos.tl );
						if( !(epos.ad==llen || epos.ad==0) )
							ok = false;
					}
				}
			}
			if( ok )
			{
				DPos bpos = MapWindowOffset( tl0, offs, nlines, mbg );
				DPos epos = MapWindowOffset( tl0, offs, nlines, med );
				if( !(epos < bpos) && !(s < bpos) )
				{
					*beg = bpos;
					*end = epos;
					return true;
				}
			}
			if( mbg == 0 )
				break;
			curMax = mbg-1;
		}
		if( tl0 == 0 )
			break;
		ulong step = nlines;
		if( winLen >= WinMax && nlines > 65 )
			step = nlines - 64;
		if( step == 0 )
			step = 1;
		if( tl0 < step )
			tl0 = 0;
		else
			tl0 -= step;
		if( tl0 > s.tl )
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
			if( bRegExp_ || bEscapes_ )
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
	doc::MacroCommand mcr;

	// 置換後文字列
	const wchar_t* raw = replStr_.ConvToWChar();
	const ulong rawLen = my_lstrlenW( raw );
	unicode* dec = NULL;
	const unicode* ustr = raw;
	ulong ulen = rawLen;
	if( bRegExp_ || bEscapes_ )
	{
		dec = new unicode[rawLen+1];
		ulen = DecodeReplacement( raw, dec );
		ustr = dec;
	}

	ulong newBreaksAll = 0;
	ulong lastSegAll = ulen;
	for( ulong ri=0; ri<ulen; )
	{
		if( ustr[ri]==L'\r' )
		{
			++newBreaksAll;
			++ri;
			if( ri<ulen && ustr[ri]==L'\n' )
				++ri;
			lastSegAll = ulen - ri;
		}
		else if( ustr[ri]==L'\n' )
		{
			++newBreaksAll;
			++ri;
			lastSegAll = ulen - ri;
		}
		else
		{
			++ri;
		}
	}

	DPos lastB(0,0);
	bool haveLast = false;
	if( bMultiSearch_ )
	{
		ulong tlAddM = 0;
		bool firstM = true;
		DPos prevOrigEnd(0,0), prevEnd(0,0);
		DPos s(0,0), b, e;
		while( FindNextFromImpl( s, &b, &e ) )
		{
			if( b == e )
			{
				if( s.ad < edit_.getDoc().len( s.tl ) )
					s.ad = s.ad + 1;
				else
					s = DPos( s.tl+1, 0 );
				if( s.tl >= edit_.getDoc().tln() )
					break;
				continue;
			}
			DPos ob = b, oe = e;
			ulong origBreaks = (oe.tl >= ob.tl ? oe.tl - ob.tl : 0);
			DPos nb = ob, ne = oe;
			nb.tl = ob.tl + tlAddM;
			ne.tl = oe.tl + tlAddM;
			if( !firstM && ob.tl == prevOrigEnd.tl )
				nb.ad = prevEnd.ad + (ob.ad - prevOrigEnd.ad);
			else
				nb.ad = ob.ad;
			if( !firstM && oe.tl == prevOrigEnd.tl )
				ne.ad = prevEnd.ad + (oe.ad - prevOrigEnd.ad);
			else
				ne.ad = oe.ad;
			s = oe;
			mcr.Add( new doc::Replace(nb,ne,ustr,ulen) );
			prevOrigEnd = oe;
			prevEnd = ReplaceEndPos( nb, ustr, ulen );
			tlAddM += newBreaksAll;
			if( origBreaks <= tlAddM )
				tlAddM -= origBreaks;
			else
				tlAddM = 0;
			lastB = nb;
			haveLast = true;
			firstM = false;
		}
	}

	else
	{

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
		lastB = b;
		haveLast = true;
	}
	}

	if( mcr.size() > 0 )
	{
		// ここで連続置換
		edit_.getDoc().Execute( mcr );
		// カーソル移動
		DPos lastEnd = ReplaceEndPos( lastB, ustr, ulen );
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
