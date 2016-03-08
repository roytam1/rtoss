
#include	"compiler.h"

#if defined(SUPPORT_LGY98)

#include <winioctl.h>

//#include	"lgy98dev.h"
#include	"net.h"

#pragma warning(disable: 4996)
 
#pragma comment(lib, "Advapi32.lib")
 
#define DEVICE_PATH_FMT "\\\\.\\Global\\%s.tap"
 
#define TAP_CONTROL_CODE(request,method) \
  CTL_CODE (FILE_DEVICE_UNKNOWN, request, method, FILE_ANY_ACCESS)
 
#define TAP_IOCTL_SET_MEDIA_STATUS \
  TAP_CONTROL_CODE (6, METHOD_BUFFERED)
 
#define TAP_BUFMAX (1024*10)
#define NET_BUFLEN (10*1024)
#define NET_ARYLEN (64)

//static LGY98*	np2net_lgy98 = NULL;
static HANDLE	np2net_hTap = NULL;
static HANDLE	np2net_hThread = NULL;
static HANDLE	np2net_hThreadW = NULL;
static int		np2net_hThreadexit = 0;
static int		np2net_BufLen = 0;
static int		np2net_BufReadyFlag = 0;
static UINT8*	np2net_Buf[TAP_BUFMAX] = {0};
static VLANClientState*	np2net_vc = NULL;
static CRITICAL_SECTION np2net_cs;

CHAR *GetNetWorkDeviceGuid(CONST CHAR *, CHAR *, DWORD);

static REG8		np2net_membuf[NET_ARYLEN][NET_BUFLEN]; // 送信用バッファ
static int		np2net_membuflen[NET_ARYLEN];
//static int		np2net_membuf_datalen = 0;
static int		np2net_membuf_readpos = 0;
static int		np2net_membuf_writepos = 0;

HANDLE np2net_write_hEvent;
OVERLAPPED np2net_write_ovl;

// パケットデータを TAP へ書き出す
int doWriteTap(HANDLE hTap, const UCHAR *pSendBuf, DWORD len)
{
  #define ETHERDATALEN_MIN 46
  DWORD dwWriteLen;

  if (!WriteFile(hTap, pSendBuf, len, &dwWriteLen, &np2net_write_ovl)) {
    DWORD err = GetLastError();
    if (err == ERROR_IO_PENDING) {
      WaitForSingleObject(np2net_write_hEvent, INFINITE); // 完了待ち
      GetOverlappedResult(hTap, &np2net_write_ovl, &dwWriteLen, FALSE);
    } else {
      TRACEOUT(("LGY-98: WriteFile err=0x%08X\n", err));
      return -1;
    }
  }
  //TRACEOUT(("LGY-98: send %u bytes\n", dwWriteLen));
  return 0;
}

int sendDataToBuffer(UCHAR *pSendBuf, DWORD len){
	//int wpos = np2net_membuf_writepos;
	if(len > NET_BUFLEN){
		TRACEOUT(("LGY-98: too large packet!! %d bytes", len));
		return 1;
	}
	if(np2net_membuf_readpos==(np2net_membuf_writepos+1)%NET_ARYLEN){
		TRACEOUT(("LGY-98: buffer full"));
		while(np2net_membuf_readpos==(np2net_membuf_writepos+1)%NET_ARYLEN){
			Sleep(0); // バッファがいっぱいなので待つ
			//return;
		}
	}
	memcpy(np2net_membuf[np2net_membuf_writepos], pSendBuf, len);
	np2net_membuflen[np2net_membuf_writepos] = len;
	np2net_membuf_writepos = (np2net_membuf_writepos+1)%NET_ARYLEN;
	return 0;
}

//  非同期で通信してみる
DWORD WINAPI np2net_ThreadFuncW(LPVOID vdParam) {
	while (!np2net_hThreadexit) {
		if(np2net_vc){
			if(np2net_membuf_readpos!=np2net_membuf_writepos){
				doWriteTap(np2net_hTap, (UCHAR*)(np2net_membuf[np2net_membuf_readpos]), np2net_membuflen[np2net_membuf_readpos]);
				np2net_membuf_readpos = (np2net_membuf_readpos+1)%NET_ARYLEN;
			}else{
				Sleep(0);
			}
		}
	}
	return 0;
}
DWORD WINAPI np2net_ThreadFunc(LPVOID vdParam) {
	HANDLE hEvent = NULL;
	DWORD dwLen;
	OVERLAPPED ovl;
	int nodatacount = 0;
	int sleepcount = 0;

	memset(&ovl, 0, sizeof(OVERLAPPED));
	ovl.hEvent = hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	ovl.Offset = 0;
	ovl.OffsetHigh = 0;
 
	while (!np2net_hThreadexit) {
		if(np2net_vc){
			if (!ReadFile(np2net_hTap, np2net_Buf, sizeof(np2net_Buf), &dwLen, &ovl)) {
				DWORD err = GetLastError();
				if (err == ERROR_IO_PENDING) {
					WaitForSingleObject(hEvent, INFINITE); // 受信完了待ち
					GetOverlappedResult(np2net_hTap, &ovl, &dwLen, FALSE);
					if(dwLen>0){
						//TRACEOUT(("LGY-98: recieve %u bytes\n", dwLen));
						np2net_vc->fd_read(np2net_vc->opaque, (UINT8*)np2net_Buf, dwLen);
					}
					//dump(Buf, dwLen);
				} else {
					printf("TAP-Win32: ReadFile err=0x%08X\n", err);
					//CloseHandle(hTap);
					//return -1;
					Sleep(0);
				}
			} else {
				//dump(Buf, dwLen);
				if(dwLen>0){
					//TRACEOUT(("LGY-98: recieve %u bytes\n", dwLen));
					np2net_vc->fd_read(np2net_vc->opaque, (UINT8*)np2net_Buf, dwLen);
				}else{
					Sleep(0);
				}
			}
			//if(!(sleepcount = (sleepcount+1)%100000)){
			//	Sleep(0);
			//}
		}
	}
	CloseHandle(hEvent);
	hEvent = NULL;
	return 0;
}

void np2net_closeTAP(){
    if (np2net_hTap != NULL) {
		if(np2net_hThread){
			np2net_hThreadexit = 1;
			WaitForSingleObject(np2net_hThread,  INFINITE);
			WaitForSingleObject(np2net_hThreadW, INFINITE);
			np2net_hThreadexit = 0;
			np2net_hThread = NULL;
		}
		CloseHandle(np2net_hTap);
		TRACEOUT(("LGY-98: TAP is closed"));
		np2net_hTap = NULL;
		np2net_vc = NULL;
    }
}
int np2net_openTAP(CHAR* tapname){
	DWORD dwID;
	DWORD dwLen;
	ULONG status = TRUE;
	OVERLAPPED ovl;
	UCHAR Buf[2048];
	CHAR szDevicePath[256];
	CHAR *szTAPname;
	CHAR szdefTAPname[] = "TAP1";

	if(strlen(tapname)){
		szTAPname = tapname;
	}else{
		szTAPname = szdefTAPname;
	}

	np2net_closeTAP();

	// 指定された表示名から TAP の GUID を得る
	if (!GetNetWorkDeviceGuid(szTAPname, (CHAR*)Buf, 2048)) {
		TRACEOUT(("LGY-98: [%s] GUID is not found\n", szTAPname));
		return 1;
	}
	TRACEOUT(("LGY-98: [%s] GUID = %s\n", szTAPname, Buf));
	sprintf(szDevicePath, DEVICE_PATH_FMT, Buf);
 
	// TAP デバイスを開く
	np2net_hTap = CreateFile (szDevicePath, GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_OVERLAPPED, 0);
 
	if (np2net_hTap == INVALID_HANDLE_VALUE) {
		TRACEOUT(("LGY-98: Failed to open [%s]", szDevicePath));
		return 2;
	}

	TRACEOUT(("LGY-98: TAP is opened"));
	
	// TAP デバイスをアクティブに
	status = TRUE;
	if (!DeviceIoControl(np2net_hTap,TAP_IOCTL_SET_MEDIA_STATUS,
				&status, sizeof(status), &status, sizeof(status),
				&dwLen, NULL)) {
		TRACEOUT(("LGY-98: TAP_IOCTL_SET_MEDIA_STATUS err"));
		np2net_closeTAP();
		return 3;
	}
 
	np2net_hThread  = CreateThread(NULL , 0 , np2net_ThreadFunc  , NULL , 0 , &dwID);
	np2net_hThreadW = CreateThread(NULL , 0 , np2net_ThreadFuncW , NULL , 0 , &dwID);
	
	return 0;
}

void np2net_getmacaddr(REG8 *macaddr){
	REG8 macaddrtmp[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
	//REG8 macaddrtmp[6] = {0x00, 0xff, 0xdb, 0x06, 0x80, 0xa1};
	memcpy(macaddr, macaddrtmp, 6);
}

int np2net_gsuspflag = 0;
void np2net_send_packet(VLANClientState *vc1, const UINT8 *buf, int size)
{
    VLANState *vlan = vc1->vlan;
    VLANClientState *vc;

    if (vc1->link_down)
        return;

#ifdef DEBUG_NET
    printf("vlan %d send:\n", vlan->id);
    hex_dump(stdout, buf, size);
#endif
	//if(!np2net_gsuspflag)SuspendThread(np2net_hThread);
	sendDataToBuffer((UCHAR*)buf, size);
	//doWriteTap(np2net_hTap, (UCHAR*)buf, size);
	//if(!np2net_gsuspflag)ResumeThread(np2net_hThread);
	//np2net_Buf = buf;
	//np2net_BufLen = size;
	//np2net_BufFillFlag = 0;
    /*for(vc = vlan->first_client; vc != NULL; vc = vc->next) {
        if (vc != vc1 && !vc->link_down) {
            vc->fd_read(vc->opaque, buf, size);
        }
    }*/
}

void np2net_setVC(VLANClientState *vc1)
{
	np2net_vc = vc1;
}
void np2net_init()
{
	InitializeCriticalSection(&np2net_cs);

	memset(&np2net_write_ovl, 0, sizeof(OVERLAPPED));
	np2net_write_ovl.hEvent = np2net_write_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	np2net_write_ovl.Offset = 0;
	np2net_write_ovl.OffsetHigh = 0;
}
int np2net_reset(CHAR* tapname)
{
	return np2net_openTAP(tapname);
}
void np2net_shutdown()
{
	np2net_hThreadexit = 1;
	np2net_closeTAP();
	DeleteCriticalSection(&np2net_cs);
}
void np2net_suspend()
{
	//SuspendThread(np2net_hThread);
	np2net_gsuspflag = 1;
}
void np2net_resume()
{
	//ResumeThread(np2net_hThread);
	np2net_gsuspflag = 0;
}



// 参考文献: http://dsas.blog.klab.org/archives/51012690.html

// ネットワークデバイス表示名からデバイス GUID 文字列を検索
CHAR *GetNetWorkDeviceGuid(CONST CHAR *pDisplayName, CHAR *pszBuf, DWORD cbBuf)
{
  CONST CHAR *SUBKEY = "SYSTEM\\CurrentControlSet\\Control\\Network";
 
#define BUFSZ 256
  // HKLM\SYSTEM\\CurrentControlSet\\Control\\Network\{id1]\{id2}\Connection\Name が
  // ネットワークデバイス名（ユニーク）の格納されたエントリであり、
  // {id2} がこのデバイスの GUID である
 
  HKEY hKey1, hKey2, hKey3;
  LONG nResult;
  DWORD dwIdx1, dwIdx2;
  CHAR szData[64], *pKeyName1, *pKeyName2, *pKeyName3, *pKeyName4; 
  DWORD dwSize, dwType = REG_SZ;
  BOOL bDone = FALSE;
  FILETIME ft;

  hKey1 = hKey2 = hKey3 = NULL;
  pKeyName1 = pKeyName2 = pKeyName3 = pKeyName4 = NULL;
 
  // 主キーのオープン
  nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SUBKEY, 0, KEY_READ, &hKey1);
  if (nResult != ERROR_SUCCESS) {
    printf("GetNetWorkDeviceGuid: open key err HKLM%s\n", SUBKEY);
    return NULL;
  }
  pKeyName1 = (CHAR*)malloc(BUFSZ);
  pKeyName2 = (CHAR*)malloc(BUFSZ);
  pKeyName3 = (CHAR*)malloc(BUFSZ);
  pKeyName4 = (CHAR*)malloc(BUFSZ);
 
  dwIdx1 = 0;
  while (bDone != TRUE) { // {id1} を列挙するループ
 
    dwSize = BUFSZ;
    nResult = RegEnumKeyEx(hKey1, dwIdx1++, pKeyName1,
                          &dwSize, NULL, NULL, NULL, &ft);
    if (nResult == ERROR_NO_MORE_ITEMS) {
      break;
    }
 
    // SUBKEY\{id1} キーをオープン
    sprintf(pKeyName2, "%s\\%s", SUBKEY, pKeyName1);
    nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pKeyName2,
                          0, KEY_READ, &hKey2);
    if (nResult != ERROR_SUCCESS) {
      continue;
    }
    dwIdx2 = 0;
    while (1) { // {id2} を列挙するループ
      dwSize = BUFSZ;
      nResult = RegEnumKeyEx(hKey2, dwIdx2++, pKeyName3,
                          &dwSize, NULL, NULL, NULL, &ft);
      if (nResult == ERROR_NO_MORE_ITEMS) {
        break;
      }
 
      if (nResult != ERROR_SUCCESS) {
        continue;
      }
 
      // SUBKEY\{id1}\{id2]\Connection キーをオープン
      sprintf(pKeyName4, "%s\\%s\\%s",
                      pKeyName2, pKeyName3, "Connection");
      nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                      pKeyName4, 0, KEY_READ, &hKey3);
      if (nResult != ERROR_SUCCESS) {
        continue;
      }
 
      // SUBKEY\{id1}\{id2]\Connection\Name 値を取得
      dwSize = sizeof(szData);
      nResult = RegQueryValueEx(hKey3, "Name",
                      0, &dwType, (LPBYTE)szData, &dwSize);
 
      if (nResult == ERROR_SUCCESS) {
        if (stricmp(szData, pDisplayName) == 0) {
          strcpy(pszBuf, pKeyName3);
          bDone = TRUE;
          break;
        }
      }
      RegCloseKey(hKey3);
      hKey3 = NULL;
    }
    RegCloseKey(hKey2);
    hKey2 = NULL;
  }
 
  if (hKey1) { RegCloseKey(hKey1); }
  if (hKey2) { RegCloseKey(hKey2); }
  if (hKey3) { RegCloseKey(hKey3); }
 
  if (pKeyName1) { free(pKeyName1); }
  if (pKeyName2) { free(pKeyName2); }
  if (pKeyName3) { free(pKeyName3); }
  if (pKeyName4) { free(pKeyName4); }
 
  // GUID を発見できず
  if (bDone != TRUE) {
    return NULL;
  }
  return pszBuf;
}

#endif	/* SUPPORT_LGY98 */