.section .text.boot
.arm

.global __boot
__boot:
    nop
    nop
    nop

    cpsid aif, #0x13

    @ Writeback and invalidate caches
    mov r0, #0
    mcr p15, 0, r0, c7, c7, 0
    mcr p15, 0, r0, c7, c14, 0
    mcr p15, 0, r0, c7, c10, 4

    ldr sp, =_stack_top

    @ Reset values
    ldr r0, =0x00054078
    ldr r1, =0x0000000F
    ldr r2, =0x00000000
    mcr p15, 0, r0, c1, c0, 0
    mcr p15, 0, r1, c1, c0, 1
    mcr p15, 0, r2, c1, c0, 2

    bl clear_bss

    bl main

    b __boot

clear_bss:
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r2, #0
    .clear_bss_:
    cmp r0, r1
        strlt r2, [r0], #4
        blt .clear_bss_
    bx lr
