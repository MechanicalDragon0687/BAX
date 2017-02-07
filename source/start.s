.section .text.start
.align 4

.arm
.global _main_
_main_:
    msr cpsr_c, #0xD3    @ system mode, interrupts disabled

    bl flush_all_caches
    bl mpu_disable

    bl clear_bss

    ldr sp, =__sys_stack @ set up the stack

    bl mpu_setup         @ set up the MPU regions/caches
    bl mpu_enable        @ enable MPU/ITCM/etc

    bl install_exception_vectors

    ldr r0, =0x10000020
    mov r1, #0x340
    strh r1, [r0]        @ fix SD mounting issue

    bl setup_heap

    bl flush_all_caches  @ caches are nightmare inducing

    bl bax_main          @ branch to the main code

    b _main_ @ wot

clear_bss:
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
    .clear_bss:
        str r2, [r0], #4
        cmp r0, r1
        blt .clear_bss
    bx lr

setup_heap:
    ldr r0, =fake_heap_start
    ldr r1, =0x24000000
    str r1, [r0]

    ldr r0, =fake_heap_end
    add r1, #0x1000000
    str r1, [r0]

    bx lr
