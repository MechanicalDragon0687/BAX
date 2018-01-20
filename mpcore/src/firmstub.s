#include <asm.h>
.align 2
.arm

ASM_FUNCTION firmstub
    cpsid aif
    clrex

    mov r0, #0
    mcr p15, 0, r0, c7, c10, 0
    mcr p15, 0, r0, c7, c7, 0
    mcr p15, 0, r0, c7, c10, 4
    mcr p15, 0, r0, c7, c10, 5
    mcr p15, 0, r0, c7, c5, 4

    mov r0, #0x54000
    orr r0, r0, #0x78
    mcr p15, 0, r0, c1, c0, 0

    NOP_SLED 3
    mov r0, #0x1FFFFFFC
    1:
        ldr r1, [r0]
        cmp r1, #0
        bxne r1
        b 1b
firmstub_e:

.global firmstub_len
firmstub_len:
    .word (firmstub_e - firmstub)
