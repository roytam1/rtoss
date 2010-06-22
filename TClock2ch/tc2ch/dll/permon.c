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

static PDH_STATUS (WINAPI *pPdhOpenQueryW)(LPCWSTR, DWORD_PTR, PDH_HQUERY*);
static PDH_STATUS (WINAPI *pPdhAddCounterW)(PDH_HQUERY, LPCWSTR, DWORD_PTR, PDH_HCOUNTER*);
static PDH_STATUS (WINAPI *pPdhCollectQueryData)(PDH_HQUERY);
static PDH_STATUS (WINAPI *pPdhGetFormattedCounterValue)(PDH_HCOUNTER, DWORD, LPDWORD, PPDH_FMT_COUNTERVALUE);
static PDH_STATUS (WINAPI *pPdhCloseQuery)(PDH_HQUERY);
static PDH_STATUS (WINAPI *pPdhRemoveCounter)(PDH_HCOUNTER);

extern BOOL bWinNT;
extern LONG GetRegLong(HKEY rootkey, char*subkey, char* entry, LONG defval);

void PerMoni_start(void)
{
	int i;

	if(bWinNT) // WinNT4, 2000
	{
		if(hmodPDH) PerMoni_end();
		hmodPDH = LoadLibrary("pdh.dll");
		if(hmodPDH == NULL) return;

		(FARPROC)pPdhOpenQueryW = GetProcAddress(hmodPDH, "PdhOpenQueryW");
		(FARPROC)pPdhAddCounterW = GetProcAddress(hmodPDH, "PdhAddCounterW");
		(FARPROC)pPdhRemoveCounter = GetProcAddress(hmodPDH, "PdhRemoveCounter");
		(FARPROC)pPdhCollectQueryData = GetProcAddress(hmodPDH, "PdhCollectQueryData");
		(FARPROC)pPdhGetFormattedCounterValue = GetProcAddress(hmodPDH, "PdhGetFormattedCounterValue");
		(FARPROC)pPdhCloseQuery = GetProcAddress(hmodPDH, "PdhCloseQuery");

		if(pPdhOpenQueryW == NULL || pPdhAddCounterW == NULL ||
			pPdhCollectQueryData == NULL || pPdhRemoveCounter == NULL ||
			pPdhGetFormattedCounterValue == NULL || pPdhCloseQuery == NULL)
		{
			FreeLibrary(hmodPDH); hmodPDH = NULL;
			return;
		}

		if(pPdhOpenQueryW(NULL, 0, &hQuery) == ERROR_SUCCESS)
		{
			for(i=0; i<4; i++)
			{
				PDH_STATUS s = pPdhAddCounterW(hQuery, pwszCounter[i], 0, &hCounter[i]) ;
				if(s != ERROR_SUCCESS)
				{
					hQuery = NULL;
					break;
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
					hQuery = NULL;
					break;
				}
			}

			if(hQuery)
			{
				if(pPdhCollectQueryData(hQuery) != ERROR_SUCCESS)
					hQuery = NULL;
			}
		}
		else hQuery = NULL;

		if(hQuery == NULL)
		{
			FreeLibrary(hmodPDH); hmodPDH = NULL;
		}
	}
	else // Win95,98,Me
	{
	}
}

int PerMoni_get(void)
{
	int i;

	if(bWinNT)
	{
		PDH_FMT_COUNTERVALUE FmtValue;

		if(hmodPDH)
		{
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
					CPUUsage[i] = (int)(FmtValue.doubleValue);
				}
				else
				{
					CPUUsage[i] = 0;
				}
			}
		}
	}
	else
	{
	}
	return -1;
}

void PerMoni_end(void)
{
	if(bWinNT)
	{
		if(hmodPDH)
		{
			pPdhCloseQuery(hQuery);
			FreeLibrary(hmodPDH);
		}
		hmodPDH = NULL; hQuery = NULL;
	}
	else
	{
	}
}
