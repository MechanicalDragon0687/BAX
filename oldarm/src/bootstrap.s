#include <asm.h>
.align 2
.arm

.section .bootstrap, "ax"
.global bootstrap
bootstrap:
    @ Switch to supervisor mode and disable interrupts
    msr cpsr_c, #(SR_SVC | SR_I | SR_F)


    @ Disable the MPU, caches, TCMs, set high exception vectors
    ldr r4, =0x2078
    mcr p15, 0, r4, c1, c0, 0
    NOP_SLED 4


    @ Cache coherency stuff
    mov r4, #0
    mcr p15, 0, r4, c7, c5, 0
    mcr p15, 0, r4, c7, c6, 0
    mcr p15, 0, r4, c7, c10, 4


    @ Setup Tightly Coupled Memory
    ldr r4, =0x3000000A @ DTCM @ 0x30000000 / 16KB (16KB)
    ldr r5, =0x00000024 @ ITCM @ 0x00000000 / 32KB (128MB)
    mcr p15, 0, r4, c9, c1, 0
    mcr p15, 0, r5, c9, c1, 1
    NOP_SLED 4


    @ Enable TCMs
    mrc p15, 0, r4, c1, c0, 0
    orr r4, r4, #0x50000
    mcr p15, 0, r4, c1, c0, 0
    NOP_SLED 4


    @ Relocate .vectors
    ldr r4, =__vectors_lma
    ldr r5, =__vectors_s
    ldr r6, =__vectors_e
    bl bootstrap_copy


    @ Relocate .text
    ldr r4, =__text_lma
    ldr r5, =__text_s
    ldr r6, =__text_e
    bl bootstrap_copy


    @ Relocate .data
    ldr r4, =__data_lma
    ldr r5, =__data_s
    ldr r6, =__data_e
    bl bootstrap_copy


    @ Branch to code
    ldr r4, =start
    bx r4


@ R4 = LMA
@ R5 = VMA
@ R6 = VMA_end
bootstrap_copy:
    cmp r5, r6
    ldrlo r7, [r4], #4
    strlo r7, [r5], #4
    blo bootstrap_copy

    bx lr
