#ifndef GX_H
#define GX_H

#include <common.h>

#define GX_DIM(w,h) ((u32)(((h) << 16) | (w)))
void GX_Reset(void);



/* Memory Fill */
#define REG_GX_PSC0 ((vu32*)(0x10400010))
#define REG_GX_PSC1 ((vu32*)(0x10400020))
enum {
    PSC_FILL16 = 0 << 8,
    PSC_FILL24 = 1 << 8,
    PSC_FILL32 = 2 << 8
};

void GX_PSCFillAsync(u32 start, u32 len, u32 fv, int mode);
static inline void GX_PSCWait(void) {
    while((REG_GX_PSC0[3] | REG_GX_PSC1[3]) & 1);
    return;
}

static inline void GX_PSCFill(u32 start, u32 len, u32 fv, int mode) {
    GX_PSCFillAsync(start, len, fv, mode);
    GX_PSCWait();
    return;
}



/* Framebuffer Setup */
#define REG_GX_PDC0 ((vu32*)(0x10400400))
#define REG_GX_PDC1 ((vu32*)(0x10400500))
enum {
    PDC_RGBA8   = 0, // RGBA8 mode  - 32bpp
    PDC_BGR8    = 1, // BGR24 mode  - 24bpp
    PDC_RGB565  = 2, // RGB565 mode - 16bpp
    PDC_INVALID = 3
};

typedef u32 GX_FrameBuffers[2];

void GX_SwapBuffers(void);
void GX_SelectBuffers(int fb);
int  GX_CurrentBuffer(void);
void GX_SetFramebuffers(const GX_FrameBuffers *fb);
void GX_SetFramebufferMode(int fmt);

static inline u32 GX_Framebuffer(void) {
    return REG_GX_PDC0[26 + GX_CurrentBuffer()];
}

static inline u32 GX_NextFramebuffer(void) {
    return REG_GX_PDC0[27 - GX_CurrentBuffer()];
}



/* Transfer Engine */
#define REG_GX_DMA           (((vu32*)(0x10400C00)))
#define GX_TEXTURE_ALIGNMENT (16)

static inline void GX_DMACopyAsync(const void *in, u32 indim, void *out, u32 outdim, u32 size) {
    REG_GX_DMA[0] = ((u32)in) >> 3;
    REG_GX_DMA[1] = ((u32)out) >> 3;
    REG_GX_DMA[4] = BIT(3);
    REG_GX_DMA[8] = size;
    REG_GX_DMA[9] = indim;
    REG_GX_DMA[10] = outdim;
    REG_GX_DMA[6] = 1;
    return;
}

static inline void GX_DMAWait(void) {
    while(REG_GX_DMA[6] & 1);
    return;
}

static inline void GX_DMACopy(const void *in, u32 indim, void *out, u32 outdim, u32 size)
{
    GX_DMACopyAsync(in, indim, out, outdim, size);
    GX_DMAWait();
    return;
}

#endif // GX_H
