#ifndef LKBY_DISCOVERY
#define LKBY_DISCOVERY

#include "lkby_queue.h"


extern struct lkbyqueue_sync g_transmit_queue;


/**
 * This function is responsible to do the job of the
 * thread that searches for new keyboards.
 */
extern void *lkby_start_discovery(void *none);


#endif
