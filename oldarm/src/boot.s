.section .boot
.arm

#include <arm.h>

.global __boot
__boot:
    @ Disable IRQs
    mrs r4, cpsr
    orr r4, #(SR_IRQ_BIT | SR_FIQ_BIT)
    msr cpsr, r4

    @ Cache writeback/invalidation
    ldr r4, =BOOTROM_WRITEBACK_INVALIDATE_DCACHE
    ldr r5, =BOOTROM_INVALIDATE_ICACHE
    blx r4
    blx r5

    @ Setup IRQ stack
    msr cpsr_c, #(SR_IRQ_MODE | SR_IRQ_BIT | SR_FIQ_BIT)
    ldr sp, =_irq_stack

    @ Setup Supervisor stack
    msr cpsr_c, #(SR_SVC_MODE | SR_IRQ_BIT | SR_FIQ_BIT)
    ldr sp, =_prg_stack

    @ Disable MPU, Caches, select high exception vectors and enable TCMs
    ldr r5, =(CR_ENABLE_MPU | CR_ENABLE_ICACHE | CR_ENABLE_DCACHE | \
              CR_DISABLE_TBIT | CR_DTCM_LMODE | CR_ITCM_LMODE)
    ldr r6, =(CR_ENABLE_DTCM | CR_ENABLE_ITCM | CR_ALT_VECTORS)
    mrc p15, 0, r4, c1, c0, 0
    bic r4, r5
    orr r4, r6
    mcr p15, 0, r4, c1, c0, 0

    @ Setup access permissions (RW for all)
    ldr r4, =0x33333333
    mcr p15, 0, r4, c5, c0, 2
    mcr p15, 0, r4, c5, c0, 3

    @ Setup MPU regions
    adr r4, __mpu_regions
    ldmia r4, {r5-r12}
    mcr p15, 0, r5, c6, c0, 0
    mcr p15, 0, r6, c6, c1, 0
    mcr p15, 0, r7, c6, c2, 0
    mcr p15, 0, r8, c6, c3, 0
    mcr p15, 0, r9, c6, c4, 0
    mcr p15, 0, r10, c6, c5, 0
    mcr p15, 0, r11, c6, c6, 0
    mcr p15, 0, r12, c6, c7, 0

    @ Setup DTCM
    ldr r4, =0xFFF0000A
    mcr p15, 0, r4, c9, c1, 0

    @ Enable caching for ARM9 RAM, VRAM and FCRAM
    mov r4, #0b00101010
    mcr p15, 0, r4, c3, c0, 0 @ Write buffer control
    mcr p15, 0, r4, c2, c0, 0 @ DCache control
    mcr p15, 0, r4, c2, c0, 1 @ ICache control

    @ Enable MPU and Caches
    ldr r4, =(CR_ENABLE_MPU | CR_ENABLE_DCACHE | CR_ENABLE_ICACHE | \
              CR_CACHE_RROBIN)
    ldr r5, =(CR_DTCM_LMODE | CR_ITCM_LMODE)
    mrc p15, 0, r6, c1, c0, 0
    orr r6, r4
    bic r6, r5
    mcr p15, 0, r6, c1, c0, 0

    ldr r4, =__bss_start
    ldr r5, =__bss_size
    mov r6, #0
    .Lclear_bss:
        subs r5, #4
        strge r6, [r4, r5]
        bge .Lclear_bss

    @ Undocumented register
    ldr r4, =0x10000000
    mov r5, #0x340
    strh r5, [r4, #0x20]

    @ Enable IRQs
    msr cpsr_c, #(SR_SVC_MODE | SR_FIQ_BIT)

    @ Branch to C code
    b main

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
