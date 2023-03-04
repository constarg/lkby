#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "lkby_scheduler.h"
#include "lkby_transmitter.h"
#include "lkby_queue.h"


int main()
{
    // declare the queues.
    struct sync_queue sched_queue;    // The schedule queue.
    struct sync_queue transmit_queue; // The queue used to transmit data.
    // declare the threads.
    pthread_t sched_th;    // The schedule thread.
    pthread_t transmit_th; // The transmitter thread.
    pthread_t discov_th;   // The discovery thread.

    // initialize queues.
    lkbyqueue_sync_init(&sched_queue);
    lkbyqueue_sync_init(&transmit_queue);

}
