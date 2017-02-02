.arm
@ uint8_t *get_framebuffer(const int screen)

.type get_framebuffer, %function
.global get_framebuffer
get_framebuffer:
    push {r4, lr}
    ldr r4, =0x23FFFE00
    cmp r0, #0
    ldreq r0, [r4]
    ldrne r0, [r4, #8]
    pop {r4, pc}


.arm
@ void clear_screen(const int screen, const uint32_t rgb)

.type clear_screen, %function
.global clear_screen
clear_screen:

    push {r4, lr}
    ldr r4, =0x23FFFE00  @ CakeHax framebuffer struct
    cmp r0, #0

    ldreq r0, [r4] @ top screen
    ldreq r2, =(400*240*2)

    ldrne r0, [r4, #8] @ bottom screen
    ldrne r2, =(320*240*2)

    add r2, r0, r2

    @ RGB565
    orr r1, r1, r1, lsl #16 @ halfword -> word

    loop:
        cmp r0, r2
        strlt r1, [r0], #4
        blt loop

    pop {r4, pc}


.arm
@ void gfx_set_pixel(uint8_t *fb, const uint32_t x, const uint32_t y, const uint16_t rgb)

.type gfx_set_pixel, %function
.global gfx_set_pixel
gfx_set_pixel:
    @ offset = r0 + (((r1 * 240) - r2 + 239) * 2);

    push {r4, lr}
    mov r4, #240
    mul r1, r4, r1         @ r1 := r1 * 240
    sub r1, r1, r2         @ r1 := r1 - r2
    add r1, r1, #239       @ r1 := r1 + 239

    add r0, r0, r1, lsl #1
    bic r0, #1 @ Make sure it's halfword aligned

    strh r3, [r0]

    pop {r4, pc}
