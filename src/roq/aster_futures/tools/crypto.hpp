/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <array>
#include <chrono>
#include <string>
#include <string_view>

#include "roq/utils/mac/hmac.hpp"

namespace roq {
namespace aster_futures {
namespace tools {

struct Crypto final {
  Crypto(std::string_view const &key, std::string_view const &secret);

  Crypto(Crypto &&) = delete;
  Crypto(Crypto const &) = delete;

  std::string_view get_headers() const { return headers_; }

  std::string create_ws_login(std::chrono::milliseconds now);

  std::string create_query(std::chrono::milliseconds now_utc);
  std::string create_query(std::chrono::milliseconds now_utc, std::string_view const &query);

 private:
  using MAC = utils::mac::HMAC<utils::hash::SHA256>;
  using Digest = std::array<std::byte, MAC::DIGEST_LENGTH>;

  std::string const key_;
  MAC mac_;
  Digest digest_;
  std::string const headers_;
};

}  // namespace tools
}  // namespace aster_futures
}  // namespace roq
