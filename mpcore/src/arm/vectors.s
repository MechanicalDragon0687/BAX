#include <asm.h>
#include <interrupt.h>
.align 2
.arm

@ TODO: implement tiny pages to map this to VA zero

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


.section .text.vectors
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
    ldr r2, =xrq_strings
    ldr r0, [r2, r0, lsl #2]
    mov r2, #XRQ_REG_COUNT
    ldr lr, =bugcheck
    bx lr



ASM_FUNCTION xrq_irq
    sub lr, lr, #4             @ Fix return address
    srsfd sp!, #SR_SYS         @ Store IRQ mode LR and SPSR on the SYS stack
    cps #SR_SYS                @ Switch to SVC mode
    push {r0-r3, r12, lr}      @ Preserve registers

    ldr lr, =0x17E00000
    ldr r0, [lr, #0x10C]       @ Get pending interrupt
    cmp r0, #IRQ_COUNT
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


.section .rodata.xrq_strings
xrq_reset_str:       .asciz "RESET"
xrq_undefined_str:   .asciz "UNDEFINED"
xrq_softwareint_str: .asciz "SOFTWARE INTERRUPT"
xrq_prefetchabt_str: .asciz "PREFETCH ABORT"
xrq_dataabt_str:     .asciz "DATA ABORT"
xrq_reserved_str:    .asciz "RESERVED"
xrq_irq_str:         .asciz "IRQ"
xrq_fiq_str:         .asciz "FIQ"

.global xrq_strings
xrq_strings:
    .word xrq_reset_str, xrq_undefined_str, xrq_softwareint_str, xrq_prefetchabt_str, \
          xrq_dataabt_str, xrq_reserved_str, xrq_irq_str, xrq_fiq_str
