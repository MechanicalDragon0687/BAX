.section .text.start
.align 4

.arm
.global _main_
_main_:
    bl disable_irqs      @ in case the entrypoint didnt do it for us...

	bl flush_all_caches
	bl mpu_disable
    bl flush_all_caches  @ just in case

    bl clear_bss

    mrs r1, cpsr
    orr r1, r1, #0x1F
    msr cpsr_c, r1       @ switch over to system mode

    bl setup_stacks      @ set up sys/abt/und stacks

	bl mpu_setup         @ set up MPU
	bl mpu_enable        @ enable MPU/ITCM/DTCM/etc

    bl install_exception_vectors

    ldr r0, =0x10000020
    mov r1, #0x340
    strh r1, [r0]        @ fix SD mounting issue

    bl flush_all_caches  @ caches are nightmare inducing

    bl bax_main         @ jump to the main code

    mvn ip, #0
    bkpt                 @ should never get here


clear_bss:
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
    clr_bss_loop:
        str r2, [r0], #4
        cmp r0, r1
        blt clr_bss_loop
    bx lr


setup_stacks:
    @ Set up the stacks for all CPU modes
    @ start by clearing mode bits
    mrs r0, cpsr
    mov r1, r0         @ save current mode
    bic r0, r0, #0x1F  @ clear the processor mode bits

    @ SYS Mode
    orr r2, r0, #0x1F
    msr cpsr_c, r2
    ldr sp, =__sys_stack_start

    @ ABT Mode
    orr r2, r0, #0x17
    msr cpsr_c, r2
    ldr sp, =__abt_stack_start

    @ UND Mode
    orr r2, r0, #0x1B
    msr cpsr_c, r2
    ldr sp, =__und_stack_start

    @ Restore pmode
    msr cpsr_c, r1

    bx lr
