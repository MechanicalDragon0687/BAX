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
    ldr pc, _abort_h  @ UNDEFINED INSTRUCTION
    subs pc, lr, #4   @ SOFTWARE INTERRUPT (unhandled)
    ldr pc, _abort_h  @ PREFETCH ABORT
    ldr pc, _abort_h  @ DATA ABORT
    subs pc, lr, #4   @ RESERVED (unhandled)
    subs pc, lr, #4   @ IRQ (unhandled)
    subs pc, lr, #4   @ FIQ (unhandled)

    _reset_h: .word _main_
    _abort_h: .word abrt_handler
ex_vector_table_end:

exception_regs:
.skip (18*4) @ preallocate space for 18 words

.arm
abrt_handler:
    ldr sp, =exception_regs
    stmia sp!, {r0-r12} @ store r0-r12

    @ Get previous state
    mrs r0, cpsr @ save current pstatus
    mrs r1, spsr @ save previous pstatus
    mov r6, r1   @ make a copy
    mov r7, r0

    orr r1, #0xC0 @ disable IRQ/FIQs
    bic r1, #0x20 @ force ARM mode

    and r1, r1, #0x1F   @ get processor mode bits of previous pstatus
    cmp r1, #0x10       @ came from user
    orreq r1, r6, #0x1F @ set to system

    msr cpsr_c, r1 @ switch to the previous pstatus
    mov r3, sp     @ save sp
    mov r4, lr     @ save lr
    msr cpsr_c, r0 @ switch back to the handler

    mov r5, lr     @ get pc

    stmia sp!, {r3-r7}  @ store sp, lr, pc, spsr and cpsr

    sub r0, sp, #(18*4)
    ldr sp, =__abt_stack

    blx print_registers

.pool
