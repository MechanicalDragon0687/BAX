#include <asm.h>
.arm

ASM_FUNCTION boot
    cpsid if @ Disable interrupts
    clrex    @ Clear any exclusive locks

    mov r0, #0
    mcr p15, 0, r0, c1, c0, 0  @ Clear Auxiliary Control register

    mcr p15, 0, r0, c13, c0, 0 @ Clear FCSE PID register
    mcr p15, 0, r0, c13, c0, 1 @ Clear Context ID register
    mcr p15, 0, r0, c13, c0, 2 @ Clear Thread ID registers
    mcr p15, 0, r0, c13, c0, 3
    mcr p15, 0, r0, c13, c0, 4
    mcr p15, 0, r0, c15, c12, 0 @ Disable Performance Monitors

    mcr p15, 0, r0, c7, c10, 0 @ Writeback Data Cache
    mcr p15, 0, r0, c7, c7, 0  @ Invalidate Data and Instruction Cache

    mcr p15, 0, r0, c7, c10, 4 @ DSB
    mcr p15, 0, r0, c7, c10, 5 @ DMB

    mcr p15, 0, r0, c7, c5, 4  @ Invalidate Prefetch Buffer

    ldr r0, =0x54078 @ Control Register reset value
    mcr p15, 0, r0, c1, c0, 0
    @ At this point all caches and the MMU are guaranteed to be disabled

    @ Setup stacks
    cpsid if, #SR_ABT
    ldr sp, =__stack_abt

    cpsid if, #SR_UND
    ldr sp, =__stack_abt

    cpsid if, #SR_SVC
    ldr sp, =__stack_svc

    @ Clear BSS section
    mov r0, #0
    ldr r1, =__bss_s
    ldr r2, =__bss_e
    .LClearBSS:
        cmp r1, r2
        strlo r0, [r1], #4
        blo .LClearBSS

    @ VFPv2 init
    @ Thanks derrek & profi200 for saving me from becoming bald
    @ https://github.com/derrekr/fastboot3DS/blob/f63c967369451b1fd0078e649cf0010fe10a62fd/source/arm11/start.s#L195    
    mov r1, #0xF00000           @ Give full access to cp10/11 in user and privileged mode
    mov r2, #0x40000000         @ Clear exception bits and enable VFP11
    mov r3, #0x3C00000          @ Round towards zero (RZ) mode, flush-to-zero mode, default NaN mode
    mcr p15, 0, r1, c1, c0, 2   @ Write Coprocessor Access Control Register
    mcr p15, 0, r0, c7, c5, 4   @ Flush Prefetch Buffer
    fmxr fpexc, r2              @ Write Floating-point exception register
    fmxr fpscr, r3              @ Write Floating-Point Status and Control Register

    @ Heap init
    ldr r0, =fake_heap_start
    ldr r1, =0x20000000
    str r1, [r0]

    ldr r0, =fake_heap_end
    ldr r1, =0x28000000
    str r1, [r0]


    @ Device initialization (reset, setup, etc)
    @ Branches to main code
    bl ll_init

    cpsid i
    wfi
    b .
