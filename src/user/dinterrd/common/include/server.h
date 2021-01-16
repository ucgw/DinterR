#ifndef _SERVER_H_
#define _SERVER_H_

#include <map>
#include <iostream>

#include "netproto.h"
#include "serdes.h"
#include "sockio.h"

int dinterrd_processor_sm_wrapper(dinterr_sock_t*, char*, uint16_t);
int _dinterrd_processor(dinterr_sock_t*, sml::sm<ddtp_server>*, char*, uint16_t);
bool ddtp_server_validate_incoming_type(short, sml::sm<ddtp_server>*);
void ddtp_server_process_incoming(ddtp_payload_t*, sml::sm<ddtp_server>*, bool);

#endif // _SERVER_H_
