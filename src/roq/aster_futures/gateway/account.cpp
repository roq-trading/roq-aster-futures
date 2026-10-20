/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/account.hpp"

#include "roq/logging.hpp"

#include "roq/utils/safe_cast.hpp"

using namespace std::literals;
using namespace std::chrono_literals;

namespace roq {
namespace aster_futures {
namespace gateway {

// === HELPERS ===

namespace {
auto create_crypto(auto &config, auto &name) -> tools::Crypto {
  auto ready = true;
  auto key = config.get_api_key(name);
  if (std::empty(key)) {
    ready = false;
    log::warn(R"(Unexpected: missing key for name="{}")"sv, name);
  }
  auto secret = config.get_secret(name);
  if (std::empty(secret)) {
    ready = false;
    log::warn(R"(Unexpected: missing secret for name="{}")"sv, name);
  }
  if (!ready) {
    log::fatal("Invalid config"sv);
  }
  return {key, secret};
}
}  // namespace

// === IMPLEMENTATION ===

Account::Account(Settings const &, Config const &config, std::string_view const &name) : name{name}, crypto_{create_crypto(config, name)} {
}

std::string Account::create_query(TraceInfo const &trace_info, std::string_view const &message) {
  return crypto_.sign_message(utils::safe_cast(trace_info.origin_create_time_utc), message);
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
