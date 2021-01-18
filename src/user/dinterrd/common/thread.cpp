#include <iostream>
#include "thread.h"

#include "dataproto.cpp"

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

    lock_retval = ddtp_lock(&dlock->ref_count_lock);
    if (lock_retval == 0) {
        ddtp_ref_count += 1;
        lock_retval = ddtp_unlock(&dlock->ref_count_lock);
    }
    return(lock_retval);
}

int ddtp_decrement_ref_count(ddtp_locks_t* dlock) {
    int lock_retval = -1;

    lock_retval = ddtp_lock(&dlock->ref_count_lock);
    if (lock_retval == 0) {
        ddtp_ref_count = ddtp_ref_count - 1;
        lock_retval = ddtp_unlock(&dlock->ref_count_lock);
    }
    return(lock_retval);
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
    
// Thread(s) and dependent functions

/*
 * core functional source code taken from inotify man page
 * for _server_load_file_and_watch() thread function
 *
 * Reference: https://man7.org/linux/man-pages/man7/inotify.7.html
 */
void* _server_inotify_file_watch(void* data) {
    ddtp_thread_data_t* pl_data = (ddtp_thread_data_t*) data;
    int poll_num;
    int poll_err;
    nfds_t nfds = 1;
    struct pollfd fds[1];

    fds[0].fd = pl_data->fd;
    fds[0].events = POLLIN;

    while (true) {
        if (ddtp_get_ref_count(pl_data->locks) <= 0)
            break;
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("_server_inotify_file_watch: poll()");
            poll_err = DDTP_POLL_ERROR1;
            pthread_exit(&poll_err);
        }

        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                if (__handle_inotify_events(pl_data->fd,pl_data->wd, pl_data->data, pl_data->locks) != 0) {
                    poll_err = DDTP_EVENT_ERROR1;
                    pthread_exit(&poll_err);
                }
            }
        }
    }
    pthread_exit(NULL);
}

int __handle_inotify_events(int fd, int *wd, dinterr_crc32_data_table_t* dt, ddtp_locks_t* dlocks) {
    char buf[4096]
    __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;
    int eventerr = 0;

    for (;;) {
        eventerr = 0;
        len = read(fd, buf, sizeof(buf));

        if (len == -1 && errno != EAGAIN) {
            perror("__handle_inotify_events: read()");
            eventerr = DDTP_POLL_ERROR2;
            ddtp_decrement_ref_count(dlocks);
            break;
        }

        if (len <= 0)
            break;

        for (char *ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {

            unsigned char attrs = 0;
            dinterr_ts_t timestamp;
            event = (const struct inotify_event *) ptr;

            if (event->ra_page_count != 0 || event->ra_misses != 0)
                set_attr(&attrs, DINTERR_ATTR_READAHEAD);
            if (event->pos != event->last_cached_pos)
                set_attr(&attrs, DINTERR_ATTR_MULTISEEK);

            timestamp.tv_sec = event->fse_atime.tv_sec;
            timestamp.tv_usec = event->fse_atime.tv_usec;

            auto drecord = dinterrRecord()
                           .add_attrs(attrs)
                           .add_pos((dinterr_pos_t)event->pos)
                           .add_count((dinterr_count_t)event->count)
                           .add_pid((dinterr_pid_t)event->pid)
                           .add_ra_page_count((unsigned int)event->ra_page_count)
                           .add_ra_cache_misses((unsigned int)event->ra_misses)
                           .add_ra_last_cache_pos((dinterr_pos_t)event->last_cached_pos)
                           .add_timestamp(&timestamp)
                           .calc_crc();

            DinterrSerdesData* serdes = drecord.get_serdes();
            uLong crc32_key = drecord.get_crc();

            ddtp_lock(&dlocks->data_access_lock);

            std::cerr << "DATA CRC32: " << crc32_key << std::endl;

            dt->insert(
              crc_data_pair_t(
                crc32_key,
                ddtp_serdes_create((const char*)serdes->get_data())
              )
            );

            ddtp_signal_data_ready(dlocks);
            ddtp_unlock(&dlocks->data_access_lock);

            delete serdes;
        }
    }
    return(eventerr);
}
