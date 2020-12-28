#ifndef _DINTERR_H_
#define _DINTERR_H_
#include <unistd.h>
#include <sys/time.h>  /* struct timeval */

/* DinterR extra data */
typedef ssize_t    dinterr_count_t;
typedef long long  dinterr_pos_t;
typedef pid_t      dinterr_pid_t;
typedef struct timeval dinterr_ts_t;

#endif  /* _DINTERR_H_ */
