/*****************************************************************************
 *                                                                           *
 * DH_USR.C                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>


/* Global stuff **************************************************************/
char* Downhill_User_Name = _DOWNHILL_USER_NAME;
uid_t Downhill_User_IdReal = _DOWNHILL_USER_IDREAL;
uid_t Downhill_User_IdEffective = _DOWNHILL_USER_IDEFFECTIVE;
#if defined(_DOWNHILL_UNIX_SYSV)
uid_t Downhill_User_IdSaved = _DOWNHILL_USER_IDSAVED;
#endif
char* Downhill_User_Password = _DOWNHILL_USER_PASSWORD;
char* Downhill_User_Gecos = _DOWNHILL_USER_GECOS;

char* Downhill_User_Dir = "C:\\";
char* Downhill_User_Shell = "C:\\winnt\\system32\\CMD.exe";


/* Static stuff **************************************************************/
static struct passwd downhill_User_Info;
static int           downhill_User_Position = -1;


/* Function stuff ************************************************************/

/* Get the real user ID ==================================================== */
uid_t getuid(void)
{
	return Downhill_User_IdReal;
}

/* Get effective user ID =================================================== */
uid_t geteuid(void)
{
	return Downhill_User_IdEffective;
}

/* Set the user ID ========================================================= */
int setuid(uid_t user_Id)
{
#if defined(_DOWNHILL_UNIX_SYSV)
	if ((Downhill_User_IdReal == user_Id) ||
	 (Downhill_User_IdEffective == user_Id) ||
	 (Downhill_User_IdSaved == user_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0) ||
	 (Downhill_User_IdSaved == 0))
	{
#else
	if ((Downhill_User_IdReal == user_Id) ||
	 (Downhill_User_IdEffective == user_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0))
	{
		Downhill_User_IdReal = user_Id;
#endif
		Downhill_User_IdEffective = user_Id;
		return 0;
	}

	errno = EPERM;
	return -1;
}

/* Set the real user ID ==================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int setruid(uid_t user_Id)
{
	if ((Downhill_User_IdReal == user_Id) ||
	 (Downhill_User_IdEffective == user_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0))
	{
		Downhill_User_IdReal = user_Id;
		return 0;
	}

	errno = EPERM;
	return -1;
}
#endif

/* Set the effective user ID =============================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int seteuid(uid_t user_Id)
{
	if ((Downhill_User_IdReal == user_Id) ||
	 (Downhill_User_IdEffective == user_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0))
	{
		Downhill_User_IdEffective = user_Id;
		return 0;
	}

	errno = EPERM;
	return -1;
}
#endif

/* Return a password entry ================================================= */
static struct passwd* downhill_User_GetEntry(void)
{
	/* If they've already read the "entry," error out */
	if (downhill_User_Position != -1)
	{
		return NULL;
	}

	/* Set that we've been read */
	downhill_User_Position++;

	/* Fill in our dummy values */
	downhill_User_Info.pw_name = Downhill_User_Name;
	downhill_User_Info.pw_passwd = Downhill_User_Password;
	downhill_User_Info.pw_uid = Downhill_User_IdReal;
	downhill_User_Info.pw_gid = Downhill_Group_IdReal;
	downhill_User_Info.pw_gecos = Downhill_User_Gecos;
	downhill_User_Info.pw_dir = Downhill_User_Dir;
	downhill_User_Info.pw_shell = Downhill_User_Shell;

	/* And return the structure */
	return &downhill_User_Info;
}

#if (_DOWNHILL_POSIX_SOURCE == 0)
struct passwd* getpwent(void)
{
	return downhill_User_GetEntry();
}
#endif

/* Return a password entry based on uid ==================================== */
struct passwd* getpwuid(uid_t user_Id)
{
	/* Check user ID */
	if (user_Id != Downhill_User_IdReal)
	{
		return NULL;
	}

	/* Reset to the beginning */
	downhill_User_Position = -1;

	/* Return an entry */
	return downhill_User_GetEntry();
}

/* Return a password entry based on name =================================== */
struct passwd* getpwnam(char* user_Name)
{
	/* Check user name */
	if (strcmp(user_Name,Downhill_User_Name))
	{
		return NULL;
	}

	/* Reset to the beginning */
	downhill_User_Position = -1;

	/* Return an entry */
	return downhill_User_GetEntry();
}

/* Rewind the password file ================================================ */
#if (_DOWNHILL_POSIX_SOURCE == 0)
void setpwent(void)
{
	/* Reset to the beginning */
	downhill_User_Position = -1;

	return;
}
#endif

/* Stop using the password file ============================================ */
#if (_DOWNHILL_POSIX_SOURCE == 0)
void endpwent(void)
{
	/* Reset to the beginning */
	downhill_User_Position = -1;

	return;
}
#endif

//kms
char *getlogin()
{
int n;
char *gl;

n=511;
gl=malloc(n+1); if(!gl) return NULL;
GetUserName(gl,&n);
gl=realloc(gl,n+1);
GetUserName(gl,&n);

return gl;
}