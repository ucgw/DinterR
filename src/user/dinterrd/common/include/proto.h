#ifndef _PROTO_H_
#define _PROTO_H_

#include <zlib.h>

/* DinterR single record supported attributes
 * copied ATTR_* macro pattern from include/linux/fs.h
 * for DINTERR_ATTRs here.
 * for now, we only support 4 bits for record attribute
 * field
 */
#define DINTERR_ATTR_READAHEAD (1 << 0) // 2^0
#define DINTERR_ATTR_MULTISEEK (1 << 1) // 2^1
#define DINTERR_ATTR_FUTURE1   (1 << 2) // 2^2
#define DINTERR_ATTR_FUTURE2   (1 << 3) // 2^3
#define DINTERR_ATTR_FUTURE3   (1 << 4) // 2^4

/* inline functions to perform record attribute alterations
 * effectively the same as using a #define macro
 */
static inline void set_attr(unsigned char *attrs, int attr) {
    if (attr == DINTERR_ATTR_READAHEAD || \
        attr == DINTERR_ATTR_MULTISEEK)
        *attrs |= attr;
}

static inline void clear_attr(unsigned char *attrs, int attr) {
    if (attr == DINTERR_ATTR_READAHEAD || \
        attr == DINTERR_ATTR_MULTISEEK)
        *attrs &= ~attr;
}

static inline bool is_attr_set(unsigned char *attrs, int attr) {
    if (attr == DINTERR_ATTR_READAHEAD || \
        attr == DINTERR_ATTR_MULTISEEK)
        return (*attrs & attr);
    return false;
}

enum class DinterrTransportState {
  INIT,
  LOAD,
  LOAD_ACK,
  LOAD_FAIL,
  CTS,
  AGG,
  REC,
  DATA_ACK,
  DATA_RESEND,
  DATA_STOP,
  DATA_RESUME,
  DATA_FIN,
  CLOSE
};

/* enum class describing the kinds of data expected
 * and enforced in TCP payloads of DinterR records
 * via "typestates"
 */
enum class DinterrRecordState {
  INIT,
  ATTRS,
  CHECKSUM,
  POS,
  COUNT,
  PID,
  TIMESTAMP,
  RA_PAGE_COUNT,
  RA_CACHE_MISSES,
  RA_LAST_CACHED_POS,
  SERIALIZE
};

#endif // _PROTO_H_
