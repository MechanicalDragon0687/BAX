#pragma once

#include "common.h"

// Get amount of animations
u32 check_anims();

// Read a file and get how much time it takes to read it
u32 get_read_delay();

// Helper function, in order to use the randomizer
void load_animation(u32 max);

// Main animation loop
void animation_loop(char *top_anim, char *bottom_anim, const u8 fps, const u8 compression);
