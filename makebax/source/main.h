#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include "ringbuf.h"
#include "bax.h"

#define BAXCOMP_VERSION "0.0a"

#define WORKRING_MIN_SIZE (4)
#define WORKRING_DEF_SIZE (1024)
#define WORKRING_MAX_SIZE (32768)


// Latest LZ4 does include minimum and maximum compression levels
// but this'll have to do (even buster/sid doesn't have them!)
#define LZ4_MIN_COMP   (0)
#define LZ4_DEF_COMP   (9)
#define LZ4_MAX_COMP   (16)

#define FRAME_H        (240)
#define FRAME_SIZE_H   (FRAME_H * 2)
#define FRAME_SIZE_MAX (FRAME_SIZE_H * 720)

typedef struct MainState {
    const char *in_path;
    const char *out_path;

    const char *author;
    const char *info;

    RingBuf *ring[3];
    pthread_t thread[4];

    int compression_lvl;

    volatile int state;

    uint16_t background_color;

    size_t frame_rate;
    size_t frame_count;
    size_t frame_width;
    size_t frame_offset;
} MainState;

static inline void abort_error(const char *str, ...)
{
    va_list va;
    va_start(va, str);
    vprintf(str, va);
    va_end(va);
    printf("Aborted\n");

    exit(EXIT_FAILURE);
}

// Ugly hack that needs to go away ASAP
static inline void thread_yield(void)
{
    usleep(2000);
}

static inline size_t align_up(size_t n, size_t a)
{
    if ((a & (a - 1)) == 0) {
        n += (a - 1);
        n &= ~(a - 1);
    } else if ((n % a) != 0) {
        n = (((n + a) / a) * a);
    }
    return n;
}

static inline size_t align_down(size_t n, size_t a)
{
    if ((a & (a - 1)) == 0) {
        n &= ~(a - 1);
    } else {
        n -= (n % a);
    }
    return n;
}

#endif