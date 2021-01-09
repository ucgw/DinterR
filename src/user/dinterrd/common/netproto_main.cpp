#include <stdio.h>
#include <iostream>

#include "netproto.cpp"
#include "sockio.h"

int main(int argc, char **argv) {
    dinterr_sock_t dsock;
    dinterr_sock_init(&dsock, DINTERR_SERVER);
    dinterr_sock_create(&dsock, 9000);
}
