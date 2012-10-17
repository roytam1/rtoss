#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

void DisplayVolumePaths(PWCHAR VolumeName);
WCHAR *ComputerUnits(uint64_t integer, int maxdiv);

void DisplayVolumePaths(PWCHAR VolumeName) {
    DWORD  CharCount = MAX_PATH + 1;
    PWCHAR Names     = NULL;
    PWCHAR NameIdx   = NULL;
    BOOL   Success   = FALSE;

    for (;;) {
        //
        //  Allocate a buffer to hold the paths.
        Names = (PWCHAR) malloc(CharCount * sizeof(WCHAR));

        if ( !Names ) {
            //
            //  If memory can't be allocated, return.
            return;
        }

        //
        //  Obtain all of the paths
        //  for this volume.
        Success = GetVolumePathNamesForVolumeNameW(
            VolumeName, Names, CharCount, &CharCount
            );

        if ( Success ) {
            break;
        }

        if ( GetLastError() != ERROR_MORE_DATA ) {
            break;
        }

        //
        //  Try again with the
        //  new suggested size.
        free(Names);
        Names = NULL;
    }

    if ( Success ) {
        //
        //  Display the various paths.
        for ( NameIdx = Names; 
              NameIdx[0] != L'\0'; 
              NameIdx += wcslen(NameIdx) + 1 ) {
            wprintf(L"  %s", NameIdx);
        }
        //wprintf(L"\n");
    }

    if ( Names != NULL ) {
        free(Names);
        Names = NULL;
    }

    return;
}

#define MAX_PLACES 40
WCHAR *fdigitsW(uint64_t integer) {
    static WCHAR fdigits[MAX_PLACES + (MAX_PLACES / 3) + 1];
    WCHAR digits[MAX_PLACES + 1];
    WCHAR *dgt = digits, *fdgt = fdigits;
    int places;

    places = swprintf(digits, L"%I64u", integer); /* convert integer to string */
    /* places = digits in string */
    while (*fdgt++ = *dgt++) /* while there are more digits*/
        if (--places) /* if places-1 > 0 */
            if ( !(places % 3) ) /* if places is multiple of 3 */
                *fdgt++ = L','; /* insert a comma here */

    return fdigits;
}

WCHAR *ComputerUnits(uint64_t integer, int maxdiv) {
    static WCHAR fdigits[MAX_PLACES + (MAX_PLACES / 3) + 1];
    WCHAR *units[] = {L"B",L"KB",L"MB",L"GB",L"TB",L"PB",L"EB", NULL};
    WCHAR tmp[10];
    int count = 0;
    double smallfloat;
    uint64_t smallint;

	for(smallint = smallfloat = integer; smallint > 1024 && count < maxdiv;) {
        count++;
        smallint /= 1024;
        smallfloat /= 1024;
    }

    swprintf(tmp, L"%.2f", smallfloat-smallint);
    swprintf(fdigits, L"%s%s %s", fdigitsW(smallint), tmp+1, units[count]); /* convert integer to string */

    return fdigits;
}

void DrawGauge(int length, int percent, int isempty) {
    int i,barlong,remain;
    char tmp[100];

    barlong = (length-2)*percent/100;
    remain = (length-2)-barlong;
    putchar('|');
    if(!isempty) {
        for(i=0; i<barlong; ++i)
            putchar('.');
        for(i=0; i<remain; ++i)
            putchar('#');
    } else {
        sprintf(tmp,"%%%ds",length-2);
        printf(tmp,"");
    }
    putchar('|');
}


int main(int argc, char **argv)
{
    DWORD  CharCount            = 0;
    WCHAR  DeviceName[MAX_PATH] = L"";
    DWORD  Error                = ERROR_SUCCESS;
    HANDLE FindHandle           = INVALID_HANDLE_VALUE;
    BOOL   Found                = FALSE;
    size_t Index                = 0;
    BOOL   Success              = FALSE;
    WCHAR  VolumeName[MAX_PATH] = L"";

    WCHAR* VolumeTypeName       = 0;
    UINT   VolumeType           = 0;

    DWORD  SectorsPerCluster    = 0;
    DWORD  BytesPerSector       = 0;
    DWORD  NumberOfFreeClusters = 0;
    DWORD  TotalNumberOfClusters= 0;
    uint64_t TotalSize          = 0;
    uint64_t FreeSize           = 0;

    double den, percent;

    int i         = 1;
    int flgVerbose= 0;
    int flgUnit   = 3;
    int flgGauge  = 0;

    for (; i < argc; i++) {
        if(stricmp(argv[i],"-v") == 0)
            flgVerbose = 1;
        if(strnicmp(argv[i],"-u",2) == 0)
            if(argv[i][2] >= '0' && argv[i][2] <= '9')
                flgUnit = argv[i][2] - '0';
        if(stricmp(argv[i],"-g") == 0)
            flgGauge = 1;
        if(strcmp(argv[i],"-?") == 0) {
            printf("%s [-v] [-g] [-u#] [-?]\n\n"
                " -v\t\tbe verbose (-g switch will be off)\n"
                " -g\t\tdraw gauge of free space\n"
                " -u[num]\tuse unit # (0=byte, 1=KB, 2=MB, 3=GB, 4=TB)\n"
                " -?\t\tshow this help and exit\n", argv[0]);
            return 0;
        }
    }

    setlocale(LC_CTYPE, ""); // reset locale

    //
    //  Enumerate all volumes in the system.
    FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));

    if (FindHandle == INVALID_HANDLE_VALUE)
    {
        Error = GetLastError();
        wprintf(L"FindFirstVolumeW failed with error code %d\n", Error);
        return 1;
    }

    for (;;)
    {
        //
        //  Skip the \\?\ prefix and remove the trailing backslash.
        Index = wcslen(VolumeName) - 1;

        if (VolumeName[0]     != L'\\' ||
            VolumeName[1]     != L'\\' ||
            VolumeName[2]     != L'?'  ||
            VolumeName[3]     != L'\\' ||
            VolumeName[Index] != L'\\') 
        {
            Error = ERROR_BAD_PATHNAME;
            wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
            break;
        }

        //
        //  QueryDosDeviceW does not allow a trailing backslash,
        //  so temporarily remove it.
        VolumeName[Index] = L'\0';

        CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName)); 

        VolumeName[Index] = L'\\';

        if ( CharCount == 0 ) 
        {
            Error = GetLastError();
            wprintf(L"QueryDosDeviceW failed with error code %d\n", Error);
            break;
        }

        VolumeType = GetDriveTypeW(VolumeName);
        switch(VolumeType) {
            case 0:
                VolumeTypeName = L"(Unknown)";
                break;
            case 1:
                VolumeTypeName = L"(invalid root path)";
                break;
            case 2:
                VolumeTypeName = L"(Removable)";
                break;
            case 3:
                VolumeTypeName = L"(Fixed)";
                break;
            case 4:
                VolumeTypeName = L"(Remote)";
                break;
            case 5:
                VolumeTypeName = L"(CD-ROM)";
                break;
            case 6:
                VolumeTypeName = L"(RAM Disk)";
                break;
        }

        // get free space information
        if(VolumeType == 3) {
            GetDiskFreeSpaceW(VolumeName, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
            FreeSize = TotalSize = SectorsPerCluster;
            FreeSize *= BytesPerSector;
            TotalSize *= BytesPerSector;
            FreeSize *= NumberOfFreeClusters;
            TotalSize *= TotalNumberOfClusters;
            den = TotalSize;
            percent = FreeSize;
            percent /= den;
            percent *= 100;
        }

        if(flgVerbose) {
            wprintf(L"\nFound a device: %s %s", DeviceName, VolumeTypeName);
            wprintf(L"\nVolume name: %s", VolumeName);
            wprintf(L"\nPaths:");
        }
        if(!flgVerbose && flgGauge)
            DrawGauge(36,(int)percent,VolumeType != 3);

        DisplayVolumePaths(VolumeName);


        if(VolumeType == 3) {
            // VolumeName is reused here
            wcscpy(VolumeName, ComputerUnits(FreeSize, flgUnit));
            wprintf(L" (%s / %s) %.2f%%", VolumeName, ComputerUnits(TotalSize, flgUnit), percent);
        }
        if(!flgVerbose) {
            if(VolumeType != 3) wprintf(L" %s", VolumeTypeName);
        }
        wprintf(L"\n");

        //
        //  Move on to the next volume.
        Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));

        if ( !Success ) 
        {
            Error = GetLastError();

            if (Error != ERROR_NO_MORE_FILES) 
            {
                wprintf(L"FindNextVolumeW failed with error code %d\n", Error);
                break;
            }

            //
            //  Finished iterating
            //  through all the volumes.
            Error = ERROR_SUCCESS;
            break;
        }
    }

    FindVolumeClose(FindHandle);
    FindHandle = INVALID_HANDLE_VALUE;

    return 0;
}

