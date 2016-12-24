.arm
@ void flush_all_caches(void)

.type enable_irqs, %function
.global flush_all_caches
flush_all_caches:
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 0
    bx lr


.arm
@ void enable_irqs(void)

.type enable_irqs, %function
.global enable_irqs
enable_irqs:
    mrs r0, cpsr
    bic r0, r0, #0x80
    msr cpsr_c, r0
    bx lr


.arm
@ void disable_irqs(void)

.type disable_irqs, %function
.global disable_irqs
disable_irqs:
    mrs r0, cpsr
    orr r0, r0, #0x80
    msr cpsr_c, r0
    bx lr


.arm
@ void use_low_exception_vectors(void)

.type use_low_exception_vectors, %function
.global use_low_exception_vectors
use_low_exception_vectors:
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #0x2000
    mcr p15, 0, r0, c1, c0, 0
    bx lr


.arm
@ void use_high_exception_vectors(void)

.type use_high_exception_vectors, %function
.global use_high_exception_vectors
use_high_exception_vectors:
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #0x2000
    mcr p15, 0, r0, c1, c0, 0
    bx lr


.thumb
@ uint32_t get_bootenv(void)

.type get_bootenv, %function
.global get_bootenv
get_bootenv:
    ldr r0, =0x10010000 @ CFG_BOOTENV
    ldr r0, [r0]
    bx lr

.arm
@ void wfi(void)

.type wfi, %function
.global wfi
wfi:
    mcr p15, 0, r0, c7, c0, 4
    bx lr
