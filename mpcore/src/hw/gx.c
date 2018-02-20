#include <common.h>
#include <mmap.h>

#include "hw/gx.h"

const GX_FrameBuffers GX_FallbackFramebuffers = {
    VRAM_FRAMEBUFFER_A, VRAM_FRAMEBUFFER_B
};
static u32 GX_CurrentFramebuffer;

static inline u32 GX_FramebufferStride(int pdc_mode) {
    switch(pdc_mode) {
        default:
        case PDC_RGBA8:  return 240 * 4;
        case PDC_BGR8:   return 240 * 3;
        case PDC_RGB565: return 240 * 2;
    }
}


void GX_Reset(void)
{
    GX_PSCFill(VRAM_START, VRAM_SIZE, 0x00000000, PSC_FILL32);
    GX_SetFramebufferMode(PDC_BGR8);
    GX_SetFramebuffers(&GX_FallbackFramebuffers);
    return;
}

void GX_PSCFillAsync(u32 start, u32 len, u32 fv, int mode)
{
    u32 mid = start + (len/2), end = start + len;

    REG_GX_PSC0[0] = start >> 3;
    REG_GX_PSC0[1] = mid >> 3;
    REG_GX_PSC0[2] = fv;
    REG_GX_PSC0[3] = mode | 1;

    REG_GX_PSC1[0] = mid >> 3;
    REG_GX_PSC1[1] = end >> 3;
    REG_GX_PSC1[2] = fv;
    REG_GX_PSC1[3] = mode | 1;
    return;
}

void GX_SwapBuffers(void)
{
    GX_CurrentFramebuffer ^= 1;
    REG_GX_PDC0[30] = GX_CurrentFramebuffer;
    REG_GX_PDC1[30] = GX_CurrentFramebuffer;
    return;
}

void GX_SelectBuffers(int fb)
{
    GX_CurrentFramebuffer = ~fb & 1;
    GX_SwapBuffers();
    return;
}

int GX_CurrentBuffer(void)
{
    return GX_CurrentFramebuffer;
}

void GX_SetFramebuffers(const GX_FrameBuffers *fb)
{
    REG_GX_PDC0[26] = VRAM_TOPLEFT((*fb)[0]);
    REG_GX_PDC0[27] = VRAM_TOPLEFT((*fb)[1]);

    REG_GX_PDC0[37] = VRAM_TOPRIGHT((*fb)[0]);
    REG_GX_PDC0[38] = VRAM_TOPRIGHT((*fb)[1]);

    REG_GX_PDC1[26] = VRAM_BOTTOM((*fb)[0]);
    REG_GX_PDC1[27] = VRAM_BOTTOM((*fb)[1]);

    GX_SelectBuffers(0);
    return;
}

void GX_SetFramebufferMode(int fmt)
{
    assert(fmt < PDC_INVALID);
    REG_GX_PDC0[28] = fmt | BIT(6);
    REG_GX_PDC1[28] = fmt;
    REG_GX_PDC0[36] = GX_FramebufferStride(fmt);
    REG_GX_PDC1[36] = GX_FramebufferStride(fmt);
    return;
}
