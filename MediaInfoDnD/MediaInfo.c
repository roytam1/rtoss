#define _UNICODE
#define UNICODE

#include <tchar.h>
#include <windows.h>
#include <shellapi.h>

/* MediaInfo */
#ifdef _UNICODE
const wchar_t*    __stdcall MediaInfo_Option (void* Handle, const wchar_t* Option, const wchar_t* Value);
void*             __stdcall MediaInfo_New ();
DWORD             __stdcall MediaInfo_Open (void* Handle, const wchar_t* File);
const wchar_t*    __stdcall MediaInfo_Inform (void* Handle, DWORD Reserved);
void              __stdcall MediaInfo_Close (void* Handle);
void              __stdcall MediaInfo_Delete (void* Handle);
#else
const char*       __stdcall MediaInfoA_Option (void* Handle, const char* Option, const char* Value);
#define  MediaInfo_Option   MediaInfoA_Option
void*             __stdcall MediaInfoA_New ();
#define  MediaInfoA_New     MediaInfoA_New
DWORD             __stdcall MediaInfoA_Open (void* Handle, const char* File);
#define  MediaInfoA_Open    MediaInfoA_Open
const char*       __stdcall MediaInfoA_Inform (void* Handle, DWORD Reserved);
#define  MediaInfoA_Inform  MediaInfoA_Inform
void              __stdcall MediaInfoA_Close (void* Handle);
#define  MediaInfoA_Close   MediaInfoA_Close
void              __stdcall MediaInfoA_Delete (void* Handle);
#define  MediaInfoA_Delete  MediaInfoA_Delete
#endif
/* End/MediaInfo */

const TCHAR g_szClassName[] = _T("MediaInfoDnDWindowClass");

#define IDC_MAIN_EDIT	101

HWND g_hEdit = NULL;
TCHAR g_szPath[MAX_PATH + 1];
DWORD g_dwHeight = 0;
DWORD g_dwWidth = 0;
DWORD g_dwUpdateSize = 0;
TCHAR g_FontName[100];
DWORD g_dwFontSize = 0;

void GetMediaInfo(HWND hEditCtl, TCHAR* cFile)
{
	void* hMediaInfo;
	DWORD dwRet;

	hMediaInfo = MediaInfo_New();
	dwRet = MediaInfo_Open(hMediaInfo, (TCHAR*)cFile);
	SendMessage(hEditCtl,WM_SETTEXT,0,(LPARAM)MediaInfo_Inform(hMediaInfo, 0));
	MediaInfo_Close(hMediaInfo);
	MediaInfo_Delete(hMediaInfo);
}

TCHAR* MyGetIniPath(LPCTSTR lpFileName)
{
	TCHAR* cOut;

	GetModuleFileName(NULL,g_szPath,MAX_PATH);

	//PathRemoveFileSpec(g_szPath);
	cOut = _tcsrchr(g_szPath, '\\');
	*(++cOut) = 0;
	//PathAppend(g_szPath,lpFileName);
	_tcscat(g_szPath, lpFileName);

    return g_szPath;
}

BOOL WritePrivateProfileInt( LPCTSTR lpAppName, LPCTSTR lpKeyName, INT Value, LPCTSTR lpFileName )
{
	TCHAR ValBuf[16];

	_stprintf( ValBuf,_T( "%i" ), Value);

	return( WritePrivateProfileString( lpAppName, lpKeyName, ValBuf, lpFileName ) );
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDROP hDropInfo = NULL;
	HFONT hfDefault;
	RECT rcClient;

	DWORD exStyles;

	DWORD dwBufSize = 1024;
	TCHAR cBuf[1024];

	switch(msg)
	{
		case WM_CREATE:
		{
			g_hEdit = CreateWindowEx(0/*WS_EX_CLIENTEDGE*/, _T("EDIT"), _T(""), 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | /*WS_HSCROLL |*/ ES_MULTILINE | ES_AUTOVSCROLL | /*ES_AUTOHSCROLL |*/ ES_READONLY, 
				0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(g_hEdit == NULL)
				MessageBox(hwnd, _T("Could not create edit box."), _T("Error"), MB_OK | MB_ICONERROR);
			DragAcceptFiles(hwnd,TRUE);
			if(g_dwFontSize)
			{
				hfDefault = CreateFont(-g_dwFontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, g_FontName);
			}
			else
				hfDefault = (HFONT)GetStockObject(ANSI_FIXED_FONT);
			SendMessage(g_hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

			_tcscpy(cBuf, MediaInfo_Option(0, _T("Info_Version"), _T("")));
			_tcscat(cBuf, _T("\r\n(Drop files to here to show Media Info.)"));

			SendMessage(g_hEdit,WM_SETTEXT,0,(LPARAM)cBuf);
		}
		break;
		case WM_ENTERSIZEMOVE:
		{
			exStyles = GetWindowLong(hwnd, GWL_EXSTYLE);
			exStyles |= WS_EX_COMPOSITED;
			SetWindowLong(hwnd, GWL_EXSTYLE, exStyles);
		}
		break;
		case WM_EXITSIZEMOVE:
		{
			exStyles = GetWindowLong(hwnd, GWL_EXSTYLE);
			exStyles &= ~WS_EX_COMPOSITED;
			SetWindowLong(hwnd, GWL_EXSTYLE, exStyles);
		}
		break;
		case WM_SIZE:
		{
			GetClientRect(hwnd, &rcClient);

//			hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
			SetWindowPos(g_hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
		}
		break;
		case WM_DROPFILES:
		{
			hDropInfo = (HDROP)wParam;
			DragQueryFile(hDropInfo, 0, cBuf, dwBufSize);
//			hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
//			SendMessage(g_hEdit,WM_SETTEXT,0,(LPARAM)cBuf);

			GetMediaInfo(g_hEdit, cBuf);
		}
		break;
		case WM_CLOSE:
		{
			if(g_dwUpdateSize && !IsIconic(hwnd) && !IsZoomed(hwnd))
			{
				GetWindowRect(hwnd, &rcClient);
				WritePrivateProfileInt(_T("MediaInfo"), _T("Height"), rcClient.bottom-rcClient.top, g_szPath);
				WritePrivateProfileInt(_T("MediaInfo"), _T("Width"), rcClient.right-rcClient.left, g_szPath);
			}
			DestroyWindow(hwnd);
		}
		break;
		case WM_DESTROY:
			DragAcceptFiles(hwnd,FALSE);
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	LPWSTR *szArgList;
	int argCount;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, _T("Window Registration Failed!"), _T("Error!"),
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	MyGetIniPath(_T("MediaInfo.ini"));
	g_dwHeight = GetPrivateProfileInt(_T("MediaInfo"), _T("Height"), 400, g_szPath);
	g_dwWidth = GetPrivateProfileInt(_T("MediaInfo"), _T("Width"), 600, g_szPath);
	g_dwUpdateSize = GetPrivateProfileInt(_T("MediaInfo"), _T("UpdateSize"), 0, g_szPath);
	g_dwFontSize = GetPrivateProfileInt(_T("MediaInfo"), _T("FontSize"), 0, g_szPath);
	GetPrivateProfileString(_T("MediaInfo"), _T("FontName"), _T("Courier New"), g_FontName, 99, g_szPath);

	hwnd = CreateWindowEx(
		0,
		g_szClassName,
		_T("MediaInfo DnD Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, g_dwWidth, g_dwHeight,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"),
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	/* ArgList parsing */
	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (szArgList == NULL)
	{
		MessageBox(NULL, _T("Unable to parse command line"), _T("Error"), MB_OK | MB_ICONEXCLAMATION);
		return 10;
	}

/*	for(int i = 0; i < argCount; i++)
	{
		MessageBox(NULL, szArgList[i], _T("Arglist contents"), MB_OK);
	}*/

	if(argCount > 1)
		GetMediaInfo(g_hEdit, szArgList[1]);
	LocalFree(szArgList);
	/* End/ArgList parsing */

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
