.arm
.type install_exception_vectors, %function
.global install_exception_vectors
install_exception_vectors:
    ldr r0, =0x00000000
    ldr r1, =ex_vector_table
    ldr r2, =ex_vector_table_end
    ex_vec_loop:
        ldr r3, [r1], #4
        str r3, [r0], #4
        cmp r1, r2
        ble ex_vec_loop
    bx lr

ex_vector_table:
    ldr pc, _reset_h  @ RESET
    ldr pc, _und_h    @ UNDEFINED INSTRUCTION
    subs pc, lr, #4   @ SOFTWARE INTERRUPT (unhandled)
    ldr pc, _pre_h    @ PREFETCH ABORT
    ldr pc, _dat_h    @ DATA ABORT
    subs pc, lr, #4   @ RESERVED (unhandled)
    subs pc, lr, #4   @ IRQ (unhandled)
    subs pc, lr, #4   @ FIQ (unhandled)

    _reset_h: .word _main_
    _und_h:
    _pre_h:
    _dat_h:   .word abrt_handler
ex_vector_table_end:

exception_regs:
.skip (20*4) // space for 18 registers + 2 words

abrt_handler:
    ldr sp, =exception_regs
    stmia sp!, {r0-r12} @ store r0-r12

    @ Get previous state
    mrs r0, cpsr @ save current pstatus
    mrs r1, spsr @ save previous pstatus
    mov r6, r1   @ make a copy
    mov r7, r0

    orr r1, #0xC0 @ disable IRQ/FIQ
    bic r1, #0x20 @ force ARM mode

    msr cpsr_c, r1 @ switch to the previous pstatus
    mov r3, sp     @ save sp
    mov r4, lr     @ save lr
    msr cpsr_c, r0 @ switch back to the handler

    mov r5, lr     @ get pc

    stmia sp!, {r3-r7}  @ store sp, lr, pc and cpsr of the previous mode

    sub r0, sp, #(18*4) @ pass the start of regs
    mov sp, r3

    blx print_registers

    b .

