/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <string>
#include <utility>
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

  auto discard_symbol(std::string_view const &name) const { return dispatcher.discard_symbol(name); }

  template <typename... Args>
  auto operator()(Args &&...args) {
    return dispatcher(std::forward<Args>(args)...);
  }

 public:
  server::Dispatcher &dispatcher;

  std::vector<MBPUpdate> bids, asks, final_bids, final_asks;
  std::vector<Trade> trades;
  std::vector<Fill> fills;

 public:
  Settings const &settings;
  API const api;
  core::limit::RateLimiter rate_limiter;
  core::Symbols symbols;
  utils::unordered_set<std::string> all_symbols;

 public:
  // instrument
  Instrument &get_instrument(std::string_view const &symbol);

 private:
  utils::unordered_map<std::string, Instrument> instruments_;

 public:
  core::TimerQueue<std::string> depth_request_queue;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
