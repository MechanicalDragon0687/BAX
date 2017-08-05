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
    struct qn *n = malloc(sizeof(struct qn));
    if (!n) {
        return false;
    }
    n->data = data;
    n->next = NULL;

    if (q->first) {
        q->last->next = n;
        q->last = n;
    } else {
        q->first = n;
        q->last = n;
    }
    q->count++;
    return true;
}

void *vq_fetch(vq *q)
{
    return q->first->data;
}

void vq_remove(vq *q)
{
    struct qn *n = NULL;
    if (vq_count(q) > 0) {
        n = q->first;
        q->first = q->first->next;
        q->count--;
        free(n);

        if (q->count == 0) {
            q->first = NULL;
            q->last  = NULL;
        }
    }
    return;
}

u32 vq_count(vq *q)
{
    return q->count;
}
