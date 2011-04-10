/*****************************************************************************
 *                                                                           *
 * sys/stat.h                                                                *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_STAT_H)
#define _DOWNHILL_SYS_STAT_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_INC.H"
#include "DOWNHILL/DH_POSIX.H"
#include _DOWNHILL_INCLUDE_DIR/sys/stat.h>
#include <direct.h>
#include <sys/types.h>


/* Struct stuff **************************************************************/
struct Downhill_File_Stat
{
	dev_t   st_dev;
	ino_t   st_ino;
	mode_t  st_mode;
	nlink_t st_nlink;
	uid_t   st_uid;
	gid_t   st_gid;
	dev_t   st_rdev;
	off_t   st_size;
	time_t  st_atime;
	time_t  st_mtime;
	time_t  st_ctime;
#if (_DOWNHILL_POSIX_SOURCE == 0)
	long    st_blksize;
#endif
};


/* Define stuff **************************************************************/

/* Permission stuff ======================================================== */
#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRITE
#define S_IXUSR _S_IEXEC
#define S_IRWXU S_IRUSR|S_IWUSR|S_IXUSR

#define S_IRGRP _S_IREAD
#define S_IWGRP _S_IWRITE
#define S_IXGRP _S_IEXEC
#define S_IRWXG S_IRGRP|S_IWGRP|S_IXGRP

#define S_IROTH _S_IREAD
#define S_IWOTH _S_IWRITE
#define S_IXOTH _S_IEXEC
#define S_IRWXO S_IROTH|S_IWOTH|S_IXOTH


/* Macro stuff ============================================================= */
#define S_ISDIR(file_Mode)  ((file_Mode&_S_IFDIR) == _S_IFDIR)
#define S_ISCHR(file_Mode)  ((file_Mode&_S_IFCHR) == _S_IFCHR)
#define S_ISREG(file_Mode)  ((file_Mode&_S_IFREG) == _S_IFREG)
#define S_ISBLK(file_Mode)  ((file_Mode&_S_IFBLK) == _S_IFBLK)
#define S_ISREG(file_Mode)  ((file_Mode&_S_IFREG) == _S_IFREG)
#define S_ISFIFO(file_Mode) ((file_Mode&_S_IFFIFO) == _S_IFFIFO)
#define S_ISUID(file_Mode)  (1)
#define S_ISGID(file_Mode)  (1)

/* Re-definition stuff ===================================================== */
#undef  mkdir
#define mkdir(dir_Name,dir_Permissions) \
         Downhill_Directory_Make(dir_Name,dir_Permissions)
#undef  stat
#define stat  Downhill_File_Stat
#undef  fstat
#define fstat Downhill_File_StatFd


/* Prototype stuff ***********************************************************/
int Downhill_Directory_Make(char* dir_Name,mode_t dir_Permissions);
int Downhill_File_Stat(char* file_Name,struct Downhill_File_Stat* file_Info);
int Downhill_File_StatFd(int file_Fd,struct Downhill_File_Stat* file_Info);
int lstat(const char* file_Path,struct stat* file_Stat);

#endif /* _DOWNHILL_SYS_STAT_H */
