#include <common.h>
#include <anim/frame_queue.h>

/* Simple FIFO Queue */

frame_queue frame_queue_init(void)
{
    frame_queue ret = malloc(sizeof(frame_queue_s));
    ret->frame_cnt = 0;
    ret->first = NULL;
    ret->last = NULL;
    return ret;
}

void frame_queue_kill(frame_queue queue)
{
    void *data;
    while(frame_queue_count(queue)) {
        data = frame_queue_extract(queue);
        free(frame_data(data));
        frame_free(data);
    }
    free(queue);
    return;
}

void frame_queue_add(frame_queue queue, size_t flen, void *dest, void *data)
{
    struct frame_s *n = malloc(sizeof(struct frame_s));

    n->length = flen;
    n->dest_fb = dest;
    n->frame_data = data;
    n->next = NULL;

    if (queue->first == NULL) {
        queue->first = n;
    } else {
        queue->last->next = n;
    }

    queue->last = n;
    queue->frame_cnt++;
    return;
}

/* `extract` assumes frame_queue_count > 0 */
void *frame_queue_extract(frame_queue queue)
{
    void *ret = queue->first;
    queue->first = queue->first->next;
    if (queue->first == NULL) {
        queue->last = NULL;
    }
    queue->frame_cnt--;
    return ret;
}
