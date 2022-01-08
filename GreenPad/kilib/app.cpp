#include "stdafx.h"
#include "app.h"
#include "log.h"
#include "memory.h"
#include "thread.h"
#include "window.h"
#include "string.h"
using namespace ki;



//=========================================================================

App* App::pUniqueInstance_;

inline App::App()
	: exitcode_    (-1)
	, loadedModule_(0)
	, isOldCommCtrl_(true)
	, isNewShell_(false)
	, hInst_       (::GetModuleHandle(NULL))
{
	// 唯一のインスタンスは私です。
	pUniqueInstance_ = this;

	// lets check NewShell here
	HINSTANCE hinstDll;
	hinstDll = LoadLibrary(TEXT("shell32.dll"));

	if(hinstDll) {
		isNewShell_ = GetProcAddress(hinstDll, "SHGetSpecialFolderLocation") != NULL;
		FreeLibrary(hinstDll);
	}
}

#pragma warning( disable : 4722 ) // 警告：デストラクタに値が戻りません
App::~App()
{
	// ロード済みモジュールがあれば閉じておく
#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
	if( loadedModule_ & COM )
		::CoUninitialize();
	if( loadedModule_ & OLE )
		::OleUninitialize();
#endif

	// 終～了～
	::ExitProcess( exitcode_ );
}

inline void App::SetExitCode( int code )
{
	// 終了コードを設定
	exitcode_ = code;
}

void App::InitModule( imflag what )
{
	// 初期化済みでなければ初期化する
	if( !(loadedModule_ & what) )
		switch( what )
		{
		case CTL: {
			HINSTANCE hinstDll;
			hinstDll = LoadLibrary(TEXT("comctl32.dll"));

			::InitCommonControls();

			if(hinstDll) {
				isOldCommCtrl_ = GetProcAddress(hinstDll, "DllGetVersion") == NULL;
				FreeLibrary(hinstDll);
			}

			break;
		}
#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
		case COM: ::CoInitialize( NULL ); break;
		case OLE: ::OleInitialize( NULL );break;
#endif
		}

	// 今回初期化したモノを記憶
	loadedModule_ |= what;
}

void App::Exit( int code )
{
	// 終了コードを設定して
	SetExitCode( code );

	// 自殺
	this->~App();
}



//-------------------------------------------------------------------------

const OSVERSIONINFOA& App::osver()
{
	typedef BOOL (WINAPI *PGVEXA)(OSVERSIONINFOA*);

	static OSVERSIONINFOA s_osVer;
	PGVEXA pGVEXA;

	if( s_osVer.dwOSVersionInfoSize == 0 )
	{
		// 初回だけは情報取得
		s_osVer.dwOSVersionInfoSize = sizeof( s_osVer );

		pGVEXA = (PGVEXA) ::GetProcAddress(::GetModuleHandle(TEXT("kernel32.dll")), "GetVersionExA");
		if(pGVEXA)
			pGVEXA(&s_osVer);
/*#if !defined(TARGET_VER) || (defined(TARGET_VER) && TARGET_VER>310)
		::GetVersionEx( &s_osVer );
#else*/
		else {
			/*TCHAR tmp[128];
			::wsprintf(tmp,TEXT("GetVersionExA not found. GetVersion returns 0x%08x."), ::GetVersion());
			::MessageBox( NULL, tmp, TEXT("GreenPad"), MB_OK|MB_TASKMODAL );*/

			DWORD dwVersion = ::GetVersion();

			s_osVer.dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
			s_osVer.dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

			if (dwVersion < 0x80000000) {
				s_osVer.dwBuildNumber = (DWORD)(HIWORD(dwVersion));
				s_osVer.dwPlatformId = VER_PLATFORM_WIN32_NT;
			}

			if (s_osVer.dwPlatformId != VER_PLATFORM_WIN32_NT) {
				if (s_osVer.dwMajorVersion == 3) s_osVer.dwPlatformId = VER_PLATFORM_WIN32s;
				else s_osVer.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;

				//s_osVer.dwBuildNumber = (DWORD)(HIWORD(dwVersion & 0x7FFFFFFF)); // when dwPlatformId == VER_PLATFORM_WIN32_WINDOWS, HIWORD(dwVersion) is reserved
			}
		}
//#endif
	}
	return s_osVer;
}

int App::getOSVer()
{
	static const OSVERSIONINFOA& v = osver();
	return v.dwMajorVersion*100+v.dwMinorVersion;
}

int App::getOSBuild()
{
	static const OSVERSIONINFOA& v = osver();
	return v.dwBuildNumber;
}

bool App::isNewTypeWindows()
{
	static const OSVERSIONINFOA& v = osver();
	return (
		( v.dwPlatformId==VER_PLATFORM_WIN32_NT && v.dwMajorVersion>=5 )
	 || ( v.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS &&
	          v.dwMajorVersion*100+v.dwMinorVersion>=410 )
	);
}

bool App::isWin95()
{
	static const OSVERSIONINFOA& v = osver();
	return (
		v.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS &&
		v.dwMajorVersion==4 &&
		v.dwMinorVersion==0
	);
}

bool App::isNT()
{
	static const OSVERSIONINFOA& v = osver();
	return v.dwPlatformId==VER_PLATFORM_WIN32_NT;
}
bool App::isWin32s()
{
	static const OSVERSIONINFOA& v = osver();
	return v.dwPlatformId==VER_PLATFORM_WIN32s;
}

bool App::isWin3later()
{
	static const OSVERSIONINFOA& v = osver();
	return v.dwMajorVersion>3;
}

 bool App::isNewShell()
{
	return isNewShell_;
}

bool App::isOldCommCtrl()
{
	return isOldCommCtrl_;
}


//=========================================================================

extern int kmain();

namespace ki
{
	void APIENTRY Startup()
	{
		// Startup :
		// プログラム開始すると、真っ先にここに来ます。

		// C++のローカルオブジェクトの破棄順序の仕様に
		// 自信がないので(^^;、スコープを利用して順番を強制
		// たぶん宣言の逆順だとは思うんだけど…

		LOGGER( "StartUp" );
		App myApp;
		{
			LOGGER( "StartUp app ok" );
			ThreadManager myThr;
			{
				LOGGER( "StartUp thr ok" );
				MemoryManager myMem;
				{
					LOGGER( "StartUp mem ok" );
					IMEManager myIME;
					{
						LOGGER( "StartUp ime ok" );
						String::LibInit();
						{
							const int r = kmain();
							myApp.SetExitCode( r );
						}
					}
				}
			}
		}
	}
}

#ifdef SUPERTINY

	extern "C" int __cdecl _purecall(){return 0;}
	#ifdef _DEBUG
		int main(){return 0;}
	#endif
	#pragma comment(linker, "/entry:\"Startup\"")

#else

	// VS2005でビルドしてもWin95で動くようにするため
	#if _MSC_VER >= 1400
		extern "C" BOOL WINAPI _imp__IsDebuggerPresent() { return FALSE; }
	#endif

	int APIENTRY WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
	{
		ki::Startup();
		return 0;
	}

#endif
