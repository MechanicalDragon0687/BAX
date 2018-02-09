#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "main.h"
#include "ringbuf.h"

void *VideoDecodeThread(void *main_state)
{
    MainState *state;
    RingBuf *store_ring;
    const char *vid_path;

    state = (MainState*)main_state;

    state->frame_rate = 20;
    state->frame_count = 200;
    state->frame_width = 400;
    state->frame_offset = 0;

    state->state = 0;

    store_ring = state->ring[0];

    for (size_t i = 0; i < 200; i++) {
        uint16_t *buf = malloc(400 * 240 * 2);
        for (int j = 0; j < 400 * 240; j++)
            buf[j] = i + j;
        while(!RingBuffer_Store(store_ring, buf, 400 * 240 * 2)) thread_yield();
    }

    pthread_exit(NULL);
}
