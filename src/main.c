#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "lkby_scheduler.h"
#include "lkby_transmitter.h"
#include "lkby_discovery.h"
#include "lkby_queue.h"

// declare global queues.
struct lkbyqueue_sync g_sched_queue;    // The schedule queue.
struct lkbyqueue_sync g_transmit_queue; // The queue used to transmit data.


int main()
{
    // declare the threads.
    pthread_t sched_th;    // The schedule thread.
    pthread_t transmit_th; // The transmitter thread.
    pthread_t discov_th;   // The discovery thread.

    // initialize queues.
    if (lkbyqueue_sync_init(&g_sched_queue) != 0) return -1;
    if (lkbyqueue_sync_init(&g_transmit_queue) != 0) return -1;

    lkby_start_discovery(NULL);

    return 0;
}
