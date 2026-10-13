/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/account.hpp"

#include "roq/utils/safe_cast.hpp"

#include "roq/clock.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

// === IMPLEMENTATION ===

Account::Account(Config const &config, std::string_view const &name) : name{name}, crypto_{config.get_api_key(name), config.get_secret(name)} {
}

std::string Account::create_ws_login(TraceInfo const &trace_info) {
  return crypto_.create_ws_login(utils::safe_cast(trace_info.origin_create_time_utc));
}

std::string Account::create_query(TraceInfo const &trace_info) {
  return crypto_.create_query(utils::safe_cast(trace_info.origin_create_time_utc));
}

std::string Account::create_query(TraceInfo const &trace_info, std::string_view const &query) {
  return crypto_.create_query(utils::safe_cast(trace_info.origin_create_time_utc), query);
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
