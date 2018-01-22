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

xrq_undefined:
    XRQ_PRIMARY_HANDLER 1, xrq_fatal_handler

xrq_softwareint:
    bkpt

xrq_prefetchabt:
    XRQ_PRIMARY_HANDLER 3, xrq_fatal_handler

xrq_dataabt:
    XRQ_PRIMARY_HANDLER 4, xrq_fatal_handler

xrq_fiq:
    subs pc, lr, #4

xrq_fatal_handler:
    XRQ_SECONDARY_HANDLER
    b .



ASM_FUNCTION xrq_irq
    sub lr, lr, #4
    stmfd sp!, {r0-r3, r12, lr}  @ Preserve registers

    ldr lr, =0x10001000
    ldr r12, [lr, #4]            @ Pending IRQ bitmask

    clz r12, r12
    mov r3, #1
    rsb r0, r12, #31

    cmp r0, #29
    bhs 1f                       @ Invalid interrupt source

    lsl r3, r3, r0
    str r3, [lr, #4]             @ Acknowledge interrupt

    ldr lr, =_irq_handlers
    ldr r12, [lr, r0, lsl #2]

    cmp r12, #0
    blxne r12                    @ Execute handler

    1:
    ldmfd sp!, {r0-r3, r12, pc}^ @ Return from exception
