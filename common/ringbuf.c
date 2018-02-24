#include <common.h>
#include <ringbuf.h>

void RingBuffer_Init(RingBuffer *r, size_t n)
{
    CritStat cs = CPU_EnterCritical();
    memset(r, 0, sizeof(RingBuffer));

    r->Data = malloc(sizeof(void*) * n);
    assert(r->Data != NULL);

    r->DataSize = malloc(sizeof(size_t) * n);
    assert(r->DataSize != NULL);

    r->Store = 0;
    r->Fetch = 0;
    r->Count = 0;
    r->MSize = n;
    CPU_LeaveCritical(cs);
}

void RingBuffer_Destroy(RingBuffer *r)
{
    CritStat cs = CPU_EnterCritical();
    assert(r != NULL);
    assert(r->Data != NULL);
    assert(r->DataSize != NULL);

    free(r->Data);
    free(r->DataSize);
    CPU_LeaveCritical(cs);
}

bool RingBuffer_Store(RingBuffer *r, void *d, size_t l)
{
    CritStat cs;
    bool ret = false;

    cs = CPU_EnterCritical();
    if (r->Count < r->MSize) {
        ret = true;
        r->Data[r->Store]     = d;
        r->DataSize[r->Store] = l;
        r->Store++;

        if (r->Store >= r->MSize)
            r->Store = 0;

        r->Count++;
    }
    CPU_LeaveCritical(cs);

    return ret;
}

bool RingBuffer_Fetch(RingBuffer *r, void **d, size_t *l)
{
    CritStat cs;
    bool ret = false;

    cs = CPU_EnterCritical();
    if (r->Count > 0) {
        ret = true;
        if (d) *d = r->Data[r->Fetch];
        if (l) *l = r->DataSize[r->Fetch];
        r->Fetch++;

        if (r->Fetch >= r->MSize)
            r->Fetch = 0;

        r->Count--;
    }
    CPU_LeaveCritical(cs);

    return ret;
}
