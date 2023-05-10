#include <malloc.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "lkby_scheduler.h"
#include "lkby_transmitter.h"
#include "lkby_discovery.h"
#include "lkby_queue.h"

#define SERVER_SOCK_PATH "/tmp/unix_lkby_sock.server"
#define MAX_CONNECTIONS 10

#define RETRY 5 // Seconds.

// declare global queues.
struct lkbyqueue_sync g_sched_queue;    // The schedule queue.
struct lkbyqueue_sync g_transmit_queue; // The queue used to transmit data.

int main(int argc, char *argv[])
{
    /*// initialize queues.
    if (lkbyqueue_sync_init(&g_sched_queue) != 0) return -1;
    if (lkbyqueue_sync_init(&g_transmit_queue) != 0) return -1;

    lkby_start_discovery(NULL);
    lkby_start_scheduler(NULL);*/

    /**
     * Service related variables.
    */
    pthread_t sched_th    = 0;    // The schedule thread.
    pthread_t transmit_th = 0;    // The transmitter thread.
    pthread_t discov_th   = 0;    // The discovery thread.
    /**
     * Communication related variables. 
    */
    int server_fd = 0;                  // Server socket file descriptor.
    int client_fd = 0;                  // CLient socket file descriptor.
    socklen_t len = 0;                  // The length of the server addr.
    struct sockaddr_un server_addr;     // The server address (unix file).
    struct sockaddr_un client_addr;     // The server address (unix file).

    // If any step excpet the listening/accept part failed, retry after 5 seconds again.
    while (1) {
        // initialize the sockaddr
        memset(&server_addr, 0x0, sizeof(struct sockaddr_un));
        memset(&client_addr, 0x0, sizeof(struct sockaddr_un));
        // Get the file descriptor for the server.
        server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (-1 == server_fd) {
            sleep(RETRY);
            continue;
        }

        // Configure server socket addr (unix file).
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
        len = sizeof(server_addr);

        // Remove the sock file, if previously existed.
        unlink(SERVER_SOCK_PATH);
        // Bind the sock addr with the specific file descriptor of the socket.
        if (-1 == bind(server_fd, (struct sockaddr *) &server_addr, len)) {
            sleep(RETRY);
            continue;
        }
        // Repeat the code below for ever.
        while (1) {
            // Listen for a new connection.
            //if (-1 == listen(server_fd, MAX_CONNECTIONS)) return -1;
            client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len);
            len = sizeof(client_addr);
            if (-1 == client_fd) continue;
            if (-1 == getpeername(client_fd, (struct sockaddr *) &client_addr, &len)) {
                close(client_fd);
                continue;
            }

            // After the connection established, find the available kayboards and send the information to the client.
            // TODO - discover keyboards.
            // TODO - schedule the new keyboards.
            // TODO - transmit the keystrokes to the other process.
        }
    }

    return 0;
}
