#pragma once

#include <common.h>

#define LCD0_BASE ((vu32*)(0x10202200))
#define LCD1_BASE ((vu32*)(0x10202A00))

void lcd_set_brightness(u8 brightness);
void lcd_fill(u8 r, u8 g, u8 b);
void lcd_stop_fill(void);
