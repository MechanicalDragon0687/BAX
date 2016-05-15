#pragma once

#define TOP_FB_SZ    (400*240*3)
#define SUB_FB_SZ    (320*240*3)

void compressed_animation_loop(char *top_anim, char *bottom_anim, u8 frame_rate);
void animation_loop(char *top_anim, char *bottom_anim, u32 top_frames, u32 bottom_frames, u8 frame_rate);
void load_animation(); // Helper function, in order to use the randomizer
