/*  vmcall.gas.s                                                        */
/*  VMware backdoor call functions assembly source for                  */
/*  GNU assembler, Sun Solaris native assembler, etc.                   */
/*      'as -o vmcall.gas.o vmcall.gas.s'                               */
/*                                                                      */
/*  Copyright (c) 2006 Ken Kato                                         */

/*  The following C struct is used as input/output parameter            */
/*  typedef struct _reg {                                               */
/*      uint8_t eax[4];                                                 */
/*      uint8_t ebx[4];                                                 */
/*      uint8_t ecx[4];                                                 */
/*      uint8_t edx[4];                                                 */
/*      uint8_t ebp[4];                                                 */
/*      uint8_t edi[4];                                                 */
/*      uint8_t esi[4];                                                 */
/*  } reg_t;                                                            */

.globl  vmcall_cmd
.globl  vmcall_rpc_ins
.globl  vmcall_rpc_outs

/* backdoor call with an 'in' instruction                               */
/* void vmcall_cmd(reg_t *);                                            */
vmcall_cmd:
    pushal
    movl    0x24(%esp), %eax
    pushl   %eax
    movl    0x18(%eax), %esi
    movl    0x14(%eax), %edi
    movl    0x10(%eax), %ebp
    movl    0x0c(%eax), %edx
    movl    0x08(%eax), %ecx
    movl    0x04(%eax), %ebx
    movl    0x00(%eax), %eax
    inl     (%dx)
    xchgl   %eax, 0x00(%esp)
    movl    %esi, 0x18(%eax)
    movl    %edi, 0x14(%eax)
    movl    %ebp, 0x10(%eax)
    movl    %edx, 0x0c(%eax)
    movl    %ecx, 0x08(%eax)
    movl    %ebx, 0x04(%eax)
    popl    0x00(%eax)
    popal
    ret

/* backdoor rpc data transfer with 'rep insb' instruction               */
/* void vmcall_rpc_ins(reg_t *);                                        */
vmcall_rpc_ins:
    pushal
    movl    0x24(%esp), %eax
    pushl   %eax
    movl    0x18(%eax), %esi
    movl    0x14(%eax), %edi
    movl    0x10(%eax), %ebp
    movl    0x0c(%eax), %edx
    movl    0x08(%eax), %ecx
    movl    0x04(%eax), %ebx
    movl    0x00(%eax), %eax
    pushf
    cld
    rep     /* some assemblers require 'rep' to be in a separate line   */
    insb
    popf
    xchgl   %eax, 0x00(%esp)
    movl    %esi, 0x18(%eax)
    movl    %edi, 0x14(%eax)
    movl    %ebp, 0x10(%eax)
    movl    %edx, 0x0c(%eax)
    movl    %ecx, 0x08(%eax)
    movl    %ebx, 0x04(%eax)
    popl    0x00(%eax)
    popal
    ret

/* backdoor rpc data transfer with 'rep outsb' instruction              */
/* void vmcall_rpc_outs(reg_t *);                                       */
vmcall_rpc_outs:
    pushal
    movl    0x24(%esp), %eax
    pushl   %eax
    movl    0x18(%eax), %esi
    movl    0x14(%eax), %edi
    movl    0x10(%eax), %ebp
    movl    0x0c(%eax), %edx
    movl    0x08(%eax), %ecx
    movl    0x04(%eax), %ebx
    movl    0x00(%eax), %eax
    pushf
    cld
    rep     /* some assemblers require 'rep' to be in a separate line   */
    outsb
    popf
    xchgl   %eax, 0x00(%esp)
    movl    %esi, 0x18(%eax)
    movl    %edi, 0x14(%eax)
    movl    %ebp, 0x10(%eax)
    movl    %edx, 0x0c(%eax)
    movl    %ecx, 0x08(%eax)
    movl    %ebx, 0x04(%eax)
    popl    0x00(%eax)
    popal
    ret
