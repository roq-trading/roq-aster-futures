/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/tools/crypto.hpp"

#include "roq/logging.hpp"

#include "roq/utils/codec/hex.hpp"

#include "roq/utils/eth/messages.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace tools {

// === CONSTANTS ===

namespace {
auto const EIP712_DOMAIN = utils::eth::Domain{
    .name = "AsterSignTransaction"sv,
    .version = "1"sv,
    .chain_id = 1666,  // note! mainnet = 1666, testnet = 714
    .verifying_contract = "0x0000000000000000000000000000000000000000"sv,
};
}

// === IMPLEMENTATION ===

Crypto::Crypto(std::string_view const &key, std::string_view const &secret) : key_{key}, account_{secret} {
  log::warn("DEBUG wallet address={}"sv, account_.get_address());
}

std::string Crypto::sign_message(std::chrono::microseconds now_utc, std::string_view const &message) {
  std::string message_for_signing;
  if (!std::empty(message)) {
    fmt::format_to(std::back_inserter(message_for_signing), "{}&"sv, message);
  }
  fmt::format_to(std::back_inserter(message_for_signing), "nonce={}&signer={}"sv, now_utc.count(), account_.get_address());
  std::array<utils::eth::Field, 1> data{{
      {
          .name = "msg"sv,
          .type = utils::eth::Type::STRING,
          .value = message_for_signing,
      },
  }};
  auto message_2 = utils::eth::Message{
      .type = "Message"sv,
      .data = data,
  };
  auto signable_message = utils::eth::Messages::encode_typed_data(hash_, EIP712_DOMAIN, message_2);
  auto signed_message = account_.sign_message(hash_, signable_message);
  return fmt::format("?{}&signature={}"sv, message_for_signing, utils::codec::HexPrinter{signed_message.signature});
}

}  // namespace tools
}  // namespace aster_futures
}  // namespace roq
