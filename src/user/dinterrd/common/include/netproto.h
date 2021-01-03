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
struct load_succeed {};
struct data_send {};
struct data_receive {};
struct data_retry_request {};
struct data_verify_confirm {};
struct data_process {};
struct data_purge {};
struct unload_request {};
struct unload_complete {};

/* state machine for the DDTP server */
struct ddt_server {
    auto operator()() const {
    using namespace sml;
    }
};

/* state machine for the DDTP client */
struct ddt_client {
    auto operator()() const {
    using namespace sml;
    }
};
} // namespace

#endif  // _NETPROTO_H_
