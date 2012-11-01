#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <locale.h>

WCHAR *fdigitsW(uint64_t integer);
WCHAR *ComputerUnits(uint64_t integer, int maxdiv);
void DrawGauge(int length, int percent, int isempty);
int IsTargetDrive(int DriveType, int flgNoRemoveable, int flgIsRealFloppy);
void ShowVolume(WCHAR* VolumeName, int flgShowDevName, int flgVerbose, int flgUnit, int flgGauge, int flgNoRemoveable);

#define MAX_PLACES 64
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
    WCHAR tmp[16] = {0};
    int count = 0;
    double smallfloat;
    uint64_t smallint;

    for(smallint = smallfloat = integer; smallint > 1024 && count < maxdiv;) {
        count++;
        smallint /= 1024;
        smallfloat /= 1024;
    }

    if(count) {
        swprintf(tmp, L"%.3f", smallfloat-smallint);
        tmp[4] = 0; // truncate
    }

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

int IsTargetDrive(int DriveType, int flgRemoveable, int flgIsRealFloppy) {
    int ret = 0;
    if(!flgRemoveable)
        ret = DriveType > 1 && DriveType != 2 && DriveType != 4 && DriveType != 5;
    if(flgRemoveable & 1)
        ret |= DriveType > 1 && DriveType != 4 && !flgIsRealFloppy;
    if(flgRemoveable & 2)
        ret |= DriveType > 1 && DriveType != 4;
    if(flgRemoveable & 4)
        ret |= DriveType > 1 && DriveType != 2 && DriveType != 5;
    return ret;
}

void ShowVolume(WCHAR* VolumeName, int flgShowDevName, int flgVerbose, int flgUnit, int flgGauge, int flgRemoveable) {
    DWORD  Error                = ERROR_SUCCESS;
    DWORD  CharCount            = 0;
    WCHAR  tmp[MAX_PATH]        = L"";
    WCHAR  DeviceName[MAX_PATH] = L"";
    WCHAR* VolumeTypeName       = 0;
    UINT   VolumeType           = 0;
    size_t Index                = 0;
    DWORD  SectorsPerCluster    = 0;
    DWORD  BytesPerSector       = 0;
    DWORD  NumberOfFreeClusters = 0;
    DWORD  TotalNumberOfClusters= 0;
    uint64_t TotalSize          = 0;
    uint64_t FreeSize           = 0;

    UINT   flgIsRealFloppy      = 0;

    double den                  = 0.0f;
    double percent              = 0.0f;

    wcscpy(tmp, VolumeName);

    VolumeType = GetDriveTypeW(tmp);
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

    if(!flgVerbose && !IsTargetDrive(VolumeType,7,0)) return;
    // get free space information
    if(IsTargetDrive(VolumeType,flgRemoveable,flgIsRealFloppy)) {
        GetDiskFreeSpaceW(tmp, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);
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
        wprintf(L"\nFound a device: %s", DeviceName);
        wprintf(L"\nVolume name: %s %s", tmp, VolumeTypeName);
        wprintf(L"\nPaths:");
    }
    else if(flgGauge) {
        DrawGauge(36,(int)percent,!IsTargetDrive(VolumeType,flgRemoveable,flgIsRealFloppy)|!TotalSize);
    }

    if (!flgShowDevName)
        wprintf(L"  %s", tmp);


    if(IsTargetDrive(VolumeType,flgRemoveable,flgIsRealFloppy)) {
        // VolumeName is reused here
        wcscpy(tmp, ComputerUnits(FreeSize, flgUnit));

        // DeviceName is reused here
        if(TotalSize)
            swprintf(DeviceName, L"%.2f", percent);
        else
            wcscpy(DeviceName, L"--");

        wprintf(L" (%s / %s) %s%%", tmp, ComputerUnits(TotalSize, flgUnit), DeviceName);
    }
    if(!flgVerbose) {
        if(VolumeType != 3) wprintf(L" %s", VolumeTypeName);
    }
    wprintf(L"\n");
}


int main(int argc, char **argv)
{
    DWORD  Error                = ERROR_SUCCESS;
    HANDLE FindHandle           = INVALID_HANDLE_VALUE;
    BOOL   Found                = FALSE;
    size_t Index                = 0;
    BOOL   Success              = FALSE;
    WCHAR  VolumeName[MAX_PATH] = L"";
    DWORD  dwAttrib             = 0;

    int i               = 1;
    int flgVerbose      = 0;
    int flgUnit         = 3;
    int flgGauge        = 0;
    int flgRemoveable   = 0;
    int flgNonEnum      = 0;
    int flgForceEnum    = 0;
    int OldMode; //a place to store the old error mode

    //save the old error mode and set the new mode to let us do the work:
    OldMode = SetErrorMode(SEM_FAILCRITICALERRORS); 

    for (; i < argc; i++) {
        if(stricmp(argv[i],"-g") == 0) {
            flgGauge = 1;
        } else if(strnicmp(argv[i],"-u",2) == 0) {
            if(argv[i][2] >= '0' && argv[i][2] <= '4')
                flgUnit = argv[i][2] - '0';
        } else if(strnicmp(argv[i],"-r",2) == 0) {
            flgRemoveable |= 1;
            if(argv[i][2] == 'f')
                flgRemoveable |= 2;
        } else if(stricmp(argv[i],"-n") == 0) {
            flgRemoveable |= 4;
        } else if(stricmp(argv[i],"-v") == 0) {
            flgVerbose = 1;
        } else if(stricmp(argv[i],"-a") == 0) {
            flgForceEnum = 1;
        } else if(strcmp(argv[i],"-?") == 0) {
            printf("%s [-v] [-g] [-a] [-r[f]] [-n] [-u#] [-?] [drive|path...]\n\n"
                " -v\t\tbe verbose (-g switch will be turned off)\n"
                " -g\t\tdraw gauge of free space\n"
                " -a\t\tappend drive|path to detected list\n"
                " -r[f]\t\tdetect removable device free space (-rf for real floppy drive)\n"
                " -n\t\tdetect mounted network drive free space\n"
                " -u[num]\tuse unit # (0=byte, 1=KB, 2=MB, 3=GB, 4=TB)\n"
                " -?\t\tshow this help and exit\n"
                " drive|path...\tshow individual drives\n"
                " \t\texample:  C: D: F:\\mountpoint\\\n", argv[0]);
            return 0;
        } else {
            // assume it is a path
            dwAttrib = GetFileAttributesA(argv[i]);
            if(dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
                // yes it is valid path
                if(!flgForceEnum)
                    flgNonEnum = 1;
                // dwAttrib is reused
                dwAttrib = MultiByteToWideChar(CP_ACP, 0, argv[i], -1, VolumeName, MAX_PATH);
                // workaround on getting free space from \\host\share
                if (VolumeName[dwAttrib-2] != L'\\') VolumeName[dwAttrib-1] = L'\\';
                ShowVolume(VolumeName, 0, flgVerbose, flgUnit, flgGauge, flgRemoveable);
            } else {
                printf("%s is not valid path.\n", argv[i]);
                return 0;
            }
        }
    }

    setlocale(LC_CTYPE, ""); // reset locale

    if(!flgNonEnum) {

        wcscpy(VolumeName,L"A:\\");
        if(flgRemoveable & 2) i = 0;
        else i = 2;
        // i is reused here
        for (; i< 26 ; i++) {
        	VolumeName[0] = L'A' + i;
            ShowVolume(VolumeName, 0, flgVerbose, flgUnit, flgGauge, flgRemoveable);
        }

        FindHandle = INVALID_HANDLE_VALUE;

    }

    SetErrorMode(OldMode); //put things back the way they were
    return 0;
}

