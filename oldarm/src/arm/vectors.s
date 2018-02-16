#include <asm.h>
#include <interrupt.h>
#include <pxicmd.h>
.align 2
.arm

.section .vectors, "ax"
.global vectors
vectors:
    ldr pc, =XRQ_Reset         @ Reset
    ldr pc, =XRQ_Undefined     @ Undefined
    ldr pc, =XRQ_SoftwareInt   @ Software Interrupt
    ldr pc, =XRQ_PrefetchAbort @ Prefetch Abort
    ldr pc, =XRQ_DataAbort     @ Data Abort
    b .                        @ Reserved
    ldr pc, =XRQ_IRQ           @ IRQ
    ldr pc, =XRQ_FIQ           @ FIQ
    .pool

XRQ_Reset:
    XRQ_PRIMARY_HANDLER 0, XRQ_StackTop, XRQ_FatalHandler

XRQ_Undefined:
    XRQ_PRIMARY_HANDLER 1, XRQ_StackTop, XRQ_FatalHandler

XRQ_SoftwareInt:
    bkpt

XRQ_PrefetchAbort:
    XRQ_PRIMARY_HANDLER 3, XRQ_StackTop, XRQ_FatalHandler

XRQ_DataAbort:
    XRQ_PRIMARY_HANDLER 4, XRQ_StackTop, XRQ_FatalHandler

XRQ_FIQ:
    subs pc, lr, #4

XRQ_FatalHandler:
    XRQ_SECONDARY_HANDLER
    mov r0, #PXICMD_ARM11_PANIC
    mov r1, #0
    mov r2, #0
    b .


.section .bss.XRQ_Stack
XRQ_Stack:
    .space (128 * 4)
XRQ_StackTop:


ASM_FUNCTION XRQ_IRQ
    sub lr, lr, #4
    stmfd sp!, {r0-r3, r12, lr}  @ Preserve registers

    1:
    ldr lr, =0x10001000
    ldr r12, [lr, #4]            @ Pending IRQ bitmask

    clz r12, r12
    rsb r0, r12, #31

    cmp r0, #IRQ_COUNT
    bhs 2f                       @ Invalid interrupt source

    mov r3, #1
    lsl r3, r3, r0
    str r3, [lr, #4]             @ Acknowledge interrupt

    ldr lr, =IRQ_Handlers
    ldr r12, [lr, r0, lsl #2]

    cmp r12, #0
    blxne r12                    @ Execute handler if registered

    b 1b                         @ Check if theres other pending interrupts

    2:
    ldmfd sp!, {r0-r3, r12, pc}^ @ Return from exception
