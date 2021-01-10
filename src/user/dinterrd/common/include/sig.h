#ifndef _SIG_H_
#define _SIG_H_

#include <signal.h>
#include <unistd.h>

#define SIGINT_CAUGHT 1

static volatile sig_atomic_t sigint_caught = 0;

void dinterrd_sigint_handler(int);
void dinterrd_reset_sigint_caught(void);
void dinterrd_enable_sigint_handler(void);

#endif // _SIG_H_
