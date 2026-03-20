/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/tools/crypto.hpp"

#include <fmt/format.h>

#include <array>

#include "roq/utils/codec/base64.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace tools {

// === HELPERS ===

namespace {
auto create_headers(auto &key) {
  return fmt::format("X-MBX-APIKEY: {}\r\n"sv, key);
}
}  // namespace

// === IMPLEMENTATION ===

Crypto::Crypto(std::string_view const &key, std::string_view const &secret) : key_{key}, mac_{secret}, headers_{create_headers(key)} {
}

std::string Crypto::create_ws_login(std::chrono::milliseconds timestamp) {
  auto tmp = fmt::format("{}GET/user/verify"sv, timestamp.count());
  mac_.clear();
  mac_.update(tmp);
  auto digest = mac_.final(digest_);
  std::string signature;
  utils::codec::Base64::encode(signature, digest, false, false);
  auto result = fmt::format(
      R"({{)"
      R"("op":"login",)"
      R"("args":[{{)"
      R"("apiKey":"{}",)"
      R"("passphrase":"{}",)"
      R"("timestamp":"{}",)"
      R"("sign":"{}")"
      R"(}})"
      R"(])"
      R"(}})"sv,
      key_,
      headers_,  // XXX
      timestamp.count(),
      signature);
  return result;
}

std::string Crypto::create_query(std::chrono::milliseconds now_utc) {
  auto tmp = fmt::format("timestamp={}"sv, now_utc.count());
  mac_.clear();
  mac_.update(tmp);
  auto digest = mac_.final(digest_);
  std::string signature;
  utils::codec::Base64::encode(signature, digest, false, false);
  return fmt::format("?{}&signature={}"sv, tmp, signature);
}

std::string Crypto::create_query(std::chrono::milliseconds now_utc, std::string_view const &query) {
  auto tmp = fmt::format("{}&timestamp={}"sv, query.substr(1), now_utc.count());
  mac_.clear();
  mac_.update(tmp);
  auto digest = mac_.final(digest_);
  std::string signature;
  utils::codec::Base64::encode(signature, digest, false, false);
  return fmt::format("?{}&signature={}"sv, tmp, signature);
}

}  // namespace tools
}  // namespace aster_futures
}  // namespace roq
