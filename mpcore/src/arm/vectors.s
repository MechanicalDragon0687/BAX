#include <asm.h>
.align 2
.arm

@ TODO: tiny pages to map this to zero page

.global vectors_s
vectors_s:
    @ 0x1FFFFFA0
    ldr pc, [pc, #-4]
    .word xrq_irq
    ldr pc, [pc, #-4]
    .word xrq_fiq
    ldr pc, [pc, #-4]
    .word xrq_softwareint
    ldr pc, [pc, #-4]
    .word xrq_undefined
    ldr pc, [pc, #-4]
    .word xrq_prefetchabt
    ldr pc, [pc, #-4]
    .word xrq_dataabt
.global vectors_e
vectors_e:
