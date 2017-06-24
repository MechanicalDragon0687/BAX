.arm
.section .text.boot

.global __boot
__boot:
    @ IRQ
    msr cpsr, #0xD2
    ldr sp, =0x00008000 @ end of ITCM

    @ Supervisor
    msr cpsr, #0xD3
    ldr sp, =0x08100000 @ end of ARM9 RAM

    push {r0-r2}

    @ Flush caches
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 0

    @ Control Register
    ldr r1, =0xAA005 @ MPU, Exception vectors location, Caches
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r1
    orr r0, #0x50000 @ TCMs
    mcr p15, 0, r0, c1, c0, 0

    @ Setup MPU
    ldr r0, =0x33333333
    mcr p15, 0, r0, c5, c0, 2
    mcr p15, 0, r0, c5, c0, 3

    adr r0, mpu_table
    ldmia r0, {r1-r8}
    mcr p15, 0, r1, c6, c0, 0
    mcr p15, 0, r2, c6, c1, 0
    mcr p15, 0, r3, c6, c2, 0
    mcr p15, 0, r4, c6, c3, 0
    mcr p15, 0, r5, c6, c4, 0
    mcr p15, 0, r6, c6, c5, 0
    mcr p15, 0, r7, c6, c6, 0
    mcr p15, 0, r8, c6, c7, 0

    ldr r0, =((0x28008000) | (0x5 << 1))
    mcr p15, 0, r0, c9, c1, 0 @ enable Data TCM @ <FCRAM + 0x08008000>

    mov r0, #0b11101011
    mcr p15, 0, r0, c3, c0, 0
    mcr p15, 0, r0, c2, c0, 0
    mcr p15, 0, r0, c2, c0, 1

    ldr r1, =0x1005 @ MPU & Caches
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r1
    mcr p15, 0, r0, c1, c0, 0

    ldr r0, =0x10000020
    mov r1, #0
    str r1, [r0]
    mov r1, #0x200
    str r1, [r0]

    bl init_mem
    bl init_heap

    bl irq_reset
    pop {r0-r2}

    msr cpsr, #0x13
    bl main

    mov pc, #0

mpu_table:
    .word (0x00000000 | ((0x1A) << 1) | 1) @ ITCM (+ mirrors)
    .word (0x08000000 | ((0x13) << 1) | 1) @ ARM9-only internal memory
    .word (0x10000000 | ((0x1A) << 1) | 1) @ IO Memory
    .word (0x18000000 | ((0x16) << 1) | 1) @ VRAM
    .word (0x1FF00000 | ((0x13) << 1) | 1) @ DSP / AXI WRAM
    .word (0x20000000 | ((0x1A) << 1) | 1) @ FCRAM
    .word (0x28008000 | ((0xD) << 1)  | 1) @ DTCM
    .word (0xFFFF0000 | ((0xE) << 1)  | 1) @ Bootrom
