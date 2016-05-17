#pragma once

#define TOP_FB_SZ    (400*240*3)
#define SUB_FB_SZ    (320*240*3)

typedef struct framebuffer_t { // Thanks to mid-kid & CakesFW for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} framebuffer_t;

extern framebuffer_t *framebuffer;

void animation_loop(char *top_anim, char *bottom_anim, const char frame_rate, const char compressed);
void load_animation(u32 max); // Helper function, in order to use the randomizer
