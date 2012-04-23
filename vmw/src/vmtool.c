/*
	vmtool.c
	VMware backdoor call wrapper functions

	Copyright (c) 2006 Ken Kato
*/

#include "vmcall.h"
#include "vmtool.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
	global message level flags
*/
int vmtool_msg = vmtool_curt;
int vmtool_dump = 0;

/*
	fixed message strings
*/
static const char *msg_cmd_fail = "Backdoor command call %Xh failed.\n";
static const char *msg_rpc_fail = "Backdoor RPC command call %04Xh failed.\n";

/*
	dump calling register values (with a header line)
*/
static void dump_reg_c(const reg_t *r)
{
	if (vmtool_dump) {
		printf(
			"     EAX      EBX      ECX      EDX      ESI      EDI      EBP\n"
			"> %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
			r->eax,r->ebx,r->ecx,r->edx,r->esi,r->edi,r->ebp);
	}
}

/*
	dump return register values
*/
static void dump_reg_r(const reg_t *r)
{
	if (vmtool_dump) {
		printf("< %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
			r->eax,r->ebx,r->ecx,r->edx,r->esi,r->edi,r->ebp);
	}
}

/*
	Virtual environment check
*/
#define MSG_NOT_VM	"Not running on VMware\n"

#ifdef _WIN32
/*
	Windows -- using Win32 Structured Exception Handling
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int VMCheckVirtual()
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = ~VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_VERSION;
	R_EDX(reg) = VMWARE_CMD_PORT;

	__try {
		/* execute an I/O call */
		vmcall_cmd(&reg);
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		/* The backdoor I/O access causes an EXCEPTION_PRIV_INSTRUCTION		*/
		/* exception 32 bit Windows process on non-VMware environment		*/
	}

	if (R_EBX(reg) != VMWARE_MAGIC) {
		/* get version backdoor command failure */
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, MSG_NOT_VM);
		}
		return VMTOOL_CMD_ERROR;
	}

	return VMTOOL_SUCCESS;
}

#elif defined(_MSDOS)
/*
	16 bit DOS system
	-- backdoor call just fails without any side effects
*/
int VMCheckVirtual()
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = ~VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_VERSION;
	R_EDX(reg) = VMWARE_CMD_PORT;

	vmcall_cmd(&reg);

	if (R_EBX(reg) != VMWARE_MAGIC) {
		/* get version backdoor command failure */
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, MSG_NOT_VM);
		}
		return VMTOOL_CMD_ERROR;
	}

	return VMTOOL_SUCCESS;
}

#else
/*
	unix like systems
	- backdoor access causes SIGSEGV on non-VMware environment
*/
#include <setjmp.h>
#include <signal.h>

#if HAVE_SIGLONGJMP
static sigjmp_buf	sigsegvbuffer;
#else	/* HAVE_SIGLONGJMP */
static jmp_buf		sigsegvbuffer;
#endif	/* HAVE_SIGLONGJMP */

static void sigsegvhandler(int sig)
{
#if HAVE_SIGLONGJMP
	siglongjmp(sigsegvbuffer, sig);
#else	/* HAVE_SIGLONGJMP */
	longjmp(sigsegvbuffer, sig);
#endif	/* HAVE_SIGLONGJMP */
}

int VMCheckVirtual()
{
	reg_t reg = {0};
	struct sigaction newaction, oldaction;

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = ~VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_VERSION;
	R_EDX(reg) = VMWARE_CMD_PORT;

	sigemptyset(&newaction.sa_mask);
	newaction.sa_flags = SA_RESTART;
	newaction.sa_handler = sigsegvhandler;
	sigaction(SIGSEGV, &newaction, &oldaction);

#if HAVE_SIGLONGJMP
	if (sigsetjmp(sigsegvbuffer, 1) != 0) {
#else	/* HAVE_SIGLONGJMP */
	if (setjmp(sigsegvbuffer) != 0) {
#endif	/* HAVE_SIGLONGJMP */
		/* SIGSEGV is caught during the backdoor access */
		sigaction(SIGSEGV, &oldaction, 0);

		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, MSG_NOT_VM);
		}

		return VMTOOL_CMD_ERROR;
	}

	vmcall_cmd(&reg);

	sigaction(SIGSEGV, &oldaction, 0);

	if (R_EBX(reg) != VMWARE_MAGIC) {
		/* get version backdoor command failure */
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, MSG_NOT_VM);
		}
		return VMTOOL_CMD_ERROR;
	}

	return VMTOOL_SUCCESS;
}

#endif /* target system */


/*
	Get processor clock (MHz)
*/
int VMGetMegaHerz(uint32_t *mhz)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_MEGAHERZ;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == 0 ||
		R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {

		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_MEGAHERZ);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (mhz) {
		*mhz = R_EAX(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	call APM function
*/
int VMApmFunction(uint32_t func)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = (func >> 16);
	R_ECX(reg) = (func << 16) | VMCMD_APM_FUNCTION;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {

		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_APM_FUNCTION);
		}
		return VMTOOL_CMD_ERROR;
	}

	return VMTOOL_SUCCESS;
}

/*
	Get mouse cursor position
*/
int VMGetCursorPos(uint16_t *xpos, uint16_t *ypos)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_CURSOR;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {

		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_CURSOR);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (xpos) {
		*xpos = (uint16_t)(R_EAX(reg) >> 16);
	}

	if (ypos) {
		*ypos = (uint16_t)(R_EAX(reg) & 0xffff);
	}

	return VMTOOL_SUCCESS;
}

/*
	Set mouse cursor position
*/
int VMSetCursorPos(uint16_t xpos, uint16_t ypos)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = ((uint32_t)xpos << 16) | ypos;
	R_ECX(reg) = VMCMD_SET_CURSOR;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) != 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_SET_CURSOR);
		}

		return VMTOOL_CMD_ERROR;
	}

	return VMTOOL_SUCCESS;
}

/*
	Paste text from clipboard
*/
int VMClipboardPaste(char **data, uint32_t *length)
{
	reg_t reg = {0};
	uint32_t total;
	char *buf = NULL;

	/*	get text length */

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_CLIPLEN;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	total = R_EAX(reg);

	if (total == 0) {
		if (data && vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "No data in clipboard\n");
		}
		*length = 0;

		return VMTOOL_SUCCESS;
	}

	if (total > 0xffff) {
		if (data && vmtool_msg > vmtool_quiet) {
			if (total == 0xffffffffL) {
				fprintf(stderr, "No more data in clipboard\n");
			}
			else {
				fprintf(stderr, "Unexpected data length (%lu)\n", total);
			}
		}
		return VMTOOL_CMD_ERROR;
	}

	/* allocate text buffer */

	if (data) {
		if (vmtool_msg >= vmtool_verbose) {
			printf("%lu bytes to read from clipboard\n", total);
		}

		buf = malloc(total + 1);

		if (buf == NULL) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr,
					"memory allocate error (%s)\n", strerror(errno));
			}
			return VMTOOL_SYS_ERROR;
		}

		*data = buf;
	}

	if (length) {
		*length = total;
	}

	/* get text data */

	for (;;) {
		memset(&reg, 0, sizeof(reg));

		R_EAX(reg) = VMWARE_MAGIC;
		R_ECX(reg) = VMCMD_GET_CLIPDATA;
		R_EDX(reg) = VMWARE_CMD_PORT;

		dump_reg_c(&reg);

		vmcall_cmd(&reg);

		dump_reg_r(&reg);

		if (buf) {
			memcpy(buf, &R_EAX(reg), total > 4 ? 4 : total);

			if (total <= 4) {
				*(buf + total) = '\0';
			}
			else {
				buf += 4;
			}
		}

		if (total <= 4) {
			return VMTOOL_SUCCESS;
		}
		else {
			total -= 4;
		}
	}
}

/*
	copy text to clipboard
*/
int VMClipboardCopy(const char *data, uint32_t length)
{
	reg_t reg = {0};

	if (!data) {
		return VMTOOL_SYS_ERROR;
	}

	if (length == (uint32_t)-1) {
		length = strlen(data);
	}

	if (length == 0) {
		return VMTOOL_SYS_ERROR;
	}

	if (length > 0xffff) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, "data too long (%lu bytes)\n", length);
		}
		return VMTOOL_SYS_ERROR;
	}

	/*	set text length */

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = length;
	R_ECX(reg) = VMCMD_SET_CLIPLEN;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) != 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_SET_CLIPLEN);
		}
		return VMTOOL_CMD_ERROR;
	}

	/* set text data */

	for (;;) {
		memset(&reg, 0, sizeof(reg));

		R_EAX(reg) = VMWARE_MAGIC;
		memcpy(&R_EBX(reg), data, length > 4 ? 4 : length);
		R_ECX(reg) = VMCMD_SET_CLIPDATA;
		R_EDX(reg) = VMWARE_CMD_PORT;

		dump_reg_c(&reg);

		vmcall_cmd(&reg);

		dump_reg_r(&reg);

		if (length <= 4) {
			return VMTOOL_SUCCESS;
		}

		data += 4;
		length -= 4;
	}
}

/*
	get vmware version info
*/
int VMGetVersion(uint32_t *version, uint32_t *product)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_VERSION;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EBX(reg) != VMWARE_MAGIC) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_VERSION);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (version) {
		*version = R_EAX(reg);
	}

	if (product) {
		*product = R_ECX(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	get device info
*/
int VMGetDeviceInfo(
	uint16_t dev,						/* device number 0 - 51(?)			*/
	unsigned char *info)				/* 40 bytes device info area		*/
{
	reg_t reg = {0};
	int pos;

	for (pos = 0; pos < 40; pos += 4) {
		R_EAX(reg) = VMWARE_MAGIC;
		R_EBX(reg) = (uint32_t)dev << 16 | (uint16_t)pos;
		R_ECX(reg) = VMCMD_GET_DEVINFO;
		R_EDX(reg) = VMWARE_CMD_PORT;

		dump_reg_c(&reg);

		vmcall_cmd(&reg);

		dump_reg_r(&reg);

		if (R_EAX(reg) != 1) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, msg_cmd_fail, VMCMD_GET_DEVINFO);
			}
			return VMTOOL_CMD_ERROR;
		}

		if (info) {
			*((uint32_t *)(info + pos)) = R_EBX(reg);
		}
	}

	return VMTOOL_SUCCESS;
}

/*
	connect/disconnect device
*/
int VMConnectDevice(
	uint16_t dev,						/* device number					*/
	int connect)						/* 0:disconnect !0:connect			*/
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = dev;
	R_ECX(reg) = VMCMD_SET_DEVSTAT;
	R_EDX(reg) = VMWARE_CMD_PORT;

	if (connect) {						/* connect operation ?				*/
		R_EBX(reg) |= 0x80000000L;
	}

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) != 1) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_SET_DEVSTAT);
		}
		return VMTOOL_CMD_ERROR;
	}

	return VMTOOL_SUCCESS;
}

/*
	get GUI option settings
*/
int VMGetGuiOptions(uint32_t *opt)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_OPTIONS;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_OPTIONS);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (opt) {
		*opt = R_EAX(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	set GUI option settings
*/
int VMSetGuiOptions(uint32_t opt)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = opt;
	R_ECX(reg) = VMCMD_SET_OPTIONS;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_SET_OPTIONS);
		}
		return VMTOOL_CMD_ERROR;
	}

	return VMTOOL_SUCCESS;
}

/*
	get host screen size
*/
int VMGetScreenSize(uint16_t *width, uint16_t *height)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_SCRSIZE;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == 0 ||
		R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_SCRSIZE);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (width) {
		*width = (uint16_t)(R_EAX(reg) >> 16);
	}

	if (height) {
		*height = (uint16_t)(R_EAX(reg) & 0xffff);
	}

	return VMTOOL_SUCCESS;
}

/*
	get virtual hardware version
*/
int VMGetHwVersion(uint32_t *version)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_HARDWARE;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == 0 ||
		R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_HARDWARE);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (version) {
		*version = R_EAX(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	popup "OS not found" dialog
*/
void VMOsNotFound()
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_OSNOTFOUND;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);
}

/*
	get BIOS UUID
*/
int VMGetBiosUuid(unsigned char *uuid)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_BIOSUUID;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == 0 ||
		R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_BIOSUUID);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (uuid) {
		*(uint32_t *)uuid = R_EAX(reg);
		*(uint32_t *)(uuid + 4) = R_EBX(reg);
		*(uint32_t *)(uuid + 8) = R_ECX(reg);
		*(uint32_t *)(uuid + 12) = R_EDX(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	get vm memory size (MB)
*/
int VMGetMemSize(uint32_t *memsize)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_MEMSIZE;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg) == 0 ||
		R_EAX(reg) == VMWARE_MAGIC ||
		R_EAX(reg) == 0xffffffffL) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_MEMSIZE);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (memsize) {
		*memsize = R_EAX(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	get host system time (GMT)
*/
int VMGetGmTime(
	uint32_t *time, uint32_t *micro, uint32_t *whatsthis, int32_t *offset)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_GET_GMTIME;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (!gmtime((time_t *)&R_EAX(reg))) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_GET_GMTIME);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (time) {
		*time = R_EAX(reg);
	}
	if (micro) {
		*micro = R_EBX(reg);
	}
	if (whatsthis) {
		*whatsthis = R_ECX(reg);
	}
	if (offset) {
		*offset = R_EDX(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	open RPC channel
*/
int VMRpcOpen(
	rpc_t *rpc,
	uint32_t method)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = VMRPC_OPEN_RPCI | (method & VMRPC_ENHANCED);
	R_ECX(reg) = VMCMD_INVOKE_RPC | VMRPC_OPEN;
	R_EDX(reg) = VMWARE_CMD_PORT;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg)) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_INVOKE_RPC | VMRPC_OPEN);
		}
		return VMTOOL_CMD_ERROR;
	}

	if (R_ECX(reg) != 0x00010000L) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_OPEN);
		}
		return VMTOOL_RPC_ERROR;
	}

	if (R_EDX(reg) & 0xffff) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_OPEN);
		}
		return VMTOOL_RPC_ERROR;
	}

	memset(rpc, 0, sizeof(rpc_t));

	rpc->channel = R_EDX(reg);

	if (method & VMRPC_ENHANCED) {
		rpc->cookie1 = R_ESI(reg);
		rpc->cookie2 = R_EDI(reg);
	}

	return VMTOOL_SUCCESS;
}

/*
	send RPC command
*/
int VMRpcSend(
	const rpc_t *rpc,
	const unsigned char *command,
	uint32_t length)
{
	reg_t reg = {0};

	/* send command length */

	if (length == (uint32_t)-1) {
		length = strlen((const char *)command);
	}

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = length;
	R_ECX(reg) = VMCMD_INVOKE_RPC | VMRPC_SENDLEN;
	R_EDX(reg) = rpc->channel | VMWARE_CMD_PORT;
	R_ESI(reg) = rpc->cookie1;
	R_EDI(reg) = rpc->cookie2;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg)) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_INVOKE_RPC | VMRPC_SENDLEN);
		}
		return VMTOOL_CMD_ERROR;
	}

	if ((R_ECX(reg) >> 16) == 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_SENDLEN);
		}
		return VMTOOL_RPC_ERROR;
	}

	if (!length) {
		return VMTOOL_SUCCESS;
	}

	/* send command string */

	if (rpc->cookie1 && rpc->cookie2) {

		/* enhanced RPC */

		memset(&reg, 0, sizeof(reg));

		R_EAX(reg) = VMWARE_MAGIC;
		R_EBX(reg) = VMRPC_ENH_DATA;
		R_ECX(reg) = length;
		R_EDX(reg) = rpc->channel | VMWARE_RPC_PORT;
		R_ESI(reg) = (uint32_t)(unsigned)command;
		R_EDI(reg) = rpc->cookie2;
		R_EBP(reg) = rpc->cookie1;

		dump_reg_c(&reg);

		vmcall_rpc_outs(&reg);

		dump_reg_r(&reg);

		if (R_EBX(reg) != VMRPC_ENH_DATA) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_SENDDAT);
			}
			return VMTOOL_RPC_ERROR;
		}
	}
	else {
		/* conventional RPC */

		for (;;) {
			memset(&reg, 0, sizeof(reg));

			R_EAX(reg) = VMWARE_MAGIC;
			memcpy(&R_EBX(reg), command, length > 4 ? 4 : length);
			R_ECX(reg) = VMCMD_INVOKE_RPC | VMRPC_SENDDAT;
			R_EDX(reg) = rpc->channel | VMWARE_CMD_PORT;

			dump_reg_c(&reg);

			vmcall_cmd(&reg);

			dump_reg_r(&reg);

			if (R_EAX(reg)) {
				if (vmtool_msg > vmtool_quiet) {
					fprintf(stderr, msg_cmd_fail, VMCMD_INVOKE_RPC | VMRPC_SENDDAT);
				}
				return VMTOOL_CMD_ERROR;
			}

			if ((R_ECX(reg) >> 16) == 0) {
				if (vmtool_msg > vmtool_quiet) {
					fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_SENDDAT);
				}
				return VMTOOL_RPC_ERROR;
			}

			if (length <= 4) {
				break;
			}

			length -= 4;
			command += 4;
		}
	}

	return VMTOOL_SUCCESS;
}

/*
	receive RPC reply
*/
int VMRpcRecv(
	const rpc_t *rpc,
	unsigned char **result,
	uint32_t *reslen)
{
	int ret;
	uint32_t length;
	uint16_t dataid;

	ret = VMRpcRecvLen(rpc, &length, &dataid);

	if (ret != VMTOOL_SUCCESS || length == 0) {
		return ret;
	}

	/* returned length does not include terminating NULL character */

	*result = (unsigned char *)malloc(length + 4);

	if (!*result) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr,
				"memory allocate error (%s)\n", strerror(errno));
		}
		return VMTOOL_SYS_ERROR;
	}

	if (reslen) {
		*reslen = length;
	}

	memset(*result, 0, length + 4);

	return VMRpcRecvDat(rpc, *result, length, dataid);
}

int VMRpcRecvLen(const rpc_t *rpc, uint32_t *length, uint16_t *dataid)
{
	/* receive result length */
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_INVOKE_RPC | VMRPC_RECVLEN;
	R_EDX(reg) = rpc->channel | VMWARE_CMD_PORT;
	R_ESI(reg) = rpc->cookie1;
	R_EDI(reg) = rpc->cookie2;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg)) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_INVOKE_RPC | VMRPC_RECVLEN);
		}
		return VMTOOL_CMD_ERROR;
	}

	if ((R_ECX(reg) >> 16) == 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_RECVLEN);
		}
		return VMTOOL_RPC_ERROR;
	}

	*length = R_EBX(reg);
	*dataid = (uint16_t)(R_EDX(reg) >> 16);

	return VMTOOL_SUCCESS;
}

int VMRpcRecvDat(const rpc_t *rpc, unsigned char *data, uint32_t length, uint16_t dataid)
{
	reg_t reg = {0};

	if (rpc->cookie1 && rpc->cookie2) {

		/* enhanced RPC */

		memset(&reg, 0, sizeof(reg));

		R_EAX(reg) = VMWARE_MAGIC;
		R_EBX(reg) = VMRPC_ENH_DATA;
		R_ECX(reg) = length;
		R_EDX(reg) = rpc->channel | VMWARE_RPC_PORT;
		R_ESI(reg) = rpc->cookie1;
		R_EDI(reg) = (uint32_t)(unsigned)data;
		R_EBP(reg) = rpc->cookie2;

		dump_reg_c(&reg);

		vmcall_rpc_ins(&reg);

		dump_reg_r(&reg);

		if (R_EBX(reg) != VMRPC_ENH_DATA) {
			if (vmtool_msg > vmtool_quiet) {
				fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_RECVDAT);
			}
			return VMTOOL_RPC_ERROR;
		}
	}
	else {
		/* conventional RPC */

		uint32_t *p = (uint32_t *)data;

		for (;;) {
			memset(&reg, 0, sizeof(reg));

			R_EAX(reg) = VMWARE_MAGIC;
			R_EBX(reg) = dataid;
			R_ECX(reg) = VMCMD_INVOKE_RPC | VMRPC_RECVDAT;
			R_EDX(reg) = rpc->channel | VMWARE_CMD_PORT;

			dump_reg_c(&reg);

			vmcall_cmd(&reg);

			dump_reg_r(&reg);

			if (R_EAX(reg)) {
				if (vmtool_msg > vmtool_quiet) {
					fprintf(stderr, msg_cmd_fail, VMCMD_INVOKE_RPC | VMRPC_RECVDAT);
				}
				return VMTOOL_CMD_ERROR;
			}

			if ((R_ECX(reg) >> 16) == 0) {
				if (vmtool_msg > vmtool_quiet) {
					fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_RECVDAT);
				}
				return VMTOOL_RPC_ERROR;
			}

			*(p++) = R_EBX(reg);

			if (length <= 4) {
				break;
			}

			length -= 4;
		}
	}

	/*
		I'm not sure what the following command is for.
		It's just the vmware-tools always uses this command in this place.
	*/
	memset(&reg, 0, sizeof(reg));

	R_EAX(reg) = VMWARE_MAGIC;
	R_EBX(reg) = dataid;
	R_ECX(reg) = VMCMD_INVOKE_RPC | VMRPC_RECVEND;
	R_EDX(reg) = rpc->channel | VMWARE_CMD_PORT;
	R_ESI(reg) = rpc->cookie1;
	R_EDI(reg) = rpc->cookie2;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg)) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_INVOKE_RPC | VMRPC_RECVEND);
		}
		return VMTOOL_CMD_ERROR;
	}

	if ((R_ECX(reg) >> 16) == 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_RECVEND);
		}
		return VMTOOL_RPC_ERROR;
	}

	return VMTOOL_SUCCESS;
}

/*
	close RPC channel
*/
int VMRpcClose(
	const rpc_t *rpc)
{
	reg_t reg = {0};

	R_EAX(reg) = VMWARE_MAGIC;
	R_ECX(reg) = VMCMD_INVOKE_RPC | VMRPC_CLOSE;
	R_EDX(reg) = rpc->channel | VMWARE_CMD_PORT;
	R_ESI(reg) = rpc->cookie1;
	R_EDI(reg) = rpc->cookie2;

	dump_reg_c(&reg);

	vmcall_cmd(&reg);

	dump_reg_r(&reg);

	if (R_EAX(reg)) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_cmd_fail, VMCMD_INVOKE_RPC | VMRPC_CLOSE);
		}
		return VMTOOL_CMD_ERROR;
	}

	if ((R_ECX(reg) >> 16) == 0) {
		if (vmtool_msg > vmtool_quiet) {
			fprintf(stderr, msg_rpc_fail, VMCMD_INVOKE_RPC | VMRPC_CLOSE);
		}
		return VMTOOL_RPC_ERROR;
	}

	return VMTOOL_SUCCESS;
}
