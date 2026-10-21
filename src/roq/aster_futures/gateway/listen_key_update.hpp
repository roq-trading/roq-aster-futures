/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string_view>

namespace roq {
namespace aster_futures {
namespace gateway {

struct ListenKeyUpdate final {
  std::string_view account;
  std::string_view listen_key;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
