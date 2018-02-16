#pragma once

#include <common.h>
#include <cpu.h>

// TODO: Fine grained locking between store and fetch
typedef struct {
    void  **Data;
    size_t *DataSize;

    size_t Store;
    size_t Fetch;
    size_t Count;
    size_t MSize;

    CritStat Lock;
} RingBuffer;

void RingBuffer_Init(RingBuffer *r, size_t n);
void RingBuffer_Destroy(RingBuffer *r);

bool RingBuffer_Store(RingBuffer *r, void *d, size_t l);
bool RingBuffer_Fetch(RingBuffer *r, void **d, size_t *l);

static inline size_t RingBuffer_Count(RingBuffer *r)
{
    return r->Count;
}

