/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/request.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace gateway {

// === IMPLEMENTATION ===

Request::Request(Shared &shared) : listen_key{*this}, account{*this}, open_orders{*this}, user_trades{*this}, shared_{shared} {
}

bool Request::ready() const {
  return listen_key.ready() && account.ready() && open_orders.ready() && user_trades.ready();
}

bool Request::can_request(TraceInfo const &trace_info) {
  return shared_.rate_limiter.can_request(trace_info);
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
