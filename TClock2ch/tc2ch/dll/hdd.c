/*-------------------------------------------------------------------------
  hdd.c
  HDD ‚Ì‹ó‚«—e—Ê‚ðŽæ“¾‚·‚é
---------------------------------------------------------------------------*/

#include <windows.h>
#include <iprtrmib.h>

void Hdd_get(void);
ULONGLONG GetDiskSpace(int nDrive, ULONGLONG *nFree);
double diskFree[52];
double diskAll[52];

extern int actdvl[];

void Hdd_get(void)
{
	int i;

	for(i=0;i<26;i++)
	{
		if(actdvl[i] == 1)
		{
			ULONGLONG freeDisk, allDisk;

			allDisk = GetDiskSpace(i,&freeDisk);
			diskFree[i]    = (double)(LONGLONG)(freeDisk/1048576);
			diskFree[i+26] = diskFree[i]/1024;
			diskAll[i]     = (double)(LONGLONG)(allDisk/1048576);
			diskAll[i+26]  = diskAll[i]/1024;
		}
	}
}

ULONGLONG GetDiskSpace(int nDrive, ULONGLONG *nFree)
{
	char    szDrive[5];
	ULARGE_INTEGER useByte, allByte, freeByte;

	wsprintf(szDrive, "%c:\\", nDrive + 'A');
	GetDiskFreeSpaceEx(szDrive, &useByte, &allByte, &freeByte);

	*nFree = freeByte.QuadPart;
	return(allByte.QuadPart);
}
