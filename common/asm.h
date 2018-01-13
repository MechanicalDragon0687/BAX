#pragma once

#ifdef __ASSEMBLER__
.macro ASM_FUNCTION f
    .section .text.\f, "ax", %progbits

    .global \f
    .type \f, %function
    .align 2
\f:
.endm
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
