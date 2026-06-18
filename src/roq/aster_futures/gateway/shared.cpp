/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/shared.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

// === IMPLEMENTATION ===

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher{dispatcher}, settings{settings}, api{API::create(settings)}, rate_limiter(settings.misc.request_limit, settings.misc.request_limit_interval),
      symbols(settings.ws.max_subscriptions_per_stream), depth_request_queue{settings.ws.mbp_request_delay} {
}

Instrument &Shared::get_instrument(std::string_view const &symbol) {
  auto iter = instruments_.find(symbol);
  if (iter == std::end(instruments_)) [[unlikely]] {
    auto res = instruments_.try_emplace(symbol, settings);
    assert(res.second);
    iter = res.first;
  }
  return (*iter).second;
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
