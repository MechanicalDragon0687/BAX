#pragma once

#ifdef ARM9
#define XRQ_REG_COUNT (18)
#else
#define XRQ_REG_COUNT (23)
#endif

// Status Register bits
#define SR_USR (0x10)
#define SR_FIQ (0x11)
#define SR_IRQ (0x12)
#define SR_SVC (0x13)
#define SR_ABT (0x17)
#define SR_UND (0x1B)
#define SR_SYS (0x1F)

#define SR_T   (1 << 5)
#define SR_F   (1 << 6)
#define SR_I   (1 << 7)

#ifdef __ASSEMBLER__
.macro ASM_FUNCTION f
    .section .text.\f, "ax", %progbits

    .global \f
    .type \f, %function
    .align 2
\f:
.endm

.macro XRQ_FATAL n, h
	#ifdef ARM11
	cpsid aif
	clrex
	#endif

    ldr sp, =__stack_abt
    sub sp, sp, #(XRQ_REG_COUNT*4)
    stmia sp, {r0-r12}
    add r9, sp, #(13*4)
    mov r12, #(\n)
    b \h
.endm

.macro XRQ_FATAL_HANDLER h
    mrs r10, spsr
    mrs r11, cpsr

    orr r0, r10, #(SR_I | SR_F)
    bic r0, r0, #(SR_T)
    tst r0, #0xF
    orreq r0, r0, #(SR_SYS)     @ Came from User, change to System

    @ Switch to previous mode with interrupts disabled and ARM mode
    msr cpsr_c, r0
    mov r1, sp
    mov r2, lr
    msr cpsr_c, r11

    tst r10, #(SR_T)            @ Thumb PC fixup
    bicne r2, r2, #1
    subne r3, lr, #3
    subeq r3, lr, #4

    stmia r9!, {r1, r2, r3, r10, r11}

    #ifdef ARM11
    mrc p15, 0, r0, c2, c0, 0   @ TTBR
    mrc p15, 0, r1, c3, c0, 0   @ DACR
    mrc p15, 0, r2, c5, c0, 0   @ DFSR
    mrc p15, 0, r3, c5, c0, 1   @ IFSR
    mrc p15, 0, r4, c6, c0, 0   @ FAR
    stmia r9!, {r0-r4}
    #endif

    mov r0, r12
    mov r1, sp
    bic sp, sp, #7 @ Align stack to an 8 byte boundary
    b \h
.endm
#endif
