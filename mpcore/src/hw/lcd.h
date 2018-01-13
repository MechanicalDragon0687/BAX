#pragma once

#include <common.h>
#include "hw/gx.h"

#define LCD0_BASE ((vu32*)(0x10202200))
#define LCD1_BASE ((vu32*)(0x10202A00))

// Set LCD brightness
void lcd_set_brightness(u8 brightness);

// Fill the LCD with an RGB24 color (the GPU is not involved?)
void lcd_fill(u8 r, u8 g, u8 b);

// Stop the LCD fill
void lcd_stop_fill(void);
