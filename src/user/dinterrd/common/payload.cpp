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
 * code taken from stackoverflow
 *
 * Reference:
 *   https://stackoverflow.com/questions/30840575/how-convert-unsigned-int-to-unsigned-char-array/30840656
 */
void ddtp_payload_fill_ulong_data(ddtp_payload_t* dpl, uLong data) {
    dpl->data[0] =  (uLong) data & 0xFF;
    dpl->data[1] = ((uLong) data>>8)  & 0xFF;
    dpl->data[2] = ((uLong) data>>16) & 0xFF;
    dpl->data[3] = ((uLong) data>>24) & 0xFF;
}

uLong ddtp_payload_extract_ulong_data(ddtp_payload_t* dpl) {
    printf("\n%02X\n%02X\n%02X\n%02X\n", dpl->data[0], dpl->data[1], dpl->data[2], dpl->data[3]);
    uLong dval =  (uLong) dpl->data[0] | \
                 ((uLong) dpl->data[1]<<8)  | \
                 ((uLong) dpl->data[2]<<16) | \
                 ((uLong) dpl->data[3]<<24);
    return(dval);
}
