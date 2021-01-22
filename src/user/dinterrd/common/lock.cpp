#include "lock.h"

void ddtp_locks_init(ddtp_locks_t* dlock) {
    dlock->data_ready_lock = PTHREAD_MUTEX_INITIALIZER;
    dlock->ref_count_lock = PTHREAD_MUTEX_INITIALIZER;
    dlock->data_access_lock = PTHREAD_MUTEX_INITIALIZER;

    dlock->data_ready_cond = PTHREAD_COND_INITIALIZER;

    ddtp_ref_count = 0;
    ddtp_data_ready = DDTP_DATA_NOTREADY;
}

int ddtp_lock(pthread_mutex_t* ltype) {
    if (ltype != NULL) {
        if (pthread_mutex_lock(ltype) == 0)
            return(0);
        perror("ddtp_lock: pthread_mutex_lock()");
        return(errno);
    }
    return(-1);
}

int ddtp_unlock(pthread_mutex_t* ltype) {
    if (ltype != NULL) {
        if (pthread_mutex_unlock(ltype) == 0)
            return(0);
        perror("ddtp_unlock: pthread_mutex_unlock()");
        return(errno);
    }
    return(-1);
}

int ddtp_block_until_data_ready(ddtp_locks_t* dlock) {
    int lock_retval = -1;

    lock_retval = ddtp_lock(&dlock->data_ready_lock);
    if (lock_retval == 0) {
        while (ddtp_data_ready == DDTP_DATA_NOTREADY)
            pthread_cond_wait(&dlock->data_ready_cond, &dlock->data_ready_lock);
        ddtp_data_ready = DDTP_DATA_NOTREADY;
        lock_retval = ddtp_unlock(&dlock->data_ready_lock);
    }
    return(lock_retval);
}

int ddtp_signal_data_ready(ddtp_locks_t* dlock) {
    int lock_retval = -1;

    lock_retval = ddtp_lock(&dlock->data_ready_lock);
    if (lock_retval == 0) {
        ddtp_data_ready = DDTP_DATA_READY;
        lock_retval = ddtp_unlock(&dlock->data_ready_lock);
        pthread_cond_signal(&dlock->data_ready_cond);
    }
    return(lock_retval);
}

int ddtp_increment_ref_count(ddtp_locks_t* dlock) {
    int lock_retval = -1;
    int check;

    lock_retval = ddtp_lock(&dlock->ref_count_lock);
    if (lock_retval == 0) {
        check = ddtp_ref_count;
        if (ddtp_ref_count < DDTP_MAX_REFERENCES)
            ddtp_ref_count += 1;
        if (check == ddtp_ref_count)
            check = -1;
        lock_retval = ddtp_unlock(&dlock->ref_count_lock);
    }
    if (check != -1)
        return(lock_retval);
    return(-1);
}

int ddtp_decrement_ref_count(ddtp_locks_t* dlock) {
    int lock_retval = -1;
    int check;

    lock_retval = ddtp_lock(&dlock->ref_count_lock);
    if (lock_retval == 0) {
        check = ddtp_ref_count;
        if (ddtp_ref_count > 0)
            ddtp_ref_count = ddtp_ref_count - 1;
        if (check == ddtp_ref_count)
            check = -1;
        lock_retval = ddtp_unlock(&dlock->ref_count_lock);
    }
    if (check != -1)
        return(lock_retval);
    return(-1);
}

short ddtp_get_ref_count(ddtp_locks_t* dlock) {
    int lock_retval = -1;
    short rcount = -1;

    lock_retval = ddtp_lock(&dlock->ref_count_lock);
    if (lock_retval == 0) {
        rcount = ddtp_ref_count;
        ddtp_unlock(&dlock->ref_count_lock);
    }
    return(rcount);
}
