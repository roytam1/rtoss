/*
	vmcall.h
	VMware backdoor call header

	Copyright (c) 2006 Ken Kato
*/

#ifndef _VMCALL_H_
#define _VMCALL_H_

#include "vmint.h"

/*
	data struct for register values

	DO NOT change the order of members because it is hard-coded
	in the assembly portion of the code

	use uint8_t[4] for members instead of uint32_t in order to
	avoid dealing with compiler's struct alignment
*/

typedef struct _reg {
	uint8_t eax[4];
	uint8_t ebx[4];
	uint8_t ecx[4];
	uint8_t edx[4];
	uint8_t ebp[4];
	uint8_t edi[4];
	uint8_t esi[4];
} reg_t;

/* member accessing macros */

#define R_EAX(r)	*((uint32_t *)&((r).eax))
#define R_EBX(r)	*((uint32_t *)&((r).ebx))
#define R_ECX(r)	*((uint32_t *)&((r).ecx))
#define R_EDX(r)	*((uint32_t *)&((r).edx))
#define R_EBP(r)	*((uint32_t *)&((r).ebp))
#define R_EDI(r)	*((uint32_t *)&((r).edi))
#define R_ESI(r)	*((uint32_t *)&((r).esi))

/*
	accesses VMware backdoor I/O ports
 */
#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/*	access command port with 'in' instruction */
void vmcall_cmd(reg_t *reg);

/*	access enhanced rpc data port with 'ins' instruction */
void vmcall_rpc_ins(reg_t *reg);

/*	access enhanced rpc data port with 'outs' instruction */
void vmcall_rpc_outs(reg_t *reg);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* _VMCALL_H_ */
