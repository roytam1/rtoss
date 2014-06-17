#include "windows.h"
#include "ProcessHandler.h"
#include "Tlhelp32.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
VOID SetBit(LPVOID pFlags, DWORD_PTR dwBitIndex)
{
	LPBYTE	pb = &((LPBYTE)(pFlags))[dwBitIndex >> 0x3];
	*pb |= ((BYTE)(1) << (BYTE)(dwBitIndex & 0x7));
}

VOID ClrBit(LPVOID pFlags, DWORD_PTR dwBitIndex)
{
	LPBYTE	pb = &((LPBYTE)(pFlags))[dwBitIndex >> 0x3];
	*pb &= ~((BYTE)(1) << (BYTE)(dwBitIndex & 0x7));
}

BYTE GetBit(LPCVOID pFlags, DWORD_PTR dwBitIndex)
{
	return ((((LPCBYTE)(pFlags))[dwBitIndex >> 0x3] >> (BYTE)(dwBitIndex & 0x7)) & 0x1);
}

VOID SetBitField(LPVOID pFlags, DWORD_PTR dwBitIndex, BYTE bBitCount, DWORD dwValue)
{
	BYTE	bIndex;

	if(bBitCount > 32)
		bBitCount = 32;

	for(bIndex = 0; bIndex < bBitCount; ++bIndex)
	{
		if(dwValue & 0x1)
			SetBit(pFlags, dwBitIndex + bIndex);
		else
			ClrBit(pFlags, dwBitIndex + bIndex);
		dwValue >>= 1;
	}
}

DWORD GetBitField(LPCVOID pFlags, DWORD_PTR dwBitIndex, BYTE bBitCount)
{
	DWORD	dwValue = 0;

	if(bBitCount > 32)
		bBitCount = 32;

	for(; bBitCount > 0; --bBitCount)
	{
		dwValue = (dwValue << 1) + GetBit(pFlags, dwBitIndex + bBitCount - 1);
	}

	return dwValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CProcessHandler::CProcessHandler()
{
	m_hProcess = NULL;
	m_hWnd = NULL;

	/////////////////////////////////////////////////
	// Fuzzier, 2005-10-21
	// Make sure the account who runs this application
	// has "Debug Privilege" (local security settings).
	// Otherwise, processes, invoked by accounts other than the account
	// running this application, CANNOT be attached!
	HANDLE	hProcess;
	HANDLE	hToken;
	LUID	id;

	hProcess = GetCurrentProcess();
	if(OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &id))
		{
			TOKEN_PRIVILEGES	TPEnableDebug;
			TPEnableDebug.PrivilegeCount = 1;
			TPEnableDebug.Privileges[0].Luid = id;
			TPEnableDebug.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if(!AdjustTokenPrivileges(hToken, FALSE, &TPEnableDebug, 0, NULL, NULL))
			{
				DWORD	dwErr = GetLastError();
			}
		}
		CloseHandle(hToken);
	}
	CloseHandle(hProcess);
	/////////////////////////////////////////////////
}

CProcessHandler::~CProcessHandler()
{
	Close();
}

BOOL CProcessHandler::AttachByWindowName(LPCTSTR lpszClassName, LPCTSTR lpszWindowName)
{
	DWORD dwProcessId;

	if(IsValid())
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}

	if((m_hWnd = FindWindow(lpszClassName, lpszWindowName)) == NULL)
		return FALSE;

	GetWindowThreadProcessId(m_hWnd, &dwProcessId);

	m_hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcessId);
	if(m_hProcess == NULL)
	{
		DWORD	dwErr = GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CProcessHandler::AttachByWindowHandle(HWND hWnd)
{
	DWORD	dwProcessId;

	if(IsValid())
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}

	if(IsWindow(hWnd))
	{
		m_hWnd = hWnd;
		GetWindowThreadProcessId(m_hWnd, &dwProcessId);
		m_hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcessId);
		if(m_hProcess == NULL)
			return FALSE;
	}

	return TRUE;
}

BOOL CProcessHandler::IsValid()
{
	if(m_hProcess == NULL)
		return FALSE;

	if(IsWindow(m_hWnd) == FALSE)
	{
		if(m_hProcess != NULL)
			CloseHandle(m_hProcess);
		m_hProcess = NULL;
		m_hWnd = NULL;
		return FALSE;
	}

	return TRUE;
}

VOID CProcessHandler::Close(VOID)
{
	if(m_hProcess != NULL)
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}
}

BOOL CProcessHandler::Read(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPVOID lpBuffer)
{
	LPVOID		lpBaseAddr = reinterpret_cast<LPVOID>(dwBaseAddr);
	DWORD_PTR	dwBytesRead;

	if(IsValid() == FALSE)
		return FALSE;

	if(ReadProcessMemory(m_hProcess, lpBaseAddr, lpBuffer, dwSize, &dwBytesRead) == FALSE)
		return FALSE;

	if(dwBytesRead != dwSize)
		return FALSE;

	return TRUE;
}

BOOL CProcessHandler::Write(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPCVOID lpBuffer)
{
	LPVOID		lpBaseAddr = reinterpret_cast<LPVOID>(dwBaseAddr);
	DWORD_PTR	dwBytesWritten;

	if(IsValid() == FALSE)
		return FALSE;

	if(WriteProcessMemory(m_hProcess, lpBaseAddr, lpBuffer, dwSize, &dwBytesWritten) == FALSE)
		return FALSE;

	if(dwBytesWritten != dwSize)
		return FALSE;

	return TRUE;
}

BOOL CProcessHandler::PtrRead(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPVOID lpBuffer)
{
	DWORD	dwBaseAddr;
	BOOL	bResult;

	bResult = Read(dwPtrOfBaseAddr, sizeof(dwBaseAddr), &dwBaseAddr);
	if(bResult)
	{
		bResult = Read(dwBaseAddr + dwOffset, dwSize, lpBuffer);
	}

	return bResult;
}

BOOL CProcessHandler::PtrWrite(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPCVOID lpBuffer)
{
	DWORD	dwBaseAddr;
	BOOL	bResult;

	bResult = Read(dwPtrOfBaseAddr, sizeof(dwBaseAddr), &dwBaseAddr);
	if(bResult)
	{
		bResult = Write(dwBaseAddr + dwOffset, dwSize, lpBuffer);
	}

	return bResult;
}
