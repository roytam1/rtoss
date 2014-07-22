/*
	ips.c
	IPS Patch Maker
*/
#include <windows.h>

#define EXPORT __export
#define EOF -1

static unsigned char *buffer;
static unsigned long address = 0;
static unsigned long count = 0;
static BOOL userle = TRUE;

// Simple FILE struct
typedef struct FILEINFO
{
	HANDLE hFile;
	DWORD nBufCount;
	DWORD nBufSize;
	BOOL bEof;
	BYTE lpBuffer[1024];
} FILEINFO, *LPFILEINFO;

static HANDLE ips;
static LPFILEINFO uf, cf;

static LPFILEINFO WINAPI LoadFile(LPCSTR lpszFilename);
static void WINAPI ReloadBuffer(LPFILEINFO f);
static int WINAPI ReadByte(LPFILEINFO f);
static void WINAPI flushBuf(void);
static void WINAPI addByte(unsigned char c);
static void WINAPI WriteBlock(DWORD Address,DWORD Length,LPBYTE Data);
static void WINAPI WriteBlockRLE(DWORD Address,DWORD RunLength,BYTE RunByte);
BOOL WINAPI EXPORT MakePatch(LPCSTR UnchangedFile,LPCSTR ChangedFile,LPCSTR OutputFile,LPSTR ErrorMsg,BOOL bUseRle);

// opens a file for reading
static LPFILEINFO WINAPI LoadFile(LPCSTR lpszFilename)
{
	LPFILEINFO f;

	// allocate FILEINFO
	f = (LPFILEINFO)GlobalAlloc(GPTR,sizeof(FILEINFO));
	if(!f) return NULL;

	// open file
	f->hFile = CreateFile(lpszFilename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(f->hFile == INVALID_HANDLE_VALUE) return NULL;

	ReloadBuffer(f);

	return f;
}

// reloads the read buffer of a file
static void WINAPI ReloadBuffer(LPFILEINFO f)
{
	// refill the buffer
	ReadFile(f->hFile,f->lpBuffer,1024,&f->nBufSize,NULL);
	f->bEof = (f->nBufSize == 0);
	f->nBufCount = 0;
}

// closes a file
static void WINAPI CloseFile(LPFILEINFO f)
{
	CloseHandle(f->hFile);
	GlobalFree(f);
}

// reads a byte from the file
static int WINAPI ReadByte(LPFILEINFO f)
{
	int byte;

	if(f->bEof) return EOF;

	// grab byte from buffer
	byte = f->lpBuffer[f->nBufCount++];
	if(f->nBufCount >= f->nBufSize) ReloadBuffer(f);

	return byte;
}

// write an [address, length, data] block to IPS
static void WINAPI flushBuf(void)
{
	unsigned long start, end;
	int i, j, runcount;
	unsigned char runbyte;

	if(count) {
		// if block is < size 3, write normal
		if(count <= 3 || !userle) WriteBlock(address,count,buffer);
		else if(count <= 16) { // if less than 16, check for RLE compression
			runbyte = buffer[0];
			for(i = 0; i < count; i++) if(buffer[i] != runbyte) break;
			// compress only if the entire block is the same
			if(i == 16) WriteBlockRLE(address,count,runbyte);
			else WriteBlock(address,count,buffer);
		} else { // otherwise compress all blocks > 16 dup
			start = end = address;
			for(i = 0; i < count; i++, end++) {
				// check for RLE compression
				if((i + 16) < count
				&& buffer[i] == buffer[i + 1]
				&& buffer[i] == buffer[i + 16]) {
					runbyte = buffer[i];
					runcount = 2;
					for(j = i + 2; j < count; j++, runcount++)
						if(buffer[j] != runbyte) break;
					if(runcount >= 16) {
						WriteBlock(start,end - start,buffer + start - address);
						WriteBlockRLE(i + address,runcount,runbyte);
						start = end + runcount;
						end = start - 1;
						i += runcount - 1;
					}
				}
			}
			WriteBlock(start,end - start,buffer + start - address);
		}

		count = 0;
	}
}

// writes an [address, length, data] block to IPS
static void WINAPI WriteBlock(DWORD Address,DWORD Length,LPBYTE Data)
{
	DWORD addr, nSize;
	WORD len;

	if(Length) {
		// reverse address from LSB->MSB
		addr = ((Address >> 16) & 0xFF) | (Address & 0xFF00) | ((Address & 0xFF) << 16);
		WriteFile(ips,&addr,3,&nSize,NULL);
		// LSB -> MSB
		len = (WORD)(((Length >> 8) & 0xFF) | ((Length & 0xFF) << 8));
		WriteFile(ips,&len,2,&nSize,NULL);
		// write data
		WriteFile(ips,Data,Length,&nSize,NULL);
	}
}

// writes an [address, runlength, runbyte] block to IPS
static void WINAPI WriteBlockRLE(DWORD Address,DWORD RunLength,BYTE RunByte)
{
	DWORD addr, nSize;
	WORD len;

	if(RunLength) {
		// reverse address from LSB->MSB
		addr = ((Address >> 16) & 0xFF) | (Address & 0xFF00) | ((Address & 0xFF) << 16);
		WriteFile(ips,&addr,3,&nSize,NULL);
		// write 0 count
		len = 0;
		WriteFile(ips,&len,2,&nSize,NULL);
		// LSB -> MSB
		len = (WORD)(((RunLength >> 8) & 0xFF) | ((RunLength & 0xFF) << 8));
		WriteFile(ips,&len,2,&nSize,NULL);
		// write data
		WriteFile(ips,&RunByte,1,&nSize,NULL);
	}
}

// places a byte into the block buffer
static void WINAPI addByte(unsigned char c)
{
	if(count >= 65535L) flushBuf(), address += 65535L;
	buffer[count++] = c;
}

// create a patch
BOOL WINAPI EXPORT MakePatch(LPCSTR UnchangedFile,LPCSTR ChangedFile,LPCSTR OutputFile,LPSTR ErrorMsg,BOOL bUseRle)
{
	int byte1, byte2;
	unsigned long offset;
	DWORD nSize;

	// allocate memory for buffer
	buffer = (unsigned char *)GlobalAlloc(GPTR,65536);
	if(!buffer) {
		if(ErrorMsg) wsprintf(ErrorMsg,"Out of memory.");
		return FALSE;
	}

	// load unchanged file
	uf = LoadFile(UnchangedFile);
	if(!uf) {
		if(ErrorMsg) wsprintf(ErrorMsg,"%s: Can't find file.",UnchangedFile);
		goto err0;
	}

	// load changed file
	cf = LoadFile(ChangedFile);
	if(!cf) {
		if(ErrorMsg) wsprintf(ErrorMsg,"%s: Can't find file.",ChangedFile);
		goto err1;
	}

	if(GetFileSize(cf,NULL) < GetFileSize(uf,NULL)) {
		if(ErrorMsg) wsprintf(ErrorMsg,"File size of changed file is less than the unchanged file.");
		goto err2;
	}

	// create IPS for output
	ips = CreateFile(OutputFile,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
	if(ips == INVALID_HANDLE_VALUE) {
		if(ErrorMsg) wsprintf(ErrorMsg,"%s: Can't write to file.",OutputFile);
		goto err2;
	}

	// write signature
	WriteFile(ips,"PATCH",5,&nSize,NULL);

	userle = bUseRle;

	offset = 0;
	for(;;) {
		// read byte from both files
		byte1 = ReadByte(uf);
		byte2 = ReadByte(cf);
		if(byte2 == EOF) break;

		offset++; // increment offset
		// compare the 2 bytes
		if(byte1 == EOF || byte1 != byte2) {
			// if the 2 bytes aren't equal, then add byte2 to block buffer
			addByte((unsigned char)byte2);
		} else {
			// if the bytes are the same
			flushBuf(); // flush the buffer
			address = offset; // adjust address
		}
	}
	flushBuf();

	// write EOF
	WriteFile(ips,"EOF",3,&nSize,NULL);

	CloseHandle(ips);
	CloseFile(uf);
	CloseFile(cf);
	GlobalFree(buffer);

	return TRUE;

err2:
	CloseFile(cf);
err1:
	CloseFile(uf);
err0:
	GlobalFree(buffer);

	return FALSE;
}

int main(int argc, char** argv) {
	char *errMsg;
	int isRle = 0;
	if(argc < 4) {
		fprintf(stderr,"%s <original-file> <changed-file> <output> <0|1 for RLE encoding>\n",argv[0]);
		return 1;
	}
	if(argc >= 4) isRle = atoi(argv[4]);
	errMsg = (char*)calloc(1024,1);
	MakePatch(argv[1],argv[2],argv[3],errMsg,isRle);
	if(errMsg[0]) fprintf(stderr,errMsg);
	free(errMsg);
	return 0;
}
