#ifndef LKBY_QUEUE_H
#define LKBY_QUEUE_H

#include <memory.h>
#include <malloc.h>

#include "lkby.h"


struct lkbyqueue_node 
{
    union lkby_info data;
    struct lkbyqueue_node *next;
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

/**
 * This function removes an element from the queue. After the call of this
 * function the element at the front of the queue will be removed and
 * it's contents will be placed on data.
 *
 * @param dst Where the removed data will be placed.
 * @param queue The queue of interest.
 */
static inline void lkbyqueue_dequeue(union lkby_info *dst, struct lkbyqueue *queue)
{
    if (NULL == queue->front) return;
    // save the pointer to the data to be removed.
    struct lkbyqueue_node *tmp = queue->front;
    // remove the data from the queue.
    queue->front = tmp->next;
    // place the data.
    memcpy(dst, &tmp->data, sizeof(union lkby_info));
    // free the node.
    free(tmp);
}

/**
 * This functiono add an element into the queue. After the call of this
 * function an element will be placed at the end of the queue.
 *
 * @param queue The queue of interest.
 * @param src The data to store into the queue.
 */
static inline void lkbyqueue_enqueue(struct lkbyqueue *queue, const union lkby_info *src)
{
    // create the new node.
    struct lkbyqueue_node *new_node = (struct lkbyqueue_node *) malloc(sizeof(struct lkbyqueue_node));
    if (NULL == new_node) return;
    // set new_node next as NULL.
    new_node->next = NULL;
    // new node data.
    memcpy(&new_node->data, src, sizeof(union lkby_info));
    // set the new node at the end of the queue.
    if (NULL == queue->front) {
        queue->front = new_node;
        queue->rear  = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
}

#endif
