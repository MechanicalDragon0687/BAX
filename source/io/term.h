#pragma once

#include <stdint.h>
#include <common.h>

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
    u32 x, y, width, height;
    u16 fg;
} __attribute__((packed)) console;

void term_init(console *out, const u16 fg, const enum screens sid);
