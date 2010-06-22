/*-------------------------------------------------------------------------
  sysres.c
  GetFreeSystemResources
  Kazubon 1999
  cf. http://www2.justnet.ne.jp/~tyche/samples/sysres.html
      http://ftp.digital.com/pub/published/oreilly/windows/win95.update/k32exp.html
---------------------------------------------------------------------------*/

#include <windows.h>

#if defined(_DEBUG)
# include <crtdbg.h>
#else
# define _ASSERTE(expr) ((void)0)
#endif


#define MAX_PROCESSOR               8

/* for old Platform SDK */
#ifndef PDCAP_WARM_EJECT_SUPPORTED
typedef enum {
    SystemPowerPolicyAc,
    SystemPowerPolicyDc,
    VerifySystemPolicyAc,
    VerifySystemPolicyDc,
    SystemPowerCapabilities,
    SystemBatteryState,
    SystemPowerStateHandler,
    ProcessorStateHandler,
    SystemPowerPolicyCurrent,
    AdministratorPowerPolicy,
    SystemReserveHiberFile,
    ProcessorInformation,
    SystemPowerInformation,
    ProcessorStateHandler2,
    LastWakeTime,                                   // Compare with KeQueryInterruptTime()
    LastSleepTime,                                  // Compare with KeQueryInterruptTime()
    SystemExecutionState,
    SystemPowerStateNotifyHandler,
    ProcessorPowerPolicyAc,
    ProcessorPowerPolicyDc,
    VerifyProcessorPowerPolicyAc,
    VerifyProcessorPowerPolicyDc,
    ProcessorPowerPolicyCurrent,
    SystemPowerStateLogging,
    SystemPowerLoggingEntry
} POWER_INFORMATION_LEVEL;
#endif

typedef HINSTANCE (WINAPI *pfnLoadLibrary16)(LPCSTR);
typedef void (WINAPI *pfnFreeLibrary16)(HINSTANCE);
typedef DWORD (WINAPI *pfnGetProcAddress16)(HINSTANCE, LPCSTR);
typedef void (WINAPI *pfnQT_Thunk)(DWORD);

pfnLoadLibrary16 LoadLibrary16 = NULL;
pfnFreeLibrary16 FreeLibrary16 = NULL;
pfnGetProcAddress16 GetProcAddress16 = NULL;
pfnQT_Thunk QT_Thunk = NULL;

typedef BOOL (WINAPI *pfnGetSystemPowerStatus)(LPSYSTEM_POWER_STATUS);
typedef ULONG (WINAPI *pfnCallNtPowerInformation)(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);
pfnGetSystemPowerStatus pGetSystemPowerStatus = NULL;
pfnCallNtPowerInformation pCallNtPowerInformation = NULL;

static BOOL bInitSysres = FALSE;
static HMODULE hmodKERNEL32 = NULL;
static HMODULE hmodUSER16 = NULL;
static HMODULE hmodPowrprof = NULL;
static DWORD dwNumberOfProcessors = 0;
DWORD dwGetFreeSystemResources = 0;

int blt_h, blt_m, blt_s, pw_mode;

extern int iCPUClock[];

typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

#define ENEWHDR  0x003CL    /* offset of new EXE header */
#define EMAGIC   0x5A4D     /* old EXE magic id:  'MZ'  */
#define PEMAGIC  0x4550     /* NT portable executable */

#define GET_DIR(x)  (hdr->OptionalHeader.DataDirectory[x].VirtualAddress)

/*------------------------------------------------
  get a procedure address in DLL by ordinal
--------------------------------------------------*/
FARPROC GetProcAddressByOrdinal(HMODULE hModule, int ord)
{
#ifdef _WIN64
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(ord);
	_ASSERTE(0);
	return 0;

#else
	IMAGE_NT_HEADERS *hdr;
	IMAGE_EXPORT_DIRECTORY *exp;
	DWORD *AddrFunc;
	WORD enewhdr, *pw;
	BYTE *moddb;

	moddb = (BYTE *)hModule;
	pw = (WORD *) &moddb[0];
	if (*pw != EMAGIC) return 0;
	pw = (WORD *) &moddb[ENEWHDR];
	enewhdr = *pw;
	pw = (WORD *) &moddb[enewhdr];
	if (*pw != PEMAGIC) return 0;
	hdr = (IMAGE_NT_HEADERS *) pw;

	exp = (IMAGE_EXPORT_DIRECTORY *) (((DWORD_PTR) moddb) +
		((DWORD_PTR) GET_DIR(IMAGE_DIRECTORY_ENTRY_EXPORT)));
	AddrFunc = (DWORD *) (moddb + (DWORD) exp->AddressOfFunctions);

	ord--;
	if ((DWORD)ord < exp->NumberOfFunctions)
		return (FARPROC)( (ULONG_PTR)(moddb + AddrFunc[ord]) );
	else return 0;

#endif _WIN64
}

/*----------------------------------------------------
  load 16bit "USER.EXE" and get procedure address of
  "GetFreeSystemResources"
------------------------------------------------------*/
void InitSysres(void)
{
	if(bInitSysres) return;

	bInitSysres = TRUE;

	// if (VERSION != 9x) return;
	if(!(GetVersion() & 0x80000000)) return;

	if(hmodKERNEL32 == NULL)
		hmodKERNEL32 = LoadLibrary("KERNEL32.dll");
	if(hmodKERNEL32 == NULL) return;

	LoadLibrary16 = (pfnLoadLibrary16)GetProcAddressByOrdinal(hmodKERNEL32, 35);
	FreeLibrary16 = (pfnFreeLibrary16)GetProcAddressByOrdinal(hmodKERNEL32, 36);
	GetProcAddress16 = (pfnGetProcAddress16)GetProcAddressByOrdinal(hmodKERNEL32, 37);
	QT_Thunk = (pfnQT_Thunk)GetProcAddress(hmodKERNEL32, "QT_Thunk");
	if(LoadLibrary16 == NULL || FreeLibrary16 == NULL ||
		GetProcAddress16 == NULL || QT_Thunk == NULL)
	{
		FreeLibrary(hmodKERNEL32); hmodKERNEL32 = NULL; return;
	}

	hmodUSER16 = LoadLibrary16("USER.EXE");
	if(hmodUSER16 < (HMODULE)32)
	{
		FreeLibrary(hmodKERNEL32); hmodKERNEL32 = NULL;
		hmodUSER16 = NULL; return;
	}

	dwGetFreeSystemResources = GetProcAddress16(hmodUSER16,
		"GetFreeSystemResources");
	if(dwGetFreeSystemResources == 0)
	{
		FreeLibrary(hmodKERNEL32); hmodKERNEL32 = NULL;
		FreeLibrary16(hmodUSER16); hmodUSER16 = NULL;
	}
}

/*----------------------------------------------------
  free libraries
------------------------------------------------------*/
void EndSysres(void)
{
	if(hmodKERNEL32) FreeLibrary(hmodKERNEL32); hmodKERNEL32 = NULL;
	if(hmodUSER16) FreeLibrary16(hmodUSER16); hmodUSER16 = NULL;
}

/*----------------------------------------------------
  call 16bit "GetFreeSystemResources" by Thunk-down
------------------------------------------------------*/
int GetFreeSystemResources(WORD wSysRes)
{
#ifdef _WIN64 
	UNREFERENCED_PARAMETER(wSysRes);
	_ASSERTE(0);
	return 0;

#else 
	volatile char dummy[64];
	WORD wTmp;

	dummy[0] = 0;

	if(dwGetFreeSystemResources == 0) InitSysres();
	if(dwGetFreeSystemResources == 0) return 0;

#ifdef _MSC_VER
	__asm {
		push wSysRes
		mov edx, dwGetFreeSystemResources
		call QT_Thunk
		mov wTmp, ax
	}
#else
	{
		extern WORD asm_QT_Thunk(WORD p1);
		wTmp = asm_QT_Thunk(wSysRes);
	}
#endif // _MSC_VER

	return (int)wTmp;

#endif // _WIN64
}

void InitBatteryLife(void)
{
	DWORD ver;

	ver = GetVersion();
	if((!(ver & 0x80000000) && LOBYTE(LOWORD(ver)) < 5)) // NT 4
		return;

	if(hmodKERNEL32 == NULL)
		hmodKERNEL32 = LoadLibrary("KERNEL32.dll");
	if(hmodKERNEL32 == NULL) return;

	pGetSystemPowerStatus = (pfnGetSystemPowerStatus)
		GetProcAddress(hmodKERNEL32, "GetSystemPowerStatus");
}

int GetBatteryLifePercent(void)
{
	SYSTEM_POWER_STATUS sps;

	if(pGetSystemPowerStatus == NULL) InitBatteryLife();

	if(pGetSystemPowerStatus)
	{
		if(pGetSystemPowerStatus(&sps))
		{
			BYTE b;
			DWORD blt;

			b = sps.BatteryLifePercent;
			blt = sps.BatteryLifeTime;
			pw_mode = (int)sps.ACLineStatus;

			if((int)blt == -1)
			{
				blt_h = 99;
				blt_m = 99;
				blt_s = 99;
			}
			else
			{
				blt_h = (int)blt/3600;
				blt_m = ((int)blt - blt_h*3600)/60;
				blt_s = ((int)blt - blt_h*3600 - blt_m*60);
			}

			return (int)b;
		}
		else return 255;
	}
	return 255;
}

void FreeBatteryLife(void)
{
	if(hmodKERNEL32) FreeLibrary(hmodKERNEL32);
	hmodKERNEL32 = NULL;
	pGetSystemPowerStatus = NULL;
}

void InitCpuClock(void)
{
	DWORD ver;
	SYSTEM_INFO si;

	ver = GetVersion();
	if((!(ver & 0x80000000) && LOBYTE(LOWORD(ver)) < 5)) // NT 4
		return;

	if(hmodPowrprof == NULL)
		hmodPowrprof = LoadLibrary("Powrprof.dll");
	if(hmodPowrprof == NULL) return;

	pCallNtPowerInformation = (pfnCallNtPowerInformation)
		GetProcAddress(hmodPowrprof, "CallNtPowerInformation");

	GetSystemInfo(&si);
	dwNumberOfProcessors = si.dwNumberOfProcessors;
}


void UpdateCpuClock(void)
{
	if(pCallNtPowerInformation == NULL) InitCpuClock();

	if(pCallNtPowerInformation)
	{
		PROCESSOR_POWER_INFORMATION ppi[MAX_PROCESSOR];
		ULONG status;	// NTSTATUS
		DWORD i;

		status = pCallNtPowerInformation(ProcessorInformation, NULL, 0, &ppi[0], sizeof(ppi));
		if (status != 0) return; // != STATUS_SUCCESS

		for (i=0; i<dwNumberOfProcessors; ++i)
		{
			iCPUClock[i] = ppi[i].CurrentMhz;
		}
	}
}


void FreeCpuClock(void)
{
	if(hmodPowrprof) FreeLibrary(hmodPowrprof);
	hmodPowrprof = NULL;
	pCallNtPowerInformation = NULL;
}

