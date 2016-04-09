.section .text.start
.align 4
.org 0x2B0
.global _start
_start:

    @ Fix mounting of SDMC
    ldr r0, =0x10000020
    mov r1, #0x340
    str r1, [r0]

    bl main

.die:
    b .die
