/*-------------------------------------------------------------
  sntp.c
  KAZUBON 1998-1999
               Special thanks to Tomoaki Nakashima
---------------------------------------------------------------*/

#include "tclock.h"

#include <winsock.h>
#include <ras.h>
#include "resource.h"

#undef RasEnumConnections
#undef RasGetConnectStatus

// globals
HWND hwndSNTP;     // for synchronizing time
HWND hwndSNTPLog;  // for SNTP Log

// functions
LRESULT CALLBACK SNTPWndProc(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam);
static BOOL IsConnecting(void);
static void SNTPStart(HWND hwnd, char *buf);
static void OnGetHost(HWND hwnd, WPARAM wParam, LPARAM lParam);
static void SNTPSend(HWND hwnd, unsigned long serveraddr);
static void OnReceive(HWND hwnd, WPARAM wParam, LPARAM lParam);
static void SynchronizeSystemTime(DWORD seconds, DWORD fractions);
static void SocketClose(HWND hwnd, const char *msgbuf);
static int GetServerPort(const char *buf, char *server);
static void Log(const char* msg);
void time2int(int* ph, int* pm, const char* src);

// statics
static BOOL bFirst = TRUE;
static BOOL bSendingData = FALSE;
static BOOL bSyncTimer = FALSE;
static BOOL bSyncADay = TRUE;
static int nMinutes = 60;
static BOOL bNoDial = TRUE;
static int nTimeout = 1000;
static int nLastDay = -1;
static int nLastMinute = -1;
static int nHourStart = -1, nMinuteStart = -1,
	nHourEnd = -1, nMinuteEnd = -1;
static int nMinuteDif = 0;
static DWORD dwTickCount = 0;
static DWORD dwTickCountOnGetHost = 0;
static DWORD dwTickCountOnSend = 0;
DWORD (WINAPI *RasEnumConnections)(LPRASCONN, LPDWORD, LPDWORD);
DWORD (WINAPI *RasGetConnectStatus)(HRASCONN, LPRASCONNSTATUS);
static HMODULE hRASAPI = NULL;

static char* g_pGetHost = NULL; // buffer of host entry
static HANDLE g_hGetHost;       // task handle of WSAAsyncGetHostByName()
static int g_socket;   // socket
static int g_port;     // port numer

struct NTP_Packet {          // NTP packet
	unsigned int Control_Word;
	int root_delay;
	int root_dispersion;
	int reference_identifier;
	__int64 reference_timestamp;
	__int64 originate_timestamp;
	__int64 receive_timestamp;
	int transmit_timestamp_seconds;
	int transmit_timestamp_fractions;
};

// notification of event to get host
#define WSOCK_GETHOST (WM_USER+1)
// notification of socket event
#define WSOCK_SELECT  (WM_USER+2)

/*---------------------------------------------------
    create a window, initialize WinSock
---------------------------------------------------*/
BOOL InitSyncTime(HWND hwndParent)
{
	char classname[] = "TClockSNTPWnd";
	WNDCLASS wndclass;
	WORD wVersionRequested;
	int  nErrorStatus;
	WSADATA wsaData;

	g_socket = -1;
	g_hGetHost = NULL;

	// initialize WinSock
	wVersionRequested = MAKEWORD(1, 1);
	nErrorStatus = WSAStartup(wVersionRequested, &wsaData);
	if(nErrorStatus != 0)
	{
		Log("failed to initialize");
		return FALSE;
	}

	// load DLL of RAS API

	if(!GetMyRegLong("SNTP", "NoRASAPI", FALSE))
	{
		hRASAPI = LoadLibrary("RASAPI32.dll");
		if(hRASAPI)
		{
			(FARPROC)RasEnumConnections =
				GetProcAddress(hRASAPI, "RasEnumConnectionsA");
			(FARPROC)RasGetConnectStatus =
				GetProcAddress(hRASAPI, "RasGetConnectStatusA");
			if(RasEnumConnections == NULL || RasGetConnectStatus == NULL)
			{
				FreeLibrary(hRASAPI); hRASAPI = NULL;
			}
		}
	}

	// register a window class and create
	wndclass.style         = 0;
	wndclass.lpfnWndProc   = SNTPWndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = g_hInst;
	wndclass.hIcon         = NULL;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = classname;
	RegisterClass(&wndclass);
	hwndSNTP = CreateWindow(classname, "TClockSNTP",
		WS_CHILD, 0, 0, 0, 0,
		hwndParent, (HMENU)1, g_hInst, NULL);

	hwndSNTPLog = CreateWindowEx(WS_EX_CLIENTEDGE, "listbox", "",
		WS_CHILD|WS_VSCROLL|WS_TABSTOP,
		0, 0, 0, 0,
		hwndSNTP, (HMENU)1, g_hInst, NULL);

	InitSyncTimeSetting();

	return TRUE;
}

/*---------------------------------------------------
	window procedure to process WinSock
---------------------------------------------------*/
LRESULT CALLBACK SNTPWndProc(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WSOCK_GETHOST: // get IP address
			OnGetHost(hwnd, wParam, lParam);
			return 0;
		case WSOCK_SELECT:  // receive SNTP data
			OnReceive(hwnd, wParam, lParam);
			return 0;
		case (WM_USER + 10):  // command to start
			SNTPStart(hwnd, (char*)lParam);
			return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*---------------------------------------------------
	if RASAPI.dll exists
---------------------------------------------------*/
BOOL IsRASAPI(void)
{
	return (hRASAPI != NULL);
}

/*---------------------------------------------------
    load settings for time syncronizing
---------------------------------------------------*/
void InitSyncTimeSetting(void)
{
	char section[] = "SNTP";
	char s[80];

	bSyncTimer = GetMyRegLong(section, "Timer", FALSE);
	bSyncADay = GetMyRegLong(section, "ADay", TRUE);
	nMinutes = GetMyRegLong(section, "Minutes", 60);
	bNoDial = FALSE;
	if(hRASAPI)
		bNoDial = GetMyRegLong(section, "NoDial", TRUE);
	nLastDay = GetMyRegLong(section, "LastDay", -1);

	GetMyRegStr(section, "Begin", s, 80, "");
	if(s[0])
	{
		time2int(&nHourStart, &nMinuteStart, s);
		GetMyRegStr(section, "End", s, 80, "");
		if(s[0])
			time2int(&nHourEnd, &nMinuteEnd, s);
		else nHourStart = nMinuteStart = -1;
	}
}

/*-------------------------------------------
    called from main window procedure.
    clean up
---------------------------------------------*/
void EndSyncTime(HWND hwnd)
{
	hwnd = GetDlgItem(hwnd, 1);

	SocketClose(hwnd, NULL);

	if(hRASAPI) FreeLibrary(hRASAPI);
	hRASAPI = NULL;

	WSACleanup();
}

/*------------------------------------------------
    called when main window received WM_TIMER.
    determine to start or not every second.
--------------------------------------------------*/
void OnTimerSNTP(HWND hwnd, SYSTEMTIME* st)
{
	BOOL bDo = FALSE;

	if(bSendingData) // while sending/receiving
	{
		char msg[80];
		DWORD dif;
		dif = GetTickCount() - dwTickCountOnSend;
		if(dif >= (DWORD)nTimeout)  // check timeout
		{
			wsprintf(msg, "timeout (%04d)", dif);
			SocketClose(GetDlgItem(hwnd, 1), msg);
		}
		return;
	}

	if(g_hGetHost) // while getting host address
	{
		DWORD dif;
		dif = GetTickCount() - dwTickCountOnGetHost;
		if(dif >= 10000)
		{
			SocketClose(GetDlgItem(hwnd, 1),
				"failed to get IP address");
		}
		return;
	}

	if(!bSyncTimer) return;

	if(g_socket != -1 || g_hGetHost != NULL) return;

	if(nHourStart >= 0)
	{
		if((nHourStart <= nHourEnd &&
				(nHourStart <= st->wHour && st->wHour <= nHourEnd)) ||
		   (nHourStart > nHourEnd &&
		    	(nHourStart <= st->wHour || st->wHour <= nHourEnd)))
		{
			if(st->wHour == nHourStart &&
				st->wMinute < nMinuteStart) return;
			if(st->wHour == nHourEnd &&
				nMinuteEnd < st->wMinute) return;
		}
		else return;
	}

	if(bSyncADay)
	{
		if(st->wDay != nLastDay)
		{
			if(!bNoDial || IsConnecting()) bDo = TRUE;
		}
	}
	else
	{
		if(bFirst ||
			(GetTickCount()-dwTickCount >= (DWORD)nMinutes*60*1000))
		{
			if(!bNoDial || (st->wMinute != nLastMinute && IsConnecting()))
				bDo = TRUE;
		}
	}
	nLastMinute = st->wMinute;

	if(bDo)
		StartSyncTime(hwnd, NULL, 0);
}

/*---------------------------------------------------
	check RAS connection
---------------------------------------------------*/
BOOL IsConnecting(void)
{
	RASCONN rc;
	RASCONNSTATUS rcs;
	DWORD cb, cConnections;

	if(!hRASAPI) return FALSE;

	memset(&rc, 0, sizeof(rc));
	rc.dwSize = sizeof(rc);
	cb = sizeof(rc);
	if(RasEnumConnections(&rc, &cb, &cConnections) == 0 &&
		cConnections > 0)
	{
		memset(&rcs, 0, sizeof(rcs));
		rcs.dwSize = sizeof(rcs);
		if(RasGetConnectStatus(rc.hrasconn, &rcs) == 0 &&
			rcs.rasconnstate == RASCS_Connected) return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------
    command to start syncronizing
---------------------------------------------------*/
void StartSyncTime(HWND hwndParent, char* pServer, int nto)
{
	HWND hwnd;
	SYSTEMTIME st;
	char section[] = "SNTP";
	char server[80], s[80];

	if(g_socket != -1 || g_hGetHost != NULL) return;

	if(pServer == NULL || *pServer == 0)
	{
		GetMyRegStr(section, "Server", server, 80, "");
		pServer = server;
	}
	if(nto == 0)
		nto = GetMyRegLong(section, "Timeout", 1000);

	if(*pServer == 0) return;
	hwnd = GetDlgItem(hwndParent, 1);
	if(!hwnd) return;

	nMinuteDif = 0;
	GetMyRegStr(section, "Dif", s, 80, "");
	if(s[0])
	{
		int h, m;
		time2int(&h, &m, s);
		nMinuteDif = h * 60 + m;
	}

	GetLocalTime(&st);
	nLastDay = st.wDay;
	SetMyRegLong(section, "LastDay", nLastDay);
	dwTickCount = GetTickCount();
	bFirst = FALSE;

	nTimeout = nto;

	SNTPStart(hwnd, pServer);
}

/*---------------------------------------------------
	start SNTP session
---------------------------------------------------*/
void SNTPStart(HWND hwnd, char *buf)
{
	char servername[256];
	unsigned long serveraddr;

	if(g_socket != -1 || g_hGetHost != NULL) return;

	// get server name and port
	g_port = GetServerPort(buf, servername);
	if(g_port == -1)
	{
		Log("invalid server name"); return;
	}

	// make a socket
	g_socket = socket(PF_INET, SOCK_DGRAM, 0);
	if(g_socket == INVALID_SOCKET)
	{
		Log("socket() failed");
		g_socket = -1; return;
	}

	serveraddr = inet_addr((char*)servername);
	// if server name is not "XXX.XXX.XXX.XXX"
	if(serveraddr == -1)
	{
		// request IP address
		g_pGetHost = malloc(MAXGETHOSTSTRUCT);
		g_hGetHost = WSAAsyncGetHostByName(hwnd, WSOCK_GETHOST,
			servername, g_pGetHost, MAXGETHOSTSTRUCT);
		if(g_hGetHost == 0)
		{
			SocketClose(hwnd, "WSAAsyncGetHostByName() failed");
			return;
		}
		dwTickCountOnGetHost = GetTickCount();
		return;
	}

	// send data
	SNTPSend(hwnd, serveraddr);
}

/*---------------------------------------------------
	called when the window received WSOCK_GETHOST.
	get IP address and send data.
---------------------------------------------------*/
void OnGetHost(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	struct hostent *pHostEnt;
	unsigned long serveraddr;

	if(g_hGetHost == NULL) return;

	// success ?
	if(WSAGETASYNCERROR(lParam) != 0)
	{
		SocketClose(hwnd, "failed to get IP address");
		return;
	}

	// valid handle ?
	if(g_hGetHost != (HANDLE)wParam) return;

	// get IP address
	pHostEnt = (struct hostent *)g_pGetHost;
	serveraddr =  *((unsigned long *)((pHostEnt->h_addr_list)[0]));
	free(g_pGetHost); g_pGetHost = NULL;
	g_hGetHost = NULL;

	// send data
	SNTPSend(hwnd, serveraddr);
}

/*---------------------------------------------------
	send SNTP data
---------------------------------------------------*/
void SNTPSend(HWND hwnd, unsigned long serveraddr)
{
	struct sockaddr_in serversockaddr;
	struct NTP_Packet NTP_Send;
	unsigned int sntpver;
	unsigned int Control_Word;

	// request notification of events
	if(WSAAsyncSelect(g_socket, hwnd, WSOCK_SELECT, FD_READ) == SOCKET_ERROR)
	{
		SocketClose(hwnd, "WSAAsyncSelect() failed");
		return;
	}

	// set IP address and port
	serversockaddr.sin_family = AF_INET;
	serversockaddr.sin_addr.s_addr = serveraddr;
	serversockaddr.sin_port = htons((unsigned short)g_port);
	memset(serversockaddr.sin_zero,(int)0,sizeof(serversockaddr.sin_zero));


	// init a packet
	memset(&NTP_Send, 0, sizeof(struct NTP_Packet));
	// NTP/SNTP version number = 4
	// Mode = 3 (client)
	// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	// |LI | VN  |Mode |    Stratum    |     Poll      |   Precision   |
	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	sntpver = GetMyRegLong("SNTP", "SNTPVer", 4);
	Control_Word = (sntpver << 27) | (3 << 24);
	NTP_Send.Control_Word = htonl(Control_Word);

	// send a packet
	if(sendto(g_socket, (const char *)&NTP_Send, sizeof(NTP_Send), 0,
		(struct sockaddr *)&serversockaddr,
		sizeof(serversockaddr)) == SOCKET_ERROR)
	{
		SocketClose(hwnd, "sendto() failed");
		return;
	}


	// save tickcount
	dwTickCountOnSend = GetTickCount();
	bSendingData = TRUE;
}

/*---------------------------------------------------
	called when the window received WSOCK_SELECT.
	receive SNTP data and set time.
---------------------------------------------------*/
void OnReceive(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	struct sockaddr_in serversockaddr;
	struct NTP_Packet NTP_Recv;
	int sockaddr_Size;

	if(g_socket == -1) return;
	if(WSAGETSELECTERROR(lParam))
	{
		SocketClose(hwnd, "failed to receive");
		return;
	}
	if(g_socket != (int)wParam ||
		WSAGETSELECTEVENT(lParam) != FD_READ) return;

	// receive data
	sockaddr_Size = sizeof(serversockaddr);
	if(recvfrom(g_socket, (char *)&NTP_Recv, sizeof(NTP_Recv), 0,
		(struct sockaddr *)&serversockaddr, &sockaddr_Size) == SOCKET_ERROR)
	{
		SocketClose(hwnd, "recvfrom() failed");
		return;
	}

	// if Leap Indicator is 3
	/*
	if(ntohl(NTP_Recv.Control_Word) >> 30 == 3)
	{
		SocketClose(hwnd, "server is unhealthy");
		return;
	}
	*/

	// set system time
	SynchronizeSystemTime(ntohl(NTP_Recv.transmit_timestamp_seconds),
		ntohl(NTP_Recv.transmit_timestamp_fractions));

	// close socket
	SocketClose(hwnd, NULL);
}

/*---------------------------------------------------
	set system time to received data
---------------------------------------------------*/
void SynchronizeSystemTime(DWORD seconds, DWORD fractions)
{
	FILETIME ft, ftold;
	SYSTEMTIME st, st_dif, lt;
	char s[1024];
	DWORD sr_time;
	DWORDLONG dif;
	BOOL b;

	// timeout ?
	sr_time = GetTickCount() - dwTickCountOnSend;
	if(sr_time >= (DWORD)nTimeout)
	{
		wsprintf(s, "timeout (%04d)", sr_time);
		Log(s); return;
	}

	// current time
	GetSystemTimeAsFileTime(&ftold);

	// NTP data -> FILETIME
	*(DWORDLONG*)&ft =
		// seconds from 1900/01/01 ¨ 100 nano-seconds from 1601/01/01
		M32x32to64(seconds, 10000000) + 94354848000000000i64;

	// difference
	if(nMinuteDif > 0)
		*(DWORDLONG*)&ft += M32x32to64(nMinuteDif * 60, 10000000);
	else if(nMinuteDif < 0)
		*(DWORDLONG*)&ft -= M32x32to64(-nMinuteDif * 60, 10000000);

	// set system time
	b = FileTimeToSystemTime(&ft, &st);
	if(b)
	{
		/* fractions: (2 ** 32 / 1000) */
		st.wMilliseconds = (WORD)(fractions / 4294967);
		b = SetSystemTime(&st);
	}
	if(!b)
	{
		Log("failed to set time"); return;
	}

	GetLocalTime(&lt);
	nLastDay = lt.wDay;
	SetMyRegLong("SNTP", "LastDay", nLastDay);

	SystemTimeToFileTime(&st, &ft);
	// delayed or advanced
	b = (*(DWORDLONG*)&ft > *(DWORDLONG*)&ftold);
	// get difference
	if(b) dif = *(DWORDLONG*)&ft - *(DWORDLONG*)&ftold;
	else  dif = *(DWORDLONG*)&ftold - *(DWORDLONG*)&ft;
	FileTimeToSystemTime((FILETIME*)&dif, &st_dif);

	// save log
	strcpy(s, "synchronized ");
	if(st_dif.wYear == 1601 && st_dif.wMonth == 1 &&
		st_dif.wDay == 1 && st_dif.wHour == 0)
	{
		strcat(s, b?"+":"-");
		wsprintf(s + strlen(s), "%02d:%02d.%03d ",
			st_dif.wMinute, st_dif.wSecond, st_dif.wMilliseconds);
	}
	wsprintf(s + strlen(s), "(%04d)", sr_time);
	Log(s);

	GetMyRegStr("SNTP", "Sound", s, 1024, "");
	PlayFile(g_hwndMain, s, 0);
}

/*---------------------------------------------------
	close a socket
---------------------------------------------------*/
void SocketClose(HWND hwnd, const char *msgbuf)
{
	// cancel task handle of WSAAsyncGetHostByName()
	if(g_hGetHost != NULL) WSACancelAsyncRequest(g_hGetHost);
	g_hGetHost = NULL;
	// free memory
	if(g_pGetHost) free(g_pGetHost);
	g_pGetHost = NULL;

	if(g_socket != -1)
	{
		// cancel request of notification
		WSAAsyncSelect(g_socket, hwnd, 0, 0);
		// close socket
		closesocket(g_socket);
	}
	g_socket = -1;
	bSendingData = FALSE;

	if(msgbuf) Log(msgbuf);
}

/*---------------------------------------------------
	get server name and port number from string
		buf: "ntp.xxxxx.ac.jp:123"
---------------------------------------------------*/
int GetServerPort(const char *buf, char *server)
{
	char *p;
	int port = 123;

	if(strcmp(buf,"") == 0) return -1;
	strcpy(server, buf);

	for(p = server; *p != ':' && *p != '\0'; p++);
	if(*p == ':')
	{
		*p = 0; p++; port = 0;
		while(*p)
		{
			if('0' <= *p && *p <= '9')
				port = port * 10 + *p - '0';
			else
			{
				port = -1; break;
			}
			p++;
		}
	}
	return port;
}

/*-------------------------------------------
	save log data
---------------------------------------------*/
void Log(const char* msg)
{
	SYSTEMTIME st;
	char s[160];
	int count, index;

	GetLocalTime(&st);
	wsprintf(s, "%02d/%02d %02d:%02d:%02d ",
		st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	strcat(s, msg);

	// save to listbox
	count = SendMessage(hwndSNTPLog, LB_GETCOUNT, 0, 0);
	if(count > 100)
		SendMessage(hwndSNTPLog, LB_DELETESTRING, 0, 0);
	index = SendMessage(hwndSNTPLog, LB_ADDSTRING, 0, (LPARAM)s);
	SendMessage(hwndSNTPLog, LB_SETCURSEL, index, 0);

	// save to file
	if(GetMyRegLong("SNTP", "SaveLog", TRUE))
	{
		HFILE hf;
		char fname[MAX_PATH];

		strcpy(fname, g_mydir);
		add_title(fname, "SNTP.txt");
		hf = _lopen(fname, OF_WRITE);
		if(hf == HFILE_ERROR)
			hf = _lcreat(fname, 0);
		if(hf == HFILE_ERROR) return;
		_llseek(hf, 0, 2);
		_lwrite(hf, s, strlen(s));
		_lwrite(hf, "\x0d\x0a", 2);
		_lclose(hf);
	}
}

/*-------------------------------------------
	"XX:XX" -> two integers
---------------------------------------------*/
void time2int(int* ph, int* pm, const char* src)
{
	const char* p;
	BOOL bminus;

	p = src;
	*ph = 0; *pm = 0;
	bminus = FALSE;
	if(*p == '-') { p++; bminus = TRUE; }
	while('0' <= *p && *p <='9')
		*ph = *ph * 10 + *p++ - '0';
	if(bminus) *ph *= -1;
	if(*p == ':') p++; else return;
	while('0' <= *p && *p <='9')
		*pm = *pm * 10 + *p++ - '0';
	if(bminus) *pm *= -1;
}
