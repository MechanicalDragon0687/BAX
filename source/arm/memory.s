.arm

.section .itcm.text, "a"

.set FASTCPY_REPS, 16

.global fastcpy
.type fastcpy, %function
@ void fastcpy(void *dest, void *source, size_t n)
@ fast unrolled memcpy, intended to run on ITCM
@ assumes addresses are word-aligned and memory is RW
fastcpy:
    push {r4-r9,lr}

    @ TODO: use FIQ banked registers, maybe it could provide a speed boost?

    .fastcpy_inloop:

    .rept FASTCPY_REPS
    ldmia r1!, {r3-r9, lr}
    stmia r0!, {r3-r9, lr}
    .endr

    subs r2, #(FASTCPY_REPS*32)
    pople {r4-r9,pc}

    b .fastcpy_inloop
