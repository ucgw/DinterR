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

// this will be the primary struct
// for input to thread
typedef struct ddtp_thread_data {
    dinterr_sock_t* sockfd;
    ddtp_payload_t* payload;
    ddtp_locks_t* locks;
    dinterr_crc32_data_table_t* data;
    void* _sm;  // state machine object
    int fd;
    int* wd;
    short last_error;
} ddtp_thread_data_t;

// Thread: void* input(s) will be ddtp_thread_data_t*
void* _ddtp_inotify_entry_point(void*);
void* _ddtp_watch_file_inotify(void*);

int __handle_inotify_events(int, int*, dinterr_crc32_data_table_t*, ddtp_locks_t*);

// migrated over from server.h
// (single client support)
void _ddtp_state_verbot(sml::sm<ddtp_server>*, bool);
void _ddtp_payload_md_verbot(short, bool, bool);
void _ddtp_char_verbot(const char*, bool);

bool _ddtp_server_validate_incoming_type(short, sml::sm<ddtp_server>*);
int  _ddtp_load_file_inotify(ddtp_thread_data_t*);
void _ddtp_process_client_payload(short, ddtp_thread_data_t*);
bool _ddtp_server_validate_incoming_type(short, sml::sm<ddtp_server>*);

#endif // _THREAD_H_