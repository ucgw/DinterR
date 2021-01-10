#include "sig.h"

void dinterrd_sigint_handler(int signum) {
    sigint_caught = SIGINT_CAUGHT;
}

void dinterrd_reset_sigint_caught(void) {
    sigint_caught = 0;
}

void dinterrd_enable_sigint_handler(void) {
    struct sigaction sigint_act;

    sigint_act.sa_handler = dinterrd_sigint_handler;
    sigint_act.sa_flags = 0;
    sigemptyset(&sigint_act.sa_mask);

    sigaction(SIGINT, &sigint_act, NULL);
}
