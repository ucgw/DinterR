#include <cstdio>

#include "payload.h"

void ddtp_payload_init(short type, ddtp_payload_t* dpl) {
    dpl->type = type;
    memset(&dpl->data, '\0', MAX_DATASIZE);
}

void ddtp_payload_fill_char_data(ddtp_payload_t* dpl, const char* data) {
    size_t dsize = strlen(data) + 1;

    if (dsize <= MAX_DATASIZE)
        strncpy(dpl->data, data, dsize);
}

/*
 * code derived from stackexchange
 *
 * Reference:
 *   https://electronics.stackexchange.com/questions/78589/how-to-convert-to-unsigned-long-from-4-char-array
 */
void ddtp_payload_fill_ulong_data(ddtp_payload_t* dpl, uLong data) {
     dpl->data[0] = data & 0xFF;
     dpl->data[1] = (data >> 8)  & 0xFF;
     dpl->data[2] = (data >> 16) & 0xFF;
     dpl->data[3] = (data >> 24) & 0xFF;
}

uLong ddtp_payload_extract_ulong_data(ddtp_payload_t* dpl) {
    uLong data;
    data = (dpl->data[3] & 0xFF) << 24 | \
           (dpl->data[2] & 0xFF) << 16 | \
           (dpl->data[1] & 0xFF) << 8  | \
           (dpl->data[0] & 0xFF);
    return data;
}

/*
 * short int is 2-bytes in size, so just do what we do
 * for uLongs but for shorts (only filling 2 bytes in char array)
 */
void ddtp_payload_fill_short_data(ddtp_payload_t* dpl, short data) {
    dpl->data[0] = data & 0xFF;
    dpl->data[1] = (data >> 8) & 0xFF;
}

short ddtp_payload_extract_short_data(ddtp_payload_t* dpl) {
    short data;
    data = (dpl->data[1] & 0xFF) << 8  | \
           (dpl->data[0] & 0xFF);
    return data;
}
