#pragma once

#define TOP_FB_SZ (240*400*3)
#define BOTTOM_FB_SZ (240*320*3)

void animationLoop(char *top_anim, char *bottom_anim, u32 topFrames, u32 bottomFrames, u8 frameRate); // Main animation loop, feed it with both animations' file names, their amount of frames and the framerate to be used
void loadAnimation(); // Helper function, in order to use the randomizer
