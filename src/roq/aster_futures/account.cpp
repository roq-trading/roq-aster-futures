/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/account.hpp"

#include "roq/utils/safe_cast.hpp"

#include "roq/clock.hpp"

namespace roq {
namespace aster_futures {

// === IMPLEMENTATION ===

Account::Account(Config const &config, std::string_view const &name) : name(name), crypto_(config.get_api_key(name), config.get_secret(name)) {
}

std::string Account::create_ws_login() {
  auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
  return crypto_.create_ws_login(now_utc);
}

std::string Account::create_query() {
  auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
  return crypto_.create_query(now_utc);
}

std::string Account::create_query(std::string_view const &query) {
  auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
  return crypto_.create_query(now_utc, query);
}

}  // namespace aster_futures
}  // namespace roq
