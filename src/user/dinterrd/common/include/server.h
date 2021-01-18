#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>

#include <dinterr_inotify.h>
#include <stdlib.h>
#include <string.h>

#include "netproto.h"
#include "serdes.h"
#include "sockio.h"
#include "thread.h"
#include "maps.h"

int dinterrd_processor_sm_wrapper(dinterr_sock_t*, char*, uint16_t);
int _dinterrd_processor(dinterr_sock_t*, sml::sm<ddtp_server>*, char*, uint16_t);
void _ddtp_state_verbot(sml::sm<ddtp_server>*);
bool ddtp_server_validate_incoming_type(short, sml::sm<ddtp_server>*);
void ddtp_server_process_incoming_payload(ddtp_payload_t*, sml::sm<ddtp_server>*, bool);
int ddtp_server_load_file_inotify(ddtp_thread_data_t*);

#endif // _SERVER_H_
