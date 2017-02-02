#include <unistd.h>

#include <main.h>

volatile uint32_t *a11_entry = (volatile uint32_t*)(0x1FFFFFF8);
volatile uint32_t *a11_vars  = (volatile uint32_t*)(0x1FFF4C70);
volatile uint32_t *cakehaxfb = (volatile uint32_t*)(0x23FFFE00);

void clean(void)
{
    a11_entry[0] = 0;
    a11_entry[1] = 0;
    for (int i = 0; i < 4; i++)
        a11_vars[i] = 0;
}

// mad props to bilis, Normmatt, dark_samus/stuckpixel
// and everyone else who helped to figure this out
void screen_init(uint32_t mode)
{
    uint32_t stride;

    mode &= 7;

    stride = (240 * 2);
    if (mode == GL_RGB8_OES) stride = (240 * 3);
    else if (mode == GL_RGBA8_OES) stride = (240 * 4);

    PDN_GPU_CNT = 0x1007F;
    LCD_REGS(0x14) = 0x00000001;
    LCD_REGS(0x0C) &= 0xFFFEFFFE;

    LCD_CFG(0, 0x44) = 0x1023E;
    LCD_CFG(1, 0x44) = 0x1023E;

    // Brightness
    LCD_CFG(0, 0x40) = 0x64;
    LCD_CFG(1, 0x40) = 0x64;

    // Top screen
    FBSETUP_PCDX(0, 0x00) = 0x000001C2;
    FBSETUP_PCDX(0, 0x04) = 0x000000D1;
    FBSETUP_PCDX(0, 0x08) = 0x000001C1;
    FBSETUP_PCDX(0, 0x0C) = 0x000001C1;
    FBSETUP_PCDX(0, 0x10) = 0x00000000;
    FBSETUP_PCDX(0, 0x14) = 0x000000CF;
    FBSETUP_PCDX(0, 0x18) = 0x000000D1;
    FBSETUP_PCDX(0, 0x1C) = 0x01C501C1;
    FBSETUP_PCDX(0, 0x20) = 0x00010000;
    FBSETUP_PCDX(0, 0x24) = 0x0000019D;
    FBSETUP_PCDX(0, 0x28) = 0x00000002;
    FBSETUP_PCDX(0, 0x2C) = 0x00000192;
    FBSETUP_PCDX(0, 0x30) = 0x00000192;
    FBSETUP_PCDX(0, 0x34) = 0x00000192;
    FBSETUP_PCDX(0, 0x38) = 0x00000001;
    FBSETUP_PCDX(0, 0x3C) = 0x00000002;
    FBSETUP_PCDX(0, 0x40) = 0x01960192;
    FBSETUP_PCDX(0, 0x44) = 0x00000000;
    FBSETUP_PCDX(0, 0x48) = 0x00000000;
    FBSETUP_PCDX(0, 0x5C) = ((240 << 16) | (400));
    FBSETUP_PCDX(0, 0x60) = 0x01C100D1;
    FBSETUP_PCDX(0, 0x64) = 0x01920002;
    FBSETUP_PCDX(0, 0x68) = FB_TOPL_A;
    FBSETUP_PCDX(0, 0x6C) = FB_TOPL_B;
    FBSETUP_PCDX(0, 0x70) = mode | (1 << 6);
    FBSETUP_PCDX(0, 0x74) = 0x00010501;
    FBSETUP_PCDX(0, 0x78) = 0x00000000;
    FBSETUP_PCDX(0, 0x90) = stride;
    FBSETUP_PCDX(0, 0x94) = FB_TOPR_A;
    FBSETUP_PCDX(0, 0x98) = FB_TOPR_B;
    FBSETUP_PCDX(0, 0x9C) = 0x00000000;

    // Bottom screen
    FBSETUP_PCDX(1, 0x00) = 0x000001C2;
    FBSETUP_PCDX(1, 0x04) = 0x000000D1;
    FBSETUP_PCDX(1, 0x08) = 0x000001C1;
    FBSETUP_PCDX(1, 0x0C) = 0x000001C1;
    FBSETUP_PCDX(1, 0x10) = 0x000000CD;
    FBSETUP_PCDX(1, 0x14) = 0x000000CF;
    FBSETUP_PCDX(1, 0x18) = 0x000000D1;
    FBSETUP_PCDX(1, 0x1C) = 0x01C501C1;
    FBSETUP_PCDX(1, 0x20) = 0x00010000;
    FBSETUP_PCDX(1, 0x24) = 0x0000019D;
    FBSETUP_PCDX(1, 0x28) = 0x00000052;
    FBSETUP_PCDX(1, 0x2C) = 0x00000192;
    FBSETUP_PCDX(1, 0x30) = 0x00000192;
    FBSETUP_PCDX(1, 0x34) = 0x0000004F;
    FBSETUP_PCDX(1, 0x38) = 0x00000050;
    FBSETUP_PCDX(1, 0x3C) = 0x00000052;
    FBSETUP_PCDX(1, 0x40) = 0x01980194;
    FBSETUP_PCDX(1, 0x44) = 0x00000000;
    FBSETUP_PCDX(1, 0x48) = 0x00000011;
    FBSETUP_PCDX(1, 0x5C) = ((240 << 16) | (320));
    FBSETUP_PCDX(1, 0x60) = 0x01C100D1;
    FBSETUP_PCDX(1, 0x64) = 0x01920052;
    FBSETUP_PCDX(1, 0x68) = FB_SUB_A;
    FBSETUP_PCDX(1, 0x6C) = FB_SUB_B;
    FBSETUP_PCDX(1, 0x70) = mode;
    FBSETUP_PCDX(1, 0x74) = 0x00010501;
    FBSETUP_PCDX(1, 0x78) = 0x00000000;
    FBSETUP_PCDX(1, 0x90) = stride;
    FBSETUP_PCDX(1, 0x9C) = 0x00000000;

    // Disco register
    for (uint32_t i = 0; i < 256; i++)
    {
        FBSETUP_PCDX(0, 0x84) = 0x10101 * i;
        FBSETUP_PCDX(1, 0x84) = 0x10101 * i;
    }

    cakehaxfb[0] = FB_TOPL_A;
    cakehaxfb[1] = FB_TOPR_A;
    cakehaxfb[2] = FB_SUB_A;

    return;
}

void vramfill(uint32_t start, uint32_t end, uint32_t val)
{
    MEMFILL_PSCX(0, 0x00) = start >> 3;
    MEMFILL_PSCX(0, 0x04) = end >> 3;
    MEMFILL_PSCX(0, 0x08) = val;
    MEMFILL_PSCX(0, 0x0C) = 0x201;

    while(!(MEMFILL_PSCX(0, 0x0C) & (2)));

    return;
}

void main(void)
{
    while(1)
    {
        clean();

        while(!a11_vars[0] && !a11_entry[0]);

        if (a11_entry[0])
        {
            uint32_t entry = a11_entry[0];
            if (entry == 0x544f4f42) entry = a11_entry[1]; // yls8
            void (*boot)(int, char*) = (void*)(entry);
            boot(0, NULL);
        }

        uint32_t cmd = a11_vars[0];

        uint32_t args[3];
        for (int i = 0; i < 3; i++)
            args[i] = a11_vars[1+i];

        switch(cmd)
        {
            case 1:
                screen_init(args[0]);
                break;

            case 2:
                vramfill(args[0], args[1], args[2]);
                break;

            default:
                break;
        }
    }
}
