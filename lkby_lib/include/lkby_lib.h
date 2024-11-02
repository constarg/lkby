#ifndef LKBY_LIB_H
#define LKBY_LIB_H

#include <stdbool.h>

#include "lkby.h"

/**
 * !The macros below are public and the purpose of 
 * !existance is to make the user who is using this
 * !library more comfortable with the whole experience.
*/

typedef union lkby_info lkby_info;

/**
 * Simplified versions of the server side macros
 * that retrieves the name, code and status of the
 * current info that the server sent.
*/
/**
 * Get the name of the keyboard which occure the 
 * keypress/release event.
*/
#define LKBY_LIB_KEYBOARD_NAME(lkby_info) \
    (LKBY_INFO_KEYBOARD_NAME(lkby_info, lkby_trans_key))

/**
 * Get the code of the key pressed/released.
*/
#define LKBY_LIB_KEYBOARD_CODE(lkby_info) \
    (LKBY_INFO_KEYBOARD_CODE(lkby_info))

/**
 * Get the status of the keyboard, pressed or released.
*/
#define LKBY_LIB_KEYBOARD_STATUS(lkby_info) \
    (LKBY_INFO_KEYBOARD_STATUS(lkby_info))

/**
 * The value of the keyboard status if the key is pressed.
*/
#define LKBY_LIB_KEY_RELEASED   (0x0)
/**
 * The value of the keyboard status if the key is released.
*/
#define LKBY_LIB_KEY_PRESSED    (0x1)
/**
 * The value of the keybarod status if the key is autorepeat.
*/
#define LKBY_LIB_KEY_AUTOREPEAT (0x2)

/**
 * A generic way to express the 3 next macros.
*/
#define LKBY_LIB_CHECK_KEYBOARD_STATUS_FOR(lkby_info, value) \
    ((LKBY_INFO_KEYBOARD_STATUS == value)? true:false)

/**
 * Check the current value of the keyboard status to determine
 * if it was key press.
*/
#define LKBY_LIB_IS_KEY_PRESSED(lkby_info) \
    (LKBY_CHECK_KEYBOARD_STATUS_FOR(lkby_info, LKBY_KEY_PRESSED))

/**
 * Check the current value of the keyboard status to determine
 * if it was key release.
*/
#define LKBY_LIB_IS_KEY_RELEASE(lkby_info) \
    (LKBY_CHECK_KEYBOARD_STATUS_FOR(lkby_info, LKBY_KEY_RELEASED))

/**
 * Check the current value of the keybaord status to determine
 * if it was key autorepeat.
*/
#define LKBY_LIB_IS_KEY_AUTOREPEAT(lkby_info) \
    (LKBY_CHECK_KEYBOARD_STATUS_FOR(lkby_info, LKBY_KEY_AUTOREPEAT))

/**
* Connection related errors.
*/
enum lkby_conn_error_code {
  FAILED_CONNECT  = -1,
  CONNECTION_LOST = -2,
  INTERNAL_FAILURE = -3
};

/**
 * This function establish a new connection to the lkby server. Also,
 * calls the routine, keystroke_callback, for every keystroke the server
 * sends.
 * 
 * @param name A string representing the name of the UNIX socket for the client.
 * @param lkby_lib_callback The routine to call when lkby server sent a new keystroke.
 * @return Nothing on success, cause it put the thread to sleep status (until a new keystroke arrive). 
 * In case an error occured, one of the errors listed in `enum lkby_conn_error_code` will be the
 * respective return value.
*/
extern enum lkby_conn_error_code lkby_lib_establish_connection(const char *restrict name, void (*lkby_lib_callback)(lkby_info *));


#endif
