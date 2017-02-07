#pragma once

#include <types.h>

void vramfill(u32 start, u32 end, u32 val);
void screen_init(u32 mode);

#define FB_SIZE(x)  ((x) ? (320*240*4) : (400*240*4))

#define VRAM_START  (0x18300000)

#define FB_TOPL_A   (VRAM_START)
#define FB_TOPL_B   (FB_TOPL_A + FB_SIZE(0))

#define FB_TOPR_A   (FB_TOPL_B + FB_SIZE(0))
#define FB_TOPR_B   (FB_TOPR_A + FB_SIZE(0))

#define FB_SUB_A    (FB_TOPR_B + FB_SIZE(0))
#define FB_SUB_B    (FB_SUB_A + FB_SIZE(1))

#define VRAM_END    (FB_SUB_B + FB_SIZE(1))

// LCD Registers (3dbrew.org/wiki/LCD_Registers)
#define PDN_GPU_CNT        (*((vu32*)(0x10141200)))
#define LCD_REGS(x)        (*((vu32*)(0x10202000 + (x))))
#define LCD_CFG(s, x)      (*((vu32*)(0x10202200 + ((s) * 0x800) + (x))))

// PCD Registers (http://3dbrew.org/wiki/GPU/External_Registers#LCD_Source_Framebuffer_Setup)
#define FBSETUP_PCDX(s, x) (*((vu32*)(0x10400400 + ((s) * 0x100)  + (x))))

// PSC Registers (http://3dbrew.org/wiki/GPU/External_Registers#Memory_Fill)
#define MEMFILL_PSCX(a, x)   (*((vu32*)(0x10400010 + (x) + ((a) * 0x10))))

// Framebuffer formats

enum pxfmt
{
    GL_RGBA8_OES = 0,
    GL_RGB8_OES = 1,
    GL_RGB565_OES = 2,
    GL_RGB5_A1_OES = 3,
    GL_RGBA4_OES = 4
};

/*
Known LCD Framebuffer setup registers

Offset | Length | Name                           | Comments
0x5C   | 4      | Framebuffer width & height     | Lower 16 bits: width, upper 16 bits: height
0x68   | 4      | Framebuffer A first address    | For top screen, this is the left eye 3D framebuffer.
0x6C   | 4      | Framebuffer A second address   | For top screen, this is the left eye 3D framebuffer.
0x70   | 4      | Framebuffer format             | Bit0-15: framebuffer format, bit16-31: unknown.
0x78   | 4      | Framebuffer select             | Bit0: which framebuffer to display, bit1-7: unknown.
0x84   | 4      | Disco register (unknown)       | Each poke changes the color blended over the screens content randomly.
0x90   | 4      | Framebuffer stride             | Distance in bytes between the start of two framebuffer rows (must be a multiple of 8).
0x94   | 4      | Framebuffer B first address    | For top screen, this is the right eye 3D framebuffer. Unused for bottom screen.
0x98   | 4      | Framebuffer B second address   | For top screen, this is the right eye 3D framebuffer. Unused for bottom screen.
*/

/*
Framebuffer format

Bit(s) | Description
2-0    | Color format
3      | ?
4      | Unused?
5      | Enable parallax barrier (i.e. 3D).
6      | 1 = main screen, 0 = sub screen. However if bit5 is set, this bit is cleared.
7      | ?
9-8    | 1 = unknown: get rid of rainbow strip on top of screen, 3 = unknown: black screen.
15-10  | Unused?
*/
