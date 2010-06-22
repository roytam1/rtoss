/*

  http://www.codeguru.com/ieprogram/cwebpage.html

  Display a Web Page in a Plain C Win32 Application
--------------------------------------------------------------------------------
  This article was contributed by Jeff Glatt.

*/

/*
 * Dll.c -- A dynamic link library to display a web page in your own window.
 *
 * This is very loosely based upon a C++ example written by Chris Becke. I used
 * that to learn the minimum of what I needed to know about hosting the browser
 * object. Then I wrote this example from the ground up in C.
 *
 * The functions in this DLL callable by a program:
 *
 * EmbedBrowserObject() -- Embeds a browser object in your own window.
 * UnEmbedBrowserObject() -- Detaches the browser object from your window.
 * DisplayHTMLPage() -- Displays a URL or HTML file on disk.
 * DisplayHTMLStr() -- Displays a (in memory) string of HTML code.
 *
 * For the release (ie, not debug) version, then you should set your linker to
 * ignore the default libraries. This will reduce code size.
 */
#pragma warning(push)
#pragma warning(disable: 4201)	// 非標準の拡張機能が使用されています : 無名の構造体または共用体です。
#pragma warning(disable: 4057)	// 'x' で使われているポインタ式は 'y' と基本型が異なっています。
#pragma warning(disable: 4028)	// パラメータの型 n が関数宣言と一致しません。
#pragma warning(disable: 4100)	// 引数は関数の本体部で 1 度も参照されません。

#include <windows.h>
#include <exdisp.h>		/* Defines of stuff like IWebBrowser2. This is an include file with Visual C 6 and above */
#include <mshtml.h>		/* Defines of stuff like IHTMLDocument2. This is an include file with Visual C 6 and above */

#if defined(_MSC_VER)
#include <crtdbg.h>		/* for _ASSERT() */
#else
#define _ASSERT(c)
#endif

/* Our IOleInPlaceFrame functions that the browser may call */
HRESULT STDMETHODCALLTYPE Frame_QueryInterface(IOleInPlaceFrame FAR* This, REFIID riid, LPVOID FAR* ppvObj);
HRESULT STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR* This);
HRESULT STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR* This);
HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR* This, HWND FAR* lphwnd);
HRESULT STDMETHODCALLTYPE Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR* This, BOOL fEnterMode);
HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR* This, LPRECT lprectBorder);
HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(IOleInPlaceFrame FAR* This, LPCBORDERWIDTHS pborderwidths);
HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(IOleInPlaceFrame FAR* This, LPCBORDERWIDTHS pborderwidths);
HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(IOleInPlaceFrame FAR* This, IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);
HRESULT STDMETHODCALLTYPE Frame_InsertMenus(IOleInPlaceFrame FAR* This, HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR* This, HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR* This, HMENU hmenuShared);
HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR* This, LPCOLESTR pszStatusText);
HRESULT STDMETHODCALLTYPE Frame_EnableModeless(IOleInPlaceFrame FAR* This, BOOL fEnable);
HRESULT STDMETHODCALLTYPE Frame_TranslateAccelerator(IOleInPlaceFrame FAR* This, LPMSG lpmsg, WORD wID);

/* Our IOleInPlaceFrame VTable. This is the array of pointers to the above functions in our C
 * program that the browser may call in order to interact with our frame window that contains
 * the browser object. We must define a particular set of functions that comprise the
 * IOleInPlaceFrame set of functions (see above), and then stuff pointers to those functions
 * in their respective 'slots' in this table. We want the browser to use this VTable with our
 * IOleInPlaceFrame structure.
 */
static const IOleInPlaceFrameVtbl MyIOleInPlaceFrameTable = {Frame_QueryInterface,
Frame_AddRef,
Frame_Release,
Frame_GetWindow,
Frame_ContextSensitiveHelp,
Frame_GetBorder,
Frame_RequestBorderSpace,
Frame_SetBorderSpace,
Frame_SetActiveObject,
Frame_InsertMenus,
Frame_SetMenu,
Frame_RemoveMenus,
Frame_SetStatusText,
Frame_EnableModeless,
Frame_TranslateAccelerator};

/* We need to pass an IOleInPlaceFrame struct to the browser object. And one of our IOleInPlaceFrame
 * functions (Frame_GetWindow) is going to need to access our window handle. So let's create our own
 * struct that starts off with an IOleInPlaceFrame struct (and that's important -- the IOleInPlaceFrame
 * struct *must* be first), and then has an extra data field where we can store our own window's HWND.
 *
 * And because we may want to create multiple windows, each hosting its own browser object (to
 * display its own web page), then we need to create a IOleInPlaceFrame struct for each window. So,
 * we're not going to declare our IOleInPlaceFrame struct globally. We'll allocate it later using
 * GlobalAlloc, and then stuff the appropriate HWND in it then, and also stuff a pointer to
 * MyIOleInPlaceFrameTable in it. But let's just define it here.
 */
typedef struct _IOleInPlaceFrameEx {
	IOleInPlaceFrame	frame;		// The IOleInPlaceFrame must be first!

	/* //////////////////////////////////////////////////
	// Here you add any variables that you need
	// to access in your IOleInPlaceFrame functions.
	// You don't want those functions to access global
	// variables, because then you couldn't use more
	// than one browser object. (ie, You couldn't have
	// multiple windows, each with its own embedded
	// browser object to display a different web page).
	//
	// So here is where I added my extra HWND that my
	// IOleInPlaceFrame function Frame_GetWindow() needs
	// to access.
	////////////////////////////////////////////////// */
	HWND				window;
} IOleInPlaceFrameEx;






/* Our IOleClientSite functions that the browser may call */
HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR* This, REFIID riid, void ** ppvObject);
HRESULT STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_Release(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR* This, DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk);
HRESULT STDMETHODCALLTYPE Site_GetContainer(IOleClientSite FAR* This, LPOLECONTAINER FAR* ppContainer);
HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR* This, BOOL fShow);
HRESULT STDMETHODCALLTYPE Site_RequestNewObjectLayout(IOleClientSite FAR* This);

/* Our IOleClientSite VTable. This is the array of pointers to the above functions in our C
 * program that the browser may call in order to interact with our frame window that contains
 * the browser object. We must define a particular set of functions that comprise the
 * IOleClientSite set of functions (see above), and then stuff pointers to those functions
 * in their respective 'slots' in this table. We want the browser to use this VTable with our
 * IOleClientSite structure.
 */
static const IOleClientSiteVtbl MyIOleClientSiteTable = {Site_QueryInterface,
Site_AddRef,
Site_Release,
Site_SaveObject,
Site_GetMoniker,
Site_GetContainer,
Site_ShowObject,
Site_OnShowWindow,
Site_RequestNewObjectLayout};






/* Our IOleInPlaceSite functions that the browser may call */
HRESULT STDMETHODCALLTYPE Site_GetWindow(IOleInPlaceSite FAR* This, HWND FAR* lphwnd);
HRESULT STDMETHODCALLTYPE Site_ContextSensitiveHelp(IOleInPlaceSite FAR* This, BOOL fEnterMode);
HRESULT STDMETHODCALLTYPE Site_CanInPlaceActivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_OnInPlaceActivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_OnUIActivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_GetWindowContext(IOleInPlaceSite FAR* This, LPOLEINPLACEFRAME FAR* lplpFrame,LPOLEINPLACEUIWINDOW FAR* lplpDoc,LPRECT lprcPosRect,LPRECT lprcClipRect,LPOLEINPLACEFRAMEINFO lpFrameInfo);
HRESULT STDMETHODCALLTYPE Site_Scroll(IOleInPlaceSite FAR* This, SIZE scrollExtent);
HRESULT STDMETHODCALLTYPE Site_OnUIDeactivate(IOleInPlaceSite FAR* This, BOOL fUndoable);
HRESULT STDMETHODCALLTYPE Site_OnInPlaceDeactivate(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_DiscardUndoState(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_DeactivateAndUndo(IOleInPlaceSite FAR* This);
HRESULT STDMETHODCALLTYPE Site_OnPosRectChange(IOleInPlaceSite FAR* This, LPCRECT lprcPosRect);

/* Our IOleInPlaceSite VTable. This is the array of pointers to the above functions in our C
 * program that the browser may call in order to interact with our frame window that contains
 * the browser object. We must define a particular set of functions that comprise the
 * IOleInPlaceSite set of functions (see above), and then stuff pointers to those functions
 * in their respective 'slots' in this table. We want the browser to use this VTable with our
 * IOleInPlaceSite structure.
 */
static const IOleInPlaceSiteVtbl MyIOleInPlaceSiteTable =  {Site_QueryInterface,	/* This gives a compiler warning because we're using
															* the same function as the MyIOleClientSiteTable uses.
															* And the first arg to that Site_QueryInterface() is
															* a pointer to a IOleClientSite. What we really should
															* have here is a separate function with its first arg
															* as a pointer to a IOleInPlaceSite (even though what
															* will really get passed to it is a _IOleClientSiteEx *).
															* But it's easier to use one function for QueryInterface()
															* in order to support "inheritance". ie, Sometimes, the
															* browser may call your IOleClientSite's QueryInterface
															* in order to get a pointer to your IOleInPlaceSite, or
															* vice versa. This is because these two structs will be
															* passed to the browser inside of a single struct. And
															* that makes them "interconnected" as far as
															* QueryInterface is concerned.
															*/
Site_AddRef,				/* Ditto as above. */
Site_Release,				/* Ditto as above. */
Site_GetWindow,
Site_ContextSensitiveHelp,
Site_CanInPlaceActivate,
Site_OnInPlaceActivate,
Site_OnUIActivate,
Site_GetWindowContext,
Site_Scroll,
Site_OnUIDeactivate,
Site_OnInPlaceDeactivate,
Site_DiscardUndoState,
Site_DeactivateAndUndo,
Site_OnPosRectChange};

/* The structure we need to pass to the browser object must contain an IOleClientSite structure. The
 * IOleClientSite struct *must* be first. Our IOleClientSite's QueryInterface() may also be asked to
 * return a pointer to our IOleInPlaceSite struct. So we'll need to have that object handy. Plus,
 * some of our IOleClientSite and IOleInPlaceSite functions will need to have the HWND to our window,
 * and also a pointer to our IOleInPlaceFrame struct. So let's create a single struct that has both
 * the IOleClientSite and IOleInPlaceSite structs inside it, and also has an extra data field to
 * store a pointer to our IOleInPlaceFrame struct. (The HWND is stored in the IOleInPlaceFrame
 * struct, so we can get at it there). As long as the IOleClientSite struct is the first thing, it's
 * all ok. We'll call this new struct a _IOleClientSiteEx.
 *
 * And because we may want to create multiple windows, each hosting its own browser object (to
 * display its own web page), then we need to create IOleClientSite and IOleInPlaceSite structs for
 * each window. (ie, Each window needs its own _IOleClientSiteEx struct). So, we're not going to
 * declare this struct globally. We'll allocate it later using GlobalAlloc, and then store the
 * appropriate IOleInPlaceFrame struct pointer then.
 */
typedef struct __IOleInPlaceSiteEx {
	IOleInPlaceSite		inplace;		/* My IOleInPlaceSite object. Must be first. */

	/* //////////////////////////////////////////////////
	// Here you add any variables that you need
	// to access in your IOleInPlaceSite functions.
	// You don't want those functions to access global
	// variables, because then you couldn't use more
	// than one browser object. (ie, You couldn't have
	// multiple windows, each with its own embedded
	// browser object to display a different web page.
	//
	// So here is where I added my extra pointer to my
	// IOleInPlaceFrame struct.
	////////////////////////////////////////////////// */
	IOleInPlaceFrameEx	*frame;
} _IOleInPlaceSiteEx;

typedef struct __IOleClientSiteEx {
	IOleClientSite		client;			/* My IOleClientSite object. Must be first. */
	_IOleInPlaceSiteEx	inplace;		/* My IOleInPlaceSite object. */

	/* //////////////////////////////////////////////////
	// Here you add any variables that you need
	// to access in your IOleClientSite functions.
	////////////////////////////////////////////////// */

} _IOleClientSiteEx;



// This is a simple C example. There are lots more things you can control about the browser object, but
// we don't do it in this example. _Many_ of the functions we provide below for the browser to call, will
// never actually be called by the browser in our example. Why? Because we don't do certain things
// with the browser that would require it to call those functions (even though we need to provide
// at least some stub for all of the functions).
//
// So, for these "dummy functions" that we don't expect the browser to call, we'll just stick in some
// assembly code that causes a debugger breakpoint and tells the browser object that we don't support
// the functionality. That way, if you try to do more things with the browser object, and it starts
// calling these "dummy functions", you'll know which ones you should add more meaningful code to.
#define NOTIMPLEMENTED _ASSERT(0); return(E_NOTIMPL)


///////////////////////////////// My IOleClientSite/IOleInPlaceSite functions  /////////////////////////////////

/************************* Site_QueryInterface() *************************
 * The browser object calls this when it wants a pointer to one of our
 * IOleClientSite or IOleInPlaceSite structures. They are both in the
 * _IOleClientSiteEx struct we allocated in EmbedBrowserObject() and
 * passed to DoVerb() and OleCreate().
 *
 * This =		A pointer to whatever _IOleClientSiteEx struct we passed to
 *				OleCreate() or DoVerb().
 * riid =		A GUID struct that the browser passes us to clue us as to
 *				which type of struct (object) it would like a pointer
 *				returned for.
 * ppvObject =	Where the browser wants us to return a pointer to the
 *				appropriate struct. (ie, It passes us a handle to fill in).
 *
 * RETURNS: S_OK if we return the struct, or E_NOINTERFACE if we don't have
 * the requested struct.
 */

HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR* This, REFIID riid, void ** ppvObject)
{
	// It just so happens that the first arg passed to us is our _IOleClientSiteEx struct we allocated
	// and passed to DoVerb() and OleCreate(). Nevermind that 'This' is declared is an IOleClientSite *.
	// Remember that in EmbedBrowserObject(), we allocated our own _IOleClientSiteEx struct, and lied
	// to OleCreate() and DoVerb() -- passing our _IOleClientSiteEx struct and saying it was an
	// IOleClientSite struct. It's ok. An _IOleClientSiteEx starts with an embedded IOleClientSite, so
	// the browser didn't mind. So that's what the browser object is passing us now. The browser doesn't
	// know that it's really an _IOleClientSiteEx struct. But we do. So we can recast it and use it as
	// so here.

	// If the browser is asking us to match IID_IOleClientSite, then it wants us to return a pointer to
	// our IOleClientSite struct. Then the browser will use the VTable in that struct to call our
	// IOleClientSite functions. It will also pass this same pointer to all of our IOleClientSite
	// functions.
	//
	// Actually, we're going to lie to the browser again. We're going to return our own _IOleClientSiteEx
	// struct, and tell the browser that it's a IOleClientSite struct. It's ok. The first thing in our
	// _IOleClientSiteEx is an embedded IOleClientSite, so the browser doesn't mind. We want the browser
	// to continue passing our _IOleClientSiteEx pointer wherever it would normally pass a IOleClientSite
	// pointer.
	//
	// The IUnknown interface uses the same VTable as the first object in our _IOleClientSiteEx
	// struct (which happens to be an IOleClientSite). So if the browser is asking us to match
	// IID_IUnknown, then we'll also return a pointer to our _IOleClientSiteEx.

	if (!memcmp(riid, &IID_IUnknown, sizeof(GUID)) || !memcmp(riid, &IID_IOleClientSite, sizeof(GUID)))
		*ppvObject = &((_IOleClientSiteEx *)This)->client;

	// If the browser is asking us to match IID_IOleInPlaceSite, then it wants us to return a pointer to
	// our IOleInPlaceSite struct. Then the browser will use the VTable in that struct to call our
	// IOleInPlaceSite functions.  It will also pass this same pointer to all of our IOleInPlaceSite
	// functions (except for Site_QueryInterface, Site_AddRef, and Site_Release. Those will always get
	// the pointer to our _IOleClientSiteEx.
	//
	// Actually, we're going to lie to the browser. We're going to return our own IOleInPlaceSiteEx
	// struct, and tell the browser that it's a IOleInPlaceSite struct. It's ok. The first thing in
	// our IOleInPlaceSiteEx is an embedded IOleInPlaceSite, so the browser doesn't mind. We want the
	// browser to continue passing our IOleInPlaceSiteEx pointer wherever it would normally pass a
	// IOleInPlaceSite pointer. My, we're really playing dirty tricks on the browser here. heheh.
	else if (!memcmp(riid, &IID_IOleInPlaceSite, sizeof(GUID)))
		*ppvObject = &((_IOleClientSiteEx *)This)->inplace;

	// For other types of objects the browser wants, just report that we don't have any such objects.
	// NOTE: If you want to add additional functionality to your browser hosting, you may need to
	// provide some more objects here. You'll have to investigate what the browser is asking for
	// (ie, what REFIID it is passing).
	else
	{
		*ppvObject = 0;
		return(E_NOINTERFACE);
	}

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR* This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Site_Release(IOleClientSite FAR* This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR* This)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR* This, DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetContainer(IOleClientSite FAR* This, LPOLECONTAINER FAR* ppContainer)
{
	// Tell the browser that we are a simple object and don't support a container
	*ppContainer = 0;

	return(E_NOINTERFACE);
}

HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR* This)
{
	return(NOERROR);
}

HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR* This, BOOL fShow)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_RequestNewObjectLayout(IOleClientSite FAR* This)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetWindow(IOleInPlaceSite FAR* This, HWND FAR* lphwnd)
{
	// Return the HWND of the window that contains this browser object. We stored that
	// HWND in our _IOleInPlaceSiteEx struct. Nevermind that the function declaration for
	// Site_GetWindow says that 'This' is an IOleInPlaceSite *. Remember that in
	// EmbedBrowserObject(), we allocated our own _IOleInPlaceSiteEx struct which
	// contained an embedded IOleInPlaceSite struct within it. And when the browser
	// called Site_QueryInterface() to get a pointer to our IOleInPlaceSite object, we
	// returned a pointer to our _IOleClientSiteEx. The browser doesn't know this. But
	// we do. That's what we're really being passed, so we can recast it and use it as
	// so here.
	*lphwnd = ((_IOleInPlaceSiteEx FAR*)This)->frame->window;

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_ContextSensitiveHelp(IOleInPlaceSite FAR* This, BOOL fEnterMode)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_CanInPlaceActivate(IOleInPlaceSite FAR* This)
{
	// Tell the browser we can in place activate
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_OnInPlaceActivate(IOleInPlaceSite FAR* This)
{
	// Tell the browser we did it ok
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_OnUIActivate(IOleInPlaceSite FAR* This)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_GetWindowContext(IOleInPlaceSite FAR* This, LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	// Give the browser the pointer to our IOleInPlaceFrame struct. We stored that pointer
	// in our _IOleInPlaceSiteEx struct. Nevermind that the function declaration for
	// Site_GetWindowContext says that 'This' is an IOleInPlaceSite *. Remember that in
	// EmbedBrowserObject(), we allocated our own _IOleInPlaceSiteEx struct which
	// contained an embedded IOleInPlaceSite struct within it. And when the browser
	// called Site_QueryInterface() to get a pointer to our IOleInPlaceSite object, we
	// returned a pointer to our _IOleClientSiteEx. The browser doesn't know this. But
	// we do. That's what we're really being passed, so we can recast it and use it as
	// so here.
	//
	// Actually, we're giving the browser a pointer to our own IOleInPlaceSiteEx struct,
	// but telling the browser that it's a IOleInPlaceSite struct. No problem. Our
	// IOleInPlaceSiteEx starts with an embedded IOleInPlaceSite, so the browser is
	// cool with it. And we want the browser to pass a pointer to this IOleInPlaceSiteEx
	// wherever it would pass a IOleInPlaceSite struct to our IOleInPlaceSite functions.
	*lplpFrame = (LPOLEINPLACEFRAME)((_IOleInPlaceSiteEx FAR*)This)->frame;

	// We have no OLEINPLACEUIWINDOW
	*lplpDoc = 0;

	// Fill in some other info for the browser
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = ((IOleInPlaceFrameEx FAR*)*lplpFrame)->window;
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;

	// Give the browser the dimensions of where it can draw. We give it our entire window to fill
	GetClientRect(lpFrameInfo->hwndFrame, lprcPosRect);
	GetClientRect(lpFrameInfo->hwndFrame, lprcClipRect);

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_Scroll(IOleInPlaceSite FAR* This, SIZE scrollExtent)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_OnUIDeactivate(IOleInPlaceSite FAR* This, BOOL fUndoable)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_OnInPlaceDeactivate(IOleInPlaceSite FAR* This)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_DiscardUndoState(IOleInPlaceSite FAR* This)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_DeactivateAndUndo(IOleInPlaceSite FAR* This)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_OnPosRectChange(IOleInPlaceSite FAR* This, LPCRECT lprcPosRect)
{
	return(S_OK);
}







////////////////////////////////////// My IOleInPlaceFrame functions  /////////////////////////////////////////

HRESULT STDMETHODCALLTYPE Frame_QueryInterface(IOleInPlaceFrame FAR* This, REFIID riid, LPVOID FAR* ppvObj)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR* This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR* This)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR* This, HWND FAR* lphwnd)
{
	// Give the browser the HWND to our window that contains the browser object. We
	// stored that HWND in our IOleInPlaceFrame struct. Nevermind that the function
	// declaration for Frame_GetWindow says that 'This' is an IOleInPlaceFrame *. Remember
	// that in EmbedBrowserObject(), we allocated our own IOleInPlaceFrameEx struct which
	// contained an embedded IOleInPlaceFrame struct within it. And then we lied when
	// Site_GetWindowContext() returned that IOleInPlaceFrameEx. So that's what the
	// browser is passing us. It doesn't know that. But we do. So we can recast it and
	// use it as so here.
	*lphwnd = ((IOleInPlaceFrameEx FAR*)This)->window;
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR* This, BOOL fEnterMode)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR* This, LPRECT lprectBorder)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(IOleInPlaceFrame FAR* This, LPCBORDERWIDTHS pborderwidths)
{
	return INPLACE_E_NOTOOLSPACE;
}

HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(IOleInPlaceFrame FAR* This, LPCBORDERWIDTHS pborderwidths)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(IOleInPlaceFrame FAR* This, IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_InsertMenus(IOleInPlaceFrame FAR* This, HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR* This, HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR* This, HMENU hmenuShared)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR* This, LPCOLESTR pszStatusText)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_EnableModeless(IOleInPlaceFrame FAR* This, BOOL fEnable)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_TranslateAccelerator(IOleInPlaceFrame FAR* This, LPMSG lpmsg, WORD wID)
{
	return(S_FALSE);
}








/*************************** UnEmbedBrowserObject() ************************
 * Called to detach the browser object from our host window, and free its
 * resources, right before we destroy our window.
 *
 * hwnd =		Handle to the window hosting the browser object.
 *
 * NOTE: The pointer to the browser object must have been stored in the
 * window's USERDATA field. In other words, don't call UnEmbedBrowserObject().
 * with a HWND that wasn't successfully passed to EmbedBrowserObject().
 */

void WINAPI UnEmbedBrowserObject(HWND hwnd)
{
	IOleObject	**browserHandle;
	IOleObject	*browserObject;

	// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
	// we initially attached the browser object to this window.
	if (NULL != (browserHandle = (IOleObject **)SetWindowLongPtr(hwnd, GWLP_USERDATA, 0)))
	{
		// Unembed the browser object, and release its resources.
		browserObject = *browserHandle;
		browserObject->lpVtbl->Close(browserObject, OLECLOSE_NOSAVE);
		browserObject->lpVtbl->Release(browserObject);
		GlobalFree(browserHandle);
		return;
	}

	// You must have called this for a window that wasn't successfully passed to EmbedBrowserObject().
	// Bad boy!
	_ASSERT(0);
}






/******************************* DisplayHTMLStr() ****************************
 * Takes a string containing some HTML BODY, and displays it in the specified
 * window. For example, perhaps you want to display the HTML text of...
 *
 * <P>This is a picture.<P><IMG src="mypic.jpg">
 *
 * hwnd =		Handle to the window hosting the browser object.
 * string =		Pointer to nul-terminated string containing the HTML BODY.
 *				(NOTE: No <BODY></BODY> tags are required in the string).
 *
 * RETURNS: 0 if success, or non-zero if an error.
 *
 * NOTE: EmbedBrowserObject() must have been successfully called once with the
 * specified window, prior to calling this function. You need call
 * EmbedBrowserObject() once only, and then you can make multiple calls to
 * this function to display numerous pages in the specified window.
 */

long WINAPI DisplayHTMLStr(HWND hwnd, LPCTSTR string)
{
	IWebBrowser2	*webBrowser2;
	LPDISPATCH		lpDispatch;
	LPSTREAM		lpStream;
	HGLOBAL			hbuf;
	LPVOID			lpbuf;
	DWORD			dwbuf;
	IPersistStreamInit	*htmlStreamInit;
	IOleObject		*browserObject;
	VARIANT			myURL;
	long			lRet = -1;

	// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
	// we initially attached the browser object to this window.
	browserObject = *((IOleObject **)GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (!browserObject) return(-1);

	// We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
	// object, so we can call some of the functions in the former's table.
	if (SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)&webBrowser2)))
	{
		// Ok, now the pointer to our IWebBrowser2 object is in 'webBrowser2', and so its VTable is
		// webBrowser2->lpVtbl.

		// Before we can get_Document(), we actually need to have some HTML page loaded in the browser. So,
		// let's load an empty HTML page. Then, once we have that empty page, we can get_Document() and
		// write() to stuff our HTML string into it.
		VariantInit(&myURL);
		myURL.vt = VT_BSTR;
		myURL.bstrVal = SysAllocString(L"about:blank");

		// Call the Navigate2() function to actually display the page.
		webBrowser2->lpVtbl->Navigate2(webBrowser2, &myURL, 0, 0, 0, 0);

		// Free any resources (including the BSTR).
		VariantClear(&myURL);

		// Call the IWebBrowser2 object's get_Document so we can get its DISPATCH object. I don't know why you
		// don't get the DISPATCH object via the browser object's QueryInterface(), but you don't.
		if (SUCCEEDED(webBrowser2->lpVtbl->get_Document(webBrowser2, &lpDispatch)))
		{
			if (SUCCEEDED(lpDispatch->lpVtbl->QueryInterface(lpDispatch, &IID_IPersistStreamInit, (void**)&htmlStreamInit)))
			{
				if (SUCCEEDED(htmlStreamInit->lpVtbl->InitNew(htmlStreamInit)))
				{
#ifndef UNICODE
					dwbuf = (lstrlenA(string) + 1) * sizeof(CHAR);
#else
					dwbuf = (lstrlenW(string) + 1) * sizeof(WCHAR);
#endif
					hbuf = GlobalAlloc(GMEM_MOVEABLE, dwbuf);
					if (hbuf)
					{
						lpbuf = GlobalLock(hbuf);
						if (lpbuf)
						{
							lstrcpy(lpbuf, string);
							GlobalUnlock(hbuf);
						}
						else
						{
							GlobalFree(hbuf);
							hbuf = 0;
						}
					}
					if (hbuf)
					{
						if (SUCCEEDED(CreateStreamOnHGlobal(hbuf, TRUE, &lpStream)))
						{
							ULARGE_INTEGER u64buf;
							u64buf.QuadPart = dwbuf;
							lpStream->lpVtbl->SetSize(lpStream, u64buf);
							if (SUCCEEDED(htmlStreamInit->lpVtbl->Load(htmlStreamInit, lpStream)))
								lRet = 0;
							lpStream->lpVtbl->Release(lpStream);
							hbuf = 0;
						}
					}
					if (hbuf) GlobalFree(hbuf);
				}
				htmlStreamInit->lpVtbl->Release(htmlStreamInit);
			}

			lpDispatch->lpVtbl->Release(lpDispatch);
		}

		webBrowser2->lpVtbl->Release(webBrowser2);
	}

	return lRet;
}






/******************************* DisplayHTMLPage() ****************************
 * Displays a URL, or HTML file on disk.
 *
 * hwnd =		Handle to the window hosting the browser object.
 * webPageName =	Pointer to nul-terminated name of the URL/file.
 *
 * RETURNS: 0 if success, or non-zero if an error.
 *
 * NOTE: EmbedBrowserObject() must have been successfully called once with the
 * specified window, prior to calling this function. You need call
 * EmbedBrowserObject() once only, and then you can make multiple calls to
 * this function to display numerous pages in the specified window.
 */

long WINAPI DisplayHTMLPage(HWND hwnd, LPCTSTR webPageName)
{
	IWebBrowser2	*webBrowser2;
	VARIANT			myURL;
	IOleObject		*browserObject;

	// Retrieve the browser object's pointer we stored in our window's GWL_USERDATA when
	// we initially attached the browser object to this window.
	browserObject = *((IOleObject **)GetWindowLongPtr(hwnd, GWLP_USERDATA));

	// We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
	// object, so we can call some of the functions in the former's table.
	if (SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)&webBrowser2)))
	{
		// Ok, now the pointer to our IWebBrowser2 object is in 'webBrowser2', and so its VTable is
		// webBrowser2->lpVtbl.

		// Our URL (ie, web address, such as "http://www.microsoft.com" or an HTM filename on disk
		// such as "c:\myfile.htm") must be passed to the IWebBrowser2's Navigate2() function as a BSTR.
		// A BSTR is like a pascal version of a double-byte character string. In other words, the
		// first unsigned short is a count of how many characters are in the string, and then this
		// is followed by those characters, each expressed as an unsigned short (rather than a
		// char). The string is not nul-terminated. The OS function SysAllocString can allocate and
		// copy a UNICODE C string to a BSTR. Of course, we'll need to free that BSTR after we're done
		// with it. If we're not using UNICODE, we first have to convert to a UNICODE string.
		//
		// What's more, our BSTR needs to be stuffed into a VARIENT struct, and that VARIENT struct is
		// then passed to Navigate2(). Why? The VARIENT struct makes it possible to define generic
		// 'datatypes' that can be used with all languages. Not all languages support things like
		// nul-terminated C strings. So, by using a VARIENT, whose first field tells what sort of
		// data (ie, string, float, etc) is in the VARIENT, COM interfaces can be used by just about
		// any language.
		VariantInit(&myURL);
		myURL.vt = VT_BSTR;

#ifndef UNICODE
		{
		wchar_t		*buffer;
		DWORD		size;

		size = MultiByteToWideChar(CP_ACP, 0, webPageName, -1, 0, 0);
		if (NULL == (buffer = (wchar_t *)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size))) goto badalloc;
		MultiByteToWideChar(CP_ACP, 0, webPageName, -1, buffer, size);
		myURL.bstrVal = SysAllocString(buffer);
		GlobalFree(buffer);
		}
#else
		myURL.bstrVal = SysAllocString(webPageName);
#endif
		if (!myURL.bstrVal)
		{
badalloc:	webBrowser2->lpVtbl->Release(webBrowser2);
			return(-6);
		}

		// Call the Navigate2() function to actually display the page.
		webBrowser2->lpVtbl->Navigate2(webBrowser2, &myURL, 0, 0, 0, 0);

		// Free any resources (including the BSTR we allocated above).
		VariantClear(&myURL);

		// We no longer need the IWebBrowser2 object (ie, we don't plan to call any more functions in it,
		// so we can release our hold on it). Note that we'll still maintain our hold on the browser
		// object.
		webBrowser2->lpVtbl->Release(webBrowser2);

		// Success
		return(0);
	}

	return(-5);
}





/***************************** EmbedBrowserObject() **************************
 * Puts the browser object inside our host window, and save a pointer to this
 * window's browser object in the window's GWL_USERDATA field.
 *
 * hwnd =		Handle of our window into which we embed the browser object.
 *
 * RETURNS: 0 if success, or non-zero if an error.
 *
 * NOTE: We tell the browser object to occupy the entire client area of the
 * window.
 *
 * NOTE: No HTML page will be displayed here. We can do that with a subsequent
 * call to either DisplayHTMLPage() or DisplayHTMLStr(). This is merely once-only
 * initialization for using the browser object. In a nutshell, what we do
 * here is get a pointer to the browser object in our window's GWL_USERDATA
 * so we can access that object's functions whenever we want, and we also pass
 * pointers to our IOleClientSite, IOleInPlaceFrame, and IStorage structs so that
 * the browser can call our functions in our struct's VTables.
 */

long WINAPI EmbedBrowserObject(HWND hwnd)
{
	ILockBytes			*pLkbyt;
	IStorage			*pStorage;
	IOleObject			*browserObject;
	IWebBrowser2		*webBrowser2;
	RECT				rect;
	char				*ptr;
	IOleInPlaceFrameEx	*iOleInPlaceFrameEx;
	_IOleClientSiteEx	*_iOleClientSiteEx;

	// Our IOleClientSite, IOleInPlaceSite, and IOleInPlaceFrame functions need to get our window handle. We
	// could store that in some global. But then, that would mean that our functions would work with only that
	// one window. If we want to create multiple windows, each hosting its own browser object (to display its
	// own web page), then we need to create a unique IOleClientSite, IOleInPlaceSite, and IOleInPlaceFrame
	// structs for each window. (Actually, the IOleClientSite and IOleInPlaceSite must be allocated as a
	// single struct, with the IOleClientSite embedded first). And we'll put an extra field at the end of those
	// structs to store our extra data such as a window handle. Remember that a pointer to our IOleClientSite we
	// create here will be passed as the first arg to every one of our IOleClientSite functions. Ditto with the
	// IOleInPlaceFrame object we create here, and the IOleInPlaceFrame functions. So, our functions are able to
	// retrieve the window handle we'll store here, and then, they'll work with all such windows containing a
	// browser control. Of course, that means we need to GlobalAlloc the structs now. We'll just get all 3 with
	// a single call to GlobalAlloc, but you could do them separately if desired.
	//
	// Um, we're not actually allocating a IOleClientSite, IOleInPlaceSite, and IOleInPlaceFrame structs. Because
	// we're appending our own fields to them, we're getting an IOleInPlaceFrameEx and an _IOleClientSiteEx (which
	// contains both the IOleClientSite and IOleInPlaceSite. But as far as the browser is concerned, it thinks that
	// we're giving it the plain old IOleClientSite, IOleInPlaceSite, and IOleInPlaceFrame.
	//
	// One final thing. We're going to allocate extra room to store the pointer to the browser object.
	if (NULL == (ptr = (char *)GlobalAlloc(GMEM_FIXED, sizeof(IOleInPlaceFrameEx) + sizeof(_IOleClientSiteEx) + sizeof(IOleObject *))))
		return(-1);

	// Initialize our IOleInPlaceFrame object with a pointer to our IOleInPlaceFrame VTable.
	iOleInPlaceFrameEx = (IOleInPlaceFrameEx *)(ptr + sizeof(IOleObject *));
	iOleInPlaceFrameEx->frame.lpVtbl = (IOleInPlaceFrameVtbl *)&MyIOleInPlaceFrameTable;

	// Save our HWND (in the IOleInPlaceFrame object) so our IOleInPlaceFrame functions can retrieve it.
	iOleInPlaceFrameEx->window = hwnd;

	// Initialize our IOleClientSite object with a pointer to our IOleClientSite VTable.
	_iOleClientSiteEx = (_IOleClientSiteEx *)(ptr + sizeof(IOleInPlaceFrameEx) + sizeof(IOleObject *));
	_iOleClientSiteEx->client.lpVtbl = (IOleClientSiteVtbl *)&MyIOleClientSiteTable;

	// Initialize our IOleInPlaceSite object with a pointer to our IOleInPlaceSite VTable.
	_iOleClientSiteEx->inplace.inplace.lpVtbl = (IOleInPlaceSiteVtbl *)&MyIOleInPlaceSiteTable;

	// Save a pointer to our IOleInPlaceFrameEx object so that our IOleInPlaceSite functions can retrieve it.
	_iOleClientSiteEx->inplace.frame = iOleInPlaceFrameEx;

	// Get a pointer to the browser object and lock it down (so it doesn't "disappear" while we're using
	// it in this program). We do this by calling the OS function OleCreate().
	//
	// NOTE: We need this pointer to interact with and control the browser. With normal WIN32 controls such as a
	// Static, Edit, Combobox, etc, you obtain an HWND and send messages to it with SendMessage(). Not so with
	// the browser object. You need to get a pointer to its "base structure" (as returned by OleCreate()). This
	// structure contains an array of pointers to functions you can call within the browser object. Actually, the
	// base structure contains a 'lpVtbl' field that is a pointer to that array. We'll call the array a 'VTable'.
	//
	// For example, the browser object happens to have a SetHostNames() function we want to call. So, after we
	// retrieve the pointer to the browser object (in a local we'll name 'browserObject'), then we can call that
	// function, and pass it args, as so:
	//
	// browserObject->lpVtbl->SetHostNames(browserObject, SomeString, SomeString);
	//
	// There's our pointer to the browser object in 'browserObject'. And there's the pointer to the browser object's
	// VTable in 'browserObject->lpVtbl'. And the pointer to the SetHostNames function happens to be stored in an
	// field named 'SetHostNames' within the VTable. So we are actually indirectly calling SetHostNames by using
	// a pointer to it. That's how you use a VTable.
	//
	// NOTE: We pass our _IOleClientSiteEx struct and lie -- saying that it's a IOleClientSite. It's ok. A
	// _IOleClientSiteEx struct starts with an embedded IOleClientSite. So the browser won't care, and we want
	// this extended struct passed to our IOleClientSite functions.
	if (SUCCEEDED(CreateILockBytesOnHGlobal(NULL, TRUE, &pLkbyt)))
	{
		if (SUCCEEDED(StgCreateDocfileOnILockBytes(pLkbyt, STGM_CREATE | STGM_SHARE_EXCLUSIVE | STGM_READWRITE, 0, &pStorage)))
		{
			if (SUCCEEDED(OleCreate(&CLSID_WebBrowser, &IID_IOleObject, OLERENDER_DRAW, 0, (IOleClientSite *)_iOleClientSiteEx, pStorage, (void**)&browserObject)))
			{
				// Ok, we now have the pointer to the browser object in 'browserObject'. Let's save this in the
				// memory block we allocated above, and then save the pointer to that whole thing in our window's
				// USERDATA field. That way, if we need multiple windows each hosting its own browser object, we can
				// call EmbedBrowserObject() for each one, and easily associate the appropriate browser object with
				// its matching window and its own objects containing per-window data.
				*((IOleObject **)ptr) = browserObject;
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ptr);

				// We can now call the browser object's SetHostNames function. SetHostNames lets the browser object know our
				// application's name and the name of the document in which we're embedding the browser. (Since we have no
				// document name, we'll pass a 0 for the latter). When the browser object is opened for editing, it displays
				// these names in its titlebar.
				//
				// We are passing 3 args to SetHostNames. You'll note that the first arg to SetHostNames is the base
				// address of our browser control. This is something that you always have to remember when working in C
				// (as opposed to C++). When calling a VTable function, the first arg to that function must always be the
				// structure which contains the VTable. (In this case, that's the browser control itself). Why? That's
				// because that function is always assumed to be written in C++. And the first argument to any C++ function
				// must be its 'this' pointer (ie, the base address of its class, which in this case is our browser object
				// pointer). In C++, you don't have to pass this first arg, because the C++ compiler is smart enough to
				// produce an executable that always adds this first arg. In fact, the C++ compiler is smart enough to
				// know to fetch the function pointer from the VTable, so you don't even need to reference that. In other
				// words, the C++ equivalent code would be:
				//
				// browserObject->SetHostNames(L"My Host Name", 0);
				//
				// So, when you're trying to convert C++ code to C, always remember to add this first arg whenever you're
				// dealing with a VTable (ie, the field is usually named 'lpVtbl') in the standard objects, and also add
				// the reference to the VTable itself.
				//
				// Oh yeah, the L is because we need UNICODE strings. And BTW, the host and document names can be anything
				// you want.

				browserObject->lpVtbl->SetHostNames(browserObject, L"tclock 2ch", 0);

				GetClientRect(hwnd, &rect);

				// Let browser object know that it is embedded in an OLE container.
				if (SUCCEEDED(OleSetContainedObject((struct IUnknown *)browserObject, TRUE)) &&

					// Set the display area of our browser control the same as our window's size
					// and actually put the browser object into our window.
					SUCCEEDED(browserObject->lpVtbl->DoVerb(browserObject, OLEIVERB_SHOW, NULL, (IOleClientSite *)_iOleClientSiteEx, -1, hwnd, &rect)) &&

					// Ok, now things may seem to get even trickier, One of those function pointers in the browser's VTable is
					// to the QueryInterface() function. What does this function do? It lets us grab the base address of any
					// other object that may be embedded within the browser object. And this other object has its own VTable
					// containing pointers to more functions we can call for that object.
					//
					// We want to get the base address (ie, a pointer) to the IWebBrowser2 object embedded within the browser
					// object, so we can call some of the functions in the former's table. For example, one IWebBrowser2 function
					// we intend to call below will be Navigate2(). So we call the browser object's QueryInterface to get our
					// pointer to the IWebBrowser2 object.
					SUCCEEDED(browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2, (void**)&webBrowser2)))
				{
					// Ok, now the pointer to our IWebBrowser2 object is in 'webBrowser2', and so its VTable is
					// webBrowser2->lpVtbl.

					// Let's call several functions in the IWebBrowser2 object to position the browser display area
					// in our window. The functions we call are put_Left(), put_Top(), put_Width(), and put_Height().
					// Note that we reference the IWebBrowser2 object's VTable to get pointers to those functions. And
					// also note that the first arg we pass to each is the pointer to the IWebBrowser2 object.
					webBrowser2->lpVtbl->put_Left(webBrowser2, 0);
					webBrowser2->lpVtbl->put_Top(webBrowser2, 0);
					webBrowser2->lpVtbl->put_Width(webBrowser2, rect.right);
					webBrowser2->lpVtbl->put_Height(webBrowser2, rect.bottom);
					webBrowser2->lpVtbl->put_Silent(webBrowser2, VARIANT_TRUE);

					// We no longer need the IWebBrowser2 object (ie, we don't plan to call any more functions in it
					// right now, so we can release our hold on it). Note that we'll still maintain our hold on the
					// browser object until we're done with that object.
					webBrowser2->lpVtbl->Release(webBrowser2);

					// Success
					return(0);
				}

				// Something went wrong!
				UnEmbedBrowserObject(hwnd);
				return(-3);
			}
			pStorage->lpVtbl->Release(pStorage);
		}
		pLkbyt->lpVtbl->Release(pLkbyt);
	}

	GlobalFree(ptr);
	return(-2);
}
#pragma warning(pop)
