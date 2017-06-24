#pragma once

#include <common.h>

#define GFX_TOP    (0)
#define GFX_BOTTOM (2)
#define GFX_HEIGHT (240)

static inline void *get_framebuffer(const int screen)
{
    return (void*)(((volatile uint32_t*)0x23FFFE00)[screen]);
}

static inline void plot_pixel(uint16_t *fb, int x, int y, uint16_t c)
{
    fb[(x*GFX_HEIGHT) - y + GFX_HEIGHT - 1] = c;
}

void *get_framebuffer(const int screen);
void clear_screen(const int screen, const uint16_t rgb);
void gfx_set_pixel(uint16_t *fb, const int x, const int y, const uint16_t rgb);
