/*****************************************************************************
 *                                                                           *
 * DOWNHILL.C                                                                *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <dirent.h>
#include <strings.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/utsname.h>
#ifdef WIN32
#include "downhill.h"
#endif


/* Function stuff ************************************************************/

/* Fork-and-exec a process ================================================= */
int process_Forkexec()
{
	pid_t process_Id;

#ifdef WIN32
	process_Id = Downhill_Process_Forkexec("DOWNHILL.EXE -x 42",NULL,NULL,
	 CREATE_NEW_CONSOLE);
	if (process_Id <= 0)
	{
		printf("\tError fork-and-execing!  (Errno %d)\n",errno);
		return -1;
	}
#else
	process_Id = fork();
	if (process_Id < 0)
	{
		printf("\tError forking!  (Errno %d)\n",errno);
		return -1;
	}
	else if (process_Id == 0)
	{
		if (execl("DOWNHILL","DOWNHILL","-x","42",(char*)NULL) < 0)
		{
			printf("\tError execing!  (Errno %d)\n",errno);
			return -1;
		}
	}
#endif

	return 0;
}

/* Catch signals =========================================================== */
void sig_Catch(sig_Number)
int sig_Number;
{
	printf("\tCaught signal %d\n",sig_Number);
}


/* Main ==================================================================== */
int main(arg_Count,arg_Value)
int   arg_Count;
char* arg_Value[];
{
#ifdef WIN32
	Downhill_User_Id = 1;
#endif

	/* Get arguments --------------------------------------------------- */
	{
		int arg_Character;
		while ((arg_Character = getopt(arg_Count,arg_Value,"x:")) !=
		 EOF)
		{
			switch (arg_Character)
			{
				case 'x':
					printf(
					 "\tI'm a fork-and-exec'd process!\n");
					sleep(5);
					exit(atoi(optarg));
				default:
					exit(-1);
			}
		}
	}

	/* Directory ------------------------------------------------------- */
	{
		DIR*           dir_Info;
		struct dirent* dirent_Info;

		printf("Directory stuff:\n");

		dir_Info = opendir(".");
		if (dir_Info != NULL)
		{
			printf("\tContents of current directory:\n");
			while ((dirent_Info = readdir(dir_Info)) != NULL)
			{
				printf("\t\t%s\n",dirent_Info->d_name);
			}
			printf("\n");
		}
		else
		{
			printf("\tError opendiring!  (Errno %d)\n",errno);
		}

		printf("\tStatus of mkdir(): %d\n",mkdir("FOO",0));
	}

	/* File ------------------------------------------------------------ */
	{
		struct stat file_Info;

		printf("\nFile stuff:\n");
		if (stat("FOO",&file_Info) == 0)
		{
			printf("\tFOO is owned by uid %d\n",file_Info.st_uid);
		}
		else
		{
			printf("\tError stating!  (Errno %d)\n",errno);
		}
	}

	/* User ------------------------------------------------------------ */
	{
		struct passwd* user_Info;

		printf("\nUser stuff:\n");
		while ((user_Info = getpwent()) != NULL)
		{
			printf("\tName: %s\tId: %d\n",user_Info->pw_name,
			 user_Info->pw_uid);
		}
	}

	/* Group ----------------------------------------------------------- */
	{
		struct group* group_Info;

		printf("\nGroup stuff:\n");
		while ((group_Info = getgrent()) != NULL)
		{
			printf("\tName: %s\tId: %d\n",group_Info->gr_name,
			 group_Info->gr_gid);
		}
	}

	/* Misc ------------------------------------------------------------ */
	{
		struct utsname sys_Info;

		printf("\nMisc stuff:\n");

		/* NOTE:  Under Win32 this returns "localhost" because Winsock
		   hasn't been started up.  Your job. */
		uname(&sys_Info);
		printf("\t%s (%s, v%s.%s; %s)\n",sys_Info.nodename,
		 sys_Info.sysname,sys_Info.version,sys_Info.release,
		 sys_Info.machine);
	}

	/* String ---------------------------------------------------------- */
	{
		char* string_One = "HELLO";
		char* string_Two = "hello";

		printf("\nString stuff:\n");
		printf("\tstrcmp(%s,%s) = %d\n",string_One,string_Two,
		 strcmp(string_One,string_Two));
		printf("\tstrcasecmp(%s,%s) = %d\n",string_One,string_Two,
		 strcasecmp(string_One,string_Two));
	}

	/* Process and signal ---------------------------------------------- */
	{
		struct sigaction sigaction_Info;
		sigset_t         sigset_Mask;

		printf("\nProcess and signal stuff:\n");

		/* Set up the signal handler */
		sigemptyset(&sigset_Mask);
		sigaddset(&sigset_Mask,SIGINT);
		sigaction_Info.sa_handler = sig_Catch;
		sigaction_Info.sa_mask = sigset_Mask;
		sigaction_Info.sa_flags = 0;
		sigaction(SIGCHLD,&sigaction_Info,NULL);

		/* Fork-exec away! */
		if (process_Forkexec() == 0)
		{
			int process_Status;

			if (wait(&process_Status) > 0)
			{
				printf("\tExited with status %d\n",
				 WEXITSTATUS(process_Status));
			}
			else
			{
				printf("\tError waiting!  (Errno %d)\n",errno);
			}
		}
	}

	return 0;
}
