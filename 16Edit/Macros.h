#ifndef __Macros_h__
#define __Macros_h__

#include <windows.h>

void mymemcpy(void *dest, void *src, int count);
void DebugPrint(char *szFormat, ...);

#define ARRAY_ITEMS(name) sizeof(name) / sizeof(name[0])
#define TESTBIT(val, flag)  (val & flag)
#define ZERO(strct) memset(&strct, 0, sizeof(strct));
#define MEMCPY mymemcpy

#endif
