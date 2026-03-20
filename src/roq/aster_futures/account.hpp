/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/web/http/method.hpp"

#include "roq/aster_futures/config.hpp"

#include "roq/aster_futures/tools/crypto.hpp"

namespace roq {
namespace aster_futures {

class Account final {
 public:
  Account(Config const &, std::string_view const &name);

  Account(Account const &) = delete;

  std::string_view get_headers() const { return crypto_.get_headers(); }

  std::string create_ws_login();

  std::string create_query();
  std::string create_query(std::string_view const &query);

  std::string const name;

 private:
  tools::Crypto crypto_;
};

}  // namespace aster_futures
}  // namespace roq
