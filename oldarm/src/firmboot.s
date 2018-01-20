#include <asm.h>
#include <vram.h>
.align 2
.arm


.equ ARG_MAGIC, 0x0003BEEF
@ if the MPCore firmstub ever grows because reasons
@ these will have to be increased as well
.equ ARGV_LOC,  0x1FFFFC80
.equ PATH_LOC,  0x1FFFFC88
.equ FBPTR_LOC, 0x1FFFFD88


@ void firm_boot(void *firm, char *path)
ASM_FUNCTION firm_boot
    @ Setup the framebuffer struct
    ldr r2, =FBPTR_LOC
    ldr r3, =VRAM_TOPLEFT
    ldr r4, =VRAM_TOPRIGHT
    ldr r5, =VRAM_BOTTOM
    stmia r2!, {r3-r5}
    stmia r2!, {r3-r5}


    @ Copy the FIRM path somewhere safe
    ldr r2, =PATH_LOC
    add r3, r1, #0x100
    1:
        ldrb r4, [r1], #1
        cmp r4, #0
        cmpne r1, r3
        strneb r4, [r2], #1
        bne 1b

    mov r11, r0
    add r10, r11, #0x40
    mov r9, #4
    1:
        @ Fetch source, destination and length
        ldmia r10, {r0-r2}

        cmp r2, #0
        addne r0, r0, r11

        ldrne r3, =0xFFFF0374
        blxne r3            

        subs r9, #1
        addne r10, #0x30 @ Advance to the next section
        bne 1b

    @ Boot state
    @ CPSR:
    @ ARM, Supervisor, IRQ/FIQs disabled
    @ Flags are undefined
    msr cpsr_c, #(SR_SVC | SR_I | SR_F)

    @ CP15:
    @ MPU and Caches are off
    @ TCMs are on (location/configuration is undefined)
    @ Alternative exception vectors are enabled (0xFFFF0000)
    ldr r3, =0xFFFF0830
    ldr r4, =0xFFFF0AB4
    ldr r5, =0xFFFF0C58
    blx r3
    blx r4
    blx r5

    @ Registers:
    @ R0 = 1 or 2
    @ R1 = 0x1FFFFC80
    @ R2 = 0x0003BEEF
    @ R3-R14 are undefined

    @ Check screen-init flag
    ldrb r0, [r11, #0x10]
    tst r0, #1
    moveq r0, #1
    movne r0, #2
    ldr r1, =ARGV_LOC
    ldr r2, =ARG_MAGIC

    @ Setup argv
    ldr r3, =PATH_LOC
    str r3, [r1, #0x00] @ FIRM path / argv[0]

    ldrne r3, =FBPTR_LOC
    strne r3, [r1, #0x04] @ Framebuffers / argv[1]

    @ Fetch FIRM entrypoints
    ldr r3, [r11, #0x08]  @ ARM11 entrypoint
    ldr r4, [r11, #0x0C]  @ ARM9 entrypoint

    @ Set the ARM11 entrypoint
    mov r5, #0x1FFFFFFC
    str r3, [r5]

    @ Branch to the ARM9 entrypoint
    bx r4
