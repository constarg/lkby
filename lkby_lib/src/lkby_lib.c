#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "lkby_lib.h"


#define SERVER_SOCK_PATH "/tmp/unix_lkby_sock.server"
#define CLIENT_SOCK_PATH "/tmp/unix_lkby_sock.client"


int establish_connection(void (*keystroke_callback)(const lkby_info *restrict src))
{
    
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