#ifndef LKBY_KB_TRANSMITTER_H
#define LKBY_KB_TRANSMITTER_H

#include "lkby.h"

/**
 * This function is responsible for two jobs. The first 
 * job is to retrieve the new key from the coresponded 
 * keyboard event file. The second job is to send the 
 * key that was pressed/released to the client.
 * 
 * @param client_list Clietns to recieve keystrokes.
 */
extern void *lkby_start_transmitter(void *client_list);


#endif