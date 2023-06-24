#ifndef LKBY_LKBY_H
#define LKBY_LKBY_H

#include <stdint.h>
#include <memory.h>
#include <malloc.h>

/**
 * This macro retrieves the name of the keyboard.
 */
#define LKBY_INFO_KEYBOARD_NAME(lkby_info, field) \
    (lkby_info)->field.kb_name

/**
 * This macro retrieves the eventX name of the
 * keyboard.
 */
#define LKBY_INFO_KEYBOARD_EVENT(lkby_info) \
    (lkby_info)->lkby_keyboard.kb_event

/**
 * This macro retrieves the status of a key
 */
#define LKBY_INFO_KEYBOARD_STATUS(lkby_info) \
    (lkby_info)->lkby_trans_key.kb_status

// TODO - make the status using the bits?

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
        char *kb_event; // The associated event file.
    } lkby_keyboard;

    /**
     * This structure consists of the
     * informations to transmit.
     */
    struct
    {
        char       *kb_name;    // The name of the keyboard that occure an event.
        char       *kb_code;    // The code of the pressed/released key.
        lkby_status kb_status;  // The status of the event, if the key is pressed/relased
    } lkby_trans_key;
};


static inline void lkby_init(union lkby_info *src)
{
    (void)memset(src, 0x0, sizeof(union lkby_info));
}

static inline void lkby_trans_key_free(union lkby_info *src)
{
    free(LKBY_INFO_KEYBOARD_NAME(src, lkby_trans_key));
}

static inline void lkby_keyboard_free(union lkby_info *src)
{
    free(LKBY_INFO_KEYBOARD_NAME(src, lkby_keyboard));
    free(LKBY_INFO_KEYBOARD_EVENT(src));
}

#endif
