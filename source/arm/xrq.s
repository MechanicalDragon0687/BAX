.arm
.section .itcm.start, "a"

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

xrq_wfi:
    msr cpsr, #0xD3
    mov r0, #0
    .xrq_wfi_loop:
        mcr p15, 0, r0, c7, c0, 4
        b .xrq_wfi_loop


.macro XRQ_FATAL_HANDLER xrq_id=0
    ldr sp, =(0x23F00000 - 18*4)
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

fatal_xrq_handler_common:
    mrs r1, cpsr
    orr r1, #0xC0
    msr cpsr, r1

    mrs r2, spsr
    mov r7, r1
    mov r6, r2

    and r2, #0x1F
    cmp r2, #0x10
    orreq r2, r6, #0x1F

    orr r2, #0xC0

    msr cpsr, r2
    mov r3, sp
    mov r4, lr
    msr cpsr, r1

    sub r5, lr, #4

    stmia sp!, {r3-r7}

    sub sp, #(18*4)

    ldr lr, =fatal_xrq
    mov r1, sp

    @ preserved register order
    @ r0-r15, cpsr, xpsr
    blx lr

    bl xrq_wfi
