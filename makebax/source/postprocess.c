#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "main.h"
#include "ringbuf.h"

// Dumb transpose algorithm, definitely needs to be replaced
static inline void TransposeFramebuffer(uint16_t *dest, const uint16_t *src, size_t w, size_t h)
{
    #pragma omp parallel for
    for (size_t x = 0; x < w; x++) {
        for (size_t y = 0; y < h; y++) {
            dest[(x* h) + (h - 1 - y)] = src[y * w + x];
        }
    }
    return;
}

void *PostProcessThread(void *main_state)
{
    MainState *state;
    RingBuf *fetch_ring, *store_ring;
    uint32_t *backbuffer, *newframe, *newframe_trans;
    size_t frame_size;

    state = (MainState*)main_state;
    fetch_ring = state->ring[0];
    store_ring = state->ring[1];

    backbuffer = malloc(FRAME_SIZE_MAX);
    if (backbuffer == NULL)
        abort_error("Couldn't allocate Delta backbuffer! (%d bytes)\n", FRAME_SIZE_MAX / 1024);
    memset(backbuffer, 0, FRAME_SIZE_MAX);

    while(state->state < 0) thread_yield();

    for (size_t i = 0; i < state->frame_count; i++) {
        while(!RingBuffer_Fetch(fetch_ring, (void**)&newframe, &frame_size)) thread_yield();

        newframe_trans = malloc(frame_size);
        assert(newframe_trans != NULL);

        TransposeFramebuffer(
            (uint16_t*)newframe_trans, (const uint16_t*)newframe,
            state->frame_width, FRAME_H);

        free(newframe);

        #pragma omp parallel for
        for (size_t j = 0; j < (frame_size / sizeof(uint32_t)); j++) {
            uint32_t delta = newframe_trans[j] - backbuffer[j];
            backbuffer[j] = newframe_trans[j];
            newframe_trans[j] = delta;
        }
        while(!RingBuffer_Store(store_ring, (void*)newframe_trans, frame_size)) thread_yield();
    }

    free(backbuffer);
    pthread_exit(NULL);
}
