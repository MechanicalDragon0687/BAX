.section .text.boot
.arm

#include <arm.h>

.global __boot
__boot:
    cpsid aif, #(SR_IRQ_MODE)

    mov r0, #0
    mcr p15, 0, r0, c7, c7, 0
    mcr p15, 0, r0, c7, c14, 0
    mcr p15, 0, r0, c7, c10, 4

    ldr sp, =_irq_stack

    cpsid aif, #(SR_SVC_MODE)
    ldr sp, =_prg_stack

    ldr r0, =0x00054078
    ldr r1, =0x0000000F
    ldr r2, =0x00000000

    mcr p15, 0, r0, c1, c0, 0
    mcr p15, 0, r1, c1, c0, 1
    mcr p15, 0, r2, c1, c0, 2

    @ Notify oldarm
    mov r0, #0x20000000
    mov r1, #0
    str r1, [r0, #-4]

    @ Setup heap
    ldr r0, =fake_heap_start
    ldr r1, =fake_heap_end
    ldr r2, =0x24000000
    ldr r3, =0x28000000
    str r2, [r0]
    str r3, [r1]

    bl gic_reset

    bl main

    b __boot
