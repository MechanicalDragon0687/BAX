#pragma once
#include <common.h>

#define GFX_HEIGHT (240)

enum {
    GFX_MAIN  = 0,
    GFX_MAINR = 1,
    GFX_SUB   = 2,
    GFX_INVALID
};

enum {
    GFX_WHITE = 0xFFFF,
    GFX_BLACK = 0x0000,
    GFX_RED   = 0xF800,
    GFX_GREEN = 0x07E0,
    GFX_BLUE  = 0x001F
};

static inline u16 *get_framebuffer(u32 screen)
{
    if (screen >= GFX_INVALID) {
        screen = GFX_MAIN;
    }
    return (u16*)(((u32*)0x23FFFE00)[screen]);
}

static inline void plot_pixel(u16 *fb, u32 x, u32 y, u16 color)
{
    fb[(x*GFX_HEIGHT) - y + GFX_HEIGHT - 1] = color;
    return;
}
