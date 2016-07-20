#pragma once

#define TOP_FB_SZ 0x46500
#define SUB_FB_SZ 0x38400

// Straight outta HelloEntryPoint
#define TOP_SCREEN0 (u8*)(*(u32*)0x23FFFE00)
#define TOP_SCREEN1 (u8*)(*(u32*)0x23FFFE00)
#define BOT_SCREEN0 (u8*)(*(u32*)0x23FFFE08)
#define BOT_SCREEN1 (u8*)(*(u32*)0x23FFFE08)

// Given a framebuffer, get its length
u32 fb_sz(u8 *fb);

// Clear screen (fb) with color (rgb)
void clear_screen(u8 *fb, u32 rgb);

// Delay for n ticks (max is 2^16)
void delay(u32 n);
