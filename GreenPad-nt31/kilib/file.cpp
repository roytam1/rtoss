#include "stdafx.h"
#include "file.h"
using namespace ki;



//=========================================================================

FileR::FileR()
	: handle_ ( INVALID_HANDLE_VALUE )
	, fmo_    ( NULL )
	, basePtr_( NULL )
{
}

FileR::~FileR()
{
	Close();
}

bool FileR::Open( const TCHAR* fname )
{
	Close();

	// ファイルを読みとり専用で開く
	handle_ = ::CreateFile(
		fname, GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL
	);
	if( handle_ == INVALID_HANDLE_VALUE )
		return false;

	// サイズを取得
	size_ = ::GetFileSize( handle_, NULL );

	if( size_==0 )
	{
		// 0バイトのファイルはマッピング出来ないので適当に回避
		basePtr_ = &size_;
	}
	else
	{
		// マッピングオブジェクトを作る
		fmo_ = ::CreateFileMapping(
			handle_, NULL, PAGE_READONLY, 0, 0, NULL );
		if( fmo_ == NULL )
		{
#if 1
			// We cannot use CreateFileMapping() on old Win32s beta
			// So we allocate a buffer for the whole file and use ReadFile().
			basePtr_ = new BYTE[size_];
			DWORD nBytesRead=0;
			BOOL ret = ReadFile( handle_, (void*)basePtr_, size_, &nBytesRead,  NULL);
			::CloseHandle( handle_ ); // We can already close the handle
			handle_ = INVALID_HANDLE_VALUE;
			size_ = nBytesRead; // Update size with what was actually read.
			return nBytesRead && ret;
#else
			::CloseHandle( handle_ );
			handle_ = INVALID_HANDLE_VALUE;
			return false;
#endif
		}

		// ビュー
		basePtr_ = ::MapViewOfFile( fmo_, FILE_MAP_READ, 0, 0, 0 );
		if( basePtr_ == NULL )
		{
			::CloseHandle( fmo_ );
			::CloseHandle( handle_ );
			handle_ = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	return true;
}

void FileR::Close()
{
	if( handle_ != INVALID_HANDLE_VALUE )
	{
		// ヘンテコマッピングをしてなければここで解放
		if( basePtr_ != &size_ )
			::UnmapViewOfFile( const_cast<void*>(basePtr_) );
		basePtr_ = NULL;

		if( fmo_ != NULL )
			::CloseHandle( fmo_ );
		fmo_ = NULL;

		::CloseHandle( handle_ );
		handle_ = INVALID_HANDLE_VALUE;
	}
#if 1
	else
	{
		// If basePtr_ is non-NULL it means we allocated file
		// Via ReadFile (Win32s beta), so we must free the memory.
		// File handle is already closed.
		if( basePtr_ != NULL && basePtr_ != &size_ )
			delete [] (void*)basePtr_;
	}
#endif
}



//=========================================================================

FileW::FileW()
	: BUFSIZE( 65536 )
	, handle_( INVALID_HANDLE_VALUE )
	, buf_   ( new uchar[BUFSIZE] )
{
}

FileW::~FileW()
{
	Close();
	delete [] buf_;
}

inline void FileW::Flush()
{
	DWORD dummy;
	::WriteFile( handle_, buf_, bPos_, &dummy, NULL );
	bPos_ = 0;
}

bool FileW::Open( const TCHAR* fname, bool creat )
{
	Close();

	// ファイルを書き込み専用で開く
	handle_ = ::CreateFile( fname,
		GENERIC_WRITE, FILE_SHARE_READ, NULL,
		creat ? CREATE_ALWAYS : OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( handle_ == INVALID_HANDLE_VALUE )
		return false;

	bPos_ = 0;
	return true;
}

void FileW::Close()
{
	if( handle_ != INVALID_HANDLE_VALUE )
	{
		Flush();
		::CloseHandle( handle_ );
		handle_ = INVALID_HANDLE_VALUE;
	}
}

void FileW::Write( const void* dat, ulong siz )
{
	const uchar* udat = static_cast<const uchar*>(dat);

	while( (BUFSIZE-bPos_) <= siz )
	{
		memmove( buf_+bPos_, udat, BUFSIZE-bPos_ );
		siz  -= (BUFSIZE-bPos_);
		udat += (BUFSIZE-bPos_);
		bPos_ = BUFSIZE;
		Flush();
	}

	memmove( buf_+bPos_, udat, siz );
	bPos_ += siz;
}

void FileW::WriteC( uchar ch )
{
	if( (BUFSIZE-bPos_) <= 1 )
		Flush();

	buf_[bPos_++] = ch;
}

