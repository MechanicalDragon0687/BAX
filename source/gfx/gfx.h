#pragma once

#include <stdint.h>
#include <stddef.h>

#include <common.h>

#define HEIGHT (240)
#define BPP    (2)

u16 *get_framebuffer(const int screen);
void clear_screen(const int screen, const u16 rgb);
void gfx_set_pixel(u16 *fb, const u32 x, const u32 y, const u16 rgb);
