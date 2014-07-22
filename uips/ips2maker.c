/*
	ips.c
	IPS Patch Maker
*/
#include <stdio.h>

#ifndef EOF
#define EOF -1
#endif

#ifndef BOOL
#define BOOL unsigned int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef HANDLE
#define HANDLE FILE*
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef LPCSTR
#define LPCSTR unsigned char*
#endif

#ifndef LPSTR
#define LPSTR unsigned char*
#endif

#ifndef LPBYTE
#define LPBYTE unsigned char*
#endif

#ifndef LPDWORD
#define LPDWORD unsigned long*
#endif

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

static LPFILEINFO LoadFile(LPCSTR lpszFilename);
static void ReloadBuffer(LPFILEINFO f);
static int ReadByte(LPFILEINFO f);
static void flushBuf(void);
static void addByte(unsigned char c);
static void WriteBlock(DWORD Address,DWORD Length,LPBYTE Data);
static void WriteBlockRLE(DWORD Address,DWORD RunLength,BYTE RunByte);
BOOL MakePatch(LPCSTR UnchangedFile,LPCSTR ChangedFile,LPCSTR OutputFile,LPSTR ErrorMsg,BOOL bUseRle);
static DWORD myGetFileSize(LPFILEINFO f, LPDWORD lpFileSizeHigh);

#define BSWAP16(v) ((v>>8) | (v<<8))
#define BSWAP32(v) ( ((v>>24)&0xff) |  ((v<<8)&0xff0000) |  ((v>>8)&0xff00) | ((v<<24)&0xff000000) )

// opens a file for reading
static LPFILEINFO LoadFile(LPCSTR lpszFilename)
{
	LPFILEINFO f;

	// allocate FILEINFO
	f = (LPFILEINFO)malloc(sizeof(FILEINFO));
	if(!f) return NULL;

	// open file
	f->hFile = fopen(lpszFilename,"rb");
	if(f->hFile == NULL) return NULL;

	ReloadBuffer(f);

	return f;
}

// reloads the read buffer of a file
static void ReloadBuffer(LPFILEINFO f)
{
	// refill the buffer
	f->nBufSize = fread(f->lpBuffer,1,1024,f->hFile);
	f->bEof = (f->nBufSize == 0);
	f->nBufCount = 0;
}

// closes a file
static void CloseFile(LPFILEINFO f)
{
	fclose(f->hFile);
	free(f);
}

// reads a byte from the file
static int ReadByte(LPFILEINFO f)
{
	int byte;

	if(f->bEof) return EOF;

	// grab byte from buffer
	byte = f->lpBuffer[f->nBufCount++];
	if(f->nBufCount >= f->nBufSize) ReloadBuffer(f);

	return byte;
}

// write an [address, length, data] block to IPS
static void flushBuf(void)
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
static void WriteBlock(DWORD Address,DWORD Length,LPBYTE Data)
{
	DWORD addr, nSize;
	WORD len;

	if(Length) {
		// reverse address from LSB->MSB
		addr = BSWAP32(Address);
		nSize = fwrite(&addr,4,1,ips);
		// LSB -> MSB
		len = (WORD)BSWAP16(Length);
		nSize = fwrite(&len,2,1,ips);
		// write data
		nSize = fwrite(Data,Length,1,ips);
	}
}

// writes an [address, runlength, runbyte] block to IPS
static void WriteBlockRLE(DWORD Address,DWORD RunLength,BYTE RunByte)
{
	DWORD addr, nSize;
	WORD len;

	if(RunLength) {
		// reverse address from LSB->MSB
		addr = BSWAP32(Address);
		nSize = fwrite(&addr,4,1,ips);
		// write 0 count
		len = 0;
		nSize = fwrite(&len,2,1,ips);
		// LSB -> MSB
		len = (WORD)BSWAP16(RunLength);
		nSize = fwrite(&len,2,1,ips);
		// write data
		nSize = fwrite(&RunByte,1,1,ips);
	}
}

// places a byte into the block buffer
static void addByte(unsigned char c)
{
	if(count >= 65535L) flushBuf(), address += 65535L;
	buffer[count++] = c;
}

// get file size
static DWORD myGetFileSize(LPFILEINFO f, LPDWORD lpFileSizeHigh)
{
	size_t opos, npos;
	opos = ftell(f->hFile);
	fseek(f->hFile, 0, SEEK_END);
	npos = ftell(f->hFile);
	fseek(f->hFile, opos, SEEK_SET);
	return npos;
}

// create a patch
BOOL MakePatch(LPCSTR UnchangedFile,LPCSTR ChangedFile,LPCSTR OutputFile,LPSTR ErrorMsg,BOOL bUseRle)
{
	int byte1, byte2;
	unsigned long offset;
	DWORD nSize;

	// allocate memory for buffer
	buffer = (unsigned char *)malloc(65536);
	if(!buffer) {
		if(ErrorMsg) sprintf(ErrorMsg,"Out of memory.");
		return FALSE;
	}

	// load unchanged file
	uf = LoadFile(UnchangedFile);
	if(!uf) {
		if(ErrorMsg) sprintf(ErrorMsg,"%s: Can't find file.",UnchangedFile);
		goto err0;
	}

	// load changed file
	cf = LoadFile(ChangedFile);
	if(!cf) {
		if(ErrorMsg) sprintf(ErrorMsg,"%s: Can't find file.",ChangedFile);
		goto err1;
	}

	if(myGetFileSize(cf,NULL) < myGetFileSize(uf,NULL)) {
		if(ErrorMsg) sprintf(ErrorMsg,"File size of changed file is less than the unchanged file.");
		goto err2;
	}

	// create IPS for output
	ips = fopen(OutputFile,"wb");
	if(ips == NULL) {
		if(ErrorMsg) sprintf(ErrorMsg,"%s: Can't write to file.",OutputFile);
		goto err2;
	}

	// write signature
	nSize = fwrite("_PATCH",6,1,ips);

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
	nSize = fwrite("_EOF",4,1,ips);

	fclose(ips);
	CloseFile(uf);
	CloseFile(cf);
	free(buffer);

	return TRUE;

err2:
	CloseFile(cf);
err1:
	CloseFile(uf);
err0:
	free(buffer);

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
