#pragma once

struct framebuffer_t { // Thanks to mid-kid for the framebuffer structure
    unsigned char *top_left;
    unsigned char *top_right;
    unsigned char *bottom;
};
