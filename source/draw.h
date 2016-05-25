#pragma once

typedef struct framebuffer_t { // Thanks to mid-kid & CakesFW for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} framebuffer_t;

extern framebuffer_t *framebuffer;

void animation_loop(char *top_anim, char *bottom_anim, const char frame_rate, const char compressed);
void load_animation(int max); // Helper function, in order to use the randomizer
