#ifndef _SERVER_H_
#define _SERVER_H_

#include <map>

#include "netproto.h"

// map of src port to a state machine
typedef std::map<uint16_t, sml::sm<ddtp_server>> SSM;

#endif // _SERVER_H_
