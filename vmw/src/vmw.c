/*
	vmw.c
	vmw command line tool

	Copyright (c) 2006 Ken Kato
*/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "vmtool.h"
#include "termutil.h"

/*
	program name (basename of argv[0])
*/
static const char *progname = NULL;

/*
	command processing functions
*/
typedef int (*cmdfnc)(char **args);

static int cmd_apm(char **args);
static int cmd_copy(char **args);
static int cmd_cursor(char **args);
static int cmd_device(char **args);
static int cmd_hwver(char **args);
static int cmd_memory(char **args);
static int cmd_mhz(char **args);
static int cmd_option(char **args);
static int cmd_paste(char **args);
static int cmd_rpc(char **args);
static int cmd_screen(char **args);
static int cmd_time(char **args);
static int cmd_uuid(char **args);
static int cmd_version(char **args);

static const struct {
	const char *cmd;
	cmdfnc func;
}
commands[] = {
	{ "apm",	cmd_apm		},
	{ "copy",	cmd_copy	},
	{ "cursor",	cmd_cursor	},
	{ "device",	cmd_device	},
	{ "hwver",	cmd_hwver	},
	{ "memory",	cmd_memory	},
	{ "mhz",	cmd_mhz		},
	{ "option",	cmd_option	},
	{ "paste",	cmd_paste	},
	{ "rpc",	cmd_rpc		},
	{ "screen",	cmd_screen	},
	{ "time",	cmd_time	},
	{ "uuid",	cmd_uuid	},
	{ "version",cmd_version },
	{ NULL, NULL }
};

static int parse_command(const char *cmd);

/*
	system dependent time utilities functions
*/
static void sleep_millisec(uint32_t ms);
static uint32_t get_systemtime(uint32_t *micro);
static int set_systemtime(uint32_t time, uint32_t micro);

int main(int argc, char **argv)
{
	static const char *usage =
		"Usage: %s <command>[-q|-c|-v][-d][-h][command specific options]\n"
		"\n"
		"<command>\n"
		"  a[pm]     execute an APM function\n"
		"  co[py]    copy text into clipboard\n"
		"  cu[rsor]  get/set mouse cursor location\n"
		"  d[evice]  get/set device information/state\n"
		"  h[wver]   get virtual hardware version\n"
		"  me[mory]  get memory size (MB)\n"
		"  mh[z]     get processor speed (MHz)\n"
		"  o[ption]  get/set option settings\n"
		"  p[aste]   paste text from clipboard\n"
		"  r[pc]     call guest-host RPC\n"
		"  s[creen]  get host screen size\n"
		"  t[ime]    get host system time / sync guest system time\n"
		"  u[uid]    get BIOS UUID\n"
		"  v[ersion] get VMware version\n"
		"\n"
		"common options\n"
		"  -q  quiet (no text output)\n"
		"  -c  curt (minimum text output: default)\n"
		"  -v  verbose (maximum text output)\n"
		"  -d  dump (dump register values on each backdoor call)\n"
		"  -h  print help message\n"
		"\n"
		"Type %s <command> -h for command specific options.\n"
		;

	int cmd;
	int i;

	/* get program name	*/
	progname = *argv + strlen(*argv);

	while (progname > *argv &&
		*(progname - 1) != '/' &&
		*(progname - 1) != '\\') {
		progname--;
	}

	/* at least <command> is requied */
	if (argc < 2) {
		fprintf(stderr, usage, progname, progname);
		return VMTOOL_STX_ERROR;
	}

	/* parse <command> argument */
	cmd = parse_command(*(argv + 1));

	if (cmd < 0) {
		return VMTOOL_STX_ERROR;
	}

	/* process common options */
	i = 2;

	while (i < argc) {
		if (!strcmp(*(argv + i), "-q")) {
			vmtool_msg = vmtool_quiet;
			memmove((argv + i), (argv + i + 1), sizeof(char *) * (argc - i));
			argc--;
		}
		else if (!strcmp(*(argv + i), "-c")) {
			vmtool_msg = vmtool_curt;
			memmove((argv + i), (argv + i + 1), sizeof(char *) * (argc - i));
			argc--;
		}
		else if (!strcmp(*(argv + i), "-v")) {
			vmtool_msg = vmtool_verbose;
			memmove((argv + i), (argv + i + 1), sizeof(char *) * (argc - i));
			argc--;
		}
		else if (!strcmp(*(argv + i), "-d")) {
			vmtool_dump = 1;
			memmove((argv + i), (argv + i + 1), sizeof(char *) * (argc - i));
			argc--;
		}
		else {
			i++;
		}
	}

	/* check if running in VMware */
	if (VMCheckVirtual() != VMTOOL_SUCCESS) {
		return VMTOOL_CMD_ERROR;
	}

	/* execute the command */
	return (*commands[cmd].func)(argv + 2);
}

/*
	Parse the command argument
*/
static int parse_command(const char *cmd)
{
#define NO_MATCH		-1
#define MULTIPLE_MATCH	-2

	int len = strlen(cmd);
	int idx = 0;
	int match = NO_MATCH;

	while (commands[idx].cmd) {

		if (!strncmp(cmd, commands[idx].cmd, len)) {

			if (match == NO_MATCH) {			/*	first match */
				match = idx;
			}
			else {								/*	multiple matches */
				if (match != MULTIPLE_MATCH) {	/*	first time */
					fprintf(stderr, "%s is ambiguous\n", cmd);
					fprintf(stderr, "> %s ", commands[match].cmd);
					match = MULTIPLE_MATCH;
				}

				fprintf(stderr, "%s ", commands[idx].cmd);
			}
		}

		idx++;
	}

	if (match == NO_MATCH) {					/*	match not found */
		fprintf(stderr, "unknown command %s\n", cmd);
	}
	else if (match == MULTIPLE_MATCH) {			/*	multiple matches */
		fprintf(stderr, "\n");
	}

	return match;
}

static int cmd_apm(char **args)
{
	static const char *usage =
		"Usage: %s apm {-p|-s|<value>}\n"
		"  -p      Power Off\n"
		"  -s      Suspend\n"
		"  <value> Immediate 32 bit hex value (hi:apm func lo:param)\n"
		;

	uint32_t func = 0;

	while (*args) {
		if (!strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (!strcmp(*args, "-p")) {
			func = VMAPM_POWEROFF;
		}
		else if (!strcmp(*args, "-s")) {
			func = VMAPM_SUSPEND;
		}
		else if (func || sscanf(*args, "%x", &func) <= 0) {
			fprintf(stderr, usage, progname);
			return VMTOOL_STX_ERROR;
		}
		args++;
	}

	if (func == 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, usage, progname);
		}
		return VMTOOL_STX_ERROR;
	}

	if (vmtool_msg >= vmtool_verbose) {
		fprintf(stderr, "APM func: %04Xh param: %04Xh\n",
			(uint16_t)(func & 0xffff),
			(uint16_t)(func >> 16));
	}

	return VMApmFunction(func);
}

static int cmd_copy(char **args)
{
#define COPY_BUF_SIZE	0xff00

	static const char *usage =
		"Usage: %s copy [<file>|-t<text>]\n"
		"  <file>   file to read text from\n"
		"  -t<text> text to copy. Quote if it contains a blank character.\n"
		"If neither file nor text is specified, data is read from stdin\n"
		;

	char *text = NULL;
	char *file = NULL;
	uint32_t len;
	int allocated = 0;
	int ret;

	while (*args) {
		if (!strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (!strncmp(*args, "-t", 2) &&
			file == NULL && text == NULL) {

			if (*args + 2) {
				text = *args + 2;
			}
			else if (*(args + 1)) {
				text = *(++args);
			}
			else {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}
		else if (file == NULL && text == NULL) {
			file = *args;
		}
		else {
			fprintf(stderr, usage, progname);
			return VMTOOL_STX_ERROR;
		}
		args++;
	}

	if (text == NULL) {
		FILE *fp;

		if (file == NULL) {
			fp = stdin;
		}
		else if ((fp = fopen(file, "r")) == NULL) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "%s open error (%s)\n",
					file, strerror(errno));
			}
			return VMTOOL_SYS_ERROR;
		}

		text = (char *)malloc(COPY_BUF_SIZE);

		if (text == NULL) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "memory alloc error (%s)\n",
					strerror(errno));
			}
			if (file) {
				fclose(fp);
			}
			return VMTOOL_SYS_ERROR;
		}

		allocated = 1;

		len = fread(text, 1, COPY_BUF_SIZE, fp);

		if (file) {
			fclose(fp);
		}

#if defined(_MSDOS) || defined(_WIN32)
		/* remove 0x0d */
		{
			uint32_t l = 0;

			while (l < len) {
				if (*(text + l) == 0x0d) {
					memmove(text + l, text + l + 1, len - l - 1);
					len--;
				}
				else {
					l++;
				}
			}
		}
#endif

		if (vmtool_msg >= vmtool_verbose) {
			printf("Read %lu bytes from %s\n", len, file ? file : "<stdin>");
		}
	}
	else {
		len = strlen(text);

		if (*text == '"' || *text == '\'') {
			text++;
			len--;

			if (*(text + len) == '"' || *(text + len) == '\'') {
				len--;
			}
		}
	}

	if (len == 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "empty text\n");
		}
		if (allocated) {
			free(text);
		}

		return VMTOOL_SYS_ERROR;
	}

	/* discard existing clipboard data */
	if (vmtool_msg >= vmtool_verbose) {
		printf("Emptying clipboard\n");
	}

	VMClipboardPaste(NULL, NULL);

	if (vmtool_msg >= vmtool_verbose) {
		printf("Sending %lu bytes to clipboard\n", len);
	}

	ret = VMClipboardCopy(text, len);

	if (allocated) {
		free(text);
	}

	return ret;
}

static int cmd_cursor(char **args)
{
	static const char *usage =
		"Usage: %s cursor [{-r|-R}[interval]][-s<X>,<Y>]\n"
		"  -r  Repeat every <interval> milliseconds (default=100).\n"
		"      No space between -r and <interval>\n"
		"  -R  same as -r but suppress line feeding\n"
		"  -s  Set cursor to specified location\n"
		"      Separate X and Y with a comma (required).\n"
		"      No space between -s, X and Y\n"
		;

	uint32_t interval = 0;
	int no_lf = 0;
	int set = 0;
	uint16_t x_pos = 0, y_pos = 0;
	int ret;

	while (*args) {
		if (!strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (!strncmp(*args, "-r", 2) ||
			!strncmp(*args, "-R", 2)) {
			char *p = *args + 1;

			interval = 100;

			if (*(p++) == 'R') {
				no_lf = 1;
			}

			if (*p && (sscanf(p, "%u", &interval) != 1 || interval < 1)) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}
		else if (!strncmp(*args, "-s", 2)) {
			char *p = *args + 2;

			set = 1;

			if (sscanf(p, "%hu,%hu", &x_pos, &y_pos) != 2) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}
		else {
			fprintf(stderr, usage, progname);
			return VMTOOL_STX_ERROR;
		}

		args++;
	}

	if (set) {
		if (vmtool_msg >= vmtool_verbose) {
			printf("Moving cursor to (%hu, %hu)\n", x_pos, y_pos);
		}

		ret = VMSetCursorPos(x_pos, y_pos);

		if (ret != VMTOOL_SUCCESS) {
			return ret;
		}
	}

	ret = VMGetCursorPos(&x_pos, &y_pos);

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	if (!interval) {
		if (vmtool_msg > vmtool_quiet) {
			printf("%hd %hd\n", x_pos, y_pos);
		}
		return VMTOOL_SUCCESS;
	}

	if (vmtool_msg >= vmtool_verbose) {
		printf("Repeating every %d milliseconds\n"
			"Press any key to stop\n", interval);
	}

	if (vmtool_msg > vmtool_quiet) {
		printf("%hd %hd\n", x_pos, y_pos);
	}

	peekkey_prep();

	while (peekkey() == 0) {
		sleep_millisec(interval);

		if (no_lf) {
			if (vmtool_msg > vmtool_quiet) {
				cursor_up(1);
			}
			if (vmtool_dump) {
				cursor_up(3);
			}
		}

		ret = VMGetCursorPos(&x_pos, &y_pos);

		if (ret != VMTOOL_SUCCESS) {
			break;
		}

		if (vmtool_msg > vmtool_quiet) {
			erase_eol();
			printf("%hd %hd\n", x_pos, y_pos);
		}
	}

	peekkey_prep();

	return ret;
}

static int target_devs(const char *str, unsigned char *target)
{
	unsigned char action;
	int val;
	int num;

	if (isdigit(*str) || *str == 'a' || *str == 'A') {
		action = ' ';
	}
	else {
		action = *(str++);
	}

	if (*str == '\0' || *str == 'a' || *str == 'A') {
		memset(target, action, VMDEV_DEVICE_MAX);
		return VMDEV_DEVICE_MAX;
	}

	val = 0;
	num = 0;

	for (;;) {
		if (*str == 'a' || *str == 'A') {
			memset(target, action, VMDEV_DEVICE_MAX);
			return VMDEV_DEVICE_MAX;
		}
		else if (*str == ',' || *str == '\0') {
			if (val >= VMDEV_DEVICE_MAX) {
				fprintf(stderr, "Target number %d too large\n", val);
				return 0;
			}

			*(target + val) = action;
			num++;

			if (*str == '\0') {
				return num;
			}

			val = 0;
		}
		else if (isdigit(*str)) {
			val = val * 10 + *str - '0';
		}
		else {
			fprintf(stderr, "Invalid target number '%s'\n", str);
			return 0;
		}

		str++;
	}
}

static int cmd_device(char **args)
{
	static const char *usage =
		"Usage: %s device [+|-|/][<targets>]\n"
		"  +       (Re)connect targets.\n"
		"  -       Disconnect targets.\n"
		"  /       Toggle each target state.\n"
		"<targets> Target device numbers separated by comma.\n"
		"          'a' stands for 'all' (default).\n"
		"          No space among +,-,/ and <targets>.\n"
		"\n"
		"State is not changed if none of +,- or / is present\n"
		"(current state of the targets is displayed)\n"
		;

	unsigned char target[VMDEV_DEVICE_MAX];
	int target_num;
	uint16_t i;
	int ret;

	memset(target, 0, sizeof(target));
	target_num = 0;

	/*	process command line */
	while (*args) {

		if (!strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (isdigit(**args) ||
			strchr("+-/a", **args)) {

			if (target_num) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}

			target_num = target_devs(*args, target);

			if (target_num == 0) {
				return VMTOOL_STX_ERROR;
			}
		}
		else {
			fprintf(stderr, usage, progname);
			return VMTOOL_STX_ERROR;
		}

		args++;
	}

	if (target_num == 0) {
		/*
			both target and action are not specified
			- defaults to display all info
		*/
		target_num = VMDEV_DEVICE_MAX;
		memset(target, ' ', sizeof(target));
	}

	ret = VMTOOL_SUCCESS;

	for (i = 0; i < VMDEV_DEVICE_MAX; i++) {
		unsigned char info[40];
		int msg_tmp;

		if (target[i] == 0) {
			continue;
		}

		msg_tmp = vmtool_msg;

		if (target_num == VMDEV_DEVICE_MAX) {
			/* avoid error message if target is 'all' */
			vmtool_msg = vmtool_quiet;
		}

		ret = VMGetDeviceInfo(i, info);

		if (target_num == VMDEV_DEVICE_MAX) {
			/* restore message mode */
			vmtool_msg = msg_tmp;
		}

		if (ret != VMTOOL_SUCCESS) {
			if (target_num == VMDEV_DEVICE_MAX) {
				ret = VMTOOL_SUCCESS;
			}
			break;
		}

		if (target[i] == '-') {
			ret = VMConnectDevice(i, 0);
		}
		else if (target[i] == '+') {
			ret = VMConnectDevice(i, 1);
		}
		else if (target[i] == '/') {
			ret = VMConnectDevice(i, !info[VMDEV_STATE_OFFSET]);
		}

		if (ret != VMTOOL_SUCCESS) {
			break;
		}

		ret = VMGetDeviceInfo(i, info);

		if (ret != VMTOOL_SUCCESS) {
			break;
		}

		if (vmtool_msg > vmtool_quiet) {
			printf("%2hd: %-10.10s %s\n", i, info,
				info[VMDEV_STATE_OFFSET] ? "connected" : "disconnected");
		}
	}

	if (target_num == VMDEV_DEVICE_MAX && i == 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "No connectable device\n");
		}
	}

	return ret;
}

static int cmd_hwver(char **args)
{
	static const char *usage =
		"Usage: %s hwver\n"
		;

	static const char *hwname[] = {
		"unknown",
		"WS 2.x(?)",
		"WS 3.x, GSX 2.x(?), ESX 1.x(?)",
		"WS 4.x, 5.x legacy, GSX 3.x(?), ESX 2.x(?), ACE 1.x(?)",
		"WS 5.x"
	};

	uint32_t hwver;
	int ret;

	if (*args) {
		fprintf(stderr, usage, progname);
		return VMTOOL_STX_ERROR;
	}

	ret = VMGetHwVersion(&hwver);

	if (ret == VMTOOL_SUCCESS) {
		if (vmtool_msg == vmtool_curt) {
			printf("%lu\n", hwver);
		}
		else if (vmtool_msg >= vmtool_verbose) {
			printf("%lu: %s\n", hwver,
				((hwver >= (sizeof(hwname) / sizeof(hwname[0])))
					? hwname[0] : hwname[hwver]));
		}
	}

	return ret;
}

static int cmd_memory(char **args)
{
	static const char *usage =
		"Usage: %s memory\n"
		;

	uint32_t size;
	int ret;

	if (*args) {
		fprintf(stderr, usage, progname);
		return VMTOOL_STX_ERROR;
	}

	ret = VMGetMemSize(&size);

	if (ret == VMTOOL_SUCCESS &&
		vmtool_msg > vmtool_quiet) {
		printf("%lu\n", size);
	}

	return ret;
}

static int cmd_mhz(char **args)
{
	static const char *usage =
		"Usage: %s mhz\n"
		;

	uint32_t mhz;
	int ret;

	if (*args) {
		fprintf(stderr, usage, progname);
		return VMTOOL_STX_ERROR;
	}

	ret = VMGetMegaHerz(&mhz);

	if (ret == VMTOOL_SUCCESS &&
		vmtool_msg > vmtool_quiet) {
		printf("%lu\n", mhz);
	}

	return ret;
}

static int target_opt(const char *str, uint32_t *mask)
{
	int num = 0;

	for (;;) {
		switch (*str) {
		case 'G':
		case 'g':
			*mask |= VMPREF_GRAB_MOVE;
			num++;
			break;

		case 'U':
		case 'u':
			*mask |= VMPREF_UNGRAB_MOVE;
			num++;
			break;

		case 'S':
		case 's':
			*mask |= VMPREF_SCROLL_MOVE;
			num++;
			break;

		case 'C':
		case 'c':
			*mask |= VMPREF_COPY_PASTE;
			num++;
			break;

		case 'T':
		case 't':
			*mask |= VMPREF_SYNCRONIZE;
			num++;
			break;

		case '\0':
			return num;

		default:
			fprintf(stderr, "Unknown option '%c'\n", *str);
			return 0;
		}
		str++;
	}
}

static int cmd_option(char **args)
{
	static const char *usage =
		"Usage: %s option [[+|-|/]<options>]\n"
		"  +  Enables specified option.\n"
		"  -  Disables specified option.\n"
		"  /  Toggles specified option.\n"
		"<options> one or more of the following options:\n"
		"  g  grab input when cursor enters window\n"
		"  u  ungrab input when cursor leaves window\n"
		"  s  scroll when cursor approaches window edge\n"
		"  c  copy and paste between host and guest\n"
		"  t  time syncronization between host and guest\n"
		;

	uint32_t enable = 0, disable = 0, toggle = 0;
	int ret;

	while (*args) {
		if (!strcmp(*args, "-?") || !strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (**args == '+') {
			if (target_opt(*args + 1, &enable) == 0) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}
		else if (**args == '-') {
			if (target_opt(*args + 1, &disable) == 0) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}
		else if (**args == '/') {
			if (target_opt(*args + 1, &toggle) == 0) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}
		else {
			if (target_opt(*args, &toggle) == 0) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}

		args++;
	}

	if (enable || disable || toggle) {
		uint32_t opt_new, opt_old;

		ret = VMGetGuiOptions(&opt_old);

		if (ret != VMTOOL_SUCCESS) {
			return ret;
		}

		opt_new = opt_old;

		opt_new |= enable;
		opt_new &= ~disable;
		opt_new ^= toggle;

		if (opt_new != opt_old) {
			ret = VMSetGuiOptions(opt_new);

			if (ret != VMTOOL_SUCCESS) {
				return ret;
			}
		}
	}

	if (vmtool_msg > vmtool_quiet) {
		uint32_t options;

		ret = VMGetGuiOptions(&options);

		if (ret != VMTOOL_SUCCESS) {
			return ret;
		}

		printf("%s: grab input when cursor enters window\n",
			(options & VMPREF_GRAB_MOVE) ? "On " : "Off");

		printf("%s: ungrab input when cursor leaves window\n",
			(options & VMPREF_UNGRAB_MOVE) ? "On " : "Off");

		printf("%s: scroll when cursor approaches window edge\n",
			(options & VMPREF_SCROLL_MOVE) ? "On " : "Off");

		if (vmtool_msg >= vmtool_verbose && (options & 0x08)) {
			printf("On : unknown(0x0008)\n");
		}

		printf("%s: copy and paste between host and guest\n",
			(options & VMPREF_COPY_PASTE) ? "On " : "Off");

		if (vmtool_msg >= vmtool_verbose) {
			if (options & 0x20) {
				printf("On : unknown(0x0020)\n");
			}
			if (options & VMPREF_FULLSCREEN) {
				printf("On : currently running in fullscreen mode\n");
			}
			if (options & 0x100) {
				printf("On : unknown(0x0100)\n");
			}
			if (options & 0x200) {
				printf("On : unknown(0x0200)\n");
			}
		}

		printf("%s: time syncronization between host and guest\n",
			(options & VMPREF_SYNCRONIZE) ? "On " : "Off");

		if (vmtool_msg >= vmtool_verbose) {
			if (options & 0x800) {
				printf("On : unknown(0x0800)\n");
			}
			if (options & 0x1000) {
				printf("On : unknown(0x1000)\n");
			}
			if (options & 0x2000) {
				printf("On : unknown(0x2000)\n");
			}
			if (options & 0x4000) {
				printf("On : unknown(0x4000)\n");
			}
			if (options & 0x8000) {
				printf("On : unknown(0x8000)\n");
			}
		}
	}

	return VMTOOL_SUCCESS;
}

static int cmd_paste(char **args)
{
	static const char *usage =
		"Usage: %s paste [<file>]\n"
		"  <file> file to output pasted text\n"
		"\n"
		"If a file is not specified, the text goes to stdout\n"
		"regardless of the '-q' option.\n"
		;

	const char *file = NULL;
	FILE *fp = stdout;
	char *buf;
	int ret;

	while (*args) {
		if (!strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (file == NULL) {
			file = *args;
		}
		else {
			fprintf(stderr, usage, progname);
			return VMTOOL_STX_ERROR;
		}

		args++;
	}

	if (file && (fp = fopen(file, "a+")) == NULL) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "%s open error (%s)\n",
				file, strerror(errno));
		}
		return VMTOOL_SYS_ERROR;
	}

	buf = NULL;

	ret = VMClipboardPaste(&buf, NULL);

	if (buf) {
		if (ret == VMTOOL_SUCCESS) {
#if defined(_MSDOS) || defined(_WIN32)
			/* supply 0x0d */
			int len = 0;
			int pos = 0;

			while (buf[pos]) {
				if (buf[pos] == 0x0a) {
					fputc(0x0d, fp);
					len++;
				}

				fputc(buf[pos++], fp);
				len ++;
			}
#else
			int len = strlen(buf);

			if (len) {
				len = fwrite(buf, 1, len, fp);
			}
#endif

			if (vmtool_msg >= vmtool_verbose) {
				printf("%d bytes written to %s\n",
					len, file ? file : "<stdout>");
			}
		}
		free(buf);
	}

	if (file) {
		fclose(fp);
	}

	return ret;
}

static int cmd_rpc(char **args)
{
	static const char *usage =
		"Usage: %s rpc [-e] <command>\n"
		"  -e        Use enhanced RPC mechanism (WS4.0/GSX3.2 and later)\n"
		"  <command> RPC command to send to the host\n"
		"            Quote if it contains a blank character\n"
		"\n"
		"Following RPC commands are known to work:\n"
		"  machine.id.get\n"
		"  log <message>\n"
		"  info-set guestinfo.<varname> <value>\n"
		"  info-get guestinfo.<varname>\n"
		"  tools.set.version <version>\n"
		"  disk.wiper.enable\n"
		"  disk.shrink\n"
		;

	rpc_t rpc;
	uint32_t enhanced = 0;
	unsigned char *command 	= NULL;
	unsigned char *result;
	uint32_t length;
	int ret;

	while (*args) {
		if (!strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (!strcmp(*args, "-e")) {
			enhanced = VMRPC_ENHANCED;
		}
		else if (**args != '-' && command == NULL) {
			command = (unsigned char *)*args;
		}
		else {
			fprintf(stderr, usage, progname);
			return VMTOOL_STX_ERROR;
		}

		args++;
	}

	if (!command) {
		fprintf(stderr, usage, progname);
		return VMTOOL_STX_ERROR;
	}

	ret = VMRpcOpen(&rpc, enhanced);

	if (ret != VMTOOL_SUCCESS &&
		enhanced == VMRPC_ENHANCED) {

		ret = VMRpcOpen(&rpc, 0);

		if (ret == VMTOOL_SUCCESS) {
			enhanced = 0;

			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Using conventional RPC\n");
			}
		}
	}

	if (ret != VMTOOL_SUCCESS) {
		return ret;
	}

	if (vmtool_msg >= vmtool_verbose) {
		if (enhanced == VMRPC_ENHANCED) {
			printf("RPC channel: %lu (cookie %08lx:%08lx)\n",
				(rpc.channel >> 16), rpc.cookie1, rpc.cookie2);
		}
		else {
			printf("RPC channel: %lu (conventional)\n",
				(rpc.channel >> 16));
		}
	}

	if (vmtool_msg >= vmtool_verbose) {
		printf("RPC command: %s\n", command);
	}

	ret = VMRpcSend(&rpc, command, (uint32_t)-1);

	if (ret != VMTOOL_SUCCESS) {
		goto close_rpc;
	}

	result = NULL;

	ret = VMRpcRecv(&rpc, &result, &length);

	if (ret == VMTOOL_SUCCESS && result && *result) {
		if (vmtool_msg == vmtool_curt) {
			printf("%s\n", result);
		}
		else if (vmtool_msg >= vmtool_verbose) {
			printf("RPC reply  : %s\n", result);
		}
	}

	if (result) {
		free(result);
	}

close_rpc:
	if (VMRpcClose(&rpc) == VMTOOL_SUCCESS) {
		if (vmtool_msg >= vmtool_verbose) {
			printf("RPC channel closed\n");
		}
	}

	return ret;
}

static int cmd_screen(char **args)
{
	static const char *usage =
		"Usage: %s screen\n"
		;

	uint16_t width, height;
	int ret;

	if (*args) {
		fprintf(stderr, usage, progname);
		return VMTOOL_STX_ERROR;
	}

	ret =  VMGetScreenSize(&width, &height);

	if (ret == VMTOOL_SUCCESS &&
		vmtool_msg > vmtool_quiet) {
		printf("%hu %hu\n", width, height);
	}

	return ret;
}

static int cmd_time(char **args)
{
	static const char *usage =
#if defined (_WIN32) || defined(_MSDOS)
		"Usage: %s time [-u][{-r|-R}[interval]][-g]\n"
		"  -u  Update the guest system clock\n"
		/* always gmt in Windows, always local in DOS */
#else
		"Usage: %s time [{-u|-U}][{-r|-R}[interval]][-g]\n"
		"  -u  Update the guest system clock using local time\n"
		"  -U  Update the guest system clock using gmt\n"
#endif
		"  -r  Repeat every <interval> seconds (default: 60 sec)\n"
		"  -R  same as -r but suppress line feeding\n"
		"  -g  Print time in gmt (default: local time)\n"
		;

	time_t host_time;
	uint32_t micro, whatsthis;
	int32_t offset;
	int update = 0;
	int interval = 0;
	int printgmt = 0;
	int no_lf = 0;
	struct tm *t;
	int ret;
	int i;

	while (*args) {
		if (!strcmp(*args, "-h")) {
			fprintf(stderr, usage, progname);
			return VMTOOL_SUCCESS;
		}
		else if (!strcmp(*args, "-u")) {
			update = 1;
		}
		else if (!strcmp(*args, "-U")) {
			update = 2;
		}
		else if (!strncmp(*args, "-r", 2) ||
			!strncmp(*args, "-R", 2)) {
			char *p = *args + 1;

			interval = 60;

			if (*(p++) == 'R') {
				no_lf = 1;
			}

			if (*p && (sscanf(p, "%d", &interval) != 1 || interval < 1)) {
				fprintf(stderr, usage, progname);
				return VMTOOL_STX_ERROR;
			}
		}
		else if (!strcmp(*args, "-g")) {
			printgmt = 1;
		}
		else {
			fprintf(stderr, usage, progname);
			return VMTOOL_STX_ERROR;
		}

		args++;
	}

#ifdef _MSDOS
	/*
		check TZ environment
	*/
	if (getenv("TZ") == NULL) {
		printf(
			"TZ environment is not set.  Date/time is handled in GMT.\n");
		_putenv("TZ=GMT+0");
	}
#endif

	if (interval && vmtool_msg >= vmtool_verbose) {
		printf("Repeating every %d seconds.\n"
			"Press any key to stop\n", interval);
	}

	for (;;) {
		ret = VMGetGmTime((uint32_t *)&host_time,
			&micro, &whatsthis, &offset);

		if (ret != VMTOOL_SUCCESS) {
			return ret;
		}

		if (!gmtime(&host_time)) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, "Invaid host time\n");
			}
			return VMTOOL_SYS_ERROR;
		}

		if (update) {
			time_t upd_time;

#if defined(_MSDOS)
			update = 1;	/* always local */
#elif defined(_WIN32)
			update = 2;	/* always gmt */
#endif

			if (update == 1) {
				/* convert into local time */
				upd_time = mktime(localtime(&host_time));
			}
			else {
				upd_time = host_time;
			}

			if (set_systemtime(upd_time, micro) != 0) {
				if (vmtool_msg > vmtool_quiet) {
					fprintf(stderr, "failed to update system time\n");
					return VMTOOL_SYS_ERROR;
				}
			}
		}

		if (vmtool_msg > vmtool_quiet) {
			if (printgmt) {
				t = gmtime(&host_time);
			}
			else {
				t = localtime(&host_time);
			}

			printf("%04d-%02d-%02d %02d:%02d:%02d.%06lu %s\n",
				t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
				t->tm_hour, t->tm_min, t->tm_sec, micro,
				printgmt ? "GMT" : tzname[0]);
		}

		if (!interval) {
			return VMTOOL_SUCCESS;
		}

		peekkey_prep();

		for (i = 0; i < interval; i++) {
			if (peekkey()) {
				peekkey_end();
				return VMTOOL_SUCCESS;
			}
			sleep_millisec(1000);
		}

		peekkey_end();

		if (no_lf) {
			if (vmtool_msg > vmtool_quiet) {
				cursor_up(1);
			}
			if (vmtool_dump) {
				cursor_up(3);
			}
		}
	}
}

static int cmd_uuid(char **args)
{
	static const char *usage =
		"Usage: %s uuid\n"
		;

	unsigned char uuid[VMWARE_UUID_LENGTH];
	int ret;

	if (*args) {
		fprintf(stderr, usage, progname);
		return VMTOOL_STX_ERROR;
	}

	ret = VMGetBiosUuid(uuid);

	if (ret == VMTOOL_SUCCESS &&
		vmtool_msg > vmtool_quiet) {
		int i;

		for (i = 0; i < sizeof(uuid); i++) {
			printf("%02x ", uuid[i]);
		}
		printf("\n");
	}

	return ret;
}

static int cmd_version(char **args)
{
	static const char *usage =
		"Usage: %s version\n"
		;

	static const char *prod_name[] = {
		"unknown",
		"Express",
		"ESX Server",
		"GSX Server",
		"Workstation"
	};

	uint32_t version, product;
	int ret;

	if (*args) {
		fprintf(stderr, usage, progname);
		return VMTOOL_STX_ERROR;
	}

	ret = VMGetVersion(&version, &product);

	if (ret == VMTOOL_SUCCESS) {
		if (vmtool_msg == vmtool_curt) {
			printf("%lu %lu\n", version, product);
		}
		else if (vmtool_msg >= vmtool_verbose) {
			printf("%lu %lu (%s)\n",
				version, product,
				((product >= (sizeof(prod_name) / sizeof(prod_name[0])))
					? prod_name[0] : prod_name[product]));
		}
	}

	return ret;
}

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*	sleep milliseconds */
void sleep_millisec(uint32_t ms)
{
	Sleep(ms);
}

/*	set system clock */
int set_systemtime(uint32_t seconds, uint32_t micro)
{
	SYSTEMTIME sys;
	struct tm *gmt = gmtime((time_t *)&seconds);

	sys.wYear 	= (WORD)(gmt->tm_year + 1900);
	sys.wMonth 	= (WORD)(gmt->tm_mon + 1);
	sys.wDay	= (WORD)gmt->tm_mday;
	sys.wHour	= (WORD)gmt->tm_hour;
	sys.wMinute	= (WORD)gmt->tm_min;
	sys.wSecond	= (WORD)gmt->tm_sec;
	sys.wMilliseconds = (WORD)(micro / 1000);

	if (SetSystemTime(&sys)) {
		return 0;
	}

	return -1;
}

/*	get system time */
uint32_t get_systemtime(uint32_t *micro)
{
	SYSTEMTIME sys;
	struct tm gmt;

	GetSystemTime(&sys);

	gmt.tm_year = sys.wYear - 1900;
	gmt.tm_mon	= sys.wMonth - 1;
	gmt.tm_mday	= sys.wDay;
	gmt.tm_hour	= sys.wHour;
	gmt.tm_min	= sys.wMinute;
	gmt.tm_sec	= sys.wSecond;

	*micro = (uint32_t)sys.wMilliseconds * 1000;

	return mktime(&gmt);
}

#elif defined(_MSDOS)

#include <dos.h>

/*	sleep milli seconds */
void sleep_millisec(uint32_t ms)
{
	msleep(ms);
}

/*	set system clock */
int set_systemtime(uint32_t seconds, uint32_t micro)
{
	struct _dosdate_t dos_day;
	struct _dostime_t dos_tm;
	struct tm *local;

	_tzset();

	local = localtime((time_t *)&seconds);

	dos_day.year 	= local->tm_year + 1900;
	dos_day.month	= local->tm_mon + 1;
	dos_day.day		= local->tm_mday;

	dos_tm.hour 	= local->tm_hour;
	dos_tm.minute 	= local->tm_min;
	dos_tm.second	= local->tm_sec;
	dos_tm.hsecond	= (unsigned char)(micro / 10000);

	if (_dos_setdate(&dos_day) == 0 &&
		_dos_settime(&dos_tm) == 0) {
		return 0;
	}

	return -1;
}

/*	get system time */
uint32_t get_systemtime(uint32_t *micro)
{
	struct _dosdate_t dos_day;
	struct _dostime_t dos_tm;
	struct tm local;

	_dos_getdate(&dos_day);
	_dos_gettime(&dos_tm);

	local.tm_year	= dos_day.year 	 - 1900;
	local.tm_mon	= dos_day.month	 - 1;
	local.tm_mday	= dos_day.day;

	local.tm_hour	= dos_tm.hour;
	local.tm_min	= dos_tm.minute;
	local.tm_sec	= dos_tm.second;

	_tzset();

	*micro = (uint32_t)dos_tm.hsecond * 10000;

	return (mktime(&local) + _timezone);
}

#else	/* systems other than WIN32 and DOS (unix assumed) */

#include <unistd.h>

#if HAVE_SETTIMEOFDAY
#include <sys/time.h>
#endif

/* sleep milli second */
static void sleep_millisec(uint32_t ms)
{
#if HAVE_NANOSLEEP
	struct timespec ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000L;

	nanosleep(&ts, NULL);
#elif HAVE_USLEEP
	usleep(ms * 1000);
#else
	sleep((ms + 999) / 1000);
#endif
}

/* set system clock */
static int set_systemtime(uint32_t seconds, uint32_t micro)
{
#if HAVE_CLOCK_SETTIME
	{
		struct timespec ts;

		ts.tv_sec = seconds;
		ts.tv_nsec = micro * 1000L;

		if (clock_settime(CLOCK_REALTIME, &ts) == 0) {
			return 0;
		}

		if (errno != ENOSYS) {
			return -1;
		}
	}
#endif /* HAVE_CLOCK_SETTIME */

#if HAVE_SETTIMEOFDAY
	{
		struct timeval tv;

		tv.tv_sec = seconds;
		tv.tv_usec = micro;

		if (settimeofday(&tv, 0) == 0) {
			return 0;
		}

		if (errno != ENOSYS) {
			return -1;
		}
	}
#endif /* HAVE_SETTIMEOFDAY */

#if HAVE_STIME
	if (stime((time_t *)&seconds) == 0) {
		return 0;
	}
#endif /* HAVE_STIME */

	return -1;
}

/*	get system clock */
static uint32_t get_systemtime(uint32_t *micro)
{
#if HAVE_CLOCK_SETTIME
	{
		struct timespec ts;

		if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
			*micro = ts.tv_nsec / 1000L;
			return ts.tv_sec;
		}

		if (errno != ENOSYS) {
			return 0;
		}
	}
#endif /* HAVE_CLOCK_SETTIME */

#if HAVE_SETTIMEOFDAY
	{
		struct timeval tv;

		if (gettimeofday(&tv, 0) == 0) {
			*micro = tv.tv_usec;
			return tv.tv_sec;
		}

		if (errno != ENOSYS) {
			return 0;
		}
	}
#endif /* HAVE_SETTIMEOFDAY */

	*micro = 0;
	return time(NULL);
}

#endif	/* system dependent time functions */
