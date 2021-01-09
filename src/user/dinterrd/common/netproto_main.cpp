#include <stdio.h>
#include <iostream>

#include "sockio.h"
#include "server.h"

int main(int argc, char **argv) {
    sml::sm<ddtp_server> ssm;
    dinterr_sock_t dsock;
    dinterr_sock_init(&dsock, DINTERR_SERVER);
    dinterr_sock_create(&dsock, 9000);
}
