/*****************************************************************************
 *                                                                           *
 * sys/unistd.h                                                              *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_UNISTD_H)
#define _DOWNHILL_SYS_UNISTD_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define stuff **************************************************************/

/* File premissions ======================================================== */
#define F_OK 0
#define X_OK 0
#define R_OK 2
#define W_OK 4

/* lseek() positions ======================================================= */
#if (_DOWNHILL_POSIX_SOURCE == 0)
#define L_SET  SEEK_SET
#define L_INCR SEEK_CUR
#define L_XTND SEEK_END
#endif


/* Extern stuff **************************************************************/
extern int   opterr;
extern int   optind;
extern int   optopt;
extern char* optarg;


/* Prototype stuff ***********************************************************/
uid_t        getuid(void);
uid_t        geteuid(void);
int          setuid(uid_t user_Id);
gid_t        getgid(void);
gid_t        getegid(void);
int          getgroups(int group_Max,gid_t group_Id[]);
int          setgid(gid_t group_Id);
unsigned int sleep(unsigned int sleep_Duration);
int          pause(void);
int          chown(char* file_Name,uid_t file_User,gid_t file_Group);
#if (_DOWNHILL_POSIX_SOURCE == 0)
int          setruid(uid_t user_Id);
int          seteuid(uid_t user_Id);
int          setrgid(gid_t group_Id);
int          setegid(gid_t group_Id);
int          fchown(int file_Fd,uid_t file_User,gid_t file_Group);
int          getopt(int arg_Count,char* arg_Value[],char flag_List[]);
int          readlink(char* file_Name,char* buf_Mem,int buf_Size);
int          ftruncate(int file_Fd,off_t file_Size);
int          truncate(char* file_Name,off_t file_Size);
#endif

//kms
# ifndef getpagesize
#  define getpagesize() 4096
# endif

#ifdef __cplusplus
}
#endif

#endif /* _DOWNHILL_SYS_UNISTD_H */
