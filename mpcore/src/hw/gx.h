#pragma once

#include <common.h>

#define GX_DIM(w,h) ((u32)(((h) << 16) | (w)))

void gx_reset(void);

typedef enum gx_screen
{
    GFX_TOPL = 0,
    GFX_TOPR = 1,
    GFX_BOTTOM = 2,
    GFX_INVALID = 3
} gx_screen;


/* Memory Fill */
#define GX_PSC0 ((vu32*)(0x10400010))
#define GX_PSC1 ((vu32*)(0x10400020))

typedef enum
{
    PSC_FILL16 = 0 << 8, // Fill 16 bits at a time
    PSC_FILL24 = 1 << 8, // Fill 24 bits at a time
    PSC_FILL32 = 2 << 8  // Fill 32 bits at a time
} gx_psc_mode;

// Perform an asynchronous GPU Memory Fill
void gx_psc_fill_async(u32 start, u32 len, u32 fv, gx_psc_mode mode);

// Wait until the Memory Fill is done
static inline void gx_psc_wait(void)
{
    while((GX_PSC0[3] | GX_PSC1[3]) & 1);
    return;
}

// Perform a synchronous GPU Memory Fill
static inline void gx_psc_fill(u32 start, u32 len, u32 fv, gx_psc_mode mode)
{
    gx_psc_fill_async(start, len, fv, mode);
    gx_psc_wait();
    return;
}


/* Framebuffer Setup */
#define GX_PDC0 ((vu32*)(0x10400400))
#define GX_PDC1 ((vu32*)(0x10400500))

typedef enum
{
    GL_RGBA8   = 0, // RGBA8 mode  - 32bpp
    GL_RGB8    = 1, // RGB24 mode  - 24bpp
    GL_RGB565  = 2, // RGB565 mode - 16bpp
    GL_RGB5_A1 = 3, // RGB5A1 mode - 16bpp
    GL_RGBA4   = 4, // RGBA4 mode  - 16bpp
    GL_INVALID = 5
} gx_color_format;

typedef struct
{
    u32 addr[3][2];
    // {topleft_a, topleft_b},
    // {topright_a, topright_b},
    // {bottom_a, bottom_b}
} gx_framebuffers_t;

// Swap GPU framebuffers
void gx_swap_buffers(void);

// Select GPU framebuffers to use (0 = default, 1 = alternative)
void gx_select_buffers(int fb);

// Fetch the current framebuffer
u32  gx_framebuffer(gx_screen screen);

// Fetch the !current framebuffer
u32  gx_next_framebuffer(gx_screen screen);

// Set the GPU framebuffers
void gx_set_framebuffers(const gx_framebuffers_t *fb);

// Set the GPU framebuffers mode
void gx_set_framebuffer_mode(gx_color_format fmt);

// Set the top screen stereoscopy
void gx_set_stereoscopy(bool enable);


/* Transfer Engine */
#define GX_DMA ((vu32*)(0x10400C00))

#define GX_TEXTURE_ALIGNMENT (16)

// Perform an asynchronous GPU TextureCopy
void gx_dma_copy_async(const void *in, u32 indim, void *out, u32 outdim, u32 size);

// Wait until the TextureCopy is done
static inline void gx_dma_wait(void)
{
    while(GX_DMA[6] & 1);
    return;
}

// Perform a synchronous GPU TextureCopy
static inline void gx_dma_copy(const void *in, u32 indim, void *out, u32 outdim, u32 size)
{
    gx_dma_copy_async(in, indim, out, outdim, size);
    gx_dma_wait();
    return;
}
