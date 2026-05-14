/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <cstdint>

namespace roq {
namespace aster_futures {

enum class OrderEntryState : uint8_t {
  UNDEFINED = 0,
  ACCOUNT_INFO,
  ACCOUNT_ASSETS,
  POSITION_INFO,
  OPEN_ORDERS,
  FILL_HISTORY,
  DONE,
};

}  // namespace aster_futures
}  // namespace roq
