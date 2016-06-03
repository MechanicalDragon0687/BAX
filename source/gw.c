// GW-exclusive feature ('_>')

#include "common.h"

#define set_pixel(fb, x, y, rgb) \
{ \
	u32 offset = (240 * (x) + 240 - (y) - 1) * 3; \
	*(u8*)(fb + offset++) = rgb & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 8) & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 16) & 0xFF; \
}

framebuffer_t *framebuffer;

void gw()
{
    u8 buf[230400] = {0x00};
    draw_str(framebuffer->top_left, "#Bricked4Life", 152, 120, 0xFFFFFF);
    draw_str(buf, "GATEWAY DETECTED", 96, 120, 0xFF0000);

    for (u32 i = 0; i < 3; i++)
    {
        memset(framebuffer->bottom, 0x00, 230400);
        delay(32725);
        memcpy(framebuffer->bottom, buf, 230400);
        delay(32725);
    }
    return;
}

void draw_str(const u8 *buf, const char *str, const u16 x, const u16 y, const u32 rgb)
{
    u16 _x = x;

    for (u32 i = 0; i < 50; i++)
    {
        if (str[i] == '\0')
            break;

        draw_char (buf, _x, y, str[i], rgb);
        _x += 8;
    }
    return;
}

void draw_char(const u8 *fb, const u16 x, const u16 y, const u8 c, const u32 rgb)
{
    u16 _x, _y, _c = 0;

    if (c > 0x1F)
        _c = ((c % 0x80) - 0x1F) * 8;

    for(_y = 0; _y < 8; _y++)
    {
        u8 mask = 0b10000000;
        u8 row = font[_y + _c];
        for(_x = 0; _x < 8; _x++, mask >>= 1)
        {
            if(row & mask)
            {
                set_pixel(fb, x + _x, y + _y, rgb);
            }
        }
    }
    return;
}
