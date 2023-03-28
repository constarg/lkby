#include "lkby_queue.h"

void lkbyqueue_dequeue(union lkby_info *dst, struct lkbyqueue *queue)
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

void lkbyqueue_enqueue(struct lkbyqueue *queue, const union lkby_info *src)
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

