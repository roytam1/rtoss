#ifndef _KILIB_WINUTIL_H_
#define _KILIB_WINUTIL_H_
#include "types.h"
#include "memory.h"
#include "ktlaptr.h"

#if !defined(NO_OLEDNDTAR) || !defined(NO_OLEDNDSRC)
bool coolDragDetect( HWND hwnd, LPARAM pt, WORD btup, WORD removebutton );

const IID myIID_IUnknown = { 0x00000000, 0x0000, 0x0000, {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} };
const IID myIID_IDataObject = { 0x0000010e, 0x0000, 0x0000, {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} };
const IID myIID_IDropSource = { 0x00000121, 0x0000, 0x0000, {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} };
const IID myIID_IDropTarget = { 0x00000122, 0x0000, 0x0000, {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} };
const IID myIID_IEnumFORMATETC = { 0x00000103, 0x0000, 0x0000, {0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} };
#endif

#ifndef __ccdoc__
namespace ki {
#endif

//=========================================================================
//@{ @pkg ki.WinUtil //@}
//@{
//	クリップボード管理
//
//	OpenClipboard や CloseClipboard 辺りの呼び出しを適当に自動化します。
//@}
//=========================================================================

class Clipboard : public Object
{
public:

	//@{ 開く //@}
	Clipboard( HWND owner, bool read=true );

	//@{ 閉じる //@}
	~Clipboard();

	//@{ データ読み込み //@}
	HANDLE GetData( UINT uFormat ) const;

	//@{ 指定フォーマットのデータがクリップボード上にあるか？ //@}
	bool IsAvail( UINT uFormat ) const;

	//@{ 指定フォーマットのデータがクリップボード上にあるか？(複数) //@}
	bool IsAvail( UINT uFormats[], int num ) const;

	//@{ テキスト情報保持クラス //@}
	class Text {
		friend class Clipboard;

		mutable unicode*        str_;
		enum Tp { NEW, GALLOC } mem_;

		Text( unicode* s, Tp m ) : str_(s), mem_(m) {}
		void operator=( const Text& );

	public:
		Text( const Text& t )
			: str_(t.str_), mem_(t.mem_) { t.str_=NULL; }
		~Text()
			{
				if( str_ != NULL )
					if( mem_==NEW ) delete [] str_;
					else      GlobalUnlock( str_ );
			}
		const unicode* data() const { return str_; }
	};

	//@{ テキスト読み込み //@}
	Text GetUnicodeText() const;

	//@{ データ書き込み //@}
	bool SetData( UINT uFormat, HANDLE hData );

	//@{ 独自フォーマットの登録 //@}
	static UINT RegisterFormat( const TCHAR* name );

public:

	//@{ 正常に開かれているかチェック //@}
	bool isOpened() const;

private:

	bool opened_;

private:

	NOCOPY(Clipboard);
};



//-------------------------------------------------------------------------

inline bool Clipboard::isOpened() const
	{ return opened_; }

inline HANDLE Clipboard::GetData( UINT uFormat ) const
	{ return ::GetClipboardData( uFormat ); }

inline bool Clipboard::SetData( UINT uFormat, HANDLE hData )
	{ return NULL != ::SetClipboardData( uFormat, hData ); }

inline bool Clipboard::IsAvail( UINT uFormat ) const
	{ return false!=::IsClipboardFormatAvailable(uFormat); }

inline bool Clipboard::IsAvail( UINT uFormats[], int num ) const
	{ return -1!=::GetPriorityClipboardFormat(uFormats,num); }

inline UINT Clipboard::RegisterFormat( const TCHAR* name )
	{ return ::RegisterClipboardFormat(name); }


//=========================================================================
//@{
//	IDataObjectFile: Class for a minimalist Text drag and drop data object
//
//	OleDnDSourceFile(str, len) to do the drag and drop
//@}
//=========================================================================

#ifndef NO_OLEDNDSRC
static void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source)
{
	// copy the source FORMATETC into dest
	*dest = *source;

	// We never use the ptd field!
	dest->ptd = NULL; // In case.
}
HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC *pFormatEtc, IEnumFORMATETC **ppEnumFormatEtc);
// Class for EnumFORMATETC
class CEnumFormatEtc : public IEnumFORMATETC, public Object
{
private:
	LONG		m_lRefCount;		// Reference count for this COM interface
	ULONG		m_nIndex;			// current enumerator index
	ULONG		m_nNumFormats;		// number of FORMATETC members
	FORMATETC * m_pFormatEtc;		// array of FORMATETC objects

public:

	//
	// IUnknown members
	//
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		if( memEQ(&riid, &myIID_IUnknown, sizeof(riid) )
		||  memEQ(&riid, &myIID_IEnumFORMATETC, sizeof(riid) ) )
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	ULONG STDMETHODCALLTYPE AddRef()  { return ::InterlockedIncrement(&m_lRefCount); }
	ULONG STDMETHODCALLTYPE Release()
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&m_lRefCount);

		if(count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}

	//
	// IEnumFormatEtc members
	//
	HRESULT __stdcall  Next  (ULONG celt, FORMATETC * pFormatEtc, ULONG * pceltFetched)
	{
		ULONG copied  = 0;

		// validate arguments
		if(celt == 0 || pFormatEtc == 0)
			return E_INVALIDARG;

		// copy FORMATETC structures into caller's buffer
		while(m_nIndex < m_nNumFormats && copied < celt)
		{
			DeepCopyFormatEtc(&pFormatEtc[copied], &m_pFormatEtc[m_nIndex]);
			copied++;
			m_nIndex++;
		}

		// store result
		if(pceltFetched != 0)
			*pceltFetched = copied;

		// did we copy all that was requested?
		return (copied == celt) ? S_OK : S_FALSE;
	}

	HRESULT __stdcall  Skip  (ULONG celt)
	{
		m_nIndex += celt;
		return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
	}

	HRESULT __stdcall  Reset (void)
	{
		m_nIndex = 0;
		return S_OK;
	}

	HRESULT __stdcall  Clone (IEnumFORMATETC ** ppEnumFormatEtc)
	{
		HRESULT hResult;

		// make a duplicate enumerator
		hResult = CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc);

		if(hResult == S_OK)
		{
			// manually set the index state
			((CEnumFormatEtc *) *ppEnumFormatEtc)->m_nIndex = m_nIndex;
		}

		return hResult;
	}

	//
	// Construction / Destruction
	//
	CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats)
		: m_lRefCount   ( 1 )
		, m_nIndex      ( 0 )
		, m_nNumFormats ( nNumFormats )
	{
		m_pFormatEtc  = new FORMATETC[nNumFormats];

		// copy the FORMATETC structures
		for(int i = 0; i < nNumFormats; i++)
		{
			DeepCopyFormatEtc(&m_pFormatEtc[i], &pFormatEtc[i]);
		}
	}

	~CEnumFormatEtc()
	{
		if(m_pFormatEtc)
		{
			delete[] m_pFormatEtc;
		}
	}

};
static HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC *pFormatEtc, IEnumFORMATETC **ppEnumFormatEtc)
{
	if(nNumFormats == 0 || pFormatEtc == 0 || ppEnumFormatEtc == 0)
		return E_INVALIDARG;

	*ppEnumFormatEtc = new CEnumFormatEtc(pFormatEtc, nNumFormats);

	return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
}
// Class for a minimalist File drag and drop data object
class IDataObjectFile : public IDataObject, public Object
{
private:
	LONG refcnt;
	HWND hwnd_;
	const unicode *str_;
	const size_t len_;

    enum {
        DATA_HDROP,
        DATA_NUM,
        DATA_INVALID = -1,
    };
	FORMATETC m_rgfe[DATA_NUM];
public:
	IDataObjectFile(HWND hwnd, const unicode *str, size_t len)
		: refcnt( 1 )
		, str_  ( str )
		, len_  ( len )
		, hwnd_  ( hwnd )
		{
			SetFORMATETC(&m_rgfe[DATA_HDROP], CF_HDROP);
		}
		~IDataObjectFile() {}
private:
	void SetFORMATETC(FORMATETC* pfe, UINT cf, TYMED tymed = TYMED_HGLOBAL, LONG lindex = -1,
		DWORD dwAspect = DVASPECT_CONTENT, DVTARGETDEVICE* ptd = NULL) const
	{
	    pfe->cfFormat = (CLIPFORMAT)cf;
	    pfe->tymed = tymed;
	    pfe->lindex = lindex;
	    pfe->dwAspect = dwAspect;
	    pfe->ptd = ptd;
	}
public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		if( memEQ(&riid, &myIID_IUnknown, sizeof(riid) )
		||  memEQ(&riid, &myIID_IDataObject, sizeof(riid) ) )
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	ULONG STDMETHODCALLTYPE AddRef()  { return ::InterlockedIncrement(&refcnt); }
	ULONG STDMETHODCALLTYPE Release()
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&refcnt);

		if(count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}

	HRESULT STDMETHODCALLTYPE GetData(FORMATETC *fmt,STGMEDIUM *pm)
	{
		if( S_OK == QueryGetData(fmt) )
		{
			mem00( pm, sizeof(*pm) ); // In case...
			pm->hGlobal = GlobalAlloc( GMEM_MOVEABLE, sizeof(DROPFILES)+(len_+2)*sizeof(unicode)+2*sizeof(HWND) );
			if( !pm->hGlobal )
				return E_OUTOFMEMORY;
			// Copy the data into pm
			return GetDataHere(fmt, pm);
		}
		return DV_E_FORMATETC;
	}
	HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppefe)
	{
		if (dwDirection == DATADIR_GET) {
			return CreateEnumFormatEtc(countof(m_rgfe), m_rgfe, ppefe);
		}
		*ppefe = NULL;
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC *fmt, STGMEDIUM *pm)
	{
		// Data is already allocated by caller!
		DROPFILES *data;
		if( S_OK == QueryGetData(fmt) && pm->hGlobal != NULL && (data = (DROPFILES *)GlobalLock(pm->hGlobal)) != NULL )
		{
			// Check actual size of allocated mem in case.
			size_t gmemsz = GlobalSize( pm->hGlobal );

			data->pFiles=sizeof(DROPFILES);
			data->pt.x=0;
			data->pt.y=0;
			data->fNC=1;
			data->fWide=app().isNT(); // only use unicode on NT!

			// The string starts just at the end of the structure
			char *dest = (char *)( ((BYTE*)data) + data->pFiles );

			// Destination length in BYTES!
			size_t len = Min(len_*sizeof(unicode), gmemsz-sizeof(DROPFILES)-2*sizeof(unicode));
			size_t remaining_bytes;

			if( !data->fWide )
			{	// Convert to ANSI and copy to dest!
				len = ::WideCharToMultiByte( CP_ACP, 0, str_, len_, dest, NULL, NULL, NULL ); // get length
				::WideCharToMultiByte( CP_ACP, 0, str_, len_, dest, len, NULL, NULL );
			}
			else
			{
				memmove( dest, str_, len );
			}
			remaining_bytes = gmemsz-len-(data->pFiles);
			if(remaining_bytes > 0)
				mem00(dest+len, remaining_bytes); // clear remaining bytes

			// write GPMainWnd handle as signature (previous HWND as 0 is also part of signature)
			HWND* myhwnd = (HWND *)( ((BYTE*)data) + gmemsz - sizeof(HWND) );
			*myhwnd = hwnd_;

			GlobalUnlock(pm->hGlobal);
			pm->pUnkForRelease = NULL; // Caller must free!
			pm->tymed = TYMED_HGLOBAL;
			return S_OK;
		}
		return DV_E_FORMATETC;
	}

	HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC *fmt)
	{
		// { CF_(UNI)TEXT, NULL, DVASPECT_CONTENT, -1, |TYMED_HGLOBAL } Only!
		if( fmt->cfFormat == CF_HDROP )
			if( fmt->ptd == NULL
			&&  fmt->dwAspect == DVASPECT_CONTENT
		//	&&  fmt->lindex == -1 // Skip this one?
			&&  fmt->tymed & TYMED_HGLOBAL )
				return S_OK;

		// Invalid or unsupported format
		return DV_E_FORMATETC;
	}

	HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC *fmt, FORMATETC *fout)
	{
		const FORMATETC canon = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		*fout = canon;
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE SetData(FORMATETC *pFormatetc, STGMEDIUM *pmedium, BOOL fRelease)
		{ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC *pFormatetc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection)
		{ return E_NOTIMPL; }
	HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection)
		{ return OLE_E_ADVISENOTSUPPORTED; }
	HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA ** ppenumAdvise)
		{ return OLE_E_ADVISENOTSUPPORTED; }
};
class OleDnDSourceFile : public IDropSource
{
public:
	OleDnDSourceFile(HWND hwnd, const unicode *str, size_t len, DWORD adEffect = DROPEFFECT_MOVE|DROPEFFECT_COPY)
	: refcnt    ( 1 )
	, dwEffect_ ( 0 )
	{
		ki::app().InitModule( App::OLE );
		{
			// Create IData object from the string
			IDataObjectFile data(hwnd, str, len);
			LOGGER( "OleDnDSourceFile IDataObjectFile created" );
			// Do the drag and drop and set dwEffect_ accordingly.
			DWORD effect=0;
			HRESULT ret = DoDragDrop( &data, this, adEffect, &effect );
			// Only set the resulting effect if the drop was actually performed
			if( ret == DRAGDROP_S_DROP )
				dwEffect_ = effect;
			#ifdef DO_LOGGING
			DWORD err = ::GetLastError();
			TCHAR buf[256];
			::wsprintf( buf, TEXT("OleDnDSourceTxt DoDragDrop end, ret=%x, effect=%d, err=%u"), (DWORD)ret, (int)effect, err );
			LOGGERV( buf );
			#endif
		}
	}

	DWORD getEffect() const
		{ return dwEffect_; }

	~OleDnDSourceFile(){}
private:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		IUnknown *punk = NULL;
		if( memEQ(&riid, &myIID_IUnknown, sizeof(riid) )
		||  memEQ(&riid, &myIID_IDropSource, sizeof(riid) ) )
		{
			*ppvObject = this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef()  { return ::InterlockedIncrement(&refcnt); }
	ULONG STDMETHODCALLTYPE Release()
	{
		// decrement object reference count
		LONG count = InterlockedDecrement(&refcnt);

		if(count == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return count;
		}
	}

	HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
		{
			if( fEscapePressed )
				return DRAGDROP_S_CANCEL;
			if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON)))
				return DRAGDROP_S_DROP;			// Do the drop!
			return S_OK;
		}

	HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect)
		{ return DRAGDROP_S_USEDEFAULTCURSORS; }

private:
	LONG refcnt;
	DWORD dwEffect_;
};
#endif // NO_OLEDNDSRC

//=========================================================================
//@{
//	排他制御
//
//	名前付きMutexを扱います
//@}
//=========================================================================

class Mutex : public Object
{
public:
	Mutex( const TCHAR* name );
	~Mutex();
	bool isLocked() const;

private:
	const HANDLE mtx_;
	bool locked_;

private:
	NOCOPY(Mutex);
};



//-------------------------------------------------------------------------

inline Mutex::Mutex( const TCHAR* name )
	: mtx_( ::CreateMutex( NULL, TRUE, name ) )
	, locked_ (false)
 	{
		DWORD dwError = ::GetLastError();
		if( mtx_ )
		{
			// Wait for Mutex ownership, in case it was already created.
			if( dwError == ERROR_ALREADY_EXISTS ) {
				// Wait 10 second for ownership or fail.
				DWORD dwWatiStatus = ::WaitForSingleObject(mtx_, 1000);
				locked_ = (dwWatiStatus == WAIT_OBJECT_0 || dwWatiStatus == WAIT_ABANDONED);
			} else {
				locked_ = true; // The mutex is ours.
			}
		}
		else if ( dwError == ERROR_CALL_NOT_IMPLEMENTED )
		{	// In case Mutex are not implemented
			// This is required for Win32s 1.1.
			// On Win32s 1.15a CreateMutexA is smartly implemented as
			// mov eax, 1; retn Ch;
			locked_ = true;
		}
	}

inline bool Mutex::isLocked() const
	{ return locked_; }

inline Mutex::~Mutex()
	{
		if( mtx_ != NULL )
		{
			if( locked_ ) ::ReleaseMutex( mtx_ );
			::CloseHandle( mtx_ );
		}
	}



//=========================================================================

}      // namespace ki
#endif // _KILIB_WINUTIL_H_
