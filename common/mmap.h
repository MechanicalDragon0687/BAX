#ifndef MMAP_H
#define MMAP_H

/* System stuff */
#define MPCORE_ENTRY (0x1FFFFFFC)
#define FIRMSTUB_LOC (0x1FFFFC00)
#define MPCORE_PMR   (0x17E00000)


/* VRAM / Framebuffer */
#define VRAM_START       (0x18000000)
#define VRAM_SIZE        (0x00600000)
#define VRAM_END         (VRAM_START + VRAM_SIZE)

#define VRAM_TOP_DIM     (490 * 240 * 2)
#define VRAM_BOTTOM_DIM  (320 * 240 * 2)

#define VRAM_TOP_SIZE    (VRAM_TOP_DIM * 2)
#define VRAM_BOTTOM_SIZE (VRAM_BOTTOM_DIM * 2)

#define VRAM_FRAMEBUFFER_A (VRAM_START + 0x00300000)
#define VRAM_FRAMEBUFFER_B (VRAM_FRAMEBUFFER_A + VRAM_TOP_DIM * 2 + VRAM_BOTTOM_DIM)

#define VRAM_TOPLEFT(x)    ((x))
#define VRAM_TOPRIGHT(x)   ((x) + VRAM_TOP_DIM + VRAM_BOTTOM_DIM)
#define VRAM_BOTTOM(x)     ((x) + VRAM_TOP_DIM)

#endif // MMAP_H
