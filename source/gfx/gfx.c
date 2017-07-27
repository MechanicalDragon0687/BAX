#include <common.h>
#include <arm/cpu.h>
#include <gfx/gfx.h>

uint32_t fb_width[2] = {400, 320};
void *framebuffers[2] = {NULL};

/* Initialize framebuffer pointers */
void gfx_init(void)
{
    inv_dc_range(0x23FFFE00, 0x23FFFE20);
    framebuffers[0] = (void*)(((uint32_t*)0x23FFFE00)[0]);
    framebuffers[1] = (void*)(((uint32_t*)0x23FFFE00)[2]);
    return;
}

void clear_screen(unsigned int screen, uint16_t rgb)
{
    uint32_t *fb, fill;
    int sz;

    if (screen >= GFX_INVALID) {
        return;
    }

    fb = (uint32_t*)framebuffers[screen];
    sz = (fb_width[screen]*GFX_HEIGHT*2)/4;

    fill = (rgb << 16) | rgb;
    while(sz--) {
        *(fb++) = fill;
    }

    return;
}
