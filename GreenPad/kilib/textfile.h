#ifndef _KILIB_TEXTFILE_H_
#define _KILIB_TEXTFILE_H_
#include "types.h"
#include "ktlaptr.h"
#include "memory.h"
#include "file.h"
#ifndef __ccdoc__
namespace ki {
#endif



//=========================================================================
//@{ @pkg ki.StdLib //@}
//@{
//	利用可能コードセット
//
//	ただし、ここでリストアップされたもののうち、Windowsにちゃんと
//	言語サポートがインストールされているものだけが実際には対応可能。
//	値が-100より小さいコードは、そのすぐ上にあるコードページの言語
//	サポートを利用して変換を行うため、それに依存する。
//@}
//=========================================================================

enum charset {
	AutoDetect = 0,    // 自動判定
	                   // SJIS/EucJP/IsoJP/IsoKR/IsoCN
					   // UTF5/UTF8/UTF8N/UTF16b/UTF16l/UTF32b/UTF32l
					   // を判定する。他は知らない。(^^;

	Western    = 1252, // 欧米      (Windows1252 >> ISO-8859-1)
	Turkish    = 1254, // トルコ語  (Windows1254 >> ISO-8859-9)
	Hebrew     = 1255, // ヘブライ語(Windows1255 >> ISO-8859-8)
	Arabic     = 1256, // アラビア語(Windows1256 ～ ISO-8859-6)
	Baltic     = 1257, // バルト語  (Windows1257 >> ISO-8859-13)
	Vietnamese = 1258, // ベトナム語(Windows1258 != VISCII)
	Central    = 1250, // 中央ﾖｰﾛｯﾊﾟ(Windows1250 ～ ISO-8859-2)
	Greek      = 1253, // ギリシャ語(Windows1253 ～ ISO-8859-7)
	Thai       = 874,  // タイ語

	Cyrillic   = 1251, // キリル語(Windows1251 != ISO-8859-5)
	Koi8R      = 20866,// キリル語(KOI8-R)
	Koi8U      = 21866,// キリル語(KOI8-U ウクライナ系)

	UHC        = 949,  // 韓国語１ (Unified Hangle Code >> EUC-KR)
	IsoKR      = -950, // 韓国語２ (ISO-2022-KR)
	Johab      = 1361, // 韓国語３ (Johab)

	GBK        = 936,  // 中国語１ (簡体字 GBK >> EUC-CN)
	IsoCN      = -936, // 中国語２ (簡体字 ISO-2022-CN)
	HZ         = -937, // 中国語３ (簡体字 HZ-GB2312)
	Big5       = 950,  // 中国語４ (繁体字 Big5)

	SJIS       = 932,  // 日本語１ (Shift_JIS)
	EucJP      = -932, // 日本語２ (日本語EUC)
	IsoJP      = -933, // 日本語３ (ISO-2022-JP)

	UTF5       = -2,   // Unicode  (UTF-5)   : BOM無し
	UTF7       = 65000,// Unicode  (UTF-7)   : BOM無し
	UTF8       =-65001,// Unicode  (UTF-8)   : BOM有り
	UTF8N      = 65001,// Unicode  (UTF-8N)  : BOM無し
	UTF16b     = -3,   // Unicode  (UTF-16)  : BOM有り BE
	UTF16l     = -4,   // Unicode  (UTF-16)  : BOM有り LE
	UTF16BE    = -5,   // Unicode  (UTF-16BE): BOM無し
	UTF16LE    = -6,   // Unicode  (UTF-16LE): BOM無し
	UTF32b     = -7,   // Unicode  (UTF-32)  : BOM有り BE
	UTF32l     = -8,   // Unicode  (UTF-32)  : BOM有り LE
	UTF32BE    = -9,   // Unicode  (UTF-32BE): BOM無し
	UTF32LE    = -10,  // Unicode  (UTF-32LE): BOM無し

	DOSUS      = 437   // DOSLatinUS (CP437)
};

//=========================================================================
//@{
//	改行コード
//@}
//=========================================================================

enum lbcode {
	CR   = 0,
	LF   = 1,
	CRLF = 2
};

struct TextFileRPimpl;
struct TextFileWPimpl;



//=========================================================================
//@{
//	テキストファイル読込
//
//	ファイルを指定された文字コードで解釈し、Unicode文字列として
//	一行毎に返す。文字コードや改行コードの自動判定も可能。
//@}
//=========================================================================

class TextFileR : public Object
{
public:

	//@{ コンストラクタ（コード指定）//@}
	TextFileR( int charset=AutoDetect );

	//@{ デストラクタ //@}
	~TextFileR();

	//@{ 開く //@}
	bool Open( const TCHAR* fname );

	//@{ 閉じる //@}
	void Close();

	//@{
	//	読み込み (読んだ長さを返す)
	//
	//	少なくとも20くらいのサイズを確保したバッファを指定してください。
	//@}
	size_t ReadLine( unicode* buf, ulong siz );

public:

	//@{ 読んでるファイルのコードページ //@}
	int codepage() const;

	//@{ 改行コード (0:CR, 1:LF, 2:CRLF) //@}
	int linebreak() const;

	//@{ 読み込み状況 (0:EOF, 1:EOL, 2:EOB) //@}
	int state() const;

	//@{ ファイルサイズ //@}
	ulong size() const;

	//@{ 改行が一個も見つからなかったフラグ //@}
	bool nolb_found() const;
private:

	dptr<TextFileRPimpl> impl_;
	FileR                fp_;
	int                  cs_;
	int                  lb_;
	bool          nolbFound_;

private:

	int AutoDetection( int cs, const uchar* ptr, ulong siz );

private:

	NOCOPY(TextFileR);
};



//-------------------------------------------------------------------------

inline int TextFileR::codepage() const
	{ return cs_; }

inline int TextFileR::linebreak() const
	{ return lb_; }

inline ulong TextFileR::size() const
	{ return fp_.size(); }

inline bool TextFileR::nolb_found() const
	{ return nolbFound_; }


//=========================================================================
//@{
//	テキストファイル書込
//
//	Unicode文字列を受け取り、指定された文字コードに変換しながら出力する。
//@}
//=========================================================================

class TextFileW : public Object
{
public:

	//@{ コンストラクタ（文字,改行コード指定）//@}
	TextFileW( int charset, int linebreak );
	~TextFileW();

	//@{ 開く //@}
	bool Open( const TCHAR* fname );

	//@{ 閉じる //@}
	void Close();

	//@{ 一行書き出し //@}
	void WriteLine( const unicode* buf, ulong siz, bool lastline );

private:

	dptr<TextFileWPimpl> impl_;
	FileW                fp_;
	const int            cs_;
	const int            lb_;

private:

	NOCOPY(TextFileW);
};



//=========================================================================

}      // namespace ki
#endif // _KILIB_TEXTFILE_H_
