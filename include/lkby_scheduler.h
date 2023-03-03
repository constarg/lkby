/** kb_scheduler.h */
#ifndef LKBY_KB_SCHEDULER_H
#define LKBY_KB_SCHEDULER_H

// TODO - synchronize the discovery thread and kb_scheduler

/**
 * This structure consists of the
 * informations of the current discovered
 * keyboard
 */
struct lkby_keyboard
{
    char *kb_name;   // The name of the keyboard that occure an event.
    char *kb_event;  // The associated event.  
};

/**
 * This function starts the thread that is responsible
 * to schedule every available keyboard.
 *
 * @param kbs The queue that contains all the available keyboards.
 */
extern void *lkby_start_scheduler(void *kbs);



#endif
