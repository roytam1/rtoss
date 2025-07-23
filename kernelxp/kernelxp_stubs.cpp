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

typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
    RelationProcessorCore,
    RelationNumaNode,
    RelationCache,
    RelationProcessorPackage,
    RelationGroup,
    RelationAll=0xffff
} LOGICAL_PROCESSOR_RELATIONSHIP;

typedef enum _PROCESSOR_CACHE_TYPE {
    CacheUnified,
    CacheInstruction,
    CacheData,
    CacheTrace
} PROCESSOR_CACHE_TYPE;

typedef struct _CACHE_DESCRIPTOR {
    BYTE Level;
    BYTE Associativity;
    WORD LineSize;
    DWORD Size;
    PROCESSOR_CACHE_TYPE Type;
} CACHE_DESCRIPTOR, *PCACHE_DESCRIPTOR;

typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
    ULONG_PTR ProcessorMask;
    LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
    union {
        struct {
            BYTE Flags;
        } ProcessorCore;
        struct {
            DWORD NodeNumber;
        } NumaNode;
        CACHE_DESCRIPTOR Cache;
        ULONGLONG Reserved[2];
    } DUMMYUNIONNAME;
} SYSTEM_LOGICAL_PROCESSOR_INFORMATION, *PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;

typedef DWORD (WINAPI* pfnGetLogicalProcessorInformation)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer, PDWORD ReturnedLength);

extern "C" FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
extern "C" HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
extern "C" void WINAPI SetLastError(DWORD dwErrCode);

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

BOOL WINAPI GetLogicalProcessorInformation(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer, PDWORD ReturnedLength)
{
    static pfnGetLogicalProcessorInformation getLPI  = (pfnGetLogicalProcessorInformation) GetProcAddress(GetModuleHandleA("KERNEL32.DLL"),"GetLogicalProcessorInformation");

    if (getLPI)
        return getLPI(Buffer, ReturnedLength);
    else
    {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
}
