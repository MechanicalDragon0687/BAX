#include <common.h>
#include <gfx/gfx.h>

const int fb_width[] = {400, 400, 320};

void clear_screen(const int screen, const uint16_t rgb)
{
    uint32_t *fb, fill;
    int sz;

    fb = (uint32_t*)get_framebuffer(screen);
    sz = fb_width[screen]*GFX_HEIGHT/2;

    fill = (rgb << 16) | rgb;
    while(sz --) {
        *(fb++) = fill;
    }

    return;
}
