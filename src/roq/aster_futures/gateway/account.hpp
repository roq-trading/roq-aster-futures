/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/aster_futures/gateway/config.hpp"
#include "roq/aster_futures/gateway/settings.hpp"

#include "roq/aster_futures/tools/crypto.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct Account final {
  Account(Settings const &, Config const &, std::string_view const &name);

  Account(Account const &) = delete;

  std::string_view get_key() const { return crypto_.get_key(); }

  std::string create_query(TraceInfo const &, std::string_view const &message = {});

  std::string const name;

 private:
  tools::Crypto crypto_;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
