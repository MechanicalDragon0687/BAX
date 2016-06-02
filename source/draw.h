#pragma once

typedef struct framebuffer_t { // Thanks to mid-kid & his CakesFW/CakeHax for FB offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} framebuffer_t;

extern framebuffer_t *framebuffer;

void delay(size_t n);
void clear_screen(u8 *fb, u32 rgb); // Clear screen (fb) with color (rgb)
void load_animation(int max); // Helper function, in order to use the randomizer
void animation_loop(char *top_anim, char *bottom_anim, const char frame_rate, const char compressed); // Main animation loop
