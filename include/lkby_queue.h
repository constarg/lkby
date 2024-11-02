#ifndef LKBY_QUEUE_H
#define LKBY_QUEUE_H

#include <memory.h>
#include <malloc.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

#include "lkby.h"

#include <stdio.h>

/**
 * This macro retrieves the queue from
 * the synchronization struct.
 */
#define LKBYQUEUE(sync) \
    (sync)->s_queue

/**
 * This macro retrieves the semaphore 
 * used to synchronize the two threads
 * for a specific queue.
 */
#define LKBYQUEUE_SEM(sync) \
    (sync)->s_sem

struct lkbyqueue_node
{
    union lkby_info data;
    struct lkbyqueue_node *next;
};

struct lkbyqueue
{
    struct lkbyqueue_node *front;
    struct lkbyqueue_node *rear;
};

/**
 * This structure contains informations
 * in order to synchronize the threads
 * whitch is using queues.
 */
struct lkbyqueue_sync
{
    struct lkbyqueue s_queue; // The queue that need synchronization.
    sem_t s_sem;              // The semaphore that synchronize the queue.
};

static inline void lkbyqueue_init(struct lkbyqueue *queue)
{
    (void) memset(queue, 0x0, sizeof(struct lkbyqueue));
}

static inline int lkbyqueue_sync_init(struct lkbyqueue_sync *s_queue)
{
    lkbyqueue_init(&LKBYQUEUE(s_queue));
    return sem_init(&LKBYQUEUE_SEM(s_queue), 0, 0);
}

/**
 * This function removes an element from the queue. After the call of this
 * function the element at the front of the queue will be removed and
 * it's contents will be placed on data.
 *
 * @param dst Where the removed data will be placed.
 * @param queue The queue of interest.
 */
void lkbyqueue_dequeue(union lkby_info *restrict dst, struct lkbyqueue *restrict queue);

/**
 * This function check whether the queue is empty or not.
 * @param queue The queue to check.
 * @returns True if the queue is empty, otherwise false.
*/
bool lkbyqueue_isempty(const struct lkbyqueue *queue);

/**
 * This functiono add an element into the queue. After the call of this
 * function an element will be placed at the end of the queue.
 *
 * @param queue The queue of interest.
 * @param src The data to store into the queue.
 */
void lkbyqueue_enqueue(struct lkbyqueue *restrict queue, const union lkby_info *restrict src);

#endif
