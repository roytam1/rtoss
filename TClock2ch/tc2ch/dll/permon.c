/*-------------------------------------------------------------------------
  permon.c
  get performance monitor counter
  Kazubon 2001
---------------------------------------------------------------------------*/

#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#define MAX_PROCESSOR               8

void PerMoni_start(void);
int PerMoni_get(void);
void PerMoni_end(void);

int permon[4] = { 0, 0, 0, 0 };

// cpu usage
int CPUUsage[MAX_PROCESSOR] = { 0 };
static PDH_HCOUNTER hCPUCounter[MAX_PROCESSOR] = { NULL };
// cpu total usage
int totalCPUUsage = 0;
static PDH_HCOUNTER hTotalCPUCounter = NULL;


static HMODULE hmodPDH = NULL;
static PDH_HQUERY hQuery = NULL;
static PDH_HCOUNTER hCounter[4] = { NULL, NULL, NULL, NULL };

static wchar_t *pwszCounter[4] =
{
	L"\\Network Interface(MS TCP Loopback interface)\\Bytes Sent/sec",
	L"\\Network Interface(MS TCP Loopback interface)\\Bytes Received/sec",
	L"\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Read",
	L"\\PhysicalDisk(_Total)\\Avg. Disk Bytes/Write",
};

typedef PDH_STATUS (WINAPI *pfnPdhOpenQueryW)(LPCWSTR, DWORD_PTR, PDH_HQUERY*);
typedef PDH_STATUS (WINAPI *pfnPdhAddCounterW)(PDH_HQUERY, LPCWSTR, DWORD_PTR, PDH_HCOUNTER*);
typedef PDH_STATUS (WINAPI *pfnPdhCollectQueryData)(PDH_HQUERY);
typedef PDH_STATUS (WINAPI *pfnPdhGetFormattedCounterValue)(PDH_HCOUNTER, DWORD, LPDWORD, PPDH_FMT_COUNTERVALUE);
typedef PDH_STATUS (WINAPI *pfnPdhCloseQuery)(PDH_HQUERY);
typedef PDH_STATUS (WINAPI *pfnPdhRemoveCounter)(PDH_HCOUNTER);
static pfnPdhOpenQueryW pPdhOpenQueryW;
static pfnPdhAddCounterW pPdhAddCounterW;
static pfnPdhCollectQueryData pPdhCollectQueryData;
static pfnPdhGetFormattedCounterValue pPdhGetFormattedCounterValue;
static pfnPdhCloseQuery pPdhCloseQuery;
static pfnPdhRemoveCounter pPdhRemoveCounter;

extern BOOL bWinNT;
extern LONG GetRegLong(HKEY rootkey, char*subkey, char* entry, LONG defval);

void PerMoni_start(void)
{
	int i;

	if(!bWinNT) 	
	{
		// Win95,98,Me
		return;
	}

	// WinNT4, 2000
	if(hmodPDH) PerMoni_end();
	hmodPDH = LoadLibrary("pdh.dll");
	if(hmodPDH == NULL) return;

	pPdhOpenQueryW               = (pfnPdhOpenQueryW)GetProcAddress(hmodPDH, "PdhOpenQueryW");
	pPdhAddCounterW              = (pfnPdhAddCounterW)GetProcAddress(hmodPDH, "PdhAddCounterW");
	pPdhRemoveCounter            = (pfnPdhRemoveCounter)GetProcAddress(hmodPDH, "PdhRemoveCounter");
	pPdhCollectQueryData         = (pfnPdhCollectQueryData)GetProcAddress(hmodPDH, "PdhCollectQueryData");
	pPdhGetFormattedCounterValue = (pfnPdhGetFormattedCounterValue)GetProcAddress(hmodPDH, "PdhGetFormattedCounterValue");
	pPdhCloseQuery               = (pfnPdhCloseQuery)GetProcAddress(hmodPDH, "PdhCloseQuery");

	if(pPdhOpenQueryW == NULL || 
		pPdhAddCounterW == NULL ||
		pPdhCollectQueryData == NULL || 
		pPdhRemoveCounter == NULL ||
		pPdhGetFormattedCounterValue == NULL || 
		pPdhCloseQuery == NULL)
	{
		goto FAILURE_PDH_COUNTER_INITIALIZATION;
	}

	if(pPdhOpenQueryW(NULL, 0, &hQuery) != ERROR_SUCCESS)
	{
		goto FAILURE_PDH_COUNTER_INITIALIZATION;
	}
	
	for(i=0; i<4; i++)
	{
		PDH_STATUS s = pPdhAddCounterW(hQuery, pwszCounter[i], 0, &hCounter[i]) ;
		if(s != ERROR_SUCCESS)
		{
			goto FAILURE_PDH_COUNTER_INITIALIZATION;
		}
		
		s = pPdhRemoveCounter(hCounter[i]);
	}

	// create cpu counter
	for(i=0; i<MAX_PROCESSOR; i++)
	{
		wchar_t counterName[64];
		wsprintfW(counterName, L"\\Processor(%d)\\%% Processor Time", i);

		if(pPdhAddCounterW(hQuery, counterName, 0, &hCPUCounter[i]) != ERROR_SUCCESS)
		{
			goto FAILURE_PDH_COUNTER_INITIALIZATION;
		}
	}

	// create total cpu usage counter
	if(pPdhAddCounterW(hQuery, L"\\Processor(_Total)\\% Processor Time", 
						0, &hTotalCPUCounter) != ERROR_SUCCESS)
	{
		goto FAILURE_PDH_COUNTER_INITIALIZATION;
	}

	if(pPdhCollectQueryData(hQuery) != ERROR_SUCCESS)
	{
		goto FAILURE_PDH_COUNTER_INITIALIZATION;
	}

	return; /* SUCCESS */


FAILURE_PDH_COUNTER_INITIALIZATION:
	hQuery = NULL;
	FreeLibrary(hmodPDH); hmodPDH = NULL;
	return; /* FAILURE */
}

int PerMoni_get(void)
{
	int i;

	if(!bWinNT)
	{
		return -1;
	}

	if(hmodPDH)
	{
		PDH_FMT_COUNTERVALUE FmtValue;

		pPdhCollectQueryData(hQuery);

		for(i=0; i<4; i++)
		{
			if(pPdhGetFormattedCounterValue(hCounter[i], PDH_FMT_DOUBLE, NULL, &FmtValue) == ERROR_SUCCESS)
			{
				permon[i] = (int)(FmtValue.doubleValue/1024);
			}
			else
			{
				permon[i] = 0;
			}
		}

		// get cpu counter
		for(i=0; i<MAX_PROCESSOR; i++)
		{
			if(pPdhGetFormattedCounterValue(hCPUCounter[i], PDH_FMT_DOUBLE, NULL, &FmtValue) == ERROR_SUCCESS)
			{
				CPUUsage[i] = (int)(FmtValue.doubleValue +0.5);
			}
			else
			{
				CPUUsage[i] = 0;
			}
		}

		// get total cpu usage
		if(pPdhGetFormattedCounterValue(hTotalCPUCounter, PDH_FMT_DOUBLE, NULL, &FmtValue) == ERROR_SUCCESS)
		{
			totalCPUUsage = (int)(FmtValue.doubleValue +0.5);
		}
		else
		{
			totalCPUUsage = 0;
		}
	}
	return -1;
}

void PerMoni_end(void)
{
	if(!bWinNT)
	{
		return;
	}
	
	if(hmodPDH)
	{
		pPdhCloseQuery(hQuery);
		FreeLibrary(hmodPDH);
	}
	hmodPDH = NULL; hQuery = NULL;
}
