/*
	vmshrink.c
	virtual disk shrink utility

	Copyright (c) 2006 Ken Kato
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "vmtool.h"
#include "termutil.h"

#ifdef _WIN32
/*
	Windows specific
*/
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#include <windows.h>

#define sleep(s)	Sleep(s * 1000)

#elif defined(_MSDOS)
/*
	MSDOS specific
*/
#include <dos.h>
#include <time.h>

#else
/*
	generic unix like systems
*/
#include <unistd.h>

#endif	/* unix */

/*
	internal functions
*/
static int prepare_shrink(void);
static int enum_disk(char *path, uint32_t *total, uint32_t *avail);
static int wipe_disk(const char *path, uint32_t avail);

static int create_wiper(const char *path, int idx);
static uint32_t write_wiper(int file, const void *buf, uint32_t len);
static void close_wiper(int file);
static void remove_wiper(const char *path, int idx);

/*
	VMware backdoor RPC command string
*/
static const char *rpc_wiper_enable = "disk.wiper.enable";
static const char *rpc_disk_shrink	= "disk.shrink";

/*
	global variables
*/
unsigned char *wipe_buf = NULL;		/* disk wipe buffer */
size_t wipe_size = 0;				/* size of the wipe buffer	*/

/*
	virtual disk shrink main function
*/
int main(void)
{
	rpc_t		rpc;
	char		reply[50];
	uint32_t	len;
	uint16_t	id;
	int ret;

	vmtool_msg = vmtool_curt;

	/*
		check if running in VMware
	*/
	ret = VMCheckVirtual();

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	/* open a legacy RPC channel */

	ret = VMRpcOpen(&rpc, 0);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "RPC channel: %lu (conventional)\n",
				(rpc.channel >> 16));
	}

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "RPC command: disk.wiper.enable\n");
	}

	/* check shrinking availabiliry */

	ret = VMRpcSend(&rpc,
		(const unsigned char *)rpc_wiper_enable,
		strlen(rpc_wiper_enable));

	if (ret != VMTOOL_SUCCESS) {
		goto exit_main;
	}

	ret = VMRpcRecvLen(&rpc, &len, &id);

	if (ret != VMTOOL_SUCCESS || len == 0) {
		goto exit_main;
	}

	ret = VMRpcRecvDat(&rpc, (unsigned char *)reply, len, id);

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "RPC reply  : %s\n", reply);
	}

	if (*reply != '1' || *(reply + 1) != ' ' ||
		(*(reply + 2) != '0' && *(reply + 2) != '1')) {

		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Unexpected RPC reply - '%s'\n", reply);
		}
		goto exit_main;
	}

	if (*(reply + 2) != '1') {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Disk shrinking disabled\n");
		}
		goto exit_main;
	}

	/* wipe unused disk space */

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "Wiping start\n");
	}

	if (prepare_shrink() != VMTOOL_SUCCESS) {
		goto exit_main;
	}

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "Wiping complete\n");
	}

	/* shrink disk */

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "RPC command: disk.shrink\n");
	}

	/*
		give the VM a chance to update screen before the VMware
		displays the "shrinking" dialog (and the VM become frozen)
	*/
	sleep(1);

	ret = VMRpcSend(&rpc,
		(const unsigned char *)rpc_disk_shrink,
		strlen(rpc_disk_shrink));

	if (ret != VMTOOL_SUCCESS) {
		goto exit_main;
	}

	ret = VMRpcRecvLen(&rpc, &len, &id);

	if (ret != VMTOOL_SUCCESS) {
		goto exit_main;
	}

	ret = VMRpcRecvDat(&rpc, (uint8_t *)reply, len, id);

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "RPC reply  : %s\n", reply);
	}

	if (*reply != '1') {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Cancelled\n");
		}
		goto exit_main;
	}

	if (vmtool_msg > vmtool_quiet) {
		fprintf(stderr, "Complete\n");
	}

exit_main:
	VMRpcClose(&rpc);

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "RPC channel closed\n");
	}

	return ret;
}

/*
	prepare each mounted partition for shrinking
*/
static int prepare_shrink(void)
{
	char		path[1024];
	uint32_t	total, avail;
	int			ret;

	for (;;) {
		ret = enum_disk(path, &total, &avail);

		if (ret != VMTOOL_SUCCESS || path[0] == '\0') {
			break;
		}

		if (vmtool_msg > vmtool_quiet) {
			if (total != (uint32_t)-1 &&
				avail != (uint32_t)-1) {

				printf("Wiping free space on %s (%ld / %ld MB free).\n",
					path, avail / 1024, total / 1024);
			}
			else {
				printf("Wiping free space on %s.\n", path);
			}
		}

		ret = wipe_disk(path, avail);

		if (ret != VMTOOL_SUCCESS) {
			break;
		}
	}

	/* free wiper page buffer */
	if (wipe_buf) {
		free(wipe_buf);
	}

	/* end enumeration */
	enum_disk(NULL, NULL, NULL);

	return ret;
}

/*
	wipe unused space on a mounted partition
*/
static int wipe_disk(const char *path, uint32_t avail)
{
	int				wiper_idx;		/* wiper file index			*/
	uint32_t		wiped_kb;		/* size of wiped area in KB	*/
	int				ret;

	/* prepare wiper page buffer */
	if (!wipe_buf) {
#if (__INTSIZE == 2)
		wipe_size = 32768;				/* 32 KB */
#else
		wipe_size = (1024 * 1024);		/* 1 MB */
#endif

		do {
			wipe_buf = (unsigned char *)malloc(wipe_size);

			if (wipe_buf) {
				break;
			}
		}
		while ((wipe_size /= 2) >= 1024);

		if (!wipe_buf) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Cannot allocate wiping buffer\n");
			}
			return VMTOOL_SYS_ERROR;
		}

		memset(wipe_buf, 0, wipe_size);

		if (vmtool_msg >= vmtool_verbose) {
			printf("Wiping buffer: %u bytes\n", wipe_size);
		}
	}

	if (vmtool_msg > vmtool_quiet) {
		if (avail != (uint32_t)-1) {
			printf("0 / %lu MB complete (C: cancel / S: skip)\n",
				avail / 1024);
		}
		else {
			printf("0 MB complete (C: cancel / S: skip)\n");
		}
	}

	/* prepare for non-block keyboard reading */
	peekkey_prep();

	/* start wiping free space */
	wiped_kb = 0;
	wiper_idx = 0;
	ret = VMTOOL_SUCCESS;

	while (wiped_kb < avail) {
		int			file;		/* wiper file descriptor	*/
		uint32_t	filesize;	/* wiper file size			*/
		int			complete;	/* complete flag			*/

		/* create a wiper file */

		file = create_wiper(path, wiper_idx);

		if (file == -1) {
			ret = VMTOOL_SYS_ERROR;
			break;
		}

		wiper_idx++;

		filesize = 0;
		complete = 0;

		/* each wiper file doesn't exceed 1GB */
		while (filesize < 0x40000000) {
			uint32_t written;
			int key;

			/* write a page of blank data into current wiper file */

			written = write_wiper(file, wipe_buf, wipe_size);

			filesize += written;
			wiped_kb += (written / 1024);

			/* update progress display on console */

			if (vmtool_msg > vmtool_quiet) {
				cursor_up(1);	/* bring up the cursor 1 row */

				if (avail != (uint32_t)-1) {
					printf("%lu / %lu MB complete (C: cancel / S: skip)\n",
						wiped_kb / 1024, avail / 1024);
				}
				else {
					printf("%lu MB complete (C: cancel / S: skip)\n",
						wiped_kb / 1024);
				}
			}

			if (written != wipe_size) {
				complete = 1;
				break;
			}

			/* check keyboard input for skip/cancel */

			while ((key = peekkey()) != 0) {
				if (key == 'c' || key == 'C') {
					if (vmtool_msg > vmtool_quiet) {
						printf("Cencelled.\n");
					}
					ret = VMTOOL_SYS_ERROR;
					goto close_wiper;
				}
				else if (key == 's' || key == 'S') {
					if (vmtool_msg > vmtool_quiet) {
						printf("Skipped.\n");
					}
					complete = 1;
					goto close_wiper;
				}
			}
		}

close_wiper:
		close_wiper(file);

		if (ret || complete) {
			break;
		}
	}

	/* end non-blocking keyboard read */

	peekkey_end();

	/* delete wiper files */

	while (wiper_idx) {
		remove_wiper(path, --wiper_idx);
	}

	return ret;
}

/*
	target dependent functions
*/

#ifdef _WIN32

static void win32_error(DWORD err);
static void get_drive_size(const char *path, uint32_t *total, uint32_t *avail);

/*
	enumerate mounted local disks
*/
static int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	static char root[4] = {0};
	static DWORD drives = 0;

	if (path == NULL) {
		//	end enumeration
		drives = 0;
		return VMTOOL_SUCCESS;
	}

	if (drives == 0) {
		//	first time
		strcpy(root, "A:\\");
		drives = GetLogicalDrives();

		if (drives == 0) {
			if (vmtool_msg > vmtool_quiet) {
				win32_error(GetLastError());
				fprintf(stderr, "Failed to get logical drives.\n");
			}
			return VMTOOL_SYS_ERROR;
		}
	}
	else {
		//	start from the next drive of the previous call
		drives >>= 1;
		root[0]++;
	}

	while (drives && root[0] <= 'Z') {
		//	check drive type
		if ((drives & 0x01) && GetDriveType(root) == DRIVE_FIXED) {
			strcpy(path, root);
			get_drive_size(path, total, avail);
			return VMTOOL_SUCCESS;
		}
		drives >>= 1;
		root[0]++;
	}

	//	no more fixed drives
	*path = '\0';

	return VMTOOL_SUCCESS;
}

/*
	get total drive size and available size in KB
*/
static void get_drive_size(const char *path, uint32_t *total, uint32_t *avail)
{
	typedef BOOL (WINAPI * GETDISKFREESPACEEX)(
		LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

	GETDISKFREESPACEEX pGetDiskFreeSpaceEx;

	/* GetDiskFreeSpaceEx API */

	pGetDiskFreeSpaceEx = (GETDISKFREESPACEEX)GetProcAddress(
		GetModuleHandle("kernel32.dll"), "GetDiskFreeSpaceExA");

	if (pGetDiskFreeSpaceEx) {
		/* Windows 95 OSR 2 and later */

		ULARGE_INTEGER totalsize;
		ULARGE_INTEGER totalfree;
		ULARGE_INTEGER quotafree;

		if (GetDiskFreeSpaceEx(path, &quotafree, &totalsize, &totalfree)) {
			*total = (uint32_t)(totalsize.QuadPart / 1024);
			*avail = (uint32_t)(totalfree.QuadPart / 1024);
		}
		else {
			if (vmtool_msg > vmtool_quiet) {
				win32_error(GetLastError());
				fprintf(stderr, "Failed to get %s free space\n", path);
			}

			*total = *avail = (uint32_t)-1;
		}
	}
	else {
		/* Windows 95 original */

		DWORD sec_per_clus;
		DWORD bytes_per_sec;
		DWORD availclus;
		DWORD totalclus;

		if (GetDiskFreeSpace(path, &sec_per_clus,
			&bytes_per_sec, &availclus, &totalclus)) {

			*total = (uint32_t)((uint64_t)sec_per_clus *
				bytes_per_sec * totalclus / 1024);

			*avail = (uint32_t)((uint64_t)sec_per_clus *
				bytes_per_sec * availclus / 1024);
		}
		else {
			if (vmtool_msg > vmtool_quiet) {
				win32_error(GetLastError());
				fprintf(stderr, "Failed to get %s free space\n", path);
			}

			*total = *avail = (uint32_t)-1;
		}
	}
}

/*
	create a wiper file
*/
static int create_wiper(const char *path, int idx)
{
	char	name[MAX_PATH];
	HANDLE	file;

	sprintf(name, "%svmw_wiper_%lu.%lu",
		path, GetCurrentProcessId(), idx);

	file = CreateFile(name, GENERIC_WRITE,
		0, NULL, CREATE_NEW, 0, NULL);

	if (file == INVALID_HANDLE_VALUE) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Failed to create a wiper file %s\n", name);
		}
	}

	return (int)file;
}

/*
	write data to the wiper file
*/
static uint32_t write_wiper(int file, const void *buf, uint32_t len)
{
	DWORD result;

	if (!WriteFile((HANDLE)file, buf, len, &result, NULL)) {
		return 0;
	}

	return result;
}

/*
	close the wiper file
*/
static void close_wiper(int file)
{
	CloseHandle((HANDLE)file);
}

/*
	remove a wiper file
*/
static void remove_wiper(const char *path, int idx)
{
	char name[MAX_PATH];

	sprintf(name, "%svmw_wiper_%lu.%lu",
		path, GetCurrentProcessId(), idx);

	DeleteFile(name);
}

/*
	print WIN32 system error message
*/
static void win32_error(DWORD err)
{
	char msg[512];

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err, 0, msg, sizeof(msg), NULL);

	fprintf(stderr, "%s", msg);
}

#elif defined(_MSDOS)

#define DOS_DEVTYPE_5_360K	0	// 5.25 360K floppy
#define DOS_DEVTYPE_5_1P2M	1	// 5.25 1.2MB floppy
#define DOS_DEVTYPE_3_720K	2	// 3.5	720K floppy
#define DOS_DEVTYPE_8_LOW	3	// 8 inch low-density floppy
#define DOS_DEVTYPE_8_HIGH	4	// 8 inch high-density floppy
#define DOS_DEVTYPE_FIXED	5	// hard drive
#define DOS_DEVTYPE_TAPE	6	// tape drive
#define DOS_DEVTYPE_3_1P44	7	// 3.5	1.44MB floppy
#define DOS_DEVTYPE_OPTICAL	8	// optical disk
#define DOS_DEVTYPE_3_2P88	9	// 3.5	2.88MB floppy

static int drive_is_fixed(int drive, int *fat32)
{
#pragma pack(push, 1)
	typedef struct _DOSDPB {	// DOS device parameter block
		uint8_t		special_func;
		uint8_t		device_type;
		uint16_t	device_attr;
		uint16_t	num_cylinders;
		uint8_t		media_type;
		// BPB params common to fat16/fat32
		uint16_t	bytes_per_sec;
		uint8_t		secs_per_clus;
		uint16_t	reserved_secs;
		uint8_t		number_of_fat;
		uint16_t	root_entries;
		uint16_t	small_num_secs;
		uint8_t		media_desc;
		uint16_t	secs_per_fat;
		uint16_t	secs_per_track;
		uint16_t	number_of_heads;
		uint32_t	hidden_sectors;
		uint32_t	large_num_secs;
		// BPB params for fat32 only
		// fat12/fat16 drives only have 6 bytes of reserved area instead
		uint32_t	secs_per_fat32;
		uint16_t	extended_flags;
		uint16_t	file_system_ver;
		uint32_t	root_cluster;
		uint16_t	system_info_sec;
		uint16_t	backup_boot_sec;
		uint16_t	reserved_bpb[6];
		uint8_t		reserved_dpb[32];
	} DOSDPB;
#pragma pack(pop)

	DOSDPB	dpb;

//	memset(&dpb, 0x0, sizeof(dpb));
	*(int *)&dpb = 0;
	// dpb.special_func = 0;	// return default type; do not hit disk
	// dpb.device_type = 0;

	__asm {
		mov bx, drive
		sub bx, 'A' - 1		; bx = 1-based drive number

		lea dx, dpb			; ds:dx = addres of the DPB

		mov ax, 440dh		; IOCTL - GENERIC BLOCK DEVICE REQUEST
		mov cx, 4860h		; ch(48h): first try with FAT32 category code
							; cl(60h): get device parameters

		int 21h				; DOS system call
		jnc end_asm			; success if CF==0

		; IOCTL with fat32 category code failed
		; the drive does not support fat32 functions

		mov ax, 440dh		; IOCTL - GENERIC BLOCK DEVICE REQUEST
		mov ch, 08h			; retry with FAT16/12 category code

		int 21h				; DOS system call
end_asm:
	}

	if (dpb.device_type != DOS_DEVTYPE_FIXED) {
		/* dos call failure or not a fixed drive */
		return 0;
	}

	/* the drive is fat32 if sectors per fat is 0 */
	*fat32 = (dpb.secs_per_fat == 0);

	return 1;
}

static void get_diskfree_fat(int drive, uint32_t *total, uint32_t *avail)
{
	struct _diskfree_t diskfree;

	if (_dos_getdiskfree(drive - 'A' + 1, &diskfree) == 0) {
		*avail = (uint32_t)diskfree.avail_clusters *
			diskfree.bytes_per_sector *
			diskfree.sectors_per_cluster / 1024;

		*total = (uint32_t)diskfree.total_clusters *
			diskfree.bytes_per_sector *
			diskfree.sectors_per_cluster / 1024;
	}
	else {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Failed to get free space of %c:\\\n", drive);
		}
		*avail = *total = (uint32_t)-1;
	}
}

static void get_diskfree_fat32(int drive, uint32_t *total, uint32_t *avail)
{
#pragma pack(push, 1)
	typedef struct _DOSEFS {		// DOS extended free size structure
		uint16_t	struct_size;	// (ret) size of returned structure
		uint16_t	struct_ver;		// (call) structure version (0000h)
									// (ret) actual structure version (0000h)
		uint32_t	secs_per_clus;	// number of sectors per cluster
		uint32_t	bytes_per_sec;	// number of bytes per sector
		uint32_t	avail_clusters;	// number of available clusters
		uint32_t	total_clusters;	// total number of clusters on the drive
		uint32_t	avail_sectors;	// number of physical sectors available on the drive
		uint32_t	total_sectors;	// total number of physical sectors on the drive
		uint32_t	avail_allocu;	// number of available allocation units
		uint32_t	total_allocu;	// total allocation units
		uint8_t		reserved[8];
	} DOSEFS;
#pragma pack(pop)

	DOSEFS efs;
	char path[4] = " :\\";

//	memset(&efs, 0, sizeof(efs));
	efs.struct_ver = 0;

	path[0] = drive;

	__asm {
		mov dx, ds
		mov es, dx
		lea di, efs			; es:di = EFS
		mov cx, sizeof(efs)	; cx = len
		lea dx, path		; ds:dx = path
		mov ax, 7303h		; get extended free space

		int 21h
		jc	end_asm			; failure if CF!=0
							; ax contains dos error

		xor	ax, ax			; indicates success
end_asm:
	}

	if (_AX) {	// _AX refers to the ax register (DMC)

		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Failed to get free space of %s\n", path);
		}

		*avail = *total = (uint32_t)-1;
	}
	else {
		//	assume bytes_per_sec == 512 to avoid uint32 overflowing

		*avail = efs.avail_clusters *
			efs.secs_per_clus / 2;

		*total = efs.total_clusters *
			efs.secs_per_clus / 2;
	}
}

/*
	enumerate fixed local drives
*/
static int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	static char drive = 0;
	int fat32 = 0;

	if (path == NULL) {
		//	end enumeration
		drive = 0;
		return VMTOOL_SUCCESS;
	}

	if (drive == 0) {
		//	first time
		drive = 'A';
	}
	else {
		drive++;
	}

	while (drive <= 'Z') {
		if (drive_is_fixed(drive, &fat32)) {
			sprintf(path, "%c:\\", drive);

			if (fat32) {
				get_diskfree_fat32(drive, total, avail);
			}
			else {
				get_diskfree_fat(drive, total, avail);
			}

			return VMTOOL_SUCCESS;
		}

		drive++;
	}

	//	no more fixed drives
	drive = 0;
	*path = '\0';

	return VMTOOL_SUCCESS;
}

/*
	create a wiper file
*/
static int create_wiper(const char *path, int idx)
{
	char name[_MAX_PATH];
	int file;

	sprintf(name, "%svmwiper.%03lu", path, idx);

	if (_dos_creatnew(name, _A_NORMAL, &file) != 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Failed to create a wiper file %s\n", name);
		}
		return -1;
	}

	return file;
}

/*
	write data into a wiper file
*/
static uint32_t write_wiper(int file, const void *buf, uint32_t len)
{
	unsigned result;

	if (_dos_write(file, buf, len, &result) != 0) {
		return 0;
	}

	return result;
}

/*
	close a wiper file
*/
static void close_wiper(int file)
{
	_dos_close(file);
}

/*
	remove a wiper file
*/
static void remove_wiper(const char *path, int idx)
{
	char name[_MAX_PATH];

	sprintf(name, "%svmwiper.%lu", path, idx);

	remove(name);
}
#else
/*
	unix like systems...
*/
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>

static int create_wiper(const char *path, int idx)
{
	char name[PATH_MAX];
	int file;

	sprintf(name, "%s/vmw_wiper_%lu.%lu",
		path, getpid(), idx);

	file = open(name, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);

	if (file == -1) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "Failed to create a wiper file %s\n", name);
		}
	}

	return file;
}

static uint32_t write_wiper(int file, const void *buf, uint32_t len)
{
	return (uint32_t)write(file, buf, len);
}

static void close_wiper(int file)
{
	close(file);
}

static void remove_wiper(const char *path, int idx)
{
	char name[PATH_MAX];

	sprintf(name, "%s/vmw_wiper_%lu.%lu", path, getpid(), idx);
	remove(name);
}

/*
	system dependent functions
*/
#if defined(__linux__)

#include <mntent.h>
#include <sys/types.h>
#include <sys/statvfs.h>

/*
	enumerate mounted local block devices
*/
static int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	static FILE *mtab = NULL;
	struct mntent *mnt;
	struct statvfs vfs;

	if (path == NULL) {
		//	end enumeration
		if (mtab) {
			endmntent(mtab);
			mtab = NULL;
		}
		return VMTOOL_SUCCESS;
	}

	if (mtab == NULL) {
		//	first time
		mtab = setmntent("/etc/mtab", "r");

		if (mtab == NULL) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to open /etc/mtab\n");
			}
			return VMTOOL_SYS_ERROR;
		}
	}

	while ((mnt = getmntent(mtab)) != NULL) {
		if (!mnt->mnt_type || !mnt->mnt_dir ||
			!hasmntopt(mnt, "rw")) {
			continue;
		}

		if (!strcmp(mnt->mnt_type, "adfs") ||
			!strcmp(mnt->mnt_type, "affs") ||
			//!strcmp(mnt->mnt_type, "bfs") ||
			//!strcmp(mnt->mnt_type, "coda") ||
			!strcmp(mnt->mnt_type, "coherent") ||
			//!strcmp(mnt->mnt_type, "cramfs") ||
			//!strcmp(mnt->mnt_type, "efs") ||
			!strcmp(mnt->mnt_type, "ext") ||
			!strcmp(mnt->mnt_type, "ext2") ||
			!strcmp(mnt->mnt_type, "ext3") ||
			!strcmp(mnt->mnt_type, "fat") ||
			!strcmp(mnt->mnt_type, "gfs") ||
			//!strcmp(mnt->mnt_type, "hfs") ||
			//!strcmp(mnt->mnt_type, "jfs") ||
			!strcmp(mnt->mnt_type, "minix") ||
			!strcmp(mnt->mnt_type, "msdos") ||
			!strcmp(mnt->mnt_type, "ntfs") ||
			//!strcmp(mnt->mnt_type, "qnx4") ||
			!strcmp(mnt->mnt_type, "reiserfs") ||
			!strcmp(mnt->mnt_type, "sysv") ||
			!strcmp(mnt->mnt_type, "ufs") ||
			!strcmp(mnt->mnt_type, "umsdos") ||
			!strcmp(mnt->mnt_type, "vfat") ||
			//!strcmp(mnt->mnt_type, "vxfs") ||
			!strcmp(mnt->mnt_type, "xenix") ||
			!strcmp(mnt->mnt_type, "xfs") ||
			!strcmp(mnt->mnt_type, "xiafs")) {

			strcpy(path, mnt->mnt_dir);

			if (statvfs(path, &vfs) == 0) {
				*total = (uint32_t)((long long)vfs.f_frsize *
					vfs.f_blocks / 1024);

				*avail = (uint32_t)((long long)vfs.f_frsize *
					vfs.f_bfree / 1024);
			}
			else {
				if (vmtool_msg > vmtool_quiet) {
					fprintf(stderr, "failed to get %s free space\n", path);
				}
				*total = *avail = (uint32_t)-1;
			}

			return VMTOOL_SUCCESS;
		}
	}

	//	no more mounted partition
	*path = '\0';
	endmntent(mtab);
	mtab = NULL;

	return VMTOOL_SUCCESS;
}

#elif defined(__sun)

#include <sys/mnttab.h>
#include <sys/statvfs.h>

static int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	static FILE *mnttab = NULL;
	struct mnttab mnt;
	struct statvfs vfs;

	if (path == NULL) {
		//	end enumeration
		if (mnttab) {
			fclose(mnttab);
			mnttab = NULL;
		}
		return VMTOOL_SUCCESS;
	}

	if (mnttab == NULL) {
		//	first time
		mnttab = fopen("/etc/mnttab", "r");

		if (mnttab == NULL) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to open /etc/mnttab\n");
			}
			return VMTOOL_SYS_ERROR;
		}
	}

	while (getmntent(mnttab, &mnt) == 0) {
		if (mnt.mnt_fstype && mnt.mnt_mountp &&
			!strcmp(mnt.mnt_fstype, "ufs") &&
			hasmntopt(&mnt, "rw")) {

			strcpy(path, mnt.mnt_mountp);

			if (statvfs(path, &vfs) == 0) {
				*total = (uint32_t)((long long)vfs.f_frsize *
					vfs.f_blocks / 1024);

				*avail = (uint32_t)((long long)vfs.f_frsize *
					vfs.f_bfree / 1024);
			}
			else {
				if (vmtool_msg > vmtool_quiet) {
					fprintf(stderr, "failed to get %s free space\n", path);
				}

				*total = *avail = (uint32_t)-1;
			}

			return VMTOOL_SUCCESS;
		}
	}

	//	no more mounted partition
	*path = NULL;

	fclose(mnttab);
	mnttab = NULL;

	return VMTOOL_SUCCESS;
}

#elif defined(__FreeBSD__) || defined(__OpenBSD__)

#include <sys/param.h>
#ifdef __FreeBSD__
#include <sys/ucred.h>
#endif /* __FreeBSD__ */
#include <sys/mount.h>

static int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	static struct statfs *mnt = NULL;
	static int maxfs = 0, current = 0;

	if (path == NULL) {
		//	end enumeration
		if (mnt) {
			free(mnt);
			mnt = NULL;
		}
		return VMTOOL_SUCCESS;
	}

	if (mnt == NULL) {
		//	first time
	 	maxfs = getfsstat(NULL, 0, MNT_WAIT);

	 	if (maxfs == -1) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to getfsstat\n");
			}
			return VMTOOL_SYS_ERROR;
		}

		if (maxfs == 0) {
			*path = '\0';
			return VMTOOL_SUCCESS;
		}

		mnt = (struct statfs *)malloc(
			maxfs * sizeof(struct statfs));

		if (!mnt) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to alloc memory\n");
			}
			return VMTOOL_SYS_ERROR;
		}

	 	maxfs = getfsstat(mnt, maxfs * sizeof(struct statfs), MNT_WAIT);

		if (maxfs == -1) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to getfsstat\n");
			}
			free(mnt);
			mnt = NULL;
			return VMTOOL_SYS_ERROR;
		}

		current = 0;
	}
	else {
		current++;
	}

	while (current < maxfs) {
		if ((mnt[current].f_flags & MNT_LOCAL) &&
			!(mnt[current].f_flags & MNT_RDONLY)) {

			strcpy(path, mnt[current].f_mntonname);

			*total = (uint32_t)(mnt[current].f_bsize *
				mnt[current].f_blocks / 1024);

			*avail = (uint32_t)(mnt[current].f_bsize *
				mnt[current].f_bfree / 1024);

			return VMTOOL_SUCCESS;
		}

		current++;
	}

	// no more mounted partition
	*path = '\0';

	free(mnt);
	mnt = NULL;

	return VMTOOL_SUCCESS;
}

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <sys/statvfs.h>

static int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	static struct statvfs *mnt = NULL;
	static int maxfs = 0, current = 0;

	if (path == NULL) {
		//	end enumeration
		if (mnt) {
			free(mnt);
			mnt = NULL;
		}
		return VMTOOL_SUCCESS;
	}

	if (mnt == NULL) {
		//	first time
	 	maxfs = getvfsstat(NULL, 0, ST_WAIT);

	 	if (maxfs == -1) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to getvfsstat\n");
			}
			return VMTOOL_SYS_ERROR;
		}

		if (maxfs == 0) {
			*path = '\0';
			return VMTOOL_SUCCESS;
		}

		mnt = (struct statvfs *)malloc(
			maxfs * sizeof(struct statvfs));

		if (!mnt) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to alloc memory\n");
			}
			return VMTOOL_SYS_ERROR;
		}

	 	maxfs = getvfsstat(mnt, maxfs * sizeof(struct statvfs), ST_WAIT);

		if (maxfs == -1) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Failed to getfsstat\n");
			}
			free(mnt);
			mnt = NULL;
			return VMTOOL_SYS_ERROR;
		}

		current = 0;
	}
	else {
		current++;
	}

	while (current < maxfs) {
		if ((mnt[current].f_flag & ST_LOCAL) &&
			!(mnt[current].f_flag & ST_RDONLY)) {

			strcpy(path, mnt[current].f_mntonname);

			*total = (uint32_t)(mnt[current].f_bsize *
				mnt[current].f_blocks / 1024);

			*avail = (uint32_t)(mnt[current].f_bsize *
				mnt[current].f_bfree / 1024);

			return VMTOOL_SUCCESS;
		}

		current++;
	}

	// no more mounted partition
	*path = '\0';

	free(mnt);
	mnt = NULL;

	return VMTOOL_SUCCESS;
}

#elif defined(__minix)

#include <minix/minlib.h>

static int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	static int loaded = -1;
	char dev[256], mnt[256], ver[10], rw[10];

	if (path == NULL) {
		if (loaded == 0) {
			loaded = -1;
		}
		return VMTOOL_SUCCESS;
	}

	if (loaded < 0) {
		/*	first time */
		loaded = load_mtab("vmw");

		if (loaded < 0) {
			return VMTOOL_SYS_ERROR;
		}
	}

	for (;;) {
		if (get_mtab_entry(dev, mnt, ver, rw) < 0) {
			break;
		}

		if (strcmp(rw, "rw") == 0) {
			strcpy(path, mnt);

			/* don't know how to get disk free space */
			*total = *avail = (uint32_t)-1;

			return VMTOOL_SUCCESS;
		}
	}

	/*	no more devices */
	*path = '\0';
	loaded = -1;

	return VMTOOL_SUCCESS;
}

#else	/* unknown unix system */

int enum_disk(char *path, uint32_t *total, uint32_t *avail)
{
	if (vmtool_msg > vmtool_quiet) {
		fprintf(stderr, "don't know how to get mount points\n");
	}
	return VMTOOL_SYS_ERROR;
}

#endif	/* unknown unix system */

#endif	/* unix systems */
