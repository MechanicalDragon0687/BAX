.section .text.start
.align 4

.arm
.global _main_
_main_:
    cpsid aif @ disable interrupts

    mrc p15, 0, r0, c1, c0, 0
    bic r0, #(1 << 22)    @ disable unaligned data access
    bic r0, #(1 << 12)    @ enable level one instruction cache
    bic r0, #(1 << 2 | 1) @ disable level one data cache/MMU
    mcr p15, 0, r0, c1, c0, 0

    ldr sp, =0x22000000
    bl setup_heap

    bl main
    b _main_

setup_heap:
    ldr r0, =fake_heap_start
    ldr r1, =0x22000000
    str r1, [r0]

    ldr r0, =fake_heap_end
    add r1, #0x1000000 @ 16MB of heap
    str r1, [r0]

    bx lr
