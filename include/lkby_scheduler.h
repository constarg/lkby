#ifndef LKBY_KB_SCHEDULER_H
#define LKBY_KB_SCHEDULER_H

#include "lkby.h"

// This queue contains the information to transmit.
extern struct lkbyqueue_sync g_transmit_queue;

/**
 * 
 */
extern void *lkby_start_scheduler(void *none);


#endif