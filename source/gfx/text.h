#pragma once

#include <gfx/gfx.h>

#define TEXT_WIDTH  (50)
#define TEXT_HEIGHT (30)

#define COLOR_BG (0x0000)
#define COLOR_FG (~COLOR_BG)

extern char text_font[128][8];

void show_text(const char txt[TEXT_HEIGHT][TEXT_WIDTH], const int lines);
