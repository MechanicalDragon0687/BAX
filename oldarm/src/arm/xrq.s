#include <asm.h>
.arm


ASM_FUNCTION xrq_undefined
    XRQ_FATAL 1, xrq_fatal_handler

ASM_FUNCTION xrq_softwareint
    bkpt

ASM_FUNCTION xrq_prefetchabt
    XRQ_FATAL 3, xrq_fatal_handler

ASM_FUNCTION xrq_dataabt
    XRQ_FATAL 4, xrq_fatal_handler

ASM_FUNCTION xrq_fiq
    subs pc, lr, #4

ASM_FUNCTION xrq_fatal_handler
    XRQ_FATAL_HANDLER __xrq_dummy

ASM_FUNCTION __xrq_dummy
    ldr r0, =0xF800001F
    mov r1, #0x18000000
    mov r2, #0x00600000
    1:
        subs r2, r2, #4
        strpl r0, [r1, r2]
        bpl 1b
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
