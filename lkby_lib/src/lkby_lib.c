#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>

#include "lkby_lib.h"
#include "lkby.h"

#include <string.h>
#include <errno.h>

#define SERVER_SOCK_PATH "/tmp/unix_lkby_sock.server"
#define CLIENT_SOCK_PATH "/tmp/unix_lkby_sock.client"


int establish_connection(void (*keystroke_callback)(const lkby_info *restrict src))
{

    struct sockaddr_un server_addr; 
    struct sockaddr_un client_addr;

    int client_fd; // Client's socket file descriptor.
    socklen_t len;

    (void)memset(&server_addr, 0, sizeof(struct sockaddr_un));
    (void)memset(&client_addr, 0, sizeof(struct sockaddr_un));

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("test3\n");
    if (-1 == client_fd) return -1;

    client_addr.sun_family = AF_UNIX;
    (void)strcpy(client_addr.sun_path, CLIENT_SOCK_PATH);
    (void)unlink(CLIENT_SOCK_PATH);
    len = sizeof(client_addr);
    printf("test2\n");

    if (-1 == bind(client_fd, (struct sockaddr *) &client_addr, len)) {
        printf("%s\n", strerror(errno));
        return -1;
    }

    server_addr.sun_family = AF_UNIX;
    (void)strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
    printf("test1\n");
    if (-1 == connect(client_fd, (struct sockaddr *) &server_addr, len)) {
        printf("%s\n", strerror(errno));
        return -1;
    }

    printf("Connection established!\n");
    union lkby_info buff; // The buffer of the connection.
    lkby_init(&buff);

    // From now on, the client is connected to the server. Now wait for any keystroke.
    while (true) {
        if (-1 == recv(client_fd, &buff, sizeof(union lkby_info), MSG_WAITALL)) continue;
        printf("keystroke code: %d\n", LKBY_INFO_KEYBOARD_CODE(&buff));
        // Call the callback function for the current keystroke.
        //keystroke_callback(&buff);        
    }
}


// Remove this.
void test(const lkby_info *restrict src) 
{

}


int main(void)
{
    establish_connection(&test);
    return 0;
}