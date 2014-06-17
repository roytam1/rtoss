#pragma once

enum { lang_jp=0, lang_en=1 };

///////////////////////////////////////////////////////////////////////////////////////////////////
///*
//	1. return the charater count of pszString, including the terminated-zero,
//	if failed, the return value is 0
//	2. pszString is allocated by LocalAlloc(LMEM_FIX, n), which is a null-terminating string,
//	so use LocalFree() to free the returned pszString
//	3. the pbCode is terminated by bTerminatingCode, note that 0xFF always terminates the pbCode
//*/
DWORD CodeToString(LPTSTR *pszString, LPBYTE pbCode, DWORD cbCodeLength, BYTE bTerminatingCode, DWORD dwLang);
BOOL CodeToString(CString &szString, LPBYTE pbCode, DWORD cbCodeLength, BYTE bTerminatingCode, DWORD dwLang);

/*
	1. szString must be null-terminated
	2. if the szString contains any illegal characters,
	the pbCode will be terminated at the first illegal charater
	3. if the pbCode is large enough,
	it will be terminated by bTerminatingCode,
	and the rest bytes (if any) will be filled by bPaddingCode
	4. if the return value is FALSE, the bytes within pbCode are unpredictable
*/
BOOL StringToCode(LPCTSTR szString, LPBYTE pbCode, DWORD cbCodeLength, BYTE bTerminatingCode, BYTE bPaddingCode, DWORD dwLang);

/*
	return a randomized long value
*/
DWORD	GenLongRandom(VOID);

/*
	return a randomized short value
*/
WORD	GenShortRandom(VOID);

/*
	387 <-> 412
*/
WORD ConvertBreed(WORD wBreed, BOOL b387To412);

