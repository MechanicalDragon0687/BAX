#include <asm.h>
#include <mmap.h>
#include <interrupt.h>

.global vectors_s
.align 2
.arm
vectors_s:
    @ 0x1FFFFFA0
    ldr pc, [pc, #-4]
    .word XRQ_IRQ
    ldr pc, [pc, #-4]
    .word XRQ_FIQ
    ldr pc, [pc, #-4]
    .word XRQ_SoftwareInt
    ldr pc, [pc, #-4]
    .word XRQ_Undefined
    ldr pc, [pc, #-4]
    .word XRQ_PrefetchAbort
    ldr pc, [pc, #-4]
    .word XRQ_DataAbort
.global vectors_e
vectors_e:


.section .text.vectors
XRQ_Undefined:
    XRQ_PRIMARY_HANDLER 1, __stack_abt, XRQ_FatalHandler

XRQ_SoftwareInt:
    bkpt

XRQ_PrefetchAbort:
    XRQ_PRIMARY_HANDLER 3, __stack_abt, XRQ_FatalHandler

XRQ_DataAbort:
    XRQ_PRIMARY_HANDLER 4, __stack_abt, XRQ_FatalHandler

XRQ_FIQ:
    subs pc, lr, #4

XRQ_FatalHandler:
    XRQ_SECONDARY_HANDLER

    ldr r3, =XRQ_FatalStrings
    add r0, r3, r0, lsl #2
    mov r2, r1
    mov r1, #1
    mov r3, #XRQ_REG_COUNT
    ldr r12, =BUG
    bx r12



ASM_FUNCTION XRQ_IRQ
    sub lr, lr, #4             @ Fix return address
    srsfd sp!, #SR_SYS         @ Store IRQ mode LR and SPSR on the SYS stack
    cps #SR_SYS                @ Switch to SVC mode
    push {r0-r3, r12, lr}      @ Preserve registers

    1:
    ldr lr, =MPCORE_PMR
    ldr r0, [lr, #0x10C]       @ Get pending interrupt

    ldr r1, =IRQ_SPURIOUS
    cmp r0, r1
    beq 3f                     @ Spurious interrupt, no interrupts pending

    cmp r0, #IRQ_COUNT
    bhs 2f                     @ Invalid interrupt ID

    cmp r0, #32

    @ Local CPU interrupt
    mrclo p15, 0, lr, c0, c0, 5
    andlo lr, lr, #3
    ldrlo r12, =IRQ_LocalHandlers
    addlo r12, r12, lr, lsl #5

    @ Hardware interrupt
    ldrhs r12, =(IRQ_GlobalHandlers - 32*4)

    ldr r12, [r12, r0, lsl #2]
    cmp r12, #0
    beq 2f

    push {r0, r12}
    blx r12
    pop {r0, r12}

    2:
    ldr lr, =MPCORE_PMR
    str r0, [lr, #0x110]       @ End of interrupt
    b 1b                       @ Check for any other pending interrupts

    3:
    pop {r0-r3, r12, lr}       @ Restore registers
    rfeia sp!                  @ Return From Exception


.section .rodata.XRQ_FatalStrings
XRQ_ResetString:         .asciz "RESET"
XRQ_UndefinedString:     .asciz "UNDEFINED"
XRQ_SoftwareIntString:   .asciz "SOFT INTERRUPT"
XRQ_PrefetchAbortString: .asciz "PREFETCH ABORT"
XRQ_DataAbortString:     .asciz "DATA ABORT"
XRQ_ReservedString:      .asciz "RESERVED"
XRQ_IRQString:           .asciz "IRQ"
XRQ_FIQString:           .asciz "FIQ"

.global XRQ_FatalStrings
XRQ_FatalStrings:
    .word XRQ_ResetString, XRQ_UndefinedString, XRQ_SoftwareIntString, \
          XRQ_PrefetchAbortString, XRQ_DataAbortString, XRQ_ReservedString, \
          XRQ_IRQString, XRQ_FIQString
