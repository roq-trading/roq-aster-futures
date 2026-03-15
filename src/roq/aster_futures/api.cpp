/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/api.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {

// === IMPLEMENTATION ===

API API::create(Settings const &) {
  return {
      .category = {},
      .inst_type = {},
      .market_data{
          .exchange_info = "/fapi/v1/exchangeInfo"sv,
          .depth = "/fapi/v1/depth"sv,
      },
      .order_management{
          .account_info = "/fapi/v2/account"sv,
          .account_assets = "/api/v3/account/assets"sv,
          .position_info = "/api/v3/position/current-position"sv,
          .open_orders = "/api/v3/trade/unfilled-orders"sv,
          .fill_history = "/api/v3/trade/fills"sv,
          .place_order = "/api/v3/trade/place-order"sv,
          .modify_order = "/api/v3/trade/modify-order"sv,
          .cancel_order = "/api/v3/trade/cancel-order"sv,
          .cancel_all_orders = "/api/v3/trade/cancel-symbol-order"sv,
          .countdown_cancel_all = "/api/v3/trade/countdown-cancel-all"sv,
      },
  };
}

}  // namespace aster_futures
}  // namespace roq
