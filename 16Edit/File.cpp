
#include <malloc.h>
#include "File.h"

CFile::CFile()
{
	Destroy();
}

CFile::~CFile()
{
	// Cleanup
	Destroy();
}

BOOL CFile::OpenFileForSave() {
	DWORD  dwFlags, dwAccess, dwShare;

	Destroy();

	bReadOnly = FALSE;
	dwFlags = OPEN_ALWAYS;
	dwAccess = GENERIC_READ | GENERIC_WRITE;
	dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;

	hFile = CreateFile(
		cFilePath,
		dwAccess,
		dwShare,
		NULL,
		dwFlags,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == (HANDLE)-1)
		return FALSE;

	return TRUE;
}

//
// Open a file in the specified mode
//
BOOL CFile::GetFileHandle(char *szFilePath, DWORD dwMode)
{
	DWORD  dwFlags, dwAccess, dwShare;

	// destroy if sth is already loaded
	Destroy();

	// save path
	lstrcpy(cFilePath, szFilePath);

	// handle flags
	switch(dwMode)
	{
	case F_OPENEXISTING_R:
		bReadOnly = TRUE;
		dwFlags = OPEN_EXISTING;
		dwAccess = GENERIC_READ;
		dwShare = FILE_SHARE_READ;
		break;

	case F_OPENEXISTING_RW:
		bReadOnly = FALSE;
		dwFlags = OPEN_EXISTING;
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;

	case F_CREATENEW:
		bReadOnly = FALSE;
		dwFlags = CREATE_ALWAYS;
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;

	case F_TRUNCATE: // RW
		bReadOnly = FALSE;
		dwFlags = TRUNCATE_EXISTING;
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;
	}

	hFile = CreateFile(
		szFilePath,
		dwAccess,
		dwShare,
		NULL,
		dwFlags,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == (HANDLE)-1)
		return FALSE;

	return TRUE;
}

//
// opens a file with RW access if possible else just R
BOOL CFile::GetFileHandleWithMaxAccess(char* szFilePath)
{
	BOOL bRet;

	bRet = GetFileHandle(szFilePath, F_OPENEXISTING_RW);
	if (!bRet)
		bRet = GetFileHandle(szFilePath, F_OPENEXISTING_R);

	return bRet;
}

//
// Cleanup routine
//
BOOL CFile::Destroy()
{
	BOOL bRet = FALSE;

	// cleanup
	if (hFile != INVALID_HANDLE_VALUE)
		if (CloseHandle(hFile))
			bRet = TRUE;
	if (pMap)
		free(pMap);

	// adjust variables
	hFile        = INVALID_HANDLE_VALUE;
	pMap         = NULL;
	dwMapSize    = 0;

	return bRet;
}

//
// returns:
// INVALID_HANDLE_VALUE - if no file is loaded
//
HANDLE CFile::GetHandle()
{
	return hFile;
}

BOOL CFile::IsFileReadOnly()
{
	return bReadOnly;
}

BOOL CFile::MapFile()
{
	DWORD dw, dwFSize;

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	dwFSize = GetFSize();

	// map file
	pMap = malloc( dwFSize );
	if (!pMap)
		return FALSE;
	if (!ReadFile(hFile, pMap, dwFSize, &dw, NULL))
		return FALSE;

	// set vars
	dwMapSize = dwFSize;

	return TRUE;
}

void* CFile::GetMapPtr()
{
	return pMap;
}

BOOL CFile::UnmapFile()
{
	if (!pMap)
		return FALSE;

	free(pMap);
	pMap       = NULL;
	dwMapSize  = 0;

	return TRUE;
}

//
// change size of file memory
//
BOOL CFile::ReMapFile(DWORD dwNewSize)
{
	if (!pMap)
		return FALSE; // ERR

	pMap = realloc(pMap, dwNewSize);
	if (pMap)
	{
		pMap      = NULL;
		dwMapSize = dwNewSize;
		return TRUE; // OK
	}
	else
		return FALSE; // ERR
}

//
// returns:
// -1 in the case of an error
//
DWORD CFile::GetMapSize()
{
	if (!pMap)
		return (DWORD)-1; // ERR

	return dwMapSize;
}

BOOL CFile::IsMapped()
{
	return pMap != NULL ? TRUE : FALSE;
}

//
// copy mapping memory to file
//
BOOL CFile::FlushFileMap()
{
	if ( !Truncate() )
		return FALSE; // ERR

	return Write(pMap, dwMapSize);
}

BOOL CFile::FileExits(char* szFilePath)
{
	CFile *f = new CFile;

	return f->GetFileHandle(szFilePath, F_OPENEXISTING_R);
}

DWORD CFile::GetFSize()
{
	return GetFileSize(hFile, NULL);
}
	
//
// returns:
// NULL - if no file is loaded
//	
char* CFile::GetFilePath()
{
	return cFilePath;
}

//
// write to file
//
BOOL CFile::Write(void* pBuff, DWORD dwc)
{
	DWORD  dwRet;
	BOOL   bRet;

	bRet = WriteFile(hFile, pBuff, dwc, &dwRet, NULL);
	
	return (bRet && dwc == dwRet) ? TRUE : FALSE;
}

//
// read from file
//
BOOL CFile::Read(void* pBuff, DWORD dwc)
{
	DWORD  dwRet;
	BOOL   bRet;

	bRet = ReadFile(hFile, pBuff, dwc, &dwRet, NULL);
	
	return (bRet && dwc == dwRet) ? TRUE : FALSE;
}

//
// set file pointer
//
BOOL CFile::SetFPointer(DWORD dwOff)
{
	DWORD dwRet;

	dwRet = SetFilePointer(hFile, dwOff, NULL, FILE_BEGIN);

	return (dwRet != (DWORD)-1) ? TRUE : FALSE;
}

BOOL CFile::Truncate()
{
	if (!SetFPointer(0))
		return FALSE; // ERR
	if (!SetEndOfFile(hFile))
		return FALSE; // ERR

	return TRUE; // OK
}

void CFile::SetMapPtrSize(void* ptr, DWORD dwSize)
{
	pMap      = ptr;
	dwMapSize = dwSize;

	return;
}
