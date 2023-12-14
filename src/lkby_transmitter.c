#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <semaphore.h>

#include "lkby_transmitter.h"
#include "lkby_scheduler.h"
#include "lkby_queue.h"
#include "lkby.h"

static lkby_user_id active_users[MAX_CONNECTIONS]; // The currently active users.

static inline void active_users_init()
{
    (void)memset(active_users, 0x0, sizeof(lkby_user_id) * MAX_CONNECTIONS);
}

/**
 * This function adds a new user to the list 
 * of active users.
*/
static inline void add_user(lkby_user_id src) 
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users[i] == 0x0) {
            active_users[i] = src;
            return;
        }
    }
}

/**
 * This function removes a user from the list
 * of active users.
*/
static inline void remove_user(lkby_user_id src) 
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users[i] == src) {
            active_users[i] = 0x0;
        }
    }
}

/**
 * This function checks whether a user is active by sending a test
 * message, 1 bytes, and verify that the connection is still established.
*/
static inline bool is_user_active(lkby_user_id src) 
{
    unsigned char tmp_buff = 0x0;
    if (-1 == send(src, &tmp_buff, sizeof(tmp_buff), MSG_NOSIGNAL)) {
        remove_user(src);
        return false;
    }
    return true;
}

/**
 * This functino removes any inactive user.
*/
static inline void remove_inactive_users()
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (!is_user_active(active_users[i])) {
            remove_user(active_users[i]);
        }
    }
}

/**
 * This function sends the data to the active users.
*/
static inline void send_data_to_users(union lkby_info *src) 
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (0 == active_users[i]) continue;
        if (-1 == send(active_users[i], &src, sizeof(union lkby_info), MSG_NOSIGNAL)) continue;
    }
}

void *lkby_start_transmitter(void *none)
{
    union lkby_info trasmit_data;
    union lkby_info current_user;

    while (true) {
        if (0 != sem_wait(&LKBYQUEUE_SEM(&g_transmit_queue))) continue;
        (void)sem_trywait(&LKBYQUEUE_SEM(&g_transmit_queue));
        remove_inactive_users();
        while (true != lkbyqueue_isempty(&LKBYQUEUE(&g_user_queue))) {
            lkbyqueue_dequeue(&current_user, &LKBYQUEUE(&g_user_queue));
            if (!is_user_active(current_user.user_id)) continue;
            add_user(current_user.user_id);
        }

        while (true != lkbyqueue_isempty(&LKBYQUEUE(&g_transmit_queue))) {
            lkbyqueue_dequeue(&trasmit_data, &LKBYQUEUE(&g_transmit_queue));
            send_data_to_users(&trasmit_data);
        }

    }
}