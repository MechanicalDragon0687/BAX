#include <common.h>

#include "hw/lcd.h"

void lcd_set_brightness(u8 brightness)
{
    LCD0_BASE[16] = brightness;
    LCD1_BASE[16] = brightness;
    return;
}

void lcd_fill(u8 r, u8 g, u8 b)
{
    u32 cfg = BIT(24) | (b << 16) | (g << 8) | (r);
    LCD0_BASE[1] = cfg;
    LCD1_BASE[1] = cfg;
    return;
}

void lcd_stop_fill(void)
{
    LCD0_BASE[1] = 0;
    LCD1_BASE[1] = 0;
    return;
}
