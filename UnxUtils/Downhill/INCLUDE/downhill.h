/*****************************************************************************
 *                                                                           *
 * downhill.h                                                                *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_DOWNHILL_H)
#define _DOWNHILL_DOWNHILL_H


/* Include stuff *************************************************************/
#include <windows.h>
#include <io.h>
#include <process.h>
#include <sys/stat.h>
#include <sys/types.h>


/* Extern stuff **************************************************************/

/* User stuff ============================================================== */
extern char* Downhill_User_Name;
extern uid_t Downhill_User_IdReal;
extern uid_t Downhill_User_IdEffective;
#if defined(_DOWNHILL_UNIX_SYSV)
extern uid_t Downhill_User_IdSaved;
#endif
extern char* Downhill_User_Password;
extern char* Downhill_User_Gecos;
extern char* Downhill_User_Dir;
extern char* Downhill_User_Shell;

/* Group stuff ============================================================= */
extern char* Downhill_Group_Name;
extern gid_t Downhill_Group_IdReal;
extern gid_t Downhill_Group_IdEffective;
#if defined(_DOWNHILL_UNIX_SYSV)
extern gid_t Downhill_Group_IdSaved;
#endif
extern char* Downhill_Group_Password;


/* Prototype stuff ***********************************************************/

/* Process stuff =========================================================== */
char* Downhill_Process_System(char* exec_Name,char* exec_Argv[],
       int* exec_Return,int file_Return);
pid_t Downhill_Process_Forkexec(char* exec_Name,char* exec_Argv[],
       HANDLE exec_Handle[],DWORD exec_Flags);
pid_t Downhill_Process_Add(HANDLE process_Id);

/* Signal stuff ============================================================ */
void Downhill_Signal_Check(void);


#endif /* _DOWNHILL_DOWNHILL_H */
