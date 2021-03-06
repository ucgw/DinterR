#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>

#include <dinterr_inotify.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "netproto.h"
#include "serdes.h"
#include "sockio.h"
#include "thread.h"
#include "maps.h"
#include "lock.h"

int dinterrd_accept(dinterr_sock_t*);  // server
void dinterrd_run_server(dinterr_sock_t*, uint16_t, bool verbose=false, const char* ipaddr=NULL);

int dinterrd_processor_sm_wrapper(dinterr_sock_t*, char*, uint16_t);
int _dinterrd_processor(dinterr_sock_t*, sml::sm<ddtp_server>*, char*, uint16_t);

#endif // _SERVER_H_
