#pragma once

#include "common.h"

// Helper function, basically a randomizer
void load_animation();

// Delay for n ticks (max is 2^16)
void delay(const u16 n);

// Read a file and get how much time it takes to read it
u32 get_read_delay();

// Main animation loop
void animation_loop(char *top_anim, char *bottom_anim, const u8 fps, const u8 compression);
