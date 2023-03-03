#ifndef LKBY_QUEUE_H
#define LKBY_QUEUE_H

#include "lkby.h"


struct lkbyqueue_node 
{
    union lkby_info data;
    struct lkbyqueue_node *link;
};

struct lkbyqueue 
{
    lkbyqueue_node *front;
    lkbyqueue_node *rear;
};

static inline void lkbyqueue_init(struct lkbyqueue *queue) 
{
    memset(queue, 0x0, sizeof(struct lkbyqueue));
}

static inline void lkbyqueue_dequeue(union lkby_info *data, struct lkbyqueue *queue)
{

}

static inline void lkbyqueue_enqueue(void *data, )
{
}

#endif
