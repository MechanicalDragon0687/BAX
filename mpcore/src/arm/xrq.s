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
    sub lr, lr, #4             @ Fix return address
    srsfd sp!, #SR_SYS         @ Store IRQ mode LR and SPSR on the SYS stack
    cps #SR_SYS                @ Switch to SVC mode
    push {r0-r3, r12, lr}      @ Preserve registers

    ldr lr, =0x17E00000
    ldr r0, [lr, #0x10C]       @ Get any pending interrupts
    cmp r0, #128
    bhs 1f                     @ Invalid interrupt ID

    cmp r0, #32

    @ Local CPU interrupt
    mrclo p15, 0, lr, c0, c0, 5
    andlo lr, lr, #3
    ldrlo r12, =_lirq_handlers
    addlo r12, r12, lr, lsl #5

    @ Hardware interrupt
    ldrhs r12, =(_irq_handlers - 32*4)

    ldr r12, [r12, r0, lsl #2]
    cmp r12, #0

    push {r0, r12}
    blxne r12
    pop {r0, r12}

    1:
    ldr lr, =0x17E00000
    str r0, [lr, #0x110]       @ End of interrupt

    pop {r0-r3, r12, lr}       @ Restore registers
    rfeia sp!                  @ Return From Exception
