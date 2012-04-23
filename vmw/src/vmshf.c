/*
	vmshf.c
	file transfer via VMware Shared Folders

	Copyright (c) 2006 Ken Kato
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "vmshf.h"

/*
	data transfer buffer size required for shared folder RPC call
*/
#define VMSHF_BUFFER_SIZE	(VMSHF_BLOCK_SIZE + 27)

/*
	the RPC header part of shared folder command data
*/
static const uint8_t shf_header[] =
	{ 'f', ' ', 0, 0, 0, 0 };

/*
	shared folder function debug flag
*/
int vmshf_debug = 0;

/*
	replace path delimiters
*/
static void ReplaceDelim(
	char		*str,
	uint32_t	length,
	char		delim)
{
	while (length--) {
		if (*(str + length) == '\0' ||
			*(str + length) == '/' ||
			*(str + length) == '\\') {
			*(str + length) = delim;
		}
	}
}

/*
	execute a backdoor RPC command
*/
static int ExecuteRpc(
	const shf_t	*shf,
	uint32_t	*length)
{
	int ret;
	uint16_t id;

	ret = VMRpcSend(&(shf->rpc), shf->buf, *length);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	ret = VMRpcRecvLen(&(shf->rpc), length, &id);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	if (*length > VMSHF_BUFFER_SIZE) {
		printf("RPC reply too long (%ld)\n", *length);
		return VMTOOL_RPC_ERROR;
	}

	ret = VMRpcRecvDat(&(shf->rpc), shf->buf, *length, id);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	if (*length < 10 || strncmp((char *)shf->buf, "1 ", 2) != 0) {
		printf("unexpected RPC reply '%s'\n", shf->buf);
		return VMTOOL_RPC_ERROR;
	}

	return VMTOOL_SUCCESS;
}

/*
	difference between the epoch of time_t (1970/1/1 0:0:0 UTC)
	and 64 bit filetime (1601/1/1 0:0:0 UTC) in 100 nanosec unit
*/
#define FILETIME_DELTA_LO	0xd53e8000L
#define FILETIME_DELTA_HI	0x019db1deL

/*
	convert a 64 bit filetime value into a 32 bit time_t value and
	a 32 bit nanosecond value
*/
uint32_t FtimeToTime(
	uint64_t filetime,
	uint32_t *nanosec)
{
	uint64_t delta;
	uint32_t divisor;
	uint32_t remainder;

	if (U64HI(filetime) == 0 && U64LO(filetime) == 0) {
		if (nanosec) {
			*nanosec = 0;
		}
		return 0;
	}

	/* delta = (FILETIME_DELTA_HI << 32) |	FILETIME_DELTA_LO; */
	U64INIT(delta, FILETIME_DELTA_LO, FILETIME_DELTA_HI);

	/* filetime = filetime - delta; */
	U64SUB64(filetime, filetime, delta);

	/* remainder = filetime % divisor; */
	/* filetime  = filetime / divisor; */
	divisor = 10000000L;
	U64DIV32(filetime, remainder, filetime, divisor);

	if (nanosec) {
		*nanosec = remainder * 100;
	}

	return U64LO(filetime);
}

/*
	convert a 32 bit time_t value into a 64 bit filetime value
*/
uint64_t TimeToFtime(
	uint32_t seconds,
	uint32_t nanosec)
{
	uint64_t result;
	uint64_t delta;
	uint32_t multiplier = 10000000L;

	nanosec /= 100;

	/* delta = (HILETIME_DELTA_HI << 32) | FILETIME_DELTA_LO; */
	U64INIT(delta, FILETIME_DELTA_LO, FILETIME_DELTA_HI);

	/* result = seconds * multiplier; */
	U64MUL32(result, seconds, multiplier);

	/* result = result + nanosec; */
	U64ADD32(result, result, nanosec);

	/* result = result + delta; */
	U64ADD64(result, result, delta);

	return result;
}

/*
	convert between utf8 and ansi string
*/

/*
	UTF-8 -> ASCII when no conversion routine is present
	-- replace multi byte characters with '_'
*/
static uint32_t Utf8ToAscii(char *dst, const char *src)
{
	uint32_t len = 0;

	/*
		UTF-8 byte codes
		0xxxxxxx (0x00..0x7f) ASCII code
		10xxxxxx (0x80..0xbf) trailing bytes of multi byte code
		110xxxxx (0xc0..0xdf) leading byte of 2 byte code
		1110xxxx (0xe0..0xef) leading byte of 3 byte code
		11110xxx (0xf0..0xf7) leading byte of 4 byte code
		111110xx (0xf8..0xfb) leading byte of 5 byte code
		1111110x (0xfc..0xfd) leading byte of 6 byte code
		0xfe and 0xff never appears in UTF-8
	*/
	while (*src) {
		if (*src > 0) {
			/*
				ASCII character -- copy as it is
			*/
			*dst = *src;

			dst++;
			len++;
		}
		else if ((unsigned char)*src >= 0xc0) {
			/*
				UTF-8 leading byte or a stray 0xfe/0xff
				ignore trailing bytes in order to replace
				a single UTF-8 character with a single '_'
			*/
			*dst = '_';

			dst++;
			len++;
		}

		src++;
	}

	*dst = '\0';

	return len;
}

/*
	ASCII -> UTF-8 when no conversion routine is present
	-- replace non-ASCII characters with '_'
*/
static uint32_t AsciiToUtf8(char *dst, const char *src)
{
	uint32_t len = 0;

	while (*src) {
		if (*src > 0) {
			*dst = *src;
		}
		else {
			*dst = '_';
		}

		len++;
		src++;
		dst++;
	}

	*dst = '\0';

	return len;
}

#ifdef _WIN32
/*
	win32 systems
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static uint32_t Utf8ToLocal(char *dst, const char *src)
{
	WCHAR wc[VMSHF_MAX_PATH];
	int len;

	if (*src == '\0') {
		*dst = '\0';
		return 0;
	}

	/* first convert into Unicode */
	len = MultiByteToWideChar(CP_UTF8, 0, src, -1, wc, VMSHF_MAX_PATH);

	if (len <= 0) {
		return Utf8ToAscii(dst, src);
	}

	/* then convert back into ANSI code set */
	len = WideCharToMultiByte(
		CP_ACP, 0, wc, len, dst, VMSHF_MAX_PATH, NULL, NULL);

	if (len <= 0) {
		return Utf8ToAscii(dst, src);
	}

	return len - 1;	/* len includes the terminating null byte */
}

static uint32_t LocalToUtf8(char *dst, const char *src)
{
	WCHAR wc[VMSHF_MAX_PATH];
	int len;

	if (*src == '\0') {
		*dst = '\0';
		return 0;
	}

	/* first convert into Unicode */
	len = MultiByteToWideChar(CP_ACP, 0, src, -1, wc, VMSHF_MAX_PATH);

	if (len <= 0) {
		return AsciiToUtf8(dst, src);
	}

	/* then convert back into UTF-8 */
	len = WideCharToMultiByte(
		CP_UTF8, 0, wc, len, dst, VMSHF_MAX_PATH, NULL, NULL);

	if (len <= 0) {
		return AsciiToUtf8(dst, src);
	}

	return len - 1;	/* len includes the terminating null byte */
}

#elif HAVE_ICONV_H
/*
	unix systems with iconv.h
	-- use iconv() for code conversion
*/
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/iconv.h>
typedef size_t iconv_t;
#else	/* __FreeBSD__ */
#include <iconv.h>
#endif	/* __FreeBSD__ */

#include <langinfo.h>
#include <locale.h>
#include <strings.h>

static uint32_t ConvertStub(char *dst, const char *src, int to_utf8)
{
	static char *local_set = NULL;
	iconv_t cd;
	size_t srclen, dstlen;

	if (!local_set) {
		/* first time -- get local codeset */
		setlocale(LC_CTYPE, "");
		local_set = nl_langinfo(CODESET);
	}

	if (!local_set) {
		/* unknown local codeset -- use fallback */
		if (to_utf8) {
			return AsciiToUtf8(dst, src);
		}
		else {
			return Utf8ToAscii(dst, src);
		}
	}

	if (*src == '\0') {
		*dst = '\0';
		return 0;
	}

	if (strcasecmp(local_set, "UTF-8") == 0 ||
		strcasecmp(local_set, "UTF8") == 0) {
		/* local set is UTF-8 -- no need to convert */
		srclen = strlen(src);

		if (dst != src) {
			memmove(dst, src, srclen + 1);
		}

		return srclen;
	}

	/*	open iconv descriptor */

	if (to_utf8) {
		cd = iconv_open("UTF-8", local_set);

		if (cd == (iconv_t)-1) {
			return AsciiToUtf8(dst, src);
		}
	}
	else {
		cd = iconv_open(local_set, "UTF-8");

		if (cd == (iconv_t)-1) {
			return Utf8ToAscii(dst, src);
		}
	}

	srclen = strlen(src);
	dstlen = VMSHF_MAX_PATH;

	while (srclen > 0) {
		/*
			Some version of iconv.h declares the second parameter of
			iconv() as "char **" instead of "const char **" as it should be.
			The following line may cause a compiler warning in such cases
			but we can safely ignore it.
		*/
#ifdef __linux__
/*
	On Linux iconv() has the following prototype:
		size_t iconv(iconv_t cd, char **, size_t *, char **, size_t *);
	while on most other systems:
		size_t iconv(iconv_t cd, const char **, size_t *, char **, size_t *);
	The following cast avoids a compiler warning on Linux
*/
#define ICONV_CAST	(char **)
#else
#define ICONV_CAST
#endif /* __Linux__ */

		if (iconv(cd, ICONV_CAST &src, &srclen, &dst, &dstlen) == -1) {
			/* failed to convert a character -- replace with '_' */
			*(dst++) = '_';
			dstlen--;

			/* skip the current character */
			src++;
			srclen--;

			if (!to_utf8) {
				/* skip UTF-8 trailing bytes (10xxxxxx) as well */
				while (srclen > 0 && (*src & 0xc0) == 0x80) {
					src++;
					srclen--;
				}
			}
		}
	}

	iconv_close(cd);

	*dst = '\0';

	return VMSHF_MAX_PATH - dstlen;
}

#define Utf8ToLocal(dst, src)	ConvertStub(dst, src, 0)
#define LocalToUtf8(dst, src)	ConvertStub(dst, src, 1)

#else
/*
	other systems with no means of code conversion (e.g. OpenBSD, DOS, minix)
*/
#define Utf8ToLocal(dst, src)	Utf8ToAscii(dst, src)
#define LocalToUtf8(dst, src)	AsciiToUtf8(dst, src)

#endif

/*
	open an RPC channel for shared folder functions
*/
int VMShfBeginSession(
	shf_t *shf)
{
	int ret;
	uint32_t length;
	uint16_t id;

	memset(shf, 0, sizeof(shf_t));

	/* allocate a data transfer buffer */
	shf->buf = (uint8_t *)malloc(VMSHF_BUFFER_SIZE);

	if (shf->buf == NULL) {
		printf("buffer allocation: %s\n", strerror(errno));
		return VMTOOL_SYS_ERROR;
	}

	/* open an &(shf->rpc) channel */
	ret = VMRpcOpen(&(shf->rpc), VMRPC_ENHANCED);

	if (ret != VMTOOL_SUCCESS) {
		free(shf->buf);
		shf->buf = NULL;
		return ret;
	}

	/* send &(shf->rpc) command "f " */
	ret = VMRpcSend(&(shf->rpc), shf_header, 2);

	if (ret != VMTOOL_SUCCESS) {
		goto error_exit;
	}

	/* get reply length */
	ret = VMRpcRecvLen(&(shf->rpc), &length, &id);

	if (ret != VMTOOL_SUCCESS) {
		goto error_exit;
	}

	/* get reply data */
	ret = VMRpcRecvDat(&(shf->rpc), shf->buf, length, id);

	if (ret != VMTOOL_SUCCESS) {
		goto error_exit;
	}

	/* check reply status */
	if (strncmp((char *)shf->buf, "1 ", 2) != 0) {
		printf("Unexpected RPC reply: %s\n", shf->buf);
		ret = VMTOOL_RPC_ERROR;
		goto error_exit;
	}

	/* success */
	return VMTOOL_SUCCESS;

error_exit:
	/* failure */
	free(shf->buf);
	VMRpcClose(&(shf->rpc));
	memset(shf, 0, sizeof(shf_t));

	return ret;
}

/*
	release shared folder context
*/
void VMShfEndSession(
	shf_t *shf)
{
	if (shf->buf) {
		free(shf->buf);
	}
	VMRpcClose(&(shf->rpc));
	memset(shf, 0, sizeof(shf_t));
}

/*
	open a host file
*/
int VMShfOpenFile(
	const shf_t	*shf,
	uint32_t	access,			/* 10: one of VMSHF_ACCESS_* values		*/
	uint32_t	openmode,		/* 14: one of VMSHF_OPENMODE_* values	*/
	uint8_t		filemode,		/* 18: VMSHF_FILEMODE_* mask values		*/
	const char	*filename,		/* 23: variable length file name		*/
	uint32_t	*status,		/* 10: status							*/
	uint32_t	*handle)		/* 14: file handle						*/
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		const static char *access_name[] = { "ro", "??", "rw" };
		const static char *openmode_name[] = {
			"o_exist", "?", "o_always", "c_new", "c_always"
		};

		fprintf(stderr,
			"VMSHF_OPEN_FILE  name:'%s'\n"
			"access:%lu (%s), openmode:%lu (%s), "
			"filemode:%c%c%c (0x%02x) -> ",
			filename,
			access,
			access_name[access < 3 ? access : 1],
			openmode,
			openmode_name[openmode < 5 ? openmode : 1],
			(filemode & VMSHF_FILEMODE_READ) ? 'r' : '-',
			(filemode & VMSHF_FILEMODE_WRITE) ? 'w' : '-',
			(filemode & VMSHF_FILEMODE_EXEC) ? 'x' : '-',
			filemode);
		fflush(stderr);
	}

	*status = (uint32_t)-1;
	*handle = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[ 6] = VMSHF_OPEN_FILE;
	*(uint32_t *)&shf->buf[10] = access;
	*(uint32_t *)&shf->buf[14] = openmode;
	shf->buf[18] = filemode;

	namelen = LocalToUtf8((char *)&shf->buf[23], filename);
	*(uint32_t *)&shf->buf[19] = namelen;
	ReplaceDelim((char *)&shf->buf[23], namelen, '\0');

	datalen = 24 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (datalen >= 14) {
		*handle = *(uint32_t *)&shf->buf[10];
	}

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu, handle:%ld\n", *status, *handle);
	}

	return VMTOOL_SUCCESS;
}

/*
	read data from a host file
*/
int VMShfReadFile(
	const shf_t *shf,
	uint32_t	handle,		/* 10=> file handle returned by OPEN	*/
	uint64_t	offset,		/* 14=> byte offset to stat reading		*/
	uint32_t	*length,	/* 22-> number of bytes to read			*/
	uint32_t	*status)
{
	uint32_t datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr, "VMSHF_READ_FILE handle:%lu, "
			"offset:%" I64D_FMT "u, length:%lu -> ",
			handle, offset, *length);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[ 6] = VMSHF_READ_FILE;	/* shared folder command */
	*(uint32_t *)&shf->buf[10] = handle;			/* file handle */
	*(uint64_t *)&shf->buf[14] = offset;			/* start offset */
	*(uint32_t *)&shf->buf[22] = *length;			/* read length */

	datalen = 26;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (datalen >= 14) {
		*length = *(uint32_t *)&shf->buf[10];
	}
	else {
		*length = (uint32_t)-1;
	}

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu, length:%ld\n",
			*status, *length);
	}

	return VMTOOL_SUCCESS;
}

/*
	write data to a host file
*/
int VMShfWriteFile(
	const shf_t *shf,
	uint32_t	handle,			/* 10: file handle returned by OPEN */
	uint8_t		unknown,		/* 14: purpose unknown				*/
	uint64_t	offset,			/* 15: byte offset to stat writing	*/
	uint32_t	*length,		/* 23: number of bytes to write		*/
	uint32_t	*status)
{
	uint32_t datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr, "VMSHF_WRITE_FILE handle:%lu, "
			"unknown:0x%02x, offset:%" I64D_FMT "u, length:%lu -> ",
			handle, unknown, offset, *length);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[ 6] = VMSHF_WRITE_FILE;	/* Shared folder command */
	*(uint32_t *)&shf->buf[10] = handle;			/* file handle */
	shf->buf[14] = unknown;							/* unknown */
	*(uint64_t *)&shf->buf[15] = offset;			/* start offset */
	*(uint32_t *)&shf->buf[23] = *length;			/* write length */

	datalen = 27 + *length;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (datalen >= 14) {
		*length = *(uint32_t *)&shf->buf[10];
	}
	else {
		*length = (uint32_t)-1;
	}

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu, length:%ld\n", *status, *length);
	}

	return VMTOOL_SUCCESS;
}

/*
	close a host file
*/
int VMShfCloseFile(
	const shf_t *shf,
	uint32_t	handle,		/* 10: file handle returned by OPEN */
	uint32_t	*status)
{
	uint32_t datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr, "VMSHF_CLOSE_FILE handle:%lu ", handle);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[ 6] = VMSHF_CLOSE_FILE;
	*(uint32_t *)&shf->buf[10] = handle;

	datalen = 14;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu\n", *status);
	}

	return VMTOOL_SUCCESS;
}

/*
	open directory
*/
int VMShfOpenDir(
	const shf_t *shf,
	const char	*dirname,
	uint32_t	*status,
	uint32_t	*handle)
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_OPEN_DIR   name:'%s' -> ", dirname);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[6] = VMSHF_OPEN_DIR;

	namelen = LocalToUtf8((char *)&shf->buf[14], dirname);
	ReplaceDelim((char *)&shf->buf[14], namelen, '\0');
	*(uint32_t *)&shf->buf[10] = namelen;

	datalen = 15 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		if (vmshf_debug) {
			fprintf(stderr,	"rpc failed\n");
		}
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (datalen >= 14) {
		*handle = *(uint32_t *)&shf->buf[10];
	}
	else {
		*handle = (uint32_t)-1;
	}

	if (vmshf_debug) {
		fprintf(stderr,
			"status:%lu, handle:%lu\n", *status, *handle);
	}

	return VMTOOL_SUCCESS;
}

/*
	get a directory entry
*/
int VMShfReadDir(
	const shf_t *shf,
	uint32_t	handle,		/* 10: file handle returned by OPEN */
	uint32_t	index,
	uint32_t	*status,
	uint32_t	*dirflag,
	fileattr_t	**fileattr,
	char		**filename)
{
	uint32_t datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_READ_DIR   handle:%lu, index:%lu -> ",
			handle, index);
		fflush(stderr);
	}

	*status = (uint32_t)-1;
	*filename = NULL;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[ 6] = VMSHF_READ_DIR;
	*(uint32_t *)&shf->buf[10] = handle;
	*(uint32_t *)&shf->buf[14] = index;

	datalen = 18;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		if (vmshf_debug) {
			fprintf(stderr,	"rpc failed\n");
		}
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr,
			"\n                 len:%lu, status:%lu, dir:%ld, len:%lu, name:'%s'\n",
			datalen,
			*(uint32_t *)&shf->buf[6],
			*(uint32_t *)&shf->buf[10],
			*(uint32_t *)&shf->buf[55],
			*(uint32_t *)&shf->buf[55] ? (const char *)&shf->buf[59] : "(null)");
	}

	if (datalen >= 59) {
		uint32_t namelen = *(uint32_t *)&shf->buf[55];

		if (namelen) {
			*dirflag = *(uint32_t *)&shf->buf[10];
			*fileattr = (fileattr_t *)&shf->buf[14];

			shf->buf[59 + namelen] = '\0';
			Utf8ToLocal((char *)&shf->buf[59], (char *)&shf->buf[59]);

			*filename = (char *)&shf->buf[59];
		}
	}

	return VMTOOL_SUCCESS;
}

/*
	end directory listing
*/
int VMShfCloseDir(
	const shf_t	*shf,
	uint32_t	handle,
	uint32_t	*status)
{
	uint32_t	datalen;
	int			ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_CLOSE_DIR  handle:%lu -> ", handle);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[ 6] = VMSHF_CLOSE_DIR;
	*(uint32_t *)&shf->buf[10] = handle;

	datalen = 14;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu\n", *status);
	}

	return VMTOOL_SUCCESS;
}

static void PrintFileAttr(
	uint32_t attrmask,
	const fileattr_t *fileattr)
{
	time_t t;

	if (attrmask & VMSHF_ATTRMASK_FSIZE) {
		fprintf(stderr,
			"fsize: %" I64D_FMT "u\n", fileattr->fsize);
	}
	if (attrmask & VMSHF_ATTRMASK_CTIME) {
		t = FtimeToTime(fileattr->ctime, NULL);
		fprintf(stderr,
			"ctime: 0x" I64X_FMT " = %s", fileattr->ctime, t ? ctime(&t) : "\n");
	}
	if (attrmask & VMSHF_ATTRMASK_ATIME) {
		t = FtimeToTime(fileattr->atime, NULL);
		fprintf(stderr,
			"atime: 0x" I64X_FMT " = %s", fileattr->atime, t ? ctime(&t) : "\n");
	}
	if (attrmask & VMSHF_ATTRMASK_UTIME) {
		t = FtimeToTime(fileattr->utime, NULL);
		fprintf(stderr,
			"utime: 0x" I64X_FMT " = %s", fileattr->utime, t ? ctime(&t) : "\n");
	}
	if (attrmask & VMSHF_ATTRMASK_XTIME) {
		t = FtimeToTime(fileattr->xtime, NULL);
		fprintf(stderr,
			"?time: 0x" I64X_FMT " = %s", fileattr->xtime, t ? ctime(&t) : "\n");
	}
	if (attrmask & VMSHF_ATTRMASK_FMODE) {
		fprintf(stderr,
			"fmode: %c%c%c (0x%02x)\n",
			(fileattr->fmode & VMSHF_FILEMODE_READ) ? 'r' : '-',
			(fileattr->fmode & VMSHF_FILEMODE_WRITE) ? 'w' : '-',
			(fileattr->fmode & VMSHF_FILEMODE_EXEC) ? 'x' : '-',
			fileattr->fmode);
	}
}

/*
	get file attributes
*/
int VMShfGetAttr(
	const shf_t	*shf,
	const char	*filename,
	uint32_t	*status,
	uint32_t	*dirflag,
	fileattr_t	**fileattr)
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_GET_INFO   name:'%s' -> ", filename);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[6]	= VMSHF_GET_ATTR;

	namelen = LocalToUtf8((char *)&shf->buf[14], filename);

	*(uint32_t *)&shf->buf[10] = namelen;
	ReplaceDelim((char *)&shf->buf[14], namelen, '\0');

	datalen = 15 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu\n", *status);
	}

	if (datalen >= 55) {
		*dirflag = *(uint32_t *)&shf->buf[10];
		*fileattr = GETA_BUF(shf);

		if (vmshf_debug) {
			fprintf(stderr, "dflag: %d\n", *dirflag);
			PrintFileAttr(0xff, GETA_BUF(shf));
		}
	}

	return VMTOOL_SUCCESS;
}

/*
	set file attributes
*/
int VMShfSetAttr(
	const shf_t	*shf,
	uint32_t	attrmask,
	const uint8_t unknown,
	const fileattr_t *fileattr,
	const char	*filename,
	uint32_t	*status)
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr, "VMSHF_SET_INFO   name:'%s'\n", filename);
		fprintf(stderr,
			"mask:0x%04x ( %s%s%s%s%s%s)\n",
			attrmask,
			(attrmask & VMSHF_ATTRMASK_FSIZE) ? "fsize " : "",
			(attrmask & VMSHF_ATTRMASK_CTIME) ? "ctime " : "",
			(attrmask & VMSHF_ATTRMASK_ATIME) ? "atime " : "",
			(attrmask & VMSHF_ATTRMASK_UTIME) ? "utime " : "",
			(attrmask & VMSHF_ATTRMASK_XTIME) ? "?time " : "",
			(attrmask & VMSHF_ATTRMASK_FMODE) ? "fmode " : "");

		PrintFileAttr(attrmask, fileattr ? fileattr : SETA_BUF(shf));
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[ 6] = VMSHF_SET_ATTR;
	*(uint32_t *)&shf->buf[10] = attrmask;
	*&shf->buf[14] = unknown;

	if (fileattr) {
		memcpy(SETA_BUF(shf), fileattr, sizeof(fileattr_t));
	}

	namelen = LocalToUtf8((char *)&shf->buf[60], filename);

	*(uint32_t *)&shf->buf[56] = namelen;
	ReplaceDelim((char *)&shf->buf[60], namelen, '\0');

	datalen = 61 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "-> status:%lu\n", *status);
	}

	return VMTOOL_SUCCESS;
}

/*
	create a host directory
*/
int VMShfCreateDir(
	const shf_t	*shf,
	uint8_t		dirmode,
	const char	*dirname,
	uint32_t	*status)
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_CREATE_DIR name:'%s', mode:%c%c%c (0x%02x) -> ",
			dirname,
			(dirmode & VMSHF_FILEMODE_READ) ? 'r' : '-',
			(dirmode & VMSHF_FILEMODE_WRITE) ? 'w' : '-',
			(dirmode & VMSHF_FILEMODE_EXEC) ? 'x' : '-',
			dirmode);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[6]	= VMSHF_CREATE_DIR;
	shf->buf[10] = dirmode;

	namelen = LocalToUtf8((char *)&shf->buf[15], dirname);

	*(uint32_t *)&shf->buf[11] = namelen;
	ReplaceDelim((char *)&shf->buf[15], namelen, '\0');

	datalen = 16 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu\n", *status);
	}

	return VMTOOL_SUCCESS;
}

/*
	delete a host file
*/
int VMShfDeleteFile(
	const shf_t	*shf,
	const char	*filename,
	uint32_t	*status)
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_DELETE_FILE name:'%s' -> ", filename);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[6]	= VMSHF_DELETE_FILE;

	namelen = LocalToUtf8((char *)&shf->buf[14], filename);

	*(uint32_t *)&shf->buf[10] = namelen;
	ReplaceDelim((char *)&shf->buf[14], namelen, '\0');

	datalen = 15 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu\n", *status);
	}

	return VMTOOL_SUCCESS;
}

/*
	delete a host directory
*/
int VMShfDeleteDir(
	const shf_t	*shf,
	const char	*dirname,
	uint32_t	*status)
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_DELETE_DIR name:%s -> ", dirname);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[6]	= VMSHF_DELETE_DIR;

	namelen = LocalToUtf8((char *)&shf->buf[14], dirname);

	*(uint32_t *)&shf->buf[10] = namelen;
	ReplaceDelim((char *)&shf->buf[14], namelen, '\0');

	datalen = 15 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu\n", *status);
	}

	return VMTOOL_SUCCESS;
}

/*
	move/rename a host file/directory
*/
int VMShfMoveFile(
	const shf_t	*shf,
	const char	*srcname,
	const char	*dstname,
	uint32_t	*status)
{
	uint32_t srclen, dstlen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_MOVE_FILE src:'%s', dst:'%s' -> ",
			srcname, dstname);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[6]	= VMSHF_MOVE_FILE;

	srclen = LocalToUtf8((char *)&shf->buf[14], srcname);
	*(uint32_t *)&shf->buf[10] = srclen;
	ReplaceDelim((char *)&shf->buf[14], srclen, '\0');

	datalen = 15 + srclen;

	dstlen = LocalToUtf8((char *)&shf->buf[datalen + 4], dstname);
	*(uint32_t *)&shf->buf[datalen] = dstlen;
	ReplaceDelim((char *)&shf->buf[datalen + 4], dstlen, '\0');

	datalen = datalen + 5 + dstlen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (vmshf_debug) {
		fprintf(stderr, "status:%lu\n", *status);
	}

	return VMTOOL_SUCCESS;
}

/*
	get host drive free space
*/
int VMShfGetDirSize(
	const shf_t	*shf,
	const char	*dirname,
	uint32_t	*status,
	uint64_t	*avail,
	uint64_t	*total)
{
	uint32_t namelen, datalen;
	int ret;

	if (vmshf_debug) {
		fprintf(stderr,
			"VMSHF_GET_FREESPACE name:'%s' -> ", dirname);
		fflush(stderr);
	}

	*status = (uint32_t)-1;

	memcpy(shf->buf, shf_header, sizeof(shf_header));

	*(uint32_t *)&shf->buf[6]	= VMSHF_GET_DIRSIZE;

	namelen = LocalToUtf8((char *)&shf->buf[14], dirname);

	*(uint32_t *)&shf->buf[10] = namelen;
	ReplaceDelim((char *)&shf->buf[14], namelen, '\0');

	datalen = 15 + namelen;

	ret = ExecuteRpc(shf, &datalen);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	*status = *(uint32_t *)&shf->buf[6];

	if (datalen >= 26) {
		*avail = *(uint64_t *)&shf->buf[10];
		*total = *(uint64_t *)&shf->buf[18];
	}
	else {
		U64INIT(*avail, 0, 0);
		U64INIT(*total, 0, 0);
	}

	if (vmshf_debug) {
		fprintf(stderr,
			"status: %lu, free: %" I64D_FMT "u, total: %" I64D_FMT "u\n",
			*status, *avail, *total);
	}

	return VMTOOL_SUCCESS;
}

const char *VMShfStatusName(uint32_t status)
{
	switch (status) {
	case 0: return "Success";
	case 1: return "No such file or directory";
	case 3: return "Permission denied";
	case 4: return "File exists";
	case 6: return "Directory not empty";
	case 8:	return "Cannot overwrite";
	default: return "";
	}
}

