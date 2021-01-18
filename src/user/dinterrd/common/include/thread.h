#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <poll.h>

#include "dinterr_inotify.h"
#include "sockio.h"
#include "netproto.h"
#include "maps.h"

#define DDTP_DATA_NOTREADY 0
#define DDTP_DATA_READY 1

#define DDTP_MAX_REFERENCES 1

// this will be the reference count
// for number of client sessions
// connected to server successfully
// 1:1 ratio of client:inotify watcher thread
//
// initially only supporting a single
// client, but could be useful in the
// future for extending multi-client support.
static short ddtp_ref_count;

// this will be the toggle for determining
// when data is ready to send to client.
static short ddtp_data_ready;

// locking primitives w/ struct for locks
// and values to lock on for update
typedef struct ddtp_locks {
    pthread_mutex_t data_ready_lock;
    pthread_mutex_t ref_count_lock;
    pthread_mutex_t data_access_lock;
    pthread_cond_t data_ready_cond;
} ddtp_locks_t;

void ddtp_locks_init(ddtp_locks_t*);
int ddtp_lock(pthread_mutex_t*);
int ddtp_unlock(pthread_mutex_t*);

// locking derivatives for synchronizing
// when data is available to send to client
// between main thread and inotify handler thread.
//
// main thread: ddtp_block_until_data_ready()
// inotify handler thread: ddtp_signal_data_ready()
int ddtp_block_until_data_ready(ddtp_locks_t*);
int ddtp_signal_data_ready(ddtp_locks_t*);
int ddtp_increment_ref_count(ddtp_locks_t*);
int ddtp_decrement_ref_count(ddtp_locks_t*);
short ddtp_get_ref_count(ddtp_locks_t*);

// this will be the primary struct
// for input to thread
typedef struct ddtp_thread_data {
    dinterr_sock_t* sockfd;
    ddtp_payload_t* payload;
    ddtp_locks_t* locks;
    dinterr_crc32_data_table_t* data;
    int fd;
    int* wd;
} ddtp_thread_data_t;

// Thread: input will be ddtp_thread_data_t*
void* _server_inotify_file_watch(void*);
int __handle_inotify_events(int, int*, dinterr_crc32_data_table_t*, ddtp_locks_t*);

#endif // _THREAD_H_
