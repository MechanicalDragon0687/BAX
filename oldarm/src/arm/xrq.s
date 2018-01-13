#include <asm.h>
.arm

ASM_FUNCTION xrq_vectors
	ldr pc, =xrq_reset     @ Reset
	ldr pc, =xrq_undefined @ Undefined
	ldr pc, =xrq_swi       @ Software Interrupt
	ldr pc, =xrq_prefetch  @ Prefetch Abort
	ldr pc, =xrq_data      @ Data Abort
	ldr pc, =xrq_resv      @ Reserved
	ldr pc, =xrq_irq       @ IRQ
	ldr pc, =xrq_fiq       @ FIQ
	.pool
xrq_vectors_end:

ASM_FUNCTION xrq_reset
	mov r4, #0
	bl xrq_fatal_common


ASM_FUNCTION xrq_undefined
	mov r4, #1
	bl xrq_fatal_common


ASM_FUNCTION xrq_swi
	mov r4, #2
	bl xrq_fatal_common


ASM_FUNCTION xrq_prefetch
	mov r4, #3
	bl xrq_fatal_common


ASM_FUNCTION xrq_data
	mov r4, #4
	bl xrq_fatal_common


ASM_FUNCTION xrq_resv
	mov r4, #5
	bl xrq_fatal_common


ASM_FUNCTION xrq_fiq
	mov r4, #6
	bl xrq_fatal_common


ASM_FUNCTION xrq_fatal_common
	b .



ASM_FUNCTION xrq_irq
	sub lr, lr, #4
	push {lr}
	mrs lr, spsr
	push {lr}

	tst lr, #0xF
	orreq lr, lr, #0xF         @ Came from user mode, go to system mode
	orr lr, lr, #(SR_I | SR_F) @ Disable interrupts
	msr cpsr_c, lr
	push {r0-r5, r12, lr}

    bl irq_pending
    mov r4, r0
    bl irq_handler
    movs r1, r0
    movne r0, r4
    blxne r1
    mov r0, r4
    bl irq_ack

	pop {r0-r5, r12, lr}
    msr cpsr_c, #(SR_IRQ | SR_I | SR_F)

    pop {lr}
    msr spsr_cf, lr
    ldmfd sp!, {pc}^
