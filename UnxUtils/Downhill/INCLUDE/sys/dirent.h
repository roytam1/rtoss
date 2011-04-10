/*****************************************************************************
 *                                                                           *
 * sys/dirent.h                                                              *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_DIRENT_H)
#define _DOWNHILL_SYS_DIRENT_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <stdlib.h>
#include <windows.h>


/* Struct and typedef stuff **************************************************/

/* Directory entry (file) struct =========================================== */
struct dirent
{
	unsigned long   d_fileno;
#define                 d_ino d_fileno
	unsigned short  d_reclen;
	unsigned short  d_namlen;
	char            d_name[_MAX_PATH+1];
};

/* Directory info typedef ================================================== */
typedef struct __dirdesc
{
	char          dir_Name[_MAX_PATH+1];
	HANDLE        dir_Handle;
	int           dir_FileCount;
	struct dirent dir_FileUnix;
} DIR;


/* Prototype stuff **********************************************************/
DIR*           opendir(char* dir_Name);
struct dirent* readdir(DIR* dir_Info);
void           rewinddir(DIR* dir_Info);
int            closedir(DIR* dir_Info);
#if (_DOWNHILL_POSIX_SOURCE == 0)
long           telldir(DIR* dir_Info);
void           seekdir(DIR* dir_Info,long dir_Position);
#endif


#endif /* _DOWNHILL_SYS_DIRENT_H */
