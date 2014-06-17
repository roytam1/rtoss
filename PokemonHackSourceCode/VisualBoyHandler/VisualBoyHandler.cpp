#include "windows.h"
#include "ProcessHandler.h"
#include "VisualBoyHandler.h"

#ifndef lstrncmp
#ifndef UNICODE
#define	lstrncmp strncmp
#else
#define lstrncmp wcsncmp
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD_PTR CVisualBoyHandler::CheckPtrStruct(LPCVOID lpBuffer, DWORD_PTR dwSize)
{
	DWORD_PTR	dwOffset;
	BOOL		bFound = FALSE;

	dwSize -= sizeof(VBA_PtrStruct);
	for(dwOffset = 0; dwOffset <= dwSize; dwOffset += sizeof(DWORD))
	{
		CONST VBA_PtrStruct *pps = 
			reinterpret_cast<CONST VBA_PtrStruct *>(reinterpret_cast<LPCBYTE>(lpBuffer) + dwOffset);

		if(	pps->peBIOS.dwSize		== 0x00003FFF &&
			pps->peWRAM.dwSize		== 0x0003FFFF &&
			pps->peIRAM.dwSize		== 0x00007FFF &&
	//		pps->peIO.dwSize		== 0x000003FF &&	// doesn't follow GBA Specification
			pps->pePalette.dwSize	== 0x000003FF &&
	//		pps->peVRAM.dwSize		== 0x0001FFFF &&	// doesn't follow GBA Specification
			pps->peOAM.dwSize		== 0x000003FF &&
			pps->peROM.dwSize		>= 0x01FFFFFF)
		{
			bFound = TRUE;
			break;
		}
	}

	return bFound ? dwOffset : (-1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#define SEARCH_START	0x00500000
#define SEARCH_RANGE	0x00080000	// 512KB
#define SEARCH_GUARD	0x00000100
#define SEARCH_COUNT	8			// 8 x 512KB

CVisualBoyHandler::CVisualBoyHandler(VOID)
{
	m_dwAddressOfPtrStruct = NULL;
}

CVisualBoyHandler::~CVisualBoyHandler(VOID)
{
}

BOOL CVisualBoyHandler::SearchForPtrStruct(VOID)
{
	BOOL		bResult = IsValid();
	DWORD_PTR	dwCount, dwStartAddress, dwBufferSize, dwOffset;

	if(bResult && m_dwAddressOfPtrStruct == NULL)
	{
		LPBYTE	pbBuffer = new BYTE[SEARCH_RANGE + SEARCH_GUARD];
		if(pbBuffer != NULL)
		{
			for(dwCount = 0; dwCount < SEARCH_COUNT; ++dwCount)
			{
				// steamlessly searching
				if(dwCount == 0)
				{
					dwStartAddress = SEARCH_START + dwCount * SEARCH_RANGE;
					dwBufferSize = SEARCH_RANGE;
				}
				else
				{
					dwStartAddress = SEARCH_START + dwCount * SEARCH_RANGE - SEARCH_GUARD;
					dwBufferSize = SEARCH_RANGE + SEARCH_GUARD;
				}

				bResult = CProcessHandler::Read(dwStartAddress, dwBufferSize, pbBuffer);

				if(bResult)
				{
					dwOffset = CheckPtrStruct(pbBuffer, dwBufferSize);
					if(dwOffset < dwBufferSize)
					{
						m_dwAddressOfPtrStruct = dwStartAddress + dwOffset;
						break;
					}
				}
			}

			delete []pbBuffer;
		}
	}

	if(bResult && m_dwAddressOfPtrStruct != NULL)
	{
		bResult = CProcessHandler::Read(m_dwAddressOfPtrStruct, sizeof(m_PtrStruct), &m_PtrStruct);
	}

	if(!bResult || m_dwAddressOfPtrStruct == NULL)
	{
		Close();
		m_dwAddressOfPtrStruct = NULL;
	}

	return bResult;
}

BOOL CALLBACK CVisualBoyHandler::EnumVBAWindowProc(HWND hWnd, LPARAM lParam)
{
	static CONST TCHAR	szName[] = TEXT("VisualBoyAdvance");
	static CONST DWORD	cchNameLength = sizeof(szName) / sizeof(szName[0]);

	static CONST TCHAR	szNameSpeed[] = TEXT("VisualBoyAdvance-nnn%");
	static CONST DWORD	cchNameSpeedLength = sizeof(szNameSpeed) / sizeof(szNameSpeed[0]);

	TCHAR	szWindowName[cchNameSpeedLength];
	DWORD	cchWindowName;
	BOOL	bContinue = TRUE;

	cchWindowName = GetWindowText(hWnd, szWindowName, cchNameSpeedLength);

	if(cchWindowName >= cchNameLength - 1)
	{
		if(lstrncmp(szWindowName, szName, cchNameLength - 1) == 0)
		{
			if(cchWindowName >= cchNameLength)
			{
				if(szWindowName[cchNameLength - 1] == TEXT('-'))
				{
					bContinue = FALSE;
				}
			}
			else
			{
				bContinue = FALSE;
			}
		}
	}

	if(bContinue == FALSE)
	{
		*reinterpret_cast<HWND *>(lParam) = hWnd;
	}

	return bContinue;
}

BOOL CVisualBoyHandler::Attach(VOID)
{
	BOOL	bResult;
	HWND	hWnd = NULL;

	m_dwAddressOfPtrStruct = NULL;
	bResult = EnumWindows(EnumVBAWindowProc, reinterpret_cast<LPARAM>(&hWnd));

	if(hWnd == NULL)
	{
		bResult = FALSE;
	}
	else
	{
		bResult = CProcessHandler::AttachByWindowHandle(hWnd);
	}

	if(bResult)
	{
		bResult = SearchForPtrStruct();
	}

	return bResult;
}

BOOL CVisualBoyHandler::RefreshAddressList(VOID)
{
	BOOL	bResult = IsValid();

	if(!bResult)
	{
		bResult = Attach();
	}
	else
	{
		bResult = SearchForPtrStruct();
	}

	return bResult;
}

BOOL CVisualBoyHandler::Read(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPVOID lpBuffer)
{
	CONST VBA_PtrEntry *ppe = reinterpret_cast<CONST VBA_PtrEntry *>(&m_PtrStruct);
	DWORD_PTR	dwIndex = (dwBaseAddr & 0x0F000000) >> 24;
	DWORD_PTR	dwOffset = dwBaseAddr & 0x00FFFFFF;

	if(dwOffset + dwSize <= ppe[dwIndex].dwSize)
	{
		return CProcessHandler::Read(ppe[dwIndex].dwPointer + dwOffset, dwSize, lpBuffer);
	}
	else
	{
		return FALSE;
	}
}

BOOL CVisualBoyHandler::ReadRom(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPVOID lpBuffer)
{
	CONST VBA_PtrEntry *ppe = reinterpret_cast<CONST VBA_PtrEntry *>(&m_PtrStruct);
	DWORD_PTR	dwIndex = 0x08000000 >> 24;	// rom
	DWORD_PTR	dwOffset = dwBaseAddr & 0x00FFFFFF;

	if(dwOffset + dwSize <= ppe[dwIndex].dwSize)
	{
		return CProcessHandler::Read(ppe[dwIndex].dwPointer + dwOffset, dwSize, lpBuffer);
	}
	else
	{
		return FALSE;
	}
}

BOOL CVisualBoyHandler::Write(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPCVOID lpBuffer)
{
	CONST VBA_PtrEntry *ppe = reinterpret_cast<CONST VBA_PtrEntry *>(&m_PtrStruct);
	DWORD_PTR	dwIndex = (dwBaseAddr & 0x0F000000) >> 24;
	DWORD_PTR	dwOffset = dwBaseAddr & 0x00FFFFFF;

	if(dwOffset + dwSize <= ppe[dwIndex].dwSize)
	{
		return CProcessHandler::Write(ppe[dwIndex].dwPointer + dwOffset, dwSize, lpBuffer);
	}
	else
	{
		return FALSE;
	}
}

BOOL CVisualBoyHandler::PtrRead(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPVOID lpBuffer)
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

BOOL CVisualBoyHandler::PtrWrite(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPCVOID lpBuffer)
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
