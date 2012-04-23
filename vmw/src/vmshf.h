/*
	vmshf.h
	file transfer via VMware Shared Folders

	Copyright (c) 2006 Ken Kato
*/

#ifndef _VMSHF_H_
#define _VMSHF_H_

#include "vmtool.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
	default transfer block size
*/
#define VMSHF_BLOCK_SIZE		4096

/*
	max path string length
*/
#define VMSHF_MAX_PATH			1024

/*
	known shared folder command values
*/
#define VMSHF_OPEN_FILE			0		/* open/create file					*/
#define VMSHF_READ_FILE			1		/* read data from file				*/
#define VMSHF_WRITE_FILE		2		/* write data into file				*/
#define VMSHF_CLOSE_FILE		3		/* close file						*/
#define VMSHF_OPEN_DIR			4		/* open directory					*/
#define VMSHF_READ_DIR			5		/* read a directory entry			*/
#define VMSHF_CLOSE_DIR			6		/* close directory					*/
#define VMSHF_GET_ATTR			7		/* get file/directory attributes	*/
#define VMSHF_SET_ATTR			8		/* set file/directory attributes	*/
#define VMSHF_CREATE_DIR		9		/* create directory					*/
#define VMSHF_DELETE_FILE		10		/* delete file						*/
#define VMSHF_DELETE_DIR		11		/* delete directory					*/
#define VMSHF_MOVE_FILE			12		/* move/rename file/directory		*/
#define VMSHF_GET_DIRSIZE		13		/* get directory size				*/
#define VMSHF_CMD_MAX			14		/* maximum value place holder		*/

/*
	known shared folder status values
*/
#define VMSHF_ENOTEXIST			1		/* file not exists 					*/
#define VMSHF_EACCESS			3		/* permission denied				*/
#define VMSHF_EEXIST			4		/* file exists						*/
#define VMSHF_ENOTEMPTY			6		/* directory is not empty			*/
#define VMSHF_EOVERWRITE		8		/* cannot overwrite a file			*/

/*
	file permission mask
*/
#define VMSHF_FILEMODE_EXEC		0x01	/* file is executable				*/
#define VMSHF_FILEMODE_WRITE	0x02	/* file is writable					*/
#define VMSHF_FILEMODE_READ		0x04	/* file is readable					*/
#define VMSHF_FILEMODE_MASK		0x07	/* valid filemode bit mask			*/

/*
	file open access mode
*/
#define VMSHF_ACCESS_READONLY	0		/* open for read only access		*/
#define VMSHF_ACCESS_READWRITE	2		/* open for read/write access		*/

/*
	file open mode
*/
#define VMSHF_OPENMODE_O_EXIST	0		/* file must exist					*/
#define VMSHF_OPENMODE_O_ALWAYS	2		/* created if not exists			*/
#define VMSHF_OPENMODE_C_NEW	3		/* file must not exist				*/
#define VMSHF_OPENMODE_C_ALWAYS	4		/* truncated if exists				*/

/*
	set attributes mask values
*/
#define VMSHF_ATTRMASK_FSIZE	0x01	/* set file size					*/
#define VMSHF_ATTRMASK_CTIME	0x02	/* set create time					*/
#define VMSHF_ATTRMASK_ATIME	0x04	/* set access time					*/
#define VMSHF_ATTRMASK_UTIME	0x08	/* set update time					*/
#define VMSHF_ATTRMASK_XTIME	0x10	/* set unknown time					*/
#define VMSHF_ATTRMASK_FMODE	0x20	/* set file mode					*/

/*
	struct for get/set file attributes
*/
#if defined(__GNUC__)
#define GCC_PACKED	__attribute__((packed))
#else	/* __GNUC__ */
#define GCC_PACKED
#if defined(_MSC_VER) || defined(__DMC__)
#pragma pack(push, 1)
#elif defined(__SUNPRO_C) || defined(__SCO_VERSION__) || defined(__ACK__)
#pragma pack(1)
#else
#error "No struct packing directive for the target compiler"
#endif	/* compiler */
#endif	/* __GNUC__ */

typedef struct _fileattr_t
{
	uint64_t fsize;						/* file size in bytes			*/
	uint64_t ctime;						/* created date/time			*/
	uint64_t atime;						/* last accessed date/time		*/
	uint64_t utime;						/* last updated date/time		*/
	uint64_t xtime;						/* unknown date/time: has the 	*/
										/* same value as utime in most	*/
										/* cases						*/
	uint8_t	fmode;						/* file permission mode mask	*/
										/* 0x4:r, 0x2:w, 0x1:x			*/
} GCC_PACKED fileattr_t;

/*
	file time values are 64 bit integer value representing the number
	of 100-nanosecond intervals since 1601-01-01 00:00:00 (UTC).
	(the same data as the FILETIME structure in Win32 API)
*/

#if defined(_MSC_VER) || defined(__DMC__)
#pragma pack(pop)
#elif defined(__SUNPRO_C) || defined(__SCO_VERSION__) || defined(__ACK__)
#pragma pack()
#endif	/* compiler */

/*
	shared folder context struct
*/

typedef struct _shf_t {
	rpc_t	rpc;				/* RPC context			*/
	uint8_t *buf;				/* data transfer buffer	*/
} shf_t;

#define READ_BUF(shf)	&(shf)->buf[14]
#define WRITE_BUF(shf)	&(shf)->buf[27]
#define SETA_BUF(shf)	((fileattr_t *)&(shf)->buf[15])
#define GETA_BUF(shf)	((fileattr_t *)&(shf)->buf[14])

extern int vmshf_debug;			/* debug flag	*/

/*
	convert a vmshf style 64 bit time value into
	a unix style 32 bit time value and a 32 bit nanosecond value
*/
uint32_t FtimeToTime(			/* ret: time_t value */
	uint64_t	filetime,		/* in : 64 bit time value */
	uint32_t	*nanosec);		/* out: nanosecond value */

/*
	convert a unix style 32 bit time value and a 32 bit nanosecond value
	into a vmshf style 64 bit time value
*/
uint64_t TimeToFtime(			/* ret: 64 bit time value */
	uint32_t	seconds,		/* in : time_t value */
	uint32_t	nanosec);		/* in : nanosecond value */

/*
	begin file transfer session
*/
int VMShfBeginSession(			/* ret: backdoor error code */
	shf_t		*shf);			/* out: shared folder context */

/*
	end file transfer session
*/
void VMShfEndSession(			/* ret: none */
	shf_t		*shf);			/* in : shared folder context */

/*
	open a file in a shared folder
*/
int VMShfOpenFile(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint32_t	access,			/* in : access mode */
	uint32_t	openmode,		/* in : open mode */
	uint8_t		filemode,		/* in : file permission */
	const char	*filename,		/* in : file name */
	uint32_t	*status,		/* out: shared folder status */
	uint32_t	*handle);		/* out: file handle */

/*
	read data from a shared file
*/
int VMShfReadFile(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint32_t	handle,			/* in : file handle */
	uint64_t	offset,			/* in : byte offset to read */
	uint32_t	*length,		/* i/o: bytes to read / bytes retrieved */
	uint32_t	*status);		/* out: shared folder status */

/*
	write data into a shared file
*/
int VMShfWriteFile(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint32_t	handle,			/* in : file handle */
	uint8_t		unknown,		/* in : unknown write parameter */
	uint64_t	offset,			/* in : byte offset to write */
	uint32_t	*length,		/* i/o: bytes to write / bytes written */
	uint32_t	*status);		/* out: shared folder status */

/*
	close a shared file
*/
int VMShfCloseFile(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint32_t	handle,			/* in : file handle */
	uint32_t	*status);		/* out: shared folder status */

/*
	open a shared directory for file listing
*/
int VMShfOpenDir(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	const char	*dirname,		/* in : shared directory name */
	uint32_t	*status,		/* out: shared folder status */
	uint32_t	*handle);		/* out: directory handle */

/*
	read a file entry from a shared directory
*/
int VMShfReadDir(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint32_t	handle,			/* in : directory handle */
	uint32_t	index,			/* in : 0 based index number */
	uint32_t	*status,		/* out: shared folder status */
	uint32_t	*dirflag,		/* out: directory flag */
	fileattr_t	**fileattr,		/* out: file attributes */
	char		**filename);	/* out: file name */

/*
	close a shared directory
*/
int VMShfCloseDir(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint32_t	handle,			/* in : directory handle */
	uint32_t	*status);		/* out: shared folder status */

/*
	get shared file attributes
*/
int VMShfGetAttr(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	const char	*filename,		/* in : shared file name */
	uint32_t	*status,		/* out: shared folder status */
	uint32_t	*dirflag,		/* out: directory flag */
	fileattr_t	**fileattr);	/* out: file attributes */

/*
	set shared file attributes
*/
int VMShfSetAttr(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint32_t	attrmask,		/* in : attribute bit mask to set */
	const uint8_t unknown,		/* in : inknown set attr parameter */
	const fileattr_t *fileattr,	/* in : file attributes */
	const char	*filename,		/* in : file name */
	uint32_t	*status);		/* out: shared folder status */

/*
	create a directory in a shared directory
*/
int VMShfCreateDir(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	uint8_t		dirmode,		/* in : directory permission */
	const char	*dirname,		/* in : directory name */
	uint32_t	*status);		/* out: shared folder status */

/*
	delete a file in a shared directory
*/
int VMShfDeleteFile(			/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	const char	*filename,		/* in : file name */
	uint32_t	*status);		/* out: shared folder status */

/*
	delete a directory in a shared directory
*/
int VMShfDeleteDir(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	const char	*dirname,		/* in : directory name */
	uint32_t	*status);		/* out: shared folder status */

/*
	move / rename a file an a shared directory
*/
int VMShfMoveFile(				/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	const char	*srcname,		/* in : source file name */
	const char	*dstname,		/* in : destination file name */
	uint32_t	*status);		/* out: shared folder status */

/*
	get shared directory size (?)
*/
int VMShfGetDirSize(			/* ret: backdoor error code */
	const shf_t	*shf,			/* in : shared folder context */
	const char	*dirname,		/* in : directory name */
	uint32_t	*status,		/* out: shared folder status */
	uint64_t	*avail,			/* out: available size in bytes */
	uint64_t	*total);		/* out: total size in bytes */

/*
	get shared folder status name
*/
const char *VMShfStatusName(	/* ret: status name */
	uint32_t	status);		/* in : status code */

#ifdef __cplusplus
}
#endif

#endif	/* _VMSHF_H_ */
