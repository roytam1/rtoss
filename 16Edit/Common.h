
#ifndef __Common_h__
#define __Common_h__

#include <windows.h>
#include "macros.H"

//
// constants
//

// Hooked hex edit boxes with the ES_MULTILINE flag raise an WM_COMMAND
// msg, in the case the user presses enter, with the the following notify
// command.
#define NM_HEXEDITENTER            0x2222

//
// exports
//
BOOL                  UnHookHexEditbox(HWND hEdit);
BOOL                  HookHexEditbox(HWND hEdit);
BOOL                  HookEditboxEnter(HWND hEdit);
LRESULT FUNC_CALLBACK HexOnlyEditProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
LRESULT FUNC_CALLBACK EditHookEnterProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL                  HexStrToInt(char *szHexStr, DWORD *pdwHexVal);

#endif
