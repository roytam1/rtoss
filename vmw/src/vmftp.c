/*
	vmftp.c
	simple FTP like client for Shared Folder access

	Copyright (c) 2006 Ken Kato
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "vmshf.h"

/*
	some system dependent stuff...
*/
#if defined(_WIN32) || defined(_MSDOS)

#include <ctype.h>
#include <direct.h>
#include <sys/utime.h>

#define PATH_DELIMIT		"\\"
#define FOPEN_READ_MODE		"rb"
#define FOPEN_WRITE_MODE	"wb"

#define create_dir(dir)		mkdir(dir)

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>

typedef long dir_t;

#else	/* !_WIN32 */

#include <dos.h>
#include <signal.h>

typedef struct find_t dir_t;

#endif	/* !_WIN32 */

#else	/* !(defined(_WIN32) || defined(_MSDOS)) */

#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <utime.h>

#define PATH_DELIMIT		"/"
#define FOPEN_READ_MODE		"r"
#define FOPEN_WRITE_MODE	"w"

#define create_dir(dir)		mkdir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

typedef struct _dir_t {
	DIR *dir;
	char path[VMSHF_MAX_PATH];
} dir_t;

#endif	/* !(defined(_WIN32) || defined(_MSDOS)) */

/*
	local functions
*/
static int parse_input(char *cmd, char **arg1, char **arg2);
static uint32_t normalize_path(char *str);
static const char *format_filetime(uint64_t value);
static void local_fullpath(char *dst, const char *src);
static void remote_fullpath(char *dst, const char *src);
static void safe_fullpath(char *path, int outward);

static void vmftp_put(const shf_t *shf, const char *local, const char *remote);
static int vmftp_putdir(const shf_t *shf, char *local, char *remote, uint32_t *files, uint64_t *bytes, const char *pattern);
static int vmftp_putfile(const shf_t *shf, const char *local, const char *remote, uint64_t *total, time_t modtime);
static void vmftp_get(const shf_t *shf, const char *local, const char *remote);
static int vmftp_getdir(const shf_t *shf, char *local, char *remote, uint32_t *files, uint64_t *bytes, const char *pattern);
static int vmftp_getfile(const shf_t *shf, const char *local, const char *remote, uint64_t *total, time_t modtime);
static void vmftp_ls(const shf_t *shf, const char *dirname);
static void vmftp_cd(const shf_t *shf, const char *dirname);
static void vmftp_lcd(const char *dirname);
static void vmftp_chmod(const shf_t *shf, const char *mode, const char *file);
static void vmftp_mkdir(const shf_t *shf, const char *dirname);
static void vmftp_delete(const shf_t *shf, const char *filename);
static void vmftp_rename(const shf_t *shf, const char *oldname, const char *newname);
static void vmftp_rmdir(const shf_t *shf, const char *dirname);

static void ignore_signal(void);
static void capture_signal(void);
static int get_dirent_first(dir_t *dir, const char *dirname, char *name, int *dirflag, time_t *modtime);
static int get_dirent(dir_t *dir, char *name, int *dirflag, time_t *modtime);
static int close_dir(const dir_t *dir);
static int pattern_match(const char *name, const char *pattern, int case_sensitive);
/*
	static variables
*/

/* Ctrl+C indicator during file transfer */
static int vmftp_aborted = 0;

/* working directory in the host */
static char remote_cwd[1024];

/* vmftp command constants */
enum {
	COMMAND_SHELL = 0,
	COMMAND_HELP,
	COMMAND_BYE,
	COMMAND_CD,
	COMMAND_CHMOD,
	COMMAND_DEBUG,
	COMMAND_DEL,
	COMMAND_GET,
	COMMAND_LCD,
	COMMAND_LS,
	COMMAND_MKDIR,
	COMMAND_PUT,
	COMMAND_PWD,
	COMMAND_QUIT,
	COMMAND_REN,
	COMMAND_RMDIR,
	NO_MATCH = -1
};

static const struct {
	const char *cmd;
	const char *help;
}
commands[] = {
	{ "!",		"! [command [args]]\n"
				"        Invoke an interactive shell on the local (guest) machine.  If there are\n"
				"        arguments, the first is taken to be a command to execute directly, with\n"
				"        the rest of the arguments as its arguments.\n" },

	{ "?",		"? [command]\n"
				"        Print an informative message about the meaning of <command>.  If no\n"
				"        argument is given, vmftp prints a list of the known commands.\n" },

	{ "bye",	"bye     Terminate the vmftp session and exit.  An end of file will also\n"
				"        terminate the session and exit.\n" },

	{ "cd",		"cd [remote-directory]\n"
				"        Change the working directory on the remote (host) machine to <remote-\n"
				"        directory>.  If no directory is specified, the root is used.\n" },

	{ "chmod",	"chmod <mode> <remote-file>\n"
				"        Change the permission modes of the file <remote-file> on the remote\n"
				"        (host) system to <mode>.  <mode> must be one of '+r', '-r', '+w', '-w',\n"
				"        '+r' or '-r'.  Combinations of more than one or unix-style octal values\n"
				"        (e.g. 644) are not acceptable.  'r' and 'x' modes do not have an effect\n"
				"        on Windows host.\n" },

	{ "debug",	"debug   Toggle debugging mode.  When debugging is on, vmftp prints parameters\n"
				"        and status of each shared folder function it executes.\n" },

	{ "delete",	"delete <remote-file>\n"
				"        Delete the file <remote-file> on the remote (host) machine.\n" },

	{ "get",	"get <remote-file> [local-file]\n"
				"        Retrieve the file <remote-file> and store it on the local (guest)\n"
				"        machine.  If the local file name is not specified, the remote file name\n"
				"        is used.  Existing local file is overwritten unless the file has the\n"
				"        write permission disabled.\n" },

	{ "lcd",	"lcd [directory]\n"
				"        Change the working directory on the local (guest) machine.  If no\n"
				"        directory is specified, the user's home directory is used.\n" },

	{ "ls",		"ls [remote-directory]\n"
				"        Print a listing of the directory contents in the <remote-directory>.\n"
				"        If no directory is specified, the current working directory on the\n"
				"        remote (host) machine is used.\n" },

	{ "mkdir",	"mkdir <directory-name>\n"
				"        Create a directory on the remote (host) machine.\n" },

	{ "put",	"put <local-file> [remote-file]\n"
				"        Send the file <local-file> and store it on the remote (host) machine.\n"
				"        If the remote file name is not specified, the local file name is used\n"
				"        Existing remote file is overwritten unless the file has the write\n"
				"        permission disabled.\n" },

	{ "pwd",	"pwd     Print the current working directory on the remote (host) machine.\n" },

	{ "quit",	"quit    A synonym for bye.\n" },

	{ "rename",	"rename <from> <to>\n"
				"        Rename the file <from> on the remote (host) machine, to the file <to>.\n" },

	{ "rmdir",	"rmdir <directory-name>\n"
				"        Delete a directory on the remote (host) machine.\n" },

	{ NULL, NULL }
};


/*
	vmftp main function
*/
int main(void)
{
	shf_t	shf;
	char	input_buf[1024];
	int		ret;

	/*
		check if running in VMware
	*/
	ret = VMCheckVirtual();

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	/*
		open an RPC channel for shared folder access
	*/
	ret = VMShfBeginSession(&shf);

	if (ret != VMTOOL_SUCCESS) {
		printf("Cannot access Shared Folders.\n");
		return ret;
	}

	/*
		set signal handler to catch Ctrl+C
	*/
	ignore_signal();

	/*
		show title banner
	*/
	printf(
		"VMware guest file transfer program - Copyright (c) 2006 Ken Kato\n");

#ifdef _MSDOS
	/*
		check TZ environment
	*/
	if (getenv("TZ") == NULL) {
		printf(
			"TZ environment is not set. File date/time are handled in GMT.\n");
		_putenv("TZ=GMT+0");
	}
#endif

	printf("Type '?' for a list of available commands\n");

	for (;;) {
		char *arg1, *arg2;

		printf("vmftp> ");
		fflush(stdout);

		while (fgets(input_buf, sizeof(input_buf), stdin) == NULL) {
			if (feof(stdin)) {
				goto quit_ftp;
			}
		}

		switch (parse_input(input_buf, &arg1, &arg2)) {
		case COMMAND_SHELL:
			/* escape to shell */
			if (!arg1 || !*arg1) {
#if defined(_WIN32)
				if ((arg1 = getenv("COMSPEC")) == NULL) {
					if ((GetVersion() & 0x80000000) == 0) {
						/* NT systems */
						arg1 = "cmd.exe";
					}
					else {
						/* Win 9x */
						arg1 = "command.com";
					}
				}
#elif defined(_MSDOS)
				if ((arg1 = getenv("COMSPEC")) == NULL) {
					arg1 = "command.com";
				}
#else
				if ((arg1 = getenv("SHELL")) == NULL) {
					arg1 = "/bin/sh";
				}
#endif
			}
			system(arg1);
			break;

		case COMMAND_HELP:
			ret = parse_input(arg1, NULL, NULL);

			if (ret >= 0) {
				printf("%s\n", commands[ret].help);
			}
			else {
				int i = 0;
				while (commands[i].cmd) {
					printf("%-8.8s", commands[i++].cmd);
					if (i % 8 == 0) {
						printf("\n");
					}
				}
				if (i % 8) {
					printf("\n");
				}
				printf("Type '? <command>' for details\n");
			}
			break;

		case COMMAND_BYE:
		case COMMAND_QUIT:
			goto quit_ftp;

		case COMMAND_CD:
			vmftp_cd(&shf, arg1);
			break;

		case COMMAND_CHMOD:
			vmftp_chmod(&shf, arg1, arg2);
			break;

		case COMMAND_DEBUG:
			vmshf_debug = !vmshf_debug;
			printf("Debug mode %s\n", vmshf_debug ? "on" : "off");
			break;

		case COMMAND_DEL:
			vmftp_delete(&shf, arg1);
			break;

		case COMMAND_LS:
			vmftp_ls(&shf, arg1);
			break;

		case COMMAND_GET:
			vmftp_get(&shf, arg2, arg1);
			break;

		case COMMAND_LCD:
			vmftp_lcd(arg1);
			break;

		case COMMAND_MKDIR:
			vmftp_mkdir(&shf, arg1);
			break;

		case COMMAND_PUT:
			vmftp_put(&shf, arg1, arg2);
			break;

		case COMMAND_PWD:
			printf("/%s\n", remote_cwd);
			break;

		case COMMAND_REN:
			vmftp_rename(&shf, arg1, arg2);
			break;

		case COMMAND_RMDIR:
			vmftp_rmdir(&shf, arg1);
			break;
		}
	}

quit_ftp:
	/*
		close the RPC channel
	*/
	VMShfEndSession(&shf);

	return VMTOOL_SUCCESS;
}

/*
	parse command input
*/
static int parse_input(char *cmd, char **arg1, char **arg2)
{
	int len;
	int idx;
	int match;
	char *p;

	if (!cmd) {
		return NO_MATCH;
	}

	if (arg1) {
		*arg1 = NULL;
	}

	if (arg2) {
		*arg2 = NULL;
	}

	/* skip leading blanks */
	while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n') {
		cmd++;
	}

	if (!*cmd) {
		return NO_MATCH;
	}

	/* special cases where command does not need a delimiter */
	if (*cmd == '!' || *cmd == '?') {
		if (arg1) {
			p = cmd + 1;

			while (*p == ' ' || *p == '\t' || *p == '\n') {
				p++;
			}

			if (*p) {
				*arg1 = p;

				while (*(++p)) {
					if (*p == '\n') {
						*p = '\0';
						break;
					}
				}
			}
		}
		return *cmd == '!' ? COMMAND_SHELL : COMMAND_HELP;
	}

	/* command is delimited with space, tab or newline */
	len = strcspn(cmd, " \t\n");

	if (len == 0) {
		return NO_MATCH;
	}

	idx = 0;
	match = NO_MATCH;

	while (commands[idx].cmd) {

		if (!strncmp(cmd, commands[idx].cmd, len)) {

			if (match == NO_MATCH) {			/*	first match */
				match = idx;
			}
			else {								/*	multiple matches */
				printf("Ambiguous command\n");
				return NO_MATCH;
			}
		}

		idx++;
	}

	if (match == NO_MATCH) {					/*	match not found */
		printf("Invalid command\n");
		return match;
	}

	/* get arguments */
	p = cmd + len;

	while (*p == ' ' || *p == '\t' || *p == '\n') {
		p++;
	}

	if (arg1 && *p) {
		*arg1 = p;
		if (*p == '\"') {
			(*arg1)++;
			while (*(++p)) {
				if (*p == '\"') {
					*(p++) = '\0';
					break;
				}
			}
		}
		else {
			while (*(++p)) {
				if (*p == ' ' || *p == '\t' || *p == '\n') {
					*(p++) = '\0';
					break;
				}
			}
		}
	}

	while (*p == ' ' || *p == '\t' || *p == '\n') {
		p++;
	}

	if (arg2 && *p) {
		*arg2 = p;
		if (*p == '\"') {
			(*arg2)++;
			while (*(++p)) {
				if (*p == '\"') {
					*(p++) = '\0';
					break;
				}
			}
		}
		else {
			while (*(++p)) {
				if (*p == ' ' || *p == '\t' || *p == '\n') {
					*(p++) = '\0';
					break;
				}
			}
		}
	}

	return match;
}


/*
	normalize a path string
*/
static uint32_t normalize_path(
	char *str)
{
	uint32_t i = 0, j = 0;

#if defined(_WIN32) || defined(_MSDOS)
	/* leave "x:" at the top */
	if (isalpha(*str) && *(str + 1) == ':' &&
		(*(str + 2) == '\\' || *(str + 2) == '/')) {
		str += 2;
	}
#else
	/* leave a "/" at the top */
	/*
	if (*str == '/') {
		str++;
	}
	*/
#endif

	while (*(str + i)) {
		if (*(str + i) == '/' || *(str + i) == '\\') {
			/* a path component delimiter */
			*(str + j++) = '/';

			/* skip repeating delimiters */
			i++;
			while (*(str + i) == '/' || *(str + i) == '\\') {
				i++;
			}
		}
		else if (*(str + i) == '.' &&
			(*(str + i + 1) == '\0' || *(str + i + 1) == '/' || *(str + i + 1) == '\\')) {
			/* skip a '.' component */
			i++;
			while (*(str + i) == '/' || *(str + i) == '\\') {
				i++;
			}
		}
		else if (*(str + i) == '.' && *(str + i + 1) == '.' &&
			(*(str + i + 2) == '\0' || *(str + i + 2) == '/' || *(str + i + 2) == '\\')) {
			/* skip a '..' component */
			i += 2;
			while (*(str + i) == '/' || *(str + i) == '\\') {
				i++;
			}

			/* go back to the previous '/' */
			if (j > 1) {
				while (--j) {
					if (*(str + j - 1) == '/') {
						break;
					}
				}
			}
		}
		else {
			/* copy one path component */
			while (*(str + i) != '\0' && *(str + i) != '/' && *(str + i) != '\\') {
				*(str + j++) = *(str + i++);
			}
		}
	}

	/* remove trailing '/' or '\\' */
	if (j > 1 && *(str + j - 1) == '/') {
		j--;
	}

	/* terminated the result path */
	*(str + j) = '\0';

	/* return result len */
	return j;
}

/*
	format a 64 bit file time value into a printable string
*/
static const char *format_filetime(
	uint64_t value)
{
	static char buf[30];
	time_t t;
	struct tm *l;

	t = (time_t)FtimeToTime(value, NULL);

	l = localtime(&t);

	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
		l->tm_year + 1900, l->tm_mon + 1, l->tm_mday,
		l->tm_hour, l->tm_min, l->tm_sec);

	return buf;
}

/*
	format a 64 bit file size value into a comma separated string
*/
static const char *format_filesize(
	uint64_t value)
{
	static char buf[30] = { 0 };
	uint32_t divisor;
	uint32_t remainder;
	int place;
	char *p;

	p = &buf[29];

	divisor = 10;
	place = 0;

	for (;;) {
		/* r = v % d; v = v / d */
		U64DIV32(value, remainder, value, divisor);

		*(--p) = (char)('0' + remainder);

		if (U64HI(value) == 0 && U64LO(value) == 0) {
			return p;
		}

		if (++place == 3) {
			*(--p) = ',';
			place = 0;
		}
	}
}

/*
	convert a local filename into a fullpath
*/
static void local_fullpath(
	char *dst,
	const char *src)
{
	if (!src || !*src) {
		/* no file specified - use cwd */
		getcwd(dst, VMSHF_MAX_PATH);
	}
	else if (*src == '/' || *src == '\\'
#if defined(_WIN32) || defined(_MSDOS)
		|| (isalpha(*src) && *(src + 1) == ':')
#endif
	) {
		/* full path is specified */
		strcpy(dst, src);
	}
	else {
		/* combine the cwd with specified path */
		char cwd[VMSHF_MAX_PATH];
		getcwd(cwd, sizeof(cwd));
		sprintf(dst, "%s" PATH_DELIMIT "%s", cwd, src);
	}

	normalize_path(dst);
}

/*
	convert a remote filename into a fullpath
*/
static void remote_fullpath(
	char *dst,
	const char *src)
{
	if (!src || !*src) {
		/* no destination  -- use cwd */
		strcpy(dst, remote_cwd);
	}
	else if (remote_cwd[0] == '\0' || *src == '/' || *src == '\\') {
		/* cwd is root or full path is specified */
		while (*src == '/' || *src == '\\') {
			/* skip leading delimiters */
			src++;
		}
		strcpy(dst, src);
	}
	else {
		/* combine remote cwd with specified path */
		sprintf(dst, "%s/%s", remote_cwd, src);
	}

	normalize_path(dst);
}

/*
	create a safe path string
	(remove blanks,  special characters, etc)
*/
static void safe_fullpath(
	char	*path,
	int		outward)
{
	char	*p = path;

	if (outward) {
		/* create a safe path for the host 	*/
		/* replace special characters with '_' */

		while (*p) {
			if ((*p > 0 && *p < ' ') || *p == '\"' || *p == '*' || *p ==':' ||
				*p == '<' || *p == '>' || *p == '?' || *p == '|') {
				/* any other characters to avoid ? */
				*(path++) = '_';
				p++;
			}
			else {
				*(path++) = *(p++);
			}
		}
		*path = '\0';
	}
	else {
		/* create a safe path for the guest	*/

#if defined(_WIN32) || defined(_MSDOS)
		/* leave leading "x:\" as it is */
		if (isalpha(*p) && *(p + 1) == ':' &&
			(*(p + 2) == '/' || *(p + 2) == '\\')) {
			path += 3;
			p = path;
		}
#endif

#ifdef _MSDOS
		/* convert each path component into 8.3 format */
		{
			char name[12];
			int nam_len = 0, ext_len = -1;

			for (;;) {
				if (*p == '\0' || *p == '/' || *p == '\\') {
					/* path delimiter or end of path */
					/* -- store current component and reset counters */

					if (nam_len > 0) {
						memcpy(path, name, nam_len);
						path += nam_len;
					}

					if (ext_len > 0) {
						*(path++) = '.';
						memcpy(path, &name[nam_len], ext_len);
						path += ext_len;
					}

					nam_len = 0;
					ext_len = -1;

					if (*p) {
						*(path++) = '\\';
					}
					else {
						break;
					}
				}
				else if (*p == '.') {
					/* period -- start extension */
					if (ext_len > 0) {
						/* more than one period is present */
						if ((nam_len += ext_len) > 8) {
							nam_len = 8;
						}
					}
					ext_len = 0;
				}
				else if ((*p > 0 && *p <= ' ') || *p == '\"' || *p == '*' ||
					*p == '+' || *p == ',' || *p == ':' || *p == ';' ||
					*p == '<' || *p == '=' || *p == '>' || *p == '?' ||
					*p == '[' || *p == ']' || *p == '|') {
					/* replace special characters with '_' */
					/* any other characters to avoid ? */

					if (ext_len >= 0) {			/* extension present */
						if (ext_len < 3) {		/* less than 3 chars */
							name[nam_len + ext_len++] = '_';
						}
					}
					else {
						if (nam_len < 8) {		/* name less than 8 chars */
							name[nam_len++] = '_';
						}
					}
				}
				else {
					/* normal characters */

					if (ext_len >= 0) {			/* extension present */
						if (ext_len < 3) {		/* less than 3 chars */
							name[nam_len + ext_len++] = toupper(*p);
						}
					}
					else {
						if (nam_len < 8) {		/* name less than 8 chars */
							name[nam_len++] = toupper(*p);
						}
					}
				}

				p++;
			}
		}
#else
		while (*p) {
			if ((*p > 0 && *p < ' ') || *p == '\"' || *p == '*' || *p ==':' ||
				*p == '<' || *p == '>' || *p == '?' || *p == '|') {
				/* any other characters to avoid ? */
				*(path++) = '_';
				p++;
			}
			else {
				*(path++) = *(p++);
			}
		}
#endif

		*path = '\0';
	}
}

/*
	send file/dir to the host
*/
static void vmftp_put(
	const shf_t *shf,
	const char	*local,
	const char	*remote)
{
	char		local_path[VMSHF_MAX_PATH];
	char		remote_path[VMSHF_MAX_PATH];
	char		pattern[VMSHF_MAX_PATH] = {0};
	struct stat	st;
	uint64_t	total;
	time_t		before, after;
	int			complete;

	if (!local || !*local) {
		printf("Invalid Parameter.\n");
		return;
	}

	/* get full paths of files */

	local_fullpath(local_path, local);

	if (strpbrk(local_path, "*?")) {
		/* wild card character present */

		char *p = local_path + strlen(local_path);

		while (p > local_path &&
			*(p - 1) != '/' &&
			*(p - 1) != '\\') {
			p--;
		}

		if (p > local_path) {
			*(p - 1) = '\0';
		}

		strcpy(pattern, p);
	}
	else if (remote == NULL || *remote == '\0') {
		/* if wild card is not present and remote is not specified,
			use local name (last part only) */
		remote = local_path + strlen(local_path);

		while (remote > local_path &&
			*(remote - 1) != '/' &&
			*(remote - 1) != '\\') {
			remote--;
		}
	}

	remote_fullpath(remote_path, remote);
	safe_fullpath(remote_path, 1);

	if (vmshf_debug) {
		fprintf(stderr, "local file : %s\n", local_path);
		fprintf(stderr, "remote file: %s\n", remote_path);
		if (pattern[0]) {
			fprintf(stderr, "pattern    : %s\n", pattern);
		}
	}

	/* get local file info */

	if (stat(local_path, &st) != 0) {
		printf("local (guest): %s: stat() %s\n",
			local_path, strerror(errno));
		return;
	}

	U64INIT(total, 0, 0);

	if (st.st_mode & S_IFDIR) {
		uint32_t files = 0;

		if (!pattern[0]) {
			for (;;) {
				int c;

				printf("local (guest): %s: Is a directory.\n"
					"Put all files and subdirectories ? ", local_path);
				fflush(stdout);

				fflush(stdin);
				c = getchar();
				fflush(stdin);

				if (c == 'n' || c == 'N') {
					return;
				}
				else if (c == 'y' || c == 'Y') {
					break;
				}
			}
		}

		before = time(NULL);
		capture_signal();

		complete = vmftp_putdir(
			shf, local_path, remote_path, &files, &total, pattern[0] ? pattern : NULL);

		ignore_signal();
		after = time(NULL);

		if (complete && pattern[0] && files == 0) {
			printf("No matching file.\n");
			complete = 0;
		}
	}
	else {
		printf("%s ==> %s\n", local, remote);

		before = time(NULL);
		capture_signal();

		complete = vmftp_putfile(
			shf, local_path, remote_path, &total, st.st_mtime);

		ignore_signal();
		after = time(NULL);
	}

	if (complete) {
		printf("Transfered %s bytes in %lu seconds\n",
			format_filesize(total), after - before);
	}
}

/*
	send a file to the host
*/
static int vmftp_putfile(
	const shf_t *shf,
	const char	*local,
	const char	*remote,
	uint64_t	*total,
	time_t		modtime)
{
	FILE		*fp;
	uint32_t	status;
	uint32_t	handle;
	uint64_t	ofs;
	uint32_t	len;
	int			ret;
	int			complete;
	int			hash_mark;

	fp = fopen(local, FOPEN_READ_MODE);

	if (fp == NULL) {
		printf("local (guest): %s: fopen() %s\n",
			local, strerror(errno));
		return 0;
	}

	/* open remote file */

	ret = VMShfOpenFile(shf,
		VMSHF_ACCESS_READWRITE,
		VMSHF_OPENMODE_C_ALWAYS,
		VMSHF_FILEMODE_READ | VMSHF_FILEMODE_WRITE,
		remote, &status, &handle);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: OpenFile (%ld) %s\n",
			remote, status, VMShfStatusName(status));
		fclose(fp);
		return 0;
	}

	/* transfer file data */

	U64INIT(ofs, 0, 0);
	complete = 0;
	hash_mark = 0;

	while (!vmftp_aborted) {
		len = fread(WRITE_BUF(shf), 1, VMSHF_BLOCK_SIZE, fp);

		if (len == 0) {
			if (feof(fp)) {
				complete = 1;
			}
			else {
				printf("local (guest): %s: fread() %s\n",
					local, strerror(errno));
			}
			break;
		}

		ret = VMShfWriteFile(shf, handle, 0, ofs, &len, &status);

		if (ret != VMTOOL_SUCCESS || status != 0) {
			printf("remote(host): %s: WriteFile (%ld) %s\n",
				remote, status, VMShfStatusName(status));
			break;
		}

		U64ADD32(ofs, ofs, len);	/* ofs = ofs + len */

		if ((U64LO(ofs) & 0xfffff) == 0) {
			printf("#");
			fflush(stdout);
			hash_mark = 1;
		}
	}

	if (hash_mark) {
		printf("\n");
	}

	if (vmftp_aborted) {
		printf("Transfer aborted by user.\n");
	}

	fclose(fp);

	VMShfCloseFile(shf, handle, &status);

	if (complete) {
		uint64_t tmp;

		tmp = *total;
		U64ADD64(tmp, tmp, ofs);
		*total = tmp;

		/* Transfer succeeded -- update last modification time */

		SETA_BUF(shf)->utime = TimeToFtime(modtime, 0);

		ret = VMShfSetAttr(shf, VMSHF_ATTRMASK_UTIME,
			0, NULL, remote, &status);

		if (ret != VMTOOL_SUCCESS || status != 0) {
			printf("remote(host): %s: SetAttr (%ld) %s\n",
				remote, status, VMShfStatusName(status));
		}
	}

	return complete;
}

/*
	send contents of a directory to the host
*/
static int vmftp_putdir(
	const shf_t *shf,
	char		*local,
	char		*remote,
	uint32_t	*files,
	uint64_t	*bytes,
	const char	*pattern)
{
	dir_t		handle;
	char		*remote_name;
	char		*local_name;
	int			dirflag;
	time_t		modtime;
	int			ret;

	if (!pattern) {
		/* directory recursive put */
		uint32_t	status;
		fileattr_t	*fileattr;
		uint32_t	isdir;

		/* print target directory name */
		printf("Directory: %s => %s\n", local, remote);

		/* create a remote directory */

		ret = VMShfGetAttr(shf, remote, &status, &isdir, &fileattr);

		if (ret != VMTOOL_SUCCESS) {
			printf("remote (host): %s: GetAttr RPC error\n", remote);
			return 0;
		}
		else {
			if (status == VMSHF_ENOTEXIST) {
				/* create a new directory */
				ret = VMShfCreateDir(shf,
					VMSHF_FILEMODE_READ | VMSHF_FILEMODE_WRITE,
					remote, &status);

				if (ret != VMTOOL_SUCCESS || status != 0) {
					printf("remote (host): %s: CreateDir (%ld) %s\n",
						remote, status, VMShfStatusName(status));
					return 0;
				}
			}
			else if (status == 0) {
				if (!isdir) {
					printf("remote (host): %s: Not a directory\n", remote);
					return 0;
				}
			}
			else {
				printf("remote (host): %s: GetAttr (%ld) %s\n",
					remote, status, VMShfStatusName(status));
			}
		}
	}

	remote_name = remote + strlen(remote);
	local_name = local + strlen(local);

	/* prepare local file list */
	get_dirent_first(&handle, local, local_name + 1, &dirflag, &modtime);

	do {
		if (!*(local_name + 1)) {
			break;
		}

		if (dirflag) {
			/* first, process regular files only */
			continue;
		}

#if defined(_WIN32) || defined(_MSDOS)
#define CASE_SENSITIVE 0
#else
#define CASE_SENSITIVE 1
#endif

		if (pattern && !pattern_match(local_name + 1, pattern, CASE_SENSITIVE)) {
			/* filename doesn't match */
			continue;
		}

		*local_name = '/';

		strcpy(remote_name, local_name);
		safe_fullpath(remote_name, 1);

		printf("%s ==> %s\n", local_name + 1, remote_name + 1);

		ret = vmftp_putfile(shf, local, remote, bytes, modtime);

		*remote_name = '\0';
		*local_name = '\0';

		if (!ret) {
			close_dir(&handle);
			return 0;
		}

		(*files)++;
	}
	while (get_dirent(&handle, local_name + 1, &dirflag, &modtime) == 0);

	/* close enum handle */

	close_dir(&handle);

	if (pattern) {
		/* file pattern put */
		return 1;
	}

	/* prepare local file list again */

	get_dirent_first(&handle, local, local_name + 1, &dirflag, &modtime);

	do {
		if (!*(local_name + 1)) {
			break;
		}

		if (!dirflag ||
			strcmp(local_name + 1, ".") == 0 ||
			strcmp(local_name + 1, "..") == 0) {
			/* now, process subdirectories only */
			continue;
		}

		*local_name = '/';

		strcpy(remote_name, local_name);
		safe_fullpath(remote_name, 1);

		ret = vmftp_putdir(shf, local, remote, files, bytes, pattern);

		*remote_name = '\0';
		*local_name = '\0';

		if (!ret) {
			close_dir(&handle);
			return 0;
		}
	}
	while (get_dirent(&handle, local_name + 1, &dirflag, &modtime) == 0);

	/* cleanup enum handle */

	close_dir(&handle);

	return 1;
}

/*
	get file/dir from the host
*/
static void vmftp_get(
	const shf_t *shf,
	const char	*local,
	const char	*remote)
{
	char		remote_path[VMSHF_MAX_PATH];
	char		local_path[VMSHF_MAX_PATH];
	char		pattern[VMSHF_MAX_PATH] = {0};
	uint32_t	status;
	int			ret;
	uint32_t	dirflag;
	fileattr_t	*fileattr;
	time_t		before, after;
	int			complete;
	uint64_t	total;

	if (!remote || !*remote) {
		printf("Invalid Parameter.\n");
		return;
	}

	/* get remote fullpath */

	remote_fullpath(remote_path, remote);

	if (strpbrk(remote_path, "*?")) {
		/* wild card character present */

		char *p = remote_path + strlen(remote_path);

		while (p > remote_path &&
			*(p - 1) != '/' &&
			*(p - 1) != '\\') {
			p--;
		}

		if (p > remote_path) {
			*(p - 1) = '\0';
		}

		strcpy(pattern, p);
	}
	else if (local == NULL || *local == '\0') {
		/* if wild card is not present and local file is not specified,
		use remote file name */
		local = remote_path + strlen(remote_path);

		while (local > remote_path &&
			*(local - 1) != '/' &&
			*(local - 1) != '\\') {
			local--;
		}
	}

	local_fullpath(local_path, local);
	safe_fullpath(local_path, 0);

	if (vmshf_debug) {
		fprintf(stderr, "remote file: %s\n", remote_path);
		fprintf(stderr, "local file : %s\n", local_path);
		if (pattern[0]) {
			fprintf(stderr, "pattern    : %s\n", pattern);
		}
	}

	/* get remote file info */

	ret = VMShfGetAttr(shf, remote_path, &status, &dirflag, &fileattr);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: Error (%ld) %s\n",
			remote_path, status, VMShfStatusName(status));
		return;
	}

	U64INIT(total, 0, 0);

	if (dirflag) {
		uint32_t files = 0;

		if (!pattern[0]) {
			for (;;) {
				int c;

				printf("remote (host): %s: Is a directory.\n"
					"Get all files and subdirectories ? ", remote_path);
				fflush(stdout);

				fflush(stdin);
				c = getchar();
				fflush(stdin);

				if (c == 'n' || c == 'N') {
					return;
				}
				else if (c == 'y' || c == 'Y') {
					break;
				}
			}
		}

		before = time(NULL);
		capture_signal();

		complete = vmftp_getdir(
			shf, local_path, remote_path, &files, &total, pattern[0] ? pattern : NULL);

		ignore_signal();
		after = time(NULL);

		if (complete && pattern[0] && files == 0) {
			printf("No matching file\n");
			complete = 0;
		}
	}
	else {
		printf("%s <== %s\n", local, remote);

		before = time(NULL);
		capture_signal();

		complete = vmftp_getfile(
			shf, local_path, remote_path, &total,
			FtimeToTime(fileattr->utime, NULL));

		ignore_signal();
		after = time(NULL);
	}

	if (complete) {
		printf("Transfered %s bytes in %lu seconds\n",
			format_filesize(total), after - before);
	}
}

/*
	get a file from the host
*/
static int vmftp_getfile(
	const shf_t *shf,
	const char	*local,
	const char	*remote,
	uint64_t	*total,
	time_t		modtime)
{
	uint32_t	handle;
	uint32_t	status;
	FILE		*fp;
	uint64_t	ofs;
	uint32_t	len;
	int			complete;
	int			ret;
	int			hash_mark;

	/* open remote file */

	ret = VMShfOpenFile(shf,
		VMSHF_ACCESS_READONLY,
		VMSHF_OPENMODE_O_EXIST,
		VMSHF_FILEMODE_READ | VMSHF_FILEMODE_WRITE,
		remote, &status, &handle);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: OpenFile (%ld) %s\n",
			remote, status, VMShfStatusName(status));
		return 0;
	}

	/* open local file */

	fp = fopen(local, FOPEN_WRITE_MODE);

	if (fp == NULL) {
		printf("local (guest): %s: %s\n",
			local, strerror(errno));

		VMShfCloseFile(shf, handle, &status);
		return 0;
	}
	/* transfer file data */

	U64INIT(ofs, 0, 0);
	complete = 0;
	hash_mark = 0;

	while (!vmftp_aborted) {
		len = VMSHF_BLOCK_SIZE;

		ret = VMShfReadFile(shf, handle, ofs, &len, &status);

		if (ret != VMTOOL_SUCCESS || status != 0) {
			printf("remote (host): %s: ReadFile (%ld) %s\n",
				remote, status, VMShfStatusName(status));
			break;
		}

		U64ADD32(ofs, ofs, len);	/* ofs = ofs + len */

		if (fwrite(READ_BUF(shf), 1, len, fp) < len) {
			printf("local (guest): %s: fwrite() %s\n",
				local, strerror(errno));
			break;
		}

		if (len < VMSHF_BLOCK_SIZE) {
			complete = 1;
			break;
		}

		if ((U64LO(ofs) & 0xfffff) == 0) {
			printf("#");
			fflush(stdout);
			hash_mark = 1;
		}
	}

	if (hash_mark) {
		printf("\n");
	}

	if (vmftp_aborted) {
		printf("Transfer aborted by user.\n");
	}

	fclose(fp);

	VMShfCloseFile(shf, handle, &status);

	if (complete) {
		uint64_t tmp;
		struct utimbuf times;

		tmp = *total;
		U64ADD64(tmp, tmp, ofs);
		*total = tmp;

		/* set local file last mod time */
		times.modtime = modtime;
		times.actime = time(NULL);

		if (utime(local, &times) != 0) {
			printf("local (guest): %s: utime() %s\n",
				local, strerror(errno));
		}
	}

	return complete;
}

/*
	get contents of a directory from the host
*/
static int vmftp_getdir(
	const shf_t *shf,
	char		*local,
	char		*remote,
	uint32_t	*files,
	uint64_t	*total,
	const char	*pattern)
{
	uint32_t	handle;
	uint32_t	status;
	uint32_t	index;
	uint32_t	dirflag;
	int			ret;
	int			complete;
	char		*remote_name;
	char		*local_name;
	fileattr_t	*fileattr;
	char		*name;

	if (!pattern) {
		/* print target directory name */

		printf("Directory: %s <= %s\n", local, remote);

		/* create a local directory */

		if (create_dir(local) != 0) {
			if (errno == EEXIST) {
				struct stat st;

				if (stat(local, &st) != 0) {
					printf("local (guest): %s: stat() %s\n",
						local, strerror(errno));
					return 0;
				}
				if (!(st.st_mode & S_IFDIR)) {
					printf("local (guest): %s: Not a directory\n", local);
					return 0;
				}
			}
			else {
				printf("local (guest): %s: mkdir() %s\n",
					local, strerror(errno));
				return 0;
			}
		}
	}

	remote_name = remote + strlen(remote);
	local_name = local + strlen(local);

	/* prepare remote file list */

	ret = VMShfOpenDir(shf, remote, &status, &handle);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: OpenDir (%ld) %s\n",
			remote, status, VMShfStatusName(status));
		return 0;
	}

	index = 0;
	complete = 0;

	for (;;) {
		ret = VMShfReadDir(shf, handle, index++,
			&status, &dirflag, &fileattr, &name);

		if (ret != VMTOOL_SUCCESS || status != 0) {
			printf("remote (host): %s: ReadDir (%ld) %s\n",
				remote, status, VMShfStatusName(status));
			VMShfCloseDir(shf, handle, &status);
			return 0;
		}

		if (name == NULL) {
			/* end of list */
			break;
		}

		if (dirflag) {
			/* first, process regular files only */
			continue;
		}

		if (pattern && !pattern_match(name, pattern, 1)) {
			/* file name doesn't match */
			continue;
		}

		sprintf(remote_name, "/%s", name);

		strcpy(local_name, remote_name);
		safe_fullpath(local_name, 0);

		printf("%s <== %s\n", local_name + 1, remote_name + 1);

		if (!vmftp_getfile(shf, local, remote, total,
			FtimeToTime(fileattr->utime, NULL))) {
			VMShfCloseDir(shf, handle, &status);
			return 0;
		}

		(*files)++;
		*remote_name = '\0';
		*local_name = '\0';
	}

	if (pattern) {
		complete = 1;
	}
	else {
		index = 0;

		for (;;) {
			ret = VMShfReadDir(shf, handle, index++,
				&status, &dirflag, &fileattr, &name);

			if (ret != VMTOOL_SUCCESS || status != 0) {
				printf("remote (host): %s: ReadDir (%ld) %s\n",
					remote, status, VMShfStatusName(status));
				break;
			}

			if (name == NULL) {
				/* end of list */
				complete = 1;
				break;
			}

			if (!dirflag ||
				strcmp(name, ".") == 0 ||
				strcmp(name, "..") == 0) {
				/* now, process directories only */
				continue;
			}

			sprintf(remote_name, "/%s", name);

			strcpy(local_name, remote_name);
			safe_fullpath(local_name, 0);

			if (!vmftp_getdir(shf, local, remote, files, total, pattern)) {
				break;
			}

			*remote_name = '\0';
			*local_name = '\0';
		}
	}

	VMShfCloseDir(shf, handle, &status);

	return complete;
}

/*
	list remote directory contents
*/
static void vmftp_ls(
	const shf_t *shf,
	const char	*dirname)
{
	char path[VMSHF_MAX_PATH];
	uint32_t status;
	uint32_t handle;
	uint32_t index;
	uint32_t dirflag;
	fileattr_t *fileattr;
	int ret;

	/* check if the param is a directory */
	remote_fullpath(path, dirname);

	ret = VMShfGetAttr(shf, path, &status, &dirflag, &fileattr);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: GetAttr (%ld) %s\n",
			path, status, VMShfStatusName(status));
		return;
	}

	if (dirflag == 0) {

		/* not a directory - print file information */

		printf("-%c%c%c (%02x) %15s %s %s\n",
			fileattr->fmode & VMSHF_FILEMODE_READ ? 'r' : '-',
			fileattr->fmode & VMSHF_FILEMODE_WRITE ? 'w' : '-',
			fileattr->fmode & VMSHF_FILEMODE_EXEC ? 'x' : '-',
			fileattr->fmode,
			format_filesize(fileattr->fsize),
			format_filetime(fileattr->utime),
			dirname);

		return;
	}

	/* prepare remote file list */

	handle = (uint32_t)-1;

	ret = VMShfOpenDir(shf, path, &status, &handle);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: OpenDir (%ld) %s\n",
			dirname, status, VMShfStatusName(status));
		return;
	}

	/* get file list */
	capture_signal();

	index = 0;

	while (!vmftp_aborted) {
		char *name;

		ret = VMShfReadDir(shf, handle, index,
			&status, &dirflag, &fileattr, &name);

		if (ret != VMTOOL_SUCCESS || status != 0) {
			printf("remote (host): %s: ReadDir (%ld) %s\n",
				dirname, status, VMShfStatusName(status));
			break;
		}

		if (name == NULL) {
			/* end of list */
			break;
		}

		index++;

		printf("%c%c%c%c (%02x) %15s %s %s\n",
			dirflag ? 'd' : '-',
			fileattr->fmode & VMSHF_FILEMODE_READ ? 'r' : '-',
			fileattr->fmode & VMSHF_FILEMODE_WRITE ? 'w' : '-',
			fileattr->fmode & VMSHF_FILEMODE_EXEC ? 'x' : '-',
			fileattr->fmode,
			format_filesize(fileattr->fsize),
			format_filetime(fileattr->utime),
			name);
	}

	ignore_signal();

	if (vmftp_aborted) {
		printf("Operation aborted by user.\n");
	}

	/* cleanup handles */

	if (handle != (uint32_t)-1) {
		VMShfCloseDir(shf, handle, &status);
	}

	return;
}

/*
	change remote cwd
*/
static void vmftp_cd(
	const shf_t *shf,
	const char *dirname)
{
	char path[VMSHF_MAX_PATH];
	uint32_t status;
	uint32_t dirflag;
	fileattr_t *fileattr;
	int ret;

	/* cd to root ? */

	if (!dirname || !*dirname) {
		remote_cwd[0] = '\0';

		printf("remote (host) directory now /\n");

		return;
	}

	/* check new remote directory */

	remote_fullpath(path, dirname);

	ret = VMShfGetAttr(shf, path, &status, &dirflag, &fileattr);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: GetAttr (%ld) %s\n",
			dirname, status, VMShfStatusName(status));
		return;
	}

	if (dirflag == 0) {
		printf("remote (host): %s: Not a directory\n", dirname);
		return;
	}

	strcpy(remote_cwd, path);
	printf("remote (host) directory now /%s\n", remote_cwd);

	return;
}

/*
	change local cwd
*/
static void vmftp_lcd(const char *dirname)
{
	char path[VMSHF_MAX_PATH];

	if (!dirname || !*dirname) {
		dirname = getenv("HOME");
	}

	if (dirname && *dirname) {
#ifdef _MSDOS
		/* drive must be changed explicitly */
		if (isalpha(*dirname) && *(dirname + 1) == ':') {
			if (_chdrive(toupper(*dirname) - 'A' + 1) != 0) {
				printf("local (guest): %s: _chdrive() %s\n",
					dirname, strerror(errno));
				return;
			}
		}
#endif
		if (chdir(dirname) != 0) {
			printf("local (guest): %s: chdir() %s\n",
				dirname, strerror(errno));
			return;
		}
	}

	printf("local (guest) directory now %s\n",
		getcwd(path, sizeof(path)));

	return;
}

/*
	change remote file mode
*/
static void vmftp_chmod(
	const shf_t *shf,
	const char *mode,
	const char *file)
{
	char path[VMSHF_MAX_PATH];
	uint32_t status;
	uint32_t dirflag;
	fileattr_t *fileattr;
	int ret;
	unsigned char fmode, add_mask = 0, sub_mask = 0;

	if (!mode || !*mode || !file || !*file) {
		printf("Invalid Parameter\n");
		return;
	}

	if (strcmp(mode, "+r") == 0) {
		add_mask = VMSHF_FILEMODE_READ;
	}
	else if (strcmp(mode, "-r") == 0) {
		sub_mask = VMSHF_FILEMODE_READ;
	}
	else if (strcmp(mode, "+w") == 0) {
		add_mask = VMSHF_FILEMODE_WRITE;
	}
	else if (strcmp(mode, "-w") == 0) {
		sub_mask = VMSHF_FILEMODE_WRITE;
	}
	else if (strcmp(mode, "+x") == 0) {
		add_mask = VMSHF_FILEMODE_EXEC;
	}
	else if (strcmp(mode, "-x") == 0) {
		sub_mask = VMSHF_FILEMODE_EXEC;
	}
	else {
		printf("Invalid Parameter\n");
		return;
	}

	/* get remote file info */

	remote_fullpath(path, file);

	ret = VMShfGetAttr(shf, path, &status, &dirflag, &fileattr);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: GetAttr (%ld) %s\n",
			file, status, VMShfStatusName(status));
		return;
	}

	fmode = GETA_BUF(shf)->fmode;
	fmode |= add_mask;
	fmode &= ~sub_mask;
	fmode &= VMSHF_FILEMODE_MASK;
	SETA_BUF(shf)->fmode = fmode;

	/* set remote file info */

	ret = VMShfSetAttr(shf, VMSHF_ATTRMASK_FMODE, 0, NULL, path, &status);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: SetAttr (%ld) %s\n",
			file, status, VMShfStatusName(status));
		return;
	}

	printf("OK\n");
}

/*
	create remote directory
*/
static void vmftp_mkdir(
	const shf_t *shf,
	const char *dirname)
{
	char path[VMSHF_MAX_PATH];
	uint32_t status;
	int ret;

	if (!dirname || !*dirname) {
		printf("Invalid Parameter.\n");
		return;
	}

	remote_fullpath(path, dirname);

	ret = VMShfCreateDir(shf, VMSHF_FILEMODE_MASK, path, &status);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: CreateDir (%ld) %s\n",
			dirname, status, VMShfStatusName(status));
		return;
	}

	printf("OK\n");
}

/*
	delete remote file
*/
static void vmftp_delete(
	const shf_t *shf,
	const char *filename)
{
	char path[VMSHF_MAX_PATH];
	uint32_t status;
	int ret;

	if (!filename || !*filename) {
		printf("Invalid Parameter.\n");
		return;
	}

	remote_fullpath(path, filename);

	ret = VMShfDeleteFile(shf, path, &status);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: DeleteFile (%ld) %s\n",
			filename, status, VMShfStatusName(status));
		return;
	}

	printf("OK\n");
}

/*
	rename remote file
*/
static void vmftp_rename(
	const shf_t *shf,
	const char *oldname,
	const char *newname)
{
	char oldpath[1024], newpath[1024];
	uint32_t status;
	int ret;

	if (!oldname || !*oldname || !newname || !*newname) {
		printf("Invalid Parameter.\n");
		return;
	}

	remote_fullpath(oldpath, oldname);
	remote_fullpath(newpath, newname);

	ret = VMShfMoveFile(shf, oldpath, newpath, &status);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s, %s: MoveFile (%ld) %s\n",
			oldname, newname, status, VMShfStatusName(status));
		return;
	}

	printf("OK\n");
}

/*
	delete remote directory
*/
static void vmftp_rmdir(
	const shf_t *shf,
	const char *dirname)
{
	char path[VMSHF_MAX_PATH];
	uint32_t status;
	int ret;

	if (!dirname || !*dirname) {
		printf("Invalid Parameter.\n");
		return;
	}

	remote_fullpath(path, dirname);

	ret = VMShfDeleteDir(shf, path, &status);

	if (ret != VMTOOL_SUCCESS || status != 0) {
		printf("remote (host): %s: DeleteDir (%ld) %s\n",
			dirname, status, VMShfStatusName(status));
		return;
	}

	printf("OK\n");
}

/*
	more system dependent stuff...
*/

/*
	Ctrl+C handling
*/
#if defined(_WIN32)

static BOOL WINAPI signal_handler(DWORD dwCtrlType)
{
#ifdef _MSC_VER
	UNREFERENCED_PARAMETER(dwCtrlType);
#endif
	vmftp_aborted = 1;
	return TRUE;
}

static void ignore_signal()
{
}

static void capture_signal()
{
	vmftp_aborted = 0;
	SetConsoleCtrlHandler(signal_handler, TRUE);
}

#else /* !defined(_WIN32) */
/*
	use ordinary signal handling
*/
static void signal_handler(int sig)
{
	vmftp_aborted = 1;
}

static void ignore_signal()
{
	signal(SIGINT, SIG_IGN);
}

static void capture_signal()
{
	vmftp_aborted = 0;
	signal(SIGINT, signal_handler);
}

#endif /* !defined(_WIN32) */

/*
	directory listing
*/
#if defined(_WIN32)

static int get_dirent_first(
	dir_t		*dir,
	const char	*dirname,
	char 		*name,
	int 		*dirflag,
	time_t 		*modtime)
{
	char path[MAX_PATH];
	struct _finddata_t finddata;

	sprintf(path, "%s/*", dirname);

	*name = '\0';

	*dir = _findfirst(path, &finddata);

	if (*dir == -1) {
		printf("local (guest): %s: findfirst() %s\n",
			dirname, strerror(errno));
		return -1;
	}

	strcpy(name, finddata.name);
	*dirflag = (finddata.attrib & _A_SUBDIR);
	*modtime = finddata.time_write;

	return 0;
}

static int get_dirent(
	dir_t		*dir,
	char 		*name,
	int 		*dirflag,
	time_t		*modtime)
{
	struct _finddata_t finddata;

	*name = '\0';

	if (_findnext(*dir, &finddata) == -1) {
		return -1;
	}

	strcpy(name, finddata.name);
	*dirflag = (finddata.attrib & _A_SUBDIR);
	*modtime = finddata.time_write;

	return 0;
}

static int close_dir(const dir_t *dir)
{
	_findclose(*dir);
	return 0;
}

#elif defined(_MSDOS)

static time_t dostime2time(
	unsigned short date,
	unsigned short time)
{
	struct tm t;

	t.tm_year	= (date >> 8) + 80;
	t.tm_mon	= (date >> 5) & 0x7;
	t.tm_mday	= date & 0x1f;
	t.tm_hour	= (time >> 10);
	t.tm_min	= (time >> 4) & 0x3f;
	t.tm_sec	= (time & 0x1f) << 1;

	return mktime(&t);
}

static int get_dirent_first(
	dir_t 		*dir,
	const char 	*dirname,
	char 		*name,
	int 		*dirflag,
	time_t 		*modtime)
{
	char path[260];
	unsigned ret;

	sprintf(path, "%s/*", dirname);

	*name = '\0';

	ret = _dos_findfirst(path,
		_A_ARCH | _A_HIDDEN | _A_NORMAL | _A_RDONLY | _A_SUBDIR | _A_SYSTEM,
		dir);

	if (ret != 0) {
		printf("local (guest): %s: _dos_findfirst() %s\n",
			dirname, strerror(errno));
		return -1;
	}

	strcpy(name, dir->name);
	*dirflag = (dir->attrib & _A_SUBDIR);
	*modtime = dostime2time(dir->wr_date, dir->wr_time);

	return 0;
}

static int get_dirent(
	dir_t		*dir,
	char 		*name,
	int 		*dirflag,
	time_t 		*modtime)
{
	*name = '\0';

	if (_dos_findnext(dir) != 0) {
		return -1;
	}

	strcpy(name, dir->name);
	*dirflag = (dir->attrib & _A_SUBDIR);
	*modtime = dostime2time(dir->wr_date, dir->wr_time);

	return 0;
}

static int close_dir(const dir_t *dir)
{
	return 0;
}

#else /* !defined(_WIN32) && !defined(_MSDOS) */

static int get_dirent_first(
	dir_t 		*dir,
	const char 	*dirname,
	char 		*name,
	int 		*dirflag,
	time_t 		*modtime)
{
	dir->dir = opendir(dirname);

	if (dir->dir == NULL) {
		printf("local (guest): %s: opendir() %s\n",
			dirname, strerror(errno));
		return -1;
	}

	strcpy(dir->path, dirname);

	return get_dirent(dir, name, dirflag, modtime);
}

static int get_dirent(
	dir_t 		*dir,
	char 		*name,
	int 		*dirflag,
	time_t 		*modtime)
{
	char path[VMSHF_MAX_PATH];
	struct dirent *dent;
	struct stat st;

	*name = '\0';

	dent = readdir(dir->dir);

	if (!dent) {
		return -1;
	}

	sprintf(path, "%s/%s", dir->path, dent->d_name);

	if (stat(path, &st) != 0) {
		printf("local (guest): %s: stat() %s\n",
			path, strerror(errno));
		return -1;
	}

	strcpy(name, dent->d_name);
	*dirflag = (st.st_mode & S_IFDIR);
	*modtime = (st.st_mtime);

	return 0;
}

static int close_dir(const dir_t *dir)
{
	closedir(dir->dir);
	return 0;
}

#endif /* !defined(_WIN32) */

#if defined(_WIN32) || defined(_MSDOS)
#define strncasecmp(a,b,c) strnicmp(a,b,c)
#endif

static int pattern_length(const char *pattern)
{
	int len = 0;

	while (*(pattern + len) &&
		*(pattern + len) != '*' &&
		*(pattern + len) != '?') {
		len++;
	}

	return len;
}

static int pattern_match(const char *name, const char *pattern, int case_sensitive)
{
	int wild = 0;
	int pat_len;

	pat_len = pattern_length(pattern);

	while (*pattern) {
		if (*pattern == '*') {
			wild = 1;
			pat_len = pattern_length(++pattern);
		}
		else if (*pattern == '?') {
			wild = 0;

			if (!*name) {
				break;
			}
			name++;

			pat_len = pattern_length(++pattern);
		}
		else {
			int match;

			if (!*name) {
				break;
			}

			match = 0;

			if (case_sensitive) {
				if (strncmp(name, pattern, pat_len) == 0) {
					match = 1;
				}
			}
			else {
				if (strncasecmp(name, pattern, pat_len) == 0) {
					match = 1;
				}
			}

			if (match) {
				name += pat_len;
				pattern += pat_len;
				pat_len = pattern_length(pattern);
				wild = 0;
			}
			else if (wild) {
				name++;
			}
			else {
				break;
			}
		}
	}

	return ((wild || !*name) && !*pattern);
}
