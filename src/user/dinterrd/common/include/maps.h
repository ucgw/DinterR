#ifndef _MAPS_H_
#define _MAPS_H_

#include <zlib.h>

#include <map>
#include <string>

#include "serdes.h"

#define DDTP_LOAD_ERROR1  0x1
#define DDTP_LOAD_ERROR2  0x2
#define DDTP_LOAD_ERROR3  0x3
#define DDTP_POLL_ERROR1  0x4
#define DDTP_POLL_ERROR2  0x5
#define DDTP_EVENT_ERROR1 0x6

#define CRC32_PURGE true
#define CRC32_KEEP false

typedef std::map<int, const char*> dinterr_lookup_table_t;
typedef std::map<uLong, DinterrSerdesNetwork*> dinterr_crc32_data_table_t;
typedef std::map<uLong, bool> dinterr_crc32_purge_table_t;
typedef std::pair<uLong, DinterrSerdesNetwork*> crc_data_pair_t;

static dinterr_lookup_table_t ddtpError {
  { DDTP_LOAD_ERROR1, "inotify_init1() failure" },
  { DDTP_LOAD_ERROR2, "watch descriptor (wd) memory allocation error" },
  { DDTP_LOAD_ERROR3, "inotify_add_watch() failure" },
  { DDTP_POLL_ERROR1, "poll() failure" },
  { DDTP_POLL_ERROR2, "read() failure" },
  { DDTP_EVENT_ERROR1, "inotify event handling error" }
};

#endif // _MAPS_H_
