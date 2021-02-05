#include "server.h"

int main(int argc, char **argv) {
    dinterr_sock_t dsock;
    uint16_t port = 9000;

    dinterrd_run_server(&dsock, port, true);
}
