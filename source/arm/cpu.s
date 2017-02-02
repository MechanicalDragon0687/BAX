@ void flush_all_caches(void)

.arm
.type flush_all_caches, %function
.global flush_all_caches
flush_all_caches:
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0
    mcr p15, 0, r0, c7, c6, 0
    mcr p15, 0, r0, c7, c10, 0
    bx lr


@ void enable_irqs(void)

.arm
.type enable_irqs, %function
.global enable_irqs
enable_irqs:
    mrs r0, cpsr
    bic r0, r0, #0x80
    msr cpsr_c, r0
    bx lr


@ void disable_irqs(void)

.arm
.type disable_irqs, %function
.global disable_irqs
disable_irqs:
    mrs r0, cpsr
    orr r0, r0, #0x80
    msr cpsr_c, r0
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
    mcr p15, 0, r0, c7, c0, 4
    bx lr
