//  vmcall.sco.s
//  VMware backdoor call functions assembly source for:
//      SCO OpenServer 5.0.x  native 'as': 'as -o vmcall.sco.o vmcall.sco.s'
//
//  Copyright (c) 2006 Ken kato
//  Adapted by John Morris
//
//  The following C struct is used as input/output parameter

//  typedef struct _reg {
//      uint8_t eax[4];
//      uint8_t ebx[4];
//      uint8_t ecx[4];
//      uint8_t edx[4];
//      uint8_t ebp[4];
//      uint8_t edi[4];
//      uint8_t esi[4];
//  } reg_t;

    .file   "vmcall.sco.s"
    .version "01.01"
// backdoor call with an 'in' instruction
vmcall_cmd:          // void vmcall_cmd(reg_t *);
    .def    vmcall_cmd;
        .scl    2;
        .type   044;
        .val    vmcall_cmd;
    .endef
    .globl  vmcall_cmd
    .align  4
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
    .def    vmcall_cmd;
        .scl    -1;
        .val     .;
    .endef

// backdoor rpc data transfer with 'rep insb' instruction
vmcall_rpc_ins:         // void vmcall_rpc_ins(reg_t *);
    .def    vmcall_rpc_ins;
        .scl    2;
        .type   044;
        .val    vmcall_rpc_ins;
    .endef
    .globl  vmcall_rpc_ins;
    .align  4
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
        rep
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
    .def    vmcall_rpc_ins;
        .scl    -1;
        .val     .;
    .endef

// backdoor rpc data transfer with 'rep outsb' instruction
vmcall_rpc_outs:        // void vmcall_rpc_outs(reg_t *);
    .def    vmcall_rpc_outs;
        .scl    2;
        .type   044;
        .val    vmcall_rpc_outs;
    .endef
    .globl  vmcall_rpc_outs;
    .align  4
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
        rep
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
    .def    vmcall_rpc_outs;
        .scl    -1;
        .val     .;
    .endef
