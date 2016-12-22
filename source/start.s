.section .text.start
.align 4

.global _main_
_main_:

    bl disable_irqs     @ in case we come from an entrypoint which leaves them enabled

	bl flush_all_caches @ flush any leftover data
	bl mpu_disable      @ disable the MPU in order to set it up

    bl clear_bss

    mov r0, #0x13
    bl switch_pmode     @ switch to SVC mode (supervisor/privileged)

    bl setup_stacks     @ set up svc/abt/irq/und stacks

    bl install_exception_vectors

	bl mpu_setup        @ set up the MPU (arm/mpu.s)
	bl mpu_enable

    bl main             @ jump to the main entrypoint

    bkpt                @ generate exception

clear_bss:
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
    clr_bss_loop:
        ldr r2, [r0], #4
        cmp r0, r1
        blt clr_bss_loop
    bx lr

switch_pmode:
    mrs r1, cpsr
    bic r1, r1, #0x1F
    orr r1, r0, r1
    msr cpsr_c, r1
    bx lr

setup_stacks:
    @ Set up the stacks for all CPU modes
    @ start by clearing mode bits
    mrs r0, cpsr
    mov r2, r0         @ save current mode
    bic r0, r0, #0x1F  @ clear the processor mode bits

    @ SVC Mode
    orr r1, r0, #0x13
    msr cpsr_c, r1
    ldr sp, =__svc_stack_start

    @ ABT Mode
    orr r1, r0, #0x17
    msr cpsr_c, r1
    ldr sp, =__abt_stack_start

    @ IRQ Mode
    orr r1, r0, #0x12
    msr cpsr_c, r1
    ldr sp, =__svc_stack_start

    @ UND Mode
    orr r1, r0, #0x1B
    msr cpsr_c, r1
    ldr sp, =__und_stack_start

    @ Restore previous mode
    msr cpsr_c, r2

    bx lr
