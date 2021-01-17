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

#define DDTP_DATA_READY_INIT -1
#define DDTP_DATA_READY_SUCCESS 0
#define DDTP_DATA_READY_FAILURE 1

// this will be the reference count
// for number of client sessions
// connected to server.
// initially only supporting a single
// client, but could be useful in the
// future.
static short ddtp_ref_count;

// locking primitives w/ struct for locks
// and values to lock on for update
typedef struct ddtp_locks {
    pthread_mutex_t data_ready_lock;
    pthread_mutex_t ref_count_lock;
    short ddtp_data_ready;
} ddtp_locks_t;


// this will be the primary struct
// for input to thread
typedef struct ddtp_thread_data {
    dinterr_sock_t* sockfd;
    ddtp_payload_t* payload;
    ddtp_locks_t locks;
    dinterr_crc32_data_table_t* data;
    int fd;
    int* wd;
} ddtp_thread_data_t;

void ddtp_locks_init(ddtp_locks_t*);
int ddtp_lock(pthread_mutex_t*);
int ddtp_unlock(pthread_mutex_t*);

// Thread: input will be ddtp_thread_data_t*
void* _server_load_file_and_watch(void*);
int __handle_inotify_events(int, int*, dinterr_crc32_data_table_t*, pthread_mutex_t*);

#endif // _THREAD_H_
