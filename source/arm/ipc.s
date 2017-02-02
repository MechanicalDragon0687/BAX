.arm
.global a11_loop
.type a11_loop, %function
a11_loop:
    .word 0xF10C01C0 @ cpsid aif
    mrc p15, 0, r0, c1, c0, 0
    bic r0, #(1 << 2 || 1)  @ disable level one data cache and MPU
    mcr p15, 0, r0, c1, c0, 0
    mov r0, #0x20000000
    sub r0, #8
    mov r1, #0
    str r1, [r0]

    .a11_loop:
        ldr r1, [r0]
        cmp r1, #0
        beq .a11_loop
        bx r1

    b a11_loop

.arm
.global wait11
.type wait11, %function
wait11:
    ldr r0, =0x1FFF4C70
    .wait11:
        ldr r1, [r0]
        cmp r1, #0
        bxeq lr
        b .wait11
