#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "lkby_scheduler.h"
#include "lkby_discovery.h"
#include "lkby_queue.h"
#include "lkby.h"

#define SERVER_SOCK_PATH "/tmp/unix_lkby_sock.server"

#define RETRY 5 // Seconds.

// declare global queues.
struct lkbyqueue_sync g_keyboard_queue; // The queue used to schedule data.
struct lkbyqueue_sync g_transmit_queue; // The queue used to transmit data.
struct lkbyqueue_sync g_user_queue;     // The queue used to inform tansmitter for new users.

int main(int argc, char *argv[])
{
    if (lkbyqueue_sync_init(&g_keyboard_queue) != 0) return -1;
    if (lkbyqueue_sync_init(&g_transmit_queue) != 0) return -1;

    /**
     * Service related variables.
    */
    pthread_t transmit_th = 0;         // The transmitter thread.
    pthread_t sched_th    = 0;         // The scheduler thread.
    pthread_t discov_th   = 0;         // The discovery thread.
    /**
     * Communication related variables. 
    */
    int server_fd = 0;                 // Server socket file descriptor.
    int client_fd = 0;                 // Client socket file descriptor.
    int conn_errors = 0;               // The occured errors.
    socklen_t len = 0;                 // The length of the server addr.
    struct sockaddr_un server_addr;    // The server address (unix file).
    struct sockaddr_un client_addr;    // The server address (unix file).

    // Check if there is an already active thread that discover keyboards.
    if (0 != pthread_create(&discov_th, NULL, &lkby_start_discovery, NULL)) return -1;
    if (0 != pthread_create(&sched_th, NULL, &lkby_start_scheduler, NULL)) return -1;

    // Detach each thread from the main thread.
    // pthread_detach(discov_th);
    // pthread_detach(sched_th);

    pthread_join(discov_th, NULL);
    pthread_join(sched_th, NULL);

    /*
    // If any step excpet the listening/accept part failed, retry after 5 seconds again.
    while (1) {
        // initialize the sockaddr
        (void)memset(&server_addr, 0x0, sizeof(struct sockaddr_un));
        (void)memset(&client_addr, 0x0, sizeof(struct sockaddr_un));
        // Get the file descriptor for the server.
        server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (-1 == server_fd) {
            sleep(RETRY);
            continue;
        }

        // Configure server socket addr (unix file).
        server_addr.sun_family = AF_UNIX;
        (void)strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
        len = sizeof(server_addr);

        // Remove the sock file, if previously existed.
        unlink(SERVER_SOCK_PATH);
        // Bind the sock addr with the specific file descriptor of the socket.
        if (-1 == bind(server_fd, (struct sockaddr *) &server_addr, len)) {
            close(server_fd);
            sleep(RETRY);
            continue;
        }

        // Repeat the code below for ever.
        while (1) {
            // Listen for a new connection.
            // If 3 consecutives errors have been occured, then reactivate the whole server.
            if (3 == conn_errors) break; // If more than one error has occured, then reset the whole server.
            if (-1 == listen(server_fd, MAX_CONNECTIONS)) {
                ++conn_errors; // increase occured errors.
                continue;
            }
            client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len);
            len = sizeof(client_addr);
            if (-1 == client_fd) {
                ++conn_errors;
                continue;
            }
            if (-1 == getpeername(client_fd, (struct sockaddr *) &client_addr, &len)) {
                close(client_fd);
                ++conn_errors;
                continue;
            }
            conn_errors = 0;

            // Check if there is an already active thread that discover keyboards.
            if (0 != pthread_tryjoin_np(discov_th, NULL)) {
                if (0 != pthread_create(&discov_th, NULL, &lkby_start_discovery, NULL)) continue;
            }

            // Cancel the thread in order to update the clients.
            // TODO - change this. Don't use cancel to update the client list, instead use a different queue, to update dynamicaly the clients.
            // Create again the same thread with updated clients.
            //if (0 != pthread_create(&transmit_th, NULL, &lkby_start_transmitter, (void *) g_active_clients)) continue;

            // TODO - DONE - After the connection established, find the available kayboards and send the information to the client.
            // TODO - DONE - don't make more than the necessary threads. If more than one client has connected then 
            // TODO - DONE - pass on the trasmitter a list with all the active client sockets. If the transmitter is already active
            // then kill him and after than recreate the thread with the updated list of clients.
            // TODO - DONE - discover keyboards.
            // TODO - DONE - schedule the new keyboards.
            // TODO - DONE - transmit the keystrokes to the other process.
        }
        close(server_fd);
        conn_errors = 0; // Reset occured errors.
    }*/

    // TODO - [SECURITY]
    /**
     * For the security part of the program thw user required to give a passphrashe that is stored in
     * a specific file path and only root can access it. 
    */

    return 0;
}
