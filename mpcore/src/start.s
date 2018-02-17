#include <asm.h>
#include <interrupt.h>
#include "arm/mmu.h"

ASM_FUNCTION __start
    cpsid aif @ Disable interrupts & imprecise abort
    clrex     @ Clear any exclusive locks

    mov r0, #0
    mcr p15, 0, r0, c1, c0, 0  @ Clear Auxiliary Control register

    mcr p15, 0, r0, c13, c0, 0 @ Clear FCSE PID register
    mcr p15, 0, r0, c13, c0, 1 @ Clear Context ID register
    mcr p15, 0, r0, c13, c0, 2 @ Clear Thread ID registers
    mcr p15, 0, r0, c13, c0, 3
    mcr p15, 0, r0, c13, c0, 4

    mcr p15, 0, r0, c7, c10, 0 @ Writeback Data Cache
    mcr p15, 0, r0, c7, c7, 0  @ Invalidate Data and Instruction Cache

    mcr p15, 0, r0, c7, c10, 4 @ DSB
    mcr p15, 0, r0, c7, c10, 5 @ DMB

    mcr p15, 0, r0, c7, c5, 4  @ Invalidate Prefetch Buffer

    ldr r0, =0x54078 @ Control Register reset value
    mcr p15, 0, r0, c1, c0, 0
    NOP_SLED 2


    @ Setup stacks
    cps #SR_SVC
    mov sp, #0

    cps #SR_ABT
    mov sp, #0

    cps #SR_UND
    mov sp, #0

    cps #SR_IRQ
    mov sp, #0

    cps #SR_SYS
    ldr sp, =__stack_svc


    @ Clear BSS
    mov r0, #0
    ldr r1, =__bss_s
    ldr r2, =__bss_e
    1:
        cmp r1, r2
        strlo r0, [r1], #4
        blo 1b


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


    @ Device reset
    bl GX_Reset
    bl IRQ_Reset
    bl MMU_Reset
    bl HID_Reset


    @ Clear GPUPROT (GPU can access all FCRAM + AXIRAM)
    ldr r0, =0x10140140
    mov r1, #0
    str r1, [r0]


    @ Install exception handlers
    ldr r0, =0x1FFFFFA0
    ldr r1, =vectors_s
    ldr r2, =vectors_e
    subs r2, r2, r1
    1:
        subs r2, r2, #4
        ldrpl r3, [r1, r2]
        strpl r3, [r0, r2]
        bpl 1b


    @ MMU Translation Table setup
    @ BootROM
    ldr r3, =(MMU_PAGE_DOMAIN(0) | MMU_MEM_NONCACHEABLE | MMU_AP_RO_NA | MMU_PAGE_SECTION)
    mov r2, #1 
    ldr r1, =0x00000000
    mov r0, r1
    bl MMU_MapSections


    @ IO Registers
    ldr r3, =(MMU_PAGE_DOMAIN(0) | MMU_MEM_DEVICE_NONSHARED | MMU_AP_RW_NA | MMU_PAGE_XN | MMU_PAGE_SECTION)
    mov r2, #4
    ldr r1, =0x10100000
    mov r0, r1
    bl MMU_MapSections


    @ MPCore Private Memory Region
    ldr r3, =(MMU_PAGE_DOMAIN(0) | MMU_MEM_DEVICE_NONSHARED | MMU_AP_RW_NA | MMU_PAGE_XN | MMU_PAGE_SECTION)
    mov r2, #1
    ldr r1, =0x17E00000
    mov r0, r1
    bl MMU_MapSections


    @ VRAM
    ldr r3, =(MMU_PAGE_DOMAIN(0) | MMU_MEM_WRITETHRU_NOWRALLOC | MMU_AP_RW_NA | MMU_PAGE_XN | MMU_PAGE_SECTION)
    mov r2, #6
    ldr r1, =0x18000000
    mov r0, r1
    bl MMU_MapSections


    @ AXI BUS Work RAM
    ldr r3, =(MMU_PAGE_DOMAIN(0) | MMU_MEM_WRITEBACK_WRALLOC | MMU_AP_RW_NA | MMU_PAGE_SECTION)
    mov r2, #1
    ldr r1, =0x1FF00000
    mov r0, r1
    bl MMU_MapSections


    @ FCRAM
    ldr r3, =(MMU_PAGE_DOMAIN(0) | MMU_MEM_WRITEBACK_WRALLOC | MMU_AP_RW_NA | MMU_PAGE_XN | MMU_PAGE_SECTION)
    mov r2, #128
    ldr r1, =0x20000000
    mov r0, r1
    bl MMU_MapSections


    @ BootROM mirror
    ldr r3, =(MMU_PAGE_DOMAIN(0) | MMU_MEM_NONCACHEABLE | MMU_AP_RO_NA | MMU_PAGE_SECTION)
    mov r2, #1
    ldr r1, =0xFFF00000
    mov r0, r1
    bl MMU_MapSections


    @ Set up the TTBR and invalidate the entire TLB
    ldr r0, =mmu_tt
    orr r0, r0, #0x12
    bl MMU_SetTTBR
    bl MMU_InvTLB
    NOP_SLED 4


    @ Enable return stack, branch prediction and instruction folding
    mov r0, #0xF
    mcr p15, 0, r0, c1, c0, 1
    NOP_SLED 4


    @ Enable MMU, caches, enable program flow prediction, disable subpages, enable unaligned access
    ldr r1, =0xC01805
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, r1
    mcr p15, 0, r0, c1, c0, 0
    NOP_SLED 4


    @ Setup heap
    ldr r0, =fake_heap_start
    ldr r1, =__heap_start
    str r1, [r0]

    ldr r0, =fake_heap_end
    ldr r1, =__heap_end
    str r1, [r0]


    @ PXI Setup
    mov r0, #IRQ_PXI_SYNC
    ldr r1, =PXICMD_Handler
    mov r2, #0
    bl IRQ_Register
    bl PXI_Reset


    @ Enable interrupts
    cpsie i


    @ Branch to C code
    ldr r12, =main
    bx r12
