#pragma once

#include <common.h>

extern char a11bin[];
extern uint32_t a11bin_len;

void __attribute__((naked, target("arm"))) a11_loop(void);
void wait11(void);

void init11(void);
void screen_init(uint32_t mode);
void vramfill(uint32_t start, uint32_t end, uint32_t val);

enum pxfmt
{
    GL_RGBA8_OES = 0,
    GL_RGB8_OES = 1,
    GL_RGB565_OES = 2,
    GL_RGB5_A1_OES = 3,
    GL_RGBA4_OES = 4
};
