#include <windows.h>
#include <stdio.h>

#define uint64_t unsigned __int64

#define ProcessTimes 4

typedef LONG NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

// Structure layout specifically for ProcessTimes (Class 0)
typedef struct _KERNEL_USER_TIMES {
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
} KERNEL_USER_TIMES, *PKERNEL_USER_TIMES;

// Prototype for the internal ntdll function
typedef NTSTATUS (NTAPI *_NtQueryInformationProcess)(
    HANDLE ProcessHandle,
    ULONG ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
);

NTSTATUS NtQueryInformationProcess(
    HANDLE ProcessHandle,
    ULONG ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
);

BOOL GetProcessTimesNative(HANDLE hProcess, PFILETIME lpCreateTime, PFILETIME lpExitTime, PFILETIME lpKernelTime, PFILETIME lpUserTime) {
    KERNEL_USER_TIMES times = {0};
    ULONG returnLength = 0;

    // Call the native API using the process handle directly
    NTSTATUS status = NtQueryInformationProcess(
        hProcess, 
        ProcessTimes, 
        &times, 
        sizeof(times), 
        &returnLength
    );

    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    // Map the results back to standard Win32 FILETIME structures
    lpCreateTime->dwLowDateTime = times.CreateTime.LowPart;
    lpCreateTime->dwHighDateTime = times.CreateTime.HighPart;
    
    lpExitTime->dwLowDateTime = times.ExitTime.LowPart;
    lpExitTime->dwHighDateTime = times.ExitTime.HighPart;
    
    lpKernelTime->dwLowDateTime = times.KernelTime.LowPart;
    lpKernelTime->dwHighDateTime = times.KernelTime.HighPart;
    
    lpUserTime->dwLowDateTime = times.UserTime.LowPart;
    lpUserTime->dwHighDateTime = times.UserTime.HighPart;

    return TRUE;
}

// Helper to convert FILETIME structure to a 64-bit unsigned integer (in 100-nanosecond units)
uint64_t FileTimeTo64(const FILETIME *ft) {
    ULARGE_INTEGER li;
    li.LowPart = ft->dwLowDateTime;
    li.HighPart = ft->dwHighDateTime;
    return li.QuadPart;
}

// Formats and prints time values in milliseconds
void PrintTime(char* label, uint64_t totalTime, uint64_t partTime) {
    // Converts 100-nanosecond units to milliseconds
    uint64_t totalMs = totalTime / 10000;
    uint64_t partMs = partTime / 10000;
    
    uint64_t percentage = 0;
    if (totalMs != 0) {
        percentage = (partMs * 100) / totalMs;
    }
    
    printf("%-12s = %5I64u.%03I64u = %4I64u%%\n", label, partMs / 1000, partMs % 1000, percentage);
}

int main() {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    LARGE_INTEGER qpcStart, qpcEnd, qpcFreq;
    FILETIME creationTime, exitTime, kernelTime, userTime;
    uint64_t globalTime100Ns, kernelTime64, userTime64, processTime64;

    // Retrieve the raw command line string passed to the application
    LPSTR cmdLine = GetCommandLineA();
    
    // Skip the program's own executable name/path from the command line
    if (*cmdLine == '"') {
        cmdLine++;
        while (*cmdLine && *cmdLine != '"') cmdLine++;
        if (*cmdLine == '"') cmdLine++;
    } else {
        while (*cmdLine && *cmdLine != ' ' && *cmdLine != '\t') cmdLine++;
    }
    while (*cmdLine == ' ' || *cmdLine == '\t') cmdLine++;

    // If no argument (program to time) is provided, display usage instructions
    if (*cmdLine == '\0') {
        printf("Recreated Timer from Igor Pavlov's Timer 9.01 (Public domain, 2009-05-31)\n");
        printf("\nUsage: timer ...\n");
        return 1;
    }

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Query high-resolution performance counter frequencies
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    // Attempt to spawn the targeted user process
    if (!CreateProcessA(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        DWORD err = GetLastError();
        printf("\nCan't find program. Error code = %d\n", err);
        return 1;
    }

    // Wait for the spawned process to completely exit
    WaitForSingleObject(pi.hProcess, INFINITE);
    QueryPerformanceCounter(&qpcEnd);

    // Retrieve timing metrics from the process
    GetProcessTimesNative(pi.hProcess, &creationTime, &exitTime, &kernelTime, &userTime);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Calculate global elapsed real-world time (Wall-clock)
    globalTime100Ns = ((qpcEnd.QuadPart - qpcStart.QuadPart) * 10000000) / qpcFreq.QuadPart;
    
    // Parse time metrics into 64-bit integer values
    kernelTime64 = FileTimeTo64(&kernelTime);
    userTime64 = FileTimeTo64(&userTime);
    processTime64 = kernelTime64 + userTime64;

    // Display formatted results matching binary specifications
    printf("\n");
    
    PrintTime("Kernel Time", globalTime100Ns, kernelTime64);
    PrintTime("User Time", globalTime100Ns, userTime64);
    PrintTime("Process Time", globalTime100Ns, processTime64);
    PrintTime("Global Time", globalTime100Ns, globalTime100Ns);

    return 0;
}
