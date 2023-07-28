#ifndef __Macros_h__
#define __Macros_h__

#include <windows.h>

void mymemcpy(void *dest, void *src, int count);
void DebugPrint(char *szFormat, ...);

#define ARRAY_ITEMS(name) sizeof(name) / sizeof(name[0])
#define TESTBIT(val, flag)  (val & flag)
#define ZERO(strct) memset(&strct, 0, sizeof(strct));
#define MEMCPY mymemcpy

#if defined(_WIN64) 
#define GWL_WNDPROC GWLP_WNDPROC
#define FUNC_CALLBACK 
 typedef __int64 INT_PTR;
#else
#define FUNC_CALLBACK __stdcall
 typedef int INT_PTR;
#endif

#ifndef GCL_HICON
#define GCL_HICON (-14)
#endif

#endif
