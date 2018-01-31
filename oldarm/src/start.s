#include <asm.h>
#include <interrupt.h>
.align 2

ASM_FUNCTION start
    @ Clear BSS
    mov r4, #0
    ldr r5, =__bss_s
    ldr r6, =__bss_e
    1:
        cmp r5, r6
        strlo r4, [r5], #4
        blo 1b

    @ Preserve registers to be passed to low level boot code
    ldr r4, =bootregs
    stmia r4, {r0-r3}


    @ Setup stacks
    msr cpsr_c, #(SR_IRQ | SR_NOINT)
    ldr sp, =__stack_irq

    msr cpsr_c, #(SR_SYS | SR_NOINT)
    ldr sp, =__stack_sys


    @ Reset devices
    bl irq_reset
    bl timer_reset
    bl ndma_reset


    @ MPU Regions:
    @ Name    @ Start      - End        / Data,  Inst  / IC, DC, DB
    @ ITCM    @ 0x00000000 - 0x07FFFFFF / RW_NA, RO_NA /  n,  n,  n
    @ AHBRAM  @ 0x08000000 - 0x08FFFFFF / RW_NA, RO_NA /  y,  y,  y
    @ MMIO    @ 0x10000000 - 0x101FFFFF / RW_NA, NA_NA /  n,  n,  n
    @ VRAM    @ 0x18000000 - 0x187FFFFF / RW_NA, NA_NA /  n,  n,  n
    @ AXIRAM  @ 0x1FF00000 - 0x1FFFFFFF / RW_NA, NA_NA /  n,  n,  n
    @ FCRAM   @ 0x20000000 - 0x27FFFFFF / RW_NA, NA_NA /  n,  n,  n
    @ DTCM    @ 0x30000000 - 0x30003FFF / RW_NA, NA_NA /  n,  n,  n
    @ BootROM @ 0xFFFF0000 - 0xFFFF7FFF / RO_NA, RO_NA /  y,  n,  n

    mov r0, #0b10000010 @ Instruction Cachable
    mov r1, #0b00000010 @ Data Cachable
    mov r2, #0b00000010 @ Data Bufferable

    ldr r3, =0x51111111 @ Data Access Permissions
    ldr r4, =0x50000055 @ Instruction Access Permissions

    mcr p15, 0, r0, c2, c0, 1
    mcr p15, 0, r1, c2, c0, 0
    mcr p15, 0, r2, c3, c0, 0

    mcr p15, 0, r3, c5, c0, 2
    mcr p15, 0, r4, c5, c0, 3

    ldr r8, =mpu_regions
    ldmia r8, {r0-r7}
    mcr p15, 0, r0, c6, c0, 0
    mcr p15, 0, r1, c6, c1, 0
    mcr p15, 0, r2, c6, c2, 0
    mcr p15, 0, r3, c6, c3, 0
    mcr p15, 0, r4, c6, c4, 0
    mcr p15, 0, r5, c6, c5, 0
    mcr p15, 0, r6, c6, c6, 0
    mcr p15, 0, r7, c6, c7, 0


    @ Enable MPU and caches, use low vectors
    ldr r1, =0x1005
    ldr r2, =0x2000
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, r1
    bic r0, r0, r2
    mcr p15, 0, r0, c1, c0, 0
    NOP_SLED 4


    @ Setup heap
    ldr r0, =fake_heap_start
    ldr r1, =__heap_start
    str r1, [r0]

    ldr r0, =fake_heap_end
    ldr r1, =__heap_end
    str r1, [r0]


    @ Fix SDMC?
    mov r0, #0x10000000
    ldrh r1, [r0, #0x20]
    bic r1, r1, #0x001
    orr r1, r1, #0x200
    strh r1, [r0, #0x20]


    @ PXI Setup
    mov r0, #IRQ_PXI_SYNC
    ldr r1, =pxi_handler
    bl irq_register
    bl pxi_reset


    @ Enable interrupts
    mrs r0, cpsr
    bic r0, r0, #SR_NOINT
    msr cpsr_c, r0


    @ Branch to C code
    ldr r12, =main
    ldr r0, =bootregs
    bx r12


.section .rodata.mpu_regions
.align 2
mpu_regions:
    .word 0x00000035 @ ITCM
    .word 0x08000027 @ AHBRAM
    .word 0x10000029 @ MMIO
    .word 0x1800002D @ VRAM
    .word 0x1FF00027 @ AXIRAM
    .word 0x20000035 @ FCRAM
    .word 0x3000001B @ DTCM
    .word 0xFFFF001D @ BootROM


.section .bss.bootregs
.align 2
bootregs:
    .word 0, 0, 0, 0
