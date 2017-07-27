#pragma once
#include <common.h>

struct frame_s {
    size_t length;
    void *dest_fb;
    void *frame_data;
    struct frame_s *next;
};

typedef struct frame_queue_s {
    size_t frame_cnt;
    struct frame_s *first;
    struct frame_s *last;
} frame_queue_s;

typedef frame_queue_s *frame_queue;

frame_queue frame_queue_init(void);
void frame_queue_kill(frame_queue queue);
void frame_queue_add(frame_queue queue, size_t flen, void *dest, void *data);
void *frame_queue_extract(frame_queue queue);

static inline size_t frame_queue_count(frame_queue queue)
{
    return queue->frame_cnt;
}

static inline size_t frame_length(void *frame)
{
    return ((struct frame_s*)frame)->length;
}

static inline void *frame_dest(void *frame)
{
    return ((struct frame_s*)frame)->dest_fb;
}

static inline void *frame_data(void *frame)
{
    return ((struct frame_s*)frame)->frame_data;
}

static inline void frame_free(void *frame)
{
    free(frame);
    return;
}
