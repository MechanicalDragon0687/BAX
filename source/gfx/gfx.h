#pragma once

#include <stdint.h>
#include <stddef.h>

#define HEIGHT (240)
#define BPP    (2)

uint8_t *get_framebuffer(const int screen);
void clear_screen(const int screen, const uint16_t rgb);
void gfx_set_pixel(uint8_t *fb, const uint32_t x, const uint32_t y, const uint16_t rgb);
