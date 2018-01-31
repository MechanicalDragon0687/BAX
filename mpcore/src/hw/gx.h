#pragma once

#include <common.h>

#define GX_DIM(w,h) ((u32)(((h) << 16) | (w)))
enum {
    GFX_TOPL = 0,
    GFX_TOPR,
    GFX_BOTTOM,
    GFX_INVALID
};

void gx_reset(void);



/* Memory Fill */
#define GX_PSC0 ((vu32*)(0x10400010))
#define GX_PSC1 ((vu32*)(0x10400020))
enum {
    PSC_FILL16 = 0 << 8,
    PSC_FILL24 = 1 << 8,
    PSC_FILL32 = 2 << 8
};

void gx_psc_fill_async(u32 start, u32 len, u32 fv, int mode);
static inline void gx_psc_wait(void) {
    while((GX_PSC0[3] | GX_PSC1[3]) & 1);
    return;
}

static inline void gx_psc_fill(u32 start, u32 len, u32 fv, int mode)
{
    gx_psc_fill_async(start, len, fv, mode);
    gx_psc_wait();
    return;
}



/* Framebuffer Setup */
#define GX_PDC0 ((vu32*)(0x10400400))
#define GX_PDC1 ((vu32*)(0x10400500))
enum {
    PDC_RGBA8   = 0, // RGBA8 mode  - 32bpp
    PDC_BGR8    = 1, // BGR24 mode  - 24bpp
    PDC_RGB565  = 2, // RGB565 mode - 16bpp
    PDC_INVALID = 3
};

typedef u32 gx_framebuffers_t[3][2];

void gx_swap_buffers(void);
void gx_select_buffers(int fb);
u32  gx_framebuffer(int screen);
u32  gx_next_framebuffer(int screen);
void gx_set_framebuffers(const gx_framebuffers_t *fb);
void gx_set_framebuffer_mode(int fmt);
void gx_set_stereoscopy(bool enable);



/* Transfer Engine */
#define GX_DMA ((vu32*)(0x10400C00))
#define GX_TEXTURE_ALIGNMENT (16)

void gx_dma_copy_async(const void *in, u32 indim, void *out, u32 outdim, u32 size);
static inline void gx_dma_wait(void) {
    while(GX_DMA[6] & 1);
    return;
}

static inline void gx_dma_copy(const void *in, u32 indim, void *out, u32 outdim, u32 size)
{
    gx_dma_copy_async(in, indim, out, outdim, size);
    gx_dma_wait();
    return;
}
