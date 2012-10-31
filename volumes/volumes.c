#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <locale.h>

void vpath_pairs_append(WCHAR *volpath, WCHAR *win32path);
void vpath_pairs_free();

WCHAR* FindDriveLetterFromVolumePath(WCHAR* wszVolumePath);

BOOL ProcessVolumeMountPoint(HANDLE hPt, WCHAR *PtBuf, DWORD dwPtBufSize, WCHAR *Buf);
BOOL ProcessVolume(HANDLE hVol, WCHAR *Buf, DWORD iBufSize);
int GetMountPoint(void);

void DisplayVolumePaths(PWCHAR VolumeName);
WCHAR *fdigitsW(uint64_t integer);
WCHAR *ComputerUnits(uint64_t integer, int maxdiv);
void DrawGauge(int length, int percent, int isempty);
int IsTargetDrive(int DriveType, int flgNoRemoveable, int flgIsRealFloppy);
void ShowVolume(WCHAR* VolumeName, int flgShowDevName, int flgVerbose, int flgUnit, int flgGauge, int flgNoRemoveable);

typedef BOOL (WINAPI *PGVPNFVNW)(LPCWSTR,LPWSTR,DWORD,PDWORD);

struct vpath_pairs{
 WCHAR vpath[MAX_PATH];
 WCHAR w32path[MAX_PATH];
 struct vpath_pairs *next;
};

static struct vpath_pairs *vpath = NULL;

void vpath_pairs_append(WCHAR *volpath, WCHAR *win32path) {
    struct vpath_pairs* v;
    if(!vpath) {
        vpath = (struct vpath_pairs*) malloc(sizeof(struct vpath_pairs));
        wcscpy(vpath->vpath, volpath);
        wcscpy(vpath->w32path, win32path);
        vpath->next = NULL;
    } else {
        v = vpath;
        while(v->next != NULL) v = v->next;
        v->next = (struct vpath_pairs*) malloc(sizeof(struct vpath_pairs));
        wcscpy(v->next->vpath, volpath);
        wcscpy(v->next->w32path, win32path);
        vpath->next->next = NULL;
    }
}

void vpath_pairs_free() {
    struct vpath_pairs *v, *t;
    if(vpath) {
        v = vpath;
        while(v->next != NULL) {
            t = v;
            v = v->next;
            free(t);
        }
        vpath = NULL;
    }
}


WCHAR* FindDriveLetterFromVolumePath(WCHAR* wszVolumePath) {
    static WCHAR** vPathLetters = NULL;
    static WCHAR tmp[10] = {0};
    static int vPathFilled = 0;
    int i = 0;

    if(wszVolumePath) {
        wcscpy(tmp,L"A:\\");
        if(!vPathLetters) { // malloc the 2D array
            vPathLetters = (WCHAR**) malloc(26*sizeof(WCHAR*));
            for (i = 0; i < 26; i++)
                vPathLetters[i] = (WCHAR*) malloc((MAX_PATH+1)*sizeof(WCHAR));
        }
        if(!vPathFilled) {
            for(i = 0; i < 26; i++, tmp[0]++) {
                GetVolumeNameForVolumeMountPointW(tmp, vPathLetters[i], MAX_PATH);
            }
            vPathFilled = 1;
        }

        tmp[0] = L'A';
        for(i = 0; i < 26; i++, tmp[0]++) {
            if(wcsicmp(wszVolumePath, vPathLetters[i]) == 0) {
                tmp[0] = L'A' + i;
                return tmp;
            }
        }
    } else { // free the 2D array
        if(vPathLetters) { // avoid double free/free uninitialized
            for (i = 0; i < 26; i++)
                free(vPathLetters[i]);
            free(vPathLetters);
        }
        vPathLetters = NULL;
        vPathFilled = 0;
    }
    return NULL;
}

// *******
BOOL ProcessVolumeMountPoint(HANDLE hPt, WCHAR *PtBuf, DWORD dwPtBufSize, WCHAR *Buf) {
    BOOL bFlag;
    WCHAR Path[MAX_PATH];
    WCHAR Target[MAX_PATH];

    wcscpy(Path, Buf);
    wcscat(Path, PtBuf);

    bFlag = GetVolumeNameForVolumeMountPointW(Path,Target,MAX_PATH);

    if (bFlag) {
        wcscpy(Path, FindDriveLetterFromVolumePath(Buf));
        wcscat(Path, PtBuf);
        vpath_pairs_append(Target, Path);
    }

    bFlag = FindNextVolumeMountPointW(hPt,PtBuf,dwPtBufSize);
    return (bFlag);
}

BOOL ProcessVolume(HANDLE hVol, WCHAR *Buf, DWORD iBufSize) {
    BOOL bFlag;
    HANDLE hPt;
    WCHAR PtBuf[MAX_PATH];
    DWORD dwSysFlags;
    WCHAR FileSysNameBuf[MAX_PATH];

    // is NTFS?
    GetVolumeInformationW( Buf, NULL, 0, NULL, NULL, &dwSysFlags, FileSysNameBuf, MAX_PATH);

    if ((dwSysFlags & FILE_SUPPORTS_REPARSE_POINTS)) {
        hPt = FindFirstVolumeMountPointW(Buf, PtBuf, MAX_PATH);

        if (hPt != INVALID_HANDLE_VALUE) {
            bFlag = ProcessVolumeMountPoint(hPt, PtBuf, MAX_PATH, Buf);
            while (bFlag)
                bFlag = ProcessVolumeMountPoint (hPt, PtBuf, MAX_PATH, Buf);
            FindVolumeMountPointClose(hPt);
        }
    }

    bFlag = FindNextVolumeW(hVol, Buf, iBufSize);
    return (bFlag);
}
int GetMountPoint(void) {
    WCHAR buf[MAX_PATH];
    HANDLE hVol;
    BOOL bFlag;

    hVol = FindFirstVolumeW(buf, MAX_PATH);
    if (hVol == INVALID_HANDLE_VALUE) {
       return (-1);
    }

    bFlag = ProcessVolume(hVol, buf, MAX_PATH);
    while (bFlag) {
       bFlag = ProcessVolume(hVol, buf, MAX_PATH);
    }

    bFlag = FindVolumeClose( hVol );
    return (bFlag);
}
// *******

void DisplayVolumePaths(PWCHAR VolumeName) {
    DWORD  CharCount = MAX_PATH + 1;
    PWCHAR Names     = NULL;
    PWCHAR NameIdx   = NULL;
    BOOL   Success   = FALSE;
    HANDLE hFvn      = NULL;

    struct vpath_pairs* v;

    PGVPNFVNW pGVPNFVNW;
    pGVPNFVNW = (PGVPNFVNW) GetProcAddress( 
        GetModuleHandle("kernel32.dll"),
        "GetVolumePathNamesForVolumeNameW");

    if(pGVPNFVNW) {
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
            Success = pGVPNFVNW(VolumeName, Names, CharCount, &CharCount);

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
    } else {
        // GetVolumePathNamesForVolumeNameW cannot be used (Win2000)

        // print drive letter first
        Names = FindDriveLetterFromVolumePath(VolumeName);
        wprintf(L"  %s", Names);

        // then mountpoints
        GetMountPoint();
        if(vpath) {
            v = vpath;
            while(v != NULL) {
                if(wcsicmp(v->vpath, VolumeName) == 0) {
                    wprintf(L"  %s", v->w32path);
                }
                v = v->next;
            }
        }

    }

    return;
}

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
    WCHAR tmp[10] = {0};
    int count = 0;
    double smallfloat;
    uint64_t smallint;

    for(smallint = smallfloat = integer; smallint > 1024 && count < maxdiv;) {
        count++;
        smallint /= 1024;
        smallfloat /= 1024;
    }

    if(count)
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

int IsTargetDrive(int DriveType, int flgRemoveable, int flgIsRealFloppy) {
    if(!flgRemoveable)
        return DriveType > 1 && DriveType != 2 && DriveType != 5;
    else if(flgRemoveable == 2)
        return DriveType > 1;
    else
        return DriveType > 1 && !flgIsRealFloppy;
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

    if(flgShowDevName) {
        //
        //  Skip the \\?\ prefix and remove the trailing backslash.
        Index = wcslen(tmp) - 1;

        if (tmp[0]     != L'\\' ||
            tmp[1]     != L'\\' ||
            tmp[2]     != L'?'  ||
            tmp[3]     != L'\\' ||
            tmp[Index] != L'\\') 
        {
            Error = ERROR_BAD_PATHNAME;
            wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", tmp);
            return;
        }

        //
        //  QueryDosDeviceW does not allow a trailing backslash,
        //  so temporarily remove it.
        tmp[Index] = L'\0';

        CharCount = QueryDosDeviceW(&tmp[4], DeviceName, ARRAYSIZE(DeviceName)); 

        tmp[Index] = L'\\';

        if ( CharCount == 0 ) {
            Error = GetLastError();
            wprintf(L"QueryDosDeviceW failed with error code %d\n", Error);
            return;
        }

        if(flgRemoveable == 1) {
            flgIsRealFloppy = (wcsstr(DeviceName, L"Floppy") != NULL);
        }

    }

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

    if(!flgVerbose && !IsTargetDrive(VolumeType,1,0)) return;
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

    if (flgShowDevName)
        DisplayVolumePaths(tmp);
    else
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
            flgRemoveable = 1;
            if(argv[i][2] == 'f')
                flgRemoveable += 1;
        } else if(stricmp(argv[i],"-v") == 0) {
            flgVerbose = 1;
        } else if(stricmp(argv[i],"-a") == 0) {
            flgForceEnum = 1;
        } else if(strcmp(argv[i],"-?") == 0) {
            printf("%s [-v] [-g] [-a] [-r[f]] [-u#] [-?] [drive|path...]\n\n"
                " -v\t\tbe verbose (-g switch will be turned off)\n"
                " -g\t\tdraw gauge of free space\n"
                " -a\t\tappend drive|path to detected list\n"
                " -r[f]\t\tdetect removable device free space (-rf for real floppy drive)\n"
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
        //
        //  Enumerate all volumes in the system.
        FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));

        if (FindHandle == INVALID_HANDLE_VALUE) {
            Error = GetLastError();
            wprintf(L"FindFirstVolumeW failed with error code %d\n", Error);
            return 1;
        }

        for (;;) {
            ShowVolume(VolumeName, 1, flgVerbose, flgUnit, flgGauge, flgRemoveable);

            //
            //  Move on to the next volume.
            Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));

            if ( !Success ) {
                Error = GetLastError();

                if (Error != ERROR_NO_MORE_FILES) {
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

    }

    SetErrorMode(OldMode); //put things back the way they were
    FindDriveLetterFromVolumePath(NULL); // free the array
    vpath_pairs_free(); // free the linked list
    return 0;
}

