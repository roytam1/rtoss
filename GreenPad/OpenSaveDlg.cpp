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
		TEXT("韓国語(EUC-KR)"),
		TEXT("韓国語(ISO-2022-KR)"),
		TEXT("韓国語(Johab)"),
		TEXT("中国語(GB2312)"),
		TEXT("中国語(ISO-2022-CN)"),
		TEXT("中国語(HZ)"),
		TEXT("中国語(Big5)"),
		TEXT("UTF-1"),
		TEXT("UTF-5"),
		TEXT("UTF-7"),
		TEXT("UTF-8"),
		TEXT("UTF-8N"),
		TEXT("UTF-9(1997)"),
		TEXT("UTF-16BE(BOM)"),
		TEXT("UTF-16LE(BOM)"),
		TEXT("UTF-16BE"),
		TEXT("UTF-16LE"),
		TEXT("UTF-32BE(BOM)"),
		TEXT("UTF-32LE(BOM)"),
		TEXT("UTF-32BE"),
		TEXT("UTF-32LE"),
		TEXT("欧米(DOS)"),
		TEXT("欧米"),
		TEXT("中欧(DOS)"),
		TEXT("中欧"),
		TEXT("キリル語(IBM)"),
		TEXT("キリル語(MS-DOS)"),
		TEXT("キリル語(Windows)"),
		TEXT("キリル語(KOI8-R)"),
		TEXT("キリル語(KOI8-U)"),
		TEXT("タイ語"),
		TEXT("トルコ語(DOS)"),
		TEXT("トルコ語"),
		TEXT("バルト語(IBM)"),
		TEXT("バルト語"),
		TEXT("ベトナム語"),
		TEXT("ギリシャ語(IBM)"),
		TEXT("ギリシャ語(MS-DOS)"),
		TEXT("ギリシャ語"),
		TEXT("アラビア語(IBM)"),
		TEXT("アラビア語(MS-DOS)"),
		TEXT("アラビア語"),
		TEXT("ヘブライ語(DOS)"),
		TEXT("ヘブライ語"),
		TEXT("ポルトガル語(DOS)"),
		TEXT("アイスランド語(DOS)"),
		TEXT("フランス語(カナダ)(DOS)"),
		TEXT("MSDOS(北欧)"),
		TEXT("MSDOS(us)")
	};
	static const TCHAR* const lnmEn[] = {
		TEXT("AutoDetect"),
		TEXT("Japanese(ShiftJIS)"),
		TEXT("Japanese(EUC)"),
		TEXT("Japanese(ISO-2022-JP)"),
		TEXT("Korean(EUC-KR)"),
		TEXT("Korean(ISO-2022-KR)"),
		TEXT("Korean(Johab)"),
		TEXT("Chinese(GB2312)"),
		TEXT("Chinese(ISO-2022-CN)"),
		TEXT("Chinese(HZ)"),
		TEXT("Chinese(Big5)"),
		TEXT("UTF-1"),
		TEXT("UTF-5"),
		TEXT("UTF-7"),
		TEXT("UTF-8"),
		TEXT("UTF-8N"),
		TEXT("UTF-9(1997)"),
		TEXT("UTF-16BE(BOM)"),
		TEXT("UTF-16LE(BOM)"),
		TEXT("UTF-16BE"),
		TEXT("UTF-16LE"),
		TEXT("UTF-32BE(BOM)"),
		TEXT("UTF-32LE(BOM)"),
		TEXT("UTF-32BE"),
		TEXT("UTF-32LE"),
		TEXT("Latin-1(DOS)"),
		TEXT("Latin-1"),
		TEXT("Latin-2(DOS)"),
		TEXT("Latin-2"),
		TEXT("Cyrillic(IBM)"),
		TEXT("Cyrillic(MS-DOS)"),
		TEXT("Cyrillic(Windows)"),
		TEXT("Cyrillic(KOI8-R)"),
		TEXT("Cyrillic(KOI8-U)"),
		TEXT("Thai"),
		TEXT("Turkish(DOS)"),
		TEXT("Turkish"),
		TEXT("Baltic(IBM)"),
		TEXT("Baltic"),
		TEXT("Vietnamese"),
		TEXT("Greek(IBM)"),
		TEXT("Greek(MS-DOS)"),
		TEXT("Greek"),
		TEXT("Arabic(IBM)"),
		TEXT("Arabic(MS-DOS)"),
		TEXT("Arabic"),
		TEXT("Hebrew(DOS)"),
		TEXT("Hebrew"),
		TEXT("Portuguese(DOS)"),
		TEXT("Icelandic(DOS)"),
		TEXT("Canadian French(DOS)"),
		TEXT("MSDOS(Nodic)"),
		TEXT("MSDOS(us)")
	};
	static const TCHAR* const snm[] = {
		TEXT(""),
		TEXT("SJIS"),
		TEXT("EUC"),
		TEXT("JIS"),
		TEXT("UHC"),
		TEXT("I2KR"),
		TEXT("Jhb"),
		TEXT("GBK"),
		TEXT("I2CN"),
		TEXT("HZ"),
		TEXT("BIG5"),
		TEXT("UTF1"),
		TEXT("UTF5"),
		TEXT("UTF7"),
		TEXT("UTF8"),
		TEXT("UTF8"),
		TEXT("UTF9"),
		TEXT("U16B"),
		TEXT("U16L"),
		TEXT("U16B"),
		TEXT("U16L"),
		TEXT("U32B"),
		TEXT("U32L"),
		TEXT("U32B"),
		TEXT("U32L"),
		TEXT("LN1D"),
		TEXT("LTN1"),
		TEXT("LN2D"),
		TEXT("LTN2"),
		TEXT("CYRI"),
		TEXT("CYRD"),
		TEXT("CYRL"),
		TEXT("KO8R"),
		TEXT("KO8U"),
		TEXT("THAI"),
		TEXT("TRKD"),
		TEXT("TRK"),
		TEXT("BALI"),
		TEXT("BALT"),
		TEXT("VTNM"),
		TEXT("GRKI"),
		TEXT("GRKD"),
		TEXT("GRK"),
		TEXT("ARAI"),
		TEXT("ARAD"),
		TEXT("ARA"),
		TEXT("HEBD"),
		TEXT("HEB"),
		TEXT("PRT"),
		TEXT("ICE"),
		TEXT("CFR"),
		TEXT("NOD"),
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
	                               EnrollL( AutoDetect,      0 );
	if( ::IsValidCodePage(932) )   Enroll(  SJIS,            1 ),
	                               Enroll(  EucJP,           2 ),
	                               Enroll(  IsoJP,           3 );
	if( ::IsValidCodePage(949) )   Enroll(  UHC,             4 ),
	                               Enroll(  IsoKR,           5 );
	if( ::IsValidCodePage(1361) )  Enroll(  Johab,           6 );
	if( ::IsValidCodePage(936) )   Enroll(  GBK,             7 ),
	                               Enroll(  IsoCN,           8 ),
	                               Enroll(  HZ   ,           9 );
	if( ::IsValidCodePage(950) )   Enroll(  Big5 ,          10 );
	/* if( always ) */             Enroll(  UTF1,           11 );
	                               Enroll(  UTF5,           12 );
	                               Enroll(  UTF7,           13 );
	                               Enroll(  UTF8,           14 );
	                               EnrollS( UTF8N,          15 );
	                               Enroll(  UTF9,           16 );
	                               EnrollS( UTF16b,         17 );
	                               EnrollS( UTF16l,         18 );
	                               Enroll(  UTF16BE,        19 );
	                               Enroll(  UTF16LE,        20 );
	                               EnrollS( UTF32b,         21 );
	                               EnrollS( UTF32l,         22 );
	                               Enroll(  UTF32BE,        23 );
	                               Enroll(  UTF32LE,        24 );
	if( ::IsValidCodePage(850) )   Enroll(  WesternDOS,     25 );
	                               Enroll(  Western,        26 );
	if( ::IsValidCodePage(852) )   Enroll(  CentralDOS,     27 );
	if( ::IsValidCodePage(28592) ) Enroll(  Central,        28 );
	if( ::IsValidCodePage(855) )   Enroll(  CyrillicIBM,    29 );
	if( ::IsValidCodePage(866) )   Enroll(  CyrillicDOS,    30 );
	if( ::IsValidCodePage(28595) ) Enroll(  Cyrillic,       31 );
	if( ::IsValidCodePage(20866) ) Enroll(  Koi8R,          32 );
	if( ::IsValidCodePage(21866) ) Enroll(  Koi8U,          33 );
	if( ::IsValidCodePage(874) )   Enroll(  Thai,           34 );
	if( ::IsValidCodePage(857) )   Enroll(  TurkishDOS,     35 );
	if( ::IsValidCodePage(1254) )  Enroll(  Turkish,        36 );
	if( ::IsValidCodePage(775) )   Enroll(  BalticIBM,      37 );
	if( ::IsValidCodePage(1257) )  Enroll(  Baltic,         38 );
	if( ::IsValidCodePage(1258) )  Enroll( Vietnamese,      39 );
	if( ::IsValidCodePage(737) )   Enroll(  GreekIBM,       40 );
	if( ::IsValidCodePage(869) )   Enroll(  GreekMSDOS,     41 );
	if( ::IsValidCodePage(28597) ) Enroll(  Greek,          42 );
	if( ::IsValidCodePage(720) )   Enroll(  ArabicIBM,      43 );
	if( ::IsValidCodePage(864) )   Enroll(  ArabicMSDOS,    44 );
	if( ::IsValidCodePage(1256) )  Enroll(  Arabic,         45 );
	if( ::IsValidCodePage(862) )   Enroll(  HebrewDOS,      46 );
	if( ::IsValidCodePage(1255) )  Enroll(  Hebrew,         47 );
	if( ::IsValidCodePage(860) )   Enroll(  Portuguese,     48 );
	if( ::IsValidCodePage(861) )   Enroll(  Icelandic,      49 );
	if( ::IsValidCodePage(863) )   Enroll(  CanadianFrench, 50 );
	if( ::IsValidCodePage(865) )   Enroll(  Nordic,         51 );
	                               Enroll(  DOSUS,          52 );

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
	TCHAR filepath_[MAX_PATH];

	if( fnm == NULL )
	{
		filename_[0] = TEXT('\0');
		filepath_[0] = TEXT('\0');
	}
	else
	{
		::lstrcpy(filepath_, fnm);
		::lstrcpy(filename_, fnm);
		
		int i = 0;
		int j = -1;
		
		while(filepath_[i] != TEXT('\0'))
		{
			if(filepath_[i] == TEXT('\\'))
			{
				j = i;
			}
			i++;
		}
		
		int x = 0;
		for (i = j+1; filepath_[i] != TEXT('\0'); i++)
		{
			filename_[x++] = filepath_[i];
		}
		filename_[x++] = TEXT('\0');
		filepath_[j+1] = TEXT('\0');
	}

	DWORD dwVersion, bNewShell;

	dwVersion = GetVersion();
	bNewShell = (LOBYTE(LOWORD(dwVersion))) > 3;

	OPENFILENAME ofn = {sizeof(ofn)};
	ofn.hwndOwner      = wnd;
	ofn.hInstance      = app().hinst();
	ofn.lpstrFilter    = fltr;
	ofn.lpstrFile      = filename_;
	ofn.nMaxFile       = countof(filename_);
    ofn.lpstrInitialDir= filepath_;
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

UINT_PTR CALLBACK OpenFileDlg::OfnHook( HWND dlg, UINT msg, WPARAM wp, LPARAM lp )
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
	else if( msg==WM_NOTIFY ||( msg==WM_COMMAND && LOWORD(wp)==1 ))
	{
		// OKが押されたら、文字コードの選択状況を記録
		if(( msg==WM_COMMAND && LOWORD(wp)==1 ) || ((LPOFNOTIFY)lp)->hdr.code==CDN_FILEOK )
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
	TCHAR filepath_[MAX_PATH];

	if( fnm == NULL )
	{
		filename_[0] = TEXT('\0');
		filepath_[0] = TEXT('\0');
	}
	else
	{
		::lstrcpy(filepath_, fnm);
		::lstrcpy(filename_, fnm);
		
		int i = 0;
		int j = -1;
		
		while(filepath_[i] != TEXT('\0'))
		{
			if(filepath_[i] == TEXT('\\'))
			{
				j = i;
			}
			i++;
		}
		
		int x = 0;
		for (i = j+1; filepath_[i] != TEXT('\0'); i++)
		{
			filename_[x++] = filepath_[i];
		}
		filename_[x++] = TEXT('\0');
		filepath_[j+1] = TEXT('\0');
	}

	DWORD dwVersion, bNewShell;

	dwVersion = GetVersion();
	bNewShell = (LOBYTE(LOWORD(dwVersion))) > 3;

	OPENFILENAME ofn = {sizeof(ofn)};
    ofn.hwndOwner      = wnd;
    ofn.hInstance      = app().hinst();
    ofn.lpstrFilter    = fltr;
    ofn.lpstrFile      = filename_;
    ofn.nMaxFile       = countof(filename_);
    ofn.lpstrTitle     = TEXT("Save File As");
    ofn.lpstrInitialDir= filepath_;
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

UINT_PTR CALLBACK SaveFileDlg::OfnHook( HWND dlg, UINT msg, WPARAM wp, LPARAM lp )
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
	else if( msg==WM_NOTIFY ||( msg==WM_COMMAND && LOWORD(wp)==1 ))
	{
		if(( msg==WM_COMMAND && LOWORD(wp)==1 ) || ((LPOFNOTIFY)lp)->hdr.code==CDN_FILEOK )
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
