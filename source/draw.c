#include "common.h"

u32 fb_sz(u8 *fb)
{
    if (fb == BOT_SCREEN0 || fb == BOT_SCREEN1)
        return SUB_FB_SZ;

    else if (fb == TOP_SCREEN0 || fb == TOP_SCREEN1)
        return TOP_FB_SZ;

    else
        return 0; // This should never happen unless I'm an idiot or someone messes with the source/binary (or both :P)
}

void clear_screen(u8 *fb, u32 rgb)
{
    u32 fb_len = fb_sz(fb),
        i = 0;

    u8  r = rgb & 0xFF,
        g = (rgb >> 8) & 0xFF,
        b = (rgb >> 16) & 0xFF;

    if ((r == g) && (g == b))
        memset(fb, r, fb_len);

    else while (i < fb_len)
    {
        *(u8*)(fb + i++) = r;
        *(u8*)(fb + i++) = g;
        *(u8*)(fb + i++) = b;
    }

    return;
}

// Thanks to hartmannaf for letting me know about 3dbrew.org/wiki/TIMER_Registers
inline void delay(u32 n)
{
    if (n == 0) // In case delay is 0
        return;

    if (n > 65534)
        n = 65534;

    REG_TM0VAL = 0; // Reset the timer
    REG_TM0CNT = 0x87; // Start timer count

    while(REG_TM0VAL < n); // Delay n ticks

    REG_TM0CNT = 0x07; // Stop timer count
    REG_TM0VAL = 0; // Reset the timer
}
