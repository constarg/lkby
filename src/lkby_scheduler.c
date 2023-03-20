#include <semaphore.h> 
#include <pthread.h>

#include "lkby_scheduler.h"
#include "lkby_discovery.h"
#include "lkby_queue.h"

#define LKBYACTIVE_KB_NAME(act) \
    (act)->kb_name

#define LKBYACTIVE_KB_THREAD(act) \
    (act)->kb_thread

typedef int lkby_index; // The last index in which the last keyboard was inserted.

struct active_kb 
{
    char *kb_name;     // The name of the keyboard.
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
static int init_active_kbs(void) 
{

}

/**
 * This function adds a new keyboard to the list
 * of active keyboards.
 *
 * @param src The keyboard to be added.
 **/
static int add_active_kb(const struct active_kb *src)
{

}

/**
 * This function removes a keyboard that is no longer
 * active from the keyboard active list.
 * 
 * @param src The keyboard to remove.
 **/
static int remove_active_kb(const struct active_kb *src)
{

}

/**
 * This function shedule's a new discovered keyboard. It's 
 * porpuse is to create a new thread that will transmit
 * all it's keystrokes to the client side who is listening.
 */
static void schedule_kb(const union lkby_info *src)
{

}

void *lkby_start_scheduler(void *sched_queue)
{
    union lkby_info kb_info;
    active_kbs_init();

    //while (1) {
        if (-1 == sem_wait(&LKBYQUEUE_SEM(&g_sched_queue))) {
            lkbyqueue_dequeue(&kb_info, &LKBYQUEUE(&g_sched_queue));
            schedule_kb(&kb_info);
        }
    //}

    return NULL; 
}
