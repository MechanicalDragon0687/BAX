#pragma once

#define SCREEN_HEIGHT (240)
#define SCREEN_TOP_WIDTH (400)
#define SCREEN_BOT_WIDTH (320)

#define VRAM_TOP_SIZE (SCREEN_TOP_WIDTH * SCREEN_HEIGHT * 2)
#define VRAM_BOT_SIZE (SCREEN_BOT_WIDTH * SCREEN_HEIGHT * 2)

#define VRAM_MAX_SIZE (FRAMEBUFFER_TOP_SIZE)

#define VRAM_START (0x18000000)
#define VRAM_SIZE  (0x00300000)

#define VRAM_END   (VRAM_START + VRAM_SIZE)
