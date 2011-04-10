/*****************************************************************************
 *                                                                           *
 * DH_FILE.C                                                                 *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <utime.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

// helpers

int sys_FileInformation(char* file_Path,char *full_Path, unsigned short* fake_inode)
{
    HANDLE hFile;
	BY_HANDLE_FILE_INFORMATION FileInformation;
	DWORD dwDesiredAccess=GENERIC_READ,fdwShareMode=FILE_SHARE_READ;
    char fullpath[2*MAX_PATH+2],*p;
   
	GetFullPathName( file_Path, MAX_PATH, fullpath, &p );
	hFile=CreateFile(fullpath,dwDesiredAccess,fdwShareMode,NULL,OPEN_EXISTING,0,NULL);
	if (hFile==INVALID_HANDLE_VALUE) return -1;

	if(GetFileInformationByHandle(hFile,  &FileInformation))
	{
		if(full_Path) strncpy(full_Path,fullpath,MAX_PATH);
		*fake_inode=(unsigned short) FileInformation.nFileIndexLow;
	}
	else return -2;

CloseHandle(hFile);
return 0;
}

/*  Modified version of "PJW" algorithm (see the "Dragon" compiler book). */
static unsigned
hashval (const unsigned char * str)
{
  unsigned h = 0;
  while (*str)
    {
      h = (h << 4) + *str++;
      h ^= (h >> 28);
    }
  return h;
}

int ino_Hash(char *file_Path)
{
 char fullpath[2*MAX_PATH+2],*p;

  _strlwr (file_Path);
  GetFullPathName( file_Path, MAX_PATH, fullpath, &p );
	
//	printf("%s %s %i\n",file_Path,fullpath,loc);

return (int) hashval(fullpath);
}

// end helpers

/* Function stuff ************************************************************/

/* Read info about a link ================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int readlink(char* file_Name,char* buf_Mem,int buf_Size)
{
	/* See if the file exists */
	if (access(file_Name,X_OK) == -1)
	{
		errno = ENOENT;
	}
	else
	{
		errno = EINVAL;
	}

	/* Either way, it's not a link */	
	return -1;
}
#endif

/* Set the time of a file ================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int utimes(char* file_Name,struct timeval file_Time[])
{
	struct utimbuf file_Utime;

	/* Fill out the structure */
	file_Utime.actime = file_Time[0].tv_sec;
	file_Utime.modtime = file_Time[1].tv_sec;

	/* And call utime() */
	return utime(file_Name,&file_Utime);
}
#endif

/* Change the ownership of a file ========================================== */
int chown(char* file_Name,uid_t file_User,gid_t file_Group)
{
	/* Pretend we did something */
	return 0;
}

/* Change the ownership of a file descriptor =============================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int fchown(int file_Fd,uid_t file_User,gid_t file_Group)
{
	/* Pretend we did something */
	return 0;
}
#endif

/* Create a Downhill_File_Stat structure from a _stat structure ============ */
static void downhill_Stat_Create(struct Downhill_File_Stat* file_Info,
             struct _stat* file_InfoReal)
{
	file_Info->st_dev = file_InfoReal->st_dev;
	file_Info->st_ino = file_InfoReal->st_ino;
	file_Info->st_mode = file_InfoReal->st_mode;
	file_Info->st_nlink = file_InfoReal->st_nlink;
	file_Info->st_uid = Downhill_User_IdReal;
	file_Info->st_gid = Downhill_Group_IdReal;
	file_Info->st_rdev = file_InfoReal->st_rdev;
	file_Info->st_size = file_InfoReal->st_size;
	file_Info->st_atime = file_InfoReal->st_atime;
	file_Info->st_mtime = file_InfoReal->st_mtime;
	file_Info->st_ctime = file_InfoReal->st_ctime;
#if (_DOWNHILL_POSIX_SOURCE == 0)
	file_Info->st_blksize = _DOWNHILL_STAT_BLOCKSIZE;
#endif
}

/* Stat a file ============================================================= */
int Downhill_File_Stat(char* file_Name,struct Downhill_File_Stat* file_Info)
{
	struct _stat file_InfoReal;

	/* Get the stat information */
	if (_stat(file_Name,&file_InfoReal) != 0)
	{
		return -1;
	}

	/* Fill in the structure */
	downhill_Stat_Create(file_Info,&file_InfoReal);

// fake inode
	file_Info->st_ino=ino_Hash(file_Name); // for directories
	sys_FileInformation(file_Name,NULL, &(file_Info->st_ino));  //make it better

	return 0;
}

/* Stat a file descriptor ================================================== */
int Downhill_File_StatFd(int file_Fd,struct Downhill_File_Stat* file_Info)
{
	struct _stat file_InfoReal;
	BY_HANDLE_FILE_INFORMATION FileInformation;

	/* Get the fstat information */
	if (_fstat(file_Fd,&file_InfoReal) != 0)
	{
		return -1;
	}

	/* Fill in the structure */
	downhill_Stat_Create(file_Info,&file_InfoReal);

	// fake inode
	if(GetFileInformationByHandle((HANDLE) _get_osfhandle( file_Fd ),  &FileInformation))
	file_Info->st_ino=(unsigned short)FileInformation.nFileIndexLow; // can only be a file

	return 0;
}


/* Stat a file or a symlink ================================================ */

#if (_DOWNHILL_POSIX_SOURCE == 0)


int lstat(const char* file_Path_i,struct stat* file_Stat)
{
	/* It can't be a link, so just stat file */
	char* file_Path;
	int ret;
	file_Path = _alloca(strlen(file_Path_i)+1); strcpy(file_Path,file_Path_i);
		if(!file_Path) return -1;
	ret=stat(file_Path,file_Stat);
// fake an inode value
	if(ret==0)
	{
		file_Stat->st_ino=ino_Hash(file_Path); // for directories
		sys_FileInformation(file_Path,NULL, &(file_Stat->st_ino));  //make it better
	}

return ret;
}
#endif

/* Set the host file status ================================================ */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int sethostent(int host_Stayopen)
{
	return 0;
}
#endif

/* Close the host file ===================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int endhostent(void)
{
	return 0;
}
#endif

/* Truncate an open file =================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int ftruncate(int file_Fd,off_t file_Size)
{
	return chsize(file_Fd,file_Size);
}
#endif

/* Truncate a file ========================================================= */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int truncate(char* file_Name,off_t file_Size)
{
	int file_Fd;
	int truncate_Result;

	file_Fd = open(file_Name,O_WRONLY);
	if (file_Fd == -1)
	{
		return -1;
	}
	truncate_Result = ftruncate(file_Fd,file_Size);
	close(file_Fd);

	return truncate_Result;
}
#endif
