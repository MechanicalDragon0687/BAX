#include <stdint.h>
#include "regs.h"

#define BRIGHTNESS (0x80)

volatile uint32_t *entry = (volatile uint32_t*)(0x1FFFFFFC);
volatile uint32_t *fbstr = (volatile uint32_t*)(0x23FFFE00);
const int stride[] = {240*4, 240*3, 240*2, 240*2};

void screen_set_mode(uint32_t m)
{
    m &= 3;
    REG_GPU_PCD0(0x70) = FB_CFG_MAIN_SCREEN | m;
    REG_GPU_PCD1(0x70) = m;
    REG_GPU_PCD0(0x90) = stride[m];
    REG_GPU_PCD1(0x90) = stride[m];

    REG_GPU_PCS0(0x0C) = 0;

    REG_GPU_PCS0(0x00) = VRAM_START >> 3;
    REG_GPU_PCS0(0x04) = VRAM_END >> 3;
    REG_GPU_PCS0(0x08) = ~0; /* TODO change to 0 on retail */
    REG_GPU_PCS0(0x0C) = (2<<8) | 1;

    while(!(REG_GPU_PCS0(0x0C) & 2));
    return;
}

void screen_init(void)
{
    *REG_GPU_CNT = 0x1007F;
    REG_LCD_CFG(0x014) = 0x00000001;
    REG_LCD_CFG(0x00C) &= 0xFFFEFFFE;

    REG_LCD_CFG(0x240) = BRIGHTNESS;
    REG_LCD_CFG(0xA40) = BRIGHTNESS;
    REG_LCD_CFG(0x244) = 0x1023E;
    REG_LCD_CFG(0xA44) = 0x1023E;

    REG_GPU_PCD0(0x00) = 0x000001C2;
    REG_GPU_PCD0(0x04) = 0x000000D1;
    REG_GPU_PCD0(0x08) = 0x000001C1;
    REG_GPU_PCD0(0x0C) = 0x000001C1;
    REG_GPU_PCD0(0x10) = 0x00000000;
    REG_GPU_PCD0(0x14) = 0x000000CF;
    REG_GPU_PCD0(0x18) = 0x000000D1;
    REG_GPU_PCD0(0x1C) = 0x01C501C1;
    REG_GPU_PCD0(0x20) = 0x00010000;
    REG_GPU_PCD0(0x24) = 0x0000019D;
    REG_GPU_PCD0(0x28) = 0x00000002;
    REG_GPU_PCD0(0x2C) = 0x00000192;
    REG_GPU_PCD0(0x30) = 0x00000192;
    REG_GPU_PCD0(0x34) = 0x00000192;
    REG_GPU_PCD0(0x38) = 0x00000001;
    REG_GPU_PCD0(0x3C) = 0x00000002;
    REG_GPU_PCD0(0x40) = 0x01960192;
    REG_GPU_PCD0(0x44) = 0x00000000;
    REG_GPU_PCD0(0x48) = 0x00000000;
    REG_GPU_PCD0(0x5C) = 0x00F00190;
    REG_GPU_PCD0(0x60) = 0x01C100D1;
    REG_GPU_PCD0(0x64) = 0x01920002;
    REG_GPU_PCD0(0x68) = FB_TOPL_A;
    REG_GPU_PCD0(0x6C) = FB_TOPL_B;
    REG_GPU_PCD0(0x74) = 0x00010501;
    REG_GPU_PCD0(0x78) = 0x00000000;
    REG_GPU_PCD0(0x94) = FB_TOPR_A;
    REG_GPU_PCD0(0x98) = FB_TOPR_B;
    REG_GPU_PCD0(0x9C) = 0x00000000;

    REG_GPU_PCD1(0x00) = 0x000001C2;
    REG_GPU_PCD1(0x04) = 0x000000D1;
    REG_GPU_PCD1(0x08) = 0x000001C1;
    REG_GPU_PCD1(0x0C) = 0x000001C1;
    REG_GPU_PCD1(0x10) = 0x000000CD;
    REG_GPU_PCD1(0x14) = 0x000000CF;
    REG_GPU_PCD1(0x18) = 0x000000D1;
    REG_GPU_PCD1(0x1C) = 0x01C501C1;
    REG_GPU_PCD1(0x20) = 0x00010000;
    REG_GPU_PCD1(0x24) = 0x0000019D;
    REG_GPU_PCD1(0x28) = 0x00000052;
    REG_GPU_PCD1(0x2C) = 0x00000192;
    REG_GPU_PCD1(0x30) = 0x00000192;
    REG_GPU_PCD1(0x34) = 0x0000004F;
    REG_GPU_PCD1(0x38) = 0x00000050;
    REG_GPU_PCD1(0x3C) = 0x00000052;
    REG_GPU_PCD1(0x40) = 0x01980194;
    REG_GPU_PCD1(0x44) = 0x00000000;
    REG_GPU_PCD1(0x48) = 0x00000011;
    REG_GPU_PCD1(0x5C) = 0x00F00140;
    REG_GPU_PCD1(0x60) = 0x01C100d1;
    REG_GPU_PCD1(0x64) = 0x01920052;
    REG_GPU_PCD1(0x68) = FB_SUB_A;
    REG_GPU_PCD1(0x6C) = FB_SUB_B;
    REG_GPU_PCD1(0x74) = 0x00010501;
    REG_GPU_PCD1(0x78) = 0x00000000;
    REG_GPU_PCD1(0x9C) = 0x00000000;

    for (uint32_t i = 0; i < 256; i++) {
        REG_GPU_PCD0(0x84) = 0x10101 * i;
        REG_GPU_PCD1(0x84) = 0x10101 * i;
    }

    fbstr[0] = FB_TOPL_A;
    fbstr[1] = FB_TOPL_A;
    fbstr[2] = FB_SUB_A;
    return;
}

void main(void)
{
    screen_init();
    screen_set_mode(FB_CFG_RGB565);

    *entry = 0;
    while(*entry == 0);

    screen_set_mode(FB_CFG_RGB24);
    ((void (*)())((uintptr_t)*entry))();
    return;
}
