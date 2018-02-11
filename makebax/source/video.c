#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "vpx.h"
#include "main.h"
#include "ringbuf.h"

void *VideoDecodeThread(void *main_state)
{
    MainState *state;
    RingBuf *store_ring;
    VPX_State *vpx;

    state = (MainState*)main_state;
    store_ring = state->ring[0];

    vpx = VPX_Init(state->in_path);
    if (vpx == NULL)
        abort_error("Failed to open/parse IVF\n");

    state->frame_rate  = VPX_Framerate(vpx);
    state->frame_count = VPX_Framecount(vpx);

    if (VPX_Height(vpx) != FRAME_H ||
        (VPX_Width(vpx) != 400 && VPX_Width(vpx) != 320 && VPX_Width(vpx) != 720)) {
        abort_error("Invalid size %dx%d\n", VPX_Width(vpx), VPX_Height(vpx));
    }

    state->frame_width  = VPX_Width(vpx);
    state->frame_offset = (VPX_Width(vpx) == 320) ? 400 : 0;
    state->state = 0;

    for (size_t i = 0; i < state->frame_count;) {
        uint8_t **rgb_buffers;
        size_t  rgb_buf_size;
        int rgb_frames;

        rgb_buffers = VPX_DecodeFrame(vpx, &rgb_buf_size, &rgb_frames);

        if ((i + rgb_frames) > state->frame_count)
            abort_error("Decode overflow\n");

        if (rgb_frames == 0)
            abort_error("Error while decoding?\n");

        for (int j = 0; j < rgb_frames; j++) {
            if (rgb_buffers[j] == NULL)
                abort_error("Bad frame %d %d\n", i, j);

            while(!RingBuffer_Store(store_ring, rgb_buffers[j], rgb_buf_size)) thread_yield();
        }

        free(rgb_buffers);
        i += rgb_frames;
    }

    VPX_Kill(vpx);

    pthread_exit(NULL);
}
