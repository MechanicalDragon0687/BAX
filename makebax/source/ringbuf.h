#ifndef RINGBUF_H
#define RINGBUF_H

typedef struct RingBuf RingBuf;

RingBuf *RingBuffer_Init(size_t count);
void RingBuffer_Kill(RingBuf *rb);

size_t RingBuffer_Count(RingBuf *rb);

int RingBuffer_Store(RingBuf *rb, void *data, size_t datalen);
int RingBuffer_Fetch(RingBuf *rb, void **data, size_t *datalen);

#endif
