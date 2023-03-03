#ifndef LKBY_LKBY_H
#define LKBY_LKBY_H

#include <stdint.h>

typedef int8_t lkby_status;

union lkby_info
{
    /**
     * This structure consists of the
     * informations of the current discovered
     * keyboard
     */
    struct 
    {
       char *kb_name;  // The name of the keyboard that occure an event.
       char *kb_event; // The associated event.
    } lkby_keyboard;

    /**
     * This structure consists of the
     * informations to transmit.
     */
    struct
    {
       char       *kb_name;    // The name of the keyboard that occure an event.
       lkby_status kb_status;  // The status of the event, if the key is pressed/relased
    } lkby_trans_key;
};


#endif
