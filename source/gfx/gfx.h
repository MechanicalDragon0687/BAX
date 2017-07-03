#pragma once

#include <common.h>

#define GFX_HEIGHT (240)

enum {
    GFX_TOP = 0,
    GFX_BOTTOM = 1,
    GFX_INVALID
};

enum {
    GFX_WHITE = 0xFFFF,
    GFX_BLACK = 0x0000,
    GFX_RED   = 0xF800,
    GFX_GREEN = 0x07E0,
    GFX_BLUE  = 0x001F
};

extern void *framebuffers[2];
static inline void *get_framebuffer(const unsigned int screen)
{
    return framebuffers[screen];
}

static inline void plot_pixel(uint16_t *fb, int x, int y, uint16_t c)
{
    fb[(x*GFX_HEIGHT) - y + GFX_HEIGHT - 1] = c;
/*
    uint8_t *a = (uint8_t*)fb;

    a += ((x*GFX_HEIGHT) - y + GFX_HEIGHT - 1) * 3;

    *(a++) = (c?0xFF:0x00);
    *(a++) = (c?0xFF:0x00);
    *(a++) = (c?0xFF:0x00);*/

    return;
}

void clear_screen(const unsigned int screen, const uint16_t rgb);
