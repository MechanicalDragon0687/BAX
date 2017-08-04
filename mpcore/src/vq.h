#include <common.h>

struct qn {
    void *data;
    struct qn *next;
};

typedef volatile struct {
    u32 count;
    struct qn *first;
    struct qn *last;
} vq;

void vq_init(vq *q);
bool vq_add(vq *q, void *data);
void *vq_fetch(vq *q);
u32 vq_count(vq *q);
