#pragma once

#define TOP_FB_SZ (240*400*3)
#define BOTTOM_FB_SZ (240*320*3)

void animation_loop(char *top_anim, char *bottom_anim, u32 top_frames, u32 bottom_frames, u8 frame_rate);
void load_animation(); // Helper function, in order to use the randomizer
