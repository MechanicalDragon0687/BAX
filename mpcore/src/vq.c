#include <common.h>
#include <stdlib.h>
#include <vq.h>

void vq_init(vq *q)
{
    q->first = NULL;
    q->last  = NULL;
    q->count = 0;
    return;
}

bool vq_add(vq *q, void *data)
{
    struct qn *node = malloc(sizeof(struct qn));
    if (!node) {
        return false;
    }
    node->data = data;
    node->next = NULL;

    if (q->first) {
        q->last->next = node;
        q->last = node;
    } else {
        q->first = node;
        q->last = node;
    }
    q->count++;
    return true;
}

void *vq_fetch(vq *q)
{
    void *ret = NULL;
    if (q->count > 0) {
        ret = q->first->data;
        q->first = q->first->next;
        free(q->first);

        if (q->count == 0) {
            q->first = NULL;
            q->last  = NULL;
        }
    }
    q->count--;
    return ret;
}

u32 vq_count(vq *q)
{
    return q->count;
}
