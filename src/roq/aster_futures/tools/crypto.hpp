/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <array>
#include <chrono>
#include <string>
#include <string_view>

#include "roq/utils/hash/keccak256.hpp"

#include "roq/utils/eth/account.hpp"

namespace roq {
namespace aster_futures {
namespace tools {

struct Crypto final {
  Crypto(std::string_view const &key, std::string_view const &secret);

  Crypto(Crypto &&) = delete;
  Crypto(Crypto const &) = delete;

  auto const &get_key() const { return key_; }

  std::string sign_message(std::chrono::microseconds now_utc, std::string_view const &message);

 private:
  using Hash = utils::hash::Keccak256;
  using Digest = std::array<std::byte, Hash::DIGEST_LENGTH>;

  std::string const key_;
  utils::eth::Account account_;
  Digest digest_;
  Hash hash_;

  std::chrono::microseconds nonce_ = {};
};

}  // namespace tools
}  // namespace aster_futures
}  // namespace roq
