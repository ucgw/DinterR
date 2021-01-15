#ifndef _SERVER_H_
#define _SERVER_H_

#include <map>
#include <iostream>

#include "netproto.h"
#include "sockio.h"
#include "serdes.h"

int dinterrd_processor(dinterr_sock_t*, char*, uint16_t);
void ddtp_server_process_incoming(ddtp_payload_t*, sml::sm<ddtp_server>* sm, bool);

#endif // _SERVER_H_
