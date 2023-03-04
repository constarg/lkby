#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "lkby_scheduler.h"
#include "lkby_transmitter.h"
#include "lkby_discovery.h"
#include "lkby_queue.h"


int main()
{
    // declare the queues.
    struct lkbyqueue_sync sched_queue;    // The schedule queue.
    struct lkbyqueue_sync transmit_queue; // The queue used to transmit data.
    // declare the threads.
    pthread_t sched_th;    // The schedule thread.
    pthread_t transmit_th; // The transmitter thread.
    pthread_t discov_th;   // The discovery thread.

    // initialize queues.
    if (lkbyqueue_sync_init(&sched_queue) != 0) return -1;
    if (lkbyqueue_sync_init(&transmit_queue) != 0) return -1;

    lkby_start_discovery(&sched_queue);

    return 0;
}
