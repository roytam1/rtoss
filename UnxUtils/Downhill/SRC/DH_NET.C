/*****************************************************************************
 *                                                                           *
 * DH_NET.C                                                                  *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


/* Include stuff *************************************************************/
#include "INCLUDE/DH.H"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <sys/param.h>
#include <strings.h>


/* Define stuff **************************************************************/
#define DOWNHILL_RCMD_TIMEOUT  32
#define DOWNHILL_REXEC_TIMEOUT 16


/* Function stuff ************************************************************/

/* Report a host error ===================================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
void herror(char* error_Prefix)
{
	/* Use perror(), 'cause there isn't an h_error */
	perror(error_Prefix);
}
#endif

/* Get user and password information from .netrc =========================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
static char* downhill_Ruserpass_GetWord(FILE* file_Fs)
{
	static char word_Text[1024];
	int         word_Position;
	static char line_Text[1024];
	static int  line_Position = 0;

	strcpy(word_Text,"");
	word_Position = 0;

	/* Loop to find a word */
	do
	{
		/* Get a new line */
		if (line_Text[line_Position] == '\0')
		{
			if (fgets(line_Text,sizeof(line_Text),file_Fs) == NULL)
			{
				return NULL;
			}
			line_Position = 0;
		}

		/* Skip any white space */
		while ((line_Text[line_Position] == ' ') ||
		 (line_Text[line_Position] == '\t') ||
		 (line_Text[line_Position] == '\n'))
		{
			line_Position++;
		}
	
		/* Get the word */
		while ((line_Text[line_Position] != ' ') &&
		 (line_Text[line_Position] != '\t') &&
		 (line_Text[line_Position] != '\n') &&
		 (line_Text[line_Position] != '\0'))
		{
			word_Text[word_Position] = line_Text[line_Position];
			word_Text[word_Position+1] = '\0';
			word_Position++;
			line_Position++;
		}
	} while (!strcmp(word_Text,""));

	/* Return what we got */
	return word_Text;
}
void ruserpass(char* host_Name,char** user_Name,char** user_Password)
{
	FILE*        file_Fs;
	char         netrc_File[MAXPATHLEN+1];
	char*        netrc_Word;
	static char  netrc_Name[32];
	static char  netrc_Password[32];
	int          machine_Match = 0;
	int          machine_Matched = 0;

	/* Open the file */
	strcpy(netrc_File,Downhill_User_Dir);
	strcat(netrc_File,"\\");
	strcat(netrc_File,_DOWNHILL_NETRC_FILE);
	file_Fs = fopen(netrc_File,"rt");
	if (file_Fs == NULL)
	{
		return;
	}

	/* Loop through the file */
	do
	{
		netrc_Word = downhill_Ruserpass_GetWord(file_Fs);
		if (netrc_Word == NULL)
		{
			break;
		}
		else if (!strcasecmp(netrc_Word,"machine"))
		{
			netrc_Word = downhill_Ruserpass_GetWord(file_Fs);
			if (netrc_Word != NULL)
			{
				machine_Match = (!strcasecmp(netrc_Word,
				 host_Name));
				if (machine_Match)
				{
					machine_Matched = 1;
				}
			}
		}
		else if (!strcasecmp(netrc_Word,"default"))
		{
			if (!machine_Matched)
			{
				machine_Match = 1;
			}
			else
			{
				machine_Match = 0;
			}
		}
		else if (!strcasecmp(netrc_Word,"login"))
		{
			netrc_Word = downhill_Ruserpass_GetWord(file_Fs);
			if ((netrc_Word != NULL) && (machine_Match))
			{
				strcpy(netrc_Name,netrc_Word);
				*user_Name = netrc_Name;
			}
		}
		else if (!strcasecmp(netrc_Word,"password"))
		{
			netrc_Word = downhill_Ruserpass_GetWord(file_Fs);
			if ((netrc_Word != NULL) && (machine_Match))
			{
				strcpy(netrc_Password,netrc_Word);
				*user_Password = netrc_Password;
			}
		}
	} while (netrc_Word != NULL);

	return;
}
#endif

/* Execute a password-protected remote command ============================= */
/* This is based on the BSD rexec. */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int rexec(char** server_Host,unsigned short server_Port,char* server_User,
     char* server_Password,char* server_Command,int* client_Stderr)
{
	struct hostent*    server_Info;
	struct sockaddr_in client_AddrStdout;
	int                client_Stdout;
	struct sockaddr_in client_AddrStderr;
	unsigned short     client_PortStderr = 0;
	int                sec_Count = 0;
	char               result_Character;
	
	/* Normalize the host */
	server_Info = gethostbyname(*server_Host);
	if (server_Info == NULL)
	{
		herror(*server_Host);
		return -1;
	}
	*server_Host = server_Info->h_name;

	/* Get any additional information */
	ruserpass(server_Info->h_name,&server_User,&server_Password);

retry:
	/* Open stdout ----------------------------------------------------- */
	client_Stdout = socket(AF_INET,SOCK_STREAM,0);
	if (client_Stdout < 0)
	{
		perror("rexec: socket");
		return -1;
	}
	client_AddrStdout.sin_family = server_Info->h_addrtype;
	client_AddrStdout.sin_port = htons(server_Port);
	memcpy((caddr_t)(&client_AddrStdout.sin_addr),server_Info->h_addr,
	 server_Info->h_length);
	if (connect(client_Stdout,(struct sockaddr*)(&client_AddrStdout),
	 sizeof(client_AddrStdout)) < 0)
	{
		/* Close the socket */
		closesocket(client_Stdout);

		/* Should we retry? */
		if ((errno == ECONNREFUSED) &&
		 (sec_Count <= DOWNHILL_REXEC_TIMEOUT))
		{
			/* Wait a bit */
			sleep(sec_Count);
			sec_Count *= 2;
			goto retry;
		}

		/* Couldn't get the connection */
		perror(server_Info->h_name);
		return -1;
	}

	/* Open stderr ----------------------------------------------------- */
	if (client_Stderr == NULL)
	{
		send(client_Stdout,"\0",1,0);
		client_PortStderr = 0;
	}
	else
	{
		struct sockaddr_in temp_Addr;
		int                temp_AddrSize;
		char               temp_Port[8];
		int                temp_Socket;
		int                temp_Socket2;
		
		temp_Socket = socket(AF_INET,SOCK_STREAM,0);
		if (temp_Socket < 0)
		{
			goto bad;
		}
		listen(temp_Socket,1);
		temp_AddrSize = sizeof(client_AddrStderr);
		if ((getsockname(temp_Socket,
		 (struct sockaddr*)(&client_AddrStderr),&temp_AddrSize) < 0) ||
		 (temp_AddrSize != sizeof(client_AddrStderr)))
		{
			perror("getsockname");
			closesocket(temp_Socket);
			goto bad;
		}
		client_PortStderr =
		 ntohs((unsigned short)client_AddrStderr.sin_port);
		sprintf(temp_Port,"%u",client_PortStderr);
		write(client_Stdout,temp_Port,strlen(temp_Port)+1);
		temp_AddrSize = sizeof(temp_Addr);
		temp_Socket2 = accept(temp_Socket,
		 (struct sockaddr*)(&temp_Addr),&temp_AddrSize);
		closesocket(temp_Socket);
		if (temp_Socket2 < 0)
		{
			perror("accept");
			client_PortStderr = 0;
			goto bad;
		}
		*client_Stderr = temp_Socket2;
	}

	/* Send the command ------------------------------------------------ */
	send(client_Stdout,server_User,strlen(server_User)+1,0);
	send(client_Stdout,server_Password,strlen(server_Password)+1,0);
	send(client_Stdout,server_Command,strlen(server_Command)+1,0);

	/* Handle result --------------------------------------------------- */
	if (recv(client_Stdout,&result_Character,1,0) != 1)
	{
		perror(*server_Host);
		goto bad;
	}
	if (result_Character != 0)
	{
		/* Read the fail message */
		while (recv(client_Stdout,&result_Character,1,0) == 1)
		{
			write(fileno(stderr),&result_Character,1);
			if (result_Character == '\n')
			{
				break;
			}
		}
		goto bad;
	}

	return client_Stdout;

bad:
	if (client_PortStderr)
	{
		closesocket(*client_Stderr);
	}
	closesocket(client_Stdout);
	return -1;
}
#endif

/* Open a socket to a reserved port ======================================== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int rresvport(unsigned short* client_Port)
{
	int                client_Sd;
	struct sockaddr_in client_Addr;

	/* Loop 'til we get something */
	(*client_Port)++;
	for (;;)
	{
		(*client_Port)--;
		if (*client_Port < 0)
		{
			errno = EAGAIN;
			return -1;
		}

		/* Open the socket */
		client_Sd = socket(PF_INET,SOCK_STREAM,0);
		if (client_Sd == -1)
		{
			return -1;
		}

		/* Bind to the port */
		client_Addr.sin_family = AF_INET;
		client_Addr.sin_port = htons(*client_Port);
		client_Addr.sin_addr.s_addr = INADDR_ANY;
		if (bind(client_Sd,(struct sockaddr*)(&client_Addr),
		 sizeof(client_Addr)) == -1)
		{
			int error_Number = GetLastError();

			/* Close the socket */
			closesocket(client_Sd);

			/* Try again */
			if (error_Number == EADDRINUSE)
			{
				continue;
			}

			return -1;
		}

		break;
	}

	/* Return the socket */
	return client_Sd;
}
#endif

/* Execute a remote command ================================================ */
/* This is based on the BSD rcmd. */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int rcmd(char** server_Host,unsigned short server_Port,char* client_User,
     char* server_User,char* server_Command,int* client_Stderr)
{
	int                client_Stdout;
	unsigned short     client_Port = IPPORT_RESERVED-1;
	struct sockaddr_in client_AddrStdout;
	struct sockaddr_in client_AddrStderr;
	struct hostent*    server_Info;
	int                sec_Count = 1;
	fd_set             sock_Set;
	char               result_Character;
	int                error_Number;

	/* The the info on the server */
	server_Info = gethostbyname(*server_Host);
	if (server_Info == NULL)
	{
		return -1;
	}
	*server_Host = server_Info->h_name;

	/* Find a socket --------------------------------------------------- */
	for (;;)
	{
		client_Stdout = rresvport(&client_Port);
		if (client_Stdout == -1)
		{
			if (errno == EAGAIN)
			{
				fprintf(stderr,"socket: All ports in use\n");
			}
			else
			{
				perror("rcmd: socket");
			}
			return -1;
		}

		/* Set up the socket */
		client_AddrStdout.sin_family = server_Info->h_addrtype;
		memcpy((caddr_t)(&client_AddrStdout.sin_addr),
		 server_Info->h_addr_list[0],server_Info->h_length);
		client_AddrStdout.sin_port = htons(server_Port);

		/* Connect */
		if (connect(client_Stdout,
		 (struct sockaddr*)(&client_AddrStdout),
		 sizeof(client_AddrStdout)) >= 0)
		{
			/* Connect worked */
			break;
		}
		error_Number = GetLastError();

		/* Connect failed */
		closesocket(client_Stdout);

		/* Refused? */
		if ((error_Number == ECONNREFUSED) &&
		 (sec_Count <= DOWNHILL_RCMD_TIMEOUT))
		{
			/* Wait a bit and try again */
			sleep(sec_Count);
			sec_Count *= 2;
			continue;
		}

		/* If there's other addresses, try 'em */
		if (server_Info->h_addr_list[1] != NULL)
		{
			int errno_Old = errno;

			fprintf(stderr,"connect to address %s: ",
			 inet_ntoa(client_AddrStdout.sin_addr));
			errno = errno_Old;
			perror(NULL);

			server_Info->h_addr_list++;
			memcpy((caddr_t)(&client_AddrStdout.sin_addr),
			 server_Info->h_addr_list[0],server_Info->h_length);
			fprintf(stderr,"Trying %s...\n",
			 inet_ntoa(client_AddrStdout.sin_addr));

			continue;
		}

		/* Couldn't find any */
		perror(server_Info->h_name);
		return -1;
	}

	/* Open stderr ----------------------------------------------------- */
	if (client_Stderr == NULL)
	{
		send(client_Stdout,"",1,0);
		client_Port = 0;
	}
	else
	{
		/* Create the stderr socket */
		char sock_Port[8];
		int  sock_Temp;
		int  client_Stderr2;
		int  len;

		client_Port--;
		sock_Temp = rresvport(&client_Port);
		if (sock_Temp == -1)
		{
			goto bad;
		}
		if (listen(sock_Temp,1) == -1)
		{
			goto bad;
		}

		/* Tell the host were the stderr port is */
		sprintf(sock_Port,"%d",client_Port);
		if (send(client_Stdout,sock_Port,strlen(sock_Port)+1,0) !=
		 (int)strlen(sock_Port)+1)
		{
			perror("send: setting up stderr");
			closesocket(sock_Temp);
			goto bad;
		}

		FD_ZERO(&sock_Set);
		FD_SET(client_Stdout,&sock_Set);
		FD_SET(sock_Temp,&sock_Set);
		errno = 0;
		if ((select(FD_SETSIZE,&sock_Set,(fd_set *)NULL,(fd_set *)NULL,
		 (struct timeval *)NULL) < 1) ||
		 (!FD_ISSET(sock_Temp,&sock_Set)))
		{
			if (errno != 0)
			{
				perror("select: setting up stderr");
			}
			else
			{
				fprintf(stderr,
				 "select: protocol failure in setup.\n");
			}
			closesocket(sock_Temp);
			goto bad;
		}

		/* Wait for the host to connect */
		len = sizeof(client_AddrStderr);
		client_Stderr2 = accept(sock_Temp,
		 (struct sockaddr*)&client_AddrStderr,&len);
		closesocket(sock_Temp);
		if (client_Stderr2 == -1)
		{
			perror("accept");
			client_Port = 0;
			goto bad;
		}
		*client_Stderr = client_Stderr2;

		/* Make sure we connected right */
		client_AddrStderr.sin_port = ntohs(client_AddrStderr.sin_port);
		if ((client_AddrStderr.sin_family != AF_INET) ||
		 (client_AddrStderr.sin_port >= IPPORT_RESERVED) ||
		 (client_AddrStderr.sin_port < IPPORT_RESERVED/2))
		{
			fprintf(stderr,
			 "socket: protocol failure in circuit setup.\n");
			goto bad2;
		}
	}

	/* Send the command ------------------------------------------------ */
	send(client_Stdout,client_User,strlen(client_User)+1,0);
	send(client_Stdout,server_User,strlen(server_User)+1,0);
	send(client_Stdout,server_Command,strlen(server_Command)+1,0);

	/* Handle the result ----------------------------------------------- */
	if (recv(client_Stdout,&result_Character,1,0) != 1)
	{
		perror(*server_Host);
		goto bad2;
	}
	if (result_Character != 0)
	{
		/* The command failed, read the result */
		while (recv(client_Stdout,&result_Character,1,0) == 1)
		{
			write(fileno(stderr),&result_Character,1);
			if (result_Character == '\n')
			{
				break;
			}
		}
		goto bad2;
	}

	return client_Stdout;

bad2:
	if (client_Port)
	{
		closesocket(*client_Stderr);
	}
bad:
	closesocket(client_Stdout);
	return -1;
}
#endif

/* Check to see if the hostnames match ===================================== */
static int downhill_Ruserok_CheckHost(char* hostname_Client,
            char* hostname_Candidate)
{
	char hostname_CandidateFull[MAXHOSTNAMELEN+1];

	/* Build the name */
	strcpy(hostname_CandidateFull,hostname_Candidate);

	/* Does the candidate end in a "."? */
	if (hostname_CandidateFull[strlen(hostname_CandidateFull)-1] == '.')
	{
		char hostname_Local[MAXHOSTNAMELEN+1];

		/* Get the local domain */
		if (gethostname(hostname_Local,MAXHOSTNAMELEN) == 0)
		{
			char* domain_Local = strchr(hostname_Local,'.');

			/* If there's a domain, use it */
			if (domain_Local != NULL)
			{
				strcat(hostname_CandidateFull,domain_Local);
			}
		}
	}

	/* Disallowed? */
	if ((hostname_Candidate[0] == '-') &&
	 (!strcasecmp(hostname_Client,&(hostname_CandidateFull[1]))))
	{
		return FALSE;
	}

	/* Allowed? */
	if ((!strcmp("+",hostname_CandidateFull)) ||
	 (!strcasecmp(hostname_Client,hostname_CandidateFull)))
	{
		return TRUE;
	}

	/* Couldn't find */
	return FALSE;
}

/* Check if host and user are valid ======================================== */
static int downhill_Ruserok_CheckHostUser(FILE* file_Fs,char* hostname_Client,
            char* username_Server,char* username_Client)
{
	char          line_Text[MAXHOSTNAMELEN+1];
	unsigned int  line_Index = 0;
	char          hostname_Candidate[MAXHOSTNAMELEN+1];
	unsigned int  hostname_Index = 0;
	char          username_Candidate[MAXHOSTNAMELEN+1];
	unsigned int  username_Index;

	/* Loop through all the hostnames in the file */
	while (fgets(line_Text,sizeof(line_Text),file_Fs))
	{
		/* Clear out the candidates */
		strcpy(hostname_Candidate,"");
		hostname_Index = 0;

		/* Clear out any leading white-space */
		line_Index = 0;
		while ((line_Text[line_Index] == ' ') ||
		 (line_Text[line_Index] == '\t'))
		{
			line_Index++;
		}

		/* Get the host name */
		while ((line_Text[line_Index] != '\n') &&
		 (line_Text[line_Index] != '\t') &&
		 (line_Text[line_Index] != ' ') &&
		 (line_Text[line_Index] != '\0'))
		{
			hostname_Candidate[hostname_Index] =
			 line_Text[line_Index];

			line_Index++;
			hostname_Index++;
		}
		hostname_Candidate[hostname_Index] = '\0';

		/* Do the host names match? */
		if (downhill_Ruserok_CheckHost(hostname_Client,
		 hostname_Candidate))
		{
			/* Is there a user name? */
			while ((line_Text[line_Index] == ' ') ||
			 (line_Text[line_Index] == '\t'))
			{
				line_Index++;
			}
			if ((line_Text[line_Index] == '\n') ||
			 (line_Text[line_Index] == '\0'))
			{
				/* If the client user is the same as the server
				   user, they we're OK */
				if (!strcasecmp(username_Client,
				 username_Server))
				{
					return TRUE;
				}
			}
			else
			{
				/* Loop through all the users */
				while ((line_Text[line_Index] != '\n') &&
				 (line_Text[line_Index] != '\0'))
				{
					/* Clear any white space */
					while ((line_Text[line_Index] == ' ')
					 || (line_Text[line_Index] == '\t'))
					{
						line_Index++;
					}
					if ((line_Text[line_Index] == '\n') ||
					 (line_Text[line_Index] == '\0'))
					{
						break;
					}

					/* Clear out the candidate */
					username_Index = 0;

					/* Get the user name */
					while ((line_Text[line_Index] != '\n')
					 && (line_Text[line_Index] != '\t') &&
					 (line_Text[line_Index] != ' ') &&
					 (line_Text[line_Index] != '\0'))
					{
						username_Candidate[
						 username_Index] = line_Text[
						 line_Index];
	
						line_Index++;
						username_Index++;
					}
					username_Candidate[username_Index] =
					 '\0';

					/* Are we explicitly excluded? */
					if ((username_Candidate[0] == '-') &&
					 (!strcasecmp(username_Client,
					 &(username_Candidate[1]))))
					{
						return FALSE;
					}

					/* Do we allowed? */
					if ((!strcmp("+",username_Candidate))
					 || (!strcasecmp(username_Client,
					 username_Candidate)))
					{
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

/* Check if a remote user can execute commands on this server ============== */
#if (_DOWNHILL_POSIX_SOURCE == 0)
int ruserok(char* hostname_Client,int root_Want,char* user_Client,
     char* user_Server)
{
	FILE* file_Fs;
	char  rhosts_File[MAXPATHLEN+1];

	/* If they don't want to be root, open up the "hosts.equiv" file */
	file_Fs = root_Want?(FILE*)NULL:fopen(_DOWNHILL_HOSTSEQUIV_PATH,"rt");

	/* If the "hosts.equiv" file opened up, check it */
	if (file_Fs != NULL)
	{
		if (downhill_Ruserok_CheckHostUser(file_Fs,hostname_Client,
		 user_Server,user_Client))
		{
			fclose(file_Fs);
			return 0;
		}
		fclose(file_Fs);
	}

	/* Try the .rhosts file */
	strcpy(rhosts_File,Downhill_User_Dir);
	strcat(rhosts_File,"\\");
	strcat(rhosts_File,_DOWNHILL_RHOSTS_FILE);
	file_Fs = fopen(rhosts_File,"rt");

	/* If it opened up, check it */
	if (file_Fs != NULL)
	{
		if (downhill_Ruserok_CheckHostUser(file_Fs,hostname_Client,
		 user_Server,user_Client))
		{
			fclose(file_Fs);
			return 0;
		}
		fclose(file_Fs);
	}

	/* Not allowed */
	return -1;
}
#endif
