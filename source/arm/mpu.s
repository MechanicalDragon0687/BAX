.arm
.type mpu_disable, %function
.global mpu_disable
mpu_disable:
    @ http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0201d/I1014121.html
    mrc p15, 0, r0, c1, c0, 0
    bic r0, #(1 << 18) @ ITCM
    bic r0, #(1 << 12) @ Instruction cache
    bic r0, #(1 << 2)  @ Data cache
    bic r0, #(1 << 0)  @ Memory Protection Unit
    mcr p15, 0, r0, c1, c0, 0  @ write control register
    bx lr

.arm
.type mpu_enable, %function
.global mpu_enable
mpu_enable:
    mrc p15, 0, r0, c1, c0, 0
    orr r0, #(1 << 18)
    orr r0, #(1 << 12)
    orr r0, #(1 << 2)
    orr r0, #(1 << 0)
    mcr p15, 0, r0, c1, c0, 0  @ write control register
    bx lr

.arm
.type mpu_setup, %function
.global mpu_setup
mpu_setup:
    ldr r0, =0x33333333
    mcr p15, 0, r0, c5, c0, 2
    mcr p15, 0, r0, c5, c0, 3

    ldr r0, =mpu_table
    ldmia r0, {r1-r8}
    mcr p15, 0, r1, c6, c0, 0
    mcr p15, 0, r2, c6, c1, 0
    mcr p15, 0, r3, c6, c2, 0
    mcr p15, 0, r4, c6, c3, 0
    mcr p15, 0, r5, c6, c4, 0
    mcr p15, 0, r6, c6, c5, 0
    mcr p15, 0, r7, c6, c6, 0
    mcr p15, 0, r8, c6, c7, 0

    mov r0, #0b00101011 @ ITCM, ARM9 RAM, VRAM & FCRAM
    mcr p15, 0, r0, c3, c0, 0
    mcr p15, 0, r0, c2, c0, 0
    mcr p15, 0, r0, c2, c0, 1
    bx lr

.set MPU_SIZE_16K,  ((0xD) << 1)
.set MPU_SIZE_32K,  ((0xE) << 1)
.set MPU_SIZE_1M,   ((0x13) << 1)
.set MPU_SIZE_8M,   ((0x16) << 1)
.set MPU_SIZE_128M, ((0x1A) << 1)

mpu_table:
    .word (0x00000000 | MPU_SIZE_128M | 1) @ ITCM
    .word (0x08000000 | MPU_SIZE_1M   | 1) @ ARM9-only internal memory
    .word (0x10000000 | MPU_SIZE_128M | 1) @ IO Memory
    .word (0x18000000 | MPU_SIZE_8M   | 1) @ VRAM
    .word (0x1FF00000 | MPU_SIZE_1M   | 1) @ DSP / AXI WRAM
    .word (0x20000000 | MPU_SIZE_128M | 1) @ FCRAM
    .word (0xFFF00000 | MPU_SIZE_16K  | 1) @ DTCM
    .word (0xFFFF0000 | MPU_SIZE_32K  | 1) @ Bootrom
