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

#include "lkby_scheduler.h"
#include "lkby_discovery.h"
#include "lkby.h"

#define KB_EVENTS_LOC "/dev/input/"

#define LKBYACTIVE_KB(act) \
    (act)->kb

#define LKBYACTIVE_KB_THREAD(act) \
    (act)->kb_thread

typedef size_t lkby_index; // The last index in which the last keyboard was inserted.

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


static inline void active_kb_init(struct active_kb *src)
{
    (void)memset(src, 0x0, sizeof(struct active_kb));
}

/**
 * This function initializes the list of active keyboards.
 * @return 0 on success, otherwise -1.
 */
static inline int init_active_kbs(void) 
{
    g_active_kbs = (struct active_kb **) malloc(sizeof(struct active_kb *) * g_s_active_kbs);
    if (NULL == g_active_kbs) {
        return -1;
    }
    (void)memset(g_active_kbs, 0x0, sizeof(struct active_kb *) * g_s_active_kbs);
    return 0;
}

/**
 * This function adds a new keyboard to the list
 * of active keyboards.
 *
 * @param src The keyboard to be added.
 * @return 0 on success, otherwise -1.
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
 * @return 0 on success, otherwise -1.
 **/
static int remove_active_kb(struct active_kb *restrict src)
{
    int index = -1;
    for (size_t i = 0; i < g_active_next; i++) {
        if (!strcmp(
            LKBY_INFO_KEYBOARD_NAME(&LKBYACTIVE_KB(g_active_kbs[i]), lkby_keyboard),
            LKBY_INFO_KEYBOARD_NAME(&LKBYACTIVE_KB(src), lkby_keyboard)
        )) { 
            index = i;
        }
    }
    if (-1 == index) return -1;
    g_active_kbs[index] = 0x0;

    // move all the elements.
    for (size_t i = index; i < g_active_next; i++) {
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
    for (size_t i = 0; i < g_active_next; i++) {
        if (0 == pthread_tryjoin_np(LKBYACTIVE_KB_THREAD(g_active_kbs[i]), NULL)) {
            if (-1 == remove_active_kb(g_active_kbs[i])) continue;
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
 * @return true if the keyboard is active, false otherwise.
*/
static inline bool is_keyboard_active(const union lkby_info *src) 
{
    for (size_t i = 0; i < g_active_next; i++) {
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

/**
 * This routine is used to cleanup the used memory
 * that may not be freed properly during the execution
 * of keyboard_routine.
 * 
 * @param src The keyboard which had allocated memory.
*/
static void keyboard_routine_cleanup_handler(void *src)
{
    union lkby_info *keyboard = (union lkby_info *) src;
    free(keyboard);
}

/**
 * This routine is responsible to track any keystroke from
 * a specific keyboard, given as parameter.
 * 
 * @param src The keyboard to monitor.
 * @returns Nothing.
*/
static void *keyboard_routine(void *src)
{
    pthread_cleanup_push(keyboard_routine_cleanup_handler, src);

    union lkby_info keyboard;
    (void)memcpy(&keyboard, src, sizeof(union lkby_info));

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
    if (-1 == event_fd) goto keyboard_routine_failed_label;

    // Start monitor the keyboard.
    while (true) {
        if (read(event_fd, &kb_event_buffer, sizeof(kb_event_buffer)) == -1) goto keyboard_routine_failed_label;
        // If the occured event is a keypress/release
        if (kb_event_buffer.type == EV_KEY) {
            // Build info for the transmitter.
            lkby_init(&transmit_info);
            // Fill the members.
            LKBY_INFO_KEYBOARD_NAME(&transmit_info, lkby_trans_key) = LKBY_INFO_KEYBOARD_NAME(&keyboard, lkby_keyboard);
            LKBY_INFO_KEYBOARD_CODE(&transmit_info)                 = kb_event_buffer.code;
            LKBY_INFO_KEYBOARD_STATUS(&transmit_info)               = kb_event_buffer.value;

            if (true != lkbyqueue_isempty(&LKBYQUEUE(&g_transmit_queue))) {
                if (0 != sem_wait(&LKBYQUEUE_SEM(&g_transmit_queue))) goto keyboard_routine_failed_label;
            }

            lkbyqueue_enqueue(&LKBYQUEUE(&g_transmit_queue), &transmit_info);
            // inform the transmitter thread that there is a new event.
            (void)sem_post(&LKBYQUEUE_SEM(&g_transmit_queue));
        }
    }

keyboard_routine_failed_label:
    free(absolute_path_to_event);
    (void)close(event_fd);
    pthread_cleanup_pop(1);
    return NULL;
}

/**
 * This function clear up the data that the lkby_start_scheduler
 * thread may left behind.
*/
static void scheduler_cleanup_handler(void *none __attribute__((unused)))
{
    for (size_t i = 0; i < g_active_next; i++) {
        if (-1 == remove_active_kb(g_active_kbs[i])) continue;
        --i;
    }
    free(g_active_kbs);
}

void *lkby_start_scheduler(void *none __attribute__((unused)))
{
    init_active_kbs();
    pthread_cleanup_push(scheduler_cleanup_handler, NULL);

    union lkby_info *kb     = NULL; // The current keyboard
    struct active_kb *ac_kb = NULL; // Label the current keyboard as active. 

    while (true) {
        if (0 != sem_wait(&LKBYQUEUE_SEM(&g_keyboard_queue))) continue;
        clean_threads();

        while (true != lkbyqueue_isempty(&LKBYQUEUE(&g_keyboard_queue))) {
            kb    = NULL;
            ac_kb = NULL;
            kb = (union lkby_info *) malloc(sizeof(union lkby_info));
            if (NULL == kb) continue;
            
            lkby_init(kb);
            lkbyqueue_dequeue(kb, &LKBYQUEUE(&g_keyboard_queue));
            
            if (is_keyboard_active(kb)) goto lkby_scheduler_failed_label;

            // Create a new active keyboard.
            ac_kb = (struct active_kb *) malloc(sizeof(struct active_kb));
            if (NULL == ac_kb) goto lkby_scheduler_failed_label;
            
            active_kb_init(ac_kb);
            (void)memcpy(&ac_kb->kb, kb, sizeof(union lkby_info));

            if (0 != add_active_kb((const struct active_kb *) ac_kb)) goto lkby_scheduler_failed_label;

            // Create the coresponded thread.
            if (0 != pthread_create(&ac_kb->kb_thread, NULL, keyboard_routine, (void *) kb)) goto lkby_scheduler_failed_label;
            (void)pthread_detach(ac_kb->kb_thread);

            continue; // ! DO NOT GO TO THE ARREA OF ERRORS. (label below).
            // TODO - this error handling need more tests.
lkby_scheduler_failed_label:
            free(kb);
            free(ac_kb);
        }
    }    

    pthread_cleanup_pop(1);
    return NULL;
}
