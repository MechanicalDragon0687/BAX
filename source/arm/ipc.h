#pragma once

#include <common.h>

extern char a11bin[];
extern u32 a11bin_len;

void __attribute__((naked, target("arm"))) a11_loop(void);
void wait11(void);

bool ipc_available(void);
void init11(void);
void screen_init(u32 mode);
void vramfill(u32 start, u32 end, u32 val);

u32 play_mjpeg(void *dest, void *src, u32 len);

enum pxfmt
{
    GL_RGBA8_OES = 0,
    GL_RGB8_OES = 1,
    GL_RGB565_OES = 2,
    GL_RGB5_A1_OES = 3,
    GL_RGBA4_OES = 4
};
