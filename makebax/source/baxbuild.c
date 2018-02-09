#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "bax.h"
#include "main.h"
#include "ringbuf.h"

void *BAXBuildThread(void *main_state)
{
    MainState *state;
    RingBuf *fetch_ring;
    const char *bax_path;

    FILE *bax_file;
    BAX_Header *bax_hdr;
    BAX_FrameInfo *frame_info;

    void *frame_data;
    size_t frame_size, bax_size;

    state = (MainState*)main_state;
    fetch_ring = state->ring[2];
    bax_path = state->out_path;

    bax_file = fopen(bax_path, "wb+");
    if (bax_file == NULL)
        abort_error("Couldn't create \"%s\"\n", bax_path);

    while(state->state < 0) thread_yield();

    bax_hdr = BAX_Init(
        state->frame_count, state->frame_rate,
        state->background_color, state->frame_offset, state->frame_width,
        state->author, state->info);

    frame_info = BAX_FrameInfoInit(bax_hdr);
    bax_size = BAX_DataStart(bax_hdr);

    for (size_t i = 0; i < state->frame_count; i++) {
        while(!RingBuffer_Fetch(fetch_ring, &frame_data, &frame_size)) thread_yield();

        fseek(bax_file, bax_size, SEEK_SET);
        BAX_FrameInfoSet(frame_info, i, ftell(bax_file), frame_size);

        if (fwrite(frame_data, frame_size, 1, bax_file) != 1)
            abort_error("Couldn't write to output file!\n");

        free(frame_data);

        bax_size += frame_size;
        bax_size = align_up(bax_size, 8);

        printf("Wrote frame %d / %d (%d KiB)\r", i, state->frame_count, bax_size / 1024);
    }

    fseek(bax_file, 0, SEEK_SET);
    fwrite(bax_hdr, BAX_HeaderSize(), 1, bax_file);
    fwrite(frame_info, BAX_FrameInfoSize(bax_hdr), 1, bax_file);
    fclose(bax_file);

    BAX_FrameInfoKill(frame_info);
    BAX_Kill(bax_hdr);
    pthread_exit(NULL);
}
