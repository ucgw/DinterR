#ifndef _PAYLOAD_H_
#define _PAYLOAD_H_

#include <cstring>

#include "netproto.h"

void ddtp_payload_init(short, ddtp_payload_t*);
void ddtp_payload_fill_char_data(ddtp_payload_t*, const char*);
void ddtp_payload_fill_ulong_data(ddtp_payload_t*, uLong);
uLong ddtp_payload_extract_ulong_data(ddtp_payload_t*);
void ddtp_payload_fill_short_data(ddtp_payload_t*, short);
short ddtp_payload_extract_short_data(ddtp_payload_t*);

#endif // _PAYLOAD_H_
