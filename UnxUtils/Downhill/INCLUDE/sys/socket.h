/*****************************************************************************
 *                                                                           *
 * sys/socket.h                                                              *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *                                                                           *
 *****************************************************************************/


#if !defined(_DOWNHILL_SYS_SOCKET_H)
#define _DOWNHILL_SYS_SOCKET_H


/* Include stuff *************************************************************/
#include "DOWNHILL/DH_POSIX.H"
#include <winsock.h>


/* Define stuff **************************************************************/
#define EWOULDBLOCK  WSAEWOULDBLOCK
#define ECONNRESET   WSAECONNRESET
#define ETIMEDOUT    WSAETIMEDOUT
#define EADDRINUSE   WSAEADDRINUSE
#define ECONNREFUSED WSAECONNREFUSED


/* Prototype stuff ***********************************************************/
#if (_DOWNHILL_POSIX_SOURCE == 0)
int  ruserok(char* hostname_Client,int root_Want,char* user_Client,
      char* user_Server);
int  rresvport(unsigned short* client_Port);
int  rcmd(char** server_Host,unsigned short server_Port,char* client_User,
     char* server_User,char* server_Command,int* client_Stderr);
void ruserpass(char* host_Name,char** user_Name,char** user_Password);
int  rexec(char** server_Host,unsigned short server_Port,char* server_User,
      char* server_Password,char* server_Command,int* client_Stderr);
#endif


#endif /* _DOWNHILL_SYS_SOCKET_H */
