/** kb_scheduler.h */
#ifndef LKBY_KB_SCHEDULER_H
#define LKBY_KB_SCHEDULER_H

// TODO - synchronize the discovery thread and kb_scheduler
#include "lkby.h"

/**
 * This function starts the thread that is responsible
 * to schedule every available keyboard.
 *
 * @param sched_queue The queue that contains all the available keyboards.
 */
extern void *lkby_start_scheduler(void *sched_queue);



#endif
