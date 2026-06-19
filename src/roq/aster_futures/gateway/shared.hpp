/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <vector>

#include "roq/api.hpp"

#include "roq/server.hpp"

#include "roq/utils/container.hpp"

#include "roq/core/symbols.hpp"
#include "roq/core/timer_queue.hpp"

#include "roq/core/limit/rate_limiter.hpp"

#include "roq/aster_futures/gateway/api.hpp"
#include "roq/aster_futures/gateway/instrument.hpp"
#include "roq/aster_futures/gateway/settings.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct Shared final {
  Shared(server::Dispatcher &, Settings const &);

  Shared(Shared const &) = delete;

  Instrument &get_instrument(std::string_view const &symbol);

  server::Dispatcher &dispatcher;

  Settings const &settings;
  API const api;

  core::limit::RateLimiter rate_limiter;

  core::Symbols symbols;
  utils::unordered_set<std::string> all_symbols;

  core::TimerQueue<std::string> depth_request_queue;

  std::vector<MBPUpdate> bids, asks, final_bids, final_asks;
  std::vector<Trade> trades;
  std::vector<Fill> fills;

 private:
  utils::unordered_map<std::string, Instrument> instruments_;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
