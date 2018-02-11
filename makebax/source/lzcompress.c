#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <lz4.h>
#include <lz4hc.h>

#include "main.h"
#include "ringbuf.h"

void *LZ4CompressThread(void *main_state)
{
    MainState *state;
    RingBuf *fetch_ring, *store_ring;

    void *raw_buffer, *comp_buffer;
    size_t raw_size, comp_size, max_comp_size;

    state = (MainState*)main_state;

    fetch_ring = state->ring[1];
    store_ring = state->ring[2];

    while(state->state < 0) thread_yield();

    for (size_t i = 0; i < state->frame_count; i++) {
        while(!RingBuffer_Fetch(fetch_ring, &raw_buffer, &raw_size)) thread_yield();

        max_comp_size = LZ4_compressBound(raw_size);
        comp_buffer = malloc(max_comp_size);
        if (comp_buffer == NULL)
            abort_error("Failed to allocate compression buffer %d!\n", i);

        if (state->compression_lvl > LZ4_DEF_COMP) {
            comp_size = LZ4_compress_HC(
                (const char*)raw_buffer, (char*)comp_buffer,
                raw_size, max_comp_size, state->compression_lvl);
        } else {
            comp_size = LZ4_compress_default(
                (const char*)raw_buffer, (char*)comp_buffer,
                raw_size, max_comp_size);
        }

        if (comp_size <= 0)
            abort_error("Failed to compress frame %d!\n", i);

        free(raw_buffer);
        while(!RingBuffer_Store(store_ring, comp_buffer, comp_size)) thread_yield();
    }

    pthread_exit(NULL);
}
