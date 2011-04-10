/*****************************************************************************
 *                                                                           *
 * DH_DIR.C                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <io.h>
#include <sys/dirent.h>


/* Function stuff ************************************************************/

/* Open a directory ======================================================== */
DIR* opendir(char* dir_Name)
{
	DIR*        dir_Info;
	struct stat dir_Stat;

	/* Make sure directory is actually a directory */
	if (stat(dir_Name,&dir_Stat) != 0)
	{
		errno = ENOENT;
		return NULL;
	}
	if ((dir_Stat.st_mode&_S_IFDIR) == 0)
	{
		errno = ENOTDIR;
		return NULL;
	}

	/* Get some memory */
	dir_Info = (DIR*)calloc(1,sizeof(DIR));
	if (dir_Info == NULL)
	{
		return NULL;
	}

	/* Save some info about the directory */
	strcpy(dir_Info->dir_Name,dir_Name);
	if ((dir_Info->dir_Name[strlen(dir_Info->dir_Name)-1] != '/') &&
	 (dir_Info->dir_Name[strlen(dir_Info->dir_Name)-1] != '\\'))
	{
		strcat(dir_Info->dir_Name,"/");
	}
	strcat(dir_Info->dir_Name,"*");
	dir_Info->dir_Handle = INVALID_HANDLE_VALUE;
	dir_Info->dir_FileCount = 0;

	/* And return the directory info */
	return dir_Info;
}

/* Read the next file in a directory ======================================= */
struct dirent* readdir(DIR* dir_Info)
{
	WIN32_FIND_DATA dir_FileWin;

	/* Get the info about the next file */
	if (dir_Info->dir_FileCount == 0)
	{
		dir_Info->dir_Handle = FindFirstFile(dir_Info->dir_Name,
		 &dir_FileWin);
		if (dir_Info->dir_Handle == INVALID_HANDLE_VALUE)
		{
			return NULL;
		}
	}
	else
	{
		if (!FindNextFile(dir_Info->dir_Handle,&dir_FileWin))
		{
			return NULL;
		}
	}

	/* Dummy up a UNIX file */
	dir_Info->dir_FileUnix.d_fileno = dir_Info->dir_FileCount;
	dir_Info->dir_FileUnix.d_reclen = sizeof(struct dirent);
	dir_Info->dir_FileUnix.d_namlen = strlen(dir_FileWin.cFileName);
	strcpy(dir_Info->dir_FileUnix.d_name,dir_FileWin.cFileName);

	/* Bump the number we've read */
	dir_Info->dir_FileCount++;

	/* And return the UNIX file info */
	return &(dir_Info->dir_FileUnix);
}

/* Rewind a directory ====================================================== */
void rewinddir(DIR* dir_Info)
{
	/* Re-set to the beginning */
	dir_Info->dir_Handle = INVALID_HANDLE_VALUE;
	dir_Info->dir_FileCount = 0;
}

/* Close a directory ======================================================= */
int closedir(DIR* dir_Info)
{
	/* Close down the directory */
	if (!FindClose(dir_Info->dir_Handle))
	{
		errno = EBADF;
		return -1;
	}

	/* Free up the memory */
	free(dir_Info);

	/* And return that everything went fine */
	return 0;
}

/* Tell our position in a directory ======================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
long telldir(DIR* dir_Info)
{
	/* Return our last position */
	return dir_Info->dir_FileCount;
}
#endif

/* Seek to a position in a directory ======================================= */
#if (_DOWNHILL_POSIX_SOURCE == 0)
void seekdir(DIR* dir_Info,long dir_Position)
{
	/* Jump back to the beginning of the file */
	rewinddir(dir_Info);

	/* And read to a specific position */
	while (--dir_Position > 0)
	{
		readdir(dir_Info);
	}
}
#endif

/* Make a directory ======================================================== */
int Downhill_Directory_Make(char* dir_Name,mode_t dir_Permissions)
{
	/* Make the directory */
	if (_mkdir(dir_Name) != 0)
	{
		return -1;
	}

	/* Set the permissions */
	return chmod(dir_Name,dir_Permissions);
}
