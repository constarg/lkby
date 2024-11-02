#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <malloc.h>
#include <string.h>
#include <poll.h>

#include "lkby_lib.h"
#include "lkby.h"

#define SERVER_SOCK_PATH "/tmp/unix_lkby_sock.server"
#define SOCK_PATH "/tmp/"
#define CLIENT_SOCK_ENDING ".client"

static int g_client_fd;

static void lkby_lib_signal_handler(int sig __attribute__((unused)))
{
    (void) close(g_client_fd);
    exit(0);
}

enum lkby_conn_error_code lkby_lib_establish_connection
(const char *restrict name, void (*lkby_lib_callback)(lkby_info *))
{
    signal(SIGHUP, lkby_lib_signal_handler);
    signal(SIGINT, lkby_lib_signal_handler);
    signal(SIGQUIT, lkby_lib_signal_handler);

    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;

    char *client_sock_path;
    socklen_t len;

    // poll related.
    nfds_t nfds = 1;
    struct pollfd fds[1];

    enum lkby_conn_error_code ret_code;

    (void) memset(&server_addr, 0, sizeof(struct sockaddr_un));
    (void) memset(&client_addr, 0, sizeof(struct sockaddr_un));

    g_client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (-1 == g_client_fd) return INTERNAL_FAILURE;

    fds[0].fd     = g_client_fd;
    fds[0].events = POLLRDHUP;

    // Create the UNIX socket file, based on the given name.
    client_sock_path = (char *) malloc(sizeof(char) * strlen(name) +
                                       strlen(SOCK_PATH) +
                                       strlen(CLIENT_SOCK_ENDING) + 1);
    if (NULL == client_sock_path) {
        ret_code = INTERNAL_FAILURE;
        goto lkby_lib_establish_error_label;
    }

    strcpy(client_sock_path, SOCK_PATH);
    strcat(client_sock_path, name);
    strcat(client_sock_path, CLIENT_SOCK_ENDING);

    client_addr.sun_family = AF_UNIX;
    (void) strcpy(client_addr.sun_path, client_sock_path);
    (void) unlink(client_sock_path);
    len = sizeof(client_addr);

    if (-1 == bind(g_client_fd, (struct sockaddr *) &client_addr, len)) {
        ret_code = INTERNAL_FAILURE;
        goto lkby_lib_establish_error_label;
    }

    server_addr.sun_family = AF_UNIX;
    (void) strcpy(server_addr.sun_path, SERVER_SOCK_PATH);

    if (-1 == connect(g_client_fd, (struct sockaddr *) &server_addr, len)) {
        ret_code = FAILED_CONNECT;
        goto lkby_lib_establish_error_label;
    }
    union lkby_info buff; // The buffer of the connection.
    lkby_init(&buff);

    free(client_sock_path);
    client_sock_path = NULL;
    // From now on, the client is connected to the server. Now wait for any keystroke.
    while (true) {
        lkby_init(&buff);

        if (-1 == poll(fds, nfds, 0)) {
            ret_code = INTERNAL_FAILURE;
            goto lkby_lib_establish_error_label;
        }

        if (fds[0].revents & POLLRDHUP) {
            ret_code = CONNECTION_LOST;
            goto lkby_lib_establish_error_label;
        }

        if (-1 == recv(g_client_fd, &buff, sizeof(union lkby_info), 0)) goto lkby_lib_establish_error_label;
        // Call the callback function for the current keystroke.
        lkby_lib_callback(&buff);
    }

lkby_lib_establish_error_label:
    (void) close(g_client_fd);
    return ret_code;
}
