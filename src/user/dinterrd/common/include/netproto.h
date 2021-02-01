/* Dinterr Data Transport (DDT) Protocol implementation:
 * pattern and templating functionality via
 * Boost.SML
 *
 * Reference:
 * https://boost-ext.github.io/sml/index.html
 *
 * (requires minimum of -std=c++14 (i.e. c++ 14 standard))
 */

#ifndef _NETPROTO_H_
#define _NETPROTO_H_

#include <cassert>

#include <zlib.h>

#include "sml.hpp"

namespace sml = boost::sml;

namespace {
/* events */
struct load_request {};
struct load_fail {};
struct load_success {};
struct unload_request {};
struct unload_complete {};
struct terminate {};

/* state machine for the DDTP server */
struct ddtp_server {
    auto operator()() const {
        using namespace sml;
        return make_transition_table(
          *"load_wait"_s   + event<load_fail> = X,
           "load_wait"_s   + event<load_success> = "data_ready"_s,
           "data_ready"_s  + event<unload_request> = "unload_pend"_s,
           "unload_pend"_s  + event<terminate> = X
        );
    }
};

/* state machine for the DDTP client */
struct ddtp_client {
    auto operator()() const {
        using namespace sml;
        return make_transition_table(
          *"load_init"_s    + event<load_request> = "load_pend"_s,
           "load_pend"_s    + event<load_fail> = X,
           "load_pend"_s    + event<load_success> = "data_wait"_s,
           "data_wait"_s    + event<unload_request> = "unload_pend"_s,
           "unload_pend"_s  + event<unload_complete> = X
        );
    }
};
} // namespace

/*
 * DDT Protocol Types
 * (akin to certain SML events)
 */
#define LOAD_REQUEST    0x1
#define LOAD_FAIL       0x2
#define LOAD_SUCCEED    0x4
#define UNLOAD_REQUEST  0x8
#define DATA_COMPLETE   0x8

#define MAX_DATASIZE 128

typedef struct ddtp_payload {
    short type;
    char data[MAX_DATASIZE];
} ddtp_payload_t;

#define MAX_PAYLOAD_SIZE sizeof(ddtp_payload_t)

#endif  // _NETPROTO_H_
