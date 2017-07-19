.section .text.boot
.arm

#include <arm/arm.h>
#include <arm/brfunc.h>

.global __boot
__boot:
    @ Disable IRQs
    mrs r0, cpsr
    orr r0, #(SR_IRQ_BIT | SR_FIQ_BIT)
    msr cpsr, r0

    @ Cache writeback/invalidation
    ldr r4, =(BOOTROM_WRITEBACK_INVALIDATE_DCACHE)
    ldr r5, =(BOOTROM_INVALIDATE_ICACHE)
    blx r4
    blx r5

    @ Setup IRQ stack
    msr cpsr_c, #(SR_IRQ_MODE | SR_IRQ_BIT | SR_FIQ_BIT)
    ldr sp, =_irq_stack

    @ Setup Supervisor stack
    msr cpsr_c, #(SR_SVC_MODE | SR_IRQ_BIT | SR_FIQ_BIT)
    ldr sp, =_prg_stack

    @ Disable MPU, Caches, select low exception vectors and enable TCMs
    ldr r1, =(CR_ENABLE_MPU | CR_ENABLE_ICACHE | CR_ENABLE_DCACHE | CR_ALT_VECTORS | \
              CR_DISABLE_TBIT | CR_DTCM_LMODE | CR_ITCM_LMODE)
    ldr r2, =(CR_ENABLE_DTCM | CR_ENABLE_ITCM)
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r1
    orr r0, r2
    mcr p15, 0, r0, c1, c0, 0

    @ Setup access permissions (RW for all)
    ldr r0, = \
    MPU_ACCESS(MPU_RW_RW, \
               MPU_RW_RW, \
               MPU_RW_RW, \
               MPU_RW_RW, \
               MPU_RW_RW, \
               MPU_RW_RW, \
               MPU_RW_RW, \
               MPU_RW_RW)
    mcr p15, 0, r0, c5, c0, 2
    mcr p15, 0, r0, c5, c0, 3

    @ Setup MPU regions
    adr r8, __mpu_regions
    ldmia r8, {r0-r7}
    mcr p15, 0, r0, c6, c0, 0
    mcr p15, 0, r1, c6, c1, 0
    mcr p15, 0, r2, c6, c2, 0
    mcr p15, 0, r3, c6, c3, 0
    mcr p15, 0, r4, c6, c4, 0
    mcr p15, 0, r5, c6, c5, 0
    mcr p15, 0, r6, c6, c6, 0
    mcr p15, 0, r7, c6, c7, 0

    @ Setup Data TCM
    ldr r0, =_dtcm_loc
    orr r0, #(0x5 << 1)
    mcr p15, 0, r0, c9, c1, 0

    @ Undocumented register
    ldr r0, =0x10000000
    mov r1, #0
    strh r1, [r0, #0x20]
    mov r1, #0x340
    strh r1, [r0, #0x20]

    @ Initialize subsystems
    bl init_mem
    bl irq_reset
    bl proxy_init
    bl gfx_init
    bl dma_init
    bl init_heap

    @ Enable caching for ITCM, ARM9 RAM, VRAM, FCRAM, DTCM and BootROM
    mov r0, #0b11101011
    mcr p15, 0, r0, c3, c0, 0 @ Write buffer control
    mcr p15, 0, r0, c2, c0, 0 @ DCache control
    mcr p15, 0, r0, c2, c0, 1 @ ICache control

    @ Enable MPU and Caches
    ldr r1, =(CR_ENABLE_MPU | CR_ENABLE_DCACHE | CR_ENABLE_ICACHE | CR_CACHE_RROBIN)
    ldr r2, =(CR_DTCM_LMODE | CR_ITCM_LMODE)
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r1
    bic r0, r2
    mcr p15, 0, r0, c1, c0, 0

    @ Enable IRQs
    msr cpsr_c, #(SR_SVC_MODE | SR_FIQ_BIT)

    @ Branch to C code
    bl main

.pool

.word 0x13161616, \
      0x1C0F161D, \
      0x4157415A, \
      0x4F445552

__mpu_regions:
    .word (0x00000000 | MPU_128M | 1) @ ITCM
    .word (0x08000000 | MPU_1M   | 1) @ ARM9 memory
    .word (0x10000000 | MPU_2M   | 1) @ IO Memory
    .word (0x18000000 | MPU_8M   | 1) @ VRAM
    .word (0x1FF00000 | MPU_1M   | 1) @ DSP / AXI WRAM
    .word (0x20000000 | MPU_256M | 1) @ FCRAM
    .word (0xFFF00000 | MPU_16K  | 1) @ DTCM
    .word (0xFFFF0000 | MPU_32K  | 1) @ BootROM
