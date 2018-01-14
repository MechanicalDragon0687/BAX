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
    mov r0, #0xFFFFFFFF
    mov r1, #0x18000000
    mov r2, #0x00600000
    __xrq_dummy_clr:
        subs r2, r2, #4
        strpl r0, [r1, r2]
        bpl __xrq_dummy_clr
    b .


ASM_FUNCTION xrq_irq
    sub lr, lr, #4
    push {lr}
    mrs lr, spsr
    push {lr}

    tst lr, #0xF
    orreq lr, lr, #0xF         @ Came from user mode, go to system mode
    orr lr, lr, #(SR_I | SR_F) @ Disable interrupts
    bic lr, lr, #(SR_T)        @ Disable Thumb
    msr cpsr_c, lr
    push {r0-r5, r12, lr}

    bl irq_pending
    mov r4, r0
    bl irq_handler
    movs r1, r0
    movne r0, r4
    blxne r1
    mov r0, r4
    bl irq_ack

    pop {r0-r5, r12, lr}
    msr cpsr_c, #(SR_IRQ | SR_I | SR_F)

    pop {lr}
    msr spsr, lr
    ldmfd sp!, {pc}^
