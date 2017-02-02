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
    int screen;
    uint32_t x, y, width, height;
    uint16_t fg;
} console;

void term_init(console *out, const uint16_t fg, const enum screens sid);
