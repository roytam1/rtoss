#pragma once

// utility functions for working with bits
VOID SetBit(LPVOID pFlags, DWORD_PTR dwBitIndex);
VOID ClrBit(LPVOID pFlags, DWORD_PTR dwBitIndex);
BYTE GetBit(LPCVOID pFlags, DWORD_PTR dwBitIndex);

// 0 <= bBitCount <= 32
VOID SetBitField(LPVOID pFlags, DWORD_PTR dwBitIndex, BYTE bBitCount, DWORD dwValue);
DWORD GetBitField(LPCVOID pFlags, DWORD_PTR dwBitIndex, BYTE bBitCount);


class CProcessHandler
{
public:
	CProcessHandler(VOID);
	virtual ~CProcessHandler(VOID);

	/*
		find the first process whose main window has the specified class and window name,
		the handle will be automatically closed, unless it is detached
	*/
	BOOL AttachByWindowName(LPCTSTR lpszClassName, LPCTSTR lpszWindowName);

	/*
		attach the process whose main window is the specified window
		the handle will be automatically closed, unless it is detached
	*/
	BOOL AttachByWindowHandle(HWND hWnd);

	/*
		attach to a process handle
		the handle will be automatically closed, unless it is detached
	*/
	//BOOL AttachProcessHandle(HANDLE hProcess);

	/*
		detach a process handle without closing it,
		return value is the handle of the process
	*/
	//HANDLE DetachProcessHandle(VOID);

	/*
		return value is TRUE, if this object has attached to a valid process handle
	*/
	BOOL IsValid();

	/*
		close the handle
	*/
	VOID Close(VOID);

	BOOL Read(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPVOID lpBuffer);
	BOOL Write(DWORD_PTR dwBaseAddr, DWORD_PTR dwSize, LPCVOID lpBuffer);

	BOOL PtrRead(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPVOID lpBuffer);
	BOOL PtrWrite(DWORD_PTR dwPtrOfBaseAddr, DWORD_PTR dwOffset, DWORD_PTR dwSize, LPCVOID lpBuffer);

private:
	HANDLE		m_hProcess;
	HWND		m_hWnd;
};
