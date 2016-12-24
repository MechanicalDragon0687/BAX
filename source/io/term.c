#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <gfx/gfx.h>

#include <io/term.h>
#include <io/printf.h>

static console *active_console = NULL;

void draw_char(uint8_t *fb, const uint32_t x, const uint32_t y, const uint8_t c, const uint32_t yc, const uint32_t nc)
{
    const uint32_t _c = (c & 0x7F) * FONT_Y;
    for (uint32_t _y = 0; _y < FONT_Y; _y++)
    {
        uint8_t mask = 0x80;
        uint8_t row = font[_y + _c];
        for (uint32_t _x = 0; _x < FONT_X; _x++, mask >>= 1)
            gfx_set_pixel(fb, x + _x, y + _y, (row & mask) ? yc : nc);
    }
    return;
}

void scroll_fb(console *term)
{
    for (uint32_t x = 0; x < (term->width * FONT_X); x++)
    {
        uint8_t *src  = term->fb + (x * HEIGHT * BPP),
                        // source is framebuffer column
                        *dest = term->fb + (((x * HEIGHT) + FONT_Y) * BPP);
                        // destination framebuffer + one line (bottom-top)
        uint32_t mvsize = ((HEIGHT - FONT_Y) * BPP),
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
            draw_char(active_console->fb, active_console->x * FONT_X, active_console->y * FONT_Y, c, active_console->fg, active_console->bg);
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

void term_init(console *out, const uint32_t fg, const uint32_t bg, const enum screens sid)
{
    if (!out) return;
    out->fb = get_framebuffer(sid);
    out->width = (sid == TOP_SCREEN ? (400/FONT_X) : (320/FONT_X));
    out->height = (HEIGHT/FONT_Y);
    out->x = 0;
    out->y = 0;
    out->fg = fg;
    out->bg = bg;
    active_console = out;

    clear_screen(sid, bg);
    init_printf(NULL, term_putc);
    return;
}
