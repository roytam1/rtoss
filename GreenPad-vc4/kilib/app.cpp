#include "stdafx.h"
#include "app.h"
#include "log.h"
#include "memory.h"
#include "thread.h"
#include "window.h"
#include "string.h"
#include "path.h"
using namespace ki;


BOOL GetNtVersionNumbers(DWORD* dwMajorVer, DWORD* dwMinorVer,DWORD* dwBuildNumber)
{
	// call NTDLL.RtlGetNtVersionNumbers for real version numbers
	// it is firstly existed as `RtlGetNtVersionInfo` in XP Beta Build 2495 and quickly renamed to retail name in Build 2517
	BOOL bRet = FALSE;
	HMODULE hModNtdll = NULL;
	if (hModNtdll = LoadLibrary(TEXT("ntdll.dll")))
	{
		typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
		pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
		pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");

		if (pfRtlGetNtVersionNumbers)
		{
			pfRtlGetNtVersionNumbers(dwMajorVer, dwMinorVer, dwBuildNumber);
			*dwBuildNumber &= 0x0ffff;
			bRet = TRUE;
		}

		FreeLibrary(hModNtdll);
		hModNtdll = NULL;
	}
	return bRet;
}


//=========================================================================

App* App::pUniqueInstance_;

inline App::App()
	: exitcode_    (-1)
	, loadedModule_(0)
	, hasOldCommCtrl_(true)
	, triedLoadingCommCtrl_(false)
	, isNewShell_(false)
	, hInstComCtl_(NULL)
	, hInst_       (::GetModuleHandle(NULL))
{
	// 唯一のインスタンスは私です。
	pUniqueInstance_ = this;

	init_osver();

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

	// only free library when program quits
	if(hInstComCtl_) FreeLibrary(hInstComCtl_);

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
			if(hInstComCtl_) {
				void (WINAPI *dyn_InitCommonControls)(void) = ( void (WINAPI *)(void) )
					GetProcAddress( hInstComCtl_, "InitCommonControls" );
				if (dyn_InitCommonControls)
					dyn_InitCommonControls();

				hasOldCommCtrl_ = GetProcAddress(hInstComCtl_, "DllGetVersion") == NULL;
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

void App::init_osver()
{
	OSVERSIONINFOA osvi;

	// zero-filling structs for safety
	mem00(&mvi_, sizeof(MYVERINFO));
	mem00(&osvi, sizeof(OSVERSIONINFOA));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

	if(GetNtVersionNumbers(&osvi.dwMajorVersion, &osvi.dwMinorVersion,&osvi.dwBuildNumber)) {
		mvi_.MVI_MINOR = (BYTE)osvi.dwMinorVersion;
		mvi_.MVI_MAJOR = (BYTE)osvi.dwMajorVersion;
		mvi_.MVI_BUILD = (WORD)osvi.dwBuildNumber;
		mvi_.wPlatform = VER_PLATFORM_WIN32_NT;
		mvi_.wType = 2;
	} else {
		typedef BOOL (WINAPI *PGVEXA)(OSVERSIONINFOA*);
		PGVEXA pGVEXA;
		pGVEXA = (PGVEXA) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetVersionExA");
		if(pGVEXA) {
			pGVEXA(&osvi);

			mvi_.MVI_MINOR = (BYTE)osvi.dwMinorVersion;
			mvi_.MVI_MAJOR = (BYTE)osvi.dwMajorVersion;
			mvi_.MVI_BUILD = (WORD)osvi.dwBuildNumber;
			mvi_.wPlatform = (WORD)osvi.dwPlatformId;
			mvi_.wType = 1;
		} else {
			osvi.dwBuildNumber = GetVersion();

			mvi_.MVI_MAJOR = (LOBYTE(LOWORD(osvi.dwBuildNumber)));
			mvi_.MVI_MINOR = (HIBYTE(LOWORD(osvi.dwBuildNumber)));

			if (osvi.dwBuildNumber < 0x80000000) {
				mvi_.MVI_BUILD = (HIWORD(osvi.dwBuildNumber));
				mvi_.wPlatform = VER_PLATFORM_WIN32_NT;
			}

			if (mvi_.wPlatform != VER_PLATFORM_WIN32_NT) {
				if (mvi_.MVI_MAJOR == 3) mvi_.wPlatform = VER_PLATFORM_WIN32s;
				else mvi_.wPlatform = VER_PLATFORM_WIN32_WINDOWS;

				//mvi_.MVI_BUILD = (DWORD)(HIWORD(osvi.dwBuildNumber & 0x7FFFFFFF)); // when dwPlatformId == VER_PLATFORM_WIN32_WINDOWS, HIWORD(dwVersion) is reserved
			}

		}
	}
}

WORD App::getOSVer()
{
	return mvi_.MVI_VER;
}

WORD App::getOSBuild()
{
	return mvi_.MVI_BUILD;
}

bool App::isNewTypeWindows()
{
	return (
		( mvi_.wPlatform==VER_PLATFORM_WIN32_NT && mvi_.MVI_MAJOR >= 5 )
	 || ( mvi_.wPlatform==VER_PLATFORM_WIN32_WINDOWS && mvi_.MVI_BUILD >= MKVER(4, 10) )
	);
}

bool App::isBuildEqual(DWORD dwTarget)
{
	return mvi_.MVI_VBN == dwTarget;
}

bool App::isBuildGreater(DWORD dwTarget)
{
	return mvi_.MVI_VBN >= dwTarget;
}

bool App::isWin95()
{
	MYVERINFO_PV* pv = (MYVERINFO_PV*)&mvi_;
	return pv->MPV_PV == MKPV(VER_PLATFORM_WIN32_WINDOWS, 4, 0);
}

bool App::isNT()
{
	return mvi_.wPlatform==VER_PLATFORM_WIN32_NT;
}

bool App::isWin32s()
{
	return mvi_.wPlatform==VER_PLATFORM_WIN32s;
}

bool App::isWin3later()
{
	return mvi_.MVI_MAJOR>3;
}

 bool App::isNewShell()
{
	return isNewShell_;
}

void App::loadCommCtrl()
{
	if(!triedLoadingCommCtrl_) {
		if(App::checkDLLExist(TEXT("comctl32.dll")))
			hInstComCtl_ = LoadLibrary(TEXT("comctl32.dll"));
		triedLoadingCommCtrl_ = true;
	}
}

bool App::hasOldCommCtrl()
{
	app().loadCommCtrl();
	return hasOldCommCtrl_;
}

bool App::isCommCtrlAvailable()
{
	app().loadCommCtrl();
	return hInstComCtl_ != NULL;
}

const TCHAR* App::checkDLLExist(TCHAR* dllname)
{
	bool dllexist = false;

	// sys dir
	Path dll_in_dir = Path(Path::Sys) + String(dllname);
	dllexist = dll_in_dir.exist();
	if(dllexist) return dll_in_dir.c_str();

	if(app().isWin32s()) {
		// win32s dir
		dll_in_dir = Path(Path::Sys) + String(TEXT("win32s\\")) + String(dllname);
		dllexist = dll_in_dir.exist();
		if(dllexist) return dll_in_dir.c_str();
	}

	// exe dir
	dll_in_dir = Path(Path::Exe) + String(dllname);
	dllexist = dll_in_dir.exist();
	if(dllexist) return dll_in_dir.c_str();
	else return NULL;
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
