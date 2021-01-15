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

#include "sml.hpp"

namespace sml = boost::sml;

namespace {
/* events */
struct load_request {};
struct load_fail {};
struct load_success {};
struct data_send {};
struct data_receive {};  // client internal
struct data_retry_request {};
struct data_verify_confirm {};
struct data_process {};  // client internal
struct data_purge {};    // server internal
struct unload_request {};
struct unload_complete {};
struct terminate {};

/* state machine for the DDTP server */
struct ddtp_server {
    auto operator()() const {
        using namespace sml;
        return make_transition_table(
          *"load_wait"_s   + event<load_fail> = "load_wait"_s,
           "load_wait"_s   + event<load_success> = "data_ready"_s,
           "data_ready"_s  + event<data_send> = "data_pend"_s,
           "data_ready"_s  + event<unload_request> = "unload_pend"_s,
           "data_pend"_s   + event<data_verify_confirm> = "data_purge"_s,
           "data_pend"_s   + event<data_retry_request> = "data_ready"_s,
           "data_purge"_s  + event<data_purge> = "data_ready"_s,
           "unload_pend"_s + event<unload_complete> = "load_wait"_s,
           "load_wait"_s   + event<terminate> = X,
           "data_ready"_s  + event<terminate> = X
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
           "data_wait"_s    + event<data_receive> = "data_verify"_s,
           "data_wait"_s    + event<unload_request> = "unload_pend"_s,
           "data_verify"_s  + event<data_retry_request> = "data_wait"_s,
           "data_verify"_s  + event<data_process> = "data_process"_s,
           "data_process"_s + event<data_verify_confirm> = "data_wait"_s,
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
#define DATA_SEND       0x8
#define DATA_RETRY      0x10
#define DATA_CONFIRM    0x20
#define UNLOAD_REQUEST  0x40
#define UNLOAD_COMPLETE 0x80

static inline bool validate_ddtp_type(short type) {
    if (type == LOAD_REQUEST || \
        type == LOAD_FAIL || \
        type == LOAD_SUCCEED || \
        type == DATA_SEND || \
        type == DATA_RETRY || \
        type == DATA_CONFIRM || \
        type == UNLOAD_REQUEST || \
        type == UNLOAD_COMPLETE)
        return true;
    return false;
}

typedef struct ddtp_payload {
    short type;
    void* payload;
} ddtp_payload_t;

#endif  // _NETPROTO_H_
