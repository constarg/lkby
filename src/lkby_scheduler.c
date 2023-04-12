#include <semaphore.h> 
#include <pthread.h>

#include "lkby_scheduler.h"
#include "lkby_discovery.h"
#include "lkby_queue.h"
#include "lkby_transmitter.h"

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
static size_t g_s_active_kbs           = 10; // The initialize keyboard number.
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
    g_active_kbs[g_active_next++] = (struct active_kb *) src;
    if (g_active_next == g_s_active_kbs) {
        g_s_active_kbs += 10;
        g_active_kbs = (struct active_kb **) realloc(g_active_kbs, sizeof(struct active_kb *) * g_s_active_kbs);
        if (NULL == g_active_kbs) return -1;
    }

    return 0;
}

/**
 * This function removes a keyboard that is no longer
 * active from the keyboard active list.
 * 
 * @param src The keyboard to remove.
 **/
static inline int remove_active_kb(struct active_kb *src)
{
    int index = -1;
    for (int i = 0; i < g_active_next; i++) {
        if (g_active_kbs[i] == src) {
            index = i;
        }
    }
    if (-1 == index) return -1;

    // move all the elements.
    for (int i = index; i < (g_active_next - 1); i++) {
        g_active_kbs[i] = g_active_kbs[i + 1];
    }
    // free the keyboard info.
    lkby_keyboard_free(&LKBYACTIVE_KB(src));
    // free the previous allocated active_keyboard.
    free(src);
    --g_active_next;

    return 0;
}

/**
 * This function adds a new element into the queue that the 
 * transmitter will see and handle the current keyboard.
 *
 * @param src The information about the current keyboard.
 */
static void store_kb_to_transmit_queue(const union lkby_info *src) 
{
    // TODO - do a copy of the src in a temp lkby_info, in order to prevent free of the memory.
    // store the information needed to schedule the keyboard.
    LKBY_INFO_KEYBOARD_NAME((union lkby_info *) src, lkby_keyboard) = (char *) kb_name;
    LKBY_INFO_KEYBOARD_EVENT((union lkby_info *) src)               = (char *) kb_event; 

    // add the new schedule info into the queue.
    lkbyqueue_enqueue(&LKBYQUEUE(&g_transmit_queue), (union lkby_info *) src);
    // inform the scheduler thread that there is a new keyboard.
    sem_post(&LKBYQUEUE_SEM(&g_transmit_queue));
}

/**
 * This function shedule's a new discovered keyboard. It's 
 * porpuse is to create a new thread that will transmit
 * all it's keystrokes to the client side who is listening.
 *
 * @param src The info of the connected keyboard
 */
static void schedule_kb(const union lkby_info *src)
{
    struct active_kb *new_active_keyboard = NULL;
    // allocate space for the new active keyboard.
    new_active_keyboard = (struct active_kb *) malloc(sizeof(struct active_kb));
    if (NULL == new_active_keyboard) return;
    memset(new_active_keyboard, 0x0, sizeof(struct active_kb));

    // copy the keyboard infos.
    memcpy(&LKBYACTIVE_KB(new_active_keyboard), src, sizeof(union lkby_info));
    // start the thread.
    if (pthread_create(&LKBYACTIVE_KB_THREAD(new_active_keyboard), NULL,
                       &lkby_start_transmitter, NULL) != 0) {
        free(new_active_keyboard);
        return;
    }
    if (-1 == add_active_kb(new_active_keyboard)) {
        free(new_active_keyboard);
        return;
    }
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

void *lkby_start_scheduler(void *none)
{
    union lkby_info kb_info;
    init_active_kbs();

    //while (1) {
    // TODO - remove this fore and replace it with the while.
    for (int i = 0; i < 2; i++) {
        if (0 == sem_wait(&LKBYQUEUE_SEM(&g_sched_queue))) {
            lkbyqueue_dequeue(&kb_info, &LKBYQUEUE(&g_sched_queue));
            clean_threads();
            schedule_kb(&kb_info);
        }
    }

    free(g_active_kbs);
    return NULL; 
}
