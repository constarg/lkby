#ifndef LKBY_KB_TRANSMITTER_H
#define LKBY_KB_TRANSMITTER_H


// This queue is used to inform the transmitter if any new user comes in.
extern struct lkbyqueue_sync g_user_queue; // Declared in main.


extern void *lkby_start_transmitter(void *none);

#endif