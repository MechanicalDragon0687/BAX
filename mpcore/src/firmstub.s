#include <asm.h>
#include <mmap.h>
#include <pxicmd.h>

@ This function MUST be position independent
ASM_FUNCTION firmstub
    cpsid aif
    clrex

    mov r0, #0
    mcr p15, 0, r0, c7, c10, 0
    mcr p15, 0, r0, c7, c7, 0
    mcr p15, 0, r0, c7, c10, 4
    mcr p15, 0, r0, c7, c10, 5
    mcr p15, 0, r0, c7, c5, 4

    ldr r0, =0x54078
    mcr p15, 0, r0, c1, c0, 0

    NOP_SLED 2
    ldr r0, =MPCORE_ENTRY
    1:
        ldr r1, [r0]
        cmp r1, #0
        bxne r1
        b 1b
    .pool
firmstub_e:

firmstub_len:
    .word (firmstub_e - firmstub)


@ int FIRM_Launch(const void *firm, size_t firm_sz, const char *path)
ASM_FUNCTION FIRM_Launch
    push {r4-r6, lr}
    mov r4, r0
    mov r5, r1
    mov r6, r2

    mov r0, #PXICMD_ARM9_FIRMVERIFY
    sub sp, sp, #8
    mov r1, sp
    stmia r1, {r4, r5}
    mov r2, #2
    bl PXICMD_Send
    add sp, sp, #8

    cmp r0, #0
    popne {r4-r6, pc}

    @ Give approximately zero fucks about what happens after this
    cpsid if

    bl GX_Reset

    mov r0, #0
    ldr r1, =MPCORE_ENTRY
    str r0, [r1]

    ldr r0, =FIRMSTUB_LOC
    ldr r1, =firmstub
    ldr r2, =firmstub_len
    ldr r2, [r2]
    bl memcpy

    mov r0, #0
    mcr p15, 0, r0, c7, c10, 0 @ Writeback Data Cache
    mcr p15, 0, r0, c7, c10, 4 @ Data Sync Barrier
    mcr p15, 0, r0, c7, c5, 0  @ Invalidate Instruction Cache


    mov r0, #PXICMD_ARM9_FIRMBOOT
    sub sp, sp, #8
    mov r1, sp
    stmia r1, {r4, r6}
    mov r2, #2
    ldr r4, =FIRMSTUB_LOC
    bl PXICMD_SendAsync

    bx r4
