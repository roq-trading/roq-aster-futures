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

#include "roq/aster_futures/api.hpp"
#include "roq/aster_futures/instrument.hpp"
#include "roq/aster_futures/settings.hpp"

namespace roq {
namespace aster_futures {

struct Shared final {
  Shared(server::Dispatcher &, Settings const &);

  Shared(Shared const &) = delete;

  auto discard_symbol(std::string_view const &name) const { return dispatcher_.discard_symbol(name); }

  template <typename... Args>
  auto update_order(Args &&...args) {
    return dispatcher_.update_order(std::forward<Args>(args)...);
  }

  template <typename... Args>
  auto operator()(Args &&...args) {
    return dispatcher_(std::forward<Args>(args)...);
  }

 public:
  std::vector<MBPUpdate> bids, asks, final_bids, final_asks;
  std::vector<Trade> trades;
  std::vector<Fill> fills;

 private:
  server::Dispatcher &dispatcher_;

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

}  // namespace aster_futures
}  // namespace roq
