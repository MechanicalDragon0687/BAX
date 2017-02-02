.section .text.start
.align 4

.arm
.global _main_
_main_:
    cpsid aif               @ disable interrupts

    mrc p15, 0, r0, c1, c0, 0
    bic r0, #(1 << 22) @ disable unaligned data access
    bic r0, #(1 << 12) @ enable level one instruction cache
    bic r0, #(1 << 2)  @ disable level one data cache
    bic r0, #(1 << 0)  @ disable the MMU
    mcr p15, 0, r0, c1, c0, 0

    ldr sp, =__stack_start  @ setup the stack

entry:
    bl main
    b entry
