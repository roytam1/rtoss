/**
 * @file	WndBase.h
 * @brief	ウィンドウ基底クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief ウィンドウ基底クラス
 */
class CWndBase
{
public:
	HWND m_hWnd;            /*!< must be first data member */

	CWndBase(HWND hWnd = NULL);
	CWndBase& operator=(HWND hWnd);
	void Attach(HWND hWnd);
	HWND Detach();
//	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu);
//	BOOL DestroyWindow();

	// Attributes
	operator HWND() const;
	DWORD GetStyle() const;

	// Message Functions
	LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	// Window Text Functions
	BOOL SetWindowText(LPCTSTR lpString);
	int GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;
	int GetWindowTextLength() const;

	// Font Functions
	void SetFont(HFONT hFont, BOOL bRedraw = TRUE);

	// Menu Functions (non-child windows only)
	HMENU GetMenu() const;
	BOOL DrawMenuBar();
	HMENU GetSystemMenu(BOOL bRevert) const;

	// Window Size and Position Functions
	void MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE);

	// Window Size and Position Functions
	BOOL GetWindowRect(LPRECT lpRect) const;
	BOOL GetClientRect(LPRECT lpRect) const;

	// Update and Painting Functions
	HDC BeginPaint(LPPAINTSTRUCT lpPaint);
	void EndPaint(LPPAINTSTRUCT lpPaint);
	BOOL UpdateWindow();
	BOOL Invalidate(BOOL bErase = TRUE);
	BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);
	BOOL ShowWindow(int nCmdShow);

	// Window State Functions
	HWND SetFocus();

	// Misc. Operations
	int SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE);
	BOOL IsWindow() const;
};

/**
 * コンストラクタ
 * @param[in] hWnd ウィンドウ ハンドル
 */
inline CWndBase::CWndBase(HWND hWnd)
	: m_hWnd(hWnd)
{
}

/**
 * オペレータ
 * @param[in] hWnd ウィンドウ ハンドル
 * @return インスタンス
 */
inline CWndBase& CWndBase::operator=(HWND hWnd)
{
	m_hWnd = hWnd;
	return *this;
}

/**
 * アタッチ
 * @param[in] hWnd ウィンドウ ハンドル
 */
inline void CWndBase::Attach(HWND hWnd)
{
	m_hWnd = hWnd;
}

/**
 * デタッチ
 * @return 以前のインスタンス
 */
inline HWND CWndBase::Detach()
{
	HWND hWnd = m_hWnd;
	m_hWnd = NULL;
	return hWnd;
}

/**
 * HWND オペレータ
 * @return HWND
 */
inline CWndBase::operator HWND() const
{
	return m_hWnd;
}

/**
 * 現在のウィンドウ スタイルを返します
 * @return ウィンドウのスタイル
 */
inline DWORD CWndBase::GetStyle() const
{
	return static_cast<DWORD>(::GetWindowLong(m_hWnd, GWL_STYLE));
}

/**
 * このウィンドウに指定されたメッセージを送信します
 * @param[in] message 送信されるメッセージを指定します
 * @param[in] wParam 追加のメッセージ依存情報を指定します
 * @param[in] lParam 追加のメッセージ依存情報を指定します
 * @return メッセージの処理の結果
 */
inline LRESULT CWndBase::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::SendMessage(m_hWnd, message, wParam, lParam);
}

/**
 * メッセージをウィンドウのメッセージ キューに置き、対応するウィンドウがメッセージを処理するのを待たずに返されます
 * @param[in] message ポストするメッセージを指定します
 * @param[in] wParam メッセージの付加情報を指定します
 * @param[in] lParam メッセージの付加情報を指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::PostMessage(m_hWnd, message, wParam, lParam);
}

/**
 * 指定されたウィンドウのタイトルバーのテキストを変更します
 * @param[in] lpString 新しいウィンドウタイトルまたはコントロールのテキストとして使われる、NULL で終わる文字列へのポインタを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::SetWindowText(LPCTSTR lpString)
{
	return ::SetWindowText(m_hWnd, lpString);
}

/**
 * 指定されたウィンドウのタイトルバーのテキストをバッファへコピーします
 * @param[in] lpszStringBuf バッファへのポインタを指定します。このバッファにテキストが格納されます
 * @param[in] nMaxCount バッファにコピーする文字の最大数を指定します。テキストのこのサイズを超える部分は、切り捨てられます。NULL 文字も数に含められます
 * @return コピーされた文字列の文字数が返ります (終端の NULL 文字は含められません)
 */
inline int CWndBase::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	return ::GetWindowText(m_hWnd, lpszStringBuf, nMaxCount);
}

/**
 * 指定されたウィンドウのタイトルバーテキストの文字数を返します
 * @return 関数が成功すると、テキストの文字数が返ります
 */
inline int CWndBase::GetWindowTextLength() const
{
	return ::GetWindowTextLength(m_hWnd);
}

/**
 * 指定したフォントを使用します
 * @param[in] hFont フォント ハンドル
 * @param[in] bRedraw メッセージを処理した直後にウィンドウを再描画する場合は TRUE
 */
inline void CWndBase::SetFont(HFONT hFont, BOOL bRedraw)
{
	::SendMessage(m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), bRedraw);
}

/**
 * 指定されたウィンドウに割り当てられているメニューのハンドルを取得します
 * @return メニューのハンドルが返ります
 */
inline HMENU CWndBase::GetMenu() const
{
	return ::GetMenu(m_hWnd);
}

/**
 * 指定されたウィンドウのメニューバーを再描画します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::DrawMenuBar()
{
	return ::DrawMenuBar(m_hWnd);
}

/**
 * 指定されたウィンドウに割り当てられているシステム メニューのハンドルを取得します
 * @param[in] bRevert 実行されるアクションを指定します
 * @return メニューのハンドルが返ります
 */
inline HMENU CWndBase::GetSystemMenu(BOOL bRevert) const
{
	return ::GetSystemMenu(m_hWnd, bRevert);
}

/**
 * 位置とサイズを変更します
 * @param[in] x 左側の新しい位置を指定します
 * @param[in] y 上側の新しい位置を指定します
 * @param[in] nWidth 新しい幅を指定します
 * @param[in] nHeight 新しい高さを指定します
 * @param[in] bRepaint 再描画する必要があるかどうかを指定します
 * @
 */
inline void CWndBase::MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
	::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint);
}

/**
 * 指定されたウィンドウの左上端と右下端の座標をスクリーン座標で取得します
 * @param[out] lpRect 構造体へのポインタを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::GetWindowRect(LPRECT lpRect) const
{
	return ::GetWindowRect(m_hWnd, lpRect);
}

/**
 * lpRectが指す構造に CWnd のクライアント領域のクライアント座標をコピーします
 * @param[out] lpRect 構造体へのポインタを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::GetClientRect(LPRECT lpRect) const
{
	return ::GetClientRect(m_hWnd, lpRect);
}

/**
 * 描画を開始します
 * @param[out] lpPaint 描画情報へのポインタを指定します
 * @return デバイス コンテキスト
 */
inline HDC CWndBase::BeginPaint(LPPAINTSTRUCT lpPaint)
{
	return ::BeginPaint(m_hWnd, lpPaint);
}

/**
 * 描画の終了します
 * @param[in] lpPaint 描画情報へのポインタを指定します
 */
inline void CWndBase::EndPaint(LPPAINTSTRUCT lpPaint)
{
	::EndPaint(m_hWnd, lpPaint);
}

/**
 * 指定されたウィンドウの更新リージョンが空ではない場合、ウィンドウへ メッセージを送信し、そのウィンドウのクライアント領域を更新します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::UpdateWindow()
{
	return ::UpdateWindow(m_hWnd);
}

/**
 * 指定されたウィンドウのすべてを更新リージョンにします
 * @param[in] bErase 更新リージョンを処理するときに、更新リージョン内の背景を消去するかどうかを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::Invalidate(BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, NULL, bErase);
}

/**
 * 指定されたウィンドウの更新リージョンに1個の長方形を追加します
 * @param[in] lpRect 更新リージョンへ追加したい長方形のクライアント座標を保持する1個の構造体へのポインタを指定します
 * @param[in] bErase 更新リージョンを処理するときに、更新リージョン内の背景を消去するかどうかを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::InvalidateRect(LPCRECT lpRect, BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, lpRect, bErase);
}

/**
 * 指定されたウィンドウの表示状態を設定します
 * @param[in] nCmdShow ウィンドウの表示方法を指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::ShowWindow(int nCmdShow)
{
	return ::ShowWindow(m_hWnd, nCmdShow);
}

/**
 * 入力フォーカスを要求します
 * @return 直前に入力フォーカスを持っていたウィンドウ ハンドル
 */
inline HWND CWndBase::SetFocus()
{
	return ::SetFocus(m_hWnd);
}

/**
 * スクロールバーのさまざまなパラメータを設定します
 * @param[in] nBar パラメータを設定するべきスクロールバーのタイプを指定します
 * @param[in] lpScrollInfo 設定するべき情報を保持している、1個の構造体へのポインタを指定します
 * @param[in] bRedraw スクロールバーを再描画するかどうかを指定します
 * @return スクロールバーの現在のスクロール位置が返ります
 */
inline int CWndBase::SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
{
	return ::SetScrollInfo(m_hWnd, nBar, lpScrollInfo, bRedraw);
}

/**
 * ウィンドウが存在しているかどうかを調べます
 * @return 指定したウィンドウハンドルを持つウィンドウが存在している場合は、0 以外の値が返ります
 */
inline BOOL CWndBase::IsWindow() const
{
	return ::IsWindow(m_hWnd);
}
