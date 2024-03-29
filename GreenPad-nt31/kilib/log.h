#ifndef _KILIB_LOG_H_
#define _KILIB_LOG_H_
#include "types.h"
#ifndef __ccdoc__
namespace ki {
#endif



class String;

//=========================================================================
//@{ @pkg ki.Core //@}
//@{
//	ログ取り機能（デバッグ用）
//
//	アプリ起動/終了用処理を担当します。
//	旧kilibと違って、ユーザー側のアプリケーションクラスを
//	ここから派生させることは出来ません。ユーザーのコードは、
//	必ず kmain() というグローバル関数から実行開始されます。
//	このAppクラス自体は、主にHINSTANCEの管理を行うだけ。
//@}
//=========================================================================

class Logger
{
public:

	Logger() {}
	void WriteLine( const String& str );
	void WriteLine( const TCHAR* str );
	void WriteLine( const TCHAR* str, int siz );

private:

	NOCOPY(Logger);
};

#ifdef DO_LOGGING
	#define LOGGER(str) Logger().WriteLine(TEXT(str))
	#define LOGGERV(var) Logger().WriteLine(var)
#else
	#define LOGGER(x)
	#define LOGGERV(x)
#endif

//=========================================================================

}      // namespace ki
#endif // _KILIB_LOG_H_
