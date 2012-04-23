!  vmcalls.mnx.s
!  VMware backdoor call functions assembly source for:
!      Minix native assembler 'as': 'as -o vmcall.mnx.o vmcalls.mnx.s'
!
!  Copyright (c) 2006 Ken kato
!  Based on Ken'ichi Takahagi's contribution
!
!  The following C struct is used as input/output parameter
!
!   typedef struct _reg {
!       uint8_t eax[4];
!       uint8_t ebx[4];
!       uint8_t ecx[4];
!       uint8_t edx[4];
!       uint8_t ebp[4];
!       uint8_t edi[4];
!       uint8_t esi[4];
!   } reg_t;

.sect .text

! void vmcall_cmd(reg_t *)
.define _vmcall_cmd
_vmcall_cmd:
    pusha
    mov     eax, 0x24(esp)
    push    eax
    mov     esi, 0x18(eax)
    mov     edi, 0x14(eax)
    mov     ebp, 0x10(eax)
    mov     edx, 0x0c(eax)
    mov     ecx, 0x08(eax)
    mov     ebx, 0x04(eax)
    mov     eax, 0x00(eax)
    in      dx
    xchg    0x00(esp), eax
    mov     0x18(eax), esi
    mov     0x14(eax), edi
    mov     0x10(eax), ebp
    mov     0x0c(eax), edx
    mov     0x08(eax), ecx
    mov     0x04(eax), ebx
    pop     0x00(eax)
    popa
    ret

! void _vmcall_rpc_ins(reg_t *)
.define _vmcall_rpc_ins
_vmcall_rpc_ins:
    pusha
    mov     eax, 0x24(esp)
    push    eax
    mov     esi, 0x18(eax)
    mov     edi, 0x14(eax)
    mov     ebp, 0x10(eax)
    mov     edx, 0x0c(eax)
    mov     ecx, 0x08(eax)
    mov     ebx, 0x04(eax)
    mov     eax, 0x00(eax)
    pushf
    cld
    rep insb
    popf
    xchg    0x00(esp), eax
    mov     0x18(eax), esi
    mov     0x14(eax), edi
    mov     0x10(eax), ebp
    mov     0x0c(eax), edx
    mov     0x08(eax), ecx
    mov     0x04(eax), ebx
    pop     0x00(eax)
    popa
    ret

! void _vmcall_rpc_outs(reg_t *)
.define _vmcall_rpc_outs
_vmcall_rpc_outs:
    pusha
    mov     eax, 0x24(esp)
    push    eax
    mov     esi, 0x18(eax)
    mov     edi, 0x14(eax)
    mov     ebp, 0x10(eax)
    mov     edx, 0x0c(eax)
    mov     ecx, 0x08(eax)
    mov     ebx, 0x04(eax)
    mov     eax, 0x00(eax)
    pushf
    cld
    rep outsb
    popf
    xchg    0x00(esp), eax
    mov     0x18(eax), esi
    mov     0x14(eax), edi
    mov     0x10(eax), ebp
    mov     0x0c(eax), edx
    mov     0x08(eax), ecx
    mov     0x04(eax), ebx
    pop     0x00(eax)
    popa
    ret
