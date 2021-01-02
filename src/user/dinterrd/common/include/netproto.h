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
/* events: (hold no data) */
struct load_requested {};
struct load_failed {};
struct load_succeeded {};
struct record_data_ready_to_send {};
struct aggregate_data_ready_to_send {};
struct record_data_received {};
struct record_data_retry_requested {};
struct record_data_finished {};
struct load_finish_requested {};
struct load_finished {};

struct ddt_transitions {
    auto operator()() const noexcept {
    using namespace sml;
    }
};
} // namespace

#endif  // _NETPROTO_H_
