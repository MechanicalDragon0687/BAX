#pragma once
#include <common.h>

#define BPP (2)

#define VRAM_START (0x18300000)
#define FBTLA (0x18300000)
#define FBTLB (0x1835DC00)
#define FBTRA (0x183BB800)
#define FBTRB (0x18419400)
#define FBBA  (0x18477000)
#define FBBB  (0x184C2000)
#define VRAM_END   (0x1850D000)

static const u32 def_fbs[] = {
    FBTLA, FBTRA, FBBA, FBTLB, FBTRB, FBBB
};

#define GFX_MAIN_WIDTH (400)
#define GFX_SUB_WIDTH  (320)
#define GFX_HEIGHT     (240)

#define GFX_MAIN_LEN   (GFX_MAIN_WIDTH*GFX_HEIGHT*BPP)
#define GFX_SUB_LEN    (GFX_SUB_WIDTH*GFX_HEIGHT*BPP)

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
{;
    if (screen >= GFX_INVALID) {
        screen = GFX_MAIN;
    }
    return (u16*)def_fbs[screen];
}

static inline void plot_pixel(u16 *fb, u32 x, u32 y, u16 color)
{
    fb[(x*GFX_HEIGHT) - y + GFX_HEIGHT - 1] = color;
    return;
}
