#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <iostream>

#include <dinterr_inotify.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#include "netproto.h"
#include "serdes.h"
#include "sockio.h"
#include "maps.h"
#include "lock.h"
#include "payload.h"

int dinterrd_run_client(dinterr_sock_t*, uint16_t, const char*, const char*, bool verbose=false);
int dinterrd_connect(dinterr_sock_t*, sml::sm<ddtp_client>*, const char*);
int ddtp_client_send_payload(dinterr_sock_t*, ddtp_payload_t*);
void ddtp_client_serdes_response(const char*);

#endif // _CLIENT_H_
