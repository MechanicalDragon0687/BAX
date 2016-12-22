#pragma once

#include <stdint.h>
#include <stddef.h>

#define HEIGHT (240)
#define BPP    (2)

void gfx_set_pixel(uint8_t *fb, const uint32_t x, const uint32_t y, const uint32_t rgb);
void clear_screen(const int screen, const uint32_t rgb);
uint8_t *get_framebuffer(const int screen);
