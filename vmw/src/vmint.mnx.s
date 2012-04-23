! vmint.mnx.s
! 64 bit integer operation for minix native assembler
! Copyright (c) 2006 Ken Kato

.sect .text

! void u64add32(uint64_t *r64, uint64_t v64, uint32_t v32);
! *r64 = v64 + v32
.define _u64add32
_u64add32:
    mov edx, 0x04(esp)

    mov eax, 0x08(esp)
    add eax, 0x10(esp)
    mov 0x00(edx), eax

    mov eax, 0x0c(esp)
    adc eax, 0x00
    mov 0x04(edx), eax

    ret

! void u64add64(uint64_t *r64, uint64_t v64a, uint64_t v64b);
! *r64 = v64 + v64
.define _u64add64
_u64add64:
    mov edx, 0x04(esp)

    mov eax, 0x08(esp)
    add eax, 0x10(esp)
    mov 0x00(edx), eax

    mov eax, 0x0c(esp)
    adc eax, 0x14(esp)
    mov 0x04(edx), eax

    ret

! void u64sub32(uint64_t *r64, uint64_t v64, uint32_t v32);
! *r64 = v64 - v32
.define _u64sub32
_u64sub32:
    mov edx, 0x04(esp)

    mov eax, 0x08(esp)
    sub eax, 0x10(esp)
    mov 0x00(edx), eax

    mov eax, 0x0c(esp)
    sbb eax, 0x00
    mov 0x04(edx), eax

    ret

! void u64sub64(uint64_t *r64, uint64_t v64a, uint64_t v64a);
! *r64 = v64a - v64b
.define _u64sub64
_u64sub64:
    mov edx, 0x04(esp)

    mov eax, 0x08(esp)
    sub eax, 0x10(esp)
    mov 0x00(edx), eax

    mov eax, 0x0c(esp)
    sbb eax, 0x14(esp)
    mov 0x04(edx), eax

    ret

! void u64mul32(uint64_t *r64, uint32_t v32a, uint32_t v32b);
! *r64 = v32a * v32b
.define _u64mul32
_u64mul32:
    mov eax, 0x08(esp)
    imul eax, 0x0c(esp)

    mov ecx, 0x04(esp)
    mov 0x00(ecx), eax
    mov 0x04(ecx), edx

    ret

! void u64div32(uint64_t *q64, uint32_t *r32, uint64_t v64, uint32_t v32);
! (*r32 = v64 % v32), (*q64 = v64 / v32)
.define _u64div32
_u64div32:
    push ebx
    mov ebx,0x08(esp)

    mov eax,0x14(esp)
    xor edx,edx
    mov ecx,0x18(esp)
    div ecx
    mov 0x04(ebx),eax

    mov eax,0x10(esp)
    div ecx
    mov 0x00(ebx),eax

    mov ebx,0x0c(esp)
    mov 0x00(ebx),edx
    pop ebx
    ret
