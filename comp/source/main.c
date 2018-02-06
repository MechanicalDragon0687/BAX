#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

#include <lz4.h>
#include <pthread.h>

#include "bax.h"
#include "ringbuf.h"

#define BAXCOMP_VERSION "0.0a"

#define WORKRING_DEFWIDTH (1024)
#define WORKRING_MINWIDTH (4)
#define WORKRING_MAXWIDTH (32768)

#define FRAME_SIZE_H   (240 * 2)
#define FRAME_SIZE_MAX (720 * FRAME_SIZE_H)

// Ugly hack...
#define THREAD_YIELD    usleep(200)

/*
 Usage:
 comp `input.ivf` `output.bax`
*/

/*
 How all this works together:

    - decompress frames from codec to RGB565 through libavcodec magic
    - delta encoding and transposing (preferably) in-buffer
    - compress the frame with LZ4
    - write the frames to the output BAX file
*/

static size_t AnimOffset, AnimWidth;
static int FrameCount = -1, FrameRate = -1;
static RingBuf *WorkRing[3];
static pthread_t WorkThread[4];

static void abort_error(const char *str, ...)
{
    va_list va;
    va_start(va, str);
    vprintf(str, va);
    va_end(va);
    printf("Aborting...\n");

    exit(EXIT_FAILURE);
}

void *VideoDecodeThread(void *vid_data)
{
    // screw libav for now, it's a bit of a mess
    AnimOffset = 0;
    AnimWidth = 400;
    FrameRate = 30;
    FrameCount = 900;

    for (int i = 0; i < FrameCount; i++) {
        uint16_t *buffer = malloc(AnimWidth * FRAME_SIZE_H);
        assert(buffer != NULL);
        for (int j = 0; j < AnimWidth * FRAME_SIZE_H / 2; j++) {
            buffer[j] = 0xF800;
        }
        while(!RingBuffer_Store(WorkRing[0], buffer, AnimWidth * FRAME_SIZE_H)) THREAD_YIELD;
    }

    pthread_exit(NULL);
}

void *PostProcessThread(void *data)
{
    // TODO: transpose
    size_t *backbuffer = malloc(FRAME_SIZE_MAX),
           *newframe, frame_size;

    if (backbuffer == NULL)
        abort_error("Couldn't allocate Delta backbuffer! (%d bytes)\n", FRAME_SIZE_MAX / 1024);
    memset(backbuffer, 0, FRAME_SIZE_MAX);

    while(FrameCount < 0) THREAD_YIELD;
    for (int i = 0; i < FrameCount; i++) {
        while(!RingBuffer_Fetch(WorkRing[0], (void**)&newframe, &frame_size)) THREAD_YIELD;
        for (int j = 0; j < (frame_size / sizeof(size_t)); j++) {
            size_t delta = newframe[j] - backbuffer[j];
            backbuffer[j] = newframe[j];
            newframe[j] = delta;
        }
        while(!RingBuffer_Store(WorkRing[1], (void*)newframe, frame_size)) THREAD_YIELD;
    }

    free(backbuffer);
    pthread_exit(NULL);
}

void *LZ4CompressThread(void *data)
{
    size_t *compressed_buffer, *uncompressed_buffer;
    size_t compressed_size, uncompressed_size;
    while(FrameCount < 0) THREAD_YIELD;

    for (int i = 0; i < FrameCount; i++) {
        while(!RingBuffer_Fetch(WorkRing[1], (void**)&uncompressed_buffer, &uncompressed_size)) THREAD_YIELD;
        compressed_buffer = malloc(LZ4_compressBound(uncompressed_size));
        if (compressed_buffer == NULL)
            abort_error("Failed to allocate compression buffer %d!\n", i);
        compressed_size = LZ4_compress_default(
            (const char*)uncompressed_buffer, (char*)compressed_buffer,
            uncompressed_size, LZ4_compressBound(uncompressed_size));

        if (compressed_size <= 0)
            abort_error("Failed to compress frame %d!\n", i);
        free(uncompressed_buffer);
        while(!RingBuffer_Store(WorkRing[2], (void*)compressed_buffer, compressed_size)) THREAD_YIELD;
    }

    pthread_exit(NULL);
}

void *BAXBuildThread(void *fileout)
{
    FILE *bax_file = (FILE*)fileout;
    BAX_Header *bax_hdr;
    BAX_FrameInfo *frame_info;
    void *compressed_frame;
    size_t compressed_size, total_size;

    while(FrameCount < 0 || FrameRate < 0) THREAD_YIELD;
    bax_hdr = BAX_Init(FrameCount, FrameRate, 0, AnimOffset, AnimWidth, NULL, NULL);
    frame_info = BAX_FrameInfoInit(bax_hdr);
    total_size = BAX_DataStart(bax_hdr);

    for (int i = 0; i < FrameCount; i++) {
        while(!RingBuffer_Fetch(WorkRing[2], &compressed_frame, &compressed_size)) THREAD_YIELD;

        fseek(bax_file, total_size, SEEK_SET);
        BAX_FrameInfoSet(frame_info, i, ftell(bax_file), compressed_size);
        if (fwrite(compressed_frame, compressed_size, 1, bax_file) != 1)
            abort_error("Couldn't write to output file!\n");
        free(compressed_frame);
        total_size += compressed_size;
        printf("Writing frame %d / %d (%d KiB)\r", i, FrameCount, total_size / 1024);
    }
    printf("\n");

    fseek(bax_file, 0, SEEK_SET);
    fwrite(bax_hdr, BAX_HeaderSize(), 1, bax_file);
    fwrite(frame_info, BAX_FrameInfoSize(bax_hdr), 1, bax_file);

    BAX_FrameInfoKill(frame_info);
    BAX_Kill(bax_hdr);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    FILE *in, *out;
    void *vid_buffer;
    size_t insize, outsize;
    int workring_width = 0;

    printf("BAXcomp v" BAXCOMP_VERSION "\n");
    if (argc < 3)
        abort_error("Usage: %s input.{mp4,mkv,webm,etc} output.bax [lvl]\n", argv[0]);

    if (argc >= 4)
        workring_width = strtol(argv[3], NULL, 0);

    if (workring_width <= WORKRING_MINWIDTH || workring_width >= WORKRING_MAXWIDTH)
        workring_width = WORKRING_DEFWIDTH;

    in = fopen(argv[1], "rb+");
    if (in == NULL)
        abort_error("Couldn't open \"%s\"\n", argv[1]);

    out = fopen(argv[2], "wb+");
    if (out == NULL)
        abort_error("Couldn't open \"%s\"\n", argv[2]);

    fseek(in, 0L, SEEK_END);
    insize = ftell(in);
    fseek(in, 0L, SEEK_SET);

    vid_buffer = malloc(insize);
    if (fread(vid_buffer, insize, 1, in) != 1)
        abort_error("Couldn't read from input file!\n");
    fclose(in);

    for (int i = 0; i < 3; i++)
        WorkRing[i] = RingBuffer_Init(workring_width);

    assert(pthread_create(&WorkThread[0], NULL, VideoDecodeThread, vid_buffer) == 0);
    assert(pthread_create(&WorkThread[1], NULL, PostProcessThread, NULL) == 0);
    assert(pthread_create(&WorkThread[2], NULL, LZ4CompressThread, NULL) == 0);
    assert(pthread_create(&WorkThread[3], NULL, BAXBuildThread, out) == 0);

    pthread_join(WorkThread[0], NULL);
    pthread_join(WorkThread[1], NULL);
    pthread_join(WorkThread[2], NULL);
    pthread_join(WorkThread[3], NULL);
    fclose(out);

    for (int i = 0; i < 3; i++)
        RingBuffer_Kill(WorkRing[i]);

    free(vid_buffer);
    printf("Done\n");
    return 0;
}
