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

typedef enum _SYSTEM_INFORMATION_CLASS 
{
    SystemBasicInformation = 0,
    SystemProcessorInformation = 1,             // obsolete...delete
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemPathInformation = 4,
    SystemProcessInformation = 5,
    SystemCallCountInformation = 6,
    SystemDeviceInformation = 7,
    SystemProcessorPerformanceInformation = 8,
    SystemFlagsInformation = 9,
    SystemCallTimeInformation = 10,
    SystemModuleInformation = 11,
    SystemLocksInformation = 12,
    SystemStackTraceInformation = 13,
    SystemPagedPoolInformation = 14,
    SystemNonPagedPoolInformation = 15,
    SystemHandleInformation = 16,
    SystemObjectInformation = 17,
    SystemPageFileInformation = 18,
    SystemVdmInstemulInformation = 19,
    SystemVdmBopInformation = 20,
    SystemFileCacheInformation = 21,
    SystemPoolTagInformation = 22,
    SystemInterruptInformation = 23,
    SystemDpcBehaviorInformation = 24,
    SystemFullMemoryInformation = 25,
    SystemLoadGdiDriverInformation = 26,
    SystemUnloadGdiDriverInformation = 27,
    SystemTimeAdjustmentInformation = 28,
    SystemSummaryMemoryInformation = 29,
    SystemMirrorMemoryInformation = 30,
    SystemPerformanceTraceInformation = 31,
    SystemObsolete0 = 32,
    SystemExceptionInformation = 33,
    SystemCrashDumpStateInformation = 34,
    SystemKernelDebuggerInformation = 35,
    SystemContextSwitchInformation = 36,
    SystemRegistryQuotaInformation = 37,
    SystemExtendServiceTableInformation = 38,
    SystemPrioritySeperation = 39,
    SystemVerifierAddDriverInformation = 40,
    SystemVerifierRemoveDriverInformation = 41,
    SystemProcessorIdleInformation = 42,
    SystemLegacyDriverInformation = 43,
    SystemCurrentTimeZoneInformation = 44,
    SystemLookasideInformation = 45,
    SystemTimeSlipNotification = 46,
    SystemSessionCreate = 47,
    SystemSessionDetach = 48,
    SystemSessionInformation = 49,
    SystemRangeStartInformation = 50,
    SystemVerifierInformation = 51,
    SystemVerifierThunkExtend = 52,
    SystemSessionProcessInformation = 53,
    SystemLoadGdiDriverInSystemSpace = 54,
    SystemNumaProcessorMap = 55,
    SystemPrefetcherInformation = 56,
    SystemExtendedProcessInformation = 57,
    SystemRecommendedSharedDataAlignment = 58,
    SystemComPlusPackage = 59,
    SystemNumaAvailableMemory = 60,
    SystemProcessorPowerInformation = 61,
    SystemEmulationBasicInformation = 62,
    SystemEmulationProcessorInformation = 63,
    SystemExtendedHandleInformation = 64,
    SystemLostDelayedWriteInformation = 65,
    SystemBigPoolInformation = 66,
    SystemSessionPoolTagInformation = 67,
    SystemSessionMappedViewInformation = 68,
    SystemHotpatchInformation = 69,
    SystemObjectSecurityMode = 70,
    SystemWatchdogTimerHandler = 71,
    SystemWatchdogTimerInformation = 72,
    SystemLogicalProcessorInformation = 73,
    SystemWow64SharedInformation = 74,
    SystemRegisterFirmwareTableInformationHandler = 75,
    SystemFirmwareTableInformation = 76,
    SystemModuleInformationEx = 77,
    SystemVerifierTriageInformation = 78,
    SystemSuperfetchInformation = 79,
    SystemMemoryListInformation = 80,
    SystemFileCacheInformationEx = 81,
    MaxSystemInfoClass = 82  // MaxSystemInfoClass should always be the last enum

} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG InterruptCount;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef NTSTATUS (WINAPI* pfnQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

extern "C" FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
extern "C" HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
extern "C" void WINAPI SetLastError(DWORD dwErrCode);
extern "C" void WINAPI GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);
extern "C" HANDLE WINAPI GetProcessHeap();
extern "C" LPVOID WINAPI HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
extern "C" BOOL WINAPI HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);

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

BOOL
WINAPI
GetSystemTimes(OUT LPFILETIME lpIdleTime OPTIONAL,
               OUT LPFILETIME lpKernelTime OPTIONAL,
               OUT LPFILETIME lpUserTime OPTIONAL)
{
    PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ProcPerfInfo;
    LARGE_INTEGER TotalUserTime, TotalKernTime, TotalIdleTime;
    ULONG BufferSize, ReturnLength;
    CCHAR i;
    NTSTATUS Status;
    SYSTEM_INFO SystemInfo;
    BOOL somethingWrong = FALSE;
    static DWORD dwNumberOfProcessors = 0;
    static pfnQuerySystemInformation ntQSI = (pfnQuerySystemInformation) GetProcAddress(GetModuleHandleA("NTDLL.DLL"),"NtQuerySystemInformation");

    TotalUserTime.QuadPart = TotalKernTime.QuadPart = TotalIdleTime.QuadPart = 0;

    if(!dwNumberOfProcessors) {
        GetSystemInfo(&SystemInfo);
        dwNumberOfProcessors = SystemInfo.dwNumberOfProcessors;
    }

    BufferSize = sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) *
                 dwNumberOfProcessors;

    ProcPerfInfo = (PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)HeapAlloc(GetProcessHeap(), 0, BufferSize);
    if (!ProcPerfInfo)
    {
        return FALSE;
    }

    Status = ntQSI(SystemProcessorPerformanceInformation,
                                      ProcPerfInfo,
                                      BufferSize,
                                      &ReturnLength);
    if ((NT_SUCCESS(Status)) && (ReturnLength == BufferSize))
    {
        if (lpIdleTime)
        {
            for (i = 0; i < dwNumberOfProcessors; i++)
            {
                TotalIdleTime.QuadPart += ProcPerfInfo[i].IdleTime.QuadPart;
            }

            lpIdleTime->dwLowDateTime = TotalIdleTime.LowPart;
            lpIdleTime->dwHighDateTime = TotalIdleTime.HighPart;
        }

        if (lpKernelTime)
        {
            for (i = 0; i < dwNumberOfProcessors; i++)
            {
                TotalKernTime.QuadPart += ProcPerfInfo[i].KernelTime.QuadPart;
            }

            lpKernelTime->dwLowDateTime = TotalKernTime.LowPart;
            lpKernelTime->dwHighDateTime = TotalKernTime.HighPart;
        }

        if (lpUserTime)
        {
            for (i = 0; i < dwNumberOfProcessors; i++)
            {
                TotalUserTime.QuadPart += ProcPerfInfo[i].UserTime.QuadPart;
            }

            lpUserTime->dwLowDateTime = TotalUserTime.LowPart;
            lpUserTime->dwHighDateTime = TotalUserTime.HighPart;
        }
    }
    else if (NT_SUCCESS(Status))
    {
         somethingWrong = TRUE;
    }

    HeapFree(GetProcessHeap(), 0, ProcPerfInfo);
    if (!NT_SUCCESS(Status))
    {
        return FALSE;
    }

    if (somethingWrong)
    {
        return FALSE;
    }

    return TRUE;
}
