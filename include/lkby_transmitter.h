#ifndef LKBY_KB_TRANSMITTER_H
#define LKBY_KB_TRANSMITTER_H

#include <stdint.h>

typedef int8_t lkby_status;

/**
 * This structure consists of the
 * informations to transmit.
 */
struct lkby_trans_key
{
    char       *kb_name;   // The name of the keyboard that occure an event.
    lkby_status kb_status; // The status of the event, if the key is pressed/relased
};


/**
 * This function is responsible to transmit the
 * keyboard events, like key pressed and key release.
 * 
 * @param keys Is the queue that contains the next keystroke
 * to be transmitted to the clinets through the socket.
 */
extern void *lkby_start_transmitter(void *keys);


#endif
