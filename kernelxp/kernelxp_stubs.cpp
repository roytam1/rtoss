#include <windows.h>

#define PROCESSINFOCLASS DWORD
#define NTSTATUS DWORD
#define PPEB DWORD
#define KPRIORITY DWORD
#define ProcessBasicInformation 0

typedef struct _PROCESS_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PPEB PebBaseAddress;
    ULONG_PTR AffinityMask;
    KPRIORITY BasePriority;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

typedef DWORD (WINAPI* pfnGetProcID)(HANDLE h);

typedef NTSTATUS (WINAPI* pfnQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength);

extern "C" FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
extern "C" HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);

// stubs/wrapper belows

PVOID
WINAPI
EncodePointer(IN PVOID Pointer)
{
    return Pointer;
}

PVOID
WINAPI
DecodePointer(IN PVOID Pointer)
{
    return EncodePointer(Pointer);
}

DWORD WINAPI GetProcessId(HANDLE h)
{
    static pfnQueryInformationProcess ntQIP = (pfnQueryInformationProcess) GetProcAddress(GetModuleHandleA("NTDLL.DLL"),"NtQueryInformationProcess");
    static pfnGetProcID getPId  = (pfnGetProcID) GetProcAddress(GetModuleHandleA("KERNEL32.DLL"),"GetProcessId");

    if (getPId != NULL)
        return getPId(h);
    else
    {
        PROCESS_BASIC_INFORMATION info;
        ULONG returnSize;
        ntQIP(h, ProcessBasicInformation, &info, sizeof(info), &returnSize);  // Get basic information.
        return info.UniqueProcessId;
    }
}

BOOL WINAPI SetDllDirectoryW(LPCWSTR lpPathName)
{
    return TRUE;
}

BOOL WINAPI GetProcessHandleCount(HANDLE hProcess, PDWORD pdwHandleCount)
{
    return FALSE;
}
