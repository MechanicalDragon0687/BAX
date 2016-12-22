#pragma once

#include <stdint.h>

#define FONT_X (8)
#define FONT_Y (16)

extern const char font[2048];

enum screens
{
    TOP_SCREEN = 0,
    BOTTOM_SCREEN = 1
};

typedef struct
{
    uint8_t *fb;
    uint32_t x, y, width, height;
    uint32_t fg, bg;
} console;

void term_init(console *out, uint16_t fg, uint16_t bg, const enum screens sid);
