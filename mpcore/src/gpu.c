#include <common.h>
#include <gpu.h>
#include <gfx.h>

void gpu_memoryfill(u32 start, u32 fv, u32 end, u32 flags)
{
    u32 midpoint;

    flags &= (BIT(9) | BIT(8));
    if (start > end) {
        return;
    }
    midpoint = ((start+end)/2 & ~7);
    *GPU_PSC0(PSC_SADDR) = GPU_ADDR(start);
    *GPU_PSC0(PSC_EADDR) = GPU_ADDR(midpoint);
    *GPU_PSC0(PSC_FILL)  = fv;

    *GPU_PSC1(PSC_SADDR) = GPU_ADDR(midpoint);
    *GPU_PSC1(PSC_EADDR) = GPU_ADDR(end);
    *GPU_PSC1(PSC_FILL)  = fv;

    flags |= PSC_START;
    *GPU_PSC0(PSC_CNT)   = flags;
    *GPU_PSC1(PSC_CNT)   = flags;

    while(!((*GPU_PSC0(PSC_CNT) & *GPU_PSC1(PSC_CNT)) & PSC_DONE));

    *GPU_PSC0(PSC_CNT) = 0;
    *GPU_PSC1(PSC_CNT) = 0;
    return;
}

void gpu_setfbs(const u32 *framebuffers)
{
    *GPU_PDC0(0x68) = framebuffers[0];
    *GPU_PDC0(0x6C) = framebuffers[3];
    *GPU_PDC0(0x94) = framebuffers[1];
    *GPU_PDC0(0x98) = framebuffers[4];
    *GPU_PDC1(0x68) = framebuffers[2];
    *GPU_PDC1(0x6C) = framebuffers[5];
    *GPU_PDC0(0x78) = 0;
    *GPU_PDC1(0x78) = 0;
    return;
}

void gpu_setfbmode(u32 mode)
{
    u32 stride;

    mode &= 7;
    switch(mode) {
    case PDC_RGBA8:
        stride = 960;
        break;
    case PDC_RGB24:
        stride = 720;
        break;
    case PDC_RGB565:
    case PDC_RGB5A1:
    case PDC_RGBA4:
    default:
        stride = 480;
        break;
    }

    *GPU_PDC0(0x70) = PDC_MAINSCREEN | PDC_FIXSTRIP | mode;
    *GPU_PDC1(0x70) = PDC_FIXSTRIP | mode;

    *GPU_PDC0(0x90) = stride;
    *GPU_PDC0(0x90) = stride;
    return;
}

void gpu_setup(void)
{
    *GPU_CNT = 0x1007F;
    *LCD_CFG(0x014) = 0x00000001;
    *LCD_CFG(0x00C) &= 0xFFFEFFFE;

    *LCD_CFG(0x240) = 0x7F;
    *LCD_CFG(0xA40) = 0x7F;
    *LCD_CFG(0x244) = 0x1023E;
    *LCD_CFG(0xA44) = 0x1023E;

    *GPU_PDC0(0x00) = 0x000001C2;
    *GPU_PDC0(0x04) = 0x000000D1;
    *GPU_PDC0(0x08) = 0x000001C1;
    *GPU_PDC0(0x0C) = 0x000001C1;
    *GPU_PDC0(0x10) = 0x00000000;
    *GPU_PDC0(0x14) = 0x000000CF;
    *GPU_PDC0(0x18) = 0x000000D1;
    *GPU_PDC0(0x1C) = 0x01C501C1;
    *GPU_PDC0(0x20) = 0x00010000;
    *GPU_PDC0(0x24) = 0x0000019D;
    *GPU_PDC0(0x28) = 0x00000002;
    *GPU_PDC0(0x2C) = 0x00000192;
    *GPU_PDC0(0x30) = 0x00000192;
    *GPU_PDC0(0x34) = 0x00000192;
    *GPU_PDC0(0x38) = 0x00000001;
    *GPU_PDC0(0x3C) = 0x00000002;
    *GPU_PDC0(0x40) = 0x01960192;
    *GPU_PDC0(0x44) = 0x00000000;
    *GPU_PDC0(0x48) = 0x00000000;
    *GPU_PDC0(0x5C) = 0x00F00190;
    *GPU_PDC0(0x60) = 0x01C100D1;
    *GPU_PDC0(0x64) = 0x01920002;
    *GPU_PDC0(0x74) = 0x00010501;
    *GPU_PDC0(0x9C) = 0x00000000;

    *GPU_PDC1(0x00) = 0x000001C2;
    *GPU_PDC1(0x04) = 0x000000D1;
    *GPU_PDC1(0x08) = 0x000001C1;
    *GPU_PDC1(0x0C) = 0x000001C1;
    *GPU_PDC1(0x10) = 0x000000CD;
    *GPU_PDC1(0x14) = 0x000000CF;
    *GPU_PDC1(0x18) = 0x000000D1;
    *GPU_PDC1(0x1C) = 0x01C501C1;
    *GPU_PDC1(0x20) = 0x00010000;
    *GPU_PDC1(0x24) = 0x0000019D;
    *GPU_PDC1(0x28) = 0x00000052;
    *GPU_PDC1(0x2C) = 0x00000192;
    *GPU_PDC1(0x30) = 0x00000192;
    *GPU_PDC1(0x34) = 0x0000004F;
    *GPU_PDC1(0x38) = 0x00000050;
    *GPU_PDC1(0x3C) = 0x00000052;
    *GPU_PDC1(0x40) = 0x01980194;
    *GPU_PDC1(0x44) = 0x00000000;
    *GPU_PDC1(0x48) = 0x00000011;
    *GPU_PDC1(0x5C) = 0x00F00140;
    *GPU_PDC1(0x60) = 0x01C100d1;
    *GPU_PDC1(0x64) = 0x01920052;
    *GPU_PDC1(0x74) = 0x00010501;
    *GPU_PDC1(0x9C) = 0x00000000;
    return;
}

void gpu_init(void)
{
    if (*GPU_CNT != 1)
        gpu_setup();
    gpu_memoryfill(VRAM_START, ~0, VRAM_END, PSC_32BIT);
    gpu_setfbs(def_fbs);
    gpu_setfbmode(PDC_RGB565);
    lcd_setbrightness(0xC0);
    return;
}

void lcd_setbrightness(u8 brightness)
{
    *LCD_CFG(0x240) = brightness;
    *LCD_CFG(0x440) = brightness;
    return;
}
