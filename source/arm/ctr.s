@ void chainload(void *payload, size_t len)

@ uses memcpy256 @ 0xFFFF03F0
@ void memcpy256(void *src, void *ddest, const size_t size)

.arm
.type chainload, %function
.global chainload
chainload:
    mov r3, r0
    blx use_high_exception_vectors
    blx mpu_disable @ fix cache issues

    blx flush_all_caches
    blx mpu_disable

    mov r0, r3
    mov r2, r1 @ move size to the correct register
    ldr r1, =_main_
    mov lr, r1 @ fake return address
    ldr r3, =0xFFFF03F0
    bx r3


@ u32 get_bootenv(void)

.arm
.type get_bootenv, %function
.global get_bootenv
get_bootenv:
    ldr r0, =0x10010000 @ CFG_BOOTENV
    ldr r0, [r0]
    bx lr


@ u32 ctr_rand(void)

.arm
.type ctr_rand, %function
.global ctr_rand
ctr_rand:
    ldr r0, =0x10011000
    ldr r0, [r0]
    bx lr

.pool
