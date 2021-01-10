#include <stdio.h>
#include <iostream>

#include "sockio.h"
#include "server.h"

int main(int argc, char **argv) {
    dinterr_sock_t dsock;
    uint16_t port = 9000;

    dinterr_sock_init(&dsock, DINTERR_SERVER);
    dinterr_sock_create(&dsock, port);
    dinterrd_accept(&dsock);
}
