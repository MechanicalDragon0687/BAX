#include <asm.h>
.align 2
.arm

.section .vectors, "ax"
.global vectors
vectors:
    ldr pc, =start            @ Reset
    ldr pc, =xrq_undefined    @ Undefined
    ldr pc, =xrq_softwareint  @ Software Interrupt
    ldr pc, =xrq_prefetchabt  @ Prefetch Abort
    ldr pc, =xrq_dataabt      @ Data Abort
    b .                       @ Reserved
    ldr pc, =xrq_irq          @ IRQ
    ldr pc, =xrq_fiq          @ FIQ
    .pool
