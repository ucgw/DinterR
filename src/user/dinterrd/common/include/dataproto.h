#ifndef _DATAPROTO_H_
#define _DATAPROTO_H_

#include <zlib.h>

#include "dinterr.h"

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

/* Dinterr data to be serialized for transport
 * over a network
 *
 * => IMPORTANT: Order Matters!!
 *      Serialization occurs based on field order of this struct.
 */
typedef struct dinterr_data {
    uLong           _crc;  // uLong from zlib.h
    unsigned char   _attrs;
    dinterr_pos_t   _pos;
    dinterr_count_t _count;
    dinterr_pid_t   _pid;
    unsigned int    _ra_page_count;
    unsigned int    _ra_cache_misses;
    dinterr_pos_t   _ra_last_cache_pos;
    dinterr_ts_t    _timestamp;
} dinterr_data_t;

class DinterrSerdesData {
    private:
        void *_serdes;
        void _copy_push_along_ptrs(size_t, const char*, dinterr_data_t*);
    public:
        /* 2 different constructors based on input data type
         * determines how class object is used.
         *
         * (i.e. whether object serializes or deserializes)
         * serialize => input: dinterr_data_t*
         * deserialize => input: char*
         */
        DinterrSerdesData(const dinterr_data_t*); // serialize mode
        DinterrSerdesData(const char*);           // deserialize mode
        ~DinterrSerdesData();
        void* get_data(void);
};

#endif // _DATAPROTO_H_
