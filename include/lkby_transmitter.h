#ifndef LKBY_KB_TRANSMITTER_H
#define LKBY_KB_TRANSMITTER_H

#include "lkby.h"

/**
 * This function is responsible to transmit the
 * keyboard events, like key pressed and key release.
 * 
 * @param key_queue Is the queue that contains the next keystroke
 * to be transmitted to the clinets through the socket.
 */
extern void *lkby_start_transmitter(void *key_queue);


#endif
