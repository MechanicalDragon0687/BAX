.section .text.boot
.arm

.global __boot
__boot:
    cpsid aif, #0x13

    mov r0, #0
    mcr p15, 0, r0, c7, c7, 0
    mcr p15, 0, r0, c7, c14, 0
    mcr p15, 0, r0, c7, c10, 4

    ldr sp, =_stack_top

    ldr r0, =0x00054078
    ldr r1, =0x0000000F
    ldr r2, =0x00000000

    mcr p15, 0, r0, c1, c0, 0
    mcr p15, 0, r1, c1, c0, 1
    mcr p15, 0, r2, c1, c0, 2

    mov r0, #0x20000000
    mov r1, #0
    str r1, [r0, #-4]

    bl main

    b __boot
