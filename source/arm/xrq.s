.section .itcm.start, "a"
.arm

#include <arm/arm.h>

.align 4
.global xrq_entry
.type xrq_entry, %function
xrq_entry:
    b xrq_reset
    b xrq_undefined
    b xrq_swi
    b xrq_prefetch_abort
    b xrq_data_abort
    b xrq_reserved
    b xrq_irq
    b xrq_fiq

.macro XRQ_FATAL_HANDLER xrq_id=0
    ldr sp, =__reg_stack
    stmia sp!, {r0-r12}
    mov r0, #\xrq_id
    b fatal_xrq_handler_common
.endm

xrq_reset:
    XRQ_FATAL_HANDLER 0

xrq_undefined:
    XRQ_FATAL_HANDLER 1

xrq_swi:
    XRQ_FATAL_HANDLER 2

xrq_prefetch_abort:
    XRQ_FATAL_HANDLER 3

xrq_data_abort:
    XRQ_FATAL_HANDLER 4

xrq_reserved:
    XRQ_FATAL_HANDLER 5

xrq_fiq:
    XRQ_FATAL_HANDLER 7

@ note: r0 MUST be preserved
fatal_xrq_handler_common:

    @ Disable interrupts
    mrs r1, cpsr
    orr r2, r1, #(SR_IRQ_BIT | SR_FIQ_BIT)
    msr cpsr, r2

    @ Keep copies of CPSR and SPSR
    mrs r2, spsr
    mov r7, r1
    mov r6, r2

    @ Fix previous mode
    and r2, #(SR_PMODE_MASK)
    cmp r2, #(SR_USR_MODE)
    orreq r2, r6, #(SR_SYS_MODE)       @ if User mode, select System mode
    orr r2, #(SR_IRQ_BIT | SR_FIQ_BIT) @ disable IRQs

    @ Recover previous SP and LR
    msr cpsr, r2
    mov r3, sp
    mov r4, lr
    msr cpsr, r1

    @ Fix PC
    sub r5, lr, #4

    stmia sp!, {r3-r7}

    @ Preserved register order
    @ r0-r12, sp, lr, cpsr, xpsr
    ldr sp, =_abt_stack
    ldr r3, =xrq_fatal
    ldr r1, =__reg_stack
    blx r3

    @ Wait For Interrupt infinite loop
    msr cpsr_c, #(SR_SVC_MODE | SR_IRQ_BIT | SR_FIQ_BIT)
    mov r0, #0
    .xrq_wfi_loop:
        mcr p15, 0, r0, c7, c0, 4
        b .xrq_wfi_loop

.section .dtcm
__reg_stack:
    .skip (18*4)
