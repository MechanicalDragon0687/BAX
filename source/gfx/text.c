#include <common.h>
#include <gfx/text.h>

static uint16_t *txtfb = NULL;

void draw_char(const unsigned char c, const int x, const int y)
{
    char row, mask;
    int _x, _y;

    for (_y = 0; _y < 8; _y++) {
        mask = 0x80;
        row = text_font[c][_y];
        for (_x = 8; _x >= 0; _x--, mask>>=1) {
            if ((row & mask) == 0) {
                continue;
            }

            plot_pixel(txtfb, x + _x, y + _y, COLOR_FG);
        }
    }

    return;
}

/* Draws a text buffer onto the screen */
void show_text(const char txt[TEXT_HEIGHT][TEXT_WIDTH], int lines)
{
    int x, y;

    txtfb = get_framebuffer(GFX_TOP);
    clear_screen(GFX_TOP, COLOR_BG);

    for (y = 0; y < lines; y++) {
        for (x = 0; x < TEXT_WIDTH; x++) {
            draw_char(txt[y][x], x * 8, y * 8);
        }
    }

    return;
}
