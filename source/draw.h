#pragma once

#define TOP_FB_SZ    (400*240*3)
#define SUB_FB_SZ    (320*240*3)

void animation_loop(char *top_anim, char *bottom_anim, const char frame_rate, const char compressed);
void load_animation(); // Helper function, in order to use the randomizer
