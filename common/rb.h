#pragma once

#include <common.h>

#define FRB_MAX (256)

typedef struct {
    u32 *data[FRB_MAX];
    u32 data_len[FRB_MAX];

    u32 store;
    u32 fetch;
    u32 count;
} rb_t;

static inline u32 rb_count(rb_t *rb)
{
    return rb->count;
}

static inline void rb_init(rb_t *rb)
{
    memset(rb, 0, sizeof(rb_t));
    return;
}

static inline bool rb_store(rb_t *rb, u32 *data, u32 data_len)
{
    bool ret = false;
    if (rb_count(rb) < FRB_MAX) {
        rb->data[rb->store] = data;
        rb->data_len[rb->store] = data_len;
        rb->store = (rb->store + 1) % FRB_MAX;
        rb->count++;
        ret = true;
    }
    return ret;
}

static inline u32 *rb_fetch(rb_t *rb, u32 *size)
{
    u32 *ret = NULL;
    if (rb_count(rb) > 0) {
        ret = rb->data[rb->fetch];
        if (size) *size = rb->data_len[rb->fetch];
        rb->fetch = (rb->fetch + 1) % FRB_MAX;
        rb->count--;
    }
    return ret;
}
