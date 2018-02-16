#include <common.h>

#include "hw/lcd.h"

void LCD_SetBrightness(u8 brightness)
{
    REG_LCD0_BASE[16] = brightness;
    REG_LCD1_BASE[16] = brightness;
}

void LCD_Fill(u8 r, u8 g, u8 b)
{
    u32 cfg = BIT(24) | (b << 16) | (g << 8) | (r);
    REG_LCD0_BASE[1] = cfg;
    REG_LCD1_BASE[1] = cfg;
}

void LCD_StopFill(void)
{
    REG_LCD0_BASE[1] = 0;
    REG_LCD1_BASE[1] = 0;
}
