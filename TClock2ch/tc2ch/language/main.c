/*-----------------------------------------------------
  main.c
  Kazubon 2001
-------------------------------------------------------*/

#include <windows.h>

#pragma comment(linker, "/subsystem:windows,5.0")
#pragma comment(linker, "/base:0x63060000")
#if defined(NDEBUG)
# pragma comment(linker, "/entry:\"DllMain\"")
# pragma comment(linker, "/nodefaultlib:libcmt.lib")
# pragma comment(linker, "/defaultlib:kernel32.lib")
#endif


/*------------------------------------------------
  entry point
--------------------------------------------------*/
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwFunction, LPVOID lpNot)
{
	UNREFERENCED_PARAMETER(dwFunction);
	UNREFERENCED_PARAMETER(lpNot);
	DisableThreadLibraryCalls(hModule);
	return TRUE;
}

