#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <semaphore.h>
#include <unistd.h>

#include "lkby_transmitter.h"
#include "lkby_scheduler.h"
#include "lkby_queue.h"
#include "lkby.h"

static lkby_user_id active_users[MAX_CONNECTIONS]; // The currently active users.

static inline void active_users_init(void)
{
    (void)memset(active_users, -1, sizeof(lkby_user_id) * MAX_CONNECTIONS);
}

/**
 * This function adds a new user to the list 
 * of active users.
*/
static inline void add_user(lkby_user_id src) 
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users[i] == -1) {
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
    (void)close(src);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (active_users[i] == src) {
            active_users[i] = -1;
            return;
        }
    }
}

/**
 * This function checks whether a user is active by sending a test
 * message, 1 bytes, and verify that the connection is still established.
*/
static inline bool is_user_active(lkby_user_id src) 
{
    union lkby_info tmp_buff;
    lkby_init(&tmp_buff);
    if (-1 == send(src, &tmp_buff, sizeof(union lkby_info), MSG_NOSIGNAL)) {
        remove_user(src);
        return false;
    }
    return true;
}

/**
 * This functino removes any inactive user.
*/
static inline void remove_inactive_users(void)
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
static inline void send_data_to_users(const union lkby_info *src) 
{
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (-1 == active_users[i]) continue;
        if (-1 == send(active_users[i], (const void *) src, sizeof(union lkby_info), MSG_NOSIGNAL)) continue;
    }
}

void *lkby_start_transmitter(void *none  __attribute__((unused)))
{
    union lkby_info trasmit_data;
    union lkby_info current_user;

    while (true) {
        if (0 != sem_wait(&LKBYQUEUE_SEM(&g_transmit_queue))) continue;
        remove_inactive_users();

        // Are there new users?
        if (true != lkbyqueue_isempty(&LKBYQUEUE(&g_user_queue))) {
            // OK, wait for the server to fill proceed with the queue.
            if (0 != sem_wait(&LKBYQUEUE_SEM(&g_user_queue))) continue;
            // Then, consume all the users that has been added in the queue.
            while (true != lkbyqueue_isempty(&LKBYQUEUE(&g_user_queue))) {
                lkbyqueue_dequeue(&current_user, &LKBYQUEUE(&g_user_queue));
                if (!is_user_active(LKBY_INFO_KEYBOARD_USER_ID(&current_user))) continue;
                add_user(LKBY_INFO_KEYBOARD_USER_ID(&current_user));
            }
            // After that, notify the server, that the clients has been consumed.
            (void)sem_post(&LKBYQUEUE_SEM(&g_user_queue));
        }

        while (true != lkbyqueue_isempty(&LKBYQUEUE(&g_transmit_queue))) {
            lkbyqueue_dequeue(&trasmit_data, &LKBYQUEUE(&g_transmit_queue));
            send_data_to_users(&trasmit_data);
        }
    }
}