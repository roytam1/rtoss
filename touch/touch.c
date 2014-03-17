/* touch.c v1.21 - 2011-04-28
Update:
  http://myc01.free.fr/touch

License:
  Public Domain Dedication (CC0 1.0)
  http://creativecommons.org/publicdomain/zero/1.0/

Usage:
touch.exe [-hxamcDsvq] [-r ref_file|-t time|-d date_time] -- <file_1> [file_N]

  -h : Show this help
  -x : Modify creation time
  -a : Modify access time
  -m : Modify modification time
  -c : Do not create file if not found
  -r : Use a file or directory as a reference time source
  -D : If target doesn't exist, create a directory
  -s : Browse sub-directories
  -v : Output the result of every file processed
  -q : Silent mode, no error is displayed (disable '-v')
  -t : Use time [[CC]YY]MMDDhhmm[.SS] instead of today
  -d : Use date_time [[YYYY-]MM-DD][Thh:mm[:ss]] instead of today
  -- : Finish options, followed by the file list
The options '-r', '-d' and '-t' can not be used simultaneously.
By default, all times will be modified (creation, access and modification).

Examples:
  touch.exe -sv -d 2011-04-23          -- ".\folder\*.txt"
  touch.exe -cv -d           T10:55:00 -- *.dat
  touch.exe -xv -d 2011-04-23T10:55    -- test2.txt test3.txt

********************************************************************************
Changelog:
1.21 - 2011-04-28
  * fix decode date/time
  * add touch folder with *.*
  * Combination of the options '-s' '-v' : '-sv'

1.2 - 2011-04-26
  * argument from 'GetCommandLine' / 'CommandLineToArgv'
  * change option '-d' to '-D'
  * add option '-d date_time'

1.1 - 2011-04-24
  * add options '-v' verbose, '-s' scan-subfolder, '-q' silent mode

1.0 - 2011-04-20
  * initial version with options '-hxamcrdt -- file_n ...'
*/


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define PRG_NAME  "Touch"
#define PRG_VERS  "1.21"
#define PRG_DATE  "2011-04-28"
#define PRG_LIC   "Public Domain Dedication (CC0 1.0)"

/* Options in command line */
#define OPT_MODIFY_TIME_C     1  /* -x */
#define OPT_MODIFY_TIME_A     2  /* -a */
#define OPT_MODIFY_TIME_M     4  /* -m */
#define OPT_NO_CREATE         8  /* -c */
#define OPT_USE_REF_FILE     16  /* -r */
#define OPT_USER_TIME        32  /* -t */
#define OPT_USER_DATE        64  /* -d */
#define OPT_CREATE_DIR      128  /* -D */
#define OPT_VERBOSE         256  /* -v */
#define OPT_RECURSIVE       512  /* -s */
#define OPT_QUIET          1024  /* -q */

#define CommandLineToArgv  _CommandLineToArgvA
#define LEN_PATH           1024     /* _MAX_PATH = 260 */

LPFILETIME lpTimeC, lpTimeA, lpTimeM;
DWORD      dwOptions;
HANDLE     hStdOut;

char **ARGV;
int    ARGC;


int GetOptC (const char cOption, const int nDef)
{
    if ( (ARGC < 2) || (!ARGV) )
        return nDef;

    int i;
    for (i = 1; i < ARGC; i++)
        if ( (ARGV[i][0] == '-') && (strchr (ARGV[i], cOption) != NULL) )
            return 1;
    return nDef;
}  /* GetOptC */


char *GetOptCPStr (char **szStr, const char cOption, char *nDef)
{
    if ( (ARGC < 2) || (!ARGV) )
        return (*szStr = nDef);

    int i;
    for (i = 1; i < ARGC; i++)
        if ( (ARGV[i][0] == '-') && (ARGV[i][1] == cOption) )
        {
            if (++i < ARGC)
                return (*szStr = ARGV[i]);
            else
                return (*szStr = nDef);
        }

    return (*szStr = nDef);
}  /* GetOptCPStr */


char **_CommandLineToArgvA (const char *CmdLine, int *pnArgc)
{
    char           a;
    char         **argv, *pcArgv;
    unsigned long  len, i, j, argc = 0;
    int            in_QM = FALSE, in_TEXT = FALSE, in_SPACE = TRUE;

    len = strlen (CmdLine);
    i = ( (len + 2) / 2) * sizeof (void*) + sizeof (void*);

    argv   = (char**) calloc (i + (len + 2) * sizeof (char), 1);
    pcArgv = (char*) ( ( (unsigned char*) argv) + i);

    argv[argc] = pcArgv;
    i = j = 0;

    while ( (a = CmdLine[i]) )
    {
        if (in_QM)
        {
            if (a == '\"')
                in_QM = FALSE;
            else
                pcArgv[j++] = a;
        }
        else
        {
            switch (a)
            {
            case '\"':
                in_QM = in_TEXT = TRUE;
                if (in_SPACE)
                    argv[argc++] = pcArgv + j;
                in_SPACE = FALSE;
                break;
            case ' ' :
            case '\t':
            case '\n':
            case '\r':
                if (in_TEXT)
                    pcArgv[j++] = '\0';
                in_TEXT  = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if (in_SPACE)
                    argv[argc++] = pcArgv + j;
                pcArgv[j++] = a;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    pcArgv[j]  = '\0';
    argv[argc] = NULL;

    (*pnArgc) = argc;
    return argv;
}  /* _CommandLineToArgvA */


BOOL FolderExists (LPCTSTR lpPath)
{
    DWORD Attr = GetFileAttributes (lpPath);
    if (Attr != INVALID_FILE_ATTRIBUTES)
        if (Attr & FILE_ATTRIBUTE_DIRECTORY)
            return TRUE;

    return FALSE;
}  /* FolderExists */


BOOL Exists (LPCTSTR lpFileName)
{
    if (GetFileAttributes (lpFileName) != INVALID_FILE_ATTRIBUTES)
        return TRUE;
    else if (GetLastError () == ERROR_SHARING_VIOLATION)
        return TRUE;
    return FALSE;
} /* Exists */


int printFileTime (LPFILETIME lpFileTime)
{
    if (!lpFileTime) return 0;
    FILETIME   fTime;
    SYSTEMTIME sTime;

    FileTimeToLocalFileTime (lpFileTime, &fTime);
    FileTimeToSystemTime    (&fTime    , &sTime);
    return printf ("%04hu"      "-%02hu"      "-%02hu"    "T%02hu"     ":%02hu"       ":%02hu",
                   sTime.wYear, sTime.wMonth, sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond);
}  /* printFileTime */


int printfOem (LPCTSTR lpFmt, ...)
{
    if (dwOptions & OPT_QUIET) return 0;
    if (! lpFmt) return 0;
    if (!*lpFmt) return 0;

    PCHAR lpBuf = calloc (LEN_PATH, 1);
    INT nLen = _vsnprintf (lpBuf, LEN_PATH, lpFmt, (va_list)(&lpFmt + 1));
    if (nLen < 0)
        nLen = strlen (lpBuf);
    CharToOemBuff (lpBuf, lpBuf, nLen);
    WriteConsole (hStdOut, lpBuf, nLen, (PDWORD)&nLen, NULL);
    free (lpBuf);
    return nLen;
}  /* printfOem */


DWORD Touch (LPCTSTR lpFileName)
{
    SetLastError (ERROR_SUCCESS);
    DWORD dwFileAttributes = GetFileAttributes (lpFileName);
    DWORD dwResult = GetLastError ();
    if ((dwFileAttributes == INVALID_FILE_ATTRIBUTES) && (dwResult != ERROR_FILE_NOT_FOUND))
        return dwResult;

    BOOL bCreateDir = ((OPT_CREATE_DIR   & dwOptions)                && (dwFileAttributes == INVALID_FILE_ATTRIBUTES)) ||
                      ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (dwFileAttributes != INVALID_FILE_ATTRIBUTES));

    /* If target doesn't exist, create a directory. */
    if (bCreateDir && (dwFileAttributes == INVALID_FILE_ATTRIBUTES))
    {
        SetLastError (ERROR_SUCCESS);
        if (!CreateDirectory (lpFileName, NULL))
        {
            if ((dwResult = GetLastError ()) != ERROR_ALREADY_EXISTS)
                return dwResult;
        }
        else
            return ERROR_SUCCESS;  /* Done, the directory is created. */
    }

    SetLastError (ERROR_SUCCESS);
    HANDLE hFile = CreateFile (lpFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               ( (dwOptions & OPT_NO_CREATE) || bCreateDir) ? OPEN_EXISTING : OPEN_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL | (bCreateDir ? FILE_FLAG_BACKUP_SEMANTICS : 0), 0);
    dwResult = GetLastError ();

    /* Check for CreateFile() special cases */
    if (hFile == INVALID_HANDLE_VALUE)
    {
        if ( (dwOptions & OPT_NO_CREATE) && dwResult == ERROR_FILE_NOT_FOUND)
            return ERROR_SUCCESS;  /* not an error */
        else if (dwResult == ERROR_ALREADY_EXISTS)
            dwResult = ERROR_SUCCESS;  /* not an error according to MSDN docs */
        return dwResult;
    }

    if (lpTimeC || lpTimeA || lpTimeM)
    {
        if (SetFileTime (hFile, lpTimeC, lpTimeA, lpTimeM))
        {
            if ( (dwOptions & OPT_VERBOSE) && !(dwOptions & OPT_QUIET) )
                printfOem     ("%s\n", lpFileName);  /*
                printf        ("%s\n", lpFileName);  */
            dwResult = ERROR_SUCCESS;
        }
        else
            dwResult = GetLastError ();
    }
    CloseHandle (hFile);

    return dwResult;
}  /* Touch */


BOOL PrintHelp (LPCTSTR lpMessage)
{
    if (lpMessage)
        printf ("%s\n\n", lpMessage);

    puts (PRG_NAME" v"PRG_VERS" ("PRG_DATE") - License: "PRG_LIC);  /*
    puts ("********************************************************************************"); */
    puts ("Usage:\n"
          "touch.exe [-hxamcDsvq] [-r ref_file|-t time|-d date_time] -- <file_1> [file_N]");
    puts ("  -h : Show this help\n"
          "  -x : Modify creation time\n"
          "  -a : Modify access time\n"
          "  -m : Modify modification time\n"
          "  -c : Do not create file if not found\n"
          "  -r : Use a file or directory as a reference time source\n"
          "  -D : If target doesn't exist, create a directory\n"
          "  -s : Browse sub-directories\n"
          "  -v : Output the result of every file processed\n"
          "  -q : Silent mode, no error is displayed (disable '-v')\n"
          "  -t : Use time [[CC]YY]MMDDhhmm[.SS] instead of today\n"
          "  -d : Use date_time [[YYYY-]MM-DD][Thh:mm[:ss]] instead of today\n"
          "  -- : Finish options, followed by the file list\n"
          "The options '-r', '-d' and '-t' can not be used simultaneously.\n"
          "By default, all times will be modified (creation, access and modification).");
    puts ("Examples:\n"
          "  touch.exe -sv -d 2011-04-23          -- \".\\folder\\*.txt\"\n"
          "  touch.exe -cv -d           T10:55:00 -- *.dat\n"
          "  touch.exe -xv -d 2011-04-23T10:55    -- test2.txt test3.txt");

    return FALSE;
}  /* PrintHelp */


/* Prints a message translated from a Windows Error code, then prints the usage */
PCHAR PrintError (LPCTSTR lpInfo, DWORD dwError)
{
    if (dwOptions & OPT_QUIET) return NULL;

    LPTSTR lpText = NULL;
    FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, dwError, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpText, 0, NULL);
    printfOem ("\nError: %s = %s", lpInfo, lpText);
    LocalFree (lpText);

    return NULL;
}  /* PrintError */


BOOL GetArgs (LPDWORD dwList, LPTSTR *lpRefFile, LPTSTR *lpDateTime, LPDWORD dwOptions)
{
    if (ARGC < 2)
        return PrintHelp (NULL);
    else if (ARGC == 2)
    {
        if (GetOptC ('h', 0))
            return PrintHelp (NULL);
        else if (ARGV[1][0] == '-')
            return PrintHelp ("Error 1: Not enough arguments.");
        *dwList = 1;
    }
    else
    {
        for (*dwList = 1; *dwList < (DWORD)ARGC; (*dwList)++)
            if (!strcmp ("--", ARGV[*dwList]))
                break;
        if (++(*dwList) >= (DWORD)ARGC)
            return PrintHelp ("Error 2: No file(s) specified!");

        *dwOptions |= GetOptC ('x', 0) * OPT_MODIFY_TIME_C;
        *dwOptions |= GetOptC ('a', 0) * OPT_MODIFY_TIME_A;
        *dwOptions |= GetOptC ('m', 0) * OPT_MODIFY_TIME_M;
        *dwOptions |= GetOptC ('c', 0) * OPT_NO_CREATE;
        *dwOptions |= GetOptC ('D', 0) * OPT_CREATE_DIR;
        *dwOptions |= GetOptC ('v', 0) * OPT_VERBOSE;
        *dwOptions |= GetOptC ('s', 0) * OPT_RECURSIVE;
        *dwOptions |= GetOptC ('q', 0) * OPT_QUIET;
        /*
        if (GetOptPStr (lpRefFile , "-r", NULL)) *dwOptions |= OPT_USE_REF_FILE;
        if (GetOptPStr (lpDateTime, "-d", NULL)) *dwOptions |= OPT_USER_DATE;
        if (GetOptPStr (lpDateTime, "-t", NULL)) *dwOptions |= OPT_USER_TIME;
        */
        if (GetOptCPStr (lpRefFile , 'r', NULL)) *dwOptions |= OPT_USE_REF_FILE;
        if (GetOptCPStr (lpDateTime, 'd', NULL)) *dwOptions |= OPT_USER_DATE;
        if (GetOptCPStr (lpDateTime, 't', NULL)) *dwOptions |= OPT_USER_TIME;
    }

    if ( (*dwOptions & OPT_USE_REF_FILE) &&  (*dwOptions & OPT_USER_TIME) )
        return PrintHelp ("Error 3: You may not specify both '-r' and '-t'.");

    if ( (*dwOptions & OPT_USE_REF_FILE) &&  (*dwOptions & OPT_USER_DATE) )
        return PrintHelp ("Error 4: You may not specify both '-r' and '-d'.");

    if ( (*dwOptions & OPT_USER_TIME   ) &&  (*dwOptions & OPT_USER_DATE) )
        return PrintHelp ("Error 5: You may not specify both '-t' and '-d'.");

    if ( (*dwOptions & OPT_USE_REF_FILE) && !(*lpRefFile                ) )
        return PrintHelp ("Error 6: You must specify a 'ref_file' together with '-r'.");

    if ( (*dwOptions & OPT_USER_TIME   ) && !(*lpDateTime               ) )
        return PrintHelp ("Error 7: You must specify a timestamp together with '-t'.");

    /* If -x, -a and -m wasn't specified, then all of them are implied */
    if (! (*dwOptions &  (OPT_MODIFY_TIME_C | OPT_MODIFY_TIME_A | OPT_MODIFY_TIME_M) ) )
           *dwOptions |= (OPT_MODIFY_TIME_C | OPT_MODIFY_TIME_A | OPT_MODIFY_TIME_M);

    return TRUE;
}  /* GetArgs */


/* Gets FILETIME from a file */
BOOL GetFileTimes (LPCTSTR lpFileName, LPFILETIME fTimeC, LPFILETIME fTimeA, LPFILETIME fTimeM)
{
    BOOL bResult = TRUE;
    HANDLE hFile = CreateFile (lpFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return PrintHelp (PrintError (lpFileName, GetLastError ()));

    if (!GetFileTime (hFile, fTimeC, fTimeA, fTimeM))
        bResult = PrintHelp (PrintError (lpFileName, GetLastError ()));

    CloseHandle (hFile);
    return bResult;
}  /* GetFileTimes */


/* Format: ISO 8601:2004(E)
    http://en.wikipedia.org/wiki/ISO_8601
    http://dotat.at/tmp/ISO_8601-2004_E.pdf
    Extended format: YYYY-MM-DDThh:mm:ss Example: 1985-04-12T10:15:30

    [[[YYYY-]MM-DD][Thh:mm[:ss]]]
    123456789ABCDEF1234
    YYYY-MM-DDThh:mm:ss
    YYYY-MM-DD
    MM-DD
    Thh:mm:ss
    Thh:mm
*/
BOOL DecodeDateTime (LPTSTR lpDateTime, LPSYSTEMTIME st)
{
    char *pTime;
    int nLen = (lpDateTime) ? strlen (lpDateTime) : 0;

    if ((nLen < 5) || (nLen > 19))
        return PrintHelp ("Error 8: Invalid time format.");

    if (nLen && (pTime = strchr (lpDateTime, 'T')))
    {
        st->wSecond = st->wMilliseconds = 0;
        nLen = strlen (pTime);
             if (nLen == 9)  /* Thh:mm:ss */
            sscanf (pTime, "T%02hu:%02hu:%02hu", &st->wHour, &st->wMinute, &st->wSecond);
        else if (nLen == 6)  /* Thh:mm    */
            sscanf (pTime, "T%02hu:%02hu"      , &st->wHour, &st->wMinute              );
        else if (nLen >  0)
            return PrintHelp ("Error 9: Invalid time format.");
        *pTime = '\0';
        nLen = strlen (lpDateTime);
    }

    if (nLen > 0)
    {
             if (nLen == 10)  /* YYYY-MM-DD */
            sscanf (lpDateTime, "%4hu-%02hu-%02hu", &st->wYear, &st->wMonth, &st->wDay);
        else if (nLen ==  5)  /*      MM-DD */
            sscanf (lpDateTime,      "%02hu-%02hu",             &st->wMonth, &st->wDay);
        else
            return PrintHelp ("Error 10: Invalid date format.");
    }
    return TRUE;
}  /* DecodeDateTime */


/* Format:
  [[CC]YY]MMDDhhmm[.SS]
  123456789ABCDEF
  CCYYMMDDhhmm.SS
  CCYYMMDDhhmm
  MMDDhhmm
 */
BOOL DecodeTime (LPTSTR lpDateTime, LPSYSTEMTIME st)
{
    PCHAR pSec;
    INT   nLen = (lpDateTime) ? strlen (lpDateTime) : 0;
    WORD  CC = st->wYear / 100;
    WORD  YY = st->wYear % 100;
    st->wSecond = st->wMilliseconds = 0;

    if ((nLen < 8) || (nLen > 15))
        return PrintHelp ("Error 11: Invalid time format.");

    if ((pSec = strchr (lpDateTime, '.')))
    {
        sscanf (pSec, ".%02hu", &st->wSecond);
        *pSec = '\0';
        nLen = strlen (lpDateTime);
    }

    if (nLen > 0)
    {
             if (nLen == 12)  /* CC  YY   MM   DD  hh  mm */
            sscanf (lpDateTime, "%2hu%02hu%02hu%2hu%2hu%2hu", &CC, &YY, &st->wMonth, &st->wDay, &st->wHour, &st->wMinute);
        else if (nLen == 10)  /*     YY   MM   DD  hh  mm */
            sscanf (lpDateTime,     "%02hu%02hu%2hu%2hu%2hu",      &YY, &st->wMonth, &st->wDay, &st->wHour, &st->wMinute);
        else if (nLen ==  8)  /*          MM   DD  hh  mm */
            sscanf (lpDateTime,          "%02hu%2hu%2hu%2hu",           &st->wMonth, &st->wDay, &st->wHour, &st->wMinute);
        else
            return PrintHelp ("Error 12: Invalid date format.");
        if (nLen == 10)
            CC = (YY < 69) ? 20 : 19;
        st->wYear = CC * 100 + YY;
    }
    return TRUE;
}  /* DecodeTime */


BOOL StrToFileTime (LPTSTR lpDateTime, LPFILETIME ts)
{
    INT nLen = (lpDateTime) ? strlen (lpDateTime) : 0;
    SYSTEMTIME st;
    GetLocalTime (&st);

    if (nLen)
    {
        if (strchr (lpDateTime, ':') || strchr (lpDateTime, '-'))
        {
            if (!DecodeDateTime (lpDateTime, &st))  /* [[[YYYY-]MM-DD][Thh:mm[:ss]]] */
                return FALSE;
        }
        else
        {
            if (!DecodeTime (lpDateTime, &st))  /* [[CC]YY]MMDDhhmm[.SS] */
                return FALSE;
        }
    }

    FILETIME ft;
    if (!SystemTimeToFileTime (&st, &ft))
        return PrintHelp (PrintError ("13 SystemTimeToFileTime()"   , GetLastError ()));

    if (!LocalFileTimeToFileTime (&ft, ts))
        return PrintHelp (PrintError ("14 LocalFileTimeToFileTime()", GetLastError ()));

    return TRUE;
}  /* StrToFileTime */


DWORD FindFileRecursive (LPCTSTR lpDir, LPCTSTR lpFindName)
{
    DWORD           dwResult = ERROR_SUCCESS;
    HANDLE          hFile;
    CHAR            szPath[LEN_PATH];
    WIN32_FIND_DATA fData;

    strcpy (szPath, lpDir);
    strcat (szPath, "\\");
    strcat (szPath, (*lpFindName) ? lpFindName : "*.*");

    if ((hFile = FindFirstFile (szPath, &fData)) == INVALID_HANDLE_VALUE)
        return GetLastError ();

    do
    {
        if (!strcmp (fData.cFileName, ".") || !strcmp (fData.cFileName, ".."))
            continue;

        strcpy (szPath, lpDir);
        strcat (szPath, "\\");
        strcat (szPath, fData.cFileName);

        if (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (fData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
                continue;

            if ((dwResult = Touch (szPath)) != ERROR_SUCCESS)
                PrintError (szPath, dwResult);

            if (dwOptions & OPT_RECURSIVE)
            {
                if ((dwResult = FindFileRecursive (szPath, lpFindName)) != ERROR_SUCCESS)
                {
                    FindClose (hFile);
                    return dwResult;
                }
            }
        }
        else
            if ((dwResult = Touch (szPath)) != ERROR_SUCCESS)
                PrintError (szPath, dwResult);
    }
    while (FindNextFile (hFile, &fData));

    FindClose (hFile);
    return dwResult;
}  /* FindFileRecursive */


int ExitCode (int status)
{
    free (ARGV);
    exit (status);
}  /* ExitCode */


int main (void)  /* (int argc, char *argv[]) */
{
    DWORD     dwFilesList, dwResult;
    CHAR      szPath [LEN_PATH], szFullPath [LEN_PATH], szFindName[_MAX_FNAME];
    CHAR      szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFileName[_MAX_FNAME], szExt[_MAX_EXT];
    PCHAR     lpRefFile = NULL, lpDateTime = NULL;
    FILETIME  fTimeC, fTimeA, fTimeM;

    ARGV = CommandLineToArgv (GetCommandLine (), &ARGC);
    hStdOut = GetStdHandle (STD_OUTPUT_HANDLE);

    if (!GetArgs (&dwFilesList, &lpRefFile, &lpDateTime, &dwOptions))
        ExitCode (-1);

    if (dwOptions & OPT_USE_REF_FILE)
    {
        if (!GetFileTimes (lpRefFile, &fTimeC, &fTimeA, &fTimeM))
            ExitCode (-1);
    }
    else
    {
        if (!StrToFileTime (lpDateTime, &fTimeM))
            ExitCode (-1);
        fTimeC = fTimeA = fTimeM;
    }

    lpTimeC = (dwOptions & OPT_MODIFY_TIME_C) ? &fTimeC : NULL;
    lpTimeA = (dwOptions & OPT_MODIFY_TIME_A) ? &fTimeA : NULL;
    lpTimeM = (dwOptions & OPT_MODIFY_TIME_M) ? &fTimeM : NULL;

    if ( (dwOptions & OPT_VERBOSE) && !(dwOptions & OPT_QUIET) )
    {
        printf (    "Create       = ");
        printf ("%s\nAccess       = ", printFileTime (lpTimeC) ? "" : "unchanged");
        printf ("%s\nModification = ", printFileTime (lpTimeA) ? "" : "unchanged");
        printf ("%s\n\n"             , printFileTime (lpTimeM) ? "" : "unchanged");
    }

    while (dwFilesList < (DWORD)ARGC)
    {
        /* puts */ (strcpy (szPath, ARGV[dwFilesList++]));

        if (szPath[strlen (szPath) - 1] == '\"')
            szPath[strlen (szPath) - 1]  = '\\';

        if (Exists (szPath) && !(dwOptions & OPT_RECURSIVE) )  /* file or directory */
        {
            _fullpath (szFullPath, szPath, LEN_PATH);
            if ((dwResult = Touch (szFullPath)) != ERROR_SUCCESS)
                PrintError (szPath, dwResult);
        }
        else if ( strchr (szPath, '*') || (dwOptions & OPT_RECURSIVE) )
        {
            if (FolderExists (szPath))
                strcat (szPath, "\\");

            _splitpath (szPath, szDrive, szDir, szFileName, szExt);
            strcpy (szPath, szDrive);
            strcat (szPath, szDir);
            strcpy (szFindName, szFileName);
            strcat (szFindName, szExt);
            _fullpath (szFullPath, (szPath[0]) ? szPath : ".\\", LEN_PATH);
            szFullPath[strlen (szFullPath) - 1] = '\0';  /* Delete the last '\' */

            FindFileRecursive (szFullPath, szFindName);
        }
    }

    return ExitCode (ERROR_SUCCESS);
}  /* main */
