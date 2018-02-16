#include <asm.h>


@ void MMU_Reset(void)
ASM_FUNCTION MMU_Reset
    mov r0, #0
    ldr r1, =0x55555555

    mcr p15, 0, r0, c2, c0, 0 @ clear TTBR0
    mcr p15, 0, r0, c2, c0, 1 @ clear TTBR1
    mcr p15, 0, r0, c2, c0, 2 @ clear TTBCR
    mcr p15, 0, r1, c3, c0, 0 @ set the DACR to Client for all
    bx lr


@ void MMU_InvTLB(void)
ASM_FUNCTION MMU_InvTLB
    mov r0, #0
    mcr p15, 0, r0, c8, c7, 0
    bx lr


@ void MMU_SetTTBR(u32 ttbr_flags)
ASM_FUNCTION MMU_SetTTBR
    mcr p15, 0, r0, c2, c0, 0
    bx lr


@ u32 MMU_GetTTBR(void)
ASM_FUNCTION MMU_GetTTBR
    mrc p15, 0, r0, c2, c0, 0
    bx lr


@ void MMU_SetDACR(u32 dacr)
ASM_FUNCTION MMU_SetDACR
    mcr p15, 0, r0, c3, c0, 0
    bx lr


@ u32 MMU_GetDACR(void)
ASM_FUNCTION MMU_GetDACR
    mrc p15, 0, r0, c3, c0, 0
    bx lr


@ void MMU_MapSections(u32 pa, u32 va, u32 count, u32 flags)
ASM_FUNCTION MMU_MapSections
    ldr r12, =mmu_tt
    lsr r0, r0, #20
    lsr r1, r1, #20

    1:
        subs r2, r2, #1
        orrpl r3, r3, r1, lsl #20
        strpl r3, [r12, r0, lsl #2]
        lslpl r3, r3, #12
        lsrpl r3, r3, #12
        addpl r0, r0, #1
        addpl r1, r1, #1
        bpl 1b

    bx lr


.section .bss.mmu_tt
.global mmu_tt
.align 14
mmu_tt:
    .space (16384)
