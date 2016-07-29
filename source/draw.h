#pragma once

#include "common.h"

#define TOP_FB_SZ 0x46500
#define SUB_FB_SZ 0x38400

typedef struct fb_t
{
	u8 *top_left;
	u8 *top_right;
	u8 *bottom;
} fb_t;

extern fb_t *framebuffer;

// Given a framebuffer, get its length
u32 fb_sz(u8 *fb);

// Clear screen (fb) with color (rgb)
void clear_screen(u8 *fb, u32 rgb);

// Delay for n ticks (max is 2^16)
void delay(const u16 n);

// Draws string str at (x, y) with color rgb
void draw_str(const u8 *fb, const char *str, const u16 x, const u16 y, const u32 rgb);

// Draw char c in framebuffer fb at (x, y) with color rgb
void draw_char(const u8 *fb, const u16 x, const u16 y, const u8 c, const u32 rgb);

// Display error message
void error(const char *msg);

// Font
const char font[768];
