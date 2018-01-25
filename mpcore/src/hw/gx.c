#include <common.h>
#include <mmap.h>

#include "hw/gx.h"

static u32 selfb = 0;
const gx_framebuffers_t _fallback_framebuffers = 
{
    {VRAM_DEF_TOPLEFT,  VRAM_DEF_TOPLEFT},
    {VRAM_DEF_TOPRIGHT, VRAM_DEF_TOPRIGHT},
    {VRAM_DEF_BOTTOM,   VRAM_DEF_BOTTOM}
};

static inline vu32 *gx_framebuffer_regs(int screen)
{
    switch(screen)
    {
        default:
        case GFX_TOPL:   return &GX_PDC0[26];
        case GFX_TOPR:   return &GX_PDC0[37];
        case GFX_BOTTOM: return &GX_PDC1[26];
    }
}

static inline u32 gx_framebuffer_strides(int pdc_mode)
{
    switch(pdc_mode)
    {
        default:
        case PDC_RGBA8:  return 960;
        case PDC_BGR8:   return 720;
        case PDC_RGB565: return 480;
    }
}


void gx_reset(void)
{
    gx_psc_fill(VRAM_START, VRAM_SIZE, 0x00000000, PSC_FILL32);
    gx_set_stereoscopy(false);
    gx_set_framebuffer_mode(PDC_BGR8);
    gx_set_framebuffers(&_fallback_framebuffers);
    return;
}

void gx_psc_fill_async(u32 start, u32 len, u32 fv, int mode)
{
    u32 mid = start + (len/2), end = start + len;

    GX_PSC0[0] = start >> 3;
    GX_PSC0[1] = mid >> 3;
    GX_PSC0[2] = fv;
    GX_PSC0[3] = mode | 1;

    GX_PSC1[0] = mid >> 3;
    GX_PSC1[1] = end >> 3;
    GX_PSC1[2] = fv;
    GX_PSC1[3] = mode | 1;
    return;
}

void gx_swap_buffers(void)
{
    selfb ^= 1;
    GX_PDC0[30] = selfb;
    GX_PDC1[30] = selfb;
    return;
}

void gx_select_buffers(int fb)
{
    selfb = !fb;
    gx_swap_buffers();
    return;
}

u32 gx_framebuffer(int screen)
{
    return (gx_framebuffer_regs(screen))[selfb];
}

u32 gx_next_framebuffer(int screen)
{
    return (gx_framebuffer_regs(screen))[selfb^1];
}

void gx_set_framebuffers(const gx_framebuffers_t *fb)
{
    for (int i = 0; i < 3; i++)
    {
        (gx_framebuffer_regs(i))[0] = (*fb)[i][0];
        (gx_framebuffer_regs(i))[1] = (*fb)[i][1];
    }
    gx_select_buffers(0);
    return;
}

void gx_set_framebuffer_mode(int fmt)
{
    if (fmt < PDC_INVALID)
    {
        GX_PDC0[28] = fmt | BIT(6);
        GX_PDC1[28] = fmt;
        GX_PDC0[36] = gx_framebuffer_strides(fmt);
        GX_PDC1[36] = gx_framebuffer_strides(fmt);
    }
    return;
}

void gx_set_stereoscopy(bool enable)
{
    if (enable) GX_PDC0[28] |= BIT(5);
    else GX_PDC0[28] &= ~BIT(5);
    return;
}

void gx_dma_copy_async(const void *in, u32 indim, void *out, u32 outdim, u32 size)
{
    GX_DMA[0] = ((u32)in) >> 3;
    GX_DMA[1] = ((u32)out) >> 3;
    GX_DMA[4] = BIT(3);
    GX_DMA[8] = size;
    GX_DMA[9] = indim;
    GX_DMA[10] = outdim;
    GX_DMA[6] = 1;
    return;
}
