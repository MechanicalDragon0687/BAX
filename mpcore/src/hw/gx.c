#include <common.h>
#include "hw/gx.h"

static u32 selfb = 0;
static vu32 *const gx_framebuffer_regs[3] = {
    &GX_PDC0[26],
    &GX_PDC0[37],
    &GX_PDC1[26]
};
const u32 gx_framebuffer_strides[GL_INVALID] = {
    [GL_RGBA8]   = 960,
    [GL_RGB8]    = 720,
    [GL_RGB565]  = 480,
    [GL_RGB5_A1] = 480,
    [GL_RGBA4]   = 480
};

void gx_psc_fill_async(u32 start, u32 len, u32 fv, gx_psc_mode mode)
{
    u32 mid = start + (len/2), end = start + len;

    GX_PSC0[0] = start >> 3;
    GX_PSC0[1] = mid >> 3;
    GX_PSC0[2] = fv;

    GX_PSC1[0] = mid >> 3;
    GX_PSC1[1] = end >> 3;
    GX_PSC1[2] = fv;

    GX_PSC0[3] = mode | 1;
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

u32 gx_framebuffer(gx_screen screen)
{
    if (screen >= GFX_INVALID)
        screen = GFX_TOPL;

    return (gx_framebuffer_regs[screen])[selfb];
}

u32 gx_next_framebuffer(gx_screen screen)
{
    if (screen >= GFX_INVALID)
        screen = GFX_TOPL;

    return (gx_framebuffer_regs[screen])[selfb^1];
}

void gx_set_framebuffers(const gx_framebuffers_t *fb)
{
    for (int i = 0; i < 3; i++)
    {
        (gx_framebuffer_regs[i])[0] = (fb->addr[i])[0];
        (gx_framebuffer_regs[i])[1] = (fb->addr[i])[1];
    }
    gx_select_buffers(0);
    return;
}

void gx_set_framebuffer_mode(gx_color_format fmt)
{
    if (fmt < GL_INVALID) {
        GX_PDC0[28] = fmt | BIT(6);
        GX_PDC1[28] = fmt;

        GX_PDC0[36] = gx_framebuffer_strides[fmt];
        GX_PDC1[36] = gx_framebuffer_strides[fmt];
    }
    return;
}

void gx_set_stereoscopy(bool enable)
{
    if (enable) {
        GX_PDC0[28] = (GX_PDC0[28] | BIT(5)) & ~BIT(6);
    } else {
        GX_PDC0[28] &= ~BIT(5);
    }
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
