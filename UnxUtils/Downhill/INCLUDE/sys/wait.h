/*****************************************************************************
 *                                                                           *
 * sys/wait.h                                                                *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_WAIT_H)
#define _DOWNHILL_SYS_WAIT_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <windows.h>
#include <sys/resource.h>
#include <sys/types.h>


/* Define stuff **************************************************************/
#define WIFEXITED(status)   TRUE
#define WIFSTOPPED(status)  FALSE
#define WIFSIGNALED(status) FALSE
#define WEXITSTATUS(status) (int)((signed char)((status>>8)&0xff))
#define WTERMSIG(status)    0
#define WSTOPSIG(status)    0
#if (_DOWNHILL_POSIX_SOURCE == 0)
#define WCOREDUMP(status)   0
#endif
#define WNOHANG             1
#define WUNTRACED           0
#define _WAITMASK           (WNOHANG|WUNTRACED)


/* Prototype stuff ***********************************************************/
pid_t wait(int* process_Status);
pid_t waitpid(pid_t process_Id,int* process_Status,int wait_Flags);
#if (_DOWNHILL_POSIX_SOURCE == 0)
pid_t wait3(int* process_Status,int wait_Flags,struct rusage* process_Usage);
pid_t wait4(pid_t process_Id,int* process_Status,int wait_Flags,
       struct rusage* process_Usage);
#endif


#endif /* _DOWNHILL_SYS_WAIT_H */
