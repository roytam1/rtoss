/*-------------------------------------------------------------------------
  net.c
  get network interface info
---------------------------------------------------------------------------*/

#include <windows.h>
#include <iprtrmib.h>

void Net_start(void);
void Net_get(void);
void Net_end(void);

static HMODULE hmodIPHLP = NULL;
static HGLOBAL *buffer = NULL;
static MIB_IFROW *ifr;
static MIB_IFTABLE *ift;
static int count;
static int sec = 5;
double net[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

typedef DWORD (WINAPI *pfnGetIfTable)(PMIB_IFTABLE,PULONG,BOOL);
typedef DWORD (WINAPI *pfnGetIfEntry)(PMIB_IFROW);
static pfnGetIfTable pGetIfTable;
static pfnGetIfEntry pGetIfEntry;

extern LONG GetMyRegLong(char* section, char* entry, LONG defval);


void Net_start(void)
{
	DWORD bufsize;
	DWORD iftable;
	int i;

	if(hmodIPHLP) Net_end();
	ifr = NULL;
	ift = NULL;

	hmodIPHLP = LoadLibrary("iphlpapi.dll");
	if(hmodIPHLP == NULL) return;

	pGetIfTable = (pfnGetIfTable)GetProcAddress(hmodIPHLP, "GetIfTable");
	pGetIfEntry = (pfnGetIfEntry)GetProcAddress(hmodIPHLP, "GetIfEntry");


	if(pGetIfTable == NULL || pGetIfEntry == NULL)
	{
		FreeLibrary(hmodIPHLP); hmodIPHLP = NULL;
		return;
	}

	bufsize = 0;
	iftable = pGetIfTable((PMIB_IFTABLE)buffer, &bufsize, TRUE);
	if(iftable != ERROR_INSUFFICIENT_BUFFER)
	{
//		FreeLibrary(hmodIPHLP); hmodIPHLP = NULL;
		return;
	}
	buffer = GlobalAlloc(GPTR, bufsize);
	if(buffer == NULL)
	{
//		FreeLibrary(hmodIPHLP); hmodIPHLP = NULL;
		return;
	}
	iftable = pGetIfTable((PMIB_IFTABLE)buffer, &bufsize, TRUE);
	if(iftable != NO_ERROR)
	{
		GlobalFree(buffer); buffer = NULL;
//		FreeLibrary(hmodIPHLP); hmodIPHLP = NULL;
		return;
	}

	ift = (MIB_IFTABLE *)buffer;
	count = ift->dwNumEntries;

	net[0] = 0;
	net[1] = 0;
	net[4] = 0;
	net[5] = 0;
	net[8] = 0;
	net[9] = 0;
	for(i=0; i<count; i++)
	{
		ifr = (ift->table) + i;
		if(ifr->dwType == MIB_IF_TYPE_ETHERNET ||
		   ifr->dwType == MIB_IF_TYPE_PPP)
		{
			net[0] += ifr->dwInOctets;
			net[1] += ifr->dwOutOctets;
			net[4] += net[0]/1024;
			net[5] += net[1]/1024;
			net[8] += net[0]/1048576;
			net[9] += net[1]/1048576;
		}
	}
	sec = (int)GetMyRegLong(NULL, "IntervalSysInfo", 5);
	if(sec <= 0 || 60 < sec) sec = 5;
}


void Net_getRecvSend(double* recv,double* send)
{
	if(ift && hmodIPHLP)
	{
		int i;

		*recv = *send = 0;
		for(i=0; i<count; i++)
		{
			ifr = (ift->table) + i;
			if(ifr->dwType == MIB_IF_TYPE_ETHERNET ||
			   ifr->dwType == MIB_IF_TYPE_PPP)
			{
				pGetIfEntry(ifr);
				*recv += ifr->dwInOctets;
				*send += ifr->dwOutOctets;
			}
		}
	}
	else
	{
		*recv=*send=0;
	}
}

void Net_get(void)
{
	if(ift && hmodIPHLP)
	{
		double recv,send;
		if(sec <= 0) sec = 5;
		Net_getRecvSend(&recv,&send);

		net[2] = (recv-net[0])/sec;
		net[3] = (send-net[1])/sec;
		net[0] = recv;
		net[1] = send;
		net[6] = net[2]/1024;
		net[7] = net[3]/1024;
		net[4] = recv/1024;
		net[5] = send/1024;
		net[10]= net[2]/1048576;
		net[11]= net[3]/1048576;
		net[8] = recv/1048576;
		net[9] = send/1048576;
	}
	else
	{
		net[0] = 0;
		net[1] = 0;
		net[2] = 0;
		net[3] = 0;
		net[4] = 0;
		net[5] = 0;
		net[6] = 0;
		net[7] = 0;
		net[8] = 0;
		net[9] = 0;
		net[10] = 0;
		net[11] = 0;
	}
}

void Net_end(void)
{
	if(hmodIPHLP)
	{
		FreeLibrary(hmodIPHLP);
	}
	if(buffer)
	{
		GlobalFree(buffer);
	}

	hmodIPHLP = NULL; buffer = NULL;
}

void Net_restart(void)
{
	DWORD bufsize;
	DWORD iftable;
	int i;

	GlobalFree(buffer); buffer=NULL;
	if(pGetIfTable == NULL || pGetIfEntry == NULL)return;
	bufsize = 0;
	iftable = pGetIfTable((PMIB_IFTABLE)buffer, &bufsize, TRUE);
	if(iftable != ERROR_INSUFFICIENT_BUFFER)
	{
		FreeLibrary(hmodIPHLP); hmodIPHLP = NULL;
		return;
	}
	buffer = GlobalAlloc(GPTR, bufsize);
	if(buffer == NULL)return;
	iftable = pGetIfTable((PMIB_IFTABLE)buffer, &bufsize, TRUE);
	if(iftable != NO_ERROR)
	{
		GlobalFree(buffer); buffer = NULL;
		return;
	}
	ift = (MIB_IFTABLE *)buffer;
	count = ift->dwNumEntries;

	net[0] = 0;
	net[1] = 0;
	net[4] = 0;
	net[5] = 0;
	net[8] = 0;
	net[9] = 0;
	for(i=0; i<count; i++)
	{
		ifr = (ift->table) + i;
		if(ifr->dwType == MIB_IF_TYPE_ETHERNET ||
		   ifr->dwType == MIB_IF_TYPE_PPP)
		{
			net[0] += ifr->dwInOctets;
			net[1] += ifr->dwOutOctets;
			net[4] += net[0]/1024;
			net[5] += net[1]/1024;
			net[8] += net[0]/1048576;
			net[9] += net[1]/1048576;
		}
	}
	sec = (int)GetMyRegLong(NULL, "IntervalSysInfo", 5);
	if(sec <= 0 || 60 < sec) sec = 5;

}
