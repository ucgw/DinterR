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
#include "lock.h"
#include "queue.h"
#include "payload.h"

/*
 * The global queue (gq) where the inotify event
 * handling thread enqueues serialized dinterr data
 * with its crc32 value for the main thread to process
 */
static ThreadsafeQueue<crc_data_pair_t> gq;

// this will be the primary struct
// for input to thread
typedef struct ddtp_thread_data {
    dinterr_sock_t* sockfd;
    ddtp_payload_t* payload;
    ddtp_locks_t* locks;
    void* _sm;  // state machine object
    int fd;
    int* wd;
    short last_error;
} ddtp_thread_data_t;

// Thread: void* input(s) will be ddtp_thread_data_t*
void* _ddtp_inotify_entry_point(void*);
void* _ddtp_watch_file_inotify(void*);
void* _ddtp_async_client_payload_handler(void*);

int __handle_inotify_events(int, int*, ddtp_locks_t*);

// migrated over from server.h
// (single client support)
void _ddtp_state_verbot(sml::sm<ddtp_server>*, bool);
void _ddtp_payload_md_verbot(short, bool, bool);
void _ddtp_char_verbot(const char*, bool);
void _ddtp_data_client_target_verbot(crc_data_pair_t*, bool);

int _ddtp_send_data_client_target(crc_data_pair_t*, dinterr_sock_t*);
int _ddtp_server_send_payload(ddtp_payload_t*, dinterr_sock_t*);

bool _ddtp_server_validate_incoming_type(short, ddtp_thread_data_t*);
int  _ddtp_load_file_inotify(ddtp_thread_data_t*);
void _ddtp_process_client_payload(short, ddtp_thread_data_t*);

#endif // _THREAD_H_
