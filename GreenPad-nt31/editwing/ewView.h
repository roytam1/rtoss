#ifndef _EDITWING_VIEW_H_
#define _EDITWING_VIEW_H_
#include "ewCommon.h"
#include "ewDoc.h"
#ifndef __ccdoc__
namespace editwing {
namespace view {
#endif

using namespace ki;

class Canvas;
class ViewImpl;
class Cursor;
class Caret;
class OleDnDTarget;


//=========================================================================
//@{ @pkg editwing.View //@}
//@{
//	描画処理など
//
//	このクラスでは、メッセージの分配を行うだけで、実装は
//	Canvas/ViewImpl 等で行う。ので、詳しくはそちらを参照のこと。
//@}
//=========================================================================

class View : public ki::WndImpl, public doc::DocEvHandler
{
public:

	//@{ 何もしないコンストラクタ //@}
	View( doc::Document& d, HWND wnd );
	~View();

	//@{ 折り返し方式切替 //@}
	void SetWrapType( int wt );

	//@{ 行番号表示/非表示切替 //@}
	void ShowLineNo( bool show );

	//@{ 表示色・フォント切替 //@}
	void SetFont( const VConfig& vc );

	//@{ 内部実装 //@}
	ViewImpl& impl() { return *impl_; }

	//@{ カーソル //@}
	Cursor& cur();

private:

	doc::DocImpl&      doc_;
	ki::dptr<ViewImpl> impl_;
	static ClsName     className_;

private:

	void    on_create( CREATESTRUCT* cs );
	void    on_destroy();
	LRESULT on_message( UINT msg, WPARAM wp, LPARAM lp );
	void    on_text_update( const DPos& s, const DPos& e, const DPos& e2, bool bAft, bool mCur );
	void    on_keyword_change();
};



//=========================================================================
//@{
//	イベントハンドラインターフェイス
//
//	カーソルから発生するイベント色々を
//@}
//=========================================================================

class CurEvHandler
{
	friend class Cursor;
	virtual void on_move( const DPos& c, const DPos& s ) {}
	virtual void on_char( Cursor& cur, unicode wch );
	virtual void on_key( Cursor& cur, int vk, bool sft, bool ctl );
	virtual void on_ime( Cursor& cur, unicode* str, ulong len );
};

//=========================================================================
//@{
// OLE Drag and Drop handler.
//@}
//=========================================================================
#ifndef NO_OLEDNDTAR
class OleDnDTarget : public IDropTarget
{
	friend class Cursor;
	OleDnDTarget( HWND hwnd, ViewImpl& vw );
	~OleDnDTarget();

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

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

	HRESULT STDMETHODCALLTYPE DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		// Only accept DragEnter if we can get some text.
		comes_from_center_ = false;
		FORMATETC fmt = { CF_UNICODETEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		if( S_OK == pDataObj->QueryGetData(&fmt) )
		{
			LOGGER( "OleDnDTarget::DragEnter(CF_UNICODETEXT)" );
			return S_OK;
		}

		fmt.cfFormat = CF_TEXT;
		if( S_OK == pDataObj->QueryGetData(&fmt) )
		{
			LOGGER( "OleDnDTarget::DragEnter(CF_TEXT)" );
			return S_OK;
		}

		fmt.cfFormat = CF_HDROP;
		if( app().isWin32s() && S_OK == pDataObj->QueryGetData(&fmt) )
		{
			LOGGER( "OleDnDTarget::DragEnter(CF_HDROP)" );
			return S_OK;
		}

		LOGGER( "OleDnDTarget::DragEnter(No supported IDataObject format)" );
		return E_UNEXPECTED;
	}

	HRESULT STDMETHODCALLTYPE DragLeave()
		{ LOGGER( "OleDnDTarget::DragLeave()" ); return S_OK; }

	HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

	HRESULT STDMETHODCALLTYPE Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

private:
	void setDropEffect(DWORD grfKeyState, DWORD *pdwEffect) const;

private:
	LONG refcnt;
	HWND hwnd_;
	ViewImpl& view_;

	bool comes_from_center_;
};
#endif // NO_OLEDNDTAR

//=========================================================================
//@{
//	表示位置情報まで含めたDPos
//@}
//=========================================================================

struct VPos : public DPos
{
	ulong vl; // VLine-Index
	ulong rl; // RLine-Index
	int   vx; // スクロールを考慮しない仮想スクリーン上のx座標(pixel)
	int   rx; // 文字の並びに左右されてないx座標(pixel)
		      //   == 長い行のしっぽから短い行に [↑] で移動して
		      //   == その後 [↓] で戻れるようなアレです。
	void operator=( const DPos& dp ) { tl=dp.tl, ad=dp.ad; }

	VPos(bool) : DPos(0,0),vl(0),rl(0),vx(0),rx(0) {}
	VPos() {}
};



//=========================================================================
//@{
//	カーソル
//@}
//=========================================================================

class Cursor : public ki::Object
{
public:

	// 初期化とか
	Cursor( HWND wnd, ViewImpl& vw, doc::DocImpl& dc );
	~Cursor();
	void AddHandler( CurEvHandler* ev );
	void DelHandler( CurEvHandler* ev );

	// カーソル移動
	void MoveCur( const DPos& dp, bool select );

	// キーによるカーソル移動
	void Left( bool wide, bool select );
	void Right( bool wide, bool select );
	void Up( bool wide, bool select );
	void Down( bool wide, bool select );
	void Home( bool wide, bool select );
	void End( bool wide, bool select );
	void PageUp( bool select );
	void PageDown( bool select );

	// テキスト書き換え
	void Input( const unicode* str, ulong len );
	void Input( const char* str, ulong len );
	void InputAt( const unicode *str, ulong len, int x, int y );
	void InputAt( const char *str, ulong len, int x, int y );
	void InputChar( unicode ch );
	void Del();
	void DelBack();

	// クリップボード
	void Cut();
	void Copy();
	void Paste();

	// 選択テキスト取得
	ki::aarr<unicode> getSelectedStr() const;

	// モード切替
	void SetInsMode( bool bIns );
	void SetROMode( bool bRO );

	// IME
	void Reconv();
	void ToggleIME();

public:

	bool isInsMode() const;
	bool isROMode() const;
	bool isSelected() const;
	bool isInSelection(const VPos &vp) const;
	bool getCurPos( const VPos** start, const VPos** end ) const;
	void ResetPos();
	void on_scroll_begin();
	void on_scroll_end();
	void on_text_update( const DPos& s, const DPos& e, const DPos& e2, bool mCur );
	void on_setfocus();
	void on_killfocus();
	void on_keydown( int vk, LPARAM flag );
	void on_char( TCHAR ch );
	void on_ime_composition( LPARAM lp );
	void on_lbutton_down( short x, short y, bool shift );
	bool on_drag_start( short x, short y );
	void on_mouse_move( short x, short y, WPARAM fwKeys );
	void on_lbutton_up( short x, short y );
	void on_lbutton_dbl( short x, short y );
	bool on_contextmenu( short x, short y );
	void on_timer();
	int  on_ime_reconvertstring( RECONVERTSTRING* rs );
	bool on_ime_confirmreconvertstring( RECONVERTSTRING* rs );

private:

	doc::DocImpl&   doc_;
	ViewImpl&       view_;
	CurEvHandler*   pEvHan_;
	ki::dptr<Caret> caret_;
#ifndef NO_OLEDNDTAR
	OleDnDTarget    dndtg_;
#endif

	VPos cur_;  // カーソル位置
	VPos sel_;  // 選択時の軸足位置
	bool bIns_; // 挿入モード？
	bool bRO_;  // 読取専用？

	UINT_PTR timerID_;// マウスドラッグ制御用の
	int  keyRepTime_; // タイマー関係
	int  dragX_;      // 位置
	int  dragY_;      // 位置
	bool lineSelectMode_; // 行選択モード？

	CurEvHandler defaultHandler_;

private:

	void MoveByMouse( int x, int y );
	void MoveTo( const VPos& vp, bool sel );
	void Ud( int dy, bool select );
	void UpdateCaretPos();
	void Redraw( const VPos& s, const VPos& e );
};



//-------------------------------------------------------------------------

inline bool Cursor::isSelected() const
	{ return cur_!=sel_; }

inline bool Cursor::isInsMode() const
	{ return bIns_; }

inline bool Cursor::isROMode() const
	{ return bRO_; }

inline bool Cursor::isInSelection(const VPos &vp) const
	{ return Min(cur_, sel_) <= vp && vp < Max(cur_,sel_); }



//=========================================================================

}}    // namespace editwing::view
#endif // _EDITWING_VIEW_H_
