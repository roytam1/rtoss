/*****************************************************************************
 *                                                                           *
 * DH_GRP.C                                                                  *
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
#include <grp.h>


/* Global stuff **************************************************************/
char* Downhill_Group_Name = _DOWNHILL_GROUP_NAME;
gid_t Downhill_Group_IdReal = _DOWNHILL_GROUP_IDREAL;
gid_t Downhill_Group_IdEffective = _DOWNHILL_GROUP_IDEFFECTIVE;
#if defined(_DOWNHILL_UNIX_SYSV)
gid_t Downhill_Group_IdSaved = _DOWNHILL_GROUP_IDSAVED;
#endif
char* Downhill_Group_Password = _DOWNHILL_GROUP_PASSWORD;


/* Static stuff **************************************************************/
static struct group downhill_Group_Info;
static int          downhill_Group_Position = -1;


/* Function stuff ************************************************************/

/* Get real group ID ======================================================= */
gid_t getgid(void)
{
	return Downhill_Group_IdReal;
}

/* Get effective group ID ================================================== */
gid_t getegid(void)
{
	return Downhill_Group_IdEffective;
}

/* Set the group ID ======================================================== */
int setgid(gid_t group_Id)
{
#if defined(_DOWNHILL_UNIX_SYSV)
	if ((Downhill_Group_IdReal == group_Id) ||
	 (Downhill_Group_IdEffective == group_Id) ||
	 (Downhill_Group_IdSaved == group_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0) ||
	 (Downhill_User_IdSaved == 0))
	{
#else
	if ((Downhill_Group_IdReal == group_Id) ||
	 (Downhill_Group_IdEffective == group_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0))
	{
		Downhill_Group_IdReal = group_Id;
#endif
		Downhill_Group_IdEffective = group_Id;
		return 0;
	}

	errno = EPERM;
	return -1;
}

/* Set the real group ID =================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int setrgid(gid_t group_Id)
{
	if ((Downhill_Group_IdReal == group_Id) ||
	 (Downhill_Group_IdEffective == group_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0))
	{
		Downhill_Group_IdReal = group_Id;
		return 0;
	}

	errno = EPERM;
	return -1;
}
#endif

/* Set the effective group ID ============================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int setegid(gid_t group_Id)
{
	if ((Downhill_Group_IdReal == group_Id) ||
	 (Downhill_Group_IdEffective == group_Id) ||
	 (Downhill_User_IdReal == 0) ||
	 (Downhill_User_IdEffective == 0))
	{
		Downhill_Group_IdEffective = group_Id;
		return 0;
	}

	errno = EPERM;
	return -1;
}
#endif

/* Return a group entry ==================================================== */
static struct group* downhill_Group_GetEntry(void)
{
	/* If they've already read the "entry," error out */
	if (downhill_Group_Position != -1)
	{
		return NULL;
	}

	/* Set that we've been read */
	downhill_Group_Position++;

	/* Fill in our dummy values */
        downhill_Group_Info.gr_name = Downhill_Group_Name;
        downhill_Group_Info.gr_passwd = Downhill_Group_Password;
        downhill_Group_Info.gr_gid = Downhill_Group_IdReal;
        downhill_Group_Info.gr_mem[0] = Downhill_User_Name;
	downhill_Group_Info.gr_mem[1] = NULL;

	/* And return the structure */
	return &downhill_Group_Info;
}

#if (_DOWNHILL_POSIX_SOURCE == 0)
struct group* getgrent(void)
{
	return downhill_Group_GetEntry();
}
#endif

/* Return a group entry based on gid ======================================= */
struct group* getgrgid(gid_t group_Id)
{
	/* Check group ID */
	if (group_Id != Downhill_Group_IdReal)
	{
		return NULL;
	}

	/* Reset to the beginning */
	downhill_Group_Position = -1;

	/* Return an entry */
	return downhill_Group_GetEntry();
}

/* Return a group entry based on name ====================================== */
struct group* getgrnam(char* group_Name)
{
	/* Check group name */
	if (strcmp(group_Name,Downhill_Group_Name))
	{
		return NULL;
	}

	/* Reset to the beginning */
	downhill_Group_Position = -1;

	/* Return an entry */
	return downhill_Group_GetEntry();
}

/* Rewind the group file =================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
void setgrent(void)
{
	/* Reset to the beginning */
	downhill_Group_Position = -1;

	return;
}
#endif

/* Stop using the group file =============================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
void endgrent(void)
{
	/* Reset to the beginning */
	downhill_Group_Position = -1;

	return;
}
#endif

/* Get the number of groups ================================================ */
int getgroups(int group_Max,gid_t group_Id[])
{
	if (group_Max < 1)
	{
		return 1;
	}

	group_Id[0] = Downhill_Group_IdReal;
	return 1;
}
