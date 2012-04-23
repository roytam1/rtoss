/*
	vmcall.c
	VMware backdoor call inline assembly code
	for MSC, DMC, GNUC

	Copyright (c) 2006 Ken Kato

	Modeled on Nickolai Zeldovich's contrubution
*/

#include "vmcall.h"


#if defined(_MSC_VER) || defined(__DMC__)

/* inline assembly code for MSC or DMC (DOS or WIN) */

#ifdef _WIN32
/* param "reg_t *reg" and return address are 32 bit pointers */
#define mov_eax_reg()	__asm mov eax, 4[esp]
#else
/* param "reg_t *reg" and return address are 16 bit pointers */
#define mov_eax_reg()	__asm movzx eax, word ptr 2[esp]
#endif

#define load_regs()	__asm {				\
		__asm pushad					\
		__asm push eax					\
		__asm mov esi, 18h[eax]			\
		__asm mov edi, 14h[eax]			\
		__asm mov ebp, 10h[eax]			\
		__asm mov edx, 0ch[eax]			\
		__asm mov ecx, 08h[eax]			\
		__asm mov ebx, 04h[eax]			\
		__asm mov eax, 00h[eax]			\
}

#define store_regs() __asm {			\
		__asm xchg 00h[esp], eax		\
		__asm mov 18h[eax], esi			\
		__asm mov 14h[eax], edi			\
		__asm mov 10h[eax], ebp			\
		__asm mov 0ch[eax], edx			\
		__asm mov 08h[eax], ecx			\
		__asm mov 04h[eax], ebx			\
		__asm pop dword ptr 00h[eax]	\
		__asm popad						\
		__asm ret						\
}

__declspec(naked) void vmcall_cmd(reg_t *reg)
{
	mov_eax_reg();
	load_regs();

	__asm in eax, dx

	store_regs();
}

__declspec(naked) void vmcall_rpc_ins(reg_t *reg)
{
	mov_eax_reg();
	load_regs();

	__asm {
		pushf
		cld
		rep insb
		popf
	}

	store_regs()
}

__declspec(naked) void vmcall_rpc_outs(reg_t *reg)
{
	mov_eax_reg();
	load_regs();

	__asm {
		pushf
		cld
		rep outsb
		popf
	}

	store_regs()
}

#elif defined(__GNUC__)

/* inline assembly code for gcc */

#warning "use stand alone assembly source for gas (vmcall.gas.s) if possible."

/*
	For x86 target, gcc does not provide options to suppress generation of
	function prologue /	epilogue code, which is unnecessary in this case
	because all registers are preserved with pushal/popal instructions
*/

#define asm_op(opcode, reg) 		\
	__asm__ __volatile__ (			\
		"pushal;"					\
		"pushl %%eax;"				\
		"movl 0x18(%%eax), %%esi;"	\
		"movl 0x14(%%eax), %%edi;"	\
		"movl 0x10(%%eax), %%ebp;"	\
		"movl 0x0c(%%eax), %%edx;"	\
		"movl 0x08(%%eax), %%ecx;"	\
		"movl 0x04(%%eax), %%ebx;"	\
		"movl 0x00(%%eax), %%eax;"	\
		opcode						\
		"xchgl %%eax, 0x00(%%esp);"	\
		"movl %%esi, 0x18(%%eax);"	\
		"movl %%edi, 0x14(%%eax);"	\
		"movl %%ebp, 0x10(%%eax);"	\
		"movl %%edx, 0x0c(%%eax);"	\
		"movl %%ecx, 0x08(%%eax);"	\
		"movl %%ebx, 0x04(%%eax);"	\
		"popl 0x00(%%eax);"			\
		"popal;"					\
		::"a"(reg)					\
	);

void vmcall_cmd(reg_t *reg)
{
	asm_op("inl (%%dx);", reg);
}

void vmcall_rpc_ins(reg_t *reg)
{
	asm_op("pushf; cld; rep insb; popf;", reg);
}

void vmcall_rpc_outs(reg_t *reg)
{
	asm_op("pushf; cld; rep outsb; popf;", reg);
}

#else /* other compilers */
/*
	asm code must be provided in independent asm source file
*/
#error "No inline assembly code for the current compiler !!"

#endif /* end of backdoor access assembly code */
