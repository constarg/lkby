#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

#include <errno.h>

#include "lkby_scheduler.h"
#include "lkby_discovery.h"
#include "lkby.h"

#define KB_EVENTS_LOC "/dev/input/"

#define LKBYACTIVE_KB(act) \
    (act)->kb

#define LKBYACTIVE_KB_THREAD(act) \
    (act)->kb_thread

typedef int lkby_index; // The last index in which the last keyboard was inserted.

struct active_kb 
{
    union lkby_info kb;  // The keyboard
    pthread_t kb_thread; // The associated thread of the keyboard.
};


// This array has the names of all the active keyboards.
// If a keyboard stops from being active, then it's name will be removed
// instantly from this array.
static struct active_kb **g_active_kbs = NULL; // The active keyboards.
static size_t g_s_active_kbs           = 10; // initialize keyboard number.
static lkby_index g_active_next        = 0;

/**
 * This function initializes the list of active keyboards.
 */
static inline int init_active_kbs(void) 
{
    g_active_kbs = (struct active_kb **) malloc(sizeof(struct active_kb *) * g_s_active_kbs);
    if (NULL == g_active_kbs) {
        return -1;
    }
    memset(g_active_kbs, 0x0, sizeof(struct active_kb *) * g_s_active_kbs);
    return 0;
}

/**
 * This function adds a new keyboard to the list
 * of active keyboards.
 *
 * @param src The keyboard to be added.
 **/
static inline int add_active_kb(const struct active_kb *src)
{
    g_active_kbs[g_active_next] = (struct active_kb *) src;
    if (g_active_next == g_s_active_kbs) {
        g_s_active_kbs += 10;
        g_active_kbs = (struct active_kb **) realloc(g_active_kbs, sizeof(struct active_kb *) * g_s_active_kbs);
        if (NULL == g_active_kbs) return -1;
    }

    g_active_next++;
    return 0;
}

/**
 * This function removes a keyboard that is no longer
 * active from the keyboard active list.
 * 
 * @param src The keyboard to remove.
 **/
static int remove_active_kb(struct active_kb *src)
{
    int index = -1;
    for (int i = 0; i < g_active_next; i++) {
        // TODO - use the name of the keyboard to find out, not the meomry.
        if (!strcmp(
            LKBY_INFO_KEYBOARD_NAME(&LKBYACTIVE_KB(g_active_kbs[i]), lkby_keyboard),
            LKBY_INFO_KEYBOARD_NAME(&LKBYACTIVE_KB(src), lkby_keyboard)
        )) { // TODO - validate.
            index = i;
            printf("FIND IT\n");
        }
    }
    if (-1 == index) {
        printf("ERROR\n");
        return -1;
    }
    g_active_kbs[index] = 0x0;

    // move all the elements.
    for (int i = index; i < (g_active_next - 1); i++) {
        g_active_kbs[i] = g_active_kbs[i + 1];
    }
    // free the keyboard info.
    lkby_keyboard_free(&LKBYACTIVE_KB(src));
    // free the previous allocated active_keyboard.
    free(src);
    --g_active_next;

    if (g_s_active_kbs - g_active_next > 10) {
        /**
         * If there are no that much keyboards, why take allocate 10 extra spaces in
         * memory? just free them.
        */
        g_s_active_kbs -= 10;
        g_active_kbs = (struct active_kb **) realloc(g_active_kbs, sizeof(struct active_kb *) * g_s_active_kbs);
        if (NULL == g_active_kbs) return -1;
    }

    return 0;
}

/**
 * This function checks wether a thread is still
 * alive and if it is not, it removes it from the
 * list of active threads. In case a thread is terminating
 * and the same keyboard connected again, then it will again
 * inserted in the active keyboard list, but as a new entry.
 */
static inline void clean_threads(void)
{
    for (int i = 0; i < g_active_next; i++) {
        if (0 == pthread_tryjoin_np(LKBYACTIVE_KB_THREAD(g_active_kbs[i]), NULL)) {
            remove_active_kb(g_active_kbs[i]);
            --i; // because we remove a thread, go back one to not skip any thread.
        }
    }
}

/**
 * This functions checks wether a specific keyboard is active
 * at the time or not. If no keyboard with such an identity is
 * currently active, then the scheduler should put it in the 
 * active keyboard list, task that is not this function 
 * responsible for.
 * 
 * @param src The information about the keyboard to check.
*/
static inline bool is_keyboard_active(union lkby_info *src) 
{
    for (int i = 0; i < g_s_active_kbs; i++) {
        // Check if the given keyboard much any currently active keyboard.
        if (!strcmp(
            LKBY_INFO_KEYBOARD_EVENT(src),
            LKBY_INFO_KEYBOARD_EVENT(&LKBYACTIVE_KB(g_active_kbs[i]))
        )) {
            return true;
        }
    }

    return false;
}

// static void keyboard_routine_cleanup_handler(void *arg)
// {

// }

/**
 * This routine is responsible to track any keystroke from
 * a specific keyboard, given as parameter.
 * 
 * @param src The keyboard to monitor.
 * @returns Nothing.
*/
static void *keyboard_routine(void *src)
{
    union lkby_info keyboard;
    memcpy(&keyboard, ((union lkby_info *) src), sizeof(union lkby_info));

    union lkby_info transmit_info;
    struct input_event kb_event_buffer;
    char *absolute_path_to_event = NULL; 
    int event_fd = 0;

    // Build the absolute path that points to the eventX file of the coresponded keybaord.
    absolute_path_to_event = (char *) malloc(sizeof(char) * (
                                                strlen(LKBY_INFO_KEYBOARD_EVENT(&keyboard)) +
                                                strlen(KB_EVENTS_LOC)) + 1);

    (void)sprintf(absolute_path_to_event, "%s%s", KB_EVENTS_LOC, LKBY_INFO_KEYBOARD_EVENT(&keyboard));

    // Open the eventX file of the keyboard.
    event_fd = open(absolute_path_to_event, O_RDONLY);

    if (-1 == event_fd) {
        printf("%s\n", strerror(errno));
        goto failed_label;
    }
    // Start monitor the keyboard.
//    while (true) {
        if (read(event_fd, &kb_event_buffer, sizeof(kb_event_buffer)) == -1) goto failed_label;
        // If the occured event is a keypress/release
        if (kb_event_buffer.type == EV_KEY) {
            // Build info for the transmitter.
            lkby_init(&transmit_info);
            // Fill the members.
            LKBY_INFO_KEYBOARD_NAME(&transmit_info, lkby_trans_key) = LKBY_INFO_KEYBOARD_NAME(&keyboard, lkby_keyboard);
            LKBY_INFO_KEYBOARD_CODE(&transmit_info)   = kb_event_buffer.code;
            LKBY_INFO_KEYBOARD_STATUS(&transmit_info) = kb_event_buffer.value;

            lkbyqueue_enqueue(&LKBYQUEUE(&g_transmit_queue), &transmit_info);
            // inform the transmitter thread that there is a new event.
            (void)sem_post(&LKBYQUEUE_SEM(&g_transmit_queue));
//        }
    }


failed_label:
    free(absolute_path_to_event);
    return NULL;
}

static void scheduler_cleanup_handler(void *none)
{
    for (int i = 0; i < g_s_active_kbs; i++) {
        remove_active_kb(g_active_kbs[i]);
    }
    free(g_active_kbs);
}

void *lkby_start_scheduler(void *none)
{
    init_active_kbs();
    pthread_cleanup_push(scheduler_cleanup_handler, NULL);

    union lkby_info kb; // The current keyboard
    struct active_kb *ac_kb; // Label the current keyboard as active. 
    union lkby_info *tmp;

    for (int i = 0; i < 2; i++) {
    if (0 != sem_wait(&LKBYQUEUE_SEM(&g_keyboard_queue))) return NULL;
        tmp = (union lkby_info *) malloc(sizeof(union lkby_info));
        lkby_init(tmp);
        lkbyqueue_dequeue(tmp, &LKBYQUEUE(&g_keyboard_queue));

        // // Create a new active keyboard.
        ac_kb = (struct active_kb *) malloc(sizeof(struct active_kb));
        if (NULL == ac_kb) continue;

        (void)memset(ac_kb, 0x0, sizeof(struct active_kb));
        (void)memcpy(&ac_kb->kb, &kb, sizeof(union lkby_info));
        add_active_kb((const struct active_kb *) ac_kb);

        // // Create the coresponded thread.
        if (0 != pthread_create(&ac_kb->kb_thread, NULL, keyboard_routine, (void *) tmp)) continue;
    }
    for (int i = 0; i < g_active_next - 1; i++) {
        pthread_join(g_active_kbs[i]->kb_thread, NULL);
    }

    //while (1) {

    //}
    
    pthread_cleanup_pop(1);
    return NULL;
}
