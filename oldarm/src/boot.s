#include <asm.h>
.arm

ASM_FUNCTION boot

	@ Disable interrupts, switch to supervisor mode
	msr cpsr_c, #(SR_SVC | SR_I | SR_F)


	@ Preserve registers to be passed to low level boot code
	adr r4, _bootregs
	stmia r4, {r0-r3}


	@ Cache coherency stuff
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0   @ Invalidate IC
	mcr p15, 0, r0, c7, c6, 0   @ Invalidate DC
	mcr p15, 0, r0, c7, c10, 4  @ Drain Write Buffer


	@ Clear BSS
	ldr r1, =__bss_s
	ldr r2, =__bss_e
	.LClearBSS:
		cmp r1, r2
		strlo r0, [r1], #4
		blo .LClearBSS


	@ Disable MPU, caches, ITCM and DTCM, enable high exception vectors
	ldr r0, =0x2078
	mcr p15, 0, r0, c1, c0, 0


	@ Setup Tightly Coupled Memory
	ldr r0, =0x3000000A @ DTCM @ 0x30000000 / 16KB (16KB)
	ldr r1, =0x00000024 @ ITCM @ 0x00000000 / 32KB (128MB)

	mcr p15, 0, r0, c9, c1, 0
	mcr p15, 0, r1, c9, c1, 1


	@ Enable TCMs
	ldr r1, =0x50000
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, r1
	mcr p15, 0, r0, c1, c0, 0


	@ Setup stacks
	ldr sp, =__stack_abt @ Supervisor stack

	msr cpsr_c, #(SR_IRQ | SR_I | SR_F)
	ldr sp, =__stack_irq @ IRQ stack

	msr cpsr_c, #(SR_UND | SR_I | SR_F)
	ldr sp, =__stack_abt @ Undefined stack

	msr cpsr_c, #(SR_ABT | SR_I | SR_F)
	ldr sp, =__stack_abt @ Abort stack

	msr cpsr_c, #(SR_SYS | SR_I | SR_F)
	ldr sp, =__stack_sys @ System stack


	@ Copy exception vector table to ITCM
	ldr r0, =0x00000000
	ldr r1, =xrq_vectors
	ldmia r1!, {r2-r9}
	stmia r0!, {r2-r9}
	ldmia r1!, {r2-r9}
	stmia r0!, {r2-r9}


	@ Reset devices
	bl irq_reset
	bl timer_reset
	bl ndma_reset


	@ MPU Regions:
	@ Name    @ Start      - End        / Data,  Inst  / IC, DC, DB
	@ ITCM    @ 0x00000000 - 0x07FFFFFF / RW_NA, RO_NA /  n,  n,  n
	@ AHBRAM  @ 0x08000000 - 0x08FFFFFF / RW_NA, RO_NA /  y,  y,  y
	@ MMIO    @ 0x10000000 - 0x101FFFFF / RW_NA, NA_NA /  n,  n,  n
	@ VRAM    @ 0x18000000 - 0x187FFFFF / RW_NA, NA_NA /  n,  n,  n
	@ AXIRAM  @ 0x1FF00000 - 0x1FFFFFFF / RW_NA, NA_NA /  n,  n,  n
	@ FCRAM   @ 0x20000000 - 0x27FFFFFF / RW_NA, NA_NA /  n,  n,  n
	@ DTCM    @ 0x30000000 - 0x30003FFF / RW_NA, NA_NA /  n,  n,  n
	@ BootROM @ 0xFFFF0000 - 0xFFFF7FFF / RO_NA, RO_NA /  y,  n,  n

	mov r0, #0b10000010 @ Instruction Cachable
	mov r1, #0b00000010 @ Data Cachable
	mov r2, #0b00000010 @ Data Bufferable

	ldr r3, =0x51111111 @ Data Access Permissions
	ldr r4, =0x50000055 @ Instruction Access Permissions

	mcr p15, 0, r0, c2, c0, 1
	mcr p15, 0, r1, c2, c0, 0
	mcr p15, 0, r2, c3, c0, 0

	mcr p15, 0, r3, c5, c0, 2
	mcr p15, 0, r4, c5, c0, 3

	adr r8, _mpu_regions
	ldmia r8, {r0-r7}
	mcr p15, 0, r0, c6, c0, 0
	mcr p15, 0, r1, c6, c1, 0
	mcr p15, 0, r2, c6, c2, 0
	mcr p15, 0, r3, c6, c3, 0
	mcr p15, 0, r4, c6, c4, 0
	mcr p15, 0, r5, c6, c5, 0
	mcr p15, 0, r6, c6, c6, 0
	mcr p15, 0, r7, c6, c7, 0


	@ Enable MPU and caches, use low vectors
	ldr r1, =0x5005
	ldr r2, =0x2000
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, r1
	bic r0, r0, r2
	mcr p15, 0, r0, c1, c0, 0


	@ Setup heap
	ldr r0, =fake_heap_start
	ldr r1, =__heap_start
	str r1, [r0]

	ldr r0, =fake_heap_end
	ldr r1, =__heap_end
	str r1, [r0]


	@ Fix SDMC
    mov r0, #0x10000000
    mov r1, #0x340
    str r1, [r0, #0x20]


	@ Enable interrupts
	mrs r0, cpsr
	bic r0, r0, #SR_I
	msr cpsr_c, r0


	@ Branch to C code
	adr r0, _bootregs
	bl main
	b .


_mpu_regions:
	.word 0x00000035 @ ITCM
	.word 0x08000027 @ AHBRAM
	.word 0x10000029 @ MMIO
	.word 0x1800002D @ VRAM
	.word 0x1FF00027 @ AXIRAM
	.word 0x20000035 @ FCRAM
	.word 0x3000001B @ DTCM
	.word 0xFFFF001D @ BootROM

_bootregs:
	.word 0, 0, 0, 0
