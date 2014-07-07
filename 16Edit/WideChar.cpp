
#include "WideChar.h"

WideChar::WideChar()
{}

WideChar::~WideChar()
{}

//
// converts a single character string into a UNICODE string
//
// Note:
// IN and OUT buffers can be identical
//
BOOL WideChar::SingleToWideCharStr(IN PSTR szStr, OUT PWSTR szWideStrBuff, IN DWORD dwcBuff)
{
	DWORD  dwcStr, dwcWideStr;
	void*  pUniStr;

	dwcStr = lstrlen(szStr);
	if (!dwcStr)
		return FALSE; // ERR
	dwcWideStr = (dwcStr+1)*2;
	if (dwcBuff < dwcWideStr)
		return FALSE; // ERR
	pUniStr = malloc(dwcWideStr);
	if (!pUniStr)
		return FALSE; // ERR

	if (!MultiByteToWideChar(
			CP_OEMCP,
			MB_PRECOMPOSED | MB_USEGLYPHCHARS,
			szStr, dwcStr+1,
			(PWSTR)pUniStr, dwcWideStr / 2))
		return FALSE; // ERR

	memcpy(
		(void*)szWideStrBuff,
		(void*)pUniStr,
		dwcWideStr);

	// Cleanup
	free(pUniStr);

	return TRUE; // OK
}

//
// converts a UNICODE string into a single character string
//
// Args:
// dwcBuff             - size of output buffer
//
// Note:
// IN and OUT buffers can be identical
//
BOOL WideChar::WideToSingleCharStr(IN PWSTR szWideStr, OUT PSTR szStrBuff, IN DWORD dwcBuff)
{
	DWORD  chars;
	void*  pStr;

	chars = lstrlenW(szWideStr);
	if (!chars)
		return FALSE; // ERR
	if (dwcBuff < (chars+1))
		return FALSE; // ERR
	pStr = malloc( chars+1 );
	if (!pStr)
		return FALSE; // ERR

	if (!WideCharToMultiByte(
			CP_OEMCP,
			0,
			szWideStr, chars + 1,
			(PSTR)pStr, chars + 1,
			NULL, NULL))
		return FALSE; // ERR

	memcpy(
		(void*)szStrBuff,
		(void*)pStr,
		chars + 1);

	// Cleanup
	free(pStr);

	return TRUE; // OK
}

//
// converts a UNICODE string into a single character string
//
// Args:
// dwcMaxCharToProcess - number of characters to process at "szWideStr" excluding NUL !
// dwcBuff             - size of output buffer
//
// Note:
// IN and OUT buffers can be identical
//
BOOL WideChar::WideToSingleCharStrN(IN PWSTR szWideStr, IN DWORD dwcMaxCharToProcess, OUT PSTR szStrBuff, IN DWORD dwcBuff)
{
	DWORD  chars;
	void*  pStr;

	chars = lstrlenW(szWideStr);
	if (!chars)
		return FALSE; // ERR
	chars = __min(dwcMaxCharToProcess, chars);
	if (dwcBuff < chars+1)
		return FALSE; // ERR
	pStr = malloc( chars );
	if (!pStr)
		return FALSE; // ERR

	if (!WideCharToMultiByte(
			CP_OEMCP,
			0,
			szWideStr, chars,
			(PSTR)pStr, chars,
			NULL, NULL))
		return FALSE; // ERR

	memcpy(
		(void*)szStrBuff,
		(void*)pStr,
		chars + 1);
	szStrBuff[chars] = 0; // insert NUL

	// Cleanup
	free(pStr);

	return TRUE; // OK
}