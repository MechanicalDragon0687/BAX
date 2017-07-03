.section .itcm.text, "a"
.arm

#include <arm/arm.h>

.global chainload
.type chainload, %function
@ void chainload(void *data, void *entry, size_t n)
@ todo: suggest some kind of software interrupt
@ that receives a file path and handles the rest
chainload:
    msr cpsr, #(SVC_MODE | IRQ_BIT | FIQ_BIT)

    bic r0, #3
    bic r1, #3

    tst r2, #3
    bicne r2, #3
    addne r2, #4

    mov r3, #0
    mcr p15, 0, r3, c7, c5, 0
    mcr p15, 0, r3, c6, c5, 0
    mcr p15, 0, r3, c10, c10, 4

    ldr r3, =(ENABLE_MPU | ENABLE_DCACHE | ENABLE_ICACHE)
    mrc p15, 0, r4, c1, c0, 0
    bic r4, r3
    mcr p15, 0, r4, c1, c0, 0

    .chainload_copy:
        subs r2, #4
        bmi .chainload_copy_out
        ldr r3, [r0, r2]
        str r3, [r1, r2]
        b .chainload_copy
    .chainload_copy_out:

    mov r4, r1
    mov r0, #0
    mov r1, #0
    mov r2, #0
    mov r3, #0

    blx r3

    mov pc, #0
