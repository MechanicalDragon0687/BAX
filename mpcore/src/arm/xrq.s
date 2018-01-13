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
    XRQ_FATAL_HANDLER _bugcheck_xrq


ASM_FUNCTION xrq_irq
    sub lr, lr, #4         @ Fix return address
    srsfd sp!, #SR_SVC     @ Store IRQ mode LR and SPSR on the SVC stack
    cps #SR_SVC            @ Switch to SVC mode
    push {r0-r5, r12, lr}  @ Preserve registers

    bl irq_pending         @ Get any pending interrupts
    mov r4, r0
    bl irq_handler
    movs r1, r0
    movne r0, r4
    blxne r1               @ Run the interrupt handler
    mov r0, r4
    bl irq_ack

    pop {r0-r5, r12, lr}   @ Restore registers
    rfeia sp!              @ Return From Exception
