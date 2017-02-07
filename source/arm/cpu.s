@ void flush_all_caches(void)

.arm
.type flush_all_caches, %function
.global flush_all_caches
flush_all_caches:
@ http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0201d/ch03s03s05.html
    mov r1, #0
    outer:
        mov r0, #0
        inner:
            orr r2, r1, r0
            mcr p15, 0, r2, c7, c14, 2
            add r0, r0, #0x20
            cmp r0, #0x400
            bne inner
        add r1, r1, #0x40000000
        cmp r1, #0x0
        bne outer
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c10, 4
    bx lr

@ void use_low_exception_vectors(void)

.arm
.type use_low_exception_vectors, %function
.global use_low_exception_vectors
use_low_exception_vectors:
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #0x2000
    mcr p15, 0, r0, c1, c0, 0
    bx lr


@ void use_high_exception_vectors(void)

.arm
.type use_high_exception_vectors, %function
.global use_high_exception_vectors
use_high_exception_vectors:
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #0x2000
    mcr p15, 0, r0, c1, c0, 0
    bx lr


@ void wfi(void)

.arm
.type wfi, %function
.global wfi
wfi:
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
    bx lr
