#include <asm.h>
#include <mmap.h>
#include <interrupt.h>
.align 2

.section .bootstrap, "ax"
.global __start
__start:
    @ Switch to supervisor mode and disable interrupts
    msr cpsr_c, #(SR_SVC | SR_NOINT)


    @ Check for B9S compat
    ldr r3, =0xBEEF
    lsl r4, r2, #16
    lsr r4, r4, #16
    lsr r5, r2, #16
    cmp r4, r3        @ Check magic hword
    cmpeq r0, #2      @ Check argc == 2
    bne CRTError

    cmp r5, #2
    blo CRTError


    @ Cache coherency stuff
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 4


    @ Disable the MPU, caches, TCMs, set high exception vectors
    ldr r0, =0x2078
    mcr p15, 0, r0, c1, c0, 0
    NOP_SLED 2


    @ Setup Tightly Coupled Memory
    ldr r0, =0x3000000A @ DTCM @ 0x30000000 / 16KB (16KB)
    ldr r1, =0x00000024 @ ITCM @ 0x00000000 / 32KB (128MB)
    mcr p15, 0, r0, c9, c1, 0
    mcr p15, 0, r1, c9, c1, 1
    NOP_SLED 2


    @ Enable TCMs
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #0x50000
    mcr p15, 0, r0, c1, c0, 0
    NOP_SLED 2


    @ Relocate executable
    ldr r0, =__text_lma
    ldr r1, =__text_s
    ldr r2, =__text_e
    bl BootstrapReloc


    @ Relocate vectors
    ldr r0, =__vectors_lma
    ldr r1, =__vectors_s
    ldr r2, =__vectors_e
    bl BootstrapReloc


    @ Clear BSS
    mov r0, #0
    ldr r1, =__bss_s
    ldr r2, =__bss_e
    1:
        cmp r1, r2
        strlo r0, [r1], #4
        blo 1b


    @ Branch to main startup code
    ldr lr, =start_itcm
    bx lr


@ void BootstrapReloc(void *lma, void *vma_start, void *vma_end)
@ equivalent to memcpy(vma_start, lma, vma_end - vma_start)
BootstrapReloc:
    cmp r1, r2
    ldrlo r3, [r0], #4
    strlo r3, [r1], #4
    blo BootstrapReloc
    bx lr

@ something went really wrong
@ might be completely useless but better than nothing?
@ void CRTError(void)
CRTError:
    @ clear VRAM
    ldr r0, =VRAM_START
    ldr r1, =VRAM_END
    mov r2, #0xFFFFFFFF
    1:
        cmp r0, r1
        strlo r2, [r0], #4
        blo 1b

    @ ad infinitum
    msr cpsr_c, #(SR_NOINT | SR_SVC)
    mov r0, #0
    2:
        mcr p15, 0, r0, c7, c0, 4
        b 2b
.pool



ASM_FUNCTION start_itcm
    @ Setup stacks
    msr cpsr_c, #(SR_IRQ | SR_NOINT)
    ldr sp, =__stack_irq

    msr cpsr_c, #(SR_SYS | SR_NOINT)
    ldr sp, =__stack_sys


    @ Reset devices
    bl IRQ_Reset
    bl NDMA_Reset


    @ MPU Regions:
    @ N | Name    | Start      | End        | Data  | Inst  | IC | DC | DB
    @ 0 | ITCM    | 0x00000000 | 0x07FFFFFF | RW_NA | RO_NA | n  | n  | n
    @ 1 | AHBRAM  | 0x08000000 | 0x08FFFFFF | RW_NA | RO_NA | y  | y  | y
    @ 2 | MMIO    | 0x10000000 | 0x101FFFFF | RW_NA | NA_NA | n  | n  | n
    @ 3 | VRAM    | 0x18000000 | 0x187FFFFF | RW_NA | NA_NA | n  | n  | n
    @ 4 | AXIRAM  | 0x1FF00000 | 0x1FFFFFFF | RW_NA | NA_NA | n  | n  | n
    @ 5 | FCRAM   | 0x20000000 | 0x27FFFFFF | RW_NA | NA_NA | n  | n  | n
    @ 6 | DTCM    | 0x30000000 | 0x30003FFF | RW_NA | NA_NA | n  | n  | n
    @ 7 | BootROM | 0xFFFF0000 | 0xFFFF7FFF | RO_NA | RO_NA | y  | n  | n

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
    NOP_SLED 2


    @ Fix SDMC?
    mov r0, #0x10000000
    ldrh r1, [r0, #0x20]
    bic r1, r1, #0x001
    orr r1, r1, #0x200
    strh r1, [r0, #0x20]


    @ PXI Setup
    mov r0, #IRQ_PXI_SYNC
    ldr r1, =PXICMD_Handler
    bl IRQ_Register

    mov r0, #IRQ_PXI_RECV_NOT_EMPTY
    mov r1, #0
    bl IRQ_Register

    bl PXI_Reset


    @ Enable interrupts
    mrs r0, cpsr
    bic r0, r0, #SR_NOINT
    msr cpsr_c, r0


    @ Branch to C code
    ldr r12, =main
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
