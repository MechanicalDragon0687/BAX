#ifndef LCD_H
#define LCD_H

#include <types.h>

#define REG_LCD0_BASE ((vu32*)(0x10202200))
#define REG_LCD1_BASE ((vu32*)(0x10202A00))

void LCD_SetBrightness(u8 brightness);
void LCD_Fill(u8 r, u8 g, u8 b);
void LCD_StopFill(void);

#endif // LCD_H
