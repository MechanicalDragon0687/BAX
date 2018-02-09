#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "main.h"
#include "ringbuf.h"

void *PostProcessThread(void *main_state)
{
    MainState *state;
    RingBuf *fetch_ring, *store_ring;
    size_t *backbuffer, *newframe;
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
        for (size_t j = 0; j < (frame_size / sizeof(size_t)); j++) {
            size_t delta = newframe[j] - backbuffer[j];
            backbuffer[j] = newframe[j];
            newframe[j] = delta;
        }
        while(!RingBuffer_Store(store_ring, (void*)newframe, frame_size)) thread_yield();
    }

    free(backbuffer);
    pthread_exit(NULL);
}
