#pragma once

struct VBA_PtrEntry
{
	DWORD	dwPointer;
	DWORD	dwSize;
};

struct VBA_PtrStruct
{
	VBA_PtrEntry	peBIOS;
	VBA_PtrEntry	peConst;
	VBA_PtrEntry	peWRAM;
	VBA_PtrEntry	peIRAM;
	VBA_PtrEntry	peIO;
	VBA_PtrEntry	pePalette;
	VBA_PtrEntry	peVRAM;
	VBA_PtrEntry	peOAM;
	VBA_PtrEntry	peROM;
};


// specified for VisualBoyAdvance 1.61a or above
class CVisualBoyHandler : public CProcessHandler
{
protected:
	DWORD_PTR		m_dwAddressOfPtrStruct;
	VBA_PtrStruct	m_PtrStruct;

	/*
		if the vba_ptrstruct is found,
		return value is the byte offset inside the lpBuffer
		otherwise, return value is dwSize
	*/
	static DWORD_PTR CheckPtrStruct(LPCVOID lpBuffer, DWORD_PTR dwSize);
	static BOOL CALLBACK EnumVBAWindowProc(HWND hWnd, LPARAM lParam);
	BOOL SearchForPtrStruct(VOID);

public:
	CVisualBoyHandler(VOID);
	virtual ~CVisualBoyHandler(VOID);

	BOOL Attach(VOID);
	BOOL RefreshAddressList(VOID);

	BOOL Read(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPVOID lpBuffer);
	BOOL ReadRom(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPVOID lpBuffer);
	BOOL Write(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPCVOID lpBuffer);

	BOOL PtrRead(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPVOID lpBuffer);
	BOOL PtrWrite(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPCVOID lpBuffer);
};
