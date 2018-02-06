#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include <pthread.h>

typedef struct RingBuf {
    void  **data;
    size_t *datalen;

    size_t fetch;
    size_t store;
    size_t count;
    size_t max;

    pthread_mutex_t lock;
} RingBuf;

RingBuf *RingBuffer_Init(size_t count)
{
    RingBuf *ret = malloc(sizeof(RingBuf));
    assert(ret != NULL);

    ret->data = malloc(sizeof(void*) * count);
    ret->datalen = malloc(sizeof(size_t) * count);

    assert(ret->data != NULL);
    assert(ret->datalen != NULL);

    ret->fetch = 0;
    ret->store = 0;
    ret->count = 0;
    ret->max = count;
    pthread_mutex_init(&ret->lock, NULL);

    return ret;
}

void RingBuffer_Kill(RingBuf *rb)
{
    assert(rb != NULL);
    assert(rb->data != NULL);
    assert(rb->datalen != NULL);

    pthread_mutex_destroy(&rb->lock);
    free(rb->datalen);
    free(rb->data);
    free(rb);
    return;
}

int RingBuffer_Store(RingBuf *rb, void *data, size_t datalen)
{
    int ret = 0;
    pthread_mutex_lock(&rb->lock);
    if (rb->count < rb->max) {
        rb->data[rb->store] = data;
        rb->datalen[rb->store] = datalen;
        rb->store++;
        if (rb->store >= rb->max)
            rb->store = 0;
        ret = 1;
        rb->count++;
    }
    pthread_mutex_unlock(&rb->lock);
    return ret;
}

int RingBuffer_Fetch(RingBuf *rb, void **data, size_t *datalen)
{
    int ret = 0;
    pthread_mutex_lock(&rb->lock);
    if (rb->count > 0) {
        if (data) *data = rb->data[rb->fetch];
        if (datalen) *datalen = rb->datalen[rb->fetch];
        rb->fetch++;
        if (rb->fetch >= rb->max)
            rb->fetch = 0;
        ret = 1;
        rb->count--;
    }
    pthread_mutex_unlock(&rb->lock);
    return ret;
}
