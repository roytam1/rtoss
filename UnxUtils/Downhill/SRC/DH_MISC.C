/*****************************************************************************
 *                                                                           *
 * DH_MISC.C                                                                 *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <stdio.h>
#include <sys/utsname.h>



/* Function stuff ************************************************************/

/* Sleep =================================================================== */
unsigned int sleep(unsigned int sleep_Duration)
{
	unsigned long sleep_Time = sleep_Duration*1000;

	/* While there's time left, keep sleeping */
	while (sleep_Time > 0)
	{
		/* Return if we were interrupted */
		if (downhill_Signal_IsInterrupted())
		{
			return ((unsigned int)(sleep_Time/1000))+1;
		}

		/* Sleep it off */
		Sleep(DOWNHILL_SLEEP_TIME);
		sleep_Time -= DOWNHILL_SLEEP_TIME;
	}

	return 0;
}

/* Pause until a signal ==================================================== */
int pause(void)
{
	/* Wait for a signal */
	for (;;)
	{
		/* And return if we were interrupted */
		if (sleep(1) > 0)
		{
			errno = EINTR;
			return -1;
		}
	}
}

/* Return version information ============================================== */
int uname(struct utsname* system_Info)
{
	DWORD version_Info = GetVersion();
	WORD  version_Number = LOWORD(version_Info);

	/* Get the OS name */
	if (HIBYTE(HIWORD(version_Info))&0x80)
	{
		strcpy(system_Info->sysname,"Windows");
	}
	else
	{
		strcpy(system_Info->sysname,"WindowsNT");
	}

	/* Get the nodename */
	if (gethostname(system_Info->nodename,MAXHOSTNAMELEN) == SOCKET_ERROR)
	{
		strcpy(system_Info->nodename,"localhost");
        }

	/* Get the version and release number */
	sprintf(system_Info->version,"%d",LOBYTE(version_Number));
	sprintf(system_Info->release,"%d",HIBYTE(version_Number));

	/* Get the machine type */
	strcpy(system_Info->machine,_DOWNHILL_MACHINE_TYPE);

	/* Fake an ID number */
	{
		unsigned int id_Number = 0;
		unsigned int id_Index;
		char         id_Temp[sizeof(system_Info->nodename)+
		              sizeof(system_Info->machine)+1];

		/* This is all completely arbitrary but reproducable */
		strcpy(id_Temp,system_Info->nodename);
		strcat(id_Temp,system_Info->machine);
		for (id_Index = 0;id_Index < strlen(id_Temp);id_Index++)
		{
			id_Number += id_Temp[id_Index]*id_Index;
		}
		sprintf(system_Info->idnumber,"%lx",id_Number);
	}

	return 0;
}


/* Get command-line flags ================================================== */
#if (_DOWNHILL_POSIX_SOURCE == -1)
/* This was originally the public domain AT&T getopt().  Mine's prettier, if
   you ask _me_. */

/* Global stuff **************************************************************/
int   opterr = 1;
int   optind = 1;
int   optopt;
char* optarg;

int getopt(int arg_Count,char* arg_Value[],char flag_List[])
{
	static int opt_Position = 1;
	char*      opt_Flag;

	/* If we're at the end of the list, return */
	if (opt_Position == 1)
	{
		if (optind >= arg_Count)
		{
			return EOF;
		}
		else if (((arg_Value[optind][0] != '-')
#if defined(_DOWNHILL_GETOPT_SWITCH)
		 && (arg_Value[optind][0] != _DOWNHILL_GETOPT_SWITCH)
#endif
		 ) || (arg_Value[optind][1] == '\0'))
		{
			return EOF;
		}
		else if (!strcmp(arg_Value[optind],"--"))
		{
			optind++;
			return EOF;
		}
	}

	/* Get this flag */
	optopt = arg_Value[optind][opt_Position];
	opt_Flag = strchr(flag_List,optopt);
	if ((optopt == ':') || (opt_Flag == NULL))
	{
		fprintf(stderr,"%s: illegal option -- %c\n",
		 arg_Value[0],optopt);
		opt_Position++;
		if (arg_Value[optind][opt_Position] == '\0')
		{
			optind++;
			opt_Position = 1;
		}
		return '?';
	}

	/* Does this flag take an argument? */
	opt_Flag++;
	if (*opt_Flag == ':')
	{
		opt_Position++;
		if (arg_Value[optind][opt_Position] != '\0')
		{
			opt_Position++;
			optarg = &(arg_Value[optind][opt_Position]);
		}
		else if (optind+1 >= arg_Count)
		{
			fprintf(stderr,
			 "%s: option requires an argument -- %c\n",
			 arg_Value[0],optopt);
			opt_Position = 1;
			return '?';
		}
		else
		{
			optind++;
			optarg = arg_Value[optind];
		}
		optind++;
		opt_Position = 1;
	}
	else
	{
		opt_Position++;
		if (arg_Value[optind][opt_Position] == '\0')
		{
			opt_Position = 1;
			optind++;
		}
		optarg = NULL;
	}

	/* Return the character */
	return optopt;
}
#endif

// kms
#include <io.h>
#include <fcntl.h>

int pipe(int p[])
{
int po[2],ret;

 ret=_pipe( po, 4096 , O_BINARY | O_NOINHERIT);
 if( ret <0 ) return ret;
 p[0]= _dup(po[0]); if( p[0] <0 ) return ret;
 p[1]= _dup(po[1]); if( p[1] <0 ) return ret;
close(po[0]);close(po[1]);

return 0;
}