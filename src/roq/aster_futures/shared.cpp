/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/aster_futures/shared.hpp"

namespace roq {
namespace aster_futures {

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher_(dispatcher), settings{settings}, api{API::create(settings)}, rate_limiter(settings.misc.request_limit, settings.misc.request_limit_interval),
      symbols(settings.ws.max_subscriptions_per_stream) {
}

}  // namespace aster_futures
}  // namespace roq
