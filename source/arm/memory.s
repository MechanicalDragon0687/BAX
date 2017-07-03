.section .itcm.text, "a"
.arm

.set FASTCPY_REPS, 16

.global fastcpy
.type fastcpy, %function
@ void fastcpy(void *dest, void *source, size_t n)
@ fast unrolled CPU memory copy
fastcpy:
    push {r4-r9,lr}

    .fastcpy_inloop:

    .rept FASTCPY_REPS
    ldmia r1!, {r3-r9, lr}
    stmia r0!, {r3-r9, lr}
    .endr

    subs r2, #(FASTCPY_REPS*32)
    pople {r4-r9,pc}

    b .fastcpy_inloop
