#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <common.h>

#include <gfx/gfx.h>

#include <io/term.h>
#include <io/printf.h>

static console *active_console = NULL;

void draw_char(u16 *fb, const u32 x, const u32 y, const u8 c, const u16 color)
{
    const u32 _c = (c & 0x7F) * FONT_Y;
    for (u32 _y = 0; _y < FONT_Y; _y++)
    {
        u8 mask = 0x80;
        u8 row = font[_y + _c];
        for (u32 _x = 0; _x < FONT_X; _x++, mask >>= 1)
            gfx_set_pixel(fb, x + _x, y + _y, (row & mask) ? color : ~color);
    }
    return;
}

void scroll_fb(console *term)
{
    u16 *base_fb = get_framebuffer(term->screen);
    for (u32 x = 0; x < (term->width * FONT_X); x++)
    {
        u16 *src   = base_fb + (x * HEIGHT * BPP),
                     // source is framebuffer column
                     *dest = base_fb + (((x * HEIGHT) + FONT_Y) * BPP);
                     // destination framebuffer + one line (bottom-top)
        u32 mvsize = ((HEIGHT - FONT_Y) * BPP),
                     // move all data on column except for the first line (top-bottom)
                     stsize = (FONT_Y * BPP);
                     // clear the line of text

        memmove(dest, src, mvsize);
        memset(src, 0, stsize);
    }
    return;
}

void term_putc(__attribute__((unused)) void *p, char c)
{
    if (!active_console) return;
    if (active_console->x >= active_console->width)
    {
        active_console->x = 0;
        active_console->y++;
    }
    if (active_console->y >= active_console->height)
    {
        scroll_fb(active_console);
        active_console->y = active_console->height - 1;
    }
    switch(c)
    {
        case '\n':
            // newline
            active_console->x = 0;
            active_console->y++;
            return;
        case '\r':
            // return to start of line
            active_console->x = 0;
            return;
        default:
            // regular character
            draw_char(get_framebuffer(active_console->screen), active_console->x * FONT_X, active_console->y * FONT_Y, c, active_console->fg);
            active_console->x++;
            return;
    }
    return;
}

void set_active_console(console *in)
{
    if (in) active_console = in;
    return;
}

void term_init(console *out, const u16 fg, const enum screens sid)
{
    if (!out) return;
    out->screen = sid;
    out->width = (sid == TOP_SCREEN ? (400/FONT_X) : (320/FONT_X));
    out->height = (HEIGHT/FONT_Y);
    out->x = 0;
    out->y = 0;
    out->fg = fg;
    active_console = out;

    clear_screen(sid, ~fg);
    init_printf(NULL, term_putc);
    return;
}
