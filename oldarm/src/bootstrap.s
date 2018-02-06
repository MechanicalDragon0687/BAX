#include <asm.h>
#include <mmap.h>

.section .bootstrap, "ax"
.global bootstrap
bootstrap:
    @ Switch to supervisor mode and disable interrupts
    msr cpsr_c, #(SR_SVC | SR_NOINT)


    @ Check for B9S compat
    ldr r3, =0xBEEF
    lsl r4, r2, #16
    lsr r4, r4, #16
    lsr r5, r2, #16
    cmp r4, r3        @ Check magic hword
    cmpeq r0, #2      @ Check argc == 2
    bne bootstrap_error

    cmp r5, #2
    blo bootstrap_error


    @ Disable the MPU, caches, TCMs, set high exception vectors
    ldr r0, =0x2078
    mcr p15, 0, r0, c1, c0, 0
    NOP_SLED 2


    @ Cache coherency stuff
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 4


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


    @ Relocate .vectors
    ldr r0, =__vectors_lma
    ldr r1, =__vectors_s
    ldr r2, =__vectors_e
    bl bootstrap_reloc


    @ Relocate .text
    ldr r0, =__text_lma
    ldr r1, =__text_s
    ldr r2, =__text_e
    bl bootstrap_reloc


    @ Relocate .data
    ldr r0, =__data_lma
    ldr r1, =__data_s
    ldr r2, =__data_e
    bl bootstrap_reloc


    @ Branch to code
    ldr lr, =start
    bx lr


@ void bootstrap_reloc(u32 *lma, u32 *vma, u32 *vma_end)
bootstrap_reloc:
    cmp r1, r2
    ldrlo r3, [r0], #4
    strlo r3, [r1], #4
    blo bootstrap_reloc

    bx lr

@ something went really wrong
@ void bootstrap_error(void)
bootstrap_error:
    @ clear VRAM
    ldr r0, =VRAM_START
    ldr r1, =VRAM_END
    mov r2, #~0
    1:
        cmp r0, r1
        strlo r2, [r0], #4
        blo 1b

    @ ad infinitum
    mrs r0, cpsr
    orr r0, r0, #SR_NOINT
    msr cpsr_c, r0
    mov r0, #0
    2:
        mcr p15, 0, r0, c7, c0, 4
        b 2b
