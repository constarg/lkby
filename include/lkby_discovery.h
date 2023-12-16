#ifndef LKBY_DISCOVERY
#define LKBY_DISCOVERY

#include "lkby_queue.h"

// This queue is used to store the discovered keyboards and notify the scheduler.
extern struct lkbyqueue_sync g_keyboard_queue; // Declared in main.


/**
 * This function is responsible to do the job of the
 * thread that searches for new keyboards.
 */
extern void *lkby_start_discovery(void *none __attribute__((unused)));


#endif
