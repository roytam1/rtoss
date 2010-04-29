#include "stdafx.h"
#include "rsrc/resource.h"
#include "kilib/kilib.h"
#include "OpenSaveDlg.h"
using namespace ki;



//------------------------------------------------------------------------
// 文字コードリスト
//------------------------------------------------------------------------

CharSetList::CharSetList()
	: list_( 30 )
{
	static const TCHAR* const lnmJp[] = {
		TEXT("自動判定"),
		TEXT("日本語(ShiftJIS)"),
		TEXT("日本語(EUC)"),
		TEXT("日本語(ISO-2022-JP)"),
		TEXT("UTF-5"),
		TEXT("UTF-7"),
		TEXT("UTF-8"),
		TEXT("UTF-8N"),
		TEXT("UTF-16BE(BOM)"),
		TEXT("UTF-16LE(BOM)"),
		TEXT("UTF-16BE"),
		TEXT("UTF-16LE"),
		TEXT("UTF-32BE(BOM)"),
		TEXT("UTF-32LE(BOM)"),
		TEXT("UTF-32BE"),
		TEXT("UTF-32LE"),
		TEXT("欧米"),
		TEXT("中欧"),
		TEXT("韓国語(EUC-KR)"),
		TEXT("韓国語(ISO-2022-KR)"),
		TEXT("韓国語(Johab)"),
		TEXT("中国語(GB2312)"),
		TEXT("中国語(ISO-2022-CN)"),
		TEXT("中国語(HZ)"),
		TEXT("中国語(Big5)"),
		TEXT("キリル語(Windows)"),
		TEXT("キリル語(KOI8-R)"),
		TEXT("キリル語(KOI8-U)"),
		TEXT("タイ語"),
		TEXT("トルコ語"),
		TEXT("バルト語"),
		TEXT("ベトナム語"),
		TEXT("ギリシャ語"),
		TEXT("MSDOS(us)")
	};
	static const TCHAR* const lnmEn[] = {
		TEXT("AutoDetect"),
		TEXT("Japanese(ShiftJIS)"),
		TEXT("Japanese(EUC)"),
		TEXT("Japanese(ISO-2022-JP)"),
		TEXT("UTF-5"),
		TEXT("UTF-7"),
		TEXT("UTF-8"),
		TEXT("UTF-8N"),
		TEXT("UTF-16BE(BOM)"),
		TEXT("UTF-16LE(BOM)"),
		TEXT("UTF-16BE"),
		TEXT("UTF-16LE"),
		TEXT("UTF-32BE(BOM)"),
		TEXT("UTF-32LE(BOM)"),
		TEXT("UTF-32BE"),
		TEXT("UTF-32LE"),
		TEXT("Latin-1"),
		TEXT("Latin-2"),
		TEXT("Korean(EUC-KR)"),
		TEXT("Korean(ISO-2022-KR)"),
		TEXT("Korean(Johab)"),
		TEXT("Chinese(GB2312)"),
		TEXT("Chinese(ISO-2022-CN)"),
		TEXT("Chinese(HZ)"),
		TEXT("Chinese(Big5)"),
		TEXT("Cyrillic(Windows)"),
		TEXT("Cyrillic(KOI8-R)"),
		TEXT("Cyrillic(KOI8-U)"),
		TEXT("Thai"),
		TEXT("Turkish"),
		TEXT("Baltic"),
		TEXT("Vietnamese"),
		TEXT("Greek"),
		TEXT("MSDOS(us)")
	};
	static const TCHAR* const snm[] = {
		TEXT(""),
		TEXT("SJIS"),
		TEXT("EUC"),
		TEXT("JIS"),
		TEXT("UTF5"),
		TEXT("UTF7"),
		TEXT("UTF8"),
		TEXT("UTF8"),
		TEXT("U16B"),
		TEXT("U16L"),
		TEXT("U16B"),
		TEXT("U16L"),
		TEXT("U32B"),
		TEXT("U32L"),
		TEXT("U32B"),
		TEXT("U32L"),
		TEXT("LTN1"),
		TEXT("LTN2"),
		TEXT("UHC"),
		TEXT("I2KR"),
		TEXT("Jhb"),
		TEXT("GBK"),
		TEXT("I2CN"),
		TEXT("HZ"),
		TEXT("BIG5"),
		TEXT("CYRL"),
		TEXT("KO8R"),
		TEXT("KO8U"),
		TEXT("THAI"),
		TEXT("TRK"),
		TEXT("BALT"),
		TEXT("VTNM"),
		TEXT("GRK"),
		TEXT("DOS")
	};

	// 日本語環境なら日本語表示を選ぶ
	const TCHAR* const * lnm = (::GetACP()==932 ? lnmJp : lnmEn);

	// いちいち書くの面倒なので短縮表記(^^;
	CsInfo cs;
	#define Enroll(_id,_nm)   cs.ID=_id,             \
		cs.longName=lnm[_nm], cs.shortName=snm[_nm], \
		cs.type=LOAD|SAVE,    list_.Add( cs )
	#define EnrollS(_id,_nm)  cs.ID=_id,             \
		cs.longName=lnm[_nm], cs.shortName=snm[_nm], \
		cs.type=SAVE,         list_.Add( cs )
	#define EnrollL(_id,_nm)  cs.ID=_id,             \
		cs.longName=lnm[_nm], cs.shortName=snm[_nm], \
		cs.type=LOAD,         list_.Add( cs )

	// 適宜登録
	                               EnrollL( AutoDetect,0 );
	if( ::IsValidCodePage(932) )   Enroll(  SJIS,      1 ),
	                               Enroll(  EucJP,     2 ),
	                               Enroll(  IsoJP,     3 );
	/* if( always ) */             Enroll(  UTF5,      4 );
	                               Enroll(  UTF7,      5 );
	                               Enroll(  UTF8,      6 );
	                               EnrollS( UTF8N,     7 );
	                               EnrollS( UTF16b,    8 );
	                               EnrollS( UTF16l,    9 );
	                               Enroll(  UTF16BE,  10 );
	                               Enroll(  UTF16LE,  11 );
	                               EnrollS( UTF32b,   12 );
	                               EnrollS( UTF32l,   13 );
	                               Enroll(  UTF32BE,  14 );
	                               Enroll(  UTF32LE,  15 );
	                               Enroll(  Western,  16 );
	if( ::IsValidCodePage(28592) ) Enroll(  Central,  17 );
	if( ::IsValidCodePage(949) )   Enroll(  UHC,      18 ),
	                               Enroll(  IsoKR,    19 );
	if( ::IsValidCodePage(1361) )  Enroll(  Johab,    20 );
	if( ::IsValidCodePage(936) )   Enroll(  GBK,      21 ),
	                               Enroll(  IsoCN,    22 ),
	                               Enroll(  HZ   ,    23 );
	if( ::IsValidCodePage(950) )   Enroll(  Big5 ,    24 );
	if( ::IsValidCodePage(28595) ) Enroll(  Cyrillic, 25 );
	if( ::IsValidCodePage(20866) ) Enroll(  Koi8R,    26 );
	if( ::IsValidCodePage(21866) ) Enroll(  Koi8U,    27 );
	if( ::IsValidCodePage(874) )   Enroll(  Thai,     28 );
	if( ::IsValidCodePage(1254) )  Enroll(  Turkish,  29 );
	if( ::IsValidCodePage(1257) )  Enroll(  Baltic,   30 );
	if( ::IsValidCodePage(1258) )  Enroll( Vietnamese,31 );
	if( ::IsValidCodePage(28597) ) Enroll(  Greek,    32 );
	                               Enroll(  DOSUS,    33 );

	// 終了
	#undef Enroll
	#undef EnrollS
	#undef EnrollL
}

int CharSetList::defaultCs() const
{
	return ::GetACP();
/*
	switch( ::GetACP() )
	{
	case 932: return SJIS;
	case 936: return GBK;
	case 949: return UHC;
	case 950: return Big5;
	default:  return Western;
	}
*/
}

ulong CharSetList::defaultCsi() const
{
	return findCsi( defaultCs() );
}

ulong CharSetList::findCsi( int cs ) const
{
	for( ulong i=0,ie=list_.size(); i<ie; ++i )
		if( list_[i].ID == cs )
			return i;
	return 0xffffffff;
}



//------------------------------------------------------------------------
// 「開く」ダイアログ
//------------------------------------------------------------------------

namespace
{
	// 関数終了時に、カレントディレクトリを元に戻す
	class CurrentDirRecovery
	{
		Path cur_;
	public:
		CurrentDirRecovery() : cur_(Path::Cur) {}
		~CurrentDirRecovery() { ::SetCurrentDirectory(cur_.c_str()); }
	};
}

OpenFileDlg* OpenFileDlg::pThis;

bool OpenFileDlg::DoModal( HWND wnd, const TCHAR* fltr, const TCHAR* fnm )
{
	CurrentDirRecovery cdr;

	if( fnm == NULL )
		filename_[0] = TEXT('\0');
	else
		::lstrcpy( filename_, fnm );

	DWORD dwVersion, bNewShell;

	dwVersion = GetVersion();
	bNewShell = (LOBYTE(LOWORD(dwVersion))) > 3;

	OPENFILENAME ofn = {sizeof(ofn)};
	ofn.hwndOwner      = wnd;
	ofn.hInstance      = app().hinst();
	ofn.lpstrFilter    = fltr;
	ofn.lpstrFile      = filename_;
	ofn.nMaxFile       = countof(filename_);
	ofn.lpfnHook       = OfnHook;
	ofn.Flags = OFN_FILEMUSTEXIST |
				OFN_HIDEREADONLY  |
				OFN_ENABLEHOOK    |
				OFN_ENABLESIZING  |
				OFN_ENABLETEMPLATE;

	if (bNewShell)
	{
		// Include the OFN_EXPLORER flag to get the new look.
		ofn.Flags |= OFN_EXPLORER;
		// Use the new template sans the Open File controls.
		ofn.lpTemplateName = MAKEINTRESOURCE(IDD_OPENFILEHOOK);
	}
	else
	{
		// Running under Windows NT, use the old look template.
		ofn.lpTemplateName = (LPTSTR)MAKEINTRESOURCE(FILEOPENORD);
	}

	pThis = this;
	return ( ::GetOpenFileName(&ofn) != 0 );
}

UINT_PTR CALLBACK OpenFileDlg::OfnHook( HWND dlg, UINT msg, WPARAM, LPARAM lp )
{

	if( msg==WM_INITDIALOG )
	{
		// コンボボックスを埋めて、「自動選択」を選ぶ
		ComboBox cb( dlg, IDC_CODELIST );
		const CharSetList& csl = pThis->csl_;
		for( ulong i=0; i<csl.size(); ++i )
			if( csl[i].type & 2 ) // 2:=LOAD
				cb.Add( csl[i].longName );
		cb.Select( csl[0].longName );
	}
	else if( msg==WM_NOTIFY )
	{
		// OKが押されたら、文字コードの選択状況を記録
		if( reinterpret_cast<NMHDR*>(lp)->code==CDN_FILEOK )
		{
			ulong j=0, i=ComboBox(dlg,IDC_CODELIST).GetCurSel();
			for(;;++j,--i)
			{
				while( !(pThis->csl_[j].type & 2) ) // !LOAD
					++j;
				if( i==0 )
					break;
			}
			pThis->csIndex_ = j;
		}
	}
	return FALSE;
}



//------------------------------------------------------------------------
// 「保存」ダイアログ
//------------------------------------------------------------------------

SaveFileDlg* SaveFileDlg::pThis;

bool SaveFileDlg::DoModal( HWND wnd, const TCHAR* fltr, const TCHAR* fnm )
{
	CurrentDirRecovery cdr;

	if( fnm == NULL )
		filename_[0] = TEXT('\0');
	else
		::lstrcpy( filename_, fnm );

	DWORD dwVersion, bNewShell;

	dwVersion = GetVersion();
	bNewShell = (LOBYTE(LOWORD(dwVersion))) > 3;

	OPENFILENAME ofn = {sizeof(ofn)};
    ofn.hwndOwner      = wnd;
    ofn.hInstance      = app().hinst();
    ofn.lpstrFilter    = fltr;
    ofn.lpstrFile      = filename_;
    ofn.nMaxFile       = countof(filename_);
	ofn.lpfnHook       = OfnHook;
    ofn.Flags = OFN_HIDEREADONLY    |
				OFN_PATHMUSTEXIST   |
				OFN_ENABLESIZING    |
				OFN_ENABLEHOOK      |
				OFN_ENABLETEMPLATE  |
				OFN_OVERWRITEPROMPT;

	if (bNewShell)
	{
		// Include the OFN_EXPLORER flag to get the new look.
		ofn.Flags |= OFN_EXPLORER;
		// Use the new template sans the Open File controls.
		ofn.lpTemplateName = MAKEINTRESOURCE(IDD_SAVEFILEHOOK);
	}
	else
	{
		// Running under Windows NT, use the old look template.
		ofn.lpTemplateName = (LPTSTR)MAKEINTRESOURCE(FILEOPENORD);
	}

	pThis        = this;
	return ( ::GetSaveFileName(&ofn) != 0 );
}

UINT_PTR CALLBACK SaveFileDlg::OfnHook( HWND dlg, UINT msg, WPARAM, LPARAM lp )
{
	if( msg==WM_INITDIALOG )
	{
		// コンボボックスを埋めて、適切なのを選ぶ
		{
			ComboBox cb( dlg, IDC_CODELIST );
			const CharSetList& csl = pThis->csl_;

			for( ulong i=0; i<csl.size(); ++i )
				if( csl[i].type & 1 ) // 1:=SAVE
					cb.Add( csl[i].longName );
			cb.Select( csl[pThis->csIndex_].longName );
		}
		{
			ComboBox cb( dlg, IDC_CRLFLIST );
			static const TCHAR* const lbList[] = {
				TEXT("CR"),
				TEXT("LF"),
				TEXT("CRLF")
			};

			for( ulong i=0; i<countof(lbList); ++i )
				cb.Add( lbList[i] );
			cb.Select( lbList[pThis->lb_] );
		}
	}
	else if( msg==WM_NOTIFY )
	{
		if( reinterpret_cast<NMHDR*>(lp)->code==CDN_FILEOK )
		{
			// OKが押されたら、文字コードの選択状況を記録
			ulong j=0, i=ComboBox(dlg,IDC_CODELIST).GetCurSel();
			for(;;++j,--i)
			{
				while( !(pThis->csl_[j].type & 1) ) // !SAVE
					++j;
				if( i==0 )
					break;
			}
			pThis->csIndex_ = j;
			// 改行コードも
			pThis->lb_ = ComboBox(dlg,IDC_CRLFLIST).GetCurSel();
		}
	}
	return FALSE;
}



//------------------------------------------------------------------------
// ユーティリティー
//------------------------------------------------------------------------

ki::aarr<TCHAR> OpenFileDlg::ConnectWithNull( String lst[], int num )
{
	int TtlLen = 1;
	for( int i=0; i<num; ++i )
		TtlLen += (lst[i].len() + 1);

	aarr<TCHAR> a( new TCHAR[TtlLen] );

	TCHAR* p = a.get();
	for( int i=0; i<num; ++i )
	{
		::lstrcpy( p, lst[i].c_str() );
		p += (lst[i].len() + 1);
	}
	*p = TEXT('\0');

	return a;
}




//------------------------------------------------------------------------
// 「開き直す」ダイアログ
//------------------------------------------------------------------------

ReopenDlg::ReopenDlg( const CharSetList& csl, int csi )
	: DlgImpl(IDD_REOPENDLG), csl_(csl), csIndex_(csi)
{
}

void ReopenDlg::on_init()
{
	// コンボボックスを埋めて、「自動選択」を選ぶ
	ComboBox cb( hwnd(), IDC_CODELIST );
	for( ulong i=0; i<csl_.size(); ++i )
		if( csl_[i].type & 1 ) // 2:=SAVE
			cb.Add( csl_[i].longName );
	cb.Select( csl_[csIndex_].longName );
}

bool ReopenDlg::on_ok()
{
	// OKが押されたら、文字コードの選択状況を記録
	ulong j=0, i=ComboBox(hwnd(),IDC_CODELIST).GetCurSel();
	for(;;++j,--i)
	{
		while( !(csl_[j].type & 1) ) // !SAVE
			++j;
		if( i==0 )
			break;
	}
	csIndex_ = j;
	return true;
}
