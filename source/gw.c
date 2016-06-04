#include "common.h"

#define set_pixel(fb, x, y, rgb) \
{ \
	u32 offset = (240 * (x) + 240 - (y) - 1) * 3; \
	*(u8*)(fb + offset++) = rgb & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 8) & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 16) & 0xFF; \
}

framebuffer_t *framebuffer;

// The saltiness is strong in this file

void draw_str(const u8 *buf, const char *str, const u16 x, const u16 y, const u32 rgb)
{
    u16 _x = x;
    u16 _y = y;

    for (u32 i = 0; i < 50; i++)
    {
        if (str[i] == '\0')
            break;

        else if (str[i] == '\r')
            _x = 0;

        else if (str[i] == '\n')
        {
            _x = 0;
            _y += 8;
        }

        else
        {
            draw_char (buf, _x, y, str[i], rgb);
            _x += 8;
        }
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
