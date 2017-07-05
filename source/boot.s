.arm
.section .text.boot

#include <arm/arm.h>

.global __boot
__boot:
    nop
    nop
    nop

    @ Setup IRQ stack
    msr cpsr_c, #(SR_IRQ_MODE | SR_IRQ_BIT | SR_FIQ_BIT)
    ldr sp, =_irq_stack

    @ Setup Supervisor stack
    msr cpsr_c, #(SR_SVC_MODE | SR_IRQ_BIT | SR_FIQ_BIT)
    ldr sp, =_prg_stack

    push {r0-r3}

    @ Invalidate caches, drain write buffer
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 4

    @ Disable MPU, Caches, select low exception vectors and enable TCMs
    ldr r1, =(CR_ENABLE_MPU | CR_ENABLE_MPU | CR_ENABLE_MPU | CR_ENABLE_MPU | CR_DISABLE_TBIT)
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r1
    orr r0, #(CR_ENABLE_DTCM | CR_ENABLE_ITCM)
    mcr p15, 0, r0, c1, c0, 0

    @ Setup access permissions (RW for all modes)
    ldr r0, =0x33333333
    mcr p15, 0, r0, c5, c0, 2
    mcr p15, 0, r0, c5, c0, 3

    @ Setup MPU regions
    adr r0, __mpu_regions
    ldmia r0, {r1-r8}
    mcr p15, 0, r1, c6, c0, 0
    mcr p15, 0, r2, c6, c1, 0
    mcr p15, 0, r3, c6, c2, 0
    mcr p15, 0, r4, c6, c3, 0
    mcr p15, 0, r5, c6, c4, 0
    mcr p15, 0, r6, c6, c5, 0
    mcr p15, 0, r7, c6, c6, 0
    mcr p15, 0, r8, c6, c7, 0

    @ Setup Data TCM
    ldr r0, =_dtcm_loc
    orr r0, #(0x5 << 1)
    mcr p15, 0, r0, c9, c1, 0

    @ Initialize subsystems
    bl boot_init

    @ Enable caching for ITCM, ARM9 RAM, VRAM, FCRAM, DTCM and BootROM
    mov r0, #0b11101011
    mcr p15, 0, r0, c3, c0, 0 @ Write buffer control register
    mcr p15, 0, r0, c2, c0, 0 @ DCache control register
    mcr p15, 0, r0, c2, c0, 1 @ ICache control register

    @ Undocumented register 
    ldr r0, =0x10000020
    mov r1, #0
    str r1, [r0]
    mov r1, #0x340
    str r1, [r0]

    @ Enable MPU and Caches
    ldr r1, =(CR_ENABLE_MPU | CR_ENABLE_DCACHE | CR_ENABLE_ICACHE)
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r1
    mcr p15, 0, r0, c1, c0, 0

    pop {r0-r3}

    @ Enable IRQs
    msr cpsr_c, #(SR_SVC_MODE | SR_FIQ_BIT)
    nop

    @ Branch to C code
    bl main

    @ If it ever returns, RESET
    mov pc, #0

boot_init:
    mov r4, lr
    bl init_mem
    bl init_heap
    bl irq_reset
    bl gfx_init
    bl dma_init
    bx r4

__mpu_regions:
    .word (0x00000000 | MPU_128M | 1) @ ITCM
    .word (0x08000000 | MPU_1M   | 1) @ ARM9 memory
    .word (0x10000000 | MPU_2M   | 1) @ IO Memory
    .word (0x18000000 | MPU_8M   | 1) @ VRAM
    .word (0x1FF00000 | MPU_1M   | 1) @ DSP / AXI WRAM
    .word (0x20000000 | MPU_256M | 1) @ FCRAM
    .word (0xFFF00000 | MPU_16K  | 1) @ DTCM
    .word (0xFFFF0000 | MPU_32K  | 1) @ BootROM
