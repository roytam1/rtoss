#ifndef __Macros_h__
#define __Macros_h__

#include <windows.h>

void mymemcpy(void *dest, void *src, int count);
void DebugPrint(char *szFormat, ...);

#define ARRAY_ITEMS(name) sizeof(name) / sizeof(name[0])
#define TESTBIT(val, flag)  (val & flag)
#define ZERO(strct) memset(&strct, 0, sizeof(strct));
#define MEMCPY mymemcpy

#if defined(_M_ALPHA) && defined(_WIN64)
#ifdef GWLP_WNDPROC
#define GWL_WNDPROC GWLP_WNDPROC
#endif
 #define FUNC_CALLBACK __cdecl
 #define FUNC_RET int
#elif defined(_WIN64)
#ifdef GWLP_WNDPROC
#define GWL_WNDPROC GWLP_WNDPROC
#endif
 #define FUNC_CALLBACK 
 #define FUNC_RET __int64
#elif defined(_M_ALPHA)
 #define FUNC_CALLBACK 
 #define FUNC_RET int
#else
 #define FUNC_CALLBACK __stdcall
 #define FUNC_RET int
#endif

#ifndef GCL_HICON
#define GCL_HICON (-14)
#endif

#endif
