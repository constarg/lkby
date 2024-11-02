#include <stdio.h>

#include "lkby_lib.h"


void keystroke_handler(lkby_info *restrict src) 
{
    if (0 == LKBY_INFO_KEYBOARD_CODE(src)) return; // When is zero, it means that it is a check (from lkby server), so ignore this.
                                                   
    (void)printf("CODE: %d, STATUS = %d  (STATUS = 0 - pressed, 1 - released, 2 - autorepeat)\n", 
          LKBY_INFO_KEYBOARD_CODE(src), LKBY_INFO_KEYBOARD_STATUS(src));
}

int main(void)
{
    (void)printf("Trying to connect to lkby server...\n");
    int err = lkby_lib_establish_connection("TESTING", keystroke_handler);

    if (-1 == err) {
        (void)printf("Failed to connect to lkby server.\n");
    }
    return 0;
}
