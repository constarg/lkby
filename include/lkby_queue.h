#ifndef LKBY_QUEUE_H
#define LKBY_QUEUE_H

#include <memory.h>
#include <stdlib.h>

#define LKBYQUEUE_PEEK(queue, type) \
    *((type *) (queue)->front->data);


struct lkbyqueue_node 
{
    void *data;
    struct lkbyqueue_node *link;
};

struct lkbyqueue 
{
    lkbyqueue_node *front;
    lkbyqueue_node *rear;
};

static inline void lkbyqueue_init(struct lkbyqueue *queue) 
{
    memset(queue, 0x0, sizeof(lkbyqueue));
}

static inline void lkbyqueue_dequeue(void *data, size_t s_data, 
                                     struct lkbyqueue *queue)
{
    if (queue->front == NULL) return;

    struct lkbyqueue_node *tmp = queue->front->link;
    // return the data.
    memcpy(data, queue->front->data, s_data);

    free(queue->front->data);
    free(queue->front);
    // look into the next element.
    queue->front = tmp;
}

static inline void lkbyqueue_enqueue(void *data, )
{
}

#endif
